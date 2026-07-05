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
// TerrainVectorDb (CVX, layer 0x65) round-trip example / test.
//
// This is the M5 (a) verification for the landclass-synthesis arc (see
// newdawn's M5_PLAN.md). It:
//
//   1. Authors a CBglTerrainVectorDb record with known shapes: a water
//      polygon (type + slope + texture attributes, single water-height
//      float) and a stream (shared slope attribute, one per-point-altitude
//      polyline and one bare polyline).
//   2. Hand-writes a minimal valid .bgl containing that record under a 0x65
//      DirectQmid layer with a packed 16-byte tile pointer — the exact
//      shape real cvx files use.
//   3. Reads the file back through CBglFile::Read + AsTerrainVectorDb and
//      asserts every decoded field matches what was written, that the
//      DBRecord header fields carry the validated semantics (NAttrOffsets =
//      total per-shape references, NPoints = total points, NPointsAlt =
//      AltType-1 points, shared attribute blocks deduplicated in the blob),
//      and that the static dequantization helpers reproduce the known
//      bounds of the record's QMID.
//
// Returns 0 on success, 1 on any mismatch / I/O failure.
//

#include "BglData.h"
#include "BglFile.h"
#include "BglTypes.h"
#include "BinaryStream.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using flightsimlib::io::BinaryFileStream;
using flightsimlib::io::CBglFile;
using flightsimlib::io::CBglTerrainVectorDb;
using flightsimlib::io::CPackedQmid;
using flightsimlib::io::EBglLayerClass;
using flightsimlib::io::EBglLayerType;
using flightsimlib::io::IBglTerrainVectorDb;
using flightsimlib::io::SBglHeader;
using flightsimlib::io::SBglLayerPointer;
using flightsimlib::io::SBglVectorAttribute;
using flightsimlib::io::SBglVectorPolyline;
using flightsimlib::io::SBglVectorShape;

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

// A real level-11 QMID (an offshore-Ecuador tile from the file the M5
// survey probed): level 11, u 394, v 525 -> lon [-87.656250, -87.421875],
// lat [-2.460938, -2.285156] on the 3 * 2^(L-2) x 2^(L-1) grid.
constexpr uint32_t kQmidLow = 0x008940E6;
constexpr uint32_t kQmidHigh = 0;

auto MakeAttribute(const GUID& guid, std::vector<uint8_t> payload) -> SBglVectorAttribute
{
    SBglVectorAttribute attribute;
    attribute.Guid = guid;
    attribute.Payload = std::move(payload);
    return attribute;
}

// Author a deterministic record: distinctive, asymmetric coordinates so a
// transposed axis or a bitstream slip cannot round-trip by accident.
void BuildExpected(CBglTerrainVectorDb& record)
{
    record.SetPackedQmid(kQmidLow);
    record.SetAddToCells(0);
    record.ClearShapes();

    std::vector<uint8_t> slope_payload(8, 0);
    slope_payload[0] = 0x3F; // arbitrary non-zero float bytes
    std::vector<uint8_t> texture_payload(16);
    for (size_t i = 0; i < texture_payload.size(); ++i)
    {
        texture_payload[i] = static_cast<uint8_t>(0xA0 + i);
    }

    // Shape 0: a water polygon — type GUID + slope + texture attributes,
    // one closed 5-point ring, single water-height float.
    SBglVectorShape water;
    water.DrawMethod = 3;
    water.Attributes.push_back(MakeAttribute(IBglTerrainVectorDb::TypeWaterPolygons, {}));
    water.Attributes.push_back(MakeAttribute(IBglTerrainVectorDb::AttrSlope, slope_payload));
    water.Attributes.push_back(MakeAttribute(IBglTerrainVectorDb::AttrTexture, texture_payload));
    SBglVectorPolyline ring;
    ring.AltType = 2;
    ring.QuantizedPoints = {100, 200, 30000, 220, 29500, 31000, 90, 30500, 100, 200};
    ring.Altitudes = {2064.5F};
    water.Polylines.push_back(ring);
    record.AddShape(water);

    // Shape 1: a stream — type GUID + the SAME slope attribute bytes as
    // shape 0 (the writer must share one blob block), one polyline with
    // per-point altitudes and one without.
    SBglVectorShape stream;
    stream.DrawMethod = 2;
    stream.Attributes.push_back(MakeAttribute(IBglTerrainVectorDb::TypeStreams, {}));
    stream.Attributes.push_back(MakeAttribute(IBglTerrainVectorDb::AttrSlope, slope_payload));
    SBglVectorPolyline creek;
    creek.AltType = 1;
    creek.QuantizedPoints = {0, 16384, 5000, 17000, 12000, 15500, 32768, 14000};
    creek.Altitudes = {2100.0F, 2098.5F, 2097.25F, 2095.0F};
    stream.Polylines.push_back(creek);
    SBglVectorPolyline spur;
    spur.AltType = 0;
    spur.QuantizedPoints = {12000, 15500, 12500, 9000, 13000, 0};
    stream.Polylines.push_back(spur);
    record.AddShape(stream);
}

// Write a single 16-byte packed direct-QMID tile pointer.
void WriteTilePointer(BinaryFileStream& out, uint32_t record_offset, uint32_t record_size)
{
    out << kQmidLow << kQmidHigh << record_offset << record_size;
}

bool WriteBgl(const std::filesystem::path& path, CBglTerrainVectorDb& record)
{
    BinaryFileStream out(path, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc);
    if (!out)
    {
        return false;
    }

    constexpr int kNumLayers = 1;
    const int tile_ptr = kHeaderSize + kLayerPointerSize * kNumLayers;
    const int record_offset = tile_ptr + kTilePointerSize;
    const int record_size = record.CalculateSize();

    SBglHeader header{};
    header.Version = 0x0201;
    header.FileMagic = 0x1992;
    header.HeaderSize = kHeaderSize;
    header.FileTime = 0;
    header.QmidMagic = 0x08051803;
    header.LayerCount = kNumLayers;
    SBglHeader::WriteBinary(out, header);

    SBglLayerPointer layer{};
    layer.Type = EBglLayerType::TerrainVectorDb;
    layer.DataClass = static_cast<uint16_t>(EBglLayerClass::DirectQmid);
    layer.HasQmidHigh = 1;
    layer.TileCount = 1;
    layer.StreamOffset = static_cast<uint32_t>(tile_ptr);
    layer.SizeBytes = kTilePointerSize;
    layer.WriteBinary(out);

    WriteTilePointer(out, static_cast<uint32_t>(record_offset), static_cast<uint32_t>(record_size));
    record.WriteBinary(out);

    return static_cast<bool>(out);
}

void VerifyRawHeader(const std::filesystem::path& path)
{
    // The DBRecord header fields carry the semantics validated against real
    // FSX data; check them straight off the disk image so a model-level
    // round-trip slip cannot hide a header regression.
    BinaryFileStream in(path);
    Check(static_cast<bool>(in), "reopen for raw header check");
    const int record_offset = kHeaderSize + kLayerPointerSize + kTilePointerSize;
    in.SetPosition(record_offset);
    int32_t version = 0;
    uint32_t qmid = 0;
    int32_t add_to_cells = 0;
    int32_t num_entities = 0;
    int32_t attr_size = 0;
    int32_t num_attr_offsets = 0;
    int32_t num_points = 0;
    int32_t num_points_alt = 0;
    in >> version >> qmid >> add_to_cells >> num_entities >> attr_size >> num_attr_offsets >> num_points >>
        num_points_alt;
    Check(version == 6, "raw header: version 6");
    Check(qmid == kQmidLow, "raw header: packed QMID");
    Check(add_to_cells == 0, "raw header: AddToCells");
    Check(num_entities == 2, "raw header: NEntities");
    // Blob blocks: water type (20) + slope (20 + 8) + texture (20 + 16) +
    // stream type (20) = 104; the second slope reference shares the block.
    Check(attr_size == 104, "raw header: AttrSize (slope block shared)");
    // Total per-shape references: 3 (water) + 2 (stream).
    Check(num_attr_offsets == 5, "raw header: NAttrOffsets = total references");
    Check(num_points == 5 + 4 + 3, "raw header: NPoints = total points");
    Check(num_points_alt == 4, "raw header: NPointsAlt = AltType-1 points");
}

void VerifyModel(const IBglTerrainVectorDb& read, const CBglTerrainVectorDb& expected)
{
    Check(read.GetVersion() == expected.GetVersion(), "version");
    Check(read.GetPackedQmid() == expected.GetPackedQmid(), "packed QMID");
    Check(read.GetAddToCells() == expected.GetAddToCells(), "AddToCells");
    Check(read.GetShapeCount() == expected.GetShapeCount(), "shape count");
    Check(
        read.GetShapeAt(-1) == nullptr && read.GetShapeAt(read.GetShapeCount()) == nullptr, "GetShapeAt out of range");

    for (int s = 0; s < expected.GetShapeCount(); ++s)
    {
        const auto* a = expected.GetShapeAt(s);
        const auto* b = read.GetShapeAt(s);
        Check(a != nullptr && b != nullptr, "shape present");
        if (a == nullptr || b == nullptr)
        {
            continue;
        }
        Check(a->DrawMethod == b->DrawMethod, "shape DrawMethod");
        Check(a->Attributes.size() == b->Attributes.size(), "shape attribute count");
        for (size_t i = 0; i < a->Attributes.size() && i < b->Attributes.size(); ++i)
        {
            Check(std::memcmp(&a->Attributes[i].Guid, &b->Attributes[i].Guid, sizeof(GUID)) == 0, "attribute GUID");
            Check(a->Attributes[i].Payload == b->Attributes[i].Payload, "attribute payload");
        }
        Check(a->Polylines.size() == b->Polylines.size(), "polyline count");
        for (size_t p = 0; p < a->Polylines.size() && p < b->Polylines.size(); ++p)
        {
            Check(a->Polylines[p].AltType == b->Polylines[p].AltType, "polyline AltType");
            Check(b->Polylines[p].MethodType == 2, "polyline MethodType (writer emits 2)");
            Check(a->Polylines[p].QuantizedPoints == b->Polylines[p].QuantizedPoints, "polyline points");
            Check(a->Polylines[p].Altitudes == b->Polylines[p].Altitudes, "polyline altitudes");
        }
    }
}

void VerifyDequantization()
{
    const auto rect = IBglTerrainVectorDb::QmidRectFromPacked(kQmidLow);
    Check(rect.has_value(), "QmidRectFromPacked resolves");
    if (!rect)
    {
        return;
    }
    auto near = [](double a, double b) { return std::fabs(a - b) < 1e-9; };
    Check(near(rect->LonWest, -87.656250), "rect LonWest");
    Check(near(rect->LonEast, -87.421875), "rect LonEast");
    Check(near(rect->LatSouth, -2.460937500), "rect LatSouth");
    Check(near(rect->LatNorth, -2.285156250), "rect LatNorth");

    const auto south_west = IBglTerrainVectorDb::DequantizePoint(0, 0, *rect);
    Check(near(south_west.Lon, rect->LonWest) && near(south_west.Lat, rect->LatSouth), "dequantize (0,0)");
    const auto north_east =
        IBglTerrainVectorDb::DequantizePoint(IBglTerrainVectorDb::QuantMax, IBglTerrainVectorDb::QuantMax, *rect);
    Check(near(north_east.Lon, rect->LonEast) && near(north_east.Lat, rect->LatNorth), "dequantize (max,max)");

    Check(!IBglTerrainVectorDb::QmidRectFromPacked(0).has_value(), "QmidRectFromPacked(0) rejects");
}

void Verify(const std::filesystem::path& path, const CBglTerrainVectorDb& expected)
{
    CBglFile bgl(path.wstring());
    Check(bgl.Read(), "CBglFile::Read");

    auto* layer = bgl.GetDirectQmidLayer(EBglLayerType::TerrainVectorDb);
    Check(layer != nullptr, "GetDirectQmidLayer(TerrainVectorDb)");
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

    auto* read = data->AsTerrainVectorDb();
    Check(read != nullptr, "AsTerrainVectorDb");
    if (read == nullptr)
    {
        return;
    }

    VerifyModel(*read, expected);
}

} // namespace

int main()
{
    const auto path = std::filesystem::temp_directory_path() / "vectordb_roundtrip.bgl";

    CBglTerrainVectorDb expected;
    BuildExpected(expected);

    if (!WriteBgl(path, expected))
    {
        std::fprintf(stderr, "FAIL: could not write %s\n", path.string().c_str());
        return 1;
    }

    VerifyRawHeader(path);
    Verify(path, expected);
    VerifyDequantization();

    std::error_code ec;
    std::filesystem::remove(path, ec);

    if (g_failures != 0)
    {
        std::fprintf(stderr, "vectordb_roundtrip: %d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("vectordb_roundtrip: PASS\n");
    return 0;
}
