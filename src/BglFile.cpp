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


//******************************************************************************
//
// File:     BglFile.cpp
//
// Summary:  BGL File structure parsing and utilities
//
// Author:   Sean Isom
//
// TODO list:
// * Factories need to be refactored to standalone
// * Iteration, copy, and move semantics for Layers and Tiles
//    - Should allow seamless reversible edits and moves across files
// * Layers and Tiles should conform to IBglSerializable
// * Observable model
// * History Stack
//
//******************************************************************************       


#include "BglFile.h"
// #include "BglData.h"


namespace flightsimlib
{

namespace io
{


//******************************************************************************
// CBglData
//******************************************************************************  

template <typename... Args>
std::unique_ptr<CBglData> FactoryImpl(EBglLayerType type, 
	IBglSceneryObject::ESceneryObjectType child_type, Args&&... args)
{
	std::unique_ptr<IBglSerializable> data = nullptr;

	constexpr auto no_copy = sizeof...(Args) != 0;
	
	switch (type)  // NOLINT(clang-diagnostic-switch-enum)
	{
	case EBglLayerType::Airport:
		data = no_copy ? std::make_unique<CBglAirport>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglAirport>(*dynamic_cast<const CBglAirport*>(args)...);
		break;
	case EBglLayerType::Nav:
		data = no_copy ? std::make_unique<CBglNav>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglNav>(*dynamic_cast<const CBglNav*>(args)...);
		break;
	case EBglLayerType::Ndb:
		data = no_copy ? std::make_unique<CBglNdb>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglNdb>(*dynamic_cast<const CBglNdb*>(args)...);
		break;
	case EBglLayerType::Marker:
		data = no_copy ? std::make_unique<CBglMarker>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglMarker>(*dynamic_cast<const CBglMarker*>(args)...);
		break;
	case EBglLayerType::Boundary:
		data = no_copy ? std::make_unique<CBglBoundary>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglBoundary>(*dynamic_cast<const CBglBoundary*>(args)...);
		break;
	case EBglLayerType::Waypoint:
		data = no_copy ? std::make_unique<CBglWaypoint>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglWaypoint>(*dynamic_cast<const CBglWaypoint*>(args)...);
		break;
	case EBglLayerType::Geopol:
		data = no_copy ? std::make_unique<CBglGeopol>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglGeopol>(*dynamic_cast<const CBglGeopol*>(args)...);
		break;
	case EBglLayerType::SceneryObject:
		{
			switch (child_type)
			{
			case IBglSceneryObject::ESceneryObjectType::GenericBuilding:
				data = no_copy ? std::make_unique<CBglGenericBuilding>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglGenericBuilding>(*dynamic_cast<const CBglGenericBuilding*>(args)...);
				break;
			case IBglSceneryObject::ESceneryObjectType::LibraryObject:
				data = no_copy ? std::make_unique<CBglLibraryObject>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglLibraryObject>(*dynamic_cast<const CBglLibraryObject*>(args)...);
				break;
			case IBglSceneryObject::ESceneryObjectType::Windsock:
				data = no_copy ? std::make_unique<CBglWindsock>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglWindsock>(*dynamic_cast<const CBglWindsock*>(args)...);
				break;
			case IBglSceneryObject::ESceneryObjectType::Effect:
				data = no_copy ? std::make_unique<CBglEffect>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglEffect>(*dynamic_cast<const CBglEffect*>(args)...);
				break;
			case IBglSceneryObject::ESceneryObjectType::TaxiwaySigns:
				data = no_copy ? std::make_unique<CBglTaxiwaySigns>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglTaxiwaySigns>(*dynamic_cast<const CBglTaxiwaySigns*>(args)...);
				break;
			case IBglSceneryObject::ESceneryObjectType::Trigger:
				data = no_copy ? std::make_unique<CBglTrigger>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglTrigger>(*dynamic_cast<const CBglTrigger*>(args)...);
				break;
			case IBglSceneryObject::ESceneryObjectType::Beacon:
				data = no_copy ? std::make_unique<CBglBeacon>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglBeacon>(*dynamic_cast<const CBglBeacon*>(args)...);
				break;
			case IBglSceneryObject::ESceneryObjectType::ExtrusionBridge:
				data = no_copy ? std::make_unique<CBglExtrusionBridge>() :  // NOLINT(bugprone-branch-clone)
					std::make_unique<CBglExtrusionBridge>(*dynamic_cast<const CBglExtrusionBridge*>(args)...);
				break;
			default:
				break;
			}
		}
		break;
		
	case EBglLayerType::ModelData:
		data = no_copy ? std::make_unique<CBglModelData>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglModelData>(*dynamic_cast<const CBglModelData*>(args)...);
		break;
	case EBglLayerType::AirportSummary:
		data = no_copy ? std::make_unique<CBglAirportSummary>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglAirportSummary>(*dynamic_cast<const CBglAirportSummary*>(args)...);
		break;
	case EBglLayerType::TerrainVectorDb:
		break;
	case EBglLayerType::TerrainElevation:
		break;
	case EBglLayerType::TerrainLandClass:
		break;
	case EBglLayerType::TerrainWaterClass:
		break;
	case EBglLayerType::TerrainRegion:
		break;
	case EBglLayerType::PopulationDensity:
		break;
	case EBglLayerType::AutogenAnnotation:
		break;
	case EBglLayerType::TerrainIndex:
		break;
	case EBglLayerType::TerrainTextureLookup:
		break;
	case EBglLayerType::Tacan:
		data = no_copy ? std::make_unique<CBglTacan>() :  // NOLINT(bugprone-branch-clone)
			std::make_unique<CBglTacan>(*dynamic_cast<const CBglTacan*>(args)...);
		break;
	default:
		return nullptr;
		/*
		auto AsTerrainSeason()->IBglTerrainSeason* override;
		auto AsTerrainPhoto()->IBglTerrainPhoto* override;
		auto AsTerrainPhoto32()->IBglTerrainPhoto32* override;
		*/
	}

	return std::make_unique<CBglData>(type, std::move(data));
}

	
CBglData::CBglData(EBglLayerType type, std::unique_ptr<IBglSerializable> data) :
	m_type(type), m_data(std::move(data))
{
}

std::unique_ptr<CBglData> CBglData::Factory(EBglLayerType type, IBglSceneryObject::ESceneryObjectType child_type)
{
	return FactoryImpl(type, child_type);
}

std::unique_ptr<CBglData> CBglData::Clone() const
{
	auto child_type = IBglSceneryObject::ESceneryObjectType::Unknown;
	if (GetType() == EBglLayerType::SceneryObject)
	{
		const auto* scenery = dynamic_cast<const IBglSceneryObject*>(this);
		child_type = scenery->GetSceneryObjectType();
	}
	return FactoryImpl(GetType(), child_type, m_data.get());
}

auto CBglData::GetType() const -> EBglLayerType
{
	return m_type;
}

auto CBglData::SetType(EBglLayerType value) -> void
{
	m_type = value;
}

auto CBglData::AsAirport() -> IBglAirport*
{
	if (m_type == EBglLayerType::Airport)
	{
		return dynamic_cast<IBglAirport*>(this);
	}
	return nullptr;
}

auto CBglData::AsNav() -> IBglNav*
{
	if (m_type == EBglLayerType::Nav)
	{
		return dynamic_cast<IBglNav*>(this);
	}
	return nullptr;
}

auto CBglData::AsNdb() -> IBglNdb*
{
	if (m_type == EBglLayerType::Ndb)
	{
		return dynamic_cast<IBglNdb*>(this);
	}
	return nullptr;
}

auto CBglData::AsMarker() -> IBglMarker*
{
	if (m_type == EBglLayerType::Marker)
	{
		return dynamic_cast<IBglMarker*>(this);
	}
	return nullptr;
}

auto CBglData::AsBoundary() -> IBglBoundary*
{
	if (m_type == EBglLayerType::Boundary)
	{
		return dynamic_cast<IBglBoundary*>(this);
	}
	return nullptr;
}

auto CBglData::AsWaypoint() -> IBglWaypoint*
{
	if (m_type == EBglLayerType::Waypoint)
	{
		return dynamic_cast<IBglWaypoint*>(this);
	}
	return nullptr;
}

auto CBglData::AsGeopol() -> IBglGeopol*
{
	if (m_type == EBglLayerType::Geopol)
	{
		return dynamic_cast<IBglGeopol*>(this);
	}
	return nullptr;
}

auto CBglData::AsSceneryObject() -> IBglSceneryObject*
{
	if (m_type == EBglLayerType::SceneryObject)
	{
		return dynamic_cast<IBglSceneryObject*>(this);
	}
	return nullptr;
}

auto CBglData::AsModelData() -> IBglModelData*
{
	if (m_type == EBglLayerType::ModelData)
	{
		return dynamic_cast<IBglModelData*>(this);
	}
	return nullptr;
}

auto CBglData::AsAirportSummary() -> IBglAirportSummary*
{
	if (m_type == EBglLayerType::AirportSummary)
	{
		return dynamic_cast<IBglAirportSummary*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainVectorDb() -> IBglTerrainVectorDb*
{
	if (m_type == EBglLayerType::TerrainVectorDb)
	{
		return dynamic_cast<IBglTerrainVectorDb*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainElevation() -> IBglTerrainElevation*
{
	if (m_type == EBglLayerType::TerrainLandClass)
	{
		return dynamic_cast<IBglTerrainElevation*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainLandClass() -> IBglTerrainLandClass*
{
	if (m_type == EBglLayerType::TerrainLandClass)
	{
		return dynamic_cast<IBglTerrainLandClass*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainWaterClass() -> IBglTerrainWaterClass*
{
	if (m_type == EBglLayerType::TerrainWaterClass)
	{
		return dynamic_cast<IBglTerrainWaterClass*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainRegion() -> IBglTerrainRegion*
{
	if (m_type == EBglLayerType::TerrainRegion)
	{
		return dynamic_cast<IBglTerrainRegion*>(this);
	}
	return nullptr;
}

auto CBglData::AsPopulationDensity() -> IBglPopulationDensity*
{
	if (m_type == EBglLayerType::PopulationDensity)
	{
		return dynamic_cast<IBglPopulationDensity*>(this);
	}
	return nullptr;
}

auto CBglData::AsAutogenAnnotation() -> IBglAutogenAnnotation*
{
	if (m_type == EBglLayerType::AutogenAnnotation)
	{
		return dynamic_cast<IBglAutogenAnnotation*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainIndex() -> IBglTerrainIndex*
{
	if (m_type == EBglLayerType::TerrainIndex)
	{
		return dynamic_cast<IBglTerrainIndex*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainTextureLookup() -> IBglTerrainTextureLookup*
{
	if (m_type == EBglLayerType::TerrainTextureLookup)
	{
		return dynamic_cast<IBglTerrainTextureLookup*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainSeason() -> IBglTerrainSeason*
{
	typedef std::underlying_type<EBglLayerType>::type layer_t;

	constexpr auto lower_bound = static_cast<layer_t>(EBglLayerType::TerrainSeasonJan);
	constexpr auto upper_bound = static_cast<layer_t>(EBglLayerType::TerrainSeasonDec);
	const auto layer_type = static_cast<layer_t>(m_type);
	
	if (layer_type >= lower_bound && layer_type <= upper_bound)
	{
		return dynamic_cast<IBglTerrainSeason*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainPhoto() -> IBglTerrainPhoto*
{
	typedef std::underlying_type<EBglLayerType>::type layer_t;

	constexpr auto lower_bound = static_cast<layer_t>(EBglLayerType::TerrainPhotoJan);
	constexpr auto upper_bound = static_cast<layer_t>(EBglLayerType::TerrainPhotoNight);
	const auto layer_type = static_cast<layer_t>(m_type);

	if (layer_type >= lower_bound && layer_type <= upper_bound)
	{
		return dynamic_cast<IBglTerrainPhoto*>(this);
	}
	return nullptr;
}

auto CBglData::AsTerrainPhoto32() -> IBglTerrainPhoto32*
{
	typedef std::underlying_type<EBglLayerType>::type layer_t;

	constexpr auto lower_bound = static_cast<layer_t>(EBglLayerType::TerrainPhoto32Jan);
	constexpr auto upper_bound = static_cast<layer_t>(EBglLayerType::TerrainPhoto32Night);
	const auto layer_type = static_cast<layer_t>(m_type);

	if (layer_type >= lower_bound && layer_type <= upper_bound)
	{
		return dynamic_cast<IBglTerrainPhoto32*>(this);
	}
	return nullptr;
}

auto CBglData::AsTacan() -> IBglTacan*
{
	if (m_type == EBglLayerType::Tacan)
	{
		return dynamic_cast<IBglTacan*>(this);
	}
	return nullptr;
}

auto CBglData::CalculateSize() const -> int
{
	if (m_data == nullptr)
	{
		return 0;
	}
	return m_data->CalculateSize();
}

auto CBglData::Validate() const -> bool
{
	if (m_data == nullptr)
	{
		return false;
	}
	return m_data->Validate();
}

auto CBglData::ReadBinary(BinaryFileStream& in) -> bool
{
	if (m_data == nullptr)
	{
		return false;
	}
	
	m_data->ReadBinary(in);
	
	if(!in)
	{
		return false;
	}
	return true;
}

auto CBglData::WriteBinary(BinaryFileStream& out) -> bool
{
	if (m_data == nullptr)
	{
		return false;
	}

	m_data->WriteBinary(out);

	if (!out)
	{
		return false;
	}
	return true;
}


//******************************************************************************
// CBglTile
//****************************************************************************** 

	
CBglTile::CBglTile(EBglLayerType type, const SBglTilePointer& pointer):
	m_tile_pointer(pointer), m_type(type)
{
}

std::shared_ptr<CBglTile> CBglTile::Clone() const
{
	//return std::make_unique<CBglTile>(*this);
	// TODO copying?
	return nullptr;
}

const SBglTilePointer& CBglTile::Pointer() const
{
	return m_tile_pointer.read();
}

// TODO - currently we just support moving offset within a file
// This needs to be extended to allow  clones, merges, etc
bool CBglTile::UpdateTilePointer(BinaryFileStream& out, const CPackedQmid& qmid)
{
	m_tile_pointer.write().StreamOffset = out.GetPosition();
	m_tile_pointer.write().SizeBytes = CalculateDataSize();
	m_tile_pointer.write().RecordCount = GetRecordCount();
	m_tile_pointer.write().QmidLow = qmid.Low();
	m_tile_pointer.write().QmidHigh = qmid.High();
	return out ? true : false;
}

EBglLayerType CBglTile::Type() const
{
	return m_type;
}

bool CBglTile::ReadBinary(BinaryFileStream& in)
{
	const auto count = static_cast<int>(Pointer().RecordCount);
	if (count <= 0)
	{
		return false;
	}

	in.SetPosition(Pointer().StreamOffset);
	if (!in)
	{
		return false;
	}

	m_data.reserve(count);

	
	for (auto i = 0; i < count; ++i)
	{
		const auto pos = in.GetPosition();
		std::unique_ptr<IBglSerializable> record;
		switch (m_type)
		{
		case EBglLayerType::Airport:
			record = std::make_unique<CBglAirport>();
			break;
		case EBglLayerType::AirportSummary:
			record = std::make_unique<CBglAirportSummary>();
			break;
		case EBglLayerType::Geopol:
			record = std::make_unique<CBglGeopol>();
			break;
		case EBglLayerType::Exclusion:
			record = std::make_unique<CBglExclusion>();
			break;
		case EBglLayerType::Marker:
			record = std::make_unique<CBglMarker>();
			break;
		case EBglLayerType::Nav:
			record = std::make_unique<CBglNav>();
			break;
		case EBglLayerType::Ndb:
			record = std::make_unique<CBglNdb>();
			break;
		case EBglLayerType::SceneryObject:
		{
			auto child_type = uint16_t{};
			auto child_size = uint16_t{};
			in >> child_type;
			in >> child_size;
			in.SetPosition(pos);

			switch(static_cast<IBglSceneryObject::ESceneryObjectType>(child_type))
			{
			case IBglSceneryObject::ESceneryObjectType::GenericBuilding:
				record = std::make_unique<CBglGenericBuilding>();
				break;
			case IBglSceneryObject::ESceneryObjectType::LibraryObject:
				record = std::make_unique<CBglLibraryObject>();
				break;
			case IBglSceneryObject::ESceneryObjectType::Windsock:
				record = std::make_unique<CBglWindsock>();
				break;
			case IBglSceneryObject::ESceneryObjectType::Effect:
				record = std::make_unique<CBglEffect>();
				break;
			case IBglSceneryObject::ESceneryObjectType::TaxiwaySigns:
				record = std::make_unique<CBglTaxiwaySigns>();
				break;
			case IBglSceneryObject::ESceneryObjectType::Trigger:
				record = std::make_unique<CBglTrigger>();
				break;
			case IBglSceneryObject::ESceneryObjectType::Beacon:
				record = std::make_unique<CBglBeacon>();
				break;
			case IBglSceneryObject::ESceneryObjectType::ExtrusionBridge:
				record = std::make_unique<CBglExtrusionBridge>();
				break;
			default:
				in.SetPosition(pos + static_cast<int>(child_size));
				continue; 
			}
		}
			break;
		case EBglLayerType::ModelData:
			record = std::make_unique<CBglModelData>();
			break;
		case EBglLayerType::Tacan:
			record = std::make_unique<CBglTacan>();
			break;
		case EBglLayerType::Boundary:
			record = std::make_unique<CBglBoundary>();
			break;
		case EBglLayerType::Waypoint:
			record = std::make_unique<CBglWaypoint>();
			break;
		default:
			continue; // TODO - do we need to set position????
		}
		
		record->ReadBinary(in);
		const auto size = record->CalculateSize();
		if (!record->Validate())
		{
			return false;
		}
		m_data.push_back(std::move(record));
		in.SetPosition(pos + static_cast<int>(size));
		if (!in)
		{
			return false;
		}
	}

	return true;
}

// Invariant - UpdateTilePointer shall have already been called
bool CBglTile::WriteBinary(BinaryFileStream& out)
{
	for (auto& data : m_data)
	{
		data->WriteBinary(out);
		if (!out)
		{
			return false;
		}
	}
	return true;
}

int CBglTile::CalculateDataSize() const
{
	auto size = 0;
	for (const auto& data : m_data)
	{
		size += data->CalculateSize();
	}
	return size;
}

int CBglTile::GetRecordCount() const
{
	return static_cast<int>(m_data.size());
}


template <typename T>
T* CBglTile::GetTileDataAt(int index) const
{
	return dynamic_cast<T*>(m_data[index].get());
}

	
//******************************************************************************
// CBglLayer
//******************************************************************************  


CBglLayer::CBglLayer(EBglLayerType type, EBglLayerClass layer_class, const SBglLayerPointer& data):
	m_data(data), m_type(type), m_class(layer_class)
{
}

std::unique_ptr<CBglLayer> CBglLayer::Factory(const SBglLayerPointer& data)
{

	std::unique_ptr<CBglLayer> layer = nullptr;
	const auto type = static_cast<EBglLayerClass>(data.DataClass);

	switch (type)
	{
	case EBglLayerClass::DirectQmid:
		layer = std::make_unique<CBglDirectQmidLayer>(data, data.Type);
		break;
	case EBglLayerClass::IndirectQmid: 
		break;
	case EBglLayerClass::AirportNameIndex: 
		break;
	case EBglLayerClass::IcaoIndex: 
		break;
	case EBglLayerClass::GuidIndex: 
		break;
	case EBglLayerClass::Exclusion: 
		break;
	case EBglLayerClass::TimeZone:
		layer = std::make_unique<CBglTimeZoneLayer>(data);
		break;
	case EBglLayerClass::Unknown:
		break;
	}
	return layer;
}

/*
CBglLayer::CBglLayer(CPackedQmid qmid, std::shared_ptr<CBglTile> tile): m_type(tile->Type())
{
	AddTile(qmid, std::move(tile));
}


CBglLayer::CBglLayer(const CBglLayer& other): m_type(other.m_type)
{
	for (const auto& tile : other.m_tiles)
	{
		m_tiles[tile.first] = tile.second->Clone();
	}
}


bool CBglLayer::AddTile(CPackedQmid qmid, std::shared_ptr<CBglTile> tile)
{
	if (tile->Type() != GetType())
	{
		return false;
	}
	m_tiles[qmid] = std::move(tile);
	return true;
}

CBglTile& CBglLayer::operator[](const CPackedQmid index)
{
	return *m_tiles[index];
}

std::unique_ptr<CBglLayer> CBglLayer::ReadBinary(
	BinaryFileStream& in,							 
	const std::map<EBglLayerType, std::unique_ptr<CBglLayer>>& layers)
{
	
}

const std::map<CPackedQmid, std::shared_ptr<CBglTile>>& CBglLayer::Tiles() const
{
	return m_tiles;
}
*/

auto CBglLayer::GetType() const -> EBglLayerType
{
	return m_type;
}

auto CBglLayer::SetType(EBglLayerType value) -> void
{
	m_type = value;
}

auto CBglLayer::GetClass() const -> EBglLayerClass
{
	return m_class;
}

auto CBglLayer::SetClass(EBglLayerClass value) -> void
{
	m_class = value;
}

auto CBglLayer::GetLayerPointer() const -> const SBglLayerPointer*
{
	return &m_data.read();
}

auto CBglLayer::AsIndirectQmidLayer() -> IBglIndirectQmidLayer*
{
	if (GetClass() == EBglLayerClass::IndirectQmid)
	{
		return dynamic_cast<IBglIndirectQmidLayer*>(this);
	}
	return nullptr;
}

auto CBglLayer::AsDirectQmidLayer() -> IBglDirectQmidLayer*
{
	if (GetClass() == EBglLayerClass::DirectQmid)
	{
		return dynamic_cast<IBglDirectQmidLayer*>(this);
	}
	return nullptr;
}

auto CBglLayer::AsNameListLayer() -> IBglNameListLayer*
{
	if (GetClass() == EBglLayerClass::AirportNameIndex)
	{
		return dynamic_cast<IBglNameListLayer*>(this);
	}
	return nullptr;
}

auto CBglLayer::AsIcaoLayer() -> IBglIcaoLayer*
{
	if (GetClass() == EBglLayerClass::IcaoIndex)
	{
		return dynamic_cast<IBglIcaoLayer*>(this);
	}
	return nullptr;
}

auto CBglLayer::AsGuidLayer() -> IBglGuidLayer*
{
	if (GetClass() == EBglLayerClass::GuidIndex)
	{
		return dynamic_cast<IBglGuidLayer*>(this);
	}
	return nullptr;
}

auto CBglLayer::AsExclusionLayer() -> IBglExclusionLayer*
{
	if (GetClass() == EBglLayerClass::Exclusion)
	{
		return dynamic_cast<IBglExclusionLayer*>(this);
	}
	return nullptr;
}

auto CBglLayer::AsTimeZoneLayer() -> IBglTimeZoneLayer*
{
	if (GetClass() == EBglLayerClass::TimeZone)
	{
		return dynamic_cast<IBglTimeZoneLayer*>(this);
	}
	return nullptr;
}

bool CBglLayer::IsTrq1BglLayer(EBglLayerType layer_type)
{
	if (layer_type == EBglLayerType::TerrainIndex ||
		layer_type == EBglLayerType::TerrainLandClass ||
		layer_type == EBglLayerType::TerrainWaterClass ||
		layer_type == EBglLayerType::TerrainRegion ||
		layer_type == EBglLayerType::PopulationDensity ||
		(layer_type >= EBglLayerType::TerrainSeasonJan && layer_type <= EBglLayerType::TerrainSeasonDec) ||
		(layer_type >= EBglLayerType::TerrainPhotoJan && layer_type <= EBglLayerType::TerrainPhotoNight) ||
		(layer_type >= EBglLayerType::TerrainPhoto32Jan && layer_type <= EBglLayerType::TerrainPhoto32Night))
	{
		return true;
	}
	return false;
}

bool CBglLayer::IsRcs1BglLayer(EBglLayerType layer_type)
{
	if (layer_type == EBglLayerType::TerrainElevation)
	{
		return true;
	}
	return false;
}

CBglDirectQmidLayer::CBglDirectQmidLayer(const SBglLayerPointer& pointer, EBglLayerType type) :
	CBglLayer(type, EBglLayerClass::DirectQmid, pointer)
{
}

auto CBglDirectQmidLayer::GetQmidCount() const -> int
{
	assert(m_pointers.size() == m_tiles.size());
	return static_cast<int>(m_pointers.size());
}

auto CBglDirectQmidLayer::HasQmid(CPackedQmid qmid) const -> bool
{
	auto it = m_tiles.find(qmid);
	if (it == m_tiles.end())
	{
		return false;
	}
	return true;
}

auto CBglDirectQmidLayer::GetDataPointerAtIndex(int index) const -> const SBglTilePointer*
{
	return m_pointers[index].get();
}

auto CBglDirectQmidLayer::GetDataCountAtQmid(CPackedQmid qmid) -> int
{
	auto it = m_tiles.find(qmid);
	if (it == m_tiles.end())
	{
		return 0;
	}
	return it->second.size();
}

auto CBglDirectQmidLayer::GetDataAtQmid(CPackedQmid qmid, int index) -> IBglData*
{
	auto it = m_tiles.find(qmid);
	if (it == m_tiles.end())
	{
		return nullptr;
	}
	return it->second[index].get();
}

auto CBglDirectQmidLayer::AddDataAtQmid(CPackedQmid qmid, const IBglData* data) -> void
{
	auto& record = m_tiles[qmid];
	// TODO - verify that copy constructor here then delete of old ptr does not dangle members
	//record.emplace_back(std::make_unique<CBglData>(*static_cast<const CBglData*>(data)));
	record.emplace_back(static_cast<const CBglData*>(data)->Clone());
}
	
auto CBglDirectQmidLayer::RemoveQmid(CPackedQmid qmid) -> void
{
	const auto it = m_tiles.find(qmid);
	if (it != m_tiles.end())
	{
		m_tiles.erase(it);
	}
}

auto CBglDirectQmidLayer::RemoveDataAtQmid(CPackedQmid qmid, int index) -> void
{
	const auto it = m_tiles.find(qmid);
	if (it != m_tiles.end())
	{
		it->second.erase(it->second.begin() + index);
		if (it->second.empty())
		{
			m_tiles.erase(it);
		}
	}
}


auto GetSceneryObjectType(BinaryFileStream& in) -> IBglSceneryObject::ESceneryObjectType
{
	const auto pos = in.GetPosition();
	auto child_type = uint16_t{};
	auto child_size = uint16_t{};
	in >> child_type;
	in >> child_size;
	in.SetPosition(pos);

	return static_cast<IBglSceneryObject::ESceneryObjectType>(child_type);
}
	
auto CBglDirectQmidLayer::ReadBinary(BinaryFileStream& in) -> bool
{
	const auto* layer_pointer = CBglLayer::GetLayerPointer();
	if (!in)
	{
		return false;
	}
	const auto next_position = in.GetPosition();
	in.SetPosition(layer_pointer->StreamOffset);
	if (!in)
	{
		return false;
	}

	// TODO - This needs to become a factory method
	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	auto layer_type = EBglLayerType::None;
	switch (layer_pointer->Type)
	{
	case EBglLayerType::TerrainPhoto32Jan:
		layer_type = EBglLayerType::Tacan;
		break;
	case EBglLayerType::TerrainPhoto32Feb:
		layer_type = EBglLayerType::TacanIndex;
		break;
	default:
		layer_type = layer_pointer->Type;
		break;
	}
	const auto qmid_count = static_cast<int>(layer_pointer->TileCount);

	auto existing = false;
	//TODO - merge layers when pointer match - maybe handle outside of layer code?
	/*
	for (const auto& other_layer : layers)
	{
		if (other_layer.second->m_data->StreamOffset == layer_pointer->StreamOffset)
		{
			existing = true;
			for (const auto& tile : other_layer.second->Tiles())
			{
				const auto tile_pointer(tile.second);
				layer->AddTile(tile.first, tile_pointer);
			}
			break;
		}
	}
	*/
	if (existing == false)
	{
		m_pointers.resize(qmid_count);
		for (auto i = 0; i < qmid_count; ++i)
		{
			m_pointers[i] = std::make_unique<SBglTilePointer>();
			m_pointers[i]->ReadBinary(in, layer_pointer->HasQmidHigh != 0);
			if (!in)
			{
				return false;
			}
		}
		if (in.GetPosition() != static_cast<int>(layer_pointer->StreamOffset + layer_pointer->SizeBytes))
		{
			return false;
		}
		for (const auto& tile_pointer : m_pointers)
		{
			/*
			auto tile = std::make_shared<CBglTile>(layer_type, *tile_pointer);
			if (!tile->ReadBinary(in))
			{
				return false;
			}

			m_tiles[CPackedQmid{tile_pointer->QmidLow, tile_pointer->QmidHigh}] = std::move(tile);
			*/
			
			const auto data_count = static_cast<int>(tile_pointer->RecordCount);
			if (data_count <= 0)
			{
				return false;
			}
	
			in.SetPosition(tile_pointer->StreamOffset);
			if (!in)
			{
				return false;
			}

			auto tile = m_tiles.emplace(std::make_pair(
				CPackedQmid{ tile_pointer->QmidLow, tile_pointer->QmidHigh },
				std::vector<std::unique_ptr<CBglData>>{}));
			auto& data_list = tile.first->second;
			
			data_list.reserve(data_count);

			for (auto i = 0; i < data_count; ++i)
			{
				const auto pos = in.GetPosition();
				const auto type = GetType();
				auto child_type = IBglSceneryObject::ESceneryObjectType::Unknown;
				if (type == EBglLayerType::SceneryObject)
				{
					child_type = GetSceneryObjectType(in);
				}
				auto data = CBglData::Factory(CBglLayer::GetType(), child_type);
				if (data == nullptr)
				{
					in.SetPosition(pos + static_cast<int>(tile_pointer->SizeBytes));
					continue;
				}
				if (!data->ReadBinary(in))
				{
					return false;
				}
				const auto size = data->CalculateSize();
				if (!data->Validate())
				{
					return false;
				}
				data_list.emplace_back(std::move(data));
				in.SetPosition(pos + static_cast<int>(size));
				if (!in)
				{
					return false;
				}
				
			}
		}
	}

	in.SetPosition(next_position);
	if (!in)
	{
		return false;
	}
	return true;
}

auto CBglDirectQmidLayer::CalculateSize() const -> int
{
	auto data_size = 0;
	for (const auto& tile : m_tiles)
	{
		for (const auto& data : tile.second)
		{
			data_size += data->CalculateSize();
		}
	}
	return data_size;
}

auto CBglDirectQmidLayer::CalculateDataPointersSize() const -> int
{
	auto tile_pointer_size = 16;
	for (const auto& pointer : m_pointers)
	{
		if (pointer->QmidHigh > 0)
		{
			tile_pointer_size = 20;
			break;
		}
	}
	return static_cast<int>(m_data->TileCount) * tile_pointer_size;
}

auto CBglDirectQmidLayer::WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile) -> bool
{
	const auto tile_pointers_size = CalculateDataPointersSize();

	auto type = CBglLayer::GetType();
	switch (type)  // NOLINT(clang-diagnostic-switch-enum)
	{
	case EBglLayerType::Tacan:
		type = EBglLayerType::TerrainPhoto32Jan;
		break;
	case EBglLayerType::TacanIndex:
		type = EBglLayerType::TerrainPhoto32Feb;
		break;
	default:
		break;
	}

	auto& data = m_data.write();
	data.Type = type;

	data.TileCount = static_cast<uint32_t>(m_tiles.size());
	data.DataClass = 1;
	if (m_data->TileCount && tile_pointers_size / static_cast<int>(m_data->TileCount) == 20)
	{
		data.HasQmidHigh = 1;
	}
	else
	{
		data.HasQmidHigh = 0;
	}
	data.SizeBytes = tile_pointers_size;
	data.StreamOffset = offset_to_tile;

	m_data->WriteBinary(out);
	return true;
}

auto CBglDirectQmidLayer::WriteBinaryData(BinaryFileStream& out) -> bool
{
	for (const auto& pointer : m_pointers)
	{
		const auto& tile =
			m_tiles[CPackedQmid{ pointer->QmidLow, pointer->QmidHigh }];
		
		pointer->StreamOffset = out.GetPosition();
		pointer->SizeBytes = CalculateSize();
		
		for (const auto& data : tile)
		{
			if (!data->WriteBinary(out))
			{
				return false;
			}
		}
	}
	return true;
}

auto CBglDirectQmidLayer::WriteBinaryDataPointers(BinaryFileStream& out) -> bool
{
	for (const auto& pointer : m_pointers)
	{
		pointer->WriteBinary(out, m_data->HasQmidHigh);
		if (!out)
		{
			return false;
		}
	}
	return true;
}


//******************************************************************************
// CBglTimezoneLayer
//******************************************************************************  


CBglTimeZoneLayer::CBglTimeZoneLayer(const SBglLayerPointer& pointer) :
	CBglLayer(EBglLayerType::TimeZone, EBglLayerClass::TimeZone, pointer)
{
}

auto CBglTimeZoneLayer::GetTimeZoneCount() const -> int
{
	return static_cast<int>(m_timezones->size());
}

auto CBglTimeZoneLayer::GetDataPointer() const -> const SBglTilePointer*
{
	return m_pointer.get();
}

auto CBglTimeZoneLayer::GetTimeZoneAt(int index) -> IBglTimeZone*
{
	return &m_timezones.write()[index];
}

auto CBglTimeZoneLayer::AddTimeZone(const IBglTimeZone* timezone) -> void
{
	m_timezones.write().emplace_back(*static_cast<const CBglTimeZone*>(timezone));
}

auto CBglTimeZoneLayer::RemoveTimeZone(const IBglTimeZone* timezone) -> void
{
	// This uses a unique_ptr - need to find a way to pass these safely via interface
	const auto iter = m_timezones.read().begin() +
		std::distance(m_timezones.read().data(), static_cast<const CBglTimeZone*>(timezone));
	m_timezones.write().erase(iter);
}

auto CBglTimeZoneLayer::ReadBinary(BinaryFileStream& in) -> bool
{
	assert(m_timezones->empty());
	
	if (CBglLayer::GetType() != EBglLayerType::TimeZone || 
		m_pointer != nullptr)
	{
		return false;
	}

	m_pointer = std::make_unique<SBglTilePointer>();
	m_pointer->ReadBinary(in, CBglLayer::GetLayerPointer()->HasQmidHigh != 0);
	if (!in)
	{
		return false;
	}

	const auto count = static_cast<int>(m_pointer->StreamOffset);
	m_timezones.write().resize(count);
	
	for (auto i = 0; i < count; ++i)
	{
		m_timezones.write()[i].ReadBinary(in);
	}

	if (!in)
	{
		return false;
	}

	return true;
}

auto CBglTimeZoneLayer::CalculateSize() const -> int
{
	auto data_size = 0;
	for (const auto& tile : m_timezones.read())
	{
		data_size += tile.CalculateSize();
	}
	return data_size;
}

auto CBglTimeZoneLayer::CalculateDataPointersSize() const -> int
{
	return 16; // TODO Constant
}

auto CBglTimeZoneLayer::WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile) -> bool
{
	auto& data = m_data.write();
	
	data.Type = EBglLayerType::TimeZone;
	data.TileCount = static_cast<uint32_t>(m_timezones->size());
	data.DataClass = 1;
	data.HasQmidHigh = 0;
	data.SizeBytes = CalculateDataPointersSize();
	data.StreamOffset = offset_to_tile;
	data.WriteBinary(out);
	return true;
}

auto CBglTimeZoneLayer::WriteBinaryData(BinaryFileStream& out) -> bool
{
	if (m_pointer == nullptr)
	{
		return false;
	}
	
	m_pointer->StreamOffset = out.GetPosition();
	m_pointer->SizeBytes = CalculateSize();
	
	for (auto& tile : m_timezones.write())
	{
		tile.WriteBinary(out);
	}
	
	if (!out)
	{
		return false;
	}
	return true;
}

auto CBglTimeZoneLayer::WriteBinaryDataPointers(BinaryFileStream& out) -> bool
{
	if (!m_pointer)
	{
		return false;
	}
	
	m_pointer->WriteBinary(out, m_data->HasQmidHigh);
	
	if (!out)
	{
		return false;
	}
	
	return true;
}

//******************************************************************************
// CBglFile
//****************************************************************************** 
	
CBglFile::CBglFile(): CBglFile(L"")
{
}

CBglFile::CBglFile(std::wstring file_name):
	m_file_name(std::move(file_name)),
	m_file_size(0),
	m_header(),
	m_dirty(false)
{
}

auto CBglFile::GetLayerCount() const -> int
{
	return static_cast<int>(m_layers.size());
}

auto CBglFile::HasLayer(EBglLayerType type) const -> bool
{
	const auto it = m_layer_offsets.find(type);
	if (it != m_layer_offsets.end())
	{
		return true;
	}
	return false;
}

auto CBglFile::GetLayerAt(int index) -> IBglLayer*
{
	return m_layers[index].get();
}

auto CBglFile::GetIndirectQmidLayer(EBglLayerType type) -> IBglIndirectQmidLayer*
{
	const auto it = m_layer_offsets.find(type);
	if (it == m_layer_offsets.end())
	{
		return nullptr;
	}
	return m_layers[it->second]->AsIndirectQmidLayer();
}

auto CBglFile::GetDirectQmidLayer(EBglLayerType type) -> IBglDirectQmidLayer*
{
	const auto it = m_layer_offsets.find(type);
	if (it == m_layer_offsets.end())
	{
		return nullptr;
	}
	return m_layers[it->second]->AsDirectQmidLayer();
}

auto CBglFile::GetNameListLayer() -> IBglNameListLayer*
{
	const auto it = m_layer_offsets.find(EBglLayerType::NameList);
	if (it == m_layer_offsets.end())
	{
		return nullptr;
	}
	return m_layers[it->second]->AsNameListLayer();
}

auto CBglFile::GetIcaoLayer(EBglLayerType type) -> IBglIcaoLayer*
{
	const auto it = m_layer_offsets.find(type);
	if (it == m_layer_offsets.end())
	{
		return nullptr;
	}
	return m_layers[it->second]->AsIcaoLayer();
}

auto CBglFile::GetGuidLayer(EBglLayerType type) -> IBglGuidLayer*
{
	const auto it = m_layer_offsets.find(type);
	if (it == m_layer_offsets.end())
	{
		return nullptr;
	}
	return m_layers[it->second]->AsGuidLayer();
}

auto CBglFile::GetExclusionLayer() -> IBglExclusionLayer*
{
	const auto it = m_layer_offsets.find(EBglLayerType::Exclusion);
	if (it == m_layer_offsets.end())
	{
		return nullptr;
	}
	return m_layers[it->second]->AsExclusionLayer();
}

auto CBglFile::GetTimeZoneLayer() -> IBglTimeZoneLayer*
{
	const auto it = m_layer_offsets.find(EBglLayerType::TimeZone);
	if (it == m_layer_offsets.end())
	{
		return nullptr;
	}
	return m_layers[it->second]->AsTimeZoneLayer();
}

bool CBglFile::Open()
{
	if (m_stream.IsOpen())
	{
		return true;
	}
	m_stream.Open(m_file_name);
	if (!m_stream)
	{
		return false;
	}
	m_stream.SetPosition(0, std::fstream::end);
	m_file_size = m_stream.GetPosition();
	m_stream.SetPosition(0, std::fstream::beg);
	return true;
}

bool CBglFile::Close()
{
	m_stream.Close();
	return m_stream ? true : false;
}

bool CBglFile::Write()
{
	if (m_stream.IsOpen())
	{
		m_stream.Close();
	}
	m_stream.Open(m_file_name,
				  std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc);
	if (!m_stream)
	{
		return false;
	}
	if (!WriteHeader())
	{
		return false;
	}
	if (!WriteAllLayers())
	{
		return false;
	}
	m_dirty = false;
	return true;
}

bool CBglFile::Read()
{
	if (!m_layers.empty())
	{
		return false;
	}
	if (!Open())
	{
		return false;
	}
	if (!ReadHeader())
	{
		return false;
	}
	return ReadAllLayers();
}

auto CBglFile::GetFileName() const -> const wchar_t*
{
	return m_file_name.c_str();
}

void CBglFile::Rename(const wchar_t* file_name)
{
	m_file_name = file_name;
	m_dirty = true;
}

bool CBglFile::TryMergeLayer(IBglLayer* layer)
{
	const auto it = m_layer_offsets.find(layer->GetType());
	if (it == m_layer_offsets.end())
	{
		//TODO - This currently fails if the layer exists
		// m_layers[it->second] = ;
		// m_layers.emplace_back(dynamic_cast<CBglLayer*>(layer));
		m_layers.emplace_back(dynamic_cast<const CBglLayer*>(layer)->Clone());
		m_layer_offsets.emplace(layer->GetType(), m_layers.size());
		m_dirty = true;
		return true;
	}
	return false;
}

void CBglFile::RemoveLayer(EBglLayerType type)
{
	const auto it = m_layer_offsets.find(type);
	if (it != m_layer_offsets.end())
	{
		// TODO - this keeps a nullptr in the vector, otherwise we would have to shift all offsets
		m_layers[it->second].reset();
		m_dirty = true;
	}
	m_layer_offsets.erase(it);
}

bool CBglFile::IsDirty() const
{
	return m_dirty;
}

int CBglFile::GetFileSize() const
{
	return m_file_size;
}

/*
CBglLayer* CBglFile::GetLayer(EBglLayerType type) const
{
	const auto it = m_layers.find(type);
	if (it == m_layers.end())
	{
		return nullptr;
	}
	return it->second.get();
}
*/

bool CBglFile::ReadAllLayers()
{
	if (m_stream.GetPosition() != static_cast<int>(m_header.HeaderSize))
	{
		m_stream.SetPosition(m_header.HeaderSize);
		if (!m_stream)
		{
			return false;
		}
	}
	const auto count = static_cast<int>(m_header.LayerCount);
	// remove static size to prevent nullptr layers
	// m_layers.resize(count);
	m_layers.reserve(count);
	
	for (auto i = 0; i < count; ++i)
	{
		auto layer_pointer = SBglLayerPointer{};
		layer_pointer.ReadBinary(m_stream);
		// auto layer = CBglLayer::ReadBinary(m_stream, m_layers);
		auto layer = CBglLayer::Factory(layer_pointer);
		if (layer == nullptr)
		{
			continue; // TODO - Add an unknown layer type? So we keep the pointer
		}
		layer->ReadBinary(m_stream);
		if (!m_stream)
		{
			return false;
		}

		m_layer_offsets[layer->GetType()] = static_cast<int>(m_layers.size());
		m_layers.emplace_back(std::move(layer));
	}
	return true;
}

// File order for resample is currently all layers after header (required),
// then all data, then all QMID pointers to tile data
// Although this may just be a compiler constraint. bglcomp for example looks different
bool CBglFile::WriteAllLayers()
{
	if (m_layers.size() != m_header.LayerCount ||
		m_stream.GetPosition() != static_cast<int>(m_header.HeaderSize))
	{
		return false;
	}
	auto data_size = HeaderSize();
	data_size += CBglLayer::CalculateLayerPointerSize() * m_header.LayerCount;

	// TODO - this is not order preserving (would iterate offsets first). Does it matter?
	for (const auto& layer : m_layers)
	{
		data_size += layer->CalculateSize();
	}
	for (const auto& layer : m_layers)
	{
		if (!layer->WriteBinaryPointer(m_stream, data_size) || !m_stream)
		{
			return false;
		}
		data_size += layer->CalculateDataPointersSize();
	}
	for (const auto& layer : m_layers)
	{
		if (!layer->WriteBinaryData(m_stream) || !m_stream)
		{
			return false;
		}
	}
	for (const auto& layer : m_layers)
	{
		if (!layer->WriteBinaryDataPointers(m_stream) && !m_stream)
		{
			return false;
		}
	}
	return true;
}

bool CBglFile::ReadHeader()
{
	if (!m_stream)
	{
		return false;
	}
	m_stream.SetPosition(0, std::fstream::beg);
	m_header = SBglHeader::ReadBinary(m_stream);
	if (!m_stream)
	{
		return false;
	}
	if (m_header.Version != Version())
	{
		return false;
	}
	if (m_header.FileMagic != FileMagic())
	{
		return false;
	}
	if (m_header.HeaderSize != HeaderSize())
	{
		return false;
	}
	if (m_header.QmidMagic != QmidMagic())
	{
		return false;
	}
	const auto count = static_cast<int>(m_header.LayerCount);
	if (count < 0 || count > std::numeric_limits<int32_t>::max())
	{
		return false;
	}
	return true;
}

bool CBglFile::WriteHeader()
{
	m_stream.SetPosition(0, std::fstream::beg);
	if (!m_stream)
	{
		return false;
	}
	if (m_dirty)
	{
		BuildHeader();
	}
	SBglHeader::WriteBinary(m_stream, m_header);
	return m_stream ? true : false;
}

bool CBglFile::BuildHeader()
{
	m_header.Version = Version();
	m_header.FileMagic = FileMagic();
	m_header.HeaderSize = HeaderSize();
	m_header.FileTime = 0; // TODO FILETIME library
	m_header.QmidMagic = QmidMagic();
	m_header.LayerCount = static_cast<uint32_t>(m_layers.size());
	return ComputeHeaderQmids();
}

// TODO Build Qmid Algorithm
bool CBglFile::ComputeHeaderQmids()
{
	m_header.PackedQMIDParent0 = 0;
	m_header.PackedQMIDParent1 = 0;
	m_header.PackedQMIDParent2 = 0;
	m_header.PackedQMIDParent3 = 0;
	m_header.PackedQMIDParent4 = 0;
	m_header.PackedQMIDParent5 = 0;
	m_header.PackedQMIDParent6 = 0;
	m_header.PackedQMIDParent7 = 0;
	return true;
}
	
} // namespace io

} // namespace flightsimlib

template FLIGHTSIMLIB_EXPORTED flightsimlib::io::IBglAirport* flightsimlib::io::CBglTile::GetTileDataAt(int index) const;
template FLIGHTSIMLIB_EXPORTED flightsimlib::io::IBglExclusion* flightsimlib::io::CBglTile::GetTileDataAt(int index) const;
template FLIGHTSIMLIB_EXPORTED flightsimlib::io::IBglMarker* flightsimlib::io::CBglTile::GetTileDataAt(int index) const;
template FLIGHTSIMLIB_EXPORTED flightsimlib::io::IBglGeopol* flightsimlib::io::CBglTile::GetTileDataAt(int index) const;
template FLIGHTSIMLIB_EXPORTED flightsimlib::io::IBglModelData* flightsimlib::io::CBglTile::GetTileDataAt(int index) const;
template FLIGHTSIMLIB_EXPORTED flightsimlib::io::IBglSceneryObject* flightsimlib::io::CBglTile::GetTileDataAt(int index) const;
