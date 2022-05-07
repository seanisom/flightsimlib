#include "VectorTileBuilder.h"

#include "VectorTile.h"

#include <memory>

namespace flightsimlib::io
{
	
}

void flightsimlib::cgl::VectorTileBuilder::AddRoad(ERoadType type, RoadFeature& road,
	const std::vector<VectorVertex>& vertices)
{
	road.Start = static_cast<uint32_t>(m_vertices.size());
	road.End = static_cast<uint32_t>(vertices.size());
	
	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_roads[type].emplace_back(road);
}

void flightsimlib::cgl::VectorTileBuilder::AddLand(ELandType type, LandFeature& land,
	const std::vector<VectorVertex>& vertices)
{
	land.Start = static_cast<uint16_t>(m_vertices.size());
	land.End = static_cast<uint16_t>(vertices.size());

	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_land[type].emplace_back(land);
}

void flightsimlib::cgl::VectorTileBuilder::AddWater(EWaterType type, std::vector<std::vector<VectorVertex>>& vertices,
	const std::vector<float>& heights)
{
	m_water_features[static_cast<int>(vertices.size())] = type;
	m_water_polygons.emplace_back();
	auto& water = *m_water_polygons.end();
	
	auto count = 0;
	for (const auto& verts : vertices)
	{
		water.push_back(WaterPolygon
			{
				heights[count++],
				static_cast<uint16_t>(m_vertices.size()),
				static_cast<uint16_t>(verts.size())
			}
		);
		
		m_vertices.insert(verts.cbegin(), verts.cbegin(), verts.cend());
	}
}

void flightsimlib::cgl::VectorTileBuilder::AddRiver(RiverFeature river,
	const std::vector<VectorVertex>& vertices)
{
	river.Start = static_cast<uint16_t>(m_vertices.size());
	river.End = static_cast<uint16_t>(vertices.size());

	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_rivers.push_back(river);
}

void flightsimlib::cgl::VectorTileBuilder::AddPoints(EPointType type, const std::vector<VectorVertex>& vertices)
{
	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_points[type].emplace_back(
		VectorVertex
		{
			static_cast<uint16_t>(m_vertices.size()),
			static_cast<uint16_t>(vertices.size())
		}
	);
}

void flightsimlib::cgl::VectorTileBuilder::AddRail(ERailType type, RailFeature& rail,
	const std::vector<VectorVertex>& vertices)
{
	rail.Start = static_cast<uint16_t>(m_vertices.size());
	rail.End = static_cast<uint16_t>(vertices.size());

	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_rails[type].emplace_back(rail);
}

void flightsimlib::cgl::VectorTileBuilder::AddPower(EPowerType type, PowerFeature& power,
	const std::vector<VectorVertex>& vertices)
{
	power.Start = static_cast<uint16_t>(m_vertices.size());
	power.End = static_cast<uint16_t>(vertices.size());

	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_power[type].emplace_back(power);
}

void flightsimlib::cgl::VectorTileBuilder::AddUnknown1(EUnknown1Type type, Unknown1Feature& unknown1,
	const std::vector<VectorVertex>& vertices)
{
	unknown1.Start = static_cast<uint16_t>(m_vertices.size());
	unknown1.End = static_cast<uint16_t>(vertices.size());

	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_unknown1[type].emplace_back(unknown1);
}

void flightsimlib::cgl::VectorTileBuilder::AddUnknown2(EUnknown2Type type, Unknown2Feature& unknown2,
	const std::vector<VectorVertex>& vertices)
{
	unknown2.Start = static_cast<uint16_t>(m_vertices.size());
	unknown2.End = static_cast<uint16_t>(vertices.size());

	m_vertices.insert(vertices.cbegin(), vertices.cbegin(), vertices.cend());

	m_unknown2[type].emplace_back(unknown2);
}

flightsimlib::cgl::IVectorTile* flightsimlib::cgl::VectorTileBuilder::Build()
{
	auto tile = std::make_unique<VectorTile>();

	// Roads
	
	for (auto i = 0; i < IVectorTile::RoadTypeCount; ++i)
	{
		const auto road = m_roads.find(static_cast<ERoadType>(i));
		if (road == m_roads.end())
		{
			continue;
		}
		
		for (auto& feature : road->second)
		{
			const auto index = static_cast<uint16_t>(tile->RoadVertices.size());
			
			tile->RoadVertices.insert(m_vertices.cbegin() + feature.Start, 
				m_vertices.cbegin() + feature.Start, 
				m_vertices.cbegin() + feature.Start + feature.End);

			feature.Start = index;
			
			tile->RoadFeatures.emplace_back(feature);
		}

		tile->RoadTypes[i + 1i64] = reinterpret_cast<RoadFeature*>(road->second.size());
	}

	for (auto i = 0; i < static_cast<int>(tile->RoadTypes.size()); ++i)
	{
		tile->RoadTypes[i] = tile->RoadFeatures.data() + reinterpret_cast<uintptr_t>(tile->RoadTypes[i]);
	}

	// Land

	for (auto i = 0; i < IVectorTile::LandTypeCount; ++i)
	{
		const auto land = m_land.find(static_cast<ELandType>(i));
		if (land == m_land.end())
		{
			continue;
		}

		for (auto& feature : land->second)
		{
			const auto index = static_cast<uint16_t>(tile->LandVertices.size());

			tile->LandVertices.insert(m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start + feature.End);

			feature.Start = index;

			tile->LandFeatures.emplace_back(feature);
		}

		tile->LandTypes[i + 1i64] = reinterpret_cast<LandFeature*>(land->second.size());
	}

	for (auto i = 0; i < static_cast<int>(tile->LandTypes.size()); ++i)
	{
		tile->LandTypes[i] = tile->LandFeatures.data() + reinterpret_cast<uintptr_t>(tile->LandTypes[i]);
	}

	// Water
	
	for (auto i = 0; i < static_cast<int>(m_water_polygons.size()); ++i)
	{
		const auto& type = m_water_features[i];
		auto& polygons = m_water_polygons[i];

		tile->WaterFeatures.emplace_back(
			WaterFeature
			{
				static_cast<uint16_t>(type),
				static_cast<uint16_t>(polygons.size())
			}
		);
		
		for (auto& polygon : polygons)
		{
			const auto index = static_cast<uint16_t>(tile->WaterVertices.size());
			
			tile->WaterVertices.insert(m_vertices.cbegin() + polygon.Start,
				m_vertices.cbegin() + polygon.Start,
				m_vertices.cbegin() + polygon.Start + polygon.End);

			polygon.Start = index;
			
			tile->WaterPolygons.push_back(polygon);
		}
	}

	// Rivers

	for (auto& river : m_rivers)
	{
		const auto index = static_cast<uint16_t>(tile->RiverVertices.size());

		tile->RiverVertices.insert(m_vertices.cbegin() + river.Start,
			m_vertices.cbegin() + river.Start,
			m_vertices.cbegin() + river.Start + river.End);
		
		river.Start = index;

		tile->RiverFeatures.push_back(river);
	}

	// Points

	for (auto i = 0; i < IVectorTile::PointTypeCount; ++i)
	{
		const auto point = m_points.find(static_cast<EPointType>(i));
		if (point == m_points.end())
		{
			continue;
		}

		auto point_vertex_count = uint16_t{};
		for (const auto& feature : point->second)
		{
			tile->PointVertices.insert(m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start + feature.End);

			point_vertex_count += feature.End;
		}

		tile->PointTypes[i + 1i64] = reinterpret_cast<VectorVertex*>(point_vertex_count);
	}

	for (auto i = 0; i < static_cast<int>(tile->PointTypes.size()); ++i)
	{
		tile->PointTypes[i] = tile->PointVertices.data() + reinterpret_cast<uintptr_t>(tile->PointTypes[i]);
	}

	// Rails

	for (auto i = 0; i < IVectorTile::RailTypeCount; ++i)
	{
		const auto rail = m_rails.find(static_cast<ERailType>(i));
		if (rail == m_rails.end())
		{
			continue;
		}

		for (auto& feature : rail->second)
		{
			const auto index = static_cast<uint16_t>(tile->RailVertices.size());

			tile->RailVertices.insert(m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start + feature.End);

			feature.Start = index;

			tile->RailFeatures.emplace_back(feature);
		}

		tile->RailTypes[i + 1i64] = reinterpret_cast<RailFeature*>(rail->second.size());
	}

	for (auto i = 0; i < static_cast<int>(tile->RailTypes.size()); ++i)
	{
		tile->RailTypes[i] = tile->RailFeatures.data() + reinterpret_cast<uintptr_t>(tile->RailTypes[i]);
	}

	// Power

	for (auto i = 0; i < IVectorTile::PowerTypeCount; ++i)
	{
		const auto power = m_power.find(static_cast<EPowerType>(i));
		if (power == m_power.end())
		{
			continue;
		}

		for (auto& feature : power->second)
		{
			const auto index = static_cast<uint16_t>(tile->PowerVertices.size());

			tile->PowerVertices.insert(m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start,
				m_vertices.cbegin() + feature.Start + feature.End);

			feature.Start = index;

			tile->PowerFeatures.emplace_back(feature);
		}

		tile->PowerTypes[i + 1i64] = reinterpret_cast<PowerFeature*>(power->second.size());
	}

	for (auto i = 0; i < static_cast<int>(tile->PowerTypes.size()); ++i)
	{
		tile->PowerTypes[i] = tile->PowerFeatures.data() + reinterpret_cast<uintptr_t>(tile->PowerTypes[i]);
	}

	// Skip Unknown1, Unknown2 for now!

	// Cleanup

	m_roads.clear();
	m_land.clear();
	m_water_features.clear();
	m_water_polygons.clear();
	m_rivers.clear();
	m_points.clear();
	m_rails.clear();
	m_power.clear();
	m_unknown1.clear();
	m_unknown2.clear();
	
	return tile.release();
}
