//******************************************************************************
//
// The MIT License (MIT)
//
// Copyright (c) 2020 Sean Isom
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//******************************************************************************

//
// M2 LCLookup + land-class test .bgl generator (LANDCLASS_SYNTHESIS.md §7 M2).
//
// Produces the M2 experiment inputs as TWO files, matching how FSX's terrain
// engine actually consumes these layers:
//
//   1. <out-landclass> (always) — a TerrainLandClass (0x68) raster painted at
//      SAMPLE RESOLUTION by a --scenario (uniform | corners2 | corners3 |
//      corners4 | blocks | dualset) or an explicit --grid file. A blend "corner" is a
//      2x2 neighborhood of ~1 km land-class samples (samples sit on cell
//      corners), so the corner* scenarios lay out the corner configurations
//      that drive the dithered 1-bit-mask blend of GDC2006 §7.1 / Fig 10. The
//      tool prints a layout map (config -> sample coords -> approximate
//      lat/lon) so in-sim observations can be correlated. The raster (0x68) IS
//      overridable from a scenery-layer .bgl, so this drops into an active
//      scenery area's scenery/ folder at high priority.
//
//   2. <out-lookup> (only with --lclookup-in) — a PATCHED copy of the real
//      global lclookup. FSX's terrain engine only ever consults its single
//      GLOBAL terrain-texture lookup; a per-scenery-area 0x6F is legal by BGL
//      structure but is silently IGNORED by the engine (this differs from MS
//      Flight (2012), where the lookup is per-layer with a fallback chain). So
//      we cannot author a standalone world-valid lookup — we must take the real
//      global file as input, decode it via the M1 reader, NON-DESTRUCTIVELY
//      append two texture rows (class A / class B) carrying the swept GAP B
//      knobs, repoint classes A/B in every region table to those rows, and
//      re-emit the complete file (CBglFile::Read -> mutate -> Rename -> Write,
//      which never clobbers the input). The maintainer backs up the original
//      and swaps in the patched file as the global lookup.
//
// This is M2 *prep*: it produces the experiment input. The actual fly / observe
// / write-down-the-rule step happens on local FSX (see the M2 protocol in
// LANDCLASS_SYNTHESIS.md).
//
// All integer I/O is little-endian, matching flightsimlib's BinaryStream.
// Returns 0 on success, non-zero on failure.
//

#include "BglData.h"
#include "BglFile.h"
#include "BglTypes.h"
#include "BinaryStream.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using flightsimlib::io::BinaryFileStream;
using flightsimlib::io::CBglFile;
using flightsimlib::io::CBglTerrainTextureLookup;
using flightsimlib::io::CPackedQmid;
using flightsimlib::io::EBglLayerClass;
using flightsimlib::io::EBglLayerType;
using flightsimlib::io::ERasterCompressionType;
using flightsimlib::io::ERasterDataType;
using flightsimlib::io::IBglTerrainTextureLookup;
using flightsimlib::io::SBglHeader;
using flightsimlib::io::SBglLayerPointer;
using flightsimlib::io::SBglTextureSet;

namespace
{

constexpr int kHeaderSize = 0x38;
constexpr int kLayerPointerSize = 20;
constexpr int kTilePointerSize = 16; // packed direct-QMID tile pointer (no RecordCount). Confirmed
                                     // against `resample` output: both 0x6F LCLookup AND 0x68 terrain
                                     // rasters use the 16-byte form (an earlier 20-byte guess was wrong).
constexpr int kTrq1HeaderSize = 40;
constexpr uint32_t kTrq1Magic = 0x31515254; // 'TRQ1'

// Sample-resolution land-class layouts. A blend "corner" is a 2x2 neighborhood
// of ~1 km land-class samples; the corner* scenarios lay out corner configs as
// spaced 2x2-sample blocks so each blend config can be read off in-sim.
enum class Scenario
{
    Uniform,  // one class over the whole tile (GAP 4: tilepattern geo-indexing)
    Corners2, // all 14 two-class corner configs (GAP 2/3: draw-priority arbitration)
    Corners3, // representative 3-class corners (GAP 3: top-2 priorities blended)
    Corners4, // representative 4-class corners (GAP 3: top-3 priorities blended)
    Blocks,   // class-B shapes on a class-A field (GAP 2: M-tile sub-panel topology)
    DualSet,  // two large fields of two classes side by side (GAP 5: per-set independence)
    Grid,     // explicit class-id grid from --grid <file>
};

struct Config
{
    std::filesystem::path out_lookup = "m2_lclookup.bgl";
    std::filesystem::path out_landclass = "m2_landclass.bgl";

    // The real global lclookup.bgl to patch. Empty => skip the lookup file
    // (only the land-class raster is written), since FSX ignores a from-scratch
    // per-area 0x6F and a standalone world-valid lookup cannot be synthesized.
    std::filesystem::path lclookup_in;

    // Diagnostic: read --lclookup-in and write it back UNCHANGED to --out-lookup,
    // then assert byte-for-byte identity. Proves the flightsimlib writer is
    // faithful (no padding drift). Does nothing else.
    bool roundtrip_check = false;

    // Target QMID. Either supplied directly, or derived from lat/lon/level.
    bool qmid_explicit = false;
    uint32_t qmid_low = 0;
    uint32_t qmid_high = 0;
    double lat = 47.6; // default: near Seattle, WA
    double lon = -122.3;
    int level = 11;

    // Land-class raster.
    int raster = 64; // built-in scenarios paint a raster x raster sample grid
    int class_a = 1;
    int class_b = 2;
    int class_c = 3; // used by corners3 / corners4
    int class_d = 4; // used by corners4
    Scenario scenario = Scenario::Corners2;
    std::filesystem::path grid_file; // --grid: explicit class-id grid (Scenario::Grid)

    // --emit-resample-source: instead of the deprecated hand-rolled 0x68 writer,
    // emit a 24-bit BMP (pixel = RGB(v,v,v) => land class v) + an INF over the
    // EXACT target cell so the SDK `resample` builds an FSX-valid, fully-covered
    // multi-class land-class tile (SizeMask=0). See LANDCLASS_SYNTHESIS.md §7.3.
    bool emit_resample_source = false;
    std::filesystem::path rs_out = "m2_resample"; // output root: <rs_out>/SourceData/<base>.bmp + <rs_out>/<base>.inf
    std::string rs_base = "m2lc";                 // DestBaseFileName + source BMP stem
    int rs_margin = 4;                            // overfill: pad the source by this many cells on each side
    // Exact destination cell bounds (default = the Jenny Lake anchor, QMID 0x8304).
    double dest_north = 45.0;
    double dest_south = 42.1875;
    double dest_west = -112.5;
    double dest_east = -108.75;

    // --inspect: read-only decode of --lclookup-in. Prints, for --region and a
    // set of class ids, each class's resolved texture-row index + that row's raw
    // fields (so the active ground-set / blend-mask refs can be picked). Writes
    // nothing and exits.
    bool inspect = false;
    std::vector<int> inspect_classes; // --classes for --inspect (default A,B)

    // GAP B sweep knobs (applied to the appended texture-set rows). Names track
    // the corrected SBglTextureSet fields (LANDCLASS_SYNTHESIS.md §6): the ground
    // set number = the painted class id (--class-a/-b); TextureRegion = --region.
    int texture_variation = 0;           // TextureVariation: ground TilePattern{n} scheme
    int mask_vulcn = 0;                  // MaskVULCN: M-tile mask set (e.g. 900)
    int mask_region = 0;                 // MaskRegion: region qualifier for the mask name
    int mask_variation = 0;              // MaskVariation: mask TilePattern{n} scheme
    int64_t mask_texture_variations = 0; // MaskTextureVariations: packed 16x4-bit variant override
    int season_mask = 0x0FFF;            // all 12 months by default
    int draw_priority = 0;               // DrawPriority of the FIRST patched class...
    int draw_priority_step = 10;         // ...each subsequent class gets +step (so later class wins arbitration)

    int region = 0; // region id painted/used for this test

    bool verify = true; // read the file back and assert (self-test)
};

// Inverse of terrain::UnpackQmid (raster_tile.h): build the FSX packed QMID
// from (tile_x, tile_y, level). The level marker is a single 1-bit at bit
// 2*level+1; each level i contributes the pair (v_i<<1)|u_i at bit 2*(i-1).
uint64_t PackQmid(unsigned int tile_x, unsigned int tile_y, int level)
{
    uint64_t packed = static_cast<uint64_t>(2) << (2 * level); // marker bit
    for (int i = 1; i <= level; ++i)
    {
        const uint64_t u = (tile_x >> (i - 1)) & 1u;
        const uint64_t v = (tile_y >> (i - 1)) & 1u;
        packed |= ((v << 1) | u) << ((i - 1) * 2);
    }
    return packed;
}

// Equirectangular lat/lon -> (tile_x, tile_y) at a level. Documented estimate
// only (FSX's exact geodetic grid is an install detail); the maintainer flies
// to the printed bounds to confirm.
void LatLonToTile(double lat, double lon, int level, unsigned int& tile_x, unsigned int& tile_y)
{
    const double nx = (lon + 180.0) / 360.0;
    const double ny = (90.0 - lat) / 180.0;
    const unsigned int span = 1u << level;
    auto clampf = [](double v, double lo, double hi) { return v < lo ? lo : (v > hi ? hi : v); };
    tile_x = static_cast<unsigned int>(clampf(nx, 0.0, 0.999999) * span);
    tile_y = static_cast<unsigned int>(clampf(ny, 0.0, 0.999999) * span);
}

void TileBounds(
    unsigned int tile_x, unsigned int tile_y, int level, double& lat_n, double& lon_w, double& lat_s, double& lon_e)
{
    const double span = static_cast<double>(1u << level);
    lon_w = -180.0 + (tile_x / span) * 360.0;
    lon_e = -180.0 + ((tile_x + 1) / span) * 360.0;
    lat_n = 90.0 - (tile_y / span) * 180.0;
    lat_s = 90.0 - ((tile_y + 1) / span) * 180.0;
}

// A painted land-class raster: row-major class ids, cols x rows. Row 0 is the
// north edge of the tile (matching the equirect bounds print).
struct Raster
{
    int cols = 0;
    int rows = 0;
    std::vector<uint8_t> cells;

    void Resize(int c, int r, uint8_t fill)
    {
        cols = c;
        rows = r;
        cells.assign(static_cast<size_t>(c) * r, fill);
    }
    void Set(int col, int row, uint8_t v)
    {
        if (col >= 0 && row >= 0 && col < cols && row < rows)
        {
            cells[static_cast<size_t>(row) * cols + col] = v;
        }
    }
    uint8_t Get(int col, int row) const { return cells[static_cast<size_t>(row) * cols + col]; }
};

// One labeled footprint in the painted raster (a 2x2 corner block, or a larger
// field) for the printed layout map.
struct LayoutEntry
{
    std::string label;
    int col = 0; // top-left sample
    int row = 0;
    int w = 1;
    int h = 1;
};

// Names of the four samples of a 2x2 corner neighborhood (bit order TL,TR,BL,BR).
std::string CornerSetLabel(int b_mask)
{
    static const char* kNames[4] = {"TL", "TR", "BL", "BR"};
    std::string s;
    for (int i = 0; i < 4; ++i)
    {
        if (b_mask & (1 << i))
        {
            if (!s.empty())
            {
                s += "+";
            }
            s += kNames[i];
        }
    }
    return s.empty() ? "none" : s;
}

std::string BlockLabel(int tl, int tr, int bl, int br)
{
    return "TL=" + std::to_string(tl) + " TR=" + std::to_string(tr) + " BL=" + std::to_string(bl) +
           " BR=" + std::to_string(br);
}

// Lay out a list of 2x2 corner blocks (each = {TL,TR,BL,BR} class ids) on a
// pre-filled background, evenly spaced with margins so each config is isolated.
// Returns false if the raster is too small to give every block a margin.
bool PlaceCornerBlocks(Raster& r, const std::vector<std::array<int, 4>>& blocks, const std::vector<std::string>& labels,
    std::vector<LayoutEntry>& entries)
{
    const int n = static_cast<int>(blocks.size());
    const int gcols = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(n))));
    const int grows = (n + gcols - 1) / gcols;
    const int cellw = r.cols / gcols;
    const int cellh = r.rows / grows;
    if (cellw < 4 || cellh < 4)
    {
        return false; // need room for a 2x2 block plus a margin on every side
    }
    for (int i = 0; i < n; ++i)
    {
        const int gx = i % gcols;
        const int gy = i / gcols;
        const int c0 = gx * cellw + cellw / 2 - 1;
        const int r0 = gy * cellh + cellh / 2 - 1;
        r.Set(c0, r0, static_cast<uint8_t>(blocks[i][0]));         // TL
        r.Set(c0 + 1, r0, static_cast<uint8_t>(blocks[i][1]));     // TR
        r.Set(c0, r0 + 1, static_cast<uint8_t>(blocks[i][2]));     // BL
        r.Set(c0 + 1, r0 + 1, static_cast<uint8_t>(blocks[i][3])); // BR
        entries.push_back({labels[i], c0, r0, 2, 2});
    }
    return true;
}

// Read an explicit class-id grid (whitespace/comma separated, one raster row per
// line). All rows must have the same column count; values must be 0..255.
bool ReadGrid(const std::filesystem::path& path, Raster& r, std::string& err)
{
    std::ifstream in(path);
    if (!in)
    {
        err = "could not open --grid file " + path.string();
        return false;
    }
    std::vector<std::vector<int>> grid;
    std::string line;
    while (std::getline(in, line))
    {
        std::vector<int> rowvals;
        std::string tok;
        for (char ch : line)
        {
            if (ch == ',' || ch == ' ' || ch == '\t' || ch == '\r')
            {
                if (!tok.empty())
                {
                    rowvals.push_back(std::atoi(tok.c_str()));
                    tok.clear();
                }
            }
            else
            {
                tok += ch;
            }
        }
        if (!tok.empty())
        {
            rowvals.push_back(std::atoi(tok.c_str()));
        }
        if (!rowvals.empty())
        {
            grid.push_back(std::move(rowvals));
        }
    }
    if (grid.empty())
    {
        err = "--grid file is empty";
        return false;
    }
    const size_t cols = grid.front().size();
    for (const auto& row : grid)
    {
        if (row.size() != cols)
        {
            err = "--grid rows have inconsistent column counts";
            return false;
        }
        for (int v : row)
        {
            if (v < 0 || v > 255)
            {
                err = "--grid class ids must be 0..255";
                return false;
            }
        }
    }
    r.Resize(static_cast<int>(cols), static_cast<int>(grid.size()), 0);
    for (int row = 0; row < r.rows; ++row)
    {
        for (int col = 0; col < r.cols; ++col)
        {
            r.Set(col, row, static_cast<uint8_t>(grid[row][col]));
        }
    }
    return true;
}

// Build the land-class raster for the configured scenario and fill `entries`
// with the printed layout map. Returns false (with `err`) on a bad config.
bool BuildRaster(const Config& cfg, Raster& r, std::vector<LayoutEntry>& entries, std::string& err)
{
    if (cfg.scenario == Scenario::Grid)
    {
        if (cfg.grid_file.empty())
        {
            err = "--scenario grid requires --grid <file>";
            return false;
        }
        if (!ReadGrid(cfg.grid_file, r, err))
        {
            return false;
        }
        entries.push_back({"grid " + std::to_string(r.cols) + "x" + std::to_string(r.rows), 0, 0, r.cols, r.rows});
        return true;
    }

    const int n = cfg.raster;
    if (cfg.scenario == Scenario::Uniform)
    {
        r.Resize(n, n, static_cast<uint8_t>(cfg.class_a));
        entries.push_back({"uniform class " + std::to_string(cfg.class_a), 0, 0, n, n});
        return true;
    }
    if (cfg.scenario == Scenario::DualSet)
    {
        // Two large fields side by side; pick class_a / class_b so their ground
        // sets have DIFFERENT variant counts to probe per-set independence.
        r.Resize(n, n, static_cast<uint8_t>(cfg.class_a));
        for (int row = 0; row < n; ++row)
        {
            for (int col = n / 2; col < n; ++col)
            {
                r.Set(col, row, static_cast<uint8_t>(cfg.class_b));
            }
        }
        entries.push_back({"field A (left) class " + std::to_string(cfg.class_a), 0, 0, n / 2, n});
        entries.push_back({"field B (right) class " + std::to_string(cfg.class_b), n / 2, 0, n - n / 2, n});
        return true;
    }
    if (cfg.scenario == Scenario::Blocks)
    {
        // Holger's M-tile SUB-PANEL is chosen by the same-class BLOCK topology
        // (isolated corner-touch / straight edge / convex corner / concave
        // corner) — which the spaced 2x2 corner scenarios cannot produce. Paint
        // class_b shapes on a class_a field so every topology appears at once,
        // and read off in-sim which of the 8 M-tile sub-panels each uses.
        r.Resize(n, n, static_cast<uint8_t>(cfg.class_a));
        const uint8_t b = static_cast<uint8_t>(cfg.class_b);
        auto fill_rect = [&](int c0, int r0, int w, int h)
        {
            for (int rr = r0; rr < r0 + h; ++rr)
            {
                for (int cc = c0; cc < c0 + w; ++cc)
                {
                    r.Set(cc, rr, b);
                }
            }
        };
        // 1) solid rectangle: interior (no blend) + 4 straight edges + 4 convex corners.
        const int rw = std::max(4, n / 4);
        const int rh = std::max(4, n / 5);
        fill_rect(n / 8, n / 8, rw, rh);
        entries.push_back({"rect (edges + convex block-corners)", n / 8, n / 8, rw, rh});
        // 2) isolated single cell (class_b surrounded by class_a on all sides).
        r.Set(n * 3 / 4, n / 6, b);
        entries.push_back({"isolated 1x1 cell", n * 3 / 4, n / 6, 1, 1});
        // 3) diagonal pair: two cells touching at ONE corner only (corner-touch config).
        r.Set(n * 3 / 4, n / 2, b);
        r.Set(n * 3 / 4 + 1, n / 2 + 1, b);
        entries.push_back({"diagonal pair (corner-touch)", n * 3 / 4, n / 2, 2, 2});
        // 4) plus/cross: yields concave block-corners at the inner armpits.
        const int arm = std::max(1, n / 16);
        fill_rect(n / 4 + arm, n * 5 / 8, arm, 3 * arm);
        fill_rect(n / 4, n * 5 / 8 + arm, 3 * arm, arm);
        entries.push_back({"plus/cross (concave block-corners)", n / 4, n * 5 / 8, 3 * arm, 3 * arm});
        return true;
    }

    // corner scenarios: spaced 2x2 blocks on a class_a background.
    r.Resize(n, n, static_cast<uint8_t>(cfg.class_a));
    std::vector<std::array<int, 4>> blocks;
    std::vector<std::string> labels;
    if (cfg.scenario == Scenario::Corners2)
    {
        // all 14 two-class corner configs (skip mask 0 = all A and 15 = all B)
        for (int mask = 1; mask <= 14; ++mask)
        {
            std::array<int, 4> b{};
            for (int i = 0; i < 4; ++i)
            {
                b[i] = (mask & (1 << i)) ? cfg.class_b : cfg.class_a;
            }
            blocks.push_back(b);
            labels.push_back("cfg " + std::to_string(mask) + " (B@" + CornerSetLabel(mask) + ") " +
                             BlockLabel(b[0], b[1], b[2], b[3]));
        }
    }
    else if (cfg.scenario == Scenario::Corners3)
    {
        const int a = cfg.class_a, b = cfg.class_b, c = cfg.class_c;
        const std::array<std::array<int, 4>, 4> reps = {{{a, b, c, a}, {b, c, a, b}, {c, a, b, c}, {a, b, b, c}}};
        for (const auto& blk : reps)
        {
            blocks.push_back(blk);
            labels.push_back("3-class " + BlockLabel(blk[0], blk[1], blk[2], blk[3]));
        }
    }
    else // Corners4
    {
        const int a = cfg.class_a, b = cfg.class_b, c = cfg.class_c, d = cfg.class_d;
        const std::array<std::array<int, 4>, 3> reps = {{{a, b, c, d}, {a, b, d, c}, {d, c, b, a}}};
        for (const auto& blk : reps)
        {
            blocks.push_back(blk);
            labels.push_back("4-class " + BlockLabel(blk[0], blk[1], blk[2], blk[3]));
        }
    }
    if (!PlaceCornerBlocks(r, blocks, labels, entries))
    {
        err = "--raster too small for this scenario (increase --raster)";
        return false;
    }
    return true;
}

// The ordered list of distinct land classes a scenario paints (this is what the
// lookup patch repoints, in priority order — earlier = lower DrawPriority). For
// Grid, collect distinct raster values in ascending order.
std::vector<int> ScenarioClasses(const Config& cfg, const Raster& raster)
{
    switch (cfg.scenario)
    {
    case Scenario::Uniform:
        return {cfg.class_a};
    case Scenario::Corners3:
        return {cfg.class_a, cfg.class_b, cfg.class_c};
    case Scenario::Corners4:
        return {cfg.class_a, cfg.class_b, cfg.class_c, cfg.class_d};
    case Scenario::Grid:
    {
        std::vector<int> distinct;
        for (uint8_t v : raster.cells)
        {
            if (std::find(distinct.begin(), distinct.end(), static_cast<int>(v)) == distinct.end())
            {
                distinct.push_back(static_cast<int>(v));
            }
        }
        std::sort(distinct.begin(), distinct.end());
        return distinct;
    }
    case Scenario::Corners2:
    case Scenario::Blocks:
    case Scenario::DualSet:
    default:
        return {cfg.class_a, cfg.class_b};
    }
}

// Print the layout map: each config -> sample coords -> approximate lat/lon
// (reusing the equirect TileBounds estimate) so in-sim observations can be tied
// back to specific corner configs. lat/lon is only available for a lat/lon or
// derived QMID (not an explicit --qmid).
void PrintLayoutMap(const Raster& r, const std::vector<LayoutEntry>& entries, bool have_bounds, double lat_n,
    double lat_s, double lon_w, double lon_e)
{
    std::printf("  layout map (raster %dx%d, row 0 = north edge):\n", r.cols, r.rows);
    for (const auto& e : entries)
    {
        std::printf("    %s\n", e.label.c_str());
        if (e.w == 2 && e.h == 2)
        {
            std::printf("      samples: TL=(c%d,r%d) TR=(c%d,r%d) BL=(c%d,r%d) BR=(c%d,r%d)\n", e.col, e.row, e.col + 1,
                e.row, e.col, e.row + 1, e.col + 1, e.row + 1);
        }
        else
        {
            std::printf("      samples: cols[%d..%d] rows[%d..%d]\n", e.col, e.col + e.w - 1, e.row, e.row + e.h - 1);
        }
        if (have_bounds)
        {
            const double cc = e.col + (e.w - 1) / 2.0;
            const double cr = e.row + (e.h - 1) / 2.0;
            const double lon = lon_w + (cc + 0.5) / r.cols * (lon_e - lon_w);
            const double lat = lat_n - (cr + 0.5) / r.rows * (lat_n - lat_s);
            std::printf("      center ~ lat %.5f lon %.5f\n", lat, lon);
        }
    }
}

// Build one texture-set row carrying the swept GAP B knobs for a land class.
// Appended to each patched record; the region table is then repointed at it.
// `vulcn` (the painted class id) becomes TextureVULCN, so the ground set number
// == the class id and the generated ground tiles are named to match (author
// them with `m2_texgen --set <class id>`).
SBglTextureSet MakeTextureRow(const Config& cfg, int vulcn, int draw_priority)
{
    SBglTextureSet e{};
    e.TextureVULCN = static_cast<int16_t>(vulcn);
    e.TextureRegion = static_cast<uint8_t>(cfg.region);
    e.TextureVariation = static_cast<uint8_t>(cfg.texture_variation);
    e.SeasonMask = static_cast<int16_t>(cfg.season_mask);
    e.DrawPriority = static_cast<int16_t>(draw_priority);
    e.MaskVULCN = static_cast<int16_t>(cfg.mask_vulcn);
    e.MaskRegion = static_cast<uint8_t>(cfg.mask_region);
    e.MaskVariation = static_cast<uint8_t>(cfg.mask_variation);
    e.MaskTextureVariations = cfg.mask_texture_variations;
    e.AutogenVULCN = 0;
    e.AutogenRegion = 0;
    e.AutogenMask = 0;
    return e;
}

// Create the output's parent directory if it doesn't exist (so an --out under a
// not-yet-created folder doesn't fail at open time). Best-effort.
void EnsureParentDir(const std::filesystem::path& path)
{
    const auto parent = path.parent_path();
    if (!parent.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(parent, ec);
    }
}

// Truncate/create the file so the (non-truncating) BinaryFileStream ctor opens
// it empty. Returns false on failure.
bool TruncateCreate(const std::filesystem::path& path)
{
    std::ofstream create(path, std::ios::binary | std::ios::trunc);
    return static_cast<bool>(create);
}

// Write the fixed 0x38 file header for a single-layer DirectQmid BGL.
void WriteHeader(BinaryFileStream& out, int layer_count)
{
    SBglHeader header{};
    header.Version = 0x0201;
    header.FileMagic = 0x1992;
    header.HeaderSize = kHeaderSize;
    header.FileTime = 0;
    header.QmidMagic = 0x08051803;
    header.LayerCount = layer_count;
    header.PackedQMIDParent0 = 0;
    header.PackedQMIDParent1 = 0;
    header.PackedQMIDParent2 = 0;
    header.PackedQMIDParent3 = 0;
    header.PackedQMIDParent4 = 0;
    header.PackedQMIDParent5 = 0;
    header.PackedQMIDParent6 = 0;
    header.PackedQMIDParent7 = 0;
    SBglHeader::WriteBinary(out, header);
}

// Write a single DirectQmid layer pointer for one tile at StreamOffset.
// `tile_ptr_size` is the on-disk size of the single tile pointer that follows
// (16 for the packed 0x6F form, 20 for a full TRQ1 raster pointer).
void WriteSingleTileLayer(BinaryFileStream& out, EBglLayerType type, int tile_ptr, int tile_ptr_size)
{
    SBglLayerPointer layer{};
    layer.Type = type;
    layer.DataClass = static_cast<uint16_t>(EBglLayerClass::DirectQmid);
    layer.HasQmidHigh = 1;
    layer.TileCount = 1;
    layer.StreamOffset = static_cast<uint32_t>(tile_ptr);
    layer.SizeBytes = static_cast<uint32_t>(tile_ptr_size);
    layer.WriteBinary(out);
}

// Read two files fully and report whether they are byte-identical; on mismatch,
// print sizes and the first differing offset.
bool CompareFilesBytewise(const std::filesystem::path& a, const std::filesystem::path& b)
{
    std::ifstream fa(a, std::ios::binary);
    std::ifstream fb(b, std::ios::binary);
    if (!fa || !fb)
    {
        std::fprintf(stderr, "  compare: could not open both files\n");
        return false;
    }
    const std::vector<char> ba((std::istreambuf_iterator<char>(fa)), std::istreambuf_iterator<char>());
    const std::vector<char> bb((std::istreambuf_iterator<char>(fb)), std::istreambuf_iterator<char>());
    if (ba.size() != bb.size())
    {
        std::fprintf(stderr, "  compare: size differs (in=%zu out=%zu)\n", ba.size(), bb.size());
        return false;
    }
    for (size_t i = 0; i < ba.size(); ++i)
    {
        if (ba[i] != bb[i])
        {
            std::fprintf(stderr, "  compare: first byte differs at offset %zu (in=0x%02X out=0x%02X)\n", i,
                static_cast<uint8_t>(ba[i]), static_cast<uint8_t>(bb[i]));
            return false;
        }
    }
    return true;
}

// Read --lclookup-in and write it back UNCHANGED to --out-lookup, then assert
// the output is byte-for-byte identical. This is the faithful-writer regression
// guard: it must hold before any patch can be trusted to be FSX-safe.
bool RoundTripCheck(const Config& cfg)
{
    CBglFile bgl(cfg.lclookup_in.wstring());
    if (!bgl.Read())
    {
        std::fprintf(stderr, "FAIL: could not read %s\n", cfg.lclookup_in.string().c_str());
        return false;
    }
    EnsureParentDir(cfg.out_lookup);
    bgl.Rename(cfg.out_lookup.wstring().c_str());
    if (!bgl.Write())
    {
        std::fprintf(stderr, "FAIL: could not write %s\n", cfg.out_lookup.string().c_str());
        return false;
    }
    bgl.Close(); // flush before reading the output back for comparison
    return CompareFilesBytewise(cfg.lclookup_in, cfg.out_lookup);
}

// Patch the real global lclookup: read it, NON-DESTRUCTIVELY append class A / B
// rows carrying the swept knobs into every QMID record, repoint classes A/B in
// each region table to those rows, then re-emit the complete file to out_lookup
// (Rename before Write leaves the input file untouched). Reports how many
// records were patched and how many region entries were repointed.
bool PatchLookupBgl(const Config& cfg, const std::vector<int>& classes, int& records_patched, int& region_repoints)
{
    records_patched = 0;
    region_repoints = 0;

    CBglFile bgl(cfg.lclookup_in.wstring());
    if (!bgl.Read())
    {
        std::fprintf(stderr, "FAIL: could not read input lclookup %s\n", cfg.lclookup_in.string().c_str());
        return false;
    }

    auto* layer = bgl.GetDirectQmidLayer(EBglLayerType::TerrainTextureLookup);
    if (layer == nullptr)
    {
        std::fprintf(
            stderr, "FAIL: input has no TerrainTextureLookup (0x6F) layer: %s\n", cfg.lclookup_in.string().c_str());
        return false;
    }

    const int qmid_count = layer->GetQmidCount();
    for (int i = 0; i < qmid_count; ++i)
    {
        const auto* tile_ptr = layer->GetDataPointerAtIndex(i);
        if (tile_ptr == nullptr)
        {
            continue;
        }
        const CPackedQmid qmid{tile_ptr->QmidLow, tile_ptr->QmidHigh};
        const int data_count = layer->GetDataCountAtQmid(qmid);
        for (int j = 0; j < data_count; ++j)
        {
            auto* data = layer->GetDataAtQmid(qmid, j);
            if (data == nullptr)
            {
                continue;
            }
            auto* lookup = data->AsTerrainTextureLookup();
            if (lookup == nullptr)
            {
                continue;
            }

            const int land_count = lookup->GetLandClassCount();
            const int region_count = lookup->GetRegionCount();
            // Only patch records whose region table can address every class.
            bool addressable = true;
            for (int cls : classes)
            {
                if (cls < 0 || cls >= land_count)
                {
                    addressable = false;
                    break;
                }
            }
            if (!addressable)
            {
                continue;
            }

            // Append one texture-set row per class (TextureVULCN = class id, so the
            // ground set == the class id and our shipped tiles line up), giving each
            // a DISTINCT DrawPriority (base + index*step) so the later class wins
            // arbitration. Then repoint that class in every region table.
            for (size_t k = 0; k < classes.size(); ++k)
            {
                const int cls = classes[k];
                const int prio = cfg.draw_priority + static_cast<int>(k) * cfg.draw_priority_step;
                const int row = lookup->GetTextureCount();
                lookup->AddTexture(MakeTextureRow(cfg, cls, prio));
                for (int r = 0; r < region_count; ++r)
                {
                    lookup->SetRegionLandClassTexture(r, cls, row);
                    ++region_repoints;
                }
            }
            ++records_patched;
        }
    }

    if (records_patched == 0)
    {
        std::fprintf(stderr, "FAIL: no LCLookup record could address every test class "
                             "(input land-class count too small?). Nothing written.\n");
        return false;
    }

    EnsureParentDir(cfg.out_lookup);
    // Rename redirects the next Write at out_lookup; the input file (already
    // read into memory) is left untouched.
    bgl.Rename(cfg.out_lookup.wstring().c_str());
    if (!bgl.Write())
    {
        std::fprintf(stderr, "FAIL: could not write patched lclookup %s\n", cfg.out_lookup.string().c_str());
        return false;
    }
    return true;
}

// Write a 24-bit uncompressed BMP (bottom-up) from `pixels` (row-major, row 0 =
// north/top; each entry is a class id written as RGB(v,v,v)). Returns false on
// I/O failure.
bool WriteBmp24Gray(const std::filesystem::path& path, const std::vector<uint8_t>& pixels, int cols, int rows)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out)
    {
        return false;
    }
    const int row_stride = (cols * 3 + 3) & ~3; // padded to 4 bytes
    const uint32_t pixel_bytes = static_cast<uint32_t>(row_stride) * rows;
    const uint32_t file_size = 14 + 40 + pixel_bytes;

    auto put_u16 = [&out](uint16_t v) { out.put(static_cast<char>(v & 0xFF)).put(static_cast<char>((v >> 8) & 0xFF)); };
    auto put_u32 = [&out](uint32_t v)
    {
        out.put(static_cast<char>(v & 0xFF))
            .put(static_cast<char>((v >> 8) & 0xFF))
            .put(static_cast<char>((v >> 16) & 0xFF))
            .put(static_cast<char>((v >> 24) & 0xFF));
    };

    out.put('B').put('M'); // BITMAPFILEHEADER
    put_u32(file_size);
    put_u16(0);
    put_u16(0);
    put_u32(14 + 40);
    put_u32(40); // BITMAPINFOHEADER
    put_u32(static_cast<uint32_t>(cols));
    put_u32(static_cast<uint32_t>(rows)); // positive => bottom-up
    put_u16(1);
    put_u16(24);
    put_u32(0); // BI_RGB
    put_u32(pixel_bytes);
    put_u32(2835);
    put_u32(2835);
    put_u32(0);
    put_u32(0);

    std::vector<char> row(row_stride, 0);
    for (int y = rows - 1; y >= 0; --y) // bottom-up: emit south row first, north (row 0) last
    {
        for (int x = 0; x < cols; ++x)
        {
            const uint8_t v = pixels[static_cast<size_t>(y) * cols + x];
            row[x * 3 + 0] = static_cast<char>(v); // B
            row[x * 3 + 1] = static_cast<char>(v); // G
            row[x * 3 + 2] = static_cast<char>(v); // R
        }
        for (int p = cols * 3; p < row_stride; ++p)
        {
            row[p] = 0;
        }
        out.write(row.data(), row_stride);
    }
    return static_cast<bool>(out);
}

// Emit the resample INPUT for a multi-class land-class tile: a 24-bit BMP whose
// pixels encode class ids as RGB(v,v,v), plus a resample INF over the EXACT
// destination cell. The scenario raster is edge-clamp padded by `rs_margin`
// cells so the source OVERFILLS the cell (guaranteeing full coverage /
// SizeMask=0) while the inner NxN maps 1:1 onto the cell. This is the
// FSX-valid path (§7.3); it replaces the deprecated hand-rolled 0x68 writer.
bool WriteResampleSource(const Config& cfg, const Raster& raster)
{
    const int n_cols = raster.cols;
    const int n_rows = raster.rows;
    const int m = cfg.rs_margin < 0 ? 0 : cfg.rs_margin;
    const int pcols = n_cols + 2 * m;
    const int prows = n_rows + 2 * m;

    // Edge-clamp pad (row 0 = north). The clamped border continues the scenario's
    // edge class outward so the overfill region carries a valid land class.
    std::vector<uint8_t> padded(static_cast<size_t>(pcols) * prows, 0);
    for (int r = 0; r < prows; ++r)
    {
        const int sr = std::min(std::max(r - m, 0), n_rows - 1);
        for (int c = 0; c < pcols; ++c)
        {
            const int sc = std::min(std::max(c - m, 0), n_cols - 1);
            padded[static_cast<size_t>(r) * pcols + c] = raster.Get(sc, sr);
        }
    }

    const std::filesystem::path src_dir = cfg.rs_out / "SourceData";
    const std::filesystem::path out_dir = cfg.rs_out / "Output";
    std::error_code ec;
    std::filesystem::create_directories(src_dir, ec);
    std::filesystem::create_directories(out_dir, ec);

    const std::filesystem::path bmp_path = src_dir / (cfg.rs_base + ".bmp");
    if (!WriteBmp24Gray(bmp_path, padded, pcols, prows))
    {
        std::fprintf(stderr, "FAIL: could not write resample source BMP %s\n", bmp_path.string().c_str());
        return false;
    }

    // Degrees per scenario cell (the inner NxN maps exactly onto the dest cell),
    // then push the source NW corner out by the margin so it overfills.
    const double dlon = (cfg.dest_east - cfg.dest_west) / n_cols;
    const double dlat = (cfg.dest_north - cfg.dest_south) / n_rows;
    const double ulx = cfg.dest_west - m * dlon;
    const double uly = cfg.dest_north + m * dlat;

    const std::filesystem::path inf_path = cfg.rs_out / (cfg.rs_base + ".inf");
    std::ofstream inf(inf_path, std::ios::trunc);
    if (!inf)
    {
        std::fprintf(stderr, "FAIL: could not write resample INF %s\n", inf_path.string().c_str());
        return false;
    }
    inf.setf(std::ios::fixed);
    inf.precision(10);
    inf << "[Source]\n"
        << "Type=BMP\n"
        << "Layer=LandClass\n"
        << "SamplingMethod=Point\n"
        << "SourceDir=\"SourceData\"\n"
        << "SourceFile=\"" << cfg.rs_base << ".bmp\"\n"
        << "ULXMAP=" << ulx << "\n"
        << "ULYMAP=" << uly << "\n"
        << "XDIM=" << dlon << "\n"
        << "YDIM=" << dlat << "\n"
        << "[Destination]\n"
        << "DestDir=\"Output\"\n"
        << "DestBaseFileName=" << cfg.rs_base << "\n"
        << "DestFileType=BGL\n"
        << "UseSourceDimensions=0\n"
        << "NorthLat=" << cfg.dest_north << "\n"
        << "SouthLat=" << cfg.dest_south << "\n"
        << "WestLon=" << cfg.dest_west << "\n"
        << "EastLon=" << cfg.dest_east << "\n";
    if (!inf)
    {
        std::fprintf(stderr, "FAIL: error writing resample INF %s\n", inf_path.string().c_str());
        return false;
    }

    std::printf("m2_testgen: wrote resample source (FSX-valid multi-class land-class path):\n");
    std::printf("  BMP:  %s  (%dx%d, RGB(v,v,v)=class v; inner %dx%d maps to cell, +%d-cell overfill)\n",
        bmp_path.string().c_str(), pcols, prows, n_cols, n_rows, m);
    std::printf("  INF:  %s  (dest cell N=%.4f S=%.4f W=%.4f E=%.4f)\n", inf_path.string().c_str(), cfg.dest_north,
        cfg.dest_south, cfg.dest_west, cfg.dest_east);
    std::printf("  NEXT: run  resample \"%s\"  then verify the Output\\%s.bgl decodes to TileCount=1, the\n",
        inf_path.string().c_str(), cfg.rs_base.c_str());
    std::printf("        intended QMID, and SizeMask=0 before installing (Install-M2Scenery.ps1).\n");
    return true;
}

// The TerrainLandClass (0x68) raster on its own.
//
// !!! NOT FSX-VALID — DO NOT USE THIS OUTPUT IN THE SIM. Use the SDK `resample`
// tool instead (see LANDCLASS_SYNTHESIS.md §7.3). In-sim testing showed this
// hand-rolled writer is missing several things FSX requires; it round-trips
// through flightsimlib (lenient reader) but TMFViewer/FSX reject or corrupt it:
//   1. No companion TerrainIndex (0x6E) section — FSX loads the file but shows
//      no data and crashes without it (resample always emits 0x68 + 0x6E).
//   2. QMID is a naive equirect estimate (wrong projection AND level) — it does
//      not land on the intended cell. FSX QMID cells are ~3.75deg x 2.8125deg at
//      level 7, not the 360/2^level equirect grid this tool assumes.
//   3. The raster must FULLY cover its QMID cell (257x257, compressed). A
//      partially-covered cell keeps a missing-data mask, and land class does not
//      fill/blend missing data (unlike DEM/aerial) -> junk tiles in-sim.
// This function is retained only as a minimal 0x68 example for flightsimlib
// round-trip tests; the M2 experiment authors land class via `resample`.
bool WriteLandClassBgl(const Config& cfg, const Raster& raster)
{
    EnsureParentDir(cfg.out_landclass);
    if (!TruncateCreate(cfg.out_landclass))
    {
        return false;
    }
    BinaryFileStream out(cfg.out_landclass);
    if (!out)
    {
        return false;
    }

    const int tile_ptr = kHeaderSize + kLayerPointerSize * 1;
    const int record = tile_ptr + kTilePointerSize;
    const int class_cols = raster.cols;
    const int class_rows = raster.rows;
    const int class_payload = class_cols * class_rows;
    const int record_size = kTrq1HeaderSize + class_payload;

    WriteHeader(out, 1);
    WriteSingleTileLayer(out, EBglLayerType::TerrainLandClass, tile_ptr, kTilePointerSize);

    out << cfg.qmid_low << cfg.qmid_high << static_cast<uint32_t>(record) << static_cast<uint32_t>(record_size);
    out << kTrq1Magic;                                                    // Version
    out << static_cast<uint32_t>(record_size);                            // Size
    out << static_cast<uint16_t>(ERasterDataType::LandClass);             // DataType
    out << static_cast<uint8_t>(ERasterCompressionType::None);            // CompressionTypeData
    out << static_cast<uint8_t>(ERasterCompressionType::None);            // CompressionTypeMask
    out << cfg.qmid_low << cfg.qmid_high;                                 // QmidLow / QmidHigh
    out << static_cast<uint32_t>(0);                                      // Variations
    out << static_cast<uint16_t>(class_cols) << static_cast<uint16_t>(0); // Cols + padding
    out << static_cast<uint16_t>(class_rows) << static_cast<uint16_t>(0); // Rows + padding
    out << static_cast<uint32_t>(class_payload);                          // SizeData
    out << static_cast<uint32_t>(0);                                      // SizeMask
    for (int row = 0; row < class_rows; ++row)
    {
        for (int col = 0; col < class_cols; ++col)
        {
            out << raster.Get(col, row);
        }
    }

    return static_cast<bool>(out);
}

// Read the patched lclookup back through the M1 decoder and assert classes A/B
// in the first record's region table now resolve to rows carrying the swept
// knobs. Returns true on success.
bool VerifyPatched(const Config& cfg, const std::vector<int>& classes)
{
    int failures = 0;
    auto check = [&](bool ok, const char* msg)
    {
        if (!ok)
        {
            std::fprintf(stderr, "  verify FAIL: %s\n", msg);
            ++failures;
        }
    };

    CBglFile bgl(cfg.out_lookup.wstring());
    check(bgl.Read(), "CBglFile::Read(out_lookup)");

    auto* layer = bgl.GetDirectQmidLayer(EBglLayerType::TerrainTextureLookup);
    check(layer != nullptr, "GetDirectQmidLayer(TerrainTextureLookup)");
    if (layer == nullptr)
    {
        return false;
    }
    check(layer->GetQmidCount() > 0, "patched file retains >=1 QMID record");
    if (layer->GetQmidCount() == 0)
    {
        return false;
    }

    const auto* tile_ptr = layer->GetDataPointerAtIndex(0);
    check(tile_ptr != nullptr, "GetDataPointerAtIndex(0)");
    if (tile_ptr == nullptr)
    {
        return false;
    }
    const CPackedQmid qmid{tile_ptr->QmidLow, tile_ptr->QmidHigh};
    auto* data = layer->GetDataAtQmid(qmid, 0);
    check(data != nullptr, "GetDataAtQmid");
    if (data == nullptr)
    {
        return false;
    }
    auto* read = data->AsTerrainTextureLookup();
    check(read != nullptr, "AsTerrainTextureLookup");
    if (read == nullptr)
    {
        return false;
    }

    const int texture_count = read->GetTextureCount();
    // Every patched class must repoint to a row carrying the swept knobs + its
    // assigned per-class DrawPriority (base + index*step).
    for (size_t k = 0; k < classes.size(); ++k)
    {
        const int cls = classes[k];
        const int expected_prio = cfg.draw_priority + static_cast<int>(k) * cfg.draw_priority_step;
        const int row = read->GetRegionLandClassTexture(0, cls);
        check(row >= 0 && row < texture_count, "class repoint in range");
        const auto* e = (row >= 0 && row < texture_count) ? read->GetTextureAt(row) : nullptr;
        check(e != nullptr && e->TextureVULCN == static_cast<int16_t>(cls), "class row TextureVULCN == class id");
        check(e != nullptr && e->MaskTextureVariations == cfg.mask_texture_variations,
            "class row carries swept mask-variations");
        check(e != nullptr && e->TextureVariation == static_cast<uint8_t>(cfg.texture_variation),
            "class row carries swept texture-variation");
        check(e != nullptr && e->DrawPriority == static_cast<int16_t>(expected_prio),
            "class row carries assigned per-class DrawPriority");
    }

    // No-data-loss cross-check vs. the original input: Write() is not
    // byte-preserving (it re-emits padding tighter than the source compiler), so
    // assert that nothing beyond the two appended rows actually changed.
    CBglFile orig(cfg.lclookup_in.wstring());
    check(orig.Read(), "CBglFile::Read(lclookup_in)");
    check(orig.GetLayerCount() == bgl.GetLayerCount(), "layer count preserved (no layer dropped)");
    auto* olayer = orig.GetDirectQmidLayer(EBglLayerType::TerrainTextureLookup);
    check(olayer != nullptr, "orig GetDirectQmidLayer");
    if (olayer == nullptr || olayer->GetQmidCount() == 0)
    {
        return false;
    }
    check(olayer->GetQmidCount() == layer->GetQmidCount(), "QMID record count preserved");
    const auto* otp = olayer->GetDataPointerAtIndex(0);
    if (otp == nullptr)
    {
        return false;
    }
    const CPackedQmid oqmid{otp->QmidLow, otp->QmidHigh};
    auto* odata = olayer->GetDataAtQmid(oqmid, 0);
    auto* oread = odata != nullptr ? odata->AsTerrainTextureLookup() : nullptr;
    check(oread != nullptr, "orig AsTerrainTextureLookup");
    if (oread == nullptr)
    {
        return false;
    }

    const int orig_textures = oread->GetTextureCount();
    check(texture_count == orig_textures + static_cast<int>(classes.size()),
        "texture count grew by exactly one row per patched class");
    check(read->GetRegionCount() == oread->GetRegionCount(), "region count preserved");
    check(read->GetLandClassCount() == oread->GetLandClassCount(), "land-class count preserved");
    check(read->GetWaterClassCount() == oread->GetWaterClassCount(), "water-class count preserved");

    // Every original texture row must survive byte-for-byte (we only append).
    for (int i = 0; i < orig_textures; ++i)
    {
        const auto* op = oread->GetTextureAt(i);
        const auto* np = read->GetTextureAt(i);
        if (op == nullptr || np == nullptr)
        {
            check(false, "original texture row present after patch");
            continue;
        }
        const bool same = op->TextureVULCN == np->TextureVULCN && op->TextureRegion == np->TextureRegion &&
                          op->TextureVariation == np->TextureVariation && op->SeasonMask == np->SeasonMask &&
                          op->DrawPriority == np->DrawPriority && op->MaskVULCN == np->MaskVULCN &&
                          op->MaskRegion == np->MaskRegion && op->MaskVariation == np->MaskVariation &&
                          op->MaskTextureVariations == np->MaskTextureVariations &&
                          op->AutogenVULCN == np->AutogenVULCN && op->AutogenRegion == np->AutogenRegion &&
                          op->AutogenMask == np->AutogenMask;
        check(same, "original texture row preserved byte-for-byte");
    }

    // Region mappings for classes we did NOT patch must be untouched.
    const int land_count = read->GetLandClassCount();
    const int region_count = read->GetRegionCount();
    for (int r = 0; r < region_count; ++r)
    {
        for (int l = 0; l < land_count; ++l)
        {
            if (std::find(classes.begin(), classes.end(), l) != classes.end())
            {
                continue;
            }
            check(read->GetRegionLandClassTexture(r, l) == oread->GetRegionLandClassTexture(r, l),
                "non-patched region land-class mapping preserved");
        }
    }

    return failures == 0;
}

// FSX region qualifier -> texture-name letter. Holger: regions are coded
// A=0..Z=25, and terrain texture names use the lowercase letter.
char RegionLetter(uint8_t region) { return static_cast<char>('a' + (region % 26)); }

// Decode the packed 16 x 4-bit MaskTextureVariations into its per-slot variant
// indices (nibble 0 = least-significant). The default global lclookup stores
// 0x1111111111111111 for many ag/urban classes, which FORCES variant 1
// everywhere ("blocked" — no proper M-tile blend); a real ramp such as class
// 1's 0x1111111176543211 selects varied mask tiles. See LANDCLASS_SYNTHESIS §7.2.
std::string FormatVariantNibbles(int64_t packed)
{
    const uint64_t u = static_cast<uint64_t>(packed);
    std::string s = "[";
    for (int i = 0; i < 16; ++i)
    {
        if (i != 0)
        {
            s += ' ';
        }
        s += std::to_string(static_cast<unsigned>((u >> (i * 4)) & 0xFu));
    }
    s += "]";
    return s;
}

// --inspect (read-only): decode --lclookup-in and print, for --region and each
// requested class id, the resolved texture-set-row index (GetRegionLandClassTexture)
// and that row's raw fields + the DERIVED ground/mask file-name stems. This is
// how you pick test classes and find the active ground-set / M-tile-mask refs.
// Writes nothing.
bool InspectLookup(const Config& cfg, const std::vector<int>& classes)
{
    CBglFile bgl(cfg.lclookup_in.wstring());
    if (!bgl.Read())
    {
        std::fprintf(stderr, "FAIL: could not read %s\n", cfg.lclookup_in.string().c_str());
        return false;
    }
    auto* layer = bgl.GetDirectQmidLayer(EBglLayerType::TerrainTextureLookup);
    if (layer == nullptr)
    {
        std::fprintf(stderr, "FAIL: input has no TerrainTextureLookup (0x6F) layer\n");
        return false;
    }
    const int qmid_count = layer->GetQmidCount();
    if (qmid_count == 0)
    {
        std::fprintf(stderr, "FAIL: lookup has no QMID records\n");
        return false;
    }
    // Inspect the first record; the global table's per-QMID records share the
    // same region->texture structure, so record 0 is representative.
    const auto* tile_ptr = layer->GetDataPointerAtIndex(0);
    if (tile_ptr == nullptr)
    {
        std::fprintf(stderr, "FAIL: could not read first QMID record\n");
        return false;
    }
    const CPackedQmid qmid{tile_ptr->QmidLow, tile_ptr->QmidHigh};
    auto* data = layer->GetDataAtQmid(qmid, 0);
    auto* lookup = data != nullptr ? data->AsTerrainTextureLookup() : nullptr;
    if (lookup == nullptr)
    {
        std::fprintf(stderr, "FAIL: first record is not a TerrainTextureLookup\n");
        return false;
    }

    std::printf("m2_testgen --inspect: %s\n", cfg.lclookup_in.string().c_str());
    std::printf("  QMID records=%d  inspecting record 0 (qmidLow=0x%08X qmidHigh=0x%08X)\n", qmid_count,
        tile_ptr->QmidLow, tile_ptr->QmidHigh);
    std::printf("  textures=%d regions=%d landClasses=%d waterClasses=%d  (using region=%d)\n",
        lookup->GetTextureCount(), lookup->GetRegionCount(), lookup->GetLandClassCount(), lookup->GetWaterClassCount(),
        cfg.region);

    if (cfg.region < 0 || cfg.region >= lookup->GetRegionCount())
    {
        std::fprintf(stderr, "FAIL: --region %d out of range [0..%d)\n", cfg.region, lookup->GetRegionCount());
        return false;
    }

    for (int cls : classes)
    {
        std::printf("  class %d:\n", cls);
        if (cls < 0 || cls >= lookup->GetLandClassCount())
        {
            std::printf("    (out of land-class range [0..%d))\n", lookup->GetLandClassCount());
            continue;
        }
        const int row = lookup->GetRegionLandClassTexture(cfg.region, cls);
        std::printf("    -> texture row %d\n", row);
        const auto* e = (row >= 0 && row < lookup->GetTextureCount()) ? lookup->GetTextureAt(row) : nullptr;
        if (e == nullptr)
        {
            std::printf("    (no texture row)\n");
            continue;
        }
        // SeasonMask is conventionally displayed octal (see §6); show both.
        const unsigned season = static_cast<unsigned>(static_cast<uint16_t>(e->SeasonMask));
        std::printf("      Texture{VULCN=%d Region=%u(=%c) Variation=%u}  SeasonMask=0%o (0x%X)\n", e->TextureVULCN,
            e->TextureRegion, RegionLetter(e->TextureRegion), e->TextureVariation, season, season);
        std::printf("      DrawPriority=%d  Mask{VULCN=%d Region=%u(=%c) Variation=%u}\n", e->DrawPriority,
            e->MaskVULCN, e->MaskRegion, RegionLetter(e->MaskRegion), e->MaskVariation);
        std::printf("      MaskTextureVariations=0x%016llX nibbles=%s\n",
            static_cast<unsigned long long>(static_cast<uint64_t>(e->MaskTextureVariations)),
            FormatVariantNibbles(e->MaskTextureVariations).c_str());
        // Derived file-name stems (field->filename lock, §6.1/§7.2.6): ground
        // carries a season + hex variant; mask is season-less and its two-digit
        // suffix order differs by family (900-series m{V}1, set-specific m1{V});
        // V comes from the MaskTextureVariations nibbles above.
        std::printf("      ground ~ %03d%c2{season}{v}.bmp   (v = 1 hex digit) via TilePattern%u.bmp\n",
            e->TextureVULCN, RegionLetter(e->TextureRegion), e->TextureVariation);
        const char* mask_pat = (e->MaskVULCN >= 900) ? "m{V}1" : "m1{V}";
        std::printf("      mask   ~ %03d%c2%s.bmp         (%s) via TilePattern%u.bmp\n", e->MaskVULCN,
            RegionLetter(e->MaskRegion), mask_pat, (e->MaskVULCN >= 900 ? "900-series" : "set-specific"),
            e->MaskVariation);
        std::printf("      Autogen{VULCN=%d Region=%u Mask=%u}\n", e->AutogenVULCN, e->AutogenRegion, e->AutogenMask);
    }
    return true;
}

void PrintUsage(const char* argv0)
{
    std::printf("Usage: %s [options]\n"
                "\n"
                "Generates the M2 experiment inputs (LANDCLASS_SYNTHESIS.md M2): a land-class\n"
                "raster override .bgl, and (with --lclookup-in) a patched copy of the real\n"
                "global lclookup. FSX ignores a per-scenery-area 0x6F, so the lookup must be\n"
                "a patch of the global file, not a from-scratch standalone .bgl.\n"
                "\n"
                "Target QMID:\n"
                "  --qmid LOW [HIGH]   packed QMID directly (HIGH defaults to 0)\n"
                "  --lat DEG           latitude  (default 47.6)  [used if --qmid absent]\n"
                "  --lon DEG           longitude (default -122.3)\n"
                "  --level N           QMID level (default 11)\n"
                "\n"
                "Land-class raster (painted at sample resolution; a blend corner = a 2x2\n"
                "neighborhood of ~1 km samples). Prints a layout map (config -> sample coords\n"
                "-> approximate lat/lon) so in-sim observations can be correlated:\n"
                "  --scenario S        uniform | corners2 | corners3 | corners4 | blocks | dualset\n"
                "                      (default corners2)\n"
                "                        uniform  : one class over the tile (GAP 4 geo-indexing)\n"
                "                        corners2 : all 14 two-class corner configs (GAP 2/3)\n"
                "                        corners3 : 3-class corners; top-2 priorities blend (GAP 3)\n"
                "                        corners4 : 4-class corners; top-3 priorities blend (GAP 3)\n"
                "                        blocks   : class-B shapes on class-A; M-tile sub-panel\n"
                "                                   topology (edge/convex/concave/isolated) (GAP 2)\n"
                "                        dualset  : two large fields side by side (GAP 5)\n"
                "  --grid FILE         explicit class-id grid (whitespace/comma separated, one\n"
                "                      raster row per line); overrides --scenario\n"
                "  --raster N          built-in scenarios paint NxN samples (default 64)\n"
                "  --class-a ID        class A (default 1)\n"
                "  --class-b ID        class B (default 2)\n"
                "  --class-c ID        class C, used by corners3/corners4 (default 3)\n"
                "  --class-d ID        class D, used by corners4 (default 4)\n"
                "  --region ID         region id (default 0)\n"
                "\n"
                "Inspect (read-only; decode the real lclookup and exit):\n"
                "  --inspect           with --lclookup-in: for --region and --classes (default\n"
                "                      A,B), print each class's resolved texture row + raw\n"
                "                      fields (ground-set / blend-mask refs). Writes nothing.\n"
                "  --classes LIST      comma-separated class ids for --inspect\n"
                "\n"
                "GAP B sweep knobs (applied to the appended class A/B texture-set rows;\n"
                "the ground set number = the painted class id, TextureRegion = --region):\n"
                "  --texture-variation N  TextureVariation: ground TilePattern{n}.bmp scheme (default 0)\n"
                "  --mask-vulcn N         MaskVULCN: M-tile mask set, e.g. 900 (default 0)\n"
                "  --mask-region N        MaskRegion: region qualifier for the mask name (default 0)\n"
                "  --mask-variation N     MaskVariation: mask TilePattern{n}.bmp scheme (default 0)\n"
                "  --mask-variations N    MaskTextureVariations: packed 16x4-bit variant override,\n"
                "                         int64 (default 0). 0x1111111111111111 = force variant 1\n"
                "                         (FSX 'blocked' no-blend default); a ramp unblocks it.\n"
                "  --season-mask N        SeasonMask (default 0x0FFF = all months)\n"
                "  --draw-priority N      DrawPriority of the FIRST patched class (default 0)\n"
                "  --draw-priority-step N each further class gets +N (so the later class wins\n"
                "                         arbitration); classes patched in A,B,C,D order (default 10)\n"
                "\n"
                "Lookup patch (the FSX-correct path):\n"
                "  --lclookup-in PATH   real global lclookup.bgl to patch (REQUIRED to emit\n"
                "                       the lookup; omit to write only the land-class raster)\n"
                "  --out-lookup PATH    patched lookup output (default m2_lclookup.bgl)\n"
                "                       — back up the original, then swap this in as global\n"
                "  --roundtrip-check    read --lclookup-in, rewrite UNCHANGED to --out-lookup,\n"
                "                       assert byte-for-byte identity, then exit (writer guard)\n"
                "\n"
                "Land-class raster output — the FSX-valid path is resample (§7.3):\n"
                "  --emit-resample-source  write a 24-bit BMP (pixel RGB(v,v,v) = class v) +\n"
                "                       an INF over the exact dest cell, so the SDK `resample`\n"
                "                       builds a fully-covered multi-class tile (SizeMask=0).\n"
                "                       This REPLACES the deprecated hand-rolled 0x68 writer.\n"
                "  --rs-out DIR         output root: <DIR>/SourceData/<base>.bmp + <DIR>/<base>.inf\n"
                "                       (+ empty Output/ for resample) (default m2_resample)\n"
                "  --rs-base NAME       DestBaseFileName + source BMP stem (default m2lc)\n"
                "  --rs-margin N        overfill the source by N cells each side (default 4)\n"
                "  --north/--south/--west/--east DEG  exact dest cell bounds\n"
                "                       (default = Jenny Lake QMID 0x8304: 45 / 42.1875 / -112.5 / -108.75)\n"
                "  --out-landclass PATH legacy hand-rolled 0x68 output (default m2_landclass.bgl)\n"
                "                       — NOT FSX-valid; used only without --emit-resample-source\n"
                "  --no-verify         skip the read-back self-test\n",
        argv0);
}

std::vector<int> ParseIntList(const std::string& s)
{
    std::vector<int> out;
    size_t start = 0;
    while (start <= s.size())
    {
        const size_t comma = s.find(',', start);
        const std::string tok = s.substr(start, comma == std::string::npos ? std::string::npos : comma - start);
        if (!tok.empty())
        {
            out.push_back(std::atoi(tok.c_str()));
        }
        if (comma == std::string::npos)
        {
            break;
        }
        start = comma + 1;
    }
    return out;
}

bool ParseArgs(int argc, char** argv, Config& cfg)
{
    for (int i = 1; i < argc; ++i)
    {
        const std::string a = argv[i];
        std::string value;
        // Consume the next token as this option's value. Rejects a missing
        // value or a following long option (e.g. `--qmid --raster`, or `--lat`
        // at end of argv) so they fail fast instead of silently parsing as 0.
        // A single leading '-' is allowed so negative numbers (e.g.
        // `--lon -122.3`) still work.
        auto need = [&](const char* opt) -> bool
        {
            if (i + 1 >= argc || (argv[i + 1][0] == '-' && argv[i + 1][1] == '-'))
            {
                std::fprintf(stderr, "Option %s requires a value.\n", opt);
                return false;
            }
            value = argv[++i];
            return true;
        };
        if (a == "--qmid")
        {
            if (!need("--qmid"))
            {
                return false;
            }
            cfg.qmid_explicit = true;
            cfg.qmid_low = static_cast<uint32_t>(std::strtoul(value.c_str(), nullptr, 0));
            // Optional high word: consume only if a non-flag token follows.
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                cfg.qmid_high = static_cast<uint32_t>(std::strtoul(argv[++i], nullptr, 0));
            }
        }
        else if (a == "--lat")
        {
            if (!need("--lat"))
            {
                return false;
            }
            cfg.lat = std::atof(value.c_str());
        }
        else if (a == "--lon")
        {
            if (!need("--lon"))
            {
                return false;
            }
            cfg.lon = std::atof(value.c_str());
        }
        else if (a == "--level")
        {
            if (!need("--level"))
            {
                return false;
            }
            cfg.level = std::atoi(value.c_str());
        }
        else if (a == "--raster")
        {
            if (!need("--raster"))
            {
                return false;
            }
            cfg.raster = std::atoi(value.c_str());
        }
        else if (a == "--class-a")
        {
            if (!need("--class-a"))
            {
                return false;
            }
            cfg.class_a = std::atoi(value.c_str());
        }
        else if (a == "--class-b")
        {
            if (!need("--class-b"))
            {
                return false;
            }
            cfg.class_b = std::atoi(value.c_str());
        }
        else if (a == "--class-c")
        {
            if (!need("--class-c"))
            {
                return false;
            }
            cfg.class_c = std::atoi(value.c_str());
        }
        else if (a == "--class-d")
        {
            if (!need("--class-d"))
            {
                return false;
            }
            cfg.class_d = std::atoi(value.c_str());
        }
        else if (a == "--scenario")
        {
            if (!need("--scenario"))
            {
                return false;
            }
            if (value == "uniform")
            {
                cfg.scenario = Scenario::Uniform;
            }
            else if (value == "corners2")
            {
                cfg.scenario = Scenario::Corners2;
            }
            else if (value == "corners3")
            {
                cfg.scenario = Scenario::Corners3;
            }
            else if (value == "corners4")
            {
                cfg.scenario = Scenario::Corners4;
            }
            else if (value == "blocks")
            {
                cfg.scenario = Scenario::Blocks;
            }
            else if (value == "dualset")
            {
                cfg.scenario = Scenario::DualSet;
            }
            else
            {
                std::fprintf(stderr, "Unknown scenario: %s\n", value.c_str());
                return false;
            }
        }
        else if (a == "--grid")
        {
            if (!need("--grid"))
            {
                return false;
            }
            cfg.grid_file = value;
            cfg.scenario = Scenario::Grid;
        }
        else if (a == "--classes")
        {
            if (!need("--classes"))
            {
                return false;
            }
            cfg.inspect_classes = ParseIntList(value);
        }
        else if (a == "--inspect")
        {
            cfg.inspect = true;
        }
        else if (a == "--region")
        {
            if (!need("--region"))
            {
                return false;
            }
            cfg.region = std::atoi(value.c_str());
        }
        else if (a == "--mask-variations")
        {
            if (!need("--mask-variations"))
            {
                return false;
            }
            cfg.mask_texture_variations = static_cast<int64_t>(std::strtoll(value.c_str(), nullptr, 0));
        }
        else if (a == "--texture-variation")
        {
            if (!need("--texture-variation"))
            {
                return false;
            }
            cfg.texture_variation = std::atoi(value.c_str());
        }
        else if (a == "--mask-vulcn")
        {
            if (!need("--mask-vulcn"))
            {
                return false;
            }
            cfg.mask_vulcn = std::atoi(value.c_str());
        }
        else if (a == "--mask-region")
        {
            if (!need("--mask-region"))
            {
                return false;
            }
            cfg.mask_region = std::atoi(value.c_str());
        }
        else if (a == "--mask-variation")
        {
            if (!need("--mask-variation"))
            {
                return false;
            }
            cfg.mask_variation = std::atoi(value.c_str());
        }
        else if (a == "--season-mask")
        {
            if (!need("--season-mask"))
            {
                return false;
            }
            cfg.season_mask = static_cast<int>(std::strtol(value.c_str(), nullptr, 0));
        }
        else if (a == "--draw-priority")
        {
            if (!need("--draw-priority"))
            {
                return false;
            }
            cfg.draw_priority = std::atoi(value.c_str());
        }
        else if (a == "--draw-priority-step")
        {
            if (!need("--draw-priority-step"))
            {
                return false;
            }
            cfg.draw_priority_step = std::atoi(value.c_str());
        }
        else if (a == "--lclookup-in")
        {
            if (!need("--lclookup-in"))
            {
                return false;
            }
            cfg.lclookup_in = value;
        }
        else if (a == "--roundtrip-check")
        {
            cfg.roundtrip_check = true;
        }
        else if (a == "--out-lookup")
        {
            if (!need("--out-lookup"))
            {
                return false;
            }
            cfg.out_lookup = value;
        }
        else if (a == "--out-landclass")
        {
            if (!need("--out-landclass"))
            {
                return false;
            }
            cfg.out_landclass = value;
        }
        else if (a == "--emit-resample-source")
        {
            cfg.emit_resample_source = true;
        }
        else if (a == "--rs-out")
        {
            if (!need("--rs-out"))
            {
                return false;
            }
            cfg.rs_out = value;
        }
        else if (a == "--rs-base")
        {
            if (!need("--rs-base"))
            {
                return false;
            }
            cfg.rs_base = value;
        }
        else if (a == "--rs-margin")
        {
            if (!need("--rs-margin"))
            {
                return false;
            }
            cfg.rs_margin = std::atoi(value.c_str());
        }
        else if (a == "--north")
        {
            if (!need("--north"))
            {
                return false;
            }
            cfg.dest_north = std::atof(value.c_str());
        }
        else if (a == "--south")
        {
            if (!need("--south"))
            {
                return false;
            }
            cfg.dest_south = std::atof(value.c_str());
        }
        else if (a == "--west")
        {
            if (!need("--west"))
            {
                return false;
            }
            cfg.dest_west = std::atof(value.c_str());
        }
        else if (a == "--east")
        {
            if (!need("--east"))
            {
                return false;
            }
            cfg.dest_east = std::atof(value.c_str());
        }
        else if (a == "--no-verify")
        {
            cfg.verify = false;
        }
        else if (a == "-h" || a == "--help")
        {
            PrintUsage(argv[0]);
            std::exit(0);
        }
        else
        {
            std::fprintf(stderr, "Unknown argument: %s\n", a.c_str());
            return false;
        }
    }
    return true;
}

} // namespace

int main(int argc, char** argv)
{
    Config cfg;
    if (!ParseArgs(argc, argv, cfg))
    {
        PrintUsage(argv[0]);
        return 1;
    }

    if (cfg.raster < 2 || cfg.raster > 4096 || cfg.level < 1 || cfg.level > 24)
    {
        std::fprintf(stderr, "Invalid raster/level (raster 2..4096, level 1..24).\n");
        return 1;
    }

    // Validate that values fit their on-disk field sizes; out-of-range inputs
    // would otherwise silently wrap and produce a misleading test file.
    auto in_u8 = [](int v) { return v >= 0 && v <= 255; };
    auto in_i16 = [](int v) { return v >= -32768 && v <= 32767; };
    if (!in_u8(cfg.class_a) || !in_u8(cfg.class_b) || !in_u8(cfg.class_c) || !in_u8(cfg.class_d))
    {
        std::fprintf(stderr, "Land-class ids must be 0..255 (written as uint8 raster samples).\n");
        return 1;
    }
    if (cfg.class_a == cfg.class_b)
    {
        std::fprintf(stderr, "--class-a and --class-b must differ (they map to distinct texture rows).\n");
        return 1;
    }
    if (cfg.scenario == Scenario::Corners3 && (cfg.class_a == cfg.class_c || cfg.class_b == cfg.class_c))
    {
        std::fprintf(stderr, "corners3 needs distinct --class-a/-b/-c (they are the 3 classes).\n");
        return 1;
    }
    if (cfg.scenario == Scenario::Corners4 &&
        (cfg.class_a == cfg.class_c || cfg.class_a == cfg.class_d || cfg.class_b == cfg.class_c ||
            cfg.class_b == cfg.class_d || cfg.class_c == cfg.class_d))
    {
        std::fprintf(stderr, "corners4 needs distinct --class-a/-b/-c/-d (they are the 4 classes).\n");
        return 1;
    }
    if (!in_u8(cfg.region) || !in_u8(cfg.texture_variation) || !in_u8(cfg.mask_variation) || !in_u8(cfg.mask_region))
    {
        std::fprintf(stderr, "--region / --texture-variation / --mask-variation / --mask-region must be 0..255.\n");
        return 1;
    }
    if (!in_i16(cfg.mask_vulcn) || !in_i16(cfg.season_mask) || !in_i16(cfg.draw_priority))
    {
        std::fprintf(stderr, "--mask-vulcn / --season-mask / --draw-priority must fit int16 (-32768..32767).\n");
        return 1;
    }

    unsigned int tile_x = 0;
    unsigned int tile_y = 0;
    if (!cfg.qmid_explicit)
    {
        LatLonToTile(cfg.lat, cfg.lon, cfg.level, tile_x, tile_y);
        const uint64_t packed = PackQmid(tile_x, tile_y, cfg.level);
        cfg.qmid_low = static_cast<uint32_t>(packed & 0xFFFFFFFFu);
        cfg.qmid_high = static_cast<uint32_t>(packed >> 32);
    }

    // Diagnostic short-circuit: prove the writer round-trips the real file
    // byte-for-byte, then exit (writes nothing else).
    if (cfg.roundtrip_check)
    {
        if (cfg.lclookup_in.empty())
        {
            std::fprintf(stderr, "--roundtrip-check requires --lclookup-in <file>.\n");
            return 1;
        }
        if (!RoundTripCheck(cfg))
        {
            std::fprintf(stderr, "m2_testgen: round-trip check FAILED (writer is not byte-faithful)\n");
            return 2;
        }
        std::printf("m2_testgen: round-trip check PASS — %s rewritten byte-for-byte to %s\n",
            cfg.lclookup_in.string().c_str(), cfg.out_lookup.string().c_str());
        return 0;
    }

    // Read-only short-circuit: decode the real lclookup and report the resolved
    // ground-set / blend-mask refs for the chosen classes, then exit.
    if (cfg.inspect)
    {
        if (cfg.lclookup_in.empty())
        {
            std::fprintf(stderr, "--inspect requires --lclookup-in <file>.\n");
            return 1;
        }
        const std::vector<int> classes =
            cfg.inspect_classes.empty() ? std::vector<int>{cfg.class_a, cfg.class_b} : cfg.inspect_classes;
        if (!InspectLookup(cfg, classes))
        {
            std::fprintf(stderr, "m2_testgen: --inspect FAILED\n");
            return 2;
        }
        return 0;
    }

    // Paint the land-class raster for the chosen scenario (or explicit grid).
    Raster raster;
    std::vector<LayoutEntry> entries;
    std::string build_err;
    if (!BuildRaster(cfg, raster, entries, build_err))
    {
        std::fprintf(stderr, "FAIL: %s\n", build_err.c_str());
        return 1;
    }

    if (cfg.emit_resample_source)
    {
        // FSX-valid multi-class land-class path: emit a resample BMP+INF instead
        // of the deprecated hand-rolled 0x68 writer (see §7.3).
        if (!WriteResampleSource(cfg, raster))
        {
            return 1;
        }
    }
    else
    {
        // Legacy: the hand-rolled 0x68 raster (NOT FSX-valid — kept only as a
        // flightsimlib round-trip example; see WriteLandClassBgl's comment).
        if (!WriteLandClassBgl(cfg, raster))
        {
            std::fprintf(stderr, "FAIL: could not write %s\n", cfg.out_landclass.string().c_str());
            return 1;
        }
        std::printf("m2_testgen: wrote %s  (TerrainLandClass 0x68 -> NOT FSX-valid; use --emit-resample-source)\n",
            cfg.out_landclass.string().c_str());
    }

    // The lookup is only producible by patching the real global file.
    const std::vector<int> patch_classes = ScenarioClasses(cfg, raster);
    int records_patched = 0;
    int region_repoints = 0;
    if (!cfg.lclookup_in.empty())
    {
        if (!PatchLookupBgl(cfg, patch_classes, records_patched, region_repoints))
        {
            return 1;
        }
        std::printf("m2_testgen: wrote %s  (patched global LCLookup 0x6F -> swap in as global)\n",
            cfg.out_lookup.string().c_str());
        std::string clslist;
        for (size_t k = 0; k < patch_classes.size(); ++k)
        {
            const int prio = cfg.draw_priority + static_cast<int>(k) * cfg.draw_priority_step;
            clslist += (k ? ", " : "") + std::to_string(patch_classes[k]) + "(prio " + std::to_string(prio) + ")";
        }
        std::printf("  patched %d record(s), repointed %d region entr(ies) for classes: %s\n", records_patched,
            region_repoints, clslist.c_str());
    }
    else
    {
        std::printf("m2_testgen: lclookup NOT written — pass --lclookup-in <real global lclookup.bgl> to patch it.\n");
        std::printf("  (FSX ignores a per-scenery-area 0x6F, so there is no safe from-scratch global lookup.)\n");
    }
    bool have_bounds = false;
    double lat_n = 0, lon_w = 0, lat_s = 0, lon_e = 0;
    if (cfg.emit_resample_source)
    {
        // The resample path drives geo entirely from the explicit dest cell
        // bounds, so correlate the layout map against THOSE (not the equirect
        // QMID estimate, which is unused here).
        have_bounds = true;
        lat_n = cfg.dest_north;
        lat_s = cfg.dest_south;
        lon_w = cfg.dest_west;
        lon_e = cfg.dest_east;
        std::printf("  dest cell: lat[%.4f..%.4f] lon[%.4f..%.4f] (exact; from --north/south/west/east)\n", lat_s,
            lat_n, lon_w, lon_e);
    }
    else
    {
        std::printf(
            "  QMID: low=0x%08X high=0x%08X%s\n", cfg.qmid_low, cfg.qmid_high, cfg.qmid_explicit ? " (explicit)" : "");
        have_bounds = !cfg.qmid_explicit;
        if (have_bounds)
        {
            TileBounds(tile_x, tile_y, cfg.level, lat_n, lon_w, lat_s, lon_e);
            std::printf("  tile=(%u,%u) level=%d  ~bounds: lat[%.4f..%.4f] lon[%.4f..%.4f] (equirect estimate)\n",
                tile_x, tile_y, cfg.level, lat_s, lat_n, lon_w, lon_e);
        }
    }
    auto scenario_name = [](Scenario s) -> const char*
    {
        switch (s)
        {
        case Scenario::Uniform:
            return "uniform";
        case Scenario::Corners2:
            return "corners2";
        case Scenario::Corners3:
            return "corners3";
        case Scenario::Corners4:
            return "corners4";
        case Scenario::Blocks:
            return "blocks";
        case Scenario::DualSet:
            return "dualset";
        case Scenario::Grid:
            return "grid";
        }
        return "?";
    };
    std::printf("  scenario: %s  classes A=%d B=%d C=%d D=%d  raster=%dx%d  region=%d\n", scenario_name(cfg.scenario),
        cfg.class_a, cfg.class_b, cfg.class_c, cfg.class_d, raster.cols, raster.rows, cfg.region);
    std::printf("  swept knobs: textureVariation=%d mask{vulcn=%d region=%d variation=%d} maskVariations=0x%llX\n",
        cfg.texture_variation, cfg.mask_vulcn, cfg.mask_region, cfg.mask_variation,
        static_cast<unsigned long long>(static_cast<uint64_t>(cfg.mask_texture_variations)));
    PrintLayoutMap(raster, entries, have_bounds, lat_n, lat_s, lon_w, lon_e);

    // Self-test only applies to the patched lookup (the land-class raster is a
    // straight authored write).
    if (cfg.verify && !cfg.lclookup_in.empty())
    {
        if (!VerifyPatched(cfg, patch_classes))
        {
            std::fprintf(stderr, "m2_testgen: self-test FAILED\n");
            return 2;
        }
        std::printf("  self-test: PASS (patched lookup read back via CBglFile + AsTerrainTextureLookup)\n");
    }
    return 0;
}
