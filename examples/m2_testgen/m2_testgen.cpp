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
// Authors TWO single-layer .bgl files over a chosen QMID:
//   1. <out-lookup>    a TerrainTextureLookup (LCLookup, 0x6F) record mapping
//      land classes to texture-lookup rows, with the GAP B knobs
//      (BlendTextureVariant, the *Mask refs) exposed as CLI flags so the
//      maintainer can SWEEP them.
//   2. <out-landclass> a TerrainLandClass (0x68) raster painted with a clean
//      A/B class boundary (half / diagonal / checker) so the in-sim land-class
//      transition — the dithered 1-bit-mask blend of GDC2006 §7.1 / Fig 10 — is
//      directly observable, and so it can be cross-referenced against the
//      numbered textures from m2_texgen.
//
// Why two files instead of one 2-layer .bgl: FSX's terrain engine only honors
// LCLookup (0x6F) from its single GLOBAL lookup file — a per-scenery-area 0x6F
// is legal by BGL structure but is ignored by the terrain engine (this differs
// from MS Flight (2012), where the lookup can be overridden per layer). The
// land-class raster (0x68), by contrast, IS overridable from a scenery-layer
// .bgl. So for the M2 experiment the LCLookup file must REPLACE the global
// default lookup, while the land-class file drops into an active scenery area's
// scenery/ folder. Both records carry the same QMID.
//
// This is M2 *prep*: it produces the experiment input. The actual fly / observe
// / write-down-the-rule step happens on local FSX (see the M2 protocol in
// LANDCLASS_SYNTHESIS.md). It deliberately reuses the M1 round-trip's byte
// layout (the layout verified by lclookup_roundtrip) so the file is known-good.
//
// All integer I/O is little-endian, matching flightsimlib's BinaryStream.
// Returns 0 on success, non-zero on failure.
//

#include "BglData.h"
#include "BglFile.h"
#include "BglTypes.h"
#include "BinaryStream.h"

#include <algorithm>
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
using flightsimlib::io::SBglTextureLookupEntry;

namespace
{

constexpr int kHeaderSize = 0x38;
constexpr int kLayerPointerSize = 20;
constexpr int kTilePointerSize = 16; // packed direct-QMID form (no RecordCount)
constexpr int kTrq1HeaderSize = 40;
constexpr uint32_t kTrq1Magic = 0x31515254; // 'TRQ1'

enum class Layout
{
    Half,     // left half class A, right half class B (one clean vertical seam)
    Diagonal, // lower-left A, upper-right B (one diagonal seam)
    Checker,  // alternating A/B cells (maximises boundary length)
};

struct Config
{
    std::filesystem::path out_lookup = "m2_lclookup.bgl";
    std::filesystem::path out_landclass = "m2_landclass.bgl";

    // Target QMID. Either supplied directly, or derived from lat/lon/level.
    bool qmid_explicit = false;
    uint32_t qmid_low = 0;
    uint32_t qmid_high = 0;
    double lat = 47.6; // default: near Seattle, WA
    double lon = -122.3;
    int level = 11;

    // Land-class raster.
    int raster = 64; // raster is raster x raster cells
    int class_a = 1;
    int class_b = 2;
    Layout layout = Layout::Half;

    // GAP B sweep knobs (applied to every authored texture row).
    int64_t variant = 0;
    int blend_vulcn = 0;
    int blend_region = 0;
    int blend_mask = 0;
    int vulcn_mask = 0;
    int season_mask = 0x0FFF; // all 12 months by default
    int draw_priority = 0;

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

uint8_t ClassAtCell(const Config& cfg, int col, int row)
{
    const bool a_or_b = [&]() -> bool
    {
        switch (cfg.layout)
        {
        case Layout::Half:
            return col < cfg.raster / 2;
        case Layout::Diagonal:
            return (col + row) < cfg.raster;
        case Layout::Checker:
            return ((col / 4) + (row / 4)) % 2 == 0;
        }
        return true;
    }();
    return static_cast<uint8_t>(a_or_b ? cfg.class_a : cfg.class_b);
}

// Author the LCLookup record: one texture row per (region, class) carrying the
// swept GAP B knobs, and a region table mapping the painted classes to those
// rows. NumLandclasses is sized so the table is indexable directly by raster
// class id (slot == id), which keeps the raster<->lookup correspondence trivial
// for the experiment (documented in the M2 protocol).
void BuildLookup(const Config& cfg, CBglTerrainTextureLookup& lookup, int& row_a, int& row_b)
{
    const int num_land = std::max(cfg.class_a, cfg.class_b) + 1;
    const int num_regions = cfg.region + 1;
    const int num_water = 1;

    lookup.SetHeaderMagic(0x4C434C4B); // 'LCLK'
    lookup.ResizeTables(num_regions, num_land, num_water);
    lookup.ClearTextures();

    auto make_row = [&](int vulcn) -> SBglTextureLookupEntry
    {
        SBglTextureLookupEntry e{};
        e.VULCNNumber = static_cast<int16_t>(vulcn);
        e.VULCNRegion = static_cast<uint8_t>(cfg.region);
        e.VULCNMask = static_cast<uint8_t>(cfg.vulcn_mask);
        e.SeasonMask = static_cast<int16_t>(cfg.season_mask);
        e.DrawPriority = static_cast<int16_t>(cfg.draw_priority);
        e.BlendTextureVULCN = static_cast<int16_t>(cfg.blend_vulcn);
        e.BlendTextureRegion = static_cast<uint8_t>(cfg.blend_region);
        e.BlendTextureMask = static_cast<uint8_t>(cfg.blend_mask);
        e.BlendTextureVariant = cfg.variant;
        e.AutogenVULCN = 0;
        e.AutogenRegion = 0;
        e.AutogenMask = 0;
        return e;
    };

    row_a = 0;
    lookup.AddTexture(make_row(cfg.class_a));
    row_b = 1;
    lookup.AddTexture(make_row(cfg.class_b));

    // Region table: every land/water slot defaults to row 0; the two painted
    // classes point at their own rows.
    for (int r = 0; r < num_regions; ++r)
    {
        for (int l = 0; l < num_land; ++l)
        {
            lookup.SetRegionLandClassTexture(r, l, 0);
        }
        for (int w = 0; w < num_water; ++w)
        {
            lookup.SetRegionWaterClassTexture(r, w, 0);
        }
    }
    lookup.SetRegionLandClassTexture(cfg.region, cfg.class_a, row_a);
    lookup.SetRegionLandClassTexture(cfg.region, cfg.class_b, row_b);

    // Identity slope / vector remaps (no remap) — exercised later in M4/M5.
    for (int l = 0; l < num_land; ++l)
    {
        for (int b = 0; b < IBglTerrainTextureLookup::SlopeLookupCount; ++b)
        {
            lookup.SetSlopeLookup(l, b, static_cast<uint8_t>(l));
        }
        for (int v = 0; v < IBglTerrainTextureLookup::VectorLookupCount; ++v)
        {
            lookup.SetVectorLookup(l, v, static_cast<uint8_t>(l));
        }
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
void WriteSingleTileLayer(BinaryFileStream& out, EBglLayerType type, int tile_ptr)
{
    SBglLayerPointer layer{};
    layer.Type = type;
    layer.DataClass = static_cast<uint16_t>(EBglLayerClass::DirectQmid);
    layer.HasQmidHigh = 1;
    layer.TileCount = 1;
    layer.StreamOffset = static_cast<uint32_t>(tile_ptr);
    layer.SizeBytes = kTilePointerSize;
    layer.WriteBinary(out);
}

// File 1: the LCLookup (0x6F) record on its own. This is the file that REPLACES
// FSX's single global terrain-texture-lookup — a per-scenery-area 0x6F is not
// honored by the terrain engine (see the header comment).
bool WriteLookupBgl(const Config& cfg, CBglTerrainTextureLookup& lookup)
{
    if (!TruncateCreate(cfg.out_lookup))
    {
        return false;
    }
    BinaryFileStream out(cfg.out_lookup);
    if (!out)
    {
        return false;
    }

    const int tile_ptr = kHeaderSize + kLayerPointerSize * 1;
    const int record = tile_ptr + kTilePointerSize;
    const int record_size = lookup.CalculateSize();

    WriteHeader(out, 1);
    WriteSingleTileLayer(out, EBglLayerType::TerrainTextureLookup, tile_ptr);

    out << cfg.qmid_low << cfg.qmid_high << static_cast<uint32_t>(record) << static_cast<uint32_t>(record_size);
    lookup.WriteBinary(out);

    return static_cast<bool>(out);
}

// File 2: the TerrainLandClass (0x68) raster on its own. This is the file that
// drops into an active scenery area's scenery/ folder as a land-class override.
bool WriteLandClassBgl(const Config& cfg)
{
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
    const int class_cols = cfg.raster;
    const int class_rows = cfg.raster;
    const int class_payload = class_cols * class_rows;
    const int record_size = kTrq1HeaderSize + class_payload;

    WriteHeader(out, 1);
    WriteSingleTileLayer(out, EBglLayerType::TerrainLandClass, tile_ptr);

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
            out << ClassAtCell(cfg, col, row);
        }
    }

    return static_cast<bool>(out);
}

// Read the just-written .bgl back through the M1 decoder and assert the
// authored LCLookup fields survive the round trip. Returns true on success.
bool Verify(const Config& cfg, int row_a, int row_b)
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
    check(bgl.Read(), "CBglFile::Read");

    auto* layer = bgl.GetDirectQmidLayer(EBglLayerType::TerrainTextureLookup);
    check(layer != nullptr, "GetDirectQmidLayer(TerrainTextureLookup)");
    if (layer == nullptr)
    {
        return false;
    }

    const CPackedQmid qmid{cfg.qmid_low, cfg.qmid_high};
    check(layer->GetDataCountAtQmid(qmid) == 1, "record count at QMID == 1");
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

    check(read->GetTextureCount() == 2, "texture count == 2");
    const auto* a = read->GetTextureAt(row_a);
    const auto* b = read->GetTextureAt(row_b);
    check(a != nullptr && b != nullptr, "texture rows present");
    if (a != nullptr)
    {
        check(a->VULCNNumber == cfg.class_a, "row A VULCNNumber");
        check(a->BlendTextureVariant == cfg.variant, "row A BlendTextureVariant");
        check(a->VULCNMask == static_cast<uint8_t>(cfg.vulcn_mask), "row A VULCNMask");
    }
    if (b != nullptr)
    {
        check(b->VULCNNumber == cfg.class_b, "row B VULCNNumber");
        check(b->BlendTextureVariant == cfg.variant, "row B BlendTextureVariant");
    }
    check(read->GetRegionLandClassTexture(cfg.region, cfg.class_a) == row_a, "region table class A -> row A");
    check(read->GetRegionLandClassTexture(cfg.region, cfg.class_b) == row_b, "region table class B -> row B");

    return failures == 0;
}

void PrintUsage(const char* argv0)
{
    std::printf("Usage: %s [options]\n"
                "\n"
                "Generates an M2 LCLookup + land-class test .bgl (LANDCLASS_SYNTHESIS.md M2).\n"
                "\n"
                "Target QMID:\n"
                "  --qmid LOW [HIGH]   packed QMID directly (HIGH defaults to 0)\n"
                "  --lat DEG           latitude  (default 47.6)  [used if --qmid absent]\n"
                "  --lon DEG           longitude (default -122.3)\n"
                "  --level N           QMID level (default 11)\n"
                "\n"
                "Land-class raster:\n"
                "  --raster N          NxN cells (default 64)\n"
                "  --class-a ID        first land-class id  (default 1)\n"
                "  --class-b ID        second land-class id (default 2)\n"
                "  --layout L          half | diag | checker (default half)\n"
                "  --region ID         region id (default 0)\n"
                "\n"
                "GAP B sweep knobs (applied to every authored texture row):\n"
                "  --variant N         BlendTextureVariant (int64, default 0)\n"
                "  --vulcn-mask N      VULCNMask (default 0)\n"
                "  --blend-vulcn N     BlendTextureVULCN (default 0)\n"
                "  --blend-region N    BlendTextureRegion (default 0)\n"
                "  --blend-mask N      BlendTextureMask (default 0)\n"
                "  --season-mask N     SeasonMask (default 0x0FFF = all months)\n"
                "  --draw-priority N   DrawPriority (default 0)\n"
                "\n"
                "  --out-lookup PATH    output LCLookup .bgl (default m2_lclookup.bgl)\n"
                "                       — replaces FSX's global terrain-texture lookup\n"
                "  --out-landclass PATH output land-class .bgl (default m2_landclass.bgl)\n"
                "                       — drops into a scenery area's scenery/ folder\n"
                "  --no-verify         skip the read-back self-test\n",
        argv0);
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
        else if (a == "--layout")
        {
            if (!need("--layout"))
            {
                return false;
            }
            if (value == "half")
            {
                cfg.layout = Layout::Half;
            }
            else if (value == "diag")
            {
                cfg.layout = Layout::Diagonal;
            }
            else if (value == "checker")
            {
                cfg.layout = Layout::Checker;
            }
            else
            {
                std::fprintf(stderr, "Unknown layout: %s\n", value.c_str());
                return false;
            }
        }
        else if (a == "--region")
        {
            if (!need("--region"))
            {
                return false;
            }
            cfg.region = std::atoi(value.c_str());
        }
        else if (a == "--variant")
        {
            if (!need("--variant"))
            {
                return false;
            }
            cfg.variant = static_cast<int64_t>(std::strtoll(value.c_str(), nullptr, 0));
        }
        else if (a == "--vulcn-mask")
        {
            if (!need("--vulcn-mask"))
            {
                return false;
            }
            cfg.vulcn_mask = std::atoi(value.c_str());
        }
        else if (a == "--blend-vulcn")
        {
            if (!need("--blend-vulcn"))
            {
                return false;
            }
            cfg.blend_vulcn = std::atoi(value.c_str());
        }
        else if (a == "--blend-region")
        {
            if (!need("--blend-region"))
            {
                return false;
            }
            cfg.blend_region = std::atoi(value.c_str());
        }
        else if (a == "--blend-mask")
        {
            if (!need("--blend-mask"))
            {
                return false;
            }
            cfg.blend_mask = std::atoi(value.c_str());
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
    if (!in_u8(cfg.class_a) || !in_u8(cfg.class_b))
    {
        std::fprintf(stderr, "Land-class ids must be 0..255 (written as uint8 raster samples).\n");
        return 1;
    }
    if (cfg.class_a == cfg.class_b)
    {
        std::fprintf(stderr, "--class-a and --class-b must differ (they map to distinct texture rows).\n");
        return 1;
    }
    if (!in_u8(cfg.region) || !in_u8(cfg.vulcn_mask) || !in_u8(cfg.blend_mask) || !in_u8(cfg.blend_region))
    {
        std::fprintf(stderr, "--region / --vulcn-mask / --blend-mask / --blend-region must be 0..255.\n");
        return 1;
    }
    if (!in_i16(cfg.blend_vulcn) || !in_i16(cfg.season_mask) || !in_i16(cfg.draw_priority))
    {
        std::fprintf(stderr, "--blend-vulcn / --season-mask / --draw-priority must fit int16 (-32768..32767).\n");
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

    CBglTerrainTextureLookup lookup;
    int row_a = 0;
    int row_b = 0;
    BuildLookup(cfg, lookup, row_a, row_b);

    if (!WriteLookupBgl(cfg, lookup))
    {
        std::fprintf(stderr, "FAIL: could not write %s\n", cfg.out_lookup.string().c_str());
        return 1;
    }
    if (!WriteLandClassBgl(cfg))
    {
        std::fprintf(stderr, "FAIL: could not write %s\n", cfg.out_landclass.string().c_str());
        return 1;
    }

    std::printf("m2_testgen: wrote %s  (LCLookup 0x6F  -> replaces FSX's global lookup)\n",
        cfg.out_lookup.string().c_str());
    std::printf("m2_testgen: wrote %s  (TerrainLandClass 0x68 -> scenery-layer override)\n",
        cfg.out_landclass.string().c_str());
    std::printf(
        "  QMID: low=0x%08X high=0x%08X%s\n", cfg.qmid_low, cfg.qmid_high, cfg.qmid_explicit ? " (explicit)" : "");
    if (!cfg.qmid_explicit)
    {
        double lat_n, lon_w, lat_s, lon_e;
        TileBounds(tile_x, tile_y, cfg.level, lat_n, lon_w, lat_s, lon_e);
        std::printf("  tile=(%u,%u) level=%d  ~bounds: lat[%.4f..%.4f] lon[%.4f..%.4f] (equirect estimate)\n", tile_x,
            tile_y, cfg.level, lat_s, lat_n, lon_w, lon_e);
    }
    std::printf("  land classes: A=%d B=%d  raster=%dx%d  region=%d\n", cfg.class_a, cfg.class_b, cfg.raster,
        cfg.raster, cfg.region);
    std::printf("  texture rows: A->row%d B->row%d  variant=%lld vulcnMask=%d blendMask=%d\n", row_a, row_b,
        static_cast<long long>(cfg.variant), cfg.vulcn_mask, cfg.blend_mask);

    if (cfg.verify)
    {
        if (!Verify(cfg, row_a, row_b))
        {
            std::fprintf(stderr, "m2_testgen: self-test FAILED\n");
            return 2;
        }
        std::printf("  self-test: PASS (read back via CBglFile + AsTerrainTextureLookup)\n");
    }
    return 0;
}
