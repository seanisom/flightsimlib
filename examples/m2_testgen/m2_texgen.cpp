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
// Emits a family of "numbered / colored" placeholder land-class texture tiles
// where the identity (land-class id, region, variant index) is BAKED INTO THE
// PIXELS as readable text plus a deterministic per-identity background color.
// The point is M2: load these into FSX over a known area (paired with the
// m2_testgen .bgl), fly, screenshot, and read off *which* texture appears
// *where* and *how the variants tile / dither at class boundaries* — i.e. the
// GAP B blend / `BlendTextureVariant` rule that unblocks M3.
//
// Output is uncompressed 24-bit BMP (no external image dependency); convert to
// the sim's terrain-texture format (e.g. DDS) with the sim's imagetool as a
// protocol step (see LANDCLASS_SYNTHESIS.md M2 protocol — the exact terrain
// texture directory + filename convention is an FSX-install detail filled in
// there, not baked in here). Filenames are descriptive:
//   lc{class}_r{region}_v{variant}.bmp
//
// Returns 0 on success, 1 on any I/O failure.
//

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

// Deterministic, well-spread background color from the (class, region, variant)
// identity so adjacent variants are visually distinct in-sim (this is what
// makes the variant tiling / dither pattern legible).
Rgb IdentityColor(int land_class, int region, int variant)
{
    uint32_t h = 2166136261u; // FNV-1a over the identity triple
    for (int v : {land_class, region, variant})
    {
        h ^= static_cast<uint32_t>(v);
        h *= 16777619u;
    }
    // Bias toward mid-tones so baked text stays legible at either contrast.
    auto chan = [](uint32_t x) -> uint8_t { return static_cast<uint8_t>(64 + (x % 160)); };
    return {chan(h), chan(h >> 8), chan(h >> 16)};
}

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

// One placeholder texture for a (class, region, variant) identity.
bool WriteTexture(const std::filesystem::path& dir, int land_class, int region, int variant, int size)
{
    const Rgb bg = IdentityColor(land_class, region, variant);
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

    // Baked identity, large and centered-ish: "C<class>" / "R<region>" /
    // "V<variant>" stacked so it's readable from altitude.
    const int scale = size >= 256 ? 6 : (size >= 128 ? 3 : 2);
    const int line_h = (7 + 2) * scale;
    int y = size / 2 - line_h * 3 / 2;
    const int x = size / 2 - 4 * (5 + 1) * scale / 2;
    DrawText(img, "C" + std::to_string(land_class), x, y, scale, ink);
    y += line_h;
    DrawText(img, "R" + std::to_string(region), x, y, scale, ink);
    y += line_h;
    DrawText(img, "V" + std::to_string(variant), x, y, scale, ink);

    const std::string name =
        "lc" + std::to_string(land_class) + "_r" + std::to_string(region) + "_v" + std::to_string(variant) + ".bmp";
    return WriteBmp24(dir / name, img);
}

void PrintUsage(const char* argv0)
{
    std::printf("Usage: %s [--out DIR] [--size N] [--classes A,B,...] [--regions R,...] [--variants K]\n"
                "\n"
                "Generates numbered/colored placeholder land-class textures for the M2\n"
                "reverse-engineering experiment (LANDCLASS_SYNTHESIS.md M2). Each tile bakes\n"
                "its (land-class, region, variant) identity into the pixels.\n"
                "\n"
                "  --out DIR        output directory (default: ./m2_textures)\n"
                "  --size N         square tile size in px (default: 256)\n"
                "  --classes LIST   comma-separated land-class ids (default: 1,2)\n"
                "  --regions LIST   comma-separated region ids (default: 0)\n"
                "  --variants K     variant count per (class,region), 0..K-1 (default: 4)\n",
        argv0);
}

std::vector<int> ParseIntList(const std::string& s)
{
    std::vector<int> out;
    size_t start = 0;
    while (start <= s.size())
    {
        size_t comma = s.find(',', start);
        std::string tok = s.substr(start, comma == std::string::npos ? std::string::npos : comma - start);
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

} // namespace

int main(int argc, char** argv)
{
    std::filesystem::path out_dir = "m2_textures";
    int size = 256;
    std::vector<int> classes = {1, 2};
    std::vector<int> regions = {0};
    int variants = 4;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        auto next = [&]() -> std::string { return (i + 1 < argc) ? argv[++i] : std::string(); };
        if (arg == "--out")
        {
            out_dir = next();
        }
        else if (arg == "--size")
        {
            size = std::atoi(next().c_str());
        }
        else if (arg == "--classes")
        {
            classes = ParseIntList(next());
        }
        else if (arg == "--regions")
        {
            regions = ParseIntList(next());
        }
        else if (arg == "--variants")
        {
            variants = std::atoi(next().c_str());
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

    if (size < 16 || classes.empty() || regions.empty() || variants < 1)
    {
        std::fprintf(stderr, "Invalid parameters.\n");
        return 1;
    }

    std::error_code ec;
    std::filesystem::create_directories(out_dir, ec);

    int written = 0;
    for (int land_class : classes)
    {
        for (int region : regions)
        {
            for (int variant = 0; variant < variants; ++variant)
            {
                if (!WriteTexture(out_dir, land_class, region, variant, size))
                {
                    std::fprintf(stderr, "FAIL: could not write texture lc%d_r%d_v%d\n", land_class, region, variant);
                    return 1;
                }
                ++written;
            }
        }
    }

    std::printf("m2_texgen: wrote %d texture(s) to %s\n", written, out_dir.string().c_str());
    return 0;
}
