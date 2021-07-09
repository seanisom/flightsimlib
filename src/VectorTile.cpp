
#include "VectorTile.h"

#include "BinaryStream.h"
#include "Geo.h"
#include "WebMercator.h"

#include <cmath>


// Compiler issue: see https://developercommunity.visualstudio.com/t/c28020-false-positives/923103
#pragma warning(disable:28020)

// Issue with ReSharper:
// ReSharper disable CppRedundantCastExpression

namespace flightsimlib::cgl
{

const auto m_pi = 3.141592653589793;

namespace detail
{
 
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N>
to_array_impl(T (&&a)[N], std::index_sequence<I...>)
{
	return { {std::move(a[I])...} };
}
 
}
 
template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N])
{
	return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
}

struct RoadFixup
{
	uint32_t Id;
	int32_t  L0;
	int32_t  L1;
	int32_t  L2;
};


static const auto road_fixups = to_array<RoadFixup>(
{
	{ 0x0102D8D6, -1, -1, -1 },
	{ 0x0102D8E3, -1, -1, -1 },
	{ 0x0103782D, -1, -1, -1 },
	{ 0x01037833, -1, -1, -1 },
	{ 0x01037834, -1, -1, -1 },
	{ 0x01037835, -1, -1, -1 },
	{ 0x01037844, -1, -1, -1 },
	{ 0x01037855, -1, -1, -1 },
	{ 0x0104718C, -1, -1, -1 },
	{ 0x010471AE, -1, -1, -1 },
	{ 0x01047CE1, -1, -1, -1 },
	{ 0x00817821,  8, 12, -1 },
	{ 0x0081781E,  8, 12, -1 },
	{ 0x01E830C0, 14, -1, -1 },
	{ 0x01E83101, 14, -1, -1 },
	{ 0x01E83136, 14, -1, -1 },
	{ 0x01E830B7, 14, -1, -1 },
	{ 0x01E7C672, 12, 14, -1 },
	{ 0x01E82FE8, 12, 14, -1 },
	{ 0x01E83151, 12, 14, -1 },
	{ 0x01E8315E, 12, 14, -1 },
	{ 0x01E83171, 12, 14, -1 },
	{ 0x01E82F33,  8, 12, -1 },
	{ 0x01E82EC4, 12, 14, -1 },
	{ 0x01E8315F, -1, -1, -1 },
	{ 0x01E83163, -1, -1, -1 },
	{ 0x01E82EE8, 14, -1, -1 },
	{ 0x01E83154, 12, 14, -1 },
	{ 0x01E83152, 12, 14, -1 },
	{ 0x01E82FA3, 12, 14, -1 },
	{ 0x01E83170, 12, 14, -1 },
	{ 0x04D097A7, -1, -1, -1 },
	{ 0x00F2488E, -1, -1, -1 },
	{ 0x01BCF210, -1, -1, -1 },
	{ 0x03F6FAE8, -1, -1, -1 },
	{ 0x055342B7, -1, -1, -1 },
	{ 0x06D680A9, -1, -1, -1 },
	{ 0x06357AAC, -1, -1, -1 },
	{ 0x0676006B, -1, -1, -1 },
	{ 0x067600F8, -1, -1, -1 },
	{ 0x067600FD, -1, -1, -1 },
	{ 0x06760395, -1, -1, -1 },
	{ 0x0676039F, -1, -1, -1 },
	{ 0x067603BD, -1, -1, -1 },
	{ 0x067603C0, -1, -1, -1 },
	{ 0x067603C2, -1, -1, -1 },
	{ 0x067603C5, -1, -1, -1 },
	{ 0x067603DB, -1, -1, -1 },
	{ 0x005CC197, -1, -1, -1 },
	{ 0x01D4862F, -1, -1, -1 },
	{ 0x01D47EFE, -1, -1, -1 },
	{ 0x0628E1E0, -1, -1, -1 },
	{ 0x0628E1AD, -1, -1, -1 },
	{ 0x027EF4EA, -1, -1, -1 },
	{ 0x027EF4EB, -1, -1, -1 },
	{ 0x0728AD25, -1, -1, -1 },
	{ 0x05AC979C, -1, -1, -1 },
	{ 0x04BDF011, -1, -1, -1 },
	{ 0x06EF8996, -1, -1, -1 },
	{ 0x06533E90, -1, -1, -1 },
	{ 0x06533923, -1, -1, -1 },
	{ 0x06533E89, -1, -1, -1 },
	{ 0x005EB09D, -1, -1, -1 },
	{ 0x0623E32B, -1, -1, -1 },
	{ 0x06F3C47C, -1, -1, -1 },
	{ 0x0391FA99, -1, -1, -1 },
	{ 0x04460CB0, -1, -1, -1 },
	{ 0x008A4916, -1, -1, -1 },
	{ 0x05216484, -1, -1, -1 },
	{ 0x05E3DFE6, -1, -1, -1 },
	{ 0x0623E32B, -1, -1, -1 },
	{ 0x07FFFFFF, -1, -1, -1 },
	{ 0x033F34AA, -1, -1, -1 },
	{ 0x029F7146, -1, -1, -1 },
	{ 0x04DAA4CD, -1, -1, -1 },
	{ 0x027B8B1D, -1, -1, -1 },
	{ 0x03825BAF, -1, -1, -1 },
	{ 0x0265A627, -1, -1, -1 },
	{ 0x0265A626, -1, -1, -1 },
	{ 0x0265A0DB, -1, -1, -1 },
	{ 0x0265A632, -1, -1, -1 },
	{ 0x0265A629, -1, -1, -1 },
	{ 0x0265A62C, -1, -1, -1 },
	{ 0x0265A62D, -1, -1, -1 },
	{ 0x0265A62E, -1, -1, -1 },
	{ 0x0265A62F, -1, -1, -1 },
	{ 0x0265A630, -1, -1, -1 },
	{ 0x0265A631, -1, -1, -1 },
	{ 0x0478B01E, -1, -1, -1 },
	{ 0x02D6B3B5, -1, -1, -1 },
	{ 0x02D6B3C1, -1, -1, -1 },
	{ 0x05797AD3, -1, -1, -1 },
	{ 0x0360131D, -1, -1, -1 },
	{ 0x03601310, -1, -1, -1 },
	{ 0x0360130C, -1, -1, -1 },
	{ 0x0360130E, -1, -1, -1 },
	{ 0x0360130B, -1, -1, -1 },
	{ 0x0455DB55, -1, -1, -1 },
	{ 0x0455DB85, -1, -1, -1 },
	{ 0x0455DB89, -1, -1, -1 },
	{ 0x0455DB88, -1, -1, -1 },
	{ 0x0664B2E2, -1, -1, -1 },
	{ 0x051BDA74, -1, -1, -1 },
	{ 0x05FDF782, -1, -1, -1 },
	{ 0x01E1E701, -1, -1, -1 },
	{ 0x01E1E6FD, -1, -1, -1 },
	{ 0x01E1E6FE, -1, -1, -1 },
	{ 0x014875AB, -1, -1, -1 },
	{ 0x06F91748, -1, -1, -1 },
	{ 0x02054E70, -1, -1, -1 },
	{ 0x02054E70, -1, -1, -1 }
});


double normalized_to_lat(double norm)
{
	return 90.0 - atan(exp(norm * 2 * m_pi - m_pi)) * (360.0 / m_pi);
}


double normalized_to_lon(double norm)
{
	return (norm - 0.5) * 360.0;
}


bool IsValidHeight(float val)
{
	return !std::isnan(val) && val > -750.0f && val < 10000.0f;
}


void Q16ToFloat(float* dest, const uint16_t* quantized)  // NOLINT(clang-diagnostic-unused-function)
{
	dest[0] = (static_cast<float>(quantized[0]) - 32767.5f) * 0.000030518044f;
	dest[1] = (static_cast<float>(quantized[1]) - 32767.5f) * 0.000030518044f;
}


void Q16ToDouble(double* dest, const uint16_t* quantized)  // NOLINT(clang-diagnostic-unused-function)
{
	dest[0] = (static_cast<double>(quantized[0]) - 32767.5) * 0.000030518044;
	dest[1] = (static_cast<double>(quantized[1]) - 32767.5) * 0.000030518044;
}


void VectorTile::VerticesToBoundingBox(BoundingBox& dest, VectorVertex& min, VectorVertex& max) const
{
	dest.TLX = static_cast<float>(min.Start) * m_bounds.tile_width * 0.000030518044f;
	dest.TLY = static_cast<float>(min.End) * m_bounds.tile_height * 0.000030518044f;
	dest.BRX = static_cast<float>(max.Start) * m_bounds.tile_width * 0.000030518044f;
	dest.BRY = static_cast<float>(max.End) * m_bounds.tile_height * 0.000030518044f;
}


void VectorTile::VertexToLatLon(geo::LatLon& ll, const VectorVertex& vertex) const
{
	// More efficient than sim algorithm using pre-multiply:
	// 0.00001525902189669642175936522469 = (65536/65535) * (1 >> 16)
	ll.Lon = m_bounds.tl_lon + m_bounds.delta_lon * static_cast<double>(vertex.Start) * 0.00001525902189669642175936522469;
	ll.Lat = m_bounds.tl_lat + m_bounds.delta_lat * static_cast<double>(vertex.End) * 0.00001525902189669642175936522469;
}


void VectorTile::CalcWaterBoundingBox()
{
	WaterBounds.resize(WaterPolygons.size());

	auto i = 0;
	for (const auto& poly : WaterPolygons)
	{
		VectorVertex max = { WaterVertices[poly.Start].Start, WaterVertices[poly.Start].End };
		VectorVertex min = { max.Start, max.End };

		for (auto j = poly.Start + 1; j < poly.End; ++j)
		{
			auto& [Start, End] = WaterVertices[j];
			
			if (Start < min.Start)
			{
				min.Start = Start;
			}
			if (Start > max.Start)
			{
				max.Start = Start;
			}
			if (End < min.End)
			{
				min.End = End;
			}
			if (End > max.End)
			{
				max.End = End;
			}	
		}
		
		VerticesToBoundingBox(WaterBounds[i++], min, max);
	}
}


void VectorTile::CalcBounds()
{
	m_bounds.overlap_factor = m_version > 14 ? 80.0f : 10.0f;

	auto level_cells = 0.0;
	if (m_quad.level < 0)
	{
		level_cells = pow(2.0, m_quad.level);
	}
	else if (m_quad.level > 0)
	{
		level_cells = static_cast<double>(1i64 << m_quad.level);
	}

	const auto norm_lon = (static_cast<double>(m_quad.tile_x) + 0.5) / level_cells;
	const auto norm_lat = (static_cast<double>(m_quad.tile_y) + 0.5) / level_cells;
	const auto pixel_ratio = 1.442700600680826e10 / static_cast<double>(256i64 << m_quad.level);
	const auto overlap_y = static_cast<double>(m_bounds.overlap_factor) * 0.000008983152841195214 + pixel_ratio * 
		0.00000002495320233665337 + 0.001122894105149402;
	const auto overlap_x = 0.00000002495320233665337 / cos(normalized_to_lat(norm_lat) * m_pi / 180.0) * 
		(static_cast<double>(m_bounds.overlap_factor) * 360.0 + pixel_ratio + 45000.0);
	
	m_bounds.tl_lat = normalized_to_lat(norm_lat + 0.5 / level_cells) - overlap_y;
	m_bounds.tl_lon = normalized_to_lon(norm_lon - 0.5 / level_cells) - overlap_x;
	m_bounds.delta_lat = normalized_to_lat(norm_lat - 0.5 / level_cells) + overlap_y - m_bounds.tl_lat;
	m_bounds.delta_lon = normalized_to_lon(norm_lon + 0.5 / level_cells) + overlap_x - m_bounds.tl_lon;
	
	m_bounds.tile_width = static_cast<float>((overlap_x + overlap_x) / (m_bounds.delta_lon - (overlap_x + overlap_x)) + 1.0);
	m_bounds.tile_height = static_cast<float>((overlap_y + overlap_y) / (m_bounds.delta_lat - (overlap_y + overlap_y)) + 1.0);
}


template<typename T, typename U, std::size_t S>
bool unpack_bitmask(io::BinaryMemoryStream& in, std::array<U*, S>& types, std::vector<U>& features)
{
	const T bit_mask = in.ReadType<T>();
	
	if (!bit_mask)
	{
		return false;
	}

	std::array<uint16_t, S> lengths{};
	
	lengths[0] = 0;
	auto type = bit_mask & 1u;
	
	for (auto i = 1u; i < lengths.size() - 1; ++i)
	{
		if ((1 << i) & bit_mask)
		{
			if (((1 << i) - 1) & bit_mask)
			{
				const auto offset = in.ReadType<uint16_t>();
				for (; type <= i; ++type)
				{
					lengths[type] = offset;
				}
			}
			else
			{
				for (; type <= i; ++type)
				{
					lengths[type] = 0;
				}
			}
		}
	}

	const auto  feature_count = in.ReadType<uint16_t>();
	
	for (auto i = type; i < lengths.size(); ++i)
	{
		lengths[i] = feature_count;
	}

	features.resize(feature_count);
	
	for (auto i = 0u; i < types.size(); ++i)
	{
		const auto index = lengths[i];
		types[i] = index == feature_count ? features.data() + features.size() : &features[index];
	}

	return true;
}


void VectorTile::ReadRoads(io::BinaryMemoryStream& reader)
{
	auto val = reader.ReadType<uint16_t>();
	for (auto& entry : RoadFeatures)
	{
		entry.Start = val;
		reader >> entry.Id;

		const auto flags = reader.ReadType<uint8_t>();
		if (m_version < 21)
		{
			entry.Flags |= ((flags >> 1) & 0xE0) | 0x80;
		}
		else
		{
			entry.Flags |= (flags >> 1) & 0x60; // bits 6-7
		}
		entry.Width = 2 * (flags & 0x3F);

		if (m_version < 21)
		{
			entry.Lanes = 0;
		}
		else
		{
			const auto lanes = reader.ReadType<uint8_t>();
			entry.Lanes = lanes & 0x1F;
			entry.Flags = (4 * lanes) ^ ((entry.Flags ^ (4 * lanes)) & 0x7F);
		}
		
		if ((entry.Flags & 0x60) == 0x40) // bits 6-7 == 2
		{
			reader >> entry.Level;
		}	
		else
		{
			entry.Level = -1;
		}
			
		entry.End = val = reader.ReadType<uint16_t>();
	}

	RoadVertices.resize(val);
	reader.Read(RoadVertices.data(), static_cast<int>(sizeof(VectorVertex) * RoadVertices.size()));
}

auto VectorTile::GetRoadVertexAt(int index) const -> const VectorVertex*
{
	return &RoadVertices[index];
}

auto VectorTile::GetRoadFeatureAt(int type, int index) const -> const RoadFeature*
{
	return RoadTypes[type] + index;
}

auto VectorTile::GetRoadFeatureCount(int type) const -> int
{
	return static_cast<int>(RoadTypes[type + 1i64] - RoadTypes[type]);
}

auto VectorTile::GetLandVertexAt(int index) const -> const VectorVertex*
{
	return &LandVertices[index];
}

auto VectorTile::GetLandFeatureAt(int type, int index) const -> const LandFeature*
{
	return LandTypes[type] + index;
}

auto VectorTile::GetLandFeatureCount(int type) const -> int
{
	return static_cast<int>(LandTypes[type + 1i64] - LandTypes[type]);
}

auto VectorTile::GetWaterVertexAt(int index) const -> const VectorVertex*
{
	return &WaterVertices[index];
}

auto VectorTile::GetWaterPolygonAt(int feature, int index) const -> const WaterPolygon*
{
	// TODO - This is how MSFS stores, but gives n*log(n) when globally iterating
	// We should change to just store indices, not size
	auto count = 0i64;
	for (auto i = 0; i < feature; ++i)
	{
		count += WaterFeatures[i].End;
	}
	return &WaterPolygons[count + index];
}

auto VectorTile::GetWaterFeatureAt(int index) const -> const WaterFeature*
{
	return &WaterFeatures[index];
}

auto VectorTile::GetWaterFeatureCount() const -> int
{
	return static_cast<int>(WaterFeatures.size());
}

auto VectorTile::GetRiverVertexAt(int index) const -> const VectorVertex*
{
	return &RiverVertices[index];
}

auto VectorTile::GetRiverFeatureAt(int index) const -> const RiverFeature*
{
	return &RiverFeatures[index];
}

auto VectorTile::GetRiverFeatureCount() const -> int
{
	return static_cast<int>(RiverFeatures.size());
}

auto VectorTile::GetPointVertexAt(int type, int index) const -> const VectorVertex*
{
	return PointTypes[type] + index;
}

auto VectorTile::GetPointVertexCount(int type) const -> int
{
	return static_cast<int>(PointTypes[type + 1i64] - PointTypes[type]);
}

auto VectorTile::GetRailVertexAt(int index) const -> const VectorVertex*
{
	return &RailVertices[index];
}

auto VectorTile::GetRailFeatureAt(int type, int index) const -> const RailFeature*
{
	return RailTypes[type] + index;
}

auto VectorTile::GetRailFeatureCount(int type) const -> int
{
	return static_cast<int>(RailTypes[type + 1i64] - RailTypes[type]);
}

auto VectorTile::GetPowerVertexAt(int index) const -> const VectorVertex*
{
	return &PowerVertices[index];
}

auto VectorTile::GetPowerFeatureAt(int type, int index) const -> const PowerFeature*
{
	return PowerTypes[type] + index;
}

auto VectorTile::GetPowerFeatureCount(int type) const -> int
{
	return static_cast<int>(PowerTypes[type + 1i64] - PowerTypes[type]);
}

auto VectorTile::GetUnknown1VertexAt(int index) const -> const VectorVertex*
{
	return &Unknown1Vertices[index];
}

auto VectorTile::GetUnknown1FeatureAt(int type, int index) const -> const Unknown1Feature*
{
	return Unknown1Types[type] + index;
}

auto VectorTile::GetUnknown1FeatureCount(int type) const -> int
{
	return static_cast<int>(Unknown1Types[type + 1i64] - Unknown1Types[type]);
}

auto VectorTile::GetUnknown2VertexAt(int index) const -> const VectorVertex*
{
	return &Unknown2Vertices[index];
}

auto VectorTile::GetUnknown2FeatureAt(int type, int index) const -> const Unknown2Feature*
{
	return Unknown2Types[type] + index;
}

auto VectorTile::GetUnknown2FeatureCount(int type) const -> int
{
	return static_cast<int>(Unknown2Types[type + 1i64] - Unknown2Types[type]);
}


void VectorTile::FixRoads()
{
	static float road_widths[16] = { 12.5f, 12.5f, 12.5f, 12.5f, 12.5f,
		10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 7.5f, 3.0f };
	
	for (auto i = 0u; i < RoadTypes.size() - 1; ++i)
	{
		for (auto road = RoadTypes[i]; road < RoadTypes[i + 1i64]; ++road)
		{
			road->Flags &= 0xE0u;
			road->Flags |= i & 0x1F;
			
			if (!road->Width)
			{
				if (!(road->Id >> 27))
				{
					road->Id &= 0x7FFFFFF;
					road->Id |= 0x8000000;
				}
				road->Width = static_cast<uint8_t>(static_cast<float>(road->Id >> 27) * road_widths[i >> 1]);
			}

			if (m_quad.level == 14 && m_quad.tile_x == 8157 &&
				(m_quad.tile_y == 4718 || m_quad.tile_y == 4719))
			{
				road->Flags &= 0xBF;
				road->Flags |= 0x20;
			}

			if ((road->Flags & 0x60) == 0x20)  // bits 6-7 == 1, culvert
			{
				continue;
			}
			
			for (const auto& fix : road_fixups)
			{
				if ((road->Id & 0x7FFFFFF) == fix.Id && 
					(fix.L0 < 0 || fix.L0 == m_quad.level || fix.L1 == m_quad.level || fix.L2 == m_quad.level))
				{
					road->Flags &= 0xBF;
					road->Flags |= 0x20;
					break;
				}
			}
			
		}
	}
}

void VectorTile::ReadRails(io::BinaryMemoryStream& reader)
{
	auto val = reader.ReadType<uint16_t>();
	for (auto& entry : RailFeatures)
	{
		entry.Start = val;
		reader >> entry.Id >> entry.Width;
		entry.Crossing = static_cast<uint8_t>(entry.Id >> 30);
		entry.Id &= 0x3FFFFF;

		if (entry.Crossing == 2)
		{
			reader >> entry.Level;
		}
		else
		{
			entry.Level = -1;
		}
		
		entry.End = val = reader.ReadType<uint16_t>();
	}

	RailVertices.resize(val);
	reader.Read(RailVertices.data(), static_cast<int>(sizeof(VectorVertex) * RailVertices.size()));
}


void VectorTile::Parse(io::BinaryMemoryStream& reader, float* elevation)
{
	CalcBounds();
	
	// roads
	
	if(unpack_bitmask<uint32_t>(reader, RoadTypes, RoadFeatures))
	{
		ReadRoads(reader);
		FixRoads();
	}

	// land use
	
	if (unpack_bitmask<uint32_t>(reader, LandTypes, LandFeatures))
	{
		auto val = reader.ReadType<uint16_t>();
		for (auto& [Start, End] : LandFeatures)
		{
			Start = val;
			End = val = reader.ReadType<uint16_t>();
		}
		
		LandVertices.resize(val);
		reader.Read(LandVertices.data(), static_cast<int>(sizeof(VectorVertex) * LandVertices.size()));
	}

	// water
	
	if (const auto water_count = reader.ReadType<uint16_t>(); water_count)
	{
		WaterPolygons.resize(water_count);

		auto val = reader.ReadType<uint16_t>();
		for (auto& [Height, Start, End] : WaterPolygons)
		{
			Start = val;
			if (m_version < 20)
			{
				reader >> Height;
			}
			else if (elevation && IsValidHeight(*elevation))
			{
				Height = *elevation++;
			}
			else
			{
				Height = -750.0f;
			}
			End = val = reader.ReadType<uint16_t>();
		}

		WaterVertices.resize(val);
		reader.Read(WaterVertices.data(), static_cast<int>(sizeof(VectorVertex) * WaterVertices.size()));

		WaterFeatures.resize(reader.ReadType<uint16_t>());
		for (auto& entry : WaterFeatures)
		{
			reader >> entry.End;

			switch (reader.ReadType<uint8_t>())
			{
			case 0:
				entry.Start = 5;
				break;
			case 1:
				entry.Start = 2;
				break;
			case 2:
				entry.Start = 3;
				break;
			case 3:
				entry.Start = 4;
				break;
			case 4:
				entry.Start = 0;
				break;
			case 5:
				entry.Start = 1;
				break;
			default:
				entry.Start = 7;
				break;
			}
		}
	}

	// rivers
	
	if (const auto river_count = reader.ReadType<uint16_t>(); river_count)
	{
		RiverFeatures.resize(river_count);

		auto val = reader.ReadType<uint16_t>();
		for(auto& [Width, Start, End] : RiverFeatures)
		{
			Start = val;
			Width = reader.ReadType<uint8_t>();
			if (Width >= 100)
			{
				Width = 20;
			}
			End = val = reader.ReadType<uint16_t>();
		}

		RiverVertices.resize(val);
		reader.Read(RiverVertices.data(), static_cast<int>(sizeof(VectorVertex) * RiverVertices.size()));
	}

	// points

	bool has_points;
	if (m_version > 20)
	{
		has_points = unpack_bitmask<uint32_t>(reader, PointTypes, PointVertices);
	}
	else
	{
		has_points = unpack_bitmask<uint16_t>(reader, PointTypes, PointVertices);
	}
	if (has_points)
	{
		reader.Read(PointVertices.data(), static_cast<int>(sizeof(VectorVertex) * PointVertices.size()));
	}

	// rails
	
	if(unpack_bitmask<uint16_t>(reader, RailTypes, RailFeatures))
	{
		ReadRails(reader);

		for (auto i = 0u; i < RailTypes.size() - 1; ++i)
		{
			for (auto entry = RailTypes[i]; entry < RailTypes[i + 1i64]; ++entry)
			{
				entry->Class = static_cast<uint8_t>(i);
			}			
		}
	}
	
	// Power Lines
	
	if (unpack_bitmask<uint8_t>(reader, PowerTypes, PowerFeatures))
	{
		auto val = reader.ReadType<uint16_t>();
		for (auto& [Id, Start, End] : PowerFeatures)
		{
			Start = val;
			Id = reader.ReadType<uint32_t>();
			End = val = reader.ReadType<uint16_t>();
		}
		
		PowerVertices.resize(val);
		reader.Read(PowerVertices.data(), static_cast<int>(sizeof(VectorVertex) * PowerVertices.size()));
	}

	// Unknown Line 1

	if (m_version > 20 && unpack_bitmask<uint16_t>(reader, Unknown1Types, Unknown1Features))
	{
		auto val = reader.ReadType<uint16_t>();
		for (auto& [Start, End] : Unknown1Features)
		{
			Start = val;
			End = val = reader.ReadType<uint16_t>();
		}

		Unknown1Vertices.resize(val);
		reader.Read(Unknown1Vertices.data(), static_cast<int>(sizeof(VectorVertex) * Unknown1Vertices.size()));
	}

	// Unknown Line 2

	if (m_version > 20 && unpack_bitmask<uint8_t>(reader, Unknown2Types, Unknown2Features))
	{
		auto val = reader.ReadType<uint16_t>();
		for (auto& [Start, End] : Unknown2Features)
		{
			Start = val;
			End = val = reader.ReadType<uint16_t>();
		}

		Unknown2Vertices.resize(val);
		reader.Read(Unknown2Vertices.data(), static_cast<int>(sizeof(VectorVertex) * Unknown2Vertices.size()));
	}
}

void VectorTile::FromBinary(uint8_t* data, int length, const std::string& quad_key, int version)
{
	m_version = version;
	
	if ((m_version == 19 && length == 5) || length == 1)
	{
		return;
	}

	geo::WebMercator::QuadKeyToTileXY(quad_key, m_quad.tile_x , m_quad.tile_y, m_quad.level);

	io::BinaryMemoryStream stream(data, length);
	
	Parse(stream, nullptr);
}

}
