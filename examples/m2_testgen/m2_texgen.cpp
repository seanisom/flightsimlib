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
// M2 synthetic land-class texture generator (LANDCLASS_SYNTHESIS.md §7 M2).
//
// Emits the two FSX-named texture families that the M2 reverse-engineering
// experiment loads into the sim to pin down the (now-understood) blend /
// variation mechanism — see LANDCLASS_SYNTHESIS.md §2.1 / §6 / §7.1. GROUND
// tiles are 24-bit BMP (no external image dependency) and are converted to the
// FSX "FS70" terrain format (imagetool -terrain, NOT -dds; see
// scripts/Convert-M2Textures.ps1 -Direction bmp2fs). MASK atlases are 1-bit
// monochrome BMP to match the REAL FSX mask format (in-sim inspection: default
// 900b2m11.bmp is 1024x8192 bpp=1 BI_RGB + FS70 marker) — use --size 1024 for
// exact real dims; the FS70 wrapper (if FSX requires it) is applied at install.
//
// Selected by --mode {ground|mask|both} (default both):
//
//   GROUND family ({set:03d}{region}2{season}{v}.bmp, v=1..N; v is a single HEX
//   digit, e.g. ...fa9 -> ...faa):
//     The per-variant ground tiles for a land-class "set". Each tile bakes its
//     variant number large + altitude-legible, on a background painted with
//     Holger's 16-color tilepattern legend color for index (v-1). That ties the
//     ON-GROUND color to the per-cell variant color a tilepattern{N}.bmp index
//     map selects in-sim, so you can read which variant landed where.
//     (NOTE: 0-based legend color vs 1-based filename variant — confirm the off
//     -by-one in-sim.)  Install: scenery package texture/.
//
//   MASK family (v=1..K, default K=7). 1-BIT MONOCHROME BMP (matches the real
//   FSX mask format). The two-digit suffix order DIFFERS by family (a genuine
//   FSX quirk): generic 900-series = {set}{region}2m{V}1.bmp (variation first,
//   e.g. 900b2m21), set-specific = {set}{region}2m1{V}.bmp (constant 1 first,
//   e.g. 031b2m11):
//     The M-tiles (blend masks): ONE BMP per variant, each an 8-panel vertical
//     atlas (1024x8192 in the real 900 series = 8 x 1024^2; set-specific masks
//     are narrower/anisotropic, e.g. 031 = 32x2048). The sub-panel FSX uses is
//     selected directly by the cell's 2x2 corner config (four QMID15 corner
//     samples), decoded from the real 900b2m21 atlas: 1 = TR+BL diagonal, 2 =
//     TL+BR diagonal, 3 = vertical edge, 4 = horizontal edge, 5..8 = single
//     corner BR/BL/TR/TL. Each placeholder panel bakes a geometric approximation
//     of that coverage as a STOCHASTIC dithered 1-bit stipple (covered = black)
//     with a "V{v} {index}" label. (V comes from the row's MaskTextureVariations;
//     set-specific masks are used iff those flags are non-trivial. The panel-3/4
//     black/white polarity is confirm-in-sim.)  Install: root Scenery\World\texture.
//
// Returns 0 on success, 1 on any I/O failure.
//

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{

// ---- 5x7 bitmap glyphs (1 bit/pixel, MSB = leftmost of 5 columns) ----------
// Index by character; only the glyphs we render are populated.
struct Glyph
{
    char ch;
    uint8_t rows[7]; // low 5 bits used
};

constexpr Glyph kGlyphs[] = {
    {'0', {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}},
    {'1', {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}},
    {'2', {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}},
    {'3', {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E}},
    {'4', {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}},
    {'5', {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}},
    {'6', {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}},
    {'7', {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}},
    {'8', {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}},
    {'9', {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}},
    {'C', {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}},
    {'R', {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}},
    {'V', {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}},
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
};

const Glyph* FindGlyph(char ch)
{
    for (const auto& g : kGlyphs)
    {
        if (g.ch == ch)
        {
            return &g;
        }
    }
    return nullptr;
}

struct Rgb
{
    uint8_t r, g, b;
};

// A simple RGBA8-style image buffer (we only need RGB on disk).
struct Image
{
    int w = 0;
    int h = 0;
    std::vector<Rgb> px;

    Image(int width, int height, Rgb fill) : w(width), h(height), px(static_cast<size_t>(width) * height, fill) { }

    void Set(int x, int y, Rgb c)
    {
        if (x < 0 || y < 0 || x >= w || y >= h)
        {
            return;
        }
        px[static_cast<size_t>(y) * w + x] = c;
    }
};

// Black or white, whichever contrasts better with bg (Rec. 601 luma).
Rgb ContrastInk(Rgb bg)
{
    const int luma = (299 * bg.r + 587 * bg.g + 114 * bg.b) / 1000;
    return luma > 140 ? Rgb{0, 0, 0} : Rgb{255, 255, 255};
}

// Draw one glyph at (ox, oy) scaled by `scale`, in color `ink`.
void DrawGlyph(Image& img, const Glyph& g, int ox, int oy, int scale, Rgb ink)
{
    for (int row = 0; row < 7; ++row)
    {
        for (int col = 0; col < 5; ++col)
        {
            const bool on = (g.rows[row] >> (4 - col)) & 1;
            if (!on)
            {
                continue;
            }
            for (int sy = 0; sy < scale; ++sy)
            {
                for (int sx = 0; sx < scale; ++sx)
                {
                    img.Set(ox + col * scale + sx, oy + row * scale + sy, ink);
                }
            }
        }
    }
}

// Draw a left-to-right string starting at (ox, oy). Unknown chars are skipped.
void DrawText(Image& img, const std::string& text, int ox, int oy, int scale, Rgb ink)
{
    int x = ox;
    for (char ch : text)
    {
        const Glyph* g = FindGlyph(ch);
        if (g != nullptr)
        {
            DrawGlyph(img, *g, x, oy, scale, ink);
        }
        x += (5 + 1) * scale; // 5px glyph + 1px gap
    }
}

// Pixel advance / height of a string drawn at `scale` (5px glyph + 1px gap).
int TextWidth(const std::string& text, int scale) { return static_cast<int>(text.size()) * (5 + 1) * scale; }
int TextHeight(int scale) { return 7 * scale; }

bool WriteBmp24(const std::filesystem::path& path, const Image& img)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out)
    {
        return false;
    }

    const int row_stride = (img.w * 3 + 3) & ~3; // padded to 4 bytes
    const uint32_t pixel_bytes = static_cast<uint32_t>(row_stride) * img.h;
    const uint32_t file_size = 14 + 40 + pixel_bytes;

    auto put_u16 = [&out](uint16_t v) { out.put(static_cast<char>(v & 0xFF)).put(static_cast<char>((v >> 8) & 0xFF)); };
    auto put_u32 = [&out](uint32_t v)
    {
        out.put(static_cast<char>(v & 0xFF))
            .put(static_cast<char>((v >> 8) & 0xFF))
            .put(static_cast<char>((v >> 16) & 0xFF))
            .put(static_cast<char>((v >> 24) & 0xFF));
    };

    // BITMAPFILEHEADER
    out.put('B').put('M');
    put_u32(file_size);
    put_u16(0);
    put_u16(0);
    put_u32(14 + 40); // pixel data offset

    // BITMAPINFOHEADER
    put_u32(40);
    put_u32(static_cast<uint32_t>(img.w));
    put_u32(static_cast<uint32_t>(img.h)); // positive => bottom-up
    put_u16(1);
    put_u16(24);
    put_u32(0); // BI_RGB
    put_u32(pixel_bytes);
    put_u32(2835); // ~72 DPI
    put_u32(2835);
    put_u32(0);
    put_u32(0);

    std::vector<char> row(row_stride, 0);
    for (int y = img.h - 1; y >= 0; --y) // bottom-up
    {
        for (int x = 0; x < img.w; ++x)
        {
            const Rgb& c = img.px[static_cast<size_t>(y) * img.w + x];
            row[x * 3 + 0] = static_cast<char>(c.b); // BGR
            row[x * 3 + 1] = static_cast<char>(c.g);
            row[x * 3 + 2] = static_cast<char>(c.r);
        }
        for (int p = img.w * 3; p < row_stride; ++p)
        {
            row[p] = 0;
        }
        out.write(row.data(), row_stride);
    }

    return static_cast<bool>(out);
}

// Write a 1-bit monochrome BMP (bottom-up), 2-entry palette (0=black, 1=white).
// A pixel is bit 0 (black) if it is dark in `img`, else bit 1 (white). This
// matches the REAL FSX M-tile mask format (in-sim inspection: the default
// 900b2m11.bmp is 1024x8192, bpp=1, BI_RGB, with an FS70 marker) — see
// LANDCLASS_SYNTHESIS.md §7.1/§7.3. (The FS70 wrapper is added by the converter,
// not here.)
bool WriteBmp1Mono(const std::filesystem::path& path, const Image& img)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out)
    {
        return false;
    }
    const int row_stride = ((img.w + 31) / 32) * 4; // 1 bpp, padded to 4 bytes
    const uint32_t pixel_bytes = static_cast<uint32_t>(row_stride) * img.h;
    const uint32_t data_off = 14 + 40 + 8; // headers + 2-entry palette
    const uint32_t file_size = data_off + pixel_bytes;

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
    put_u32(data_off);
    put_u32(40); // BITMAPINFOHEADER
    put_u32(static_cast<uint32_t>(img.w));
    put_u32(static_cast<uint32_t>(img.h)); // positive => bottom-up
    put_u16(1);
    put_u16(1); // 1 bpp
    put_u32(0); // BI_RGB
    put_u32(pixel_bytes);
    put_u32(2835);
    put_u32(2835);
    put_u32(2); // biClrUsed
    put_u32(2); // biClrImportant
    // Palette: index 0 = black, index 1 = white (BGRA).
    out.put(0).put(0).put(0).put(0);
    out.put(static_cast<char>(255)).put(static_cast<char>(255)).put(static_cast<char>(255)).put(0);

    std::vector<uint8_t> row(row_stride, 0);
    for (int y = img.h - 1; y >= 0; --y) // bottom-up
    {
        std::fill(row.begin(), row.end(), static_cast<uint8_t>(0));
        for (int x = 0; x < img.w; ++x)
        {
            const Rgb& c = img.px[static_cast<size_t>(y) * img.w + x];
            const int luma = (299 * c.r + 587 * c.g + 114 * c.b) / 1000;
            if (luma >= 128) // white => bit 1
            {
                row[x >> 3] |= static_cast<uint8_t>(0x80 >> (x & 7));
            }
        }
        out.write(reinterpret_cast<const char*>(row.data()), row_stride);
    }
    return static_cast<bool>(out);
}

// ---- M2 texture families ---------------------------------------------------

// Holger's FSX Land Class Table 16-color legend (tilepattern{N}.bmp cell color
// == variant index). Index 0..15 -> the color the in-sim index map paints for
// that variant. We bake the same color into the matching ground variant tile so
// the on-ground color ties back to the tilepattern color.
constexpr Rgb kLegend[16] = {
    {0, 0, 0},       // 0  black
    {220, 0, 0},     // 1  red
    {235, 220, 0},   // 2  yellow
    {120, 220, 110}, // 3  lt green
    {120, 190, 235}, // 4  lt blue
    {0, 60, 200},    // 5  blue
    {235, 130, 175}, // 6  pink
    {80, 0, 90},     // 7  dk purple
    {175, 130, 85},  // 8  lt brown
    {0, 110, 40},    // 9  dk green
    {0, 140, 130},   // 10 blue-green
    {0, 30, 110},    // 11 dk blue
    {130, 0, 150},   // 12 purple
    {200, 235, 190}, // 13 pale green
    {200, 225, 245}, // 14 pale blue
    {200, 160, 235}, // 15 lt purple
};

// "%03d" set number prefix (e.g. 900 -> "900", 5 -> "005").
std::string SetPrefix(int set)
{
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%03d", set);
    return std::string(buf);
}

// FSX texture names use a single LOWERCASE HEX digit for the variant (e.g.
// ...fa9.bmp -> ...faa.bmp for variant 9 -> 10; block schemes reach 16). The
// 0- vs 1-based numbering is still confirm-in-sim.
char VariantDigit(int v) { return "0123456789abcdef"[v & 0xF]; }

// One ground variant tile: big legible variant number on the legend color for
// (variant-1), 1px border, small set caption. Filename:
//   {set:03d}{region}2{season}{v}.bmp   (e.g. 900b2su1.bmp)
bool WriteGroundTexture(const std::filesystem::path& dir, int set, char region, const std::string& season, int variant,
    int size, bool color_by_set)
{
    // Background color: by VARIANT (legend[v-1]) ties on-ground color to the
    // tilepattern cell color (geo-indexing test); by SET (legend[(set-1)%16])
    // gives each land-class SET a single distinct color, so different classes are
    // visually separable at a blend boundary (the M2 blend tests). The off-by-one
    // (0-based legend color vs 1-based filename variant) is the working
    // assumption; confirm in-sim (see header note).
    const int raw_index = color_by_set ? (set - 1) : (variant - 1);
    const int color_index = ((raw_index % 16) + 16) % 16;
    const Rgb bg = kLegend[color_index];
    const Rgb ink = ContrastInk(bg);
    Image img(size, size, bg);

    // Thin border so tile edges (and thus tiling/repeat) are visible in-sim.
    for (int i = 0; i < size; ++i)
    {
        img.Set(i, 0, ink);
        img.Set(i, size - 1, ink);
        img.Set(0, i, ink);
        img.Set(size - 1, i, ink);
    }

    // Big centered variant number (altitude-legible). Size it to fill the tile.
    const std::string num = std::to_string(variant);
    const int chars = static_cast<int>(num.size());
    int scale = static_cast<int>((0.70 * size) / (chars * 6));
    const int by_height = static_cast<int>((0.55 * size) / 7);
    if (scale > by_height)
    {
        scale = by_height;
    }
    if (scale < 2)
    {
        scale = 2;
    }
    const int tx = (size - TextWidth(num, scale)) / 2;
    const int ty = (size - TextHeight(scale)) / 2;
    DrawText(img, num, tx, ty, scale, ink);

    // Small set caption top-left (region/season live in the filename).
    const int sscale = size >= 128 ? 2 : 1;
    DrawText(img, std::to_string(set), 3, 3, sscale, ink);

    const std::string name =
        SetPrefix(set) + std::string(1, region) + "2" + season + std::string(1, VariantDigit(variant)) + ".bmp";
    return WriteBmp24(dir / name, img);
}

// The 8 M-tile sub-panels, top (index 1) to bottom (index 8). Decoded from the
// REAL 900b2m21 atlas (128x1024 = 8 x 128^2): the sub-panel is selected directly
// by the cell's 2x2 corner configuration (four QMID15 corner samples), and the
// black (coverage) sits in these quadrants (LANDCLASS_SYNTHESIS.md §7.2.3):
//   1 : TR+BL  (anti-diagonal / checkerboard touch)   [FSX retains WHITE]
//   2 : TL+BR  (main diagonal / checkerboard touch)    [FSX retains WHITE]
//   3 : TL+BL  (vertical edge; left/right by polarity) [retain B/W by adjacency]
//   4 : TL+TR  (horizontal edge; top/bottom by polarity)
//   5 : BR     (single corner)                         [retain BLACK]
//   6 : BL     (single corner)
//   7 : TR     (single corner)
//   8 : TL     (single corner)
// These placeholder tiles bake geometric approximations of that black
// distribution (dithered 1-bit at the boundary) so the in-sim panel choice can
// be read off; the real atlas coverage is organic/fractal. The retain-black/white
// polarity direction for panels 3/4 is still confirm-in-sim.
enum MaskShape
{
    ShDiagAnti, // 1: TR + BL
    ShDiagMain, // 2: TL + BR
    ShEdgeLeft, // 3: TL + BL (left column)
    ShEdgeTop,  // 4: TL + TR (top row)
    ShCornerBR, // 5: BR
    ShCornerBL, // 6: BL
    ShCornerTR, // 7: TR
    ShCornerTL, // 8: TL
};

struct MaskSubPanel
{
    MaskShape shape;
    const char* tag; // short label (index only; glyph set is digits/V/space)
};

constexpr MaskSubPanel kSubPanels[8] = {
    {ShDiagAnti, "1"},
    {ShDiagMain, "2"},
    {ShEdgeLeft, "3"},
    {ShEdgeTop, "4"},
    {ShCornerBR, "5"},
    {ShCornerBL, "6"},
    {ShCornerTR, "7"},
    {ShCornerTL, "8"},
};

// Signed "inside" distance for a sub-panel shape at (u,v) in [0,1] (u right, v
// down): positive inside the covered (black) region, ~0 at the boundary. A band
// around the boundary is dithered so each sub-panel keeps the M-tile stipple.
double ShapeSignedDistance(MaskShape shape, double u, double v)
{
    const double d_tl = std::min(0.5 - u, 0.5 - v);
    const double d_tr = std::min(u - 0.5, 0.5 - v);
    const double d_bl = std::min(0.5 - u, v - 0.5);
    const double d_br = std::min(u - 0.5, v - 0.5);
    switch (shape)
    {
    case ShDiagAnti:
        return std::max(d_tr, d_bl);
    case ShDiagMain:
        return std::max(d_tl, d_br);
    case ShEdgeLeft:
        return 0.5 - u;
    case ShEdgeTop:
        return 0.5 - v;
    case ShCornerBR:
        return d_br;
    case ShCornerBL:
        return d_bl;
    case ShCornerTR:
        return d_tr;
    case ShCornerTL:
        return d_tl;
    }
    return -1.0;
}

// Per-pixel stochastic dither threshold in [0,1). Mixing variant + panel makes
// each variant's mask noise distinct (the per-variant mask set is the point).
double DitherThreshold(int x, int y, int variant, int panel)
{
    uint32_t h = static_cast<uint32_t>(x) * 73856093u ^ static_cast<uint32_t>(y) * 19349663u ^
                 static_cast<uint32_t>(variant) * 83492791u ^ static_cast<uint32_t>(panel) * 2654435761u;
    h ^= h >> 13;
    h *= 0x85ebca6bu;
    h ^= h >> 16;
    return (h & 0xFFFFu) / 65536.0;
}

// One per-variant 8-sub-panel M-tile atlas (8 stacked square panels), dithered
// 1-bit coverage (covered = black). Filename: {set:03d}{region}2m1{v}.bmp
// (matching Holger's m11/m12/... series; the variant is the LAST digit).
bool WriteMaskAtlas(const std::filesystem::path& dir, int set, char region, int variant, int size)
{
    constexpr int kPanels = 8;
    constexpr double kBand = 0.22; // width of the dithered transition band
    Image img(size, size * kPanels, Rgb{255, 255, 255});

    for (int t = 0; t < kPanels; ++t)
    {
        const MaskShape shape = kSubPanels[t].shape;
        const int y0 = t * size;
        long covered = 0;
        for (int y = 0; y < size; ++y)
        {
            const double v = (y + 0.5) / size;
            for (int x = 0; x < size; ++x)
            {
                const double u = (x + 0.5) / size;
                const double d = ShapeSignedDistance(shape, u, v);
                const double cov = std::min(1.0, std::max(0.0, 0.5 + d / kBand));
                if (cov > DitherThreshold(x, y, variant, t))
                {
                    img.Set(x, y0 + y, Rgb{0, 0, 0});
                    ++covered;
                }
            }
        }

        // Label = "V{variant} {index}" in a corner, drawn in the minority color
        // over a majority-color corner so it reads without perturbing coverage.
        const double frac = static_cast<double>(covered) / (static_cast<double>(size) * size);
        const bool minority_black = frac < 0.5;
        const Rgb ink = minority_black ? Rgb{0, 0, 0} : Rgb{255, 255, 255};
        const bool want_covered = !minority_black;
        // Sample the 4 corners' coverage to find one matching the majority color.
        const double cu[4] = {0.06, 0.94, 0.06, 0.94}; // TL,TR,BL,BR
        const double cv[4] = {0.06, 0.06, 0.94, 0.94};
        int corner = 0;
        for (int i = 0; i < 4; ++i)
        {
            const bool cov_here = ShapeSignedDistance(shape, cu[i], cv[i]) > 0.0;
            if (cov_here == want_covered)
            {
                corner = i;
                break;
            }
        }
        const int lscale = size >= 64 ? std::max(1, size / 64) : 1;
        const std::string label = "V" + std::to_string(variant) + " " + kSubPanels[t].tag;
        const int lw = TextWidth(label, lscale);
        const int lh = TextHeight(lscale);
        const int margin = 3;
        const int lx = (corner == 1 || corner == 3) ? (size - lw - margin) : margin;
        const int ly = (corner == 2 || corner == 3) ? (y0 + size - lh - margin) : (y0 + margin);
        DrawText(img, label, lx, ly, lscale, ink);
    }

    // Two-digit suffix, order differs by family (a genuine FSX quirk): generic
    // 900-series = m{V}1 (variation first), set-specific = m1{V} (constant 1
    // first). V = the variation number.
    const std::string vd(1, VariantDigit(variant));
    const std::string suffix = (set >= 900) ? ("m" + vd + "1") : ("m1" + vd);
    const std::string name = SetPrefix(set) + std::string(1, region) + "2" + suffix + ".bmp";
    // Masks are 1-bit monochrome to match the real FSX format (bpp=1). Use
    // --size 1024 for the exact real 1024x8192 atlas dimensions.
    return WriteBmp1Mono(dir / name, img);
}

void PrintUsage(const char* argv0)
{
    std::printf("Usage: %s [--mode ground|mask|both] [options]\n"
                "\n"
                "Generates the M2 FSX-named land-class texture families (LANDCLASS_SYNTHESIS.md\n"
                "§7 M2). Output is 24-bit BMP; convert GROUND to the FSX FS70 terrain format\n"
                "(imagetool -terrain, not -dds; Convert-M2Textures.ps1 -Direction bmp2fs), then\n"
                "install: GROUND -> scenery package texture/ ; MASKS -> root Scenery\\World\\texture.\n"
                "\n"
                "  --mode M          ground | mask | both (default both)\n"
                "  --out DIR         output directory (default: ./m2_textures)\n"
                "  --size N          square tile size in px (default: 256)\n"
                "  --region C        single region letter, both families (default: b)\n"
                "\n"
                "GROUND family  {set:03d}{region}2{season}{v}.bmp  (v=1..N, 1-based):\n"
                "  --set N           ground set number (default 900)\n"
                "  --season S        su | sp | fa | wi | hw (default su)\n"
                "  --variants N      variant count, 1..16 (legend size) (default 7)\n"
                "                    bg color = Holger legend index (v-1); 0-based-color vs\n"
                "                    1-based-filename indexing is TO BE CONFIRMED in-sim.\n"
                "  --color-by K      ground bg color source: variant (default; ties on-ground\n"
                "                    color to the tilepattern cell for the geo-indexing test)\n"
                "                    or set (each land-class SET one distinct color, so classes\n"
                "                    are separable at a blend boundary — the M2 blend tests)\n"
                "\n"
                "MASK family  (v=1..K, one 8-sub-panel atlas each). Name suffix order differs by\n"
                "family: 900-series {set}{region}2m{v}1.bmp ; set-specific {set}{region}2m1{v}.bmp:\n"
                "  --mask-set N      mask set number (default 900; >=900 => generic 900-series\n"
                "                    naming m{v}1, else set-specific naming m1{v})\n"
                "  --mask-variants K variant count (default 7)\n"
                "                    each BMP is an 8-panel vertical atlas; panels map to the\n"
                "                    cell's 2x2 corner config (1,2 diagonal; 3,4 edge; 5-8\n"
                "                    single corner), decoded from the real 900b2m21 atlas.\n",
        argv0);
}

} // namespace

int main(int argc, char** argv)
{
    std::string mode = "both";
    std::filesystem::path out_dir = "m2_textures";
    int size = 256;
    char region = 'b';
    int set = 900;
    std::string season = "su";
    int variants = 7;
    int mask_set = 900;
    int mask_variants = 7;
    bool color_by_set = false; // ground bg color: false=by variant (geo test), true=by set (blend tests)

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        std::string value;
        // Consume the next token as this option's value, failing fast on a
        // missing value or a following option instead of silently parsing "".
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
        if (arg == "--mode")
        {
            if (!need("--mode"))
            {
                return 1;
            }
            mode = value;
        }
        else if (arg == "--out")
        {
            if (!need("--out"))
            {
                return 1;
            }
            out_dir = value;
        }
        else if (arg == "--size")
        {
            if (!need("--size"))
            {
                return 1;
            }
            size = std::atoi(value.c_str());
        }
        else if (arg == "--region")
        {
            if (!need("--region"))
            {
                return 1;
            }
            region = value.empty() ? '\0' : value[0];
        }
        else if (arg == "--set")
        {
            if (!need("--set"))
            {
                return 1;
            }
            set = std::atoi(value.c_str());
        }
        else if (arg == "--season")
        {
            if (!need("--season"))
            {
                return 1;
            }
            season = value;
        }
        else if (arg == "--variants")
        {
            if (!need("--variants"))
            {
                return 1;
            }
            variants = std::atoi(value.c_str());
        }
        else if (arg == "--mask-set")
        {
            if (!need("--mask-set"))
            {
                return 1;
            }
            mask_set = std::atoi(value.c_str());
        }
        else if (arg == "--mask-variants")
        {
            if (!need("--mask-variants"))
            {
                return 1;
            }
            mask_variants = std::atoi(value.c_str());
        }
        else if (arg == "--color-by")
        {
            if (!need("--color-by"))
            {
                return 1;
            }
            if (value == "set")
            {
                color_by_set = true;
            }
            else if (value == "variant")
            {
                color_by_set = false;
            }
            else
            {
                std::fprintf(stderr, "--color-by must be variant | set (got '%s').\n", value.c_str());
                return 1;
            }
        }
        else if (arg == "-h" || arg == "--help")
        {
            PrintUsage(argv[0]);
            return 0;
        }
        else
        {
            std::fprintf(stderr, "Unknown argument: %s\n", arg.c_str());
            PrintUsage(argv[0]);
            return 1;
        }
    }

    const bool do_ground = (mode == "ground" || mode == "both");
    const bool do_mask = (mode == "mask" || mode == "both");
    if (!do_ground && !do_mask)
    {
        std::fprintf(stderr, "--mode must be ground | mask | both (got '%s').\n", mode.c_str());
        return 1;
    }
    if (size < 16)
    {
        std::fprintf(stderr, "--size must be >= 16.\n");
        return 1;
    }
    if (region < 'a' || region > 'z')
    {
        std::fprintf(stderr, "--region must be a single lowercase letter a..z.\n");
        return 1;
    }
    if (do_ground)
    {
        if (variants < 1 || variants > 16)
        {
            std::fprintf(stderr, "--variants must be 1..16 (the 16-color legend size).\n");
            return 1;
        }
        if (season != "su" && season != "sp" && season != "fa" && season != "wi" && season != "hw")
        {
            std::fprintf(stderr, "--season must be one of su sp fa wi hw (got '%s').\n", season.c_str());
            return 1;
        }
    }
    if (do_mask && mask_variants < 1)
    {
        std::fprintf(stderr, "--mask-variants must be >= 1.\n");
        return 1;
    }

    std::error_code ec;
    std::filesystem::create_directories(out_dir, ec);
    if (ec)
    {
        std::fprintf(
            stderr, "Could not create output directory %s: %s\n", out_dir.string().c_str(), ec.message().c_str());
        return 1;
    }

    int ground_written = 0;
    int mask_written = 0;

    if (do_ground)
    {
        for (int v = 1; v <= variants; ++v)
        {
            if (!WriteGroundTexture(out_dir, set, region, season, v, size, color_by_set))
            {
                std::fprintf(stderr, "FAIL: could not write ground texture set=%d v=%d\n", set, v);
                return 1;
            }
            ++ground_written;
        }
        std::printf("m2_texgen: wrote %d ground tile(s) %s%c2%s{1..%d}.bmp -> scenery package texture/\n",
            ground_written, SetPrefix(set).c_str(), region, season.c_str(), variants);
    }

    if (do_mask)
    {
        for (int v = 1; v <= mask_variants; ++v)
        {
            if (!WriteMaskAtlas(out_dir, mask_set, region, v, size))
            {
                std::fprintf(stderr, "FAIL: could not write mask atlas set=%d v=%d\n", mask_set, v);
                return 1;
            }
            ++mask_written;
        }
        const char* pat = (mask_set >= 900) ? "m{1..K}1" : "m1{1..K}";
        std::printf(
            "m2_texgen: wrote %d M-tile atlas(es) %s%c2%s.bmp (K=%d, 8 sub-panels) -> Scenery\\World\\texture\n",
            mask_written, SetPrefix(mask_set).c_str(), region, pat, mask_variants);
    }

    std::printf("m2_texgen: %d file(s) in %s  (convert GROUND to FS70 via imagetool -terrain before install)\n",
        ground_written + mask_written, out_dir.string().c_str());
    return 0;
}
