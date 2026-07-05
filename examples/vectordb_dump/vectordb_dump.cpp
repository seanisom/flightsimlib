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
// TerrainVectorDb (CVX, layer 0x65) dump tool.
//
// Ground-truth validation aid for the M5 vector arc: decodes a cvx BGL
// through CBglFile + CBglTerrainVectorDb and prints its records so the
// output can be eyeballed against TMFViewer / known geography. No FSX data
// ships with this tool; point it at a local install.
//
//   vectordb_dump <file.bgl>              summary of every record
//   vectordb_dump <file.bgl> <qmid-hex>   full dump of one record
//                                         (e.g. vectordb_dump cvx2432.bgl 0x008940E6)
//

#include "BglData.h"
#include "BglFile.h"
#include "BglTypes.h"

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>

using flightsimlib::io::CBglFile;
using flightsimlib::io::CPackedQmid;
using flightsimlib::io::EBglLayerType;
using flightsimlib::io::IBglTerrainVectorDb;
using flightsimlib::io::SBglVectorShape;

namespace
{

struct SNamedGuid
{
    const GUID* Guid;
    const char* Name;
};

const SNamedGuid kKnownGuids[] = {
    {&IBglTerrainVectorDb::TypeAirportBounds, "AirportBounds"},
    {&IBglTerrainVectorDb::TypeParks, "Parks"},
    {&IBglTerrainVectorDb::TypeWaterPolygonsGps, "WaterPolygonsGPS"},
    {&IBglTerrainVectorDb::TypeWaterPolygons, "WaterPolygons"},
    {&IBglTerrainVectorDb::TypeExclusions, "Exclusions"},
    {&IBglTerrainVectorDb::TypeShorelines, "Shorelines"},
    {&IBglTerrainVectorDb::TypeStreams, "Streams"},
    {&IBglTerrainVectorDb::TypeUtilities, "Utilities"},
    {&IBglTerrainVectorDb::TypeRailways, "Railways"},
    {&IBglTerrainVectorDb::TypeRoads, "Roads"},
    {&IBglTerrainVectorDb::TypeFreewayTrafficRoads, "FreewayTraffic/TrafficAttr"},
    {&IBglTerrainVectorDb::TypeWaterPolygonsSlope, "WaterPolygonsSlope/SlopeAttr"},
    {&IBglTerrainVectorDb::AttrTexture, "TextureAttr"},
};

auto GuidName(const GUID& guid) -> const char*
{
    for (const auto& known : kKnownGuids)
    {
        if (std::memcmp(known.Guid, &guid, sizeof(GUID)) == 0)
        {
            return known.Name;
        }
    }
    return "unknown";
}

void PrintGuid(const GUID& guid)
{
    std::printf("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", static_cast<unsigned>(guid.Data1), guid.Data2,
        guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
}

void DumpRecord(const IBglTerrainVectorDb& record)
{
    const auto rect = IBglTerrainVectorDb::QmidRectFromPacked(record.GetPackedQmid());
    std::printf("record qmid 0x%08X version %d AddToCells %d shapes %d\n", record.GetPackedQmid(), record.GetVersion(),
        record.GetAddToCells(), record.GetShapeCount());
    if (rect)
    {
        std::printf("  bounds lon [%.6f, %.6f] lat [%.6f, %.6f]\n", rect->LonWest, rect->LonEast, rect->LatSouth,
            rect->LatNorth);
    }
    for (int s = 0; s < record.GetShapeCount(); ++s)
    {
        const SBglVectorShape* shape = record.GetShapeAt(s);
        std::printf("  shape[%d] drawMethod %d attributes %zu polylines %zu\n", s, shape->DrawMethod,
            shape->Attributes.size(), shape->Polylines.size());
        for (const auto& attribute : shape->Attributes)
        {
            std::printf("    attr ");
            PrintGuid(attribute.Guid);
            std::printf(" (%s) payload %zu bytes", GuidName(attribute.Guid), attribute.Payload.size());
            if (!attribute.Payload.empty())
            {
                std::printf(":");
                for (const auto byte : attribute.Payload)
                {
                    std::printf(" %02X", byte);
                }
            }
            std::printf("\n");
        }
        for (size_t p = 0; p < shape->Polylines.size(); ++p)
        {
            const auto& polyline = shape->Polylines[p];
            const auto point_count = polyline.QuantizedPoints.size() / 2;
            std::printf("    polyline[%zu] %zu pts altType %d method %d\n", p, point_count, polyline.AltType,
                polyline.MethodType);
            for (size_t i = 0; i < point_count; ++i)
            {
                const auto x = polyline.QuantizedPoints[2 * i];
                const auto y = polyline.QuantizedPoints[2 * i + 1];
                std::printf("      (%6d, %6d)", x, y);
                if (rect)
                {
                    const auto lat_lon = IBglTerrainVectorDb::DequantizePoint(x, y, *rect);
                    std::printf("  ->  (%.6f, %.6f)", lat_lon.Lat, lat_lon.Lon);
                }
                std::printf("\n");
            }
            if (!polyline.Altitudes.empty())
            {
                std::printf("      altitudes:");
                for (const auto altitude : polyline.Altitudes)
                {
                    std::printf(" %.2f", altitude);
                }
                std::printf("\n");
            }
        }
    }
}

void SummarizeRecord(const IBglTerrainVectorDb& record)
{
    auto points = 0;
    auto out_of_range = 0;
    auto method1 = 0;
    for (int s = 0; s < record.GetShapeCount(); ++s)
    {
        const SBglVectorShape* shape = record.GetShapeAt(s);
        for (const auto& polyline : shape->Polylines)
        {
            points += static_cast<int>(polyline.QuantizedPoints.size() / 2);
            if (polyline.MethodType != 2)
            {
                ++method1;
            }
            for (const auto value : polyline.QuantizedPoints)
            {
                if (value < 0 || value > IBglTerrainVectorDb::QuantMax)
                {
                    ++out_of_range;
                }
            }
        }
    }
    std::printf("qmid 0x%08X shapes %2d points %5d m1-polylines %d out-of-range %d\n", record.GetPackedQmid(),
        record.GetShapeCount(), points, method1, out_of_range);
}

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::fprintf(stderr, "usage: vectordb_dump <file.bgl> [qmid-hex]\n");
        return 2;
    }

    const std::string path = argv[1];
    uint32_t filter_qmid = 0;
    auto have_filter = false;
    if (argc >= 3)
    {
        filter_qmid = static_cast<uint32_t>(std::strtoul(argv[2], nullptr, 0));
        have_filter = true;
    }

    CBglFile bgl(std::filesystem::path(path).wstring());
    if (!bgl.Read())
    {
        std::fprintf(stderr, "FAIL: could not read %s\n", path.c_str());
        return 1;
    }

    auto* layer = bgl.GetDirectQmidLayer(EBglLayerType::TerrainVectorDb);
    if (layer == nullptr)
    {
        std::fprintf(stderr, "FAIL: no TerrainVectorDb (0x65) layer in %s\n", path.c_str());
        return 1;
    }

    const auto count = layer->GetQmidCount();
    std::printf("%s: %d vector record(s)\n", path.c_str(), count);
    for (int i = 0; i < count; ++i)
    {
        const auto* pointer = layer->GetDataPointerAtIndex(i);
        if (pointer == nullptr)
        {
            continue;
        }
        if (have_filter && pointer->QmidLow != filter_qmid)
        {
            continue;
        }
        auto* data = layer->GetDataAtQmid(CPackedQmid{pointer->QmidLow, pointer->QmidHigh}, 0);
        if (data == nullptr)
        {
            continue;
        }
        auto* record = data->AsTerrainVectorDb();
        if (record == nullptr)
        {
            continue;
        }
        if (have_filter)
        {
            DumpRecord(*record);
        }
        else
        {
            SummarizeRecord(*record);
        }
    }
    return 0;
}
