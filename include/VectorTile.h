#pragma once

#include "IVectorTile.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>


namespace flightsimlib::io
{
	class BinaryMemoryStream;
}


namespace flightsimlib::cgl
{

// deprecated - for compatibility but may be removed in the future.
static void Q16ToFloat(float* dest, const uint16_t* quantized);
static void Q16ToDouble(double* dest, const uint16_t* quantized);

static bool IsValidHeight(float val);

class VectorTileBuilder;

class VectorTile final : public IVectorTile
{
	friend class VectorTileBuilder;
	
	struct VectorBounds
	{
		double delta_lat;
		double delta_lon;
		float tile_width;
		float tile_height;
		double tl_lat;
		double tl_lon;
		float overlap_factor;
	};

	struct TileQuad
	{
		int32_t level;
		uint32_t tile_x;
		uint32_t tile_y;
	};
	
public:
	auto FromBinary(uint8_t* data, int length, const std::string& quad_key, int version) -> void override;
	auto ToBinary(io::IBinaryStream& stream) -> void override;
	auto VertexToLatLon(geo::LatLon& ll, const VectorVertex& vertex) const -> void override;
	void CalcWaterBoundingBox();

private:
	void Parse(io::BinaryMemoryStream& reader, float* elevation);
	void VerticesToBoundingBox(BoundingBox& dest, VectorVertex& min, VectorVertex& max) const;
	void CalcBounds();
	void ReadRails(io::BinaryMemoryStream& reader);
	void FixRoads();
	void ReadRoads(io::BinaryMemoryStream& reader);

public:
	[[nodiscard]] auto GetRoadVertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetRoadFeatureAt(int type, int index) const -> const RoadFeature* override;
	[[nodiscard]] auto GetRoadFeatureCount(int type) const -> int override;
	[[nodiscard]] auto GetLandVertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetLandFeatureAt(int type, int index) const -> const LandFeature* override;
	[[nodiscard]] auto GetLandFeatureCount(int type) const -> int override;
	[[nodiscard]] auto GetWaterVertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetWaterPolygonAt(int feature, int index) const -> const WaterPolygon* override;
	[[nodiscard]] auto GetWaterFeatureAt(int index) const -> const WaterFeature* override;
	[[nodiscard]] auto GetWaterFeatureCount() const -> int override;
	[[nodiscard]] auto GetRiverVertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetRiverFeatureAt(int index) const -> const RiverFeature* override;
	[[nodiscard]] auto GetRiverFeatureCount() const -> int override;
	[[nodiscard]] auto GetPointVertexAt(int type, int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetPointVertexCount(int type) const -> int override;
	[[nodiscard]] auto GetRailVertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetRailFeatureAt(int type, int index) const -> const RailFeature* override;
	[[nodiscard]] auto GetRailFeatureCount(int type) const -> int override;
	[[nodiscard]] auto GetPowerVertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetPowerFeatureAt(int type, int index) const -> const PowerFeature* override;
	[[nodiscard]] auto GetPowerFeatureCount(int type) const -> int override;
	[[nodiscard]] auto GetUnknown1VertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetUnknown1FeatureAt(int type, int index) const -> const Unknown1Feature* override;
	[[nodiscard]] auto GetUnknown1FeatureCount(int type) const -> int override;
	[[nodiscard]] auto GetUnknown2VertexAt(int index) const -> const VectorVertex* override;
	[[nodiscard]] auto GetUnknown2FeatureAt(int type, int index) const -> const Unknown2Feature* override;
	[[nodiscard]] auto GetUnknown2FeatureCount(int type) const -> int override;

private:
	std::vector<RoadFeature> RoadFeatures;
	std::vector<LandFeature> LandFeatures;
	std::vector<BoundingBox>  WaterBounds;
	std::vector<RailFeature> RailFeatures;
	std::vector<PowerFeature> PowerFeatures;
	std::vector<Unknown1Feature> Unknown1Features;
	std::vector<Unknown2Feature> Unknown2Features;
	std::vector<VectorVertex> RoadVertices;
	std::array<RoadFeature*, 32> RoadTypes{};
	std::vector<VectorVertex> LandVertices;
	std::array<LandFeature*, 29> LandTypes{};
	std::vector<VectorVertex> WaterVertices;
	std::vector<WaterPolygon> WaterPolygons;
	std::vector<WaterFeature> WaterFeatures;
	std::vector<VectorVertex> RiverVertices;
	std::vector<RiverFeature> RiverFeatures;
	std::vector<VectorVertex> PointVertices;
	std::array<VectorVertex*, 21> PointTypes{};
	std::vector<VectorVertex> RailVertices;
	std::array<RailFeature*, 10> RailTypes{};
	std::vector<VectorVertex> PowerVertices;
	std::array<PowerFeature*, 3> PowerTypes{};
	std::vector<VectorVertex> Unknown1Vertices;
	std::array<Unknown1Feature*, 11> Unknown1Types{};
	std::vector<VectorVertex> Unknown2Vertices;
	std::array<Unknown2Feature*, 6> Unknown2Types{};
	
	VectorBounds m_bounds{};
	TileQuad m_quad{};
	int m_version = 0;
};

}