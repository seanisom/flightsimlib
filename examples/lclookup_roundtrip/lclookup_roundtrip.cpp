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
// LCLookup (TerrainTextureLookup, layer 0x6F) round-trip example / test.
//
// This is the M1 verification for the landclass-synthesis arc (see
// LANDCLASS_SYNTHESIS.md §6/§7). It:
//
//   1. Authors a CBglTerrainTextureLookup record with known values.
//   2. Hand-writes a minimal valid .bgl containing that record under a 0x6F
//      DirectQmid layer, plus a small uncompressed TerrainLandClass (0x68)
//      raster, so the writer can author both pieces of M2/M3 test data.
//   3. Reads the file back through CBglFile::Read + AsTerrainTextureLookup and
//      asserts every decoded field matches what was written.
//
// All integer I/O is little-endian, matching flightsimlib's BinaryStream
// (which is itself little-endian-host only). Returns 0 on success, 1 on any
// mismatch / I/O failure.
//

#include "BglData.h"
#include "BglFile.h"
#include "BinaryStream.h"
#include "BglTypes.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

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

int g_failures = 0;

void Check(bool condition, const char* message)
{
    if (!condition)
    {
        std::fprintf(stderr, "FAIL: %s\n", message);
        ++g_failures;
    }
}

constexpr int kHeaderSize = 0x38;
constexpr int kLayerPointerSize = 20;
constexpr int kTilePointerSize = 16; // packed direct-QMID form (no RecordCount)
constexpr int kTrq1HeaderSize = 40;
constexpr uint32_t kTrq1Magic = 0x31515254; // 'TRQ1'

constexpr uint32_t kQmidLow = 0x26;
constexpr uint32_t kQmidHigh = 0;

// Author a deterministic LCLookup record with distinctive per-field values so
// the round-trip assertions catch any byte-order / offset slip.
void BuildExpected(CBglTerrainTextureLookup& lookup)
{
    constexpr int kNumRegions = 2;
    constexpr int kNumLandClasses = 3;
    constexpr int kNumWaterClasses = 2;

    lookup.SetHeaderMagic(0x4C434C4B); // arbitrary 'LCLK' sentinel
    lookup.ResizeTables(kNumRegions, kNumLandClasses, kNumWaterClasses);

    lookup.ClearTextures();
    for (int i = 0; i < 4; ++i)
    {
        SBglTextureLookupEntry entry{};
        entry.VULCNNumber = static_cast<int16_t>(100 + i);
        entry.VULCNRegion = static_cast<uint8_t>(i);
        entry.VULCNMask = static_cast<uint8_t>(i * 2);
        entry.SeasonMask = static_cast<int16_t>(0x0FFF);
        entry.DrawPriority = static_cast<int16_t>(10 - i);
        entry.BlendTextureVULCN = static_cast<int16_t>(200 + i);
        entry.BlendTextureRegion = static_cast<uint8_t>(i + 1);
        entry.BlendTextureMask = static_cast<uint8_t>(i + 3);
        entry.BlendTextureVariant = static_cast<int64_t>(0x1122334455667700LL + i);
        entry.AutogenVULCN = static_cast<int16_t>(300 + i);
        entry.AutogenRegion = static_cast<uint8_t>(i + 5);
        entry.AutogenMask = static_cast<uint8_t>(i + 7);
        lookup.AddTexture(entry);
    }

    for (int r = 0; r < kNumRegions; ++r)
    {
        for (int l = 0; l < kNumLandClasses; ++l)
        {
            lookup.SetRegionLandClassTexture(r, l, r * 10 + l);
        }
        for (int w = 0; w < kNumWaterClasses; ++w)
        {
            lookup.SetRegionWaterClassTexture(r, w, 50 + r * 10 + w);
        }
    }

    for (int l = 0; l < kNumLandClasses; ++l)
    {
        for (int b = 0; b < IBglTerrainTextureLookup::SlopeLookupCount; ++b)
        {
            lookup.SetSlopeLookup(l, b, static_cast<uint8_t>((l * 16 + b) & 0xFF));
        }
        for (int v = 0; v < IBglTerrainTextureLookup::VectorLookupCount; ++v)
        {
            lookup.SetVectorLookup(l, v, static_cast<uint8_t>((l * 5 + v + 1) & 0xFF));
        }
    }
}

// Write a single 16-byte packed direct-QMID tile pointer.
void WriteTilePointer(BinaryFileStream& out, uint32_t record_offset, uint32_t record_size)
{
    out << kQmidLow << kQmidHigh << record_offset << record_size;
}

bool WriteBgl(const std::filesystem::path& path, CBglTerrainTextureLookup& lookup)
{
    // Pre-create the file so BinaryFileStream's in|out open succeeds.
    {
        std::ofstream create(path, std::ios::binary | std::ios::trunc);
        if (!create)
        {
            return false;
        }
    }

    BinaryFileStream out(path);
    if (!out)
    {
        return false;
    }

    constexpr int kNumLayers = 2;
    const int after_layer_table = kHeaderSize + kLayerPointerSize * kNumLayers;

    const int tile_ptr_a = after_layer_table;
    const int record_a = tile_ptr_a + kTilePointerSize;
    const int record_a_size = lookup.CalculateSize();

    const int tile_ptr_b = record_a + record_a_size;
    const int record_b = tile_ptr_b + kTilePointerSize;
    constexpr int kClassCols = 4;
    constexpr int kClassRows = 4;
    constexpr int kClassPayload = kClassCols * kClassRows;
    const int record_b_size = kTrq1HeaderSize + kClassPayload;

    // --- Header ---
    SBglHeader header{};
    header.Version = 0x0201;
    header.FileMagic = 0x1992;
    header.HeaderSize = kHeaderSize;
    header.FileTime = 0;
    header.QmidMagic = 0x08051803;
    header.LayerCount = kNumLayers;
    header.PackedQMIDParent0 = 0;
    header.PackedQMIDParent1 = 0;
    header.PackedQMIDParent2 = 0;
    header.PackedQMIDParent3 = 0;
    header.PackedQMIDParent4 = 0;
    header.PackedQMIDParent5 = 0;
    header.PackedQMIDParent6 = 0;
    header.PackedQMIDParent7 = 0;
    SBglHeader::WriteBinary(out, header);

    // --- Layer pointer table ---
    SBglLayerPointer layer_a{};
    layer_a.Type = EBglLayerType::TerrainTextureLookup;
    layer_a.DataClass = static_cast<uint16_t>(EBglLayerClass::DirectQmid);
    layer_a.HasQmidHigh = 1;
    layer_a.TileCount = 1;
    layer_a.StreamOffset = static_cast<uint32_t>(tile_ptr_a);
    layer_a.SizeBytes = kTilePointerSize;
    layer_a.WriteBinary(out);

    SBglLayerPointer layer_b{};
    layer_b.Type = EBglLayerType::TerrainLandClass;
    layer_b.DataClass = static_cast<uint16_t>(EBglLayerClass::DirectQmid);
    layer_b.HasQmidHigh = 1;
    layer_b.TileCount = 1;
    layer_b.StreamOffset = static_cast<uint32_t>(tile_ptr_b);
    layer_b.SizeBytes = kTilePointerSize;
    layer_b.WriteBinary(out);

    // --- Layer A: LCLookup tile pointer + record ---
    WriteTilePointer(out, static_cast<uint32_t>(record_a), static_cast<uint32_t>(record_a_size));
    lookup.WriteBinary(out);

    // --- Layer B: TerrainLandClass tile pointer + uncompressed TRQ1 raster ---
    WriteTilePointer(out, static_cast<uint32_t>(record_b), static_cast<uint32_t>(record_b_size));
    // TRQ1 header (40 bytes): see SBglTerrainRasterQuad1Data / WriteBinary.
    out << kTrq1Magic;                                                    // Version
    out << static_cast<uint32_t>(record_b_size);                          // Size
    out << static_cast<uint16_t>(ERasterDataType::LandClass);             // DataType
    out << static_cast<uint8_t>(ERasterCompressionType::None);            // CompressionTypeData
    out << static_cast<uint8_t>(ERasterCompressionType::None);            // CompressionTypeMask
    out << kQmidLow << kQmidHigh;                                         // QmidLow / QmidHigh
    out << static_cast<uint32_t>(0);                                      // Variations
    out << static_cast<uint16_t>(kClassCols) << static_cast<uint16_t>(0); // Cols + padding
    out << static_cast<uint16_t>(kClassRows) << static_cast<uint16_t>(0); // Rows + padding
    out << static_cast<uint32_t>(kClassPayload);                          // SizeData
    out << static_cast<uint32_t>(0);                                      // SizeMask
    for (int i = 0; i < kClassPayload; ++i)
    {
        out << static_cast<uint8_t>(7); // constant land-class id
    }

    return static_cast<bool>(out);
}

void Verify(const std::filesystem::path& path, const CBglTerrainTextureLookup& expected)
{
    CBglFile bgl(path.wstring());
    Check(bgl.Read(), "CBglFile::Read");

    auto* layer = bgl.GetDirectQmidLayer(EBglLayerType::TerrainTextureLookup);
    Check(layer != nullptr, "GetDirectQmidLayer(TerrainTextureLookup)");
    if (layer == nullptr)
    {
        return;
    }

    const CPackedQmid qmid{kQmidLow, kQmidHigh};
    Check(layer->GetDataCountAtQmid(qmid) == 1, "record count at QMID == 1");

    auto* data = layer->GetDataAtQmid(qmid, 0);
    Check(data != nullptr, "GetDataAtQmid");
    if (data == nullptr)
    {
        return;
    }

    auto* read = data->AsTerrainTextureLookup();
    Check(read != nullptr, "AsTerrainTextureLookup");
    if (read == nullptr)
    {
        return;
    }

    Check(read->GetHeaderMagic() == expected.GetHeaderMagic(), "header magic");
    Check(read->GetTextureCount() == expected.GetTextureCount(), "texture count");
    Check(read->GetRegionCount() == expected.GetRegionCount(), "region count");
    Check(read->GetLandClassCount() == expected.GetLandClassCount(), "land-class count");
    Check(read->GetWaterClassCount() == expected.GetWaterClassCount(), "water-class count");

    for (int i = 0; i < expected.GetTextureCount(); ++i)
    {
        const auto* a = expected.GetTextureAt(i);
        const auto* b = read->GetTextureAt(i);
        Check(a != nullptr && b != nullptr, "texture entry present");
        if (a == nullptr || b == nullptr)
        {
            continue;
        }
        Check(a->VULCNNumber == b->VULCNNumber, "texture VULCNNumber");
        Check(a->VULCNRegion == b->VULCNRegion, "texture VULCNRegion");
        Check(a->VULCNMask == b->VULCNMask, "texture VULCNMask");
        Check(a->SeasonMask == b->SeasonMask, "texture SeasonMask");
        Check(a->DrawPriority == b->DrawPriority, "texture DrawPriority");
        Check(a->BlendTextureVULCN == b->BlendTextureVULCN, "texture BlendTextureVULCN");
        Check(a->BlendTextureRegion == b->BlendTextureRegion, "texture BlendTextureRegion");
        Check(a->BlendTextureMask == b->BlendTextureMask, "texture BlendTextureMask");
        Check(a->BlendTextureVariant == b->BlendTextureVariant, "texture BlendTextureVariant");
        Check(a->AutogenVULCN == b->AutogenVULCN, "texture AutogenVULCN");
        Check(a->AutogenRegion == b->AutogenRegion, "texture AutogenRegion");
        Check(a->AutogenMask == b->AutogenMask, "texture AutogenMask");
    }

    for (int r = 0; r < expected.GetRegionCount(); ++r)
    {
        for (int l = 0; l < expected.GetLandClassCount(); ++l)
        {
            Check(read->GetRegionLandClassTexture(r, l) == expected.GetRegionLandClassTexture(r, l),
                "region land-class texture index");
        }
        for (int w = 0; w < expected.GetWaterClassCount(); ++w)
        {
            Check(read->GetRegionWaterClassTexture(r, w) == expected.GetRegionWaterClassTexture(r, w),
                "region water-class texture index");
        }
    }

    for (int l = 0; l < expected.GetLandClassCount(); ++l)
    {
        for (int b = 0; b < IBglTerrainTextureLookup::SlopeLookupCount; ++b)
        {
            Check(read->GetSlopeLookup(l, b) == expected.GetSlopeLookup(l, b), "slope lookup");
        }
        for (int v = 0; v < IBglTerrainTextureLookup::VectorLookupCount; ++v)
        {
            Check(read->GetVectorLookup(l, v) == expected.GetVectorLookup(l, v), "vector lookup");
        }
    }
}

} // namespace

int main()
{
    const auto path = std::filesystem::temp_directory_path() / "lclookup_roundtrip.bgl";

    CBglTerrainTextureLookup expected;
    BuildExpected(expected);

    if (!WriteBgl(path, expected))
    {
        std::fprintf(stderr, "FAIL: could not write %s\n", path.string().c_str());
        return 1;
    }

    Verify(path, expected);

    std::error_code ec;
    std::filesystem::remove(path, ec);

    if (g_failures != 0)
    {
        std::fprintf(stderr, "lclookup_roundtrip: %d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("lclookup_roundtrip: PASS\n");
    return 0;
}
