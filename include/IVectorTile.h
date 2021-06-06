#pragma once

#include <cstdint>
#include <string>

#include "IObject.h"


namespace flightsimlib::geo
{
	struct LatLon;
}

namespace flightsimlib::io
{
	class BinaryMemoryStream;
}


namespace flightsimlib::cgl
{


#pragma pack(push)
#pragma pack(1)

struct Vert2U16
{
	uint16_t Start;
	uint16_t End;
};

#pragma pack(pop)

typedef Vert2U16 VectorVertex;
typedef Vert2U16 LandFeature;
typedef Vert2U16 WaterFeature;
typedef Vert2U16 Unknown1Feature;
typedef Vert2U16 Unknown2Feature;


#pragma pack(push)
#pragma pack(1)

struct PowerFeature
{
	uint32_t Id;
	uint16_t Start;
	uint16_t End;
};

#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)

struct RailFeature
{
	uint32_t Id;
	uint8_t  Width;
	uint16_t Start;
	uint16_t End;
	uint8_t  Class;
	uint8_t  Crossing;
	uint8_t  Level;
};

#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)

struct RiverFeature
{
	uint8_t  Width;
	uint16_t Start;
	uint16_t End;
};

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)

struct WaterPolygon
{
	float    Height;
	uint16_t Start;
	uint16_t End;
};

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)

struct BoundingBox
{
	float    TLX;
	float    TLY;
	float    BRX;
	float    BRY;
};

#pragma pack(pop)



#pragma pack(push)
#pragma pack(1)

struct RoadFeature
{
	uint32_t Id;
	uint8_t  Width;
	uint32_t Start;
	uint32_t End;
	uint8_t  Flags;
	uint8_t  Lanes;
	uint8_t  Level;
};

#pragma pack(pop)


class IVectorTile : public IObject
{
public:
	virtual auto FromBinary(uint8_t* data, int length, const std::string& quad_key, int version) -> void = 0;
	virtual auto VertexToLatLon(geo::LatLon& ll, const VectorVertex& vertex) const -> void = 0;
	[[nodiscard]] virtual auto GetRoadVertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetRoadFeatureAt(int type, int index) const -> const RoadFeature* = 0;
	[[nodiscard]] virtual auto GetRoadFeatureCount(int type) const -> int = 0;
	[[nodiscard]] virtual auto GetLandVertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetLandFeatureAt(int type, int index) const -> const LandFeature* = 0;
	[[nodiscard]] virtual auto GetLandFeatureCount(int type) const -> int = 0;
	[[nodiscard]] virtual auto GetWaterVertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetWaterPolygonAt(int feature, int index) const -> const WaterPolygon* = 0;
	[[nodiscard]] virtual auto GetWaterFeatureAt(int index) const -> const WaterFeature* = 0;
	[[nodiscard]] virtual auto GetWaterFeatureCount() const -> int = 0;
	[[nodiscard]] virtual auto GetRiverVertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetRiverFeatureAt(int index) const -> const RiverFeature* = 0;
	[[nodiscard]] virtual auto GetRiverFeatureCount() const -> int = 0;
	[[nodiscard]] virtual auto GetPointVertexAt(int type, int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetPointVertexCount(int type) const -> int = 0;
	[[nodiscard]] virtual auto GetRailVertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetRailFeatureAt(int type, int index) const -> const RailFeature* = 0;
	[[nodiscard]] virtual auto GetRailFeatureCount(int type) const -> int = 0;
	[[nodiscard]] virtual auto GetPowerVertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetPowerFeatureAt(int type, int index) const -> const PowerFeature* = 0;
	[[nodiscard]] virtual auto GetPowerFeatureCount(int type) const -> int = 0;
	[[nodiscard]] virtual auto GetUnknown1VertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetUnknown1FeatureAt(int type, int index) const -> const Unknown1Feature* = 0;
	[[nodiscard]] virtual auto GetUnknown1FeatureCount(int type) const -> int = 0;
	[[nodiscard]] virtual auto GetUnknown2VertexAt(int index) const -> const VectorVertex* = 0;
	[[nodiscard]] virtual auto GetUnknown2FeatureAt(int type, int index) const -> const Unknown1Feature* = 0;
	[[nodiscard]] virtual auto GetUnknown2FeatureCount(int type) const -> int = 0;

	static auto constexpr RoadTypeCount = 31;
	static auto constexpr LandTypeCount = 28;
	static auto constexpr PointTypeCount = 20;
	static auto constexpr RailTypeCount = 9;
	static auto constexpr PowerTypeCount = 2;
	static auto constexpr Unknown1TypeCount = 10;
	static auto constexpr Unknown2TypeCount = 5;
};


}