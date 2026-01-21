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


#ifndef FLIGHTSIMLIB_IO_BGLFILE_H
#define FLIGHTSIMLIB_IO_BGLFILE_H


//******************************************************************************
//
// Note to clients:  DO NOT USE THESE ROUTINES YET!
//
// The class structure is still being iterated and the API will continue to
// receive breaking changes until the next stable (minor) release version.
//
//******************************************************************************


#include "BinaryStream.h"
#include "Export.h"

#include "../external/stlab/copy_on_write.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

// TODO - this is for the timezone dependency
// Once we split up the headers, we can clean this up


#include "BglData.h"

// This is because of the mixins
#pragma warning( disable : 4250 )
#pragma warning( disable : 4251 )

// TODO: Library code
struct GUIDComparer
{
	bool operator()(const _GUID & Left, const _GUID & Right) const
	{
		// comparison logic goes here
		return memcmp(&Left , &Right,sizeof(Right)) < 0;
	}
};


namespace flightsimlib
{

namespace io
{

// Forward declarations
class IBglSerializable;
class IBglExclusion;


enum class EBglLayerType : uint32_t
{
	None = 0x0,
	Copyright = 0x1,
	Guid = 0x2,
	Airport = 0x3,
	Runway = 0x4,
	PrimaryOffsetThreshold = 0x5,
	SecondaryOffsetThreshold = 0x6,
	PrimaryBlastPad = 0x7,
	SecondaryBlastPad = 0x8,
	PrimaryOverrun = 0x9,
	SecondaryOverrun = 0xA,
	PrimaryLeftVasi = 0xB,
	PrimaryRightVasi = 0xC,
	SecondaryLeftVasi = 0xD,
	SecondaryRightVasi = 0xE,
	PrimaryApproachLights = 0xF,
	SecondaryApproachLights = 0x10,
	Start = 0x11,
	Com = 0x12,
	Nav = 0x13,
	Localizer = 0x14,
	GlideSlope = 0x15,
	Dme = 0x16,
	Ndb = 0x17,
	Marker = 0x18,
	Name = 0x19,
	TaxiwayPoint = 0x1A,
	Unused1B = 0x1B,
	TaxiwayPath = 0x1C,
	TaxiName = 0x1D,
	Unused1E = 0x1E,
	Unused1F = 0x1F,
	Boundary = 0x20,
	BoundaryEdges = 0x21,
	Waypoint = 0x22,
	Geopol = 0x23,
	Approach = 0x24,
	SceneryObject = 0x25,
	Helipad = 0x26,
	NameList = 0x27,
	VorIndex = 0x28,
	NdbIndex = 0x29,
	WaypointIndex = 0x2A,
	ModelData = 0x2B,
	AirportSummary = 0x2C,
	Unused2D = 0x2D, // also approach legs, but those are nested
	Exclusion = 0x2E,
	TimeZone = 0x2F,
	ApronPolygons = 0x30,
	ApronEdgeLights = 0x31,
	AirportSummaryChild = 0x32, // Don't know why this is different
	AirportDelete = 0x33,
	Apron = 0x37,
	BlastFence = 0x38,
	BoundaryFence = 0x39,
	Jetway = 0x3A,
	TaxiwayParking = 0x3D,
	TerrainVectorDb = 0x65,
	TerrainElevation = 0x67,
	TerrainLandClass = 0x68,
	TerrainWaterClass = 0x69,
	TerrainRegion = 0x6A,
	PopulationDensity = 0x6C,
	AutogenAnnotation = 0x6D,
	TerrainIndex = 0x6E,
	TerrainTextureLookup = 0x6F,
	TerrainSeasonJan = 0x78,
	TerrainSeasonFeb = 0x79,
	TerrainSeasonMar = 0x7A,
	TerrainSeasonApr = 0x7B,
	TerrainSeasonMay = 0x7C,
	TerrainSeasonJun = 0x7D,
	TerrainSeasonJul = 0x7E,
	TerrainSeasonAug = 0x7F,
	TerrainSeasonSep = 0x80,
	TerrainSeasonOct = 0x81,
	TerrainSeasonNov = 0x82,
	TerrainSeasonDec = 0x83,
	TerrainPhotoJan = 0x8C,
	TerrainPhotoFeb = 0x8D,
	TerrainPhotoMar = 0x8E,
	TerrainPhotoApr = 0x8F,
	TerrainPhotoMay = 0x90,
	TerrainPhotoJun = 0x91,
	TerrainPhotoJul = 0x92,
	TerrainPhotoAug = 0x93,
	TerrainPhotoSep = 0x94,
	TerrainPhotoOct = 0x95,
	TerrainPhotoNov = 0x96,
	TerrainPhotoDec = 0x97,
	TerrainPhotoNight = 0x98,
	TerrainPhoto32Jan = 0xA0, // CAUTION, ALSO TACAN!
	TerrainPhoto32Feb = 0xA1, // CAUTION, ALSO TACAN INDEX!
	TerrainPhoto32Mar = 0xA2,
	TerrainPhoto32Apr = 0xA3,
	TerrainPhoto32May = 0xA4,
	TerrainPhoto32Jun = 0xA5,
	TerrainPhoto32Jul = 0xA6,
	TerrainPhoto32Aug = 0xA7,
	TerrainPhoto32Sep = 0xA8,
	TerrainPhoto32Oct = 0xA9,
	TerrainPhoto32Nov = 0xAA,
	TerrainPhoto32Dec = 0xAB,
	TerrainPhoto32Night = 0xAC,
	Tacan = 0x10A0,
	TacanIndex = 0x10A1,       // See above
	FakeTypes = 0x2710,
	IcaoRunway = 0x2711,
	Max = 0x2712
};

enum class EBglLayerClass : uint32_t
{
	Unknown = 0x0,
	DirectQmid = 0x1,
	IndirectQmid = 0x2,
	AirportNameIndex = 0x3,
	IcaoIndex = 0x4,
	GuidIndex = 0x5,
	Exclusion = 0x6,
	TimeZone = 0x7
};
	

class CPackedQmid
{
public:
	
	explicit CPackedQmid(uint64_t value) : m_value(value) { }

	explicit CPackedQmid(uint32_t low, uint32_t high) : m_value(static_cast<uint64_t>(high) << 32 | low) { }
	
	bool operator<(const CPackedQmid& rhs) const noexcept
	{
		return this->m_value < rhs.m_value;
	}

	bool operator>(const CPackedQmid& rhs) const noexcept
	{
		return this->m_value > rhs.m_value;
	}

	uint32_t Low() const
	{
		return m_value & 0xFFFFFFFF;
	}

	uint32_t High() const 
	{
		return m_value >> 32;
	}

	uint64_t Value() const
	{
		return m_value;
	}

private:
	uint64_t m_value;
};


struct SBglLayerPointer
{
	EBglLayerType Type;
	uint16_t DataClass;
	uint16_t HasQmidHigh;
	uint32_t TileCount;
	uint32_t StreamOffset;
	uint32_t SizeBytes;

	void ReadBinary(BinaryFileStream& in)
	{
		int32_t type = 0;
		in >> type
			>> DataClass
			>> HasQmidHigh
			>> TileCount
			>> StreamOffset
			>> SizeBytes;
		if (type <= static_cast<int>(EBglLayerType::None) || type >= static_cast<int>(EBglLayerType::Max))
		{
			Type = EBglLayerType::None;
		}
		else
		{
			Type = static_cast<EBglLayerType>(type);
		}
	}

	void WriteBinary(BinaryFileStream& out) const
	{
		out << Type
			<< DataClass
			<< HasQmidHigh
			<< TileCount
			<< StreamOffset
			<< SizeBytes;
	}
};


struct SBglTilePointer
{
	uint32_t QmidLow;
	uint32_t QmidHigh;
	uint32_t RecordCount;
	uint32_t StreamOffset;
	uint32_t SizeBytes;
	
	void ReadBinary(BinaryFileStream& in, bool is_qmid64)
	{
		in >> QmidLow;
		if (is_qmid64)
		{
			in >> QmidHigh;
		}
		in >> RecordCount
		   >> StreamOffset
		   >> SizeBytes;
	}

	void WriteBinary(BinaryFileStream& out, bool is_qmid64) const
	{
		out << QmidLow;
		if (is_qmid64)
		{
			out << QmidHigh;
		}
		out << RecordCount
			<< StreamOffset
			<< SizeBytes;
	}
};


struct SBglGuidPointer
{
	_GUID    Name;
	uint32_t StreamOffset;
	uint32_t SizeBytes;
	
	void ReadBinary(BinaryFileStream& in)
	{
		in >> Name
		   >> StreamOffset
		   >> SizeBytes;
	}

	void WriteBinary(BinaryFileStream& out) const
	{
		out << Name
			<< StreamOffset
			<< SizeBytes;
	}
};


struct SBglIndirectPointer
{
	uint32_t StreamOffset;
	uint32_t SizeBytes;
	
	void ReadBinary(BinaryFileStream& in)
	{
		in >> StreamOffset
		   >> SizeBytes;
	}

	void WriteBinary(BinaryFileStream& out) const
	{
		out << StreamOffset
			<< SizeBytes;
	}
};


class IBglAirport;
class IBglNav;
class IBglNdb;
class IBglMarker;
class IBglBoundary;
class IBglWaypoint;
class IBglGeopol;
class IBglSceneryObject;
class IBglModelData;
class IBglAirportSummary;
class IBglTerrainVectorDb;
class IBglTerrainElevation;
class IBglTerrainLandClass;
class IBglTerrainWaterClass;
class IBglTerrainRegion;
class IBglPopulationDensity;
class IBglAutogenAnnotation;
class IBglTerrainIndex;
class IBglTerrainTextureLookup;
class IBglTerrainSeason;
class IBglTerrainPhoto;
class IBglTerrainPhoto32;
class IBglTacan;
	
class IBglData
{
public:
	virtual auto GetType() const -> EBglLayerType = 0;
	virtual auto SetType(EBglLayerType value) -> void = 0;
	virtual auto AsAirport() -> IBglAirport* = 0;
	virtual auto AsNav() -> IBglNav* = 0;
	virtual auto AsNdb() -> IBglNdb* = 0;
	virtual auto AsMarker() -> IBglMarker* = 0;
	virtual auto AsBoundary() -> IBglBoundary* = 0;
	virtual auto AsWaypoint() -> IBglWaypoint* = 0;
	virtual auto AsGeopol() -> IBglGeopol* = 0;
	virtual auto AsSceneryObject() -> IBglSceneryObject* = 0;
	virtual auto AsModelData() -> IBglModelData* = 0;
	virtual auto AsAirportSummary() -> IBglAirportSummary* = 0;
	virtual auto AsTerrainVectorDb() -> IBglTerrainVectorDb* = 0;
	virtual auto AsTerrainElevation() -> IBglTerrainElevation* = 0;
	virtual auto AsTerrainLandClass() -> IBglTerrainLandClass* = 0;
	virtual auto AsTerrainWaterClass() -> IBglTerrainWaterClass* = 0;
	virtual auto AsTerrainRegion() -> IBglTerrainRegion* = 0;
	virtual auto AsPopulationDensity() -> IBglPopulationDensity* = 0;
	virtual auto AsAutogenAnnotation() -> IBglAutogenAnnotation* = 0;
	virtual auto AsTerrainIndex() -> IBglTerrainIndex* = 0;
	virtual auto AsTerrainTextureLookup() -> IBglTerrainTextureLookup* = 0;
	virtual auto AsTerrainSeason() -> IBglTerrainSeason* = 0;
	virtual auto AsTerrainPhoto() -> IBglTerrainPhoto* = 0;
	virtual auto AsTerrainPhoto32() -> IBglTerrainPhoto32* = 0;
	virtual auto AsTacan() -> IBglTacan* = 0;
};

class CBglData : public IBglData
{
public:
	explicit CBglData(EBglLayerType type, std::unique_ptr<IBglSerializable> data);

	static std::unique_ptr<CBglData> Factory(EBglLayerType type, IBglSceneryObject::ESceneryObjectType child_type);
	std::unique_ptr<CBglData> Clone() const;
	
	auto GetType() const -> EBglLayerType override;
	auto SetType(EBglLayerType value) -> void override;
	auto AsAirport() -> IBglAirport* override;
	auto AsNav() -> IBglNav* override;
	auto AsNdb() -> IBglNdb* override;
	auto AsMarker() -> IBglMarker* override;
	auto AsBoundary() -> IBglBoundary* override;
	auto AsWaypoint() -> IBglWaypoint* override;
	auto AsGeopol() -> IBglGeopol* override;
	auto AsSceneryObject() -> IBglSceneryObject* override;
	auto AsModelData() -> IBglModelData* override;
	auto AsAirportSummary() -> IBglAirportSummary* override;
	auto AsTerrainVectorDb() -> IBglTerrainVectorDb* override;
	auto AsTerrainElevation() -> IBglTerrainElevation* override;
	auto AsTerrainLandClass() -> IBglTerrainLandClass* override;
	auto AsTerrainWaterClass() -> IBglTerrainWaterClass* override;
	auto AsTerrainRegion() -> IBglTerrainRegion* override;
	auto AsPopulationDensity() -> IBglPopulationDensity* override;
	auto AsAutogenAnnotation() -> IBglAutogenAnnotation* override;
	auto AsTerrainIndex() -> IBglTerrainIndex* override;
	auto AsTerrainTextureLookup() -> IBglTerrainTextureLookup* override;
	auto AsTerrainSeason() -> IBglTerrainSeason* override;
	auto AsTerrainPhoto() -> IBglTerrainPhoto* override;
	auto AsTerrainPhoto32() -> IBglTerrainPhoto32* override;
	auto AsTacan() -> IBglTacan* override;

	auto CalculateSize() const -> int;
	auto Validate() const -> bool;
	auto ReadBinary(BinaryFileStream& in) -> bool;
	auto WriteBinary(BinaryFileStream& out) -> bool;

private:
	EBglLayerType m_type = EBglLayerType::None;
	std::unique_ptr<IBglSerializable> m_data;
};
	

class IBglIndirectQmidLayer;
class IBglDirectQmidLayer;
class IBglNameListLayer;
class IBglIcaoLayer;
class IBglGuidLayer;
class IBglExclusionLayer;
class IBglTimeZoneLayer;

	
class IBglLayer
{
public:
	virtual auto GetType() const -> EBglLayerType = 0;
	virtual auto SetType(EBglLayerType value) -> void = 0;
	virtual auto GetClass() const -> EBglLayerClass = 0;
	virtual auto SetClass(EBglLayerClass value) -> void = 0;
	virtual auto GetLayerPointer() const -> const SBglLayerPointer* = 0;
	virtual auto AsIndirectQmidLayer() -> IBglIndirectQmidLayer* = 0;
	virtual auto AsDirectQmidLayer() -> IBglDirectQmidLayer* = 0;
	virtual auto AsNameListLayer() -> IBglNameListLayer* = 0;
	virtual auto AsIcaoLayer() -> IBglIcaoLayer* = 0;
	virtual auto AsGuidLayer() -> IBglGuidLayer* = 0;
	virtual auto AsExclusionLayer() -> IBglExclusionLayer* = 0;
	virtual auto AsTimeZoneLayer() -> IBglTimeZoneLayer* = 0;
};

class IBglDirectQmidLayer : virtual public IBglLayer
{
public:
	virtual auto GetQmidCount() const -> int = 0;
	virtual auto HasQmid(CPackedQmid qmid) const -> bool = 0;
	virtual auto GetDataPointerAtIndex(int index) const -> const SBglTilePointer* = 0;
	virtual auto GetDataCountAtQmid(CPackedQmid qmid) -> int = 0;
	virtual auto GetDataAtQmid(CPackedQmid qmid, int index) -> IBglData* = 0;
	virtual auto AddDataAtQmid(CPackedQmid qmid, const IBglData* data) -> void = 0;
	virtual auto RemoveQmid(CPackedQmid qmid) -> void = 0;
	virtual auto RemoveDataAtQmid(CPackedQmid qmid, int index) -> void = 0;
};


/// <summary>
/// Standard flow would be:
/// if (HasQmid(qmid)) // optional
/// {
///		const auto count = GetDataCountAtQmid(qmid);
///		for (auto i = 0; i < count; ++i)
///		{
///			const auto index = GetDataIndexAtQmid(qmid, i);
///			auto data = GetDataAtIndex(index);
///			// ...
///		}
/// }
/// </summary>
class IBglIndirectQmidLayer : virtual public IBglLayer
{
public:
	virtual auto GetQmidCount() const -> int = 0;
	virtual auto HasQmid(CPackedQmid qmid) const -> bool = 0;
	virtual auto GetDataPointerAtQmid(CPackedQmid qmid) const -> const SBglTilePointer* = 0;
	virtual auto GetDataCountAtQmid(CPackedQmid qmid) const -> int = 0;
	virtual auto GetIndirectPointerCount() const -> int = 0;
	virtual auto GetIndirectPointerAtIndex(int index) const -> const SBglIndirectPointer* = 0;
	virtual auto GetIndirectPointerAtQmid(CPackedQmid qmid, int index) const -> const SBglIndirectPointer* = 0;
	virtual auto GetDataIndexAtQmid(CPackedQmid qmid, int index) const -> int = 0;
	virtual auto GetDataCount() const -> int = 0;
	virtual auto GetDataAtIndex(int index) -> IBglData* = 0;
	virtual auto AddDataAtQmids(CPackedQmid* qmids, int count, const IBglData* data) -> void = 0;
	virtual auto RemoveDataAtIndex(int index) -> void = 0;
};


class IBglNameList;

class IBglNameListLayer
{
public:
	virtual auto GetDataPointer() const -> const SBglTilePointer* = 0;
	virtual auto GetNameList() -> IBglNameList* = 0;
	virtual auto SetNameList(IBglNameList* value) -> void = 0;
};
	

class IBglIcaoIndex;

class IBglIcaoLayer : virtual public IBglLayer
{
public:
	virtual auto GetIcaoCount() const -> int = 0;
	virtual auto GetDataPointer() const -> const SBglTilePointer* = 0;
	virtual auto GetIcaoAt(int index) -> IBglIcaoIndex* = 0;
	virtual auto AddIcao(const IBglIcaoIndex* index) -> void = 0;
	virtual auto RemoveIcao(const IBglIcaoIndex* index) -> void = 0;
};

	
class IBglGuidLayer : virtual public IBglLayer
{
public:
	virtual auto GetGuidCount() const -> int = 0;
	virtual auto GetDataPointer() const -> const SBglTilePointer* = 0;
	virtual auto GetGuidPointerAt(int index) const -> const SBglGuidPointer* = 0;
	virtual auto HasGuid(_GUID guid) const -> bool = 0;
	virtual auto GetData(_GUID guid) const -> const IBglData* = 0;
	virtual auto AddData(_GUID guid, const IBglData* data) -> void = 0;
	virtual auto RemoveData(_GUID guid) -> void = 0;
};
	
class IBglExclusion;
	
class IBglExclusionLayer : virtual public IBglLayer
{
public:
	virtual auto GetExclusionCount() const -> int = 0;
	virtual auto GetDataPointer() const -> const SBglTilePointer* = 0;
	virtual auto GetExclusionAt(int index) -> IBglExclusion* = 0;
	virtual auto AddExclusion(const IBglExclusion* exclusion) -> void = 0;
	virtual auto RemoveExclusion(const IBglExclusion* exclusion) -> void = 0;
};

class IBglTimeZone;

class IBglTimeZoneLayer : virtual public IBglLayer
{
public:
	virtual auto GetTimeZoneCount() const -> int = 0;
	virtual auto GetDataPointer() const -> const SBglTilePointer* = 0;
	virtual auto GetTimeZoneAt(int index) -> IBglTimeZone* = 0;
	virtual auto AddTimeZone(const IBglTimeZone* timezone) -> void = 0;
	virtual auto RemoveTimeZone(const IBglTimeZone* timezone) -> void = 0;
};


class FLIGHTSIMLIB_EXPORTED IBglFile  // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
{
public:
	virtual auto GetLayerCount() const -> int = 0;
	virtual auto HasLayer(EBglLayerType type) const -> bool = 0;
	virtual auto GetLayerAt(int index) -> IBglLayer* = 0;
	virtual auto GetIndirectQmidLayer(EBglLayerType type) -> IBglIndirectQmidLayer* = 0;
	virtual auto GetDirectQmidLayer(EBglLayerType type) -> IBglDirectQmidLayer* = 0;
	virtual auto GetNameListLayer() -> IBglNameListLayer* = 0;
	virtual auto GetIcaoLayer(EBglLayerType type) -> IBglIcaoLayer* = 0;
	virtual auto GetGuidLayer(EBglLayerType type) -> IBglGuidLayer* = 0;
	virtual auto GetExclusionLayer() -> IBglExclusionLayer* = 0;
	virtual auto GetTimeZoneLayer() -> IBglTimeZoneLayer* = 0;
	virtual auto Open() -> bool = 0;
	virtual auto Close() -> bool = 0;
	virtual auto Write() -> bool = 0;
	virtual auto Read() -> bool = 0;
	virtual auto GetFileName() const -> const wchar_t* = 0;
	virtual auto Rename(const wchar_t* file_name) -> void = 0;
	virtual auto IsDirty() const -> bool = 0;
	virtual auto GetFileSize() const -> int = 0;
	virtual auto TryMergeLayer(IBglLayer* layer) -> bool = 0;
	virtual auto RemoveLayer(EBglLayerType type) -> void = 0;

protected:
	virtual ~IBglFile() = default;
};


class FLIGHTSIMLIB_EXPORTED CBglLayer : virtual public IBglLayer
{
public:
	explicit CBglLayer(EBglLayerType type, EBglLayerClass layer_class, const SBglLayerPointer& data);
	virtual ~CBglLayer() = default;

	static std::unique_ptr<CBglLayer> Factory(const SBglLayerPointer& data);
	auto Clone() const { return std::unique_ptr<CBglLayer>(CloneImpl()); }

	virtual auto ReadBinary(BinaryFileStream& in) -> bool = 0;
	virtual int CalculateSize() const = 0;
	virtual int CalculateDataPointersSize() const = 0;
	virtual bool WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer) = 0;
	virtual bool WriteBinaryData(BinaryFileStream& out) = 0;
	virtual bool WriteBinaryDataPointers(BinaryFileStream& out) = 0;
	
	auto GetType() const -> EBglLayerType override;
	auto SetType(EBglLayerType value) -> void override;
	auto GetClass() const -> EBglLayerClass override;
	auto SetClass(EBglLayerClass value) -> void override;
	auto GetLayerPointer() const -> const SBglLayerPointer* override;
	auto AsIndirectQmidLayer() -> IBglIndirectQmidLayer* override;
	auto AsDirectQmidLayer() -> IBglDirectQmidLayer* override;
	auto AsNameListLayer() -> IBglNameListLayer* override;
	auto AsIcaoLayer() -> IBglIcaoLayer* override;
	auto AsGuidLayer() -> IBglGuidLayer* override;
	auto AsExclusionLayer() -> IBglExclusionLayer* override;
	auto AsTimeZoneLayer() -> IBglTimeZoneLayer* override;
	
	static bool IsTrq1BglLayer(EBglLayerType layer_type);
	static bool IsRcs1BglLayer(EBglLayerType layer_type);

	static constexpr int CalculateLayerPointerSize()
	{
		return 20;
	}

protected:
	virtual CBglLayer* CloneImpl() const = 0;
	
	stlab::copy_on_write<SBglLayerPointer> m_layer_pointer;
	
private:
	EBglLayerType m_type;
	EBglLayerClass m_class;
};


class CBglDirectQmidLayer final : public IBglDirectQmidLayer, public CBglLayer
{
public:
	explicit CBglDirectQmidLayer(const SBglLayerPointer& pointer, EBglLayerType type);
	CBglDirectQmidLayer(const CBglDirectQmidLayer& other);

	auto ReadBinary(BinaryFileStream& in) -> bool override;
	auto CalculateSize() const -> int override;
	auto CalculateDataPointersSize() const -> int override;
	auto WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer) -> bool override;
	auto WriteBinaryData(BinaryFileStream& out) -> bool override;
	auto WriteBinaryDataPointers(BinaryFileStream& out) -> bool override;
	
	auto GetQmidCount() const -> int override;
	auto HasQmid(CPackedQmid qmid) const -> bool override;
	auto GetDataPointerAtIndex(int index) const -> const SBglTilePointer* override;
	auto GetDataCountAtQmid(CPackedQmid qmid) -> int override;
	auto GetDataAtQmid(CPackedQmid qmid, int index) -> IBglData* override;
	auto AddDataAtQmid(CPackedQmid qmid, const IBglData* data) -> void override;
	auto RemoveQmid(CPackedQmid qmid) -> void override;
	auto RemoveDataAtQmid(CPackedQmid qmid, int index) -> void override;	

private:
	auto CloneImpl() const -> CBglLayer* override
	{
		return new CBglDirectQmidLayer(*this);
	}

	static auto GetSceneryObjectType(BinaryFileStream& in) -> IBglSceneryObject::ESceneryObjectType;
	
	std::map<CPackedQmid, std::vector<std::unique_ptr<CBglData>>> m_tiles;
	std::vector<std::unique_ptr<SBglTilePointer>> m_pointers; // TODO - why pointer?
};


class CBglIndirectQmidLayer final : public IBglIndirectQmidLayer, public CBglLayer
{
public:
	explicit CBglIndirectQmidLayer(const SBglLayerPointer& pointer, EBglLayerType type);
	CBglIndirectQmidLayer(const CBglIndirectQmidLayer& other);
	
	auto ReadBinary(BinaryFileStream& in) -> bool override;
	auto CalculateSize() const -> int override;
	auto CalculateDataPointersSize() const -> int override;
	auto WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer) -> bool override;
	auto WriteBinaryData(BinaryFileStream& out) -> bool override;
	auto WriteBinaryDataPointers(BinaryFileStream& out) -> bool override;
	
	auto GetQmidCount() const -> int override;
	auto HasQmid(CPackedQmid qmid) const -> bool override;
	auto GetDataPointerAtQmid(CPackedQmid qmid) const -> const SBglTilePointer* override;
	auto GetDataCountAtQmid(CPackedQmid qmid) const -> int override;
	auto GetIndirectPointerCount() const -> int override;
	auto GetIndirectPointerAtIndex(int index) const -> const SBglIndirectPointer* override;
	auto GetIndirectPointerAtQmid(CPackedQmid qmid, int index) const -> const SBglIndirectPointer* override;
	auto GetDataIndexAtQmid(CPackedQmid qmid, int index) const -> int override;
	auto GetDataCount() const -> int override;
	auto GetDataAtIndex(int index) -> IBglData* override;
	auto AddDataAtQmids(CPackedQmid* qmids, int count, const IBglData* data) -> void override;
	auto RemoveDataAtIndex(int index) -> void override;

private:
	auto CloneImpl() const -> CBglLayer* override
	{
		return new CBglIndirectQmidLayer(*this);
	}
	
	stlab::copy_on_write<std::map<CPackedQmid, SBglTilePointer>> m_tiles;
	stlab::copy_on_write<std::vector<SBglIndirectPointer>> m_pointers;
	stlab::copy_on_write<std::vector<int>> m_offsets;
	std::vector<std::unique_ptr<CBglData>> m_data; // TODO - cow doesn't work here
};


class CBglNameListLayer : public IBglNameListLayer, public CBglLayer
{
public:
	auto GetDataPointer() const -> const SBglTilePointer* override;
	auto GetNameList() -> IBglNameList* override;
	auto SetNameList(IBglNameList* value) -> void override;
};


class CBglIcaoLayer : public IBglIcaoLayer, public CBglLayer
{
public:
	auto GetIcaoCount() const -> int override;
	auto GetDataPointer() const -> const SBglTilePointer* override;
	auto GetIcaoAt(int index) -> IBglIcaoIndex* override;
	auto AddIcao(const IBglIcaoIndex* index) -> void override;
	auto RemoveIcao(const IBglIcaoIndex* index) -> void override;
};


class CBglGuidLayer final : public IBglGuidLayer, public CBglLayer
{
public:
	explicit CBglGuidLayer(const SBglLayerPointer& pointer, EBglLayerType type);
	CBglGuidLayer(const CBglGuidLayer& other);

	auto ReadBinary(BinaryFileStream& in) -> bool override;
	auto CalculateSize() const -> int override;
	auto CalculateDataPointersSize() const -> int override;
	auto WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer) -> bool override;
	auto WriteBinaryData(BinaryFileStream& out) -> bool override;
	auto WriteBinaryDataPointers(BinaryFileStream& out) -> bool override;
	
	auto GetGuidCount() const -> int override;
	auto GetDataPointer() const -> const SBglTilePointer* override;
	auto GetGuidPointerAt(int index) const -> const SBglGuidPointer* override;
	auto HasGuid(_GUID guid) const -> bool override;
	auto GetData(_GUID guid) const -> const IBglData* override;
	auto AddData(_GUID guid, const IBglData* data) -> void override;
	auto RemoveData(_GUID guid) -> void override;

private:
	auto CloneImpl() const -> CBglLayer* override
	{
		return new CBglGuidLayer(*this);
	}
	
	stlab::copy_on_write<SBglTilePointer> m_pointer;
	std::map<_GUID, int, GUIDComparer> m_offsets;
	// TODO: would a pair / tuple in a single vector make more sense here?
	stlab::copy_on_write<std::vector<SBglGuidPointer>> m_guids;
	std::vector<std::unique_ptr<CBglData>> m_data; // TODO - cow doesn't work here
};


class CBglExclusionLayer final : public IBglExclusionLayer, public CBglLayer
{
public:
	explicit CBglExclusionLayer(const SBglLayerPointer& pointer);
	CBglExclusionLayer(const CBglExclusionLayer& other);
	
	auto ReadBinary(BinaryFileStream& in) -> bool override;
	auto CalculateSize() const -> int override;
	auto CalculateDataPointersSize() const -> int override;
	auto WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer) -> bool override;
	auto WriteBinaryData(BinaryFileStream& out) -> bool override;
	auto WriteBinaryDataPointers(BinaryFileStream& out) -> bool override;
	
	auto GetExclusionCount() const -> int override;
	auto GetDataPointer() const -> const SBglTilePointer* override;
	auto GetExclusionAt(int index) -> IBglExclusion* override;
	auto AddExclusion(const IBglExclusion* exclusion) -> void override;
	auto RemoveExclusion(const IBglExclusion* exclusion) -> void override;

private:
	auto CloneImpl() const -> CBglLayer* override
	{
		return new CBglExclusionLayer(*this);
	}
	
	stlab::copy_on_write<SBglTilePointer> m_pointer;
	stlab::copy_on_write<std::vector<CBglExclusion>> m_exclusions;
};

	
class CBglTimeZoneLayer final : public IBglTimeZoneLayer, public CBglLayer
{
public:
	explicit CBglTimeZoneLayer(const SBglLayerPointer& pointer);
	CBglTimeZoneLayer(const CBglTimeZoneLayer& other);

	auto ReadBinary(BinaryFileStream& in) -> bool override;
	auto CalculateSize() const -> int override;
	auto CalculateDataPointersSize() const -> int override;
	auto WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer) -> bool override;
	auto WriteBinaryData(BinaryFileStream& out) -> bool override;
	auto WriteBinaryDataPointers(BinaryFileStream& out) -> bool override;
	
	auto GetTimeZoneCount() const -> int override;
	auto GetDataPointer() const -> const SBglTilePointer* override;
	auto GetTimeZoneAt(int index) -> IBglTimeZone* override;
	auto AddTimeZone(const IBglTimeZone* timezone) -> void override;
	auto RemoveTimeZone(const IBglTimeZone* timezone) -> void override;

private:
	auto CloneImpl() const -> CBglLayer* override
	{
		return new CBglTimeZoneLayer(*this);
	}
	
	stlab::copy_on_write<SBglTilePointer> m_pointer;
	stlab::copy_on_write<std::vector<CBglTimeZone>> m_timezones;
};


struct SBglHeader
{
	uint16_t Version;
	uint16_t FileMagic;
	uint32_t HeaderSize;
	uint64_t FileTime;
	uint32_t QmidMagic;
	uint32_t LayerCount;
	uint32_t PackedQMIDParent0;
	uint32_t PackedQMIDParent1;
	uint32_t PackedQMIDParent2;
	uint32_t PackedQMIDParent3;
	uint32_t PackedQMIDParent4;
	uint32_t PackedQMIDParent5;
	uint32_t PackedQMIDParent6;
	uint32_t PackedQMIDParent7;

	static SBglHeader ReadBinary(BinaryFileStream& in)
	{
		auto header =  SBglHeader{};
		in >> header.Version
		   >> header.FileMagic
		   >> header.HeaderSize
		   >> header.FileTime
		   >> header.QmidMagic
		   >> header.LayerCount
		   >> header.PackedQMIDParent0
		   >> header.PackedQMIDParent1
		   >> header.PackedQMIDParent2
		   >> header.PackedQMIDParent3
		   >> header.PackedQMIDParent4
		   >> header.PackedQMIDParent5
		   >> header.PackedQMIDParent6
		   >> header.PackedQMIDParent7;
		return header;
	}

	static void WriteBinary(BinaryFileStream& out, const SBglHeader& header)
	{
		out << header.Version
			<< header.FileMagic
			<< header.HeaderSize
			<< header.FileTime
			<< header.QmidMagic
			<< header.LayerCount
			<< header.PackedQMIDParent0
			<< header.PackedQMIDParent1
			<< header.PackedQMIDParent2
			<< header.PackedQMIDParent3
			<< header.PackedQMIDParent4
			<< header.PackedQMIDParent5
			<< header.PackedQMIDParent6
			<< header.PackedQMIDParent7;
	}
};

class FLIGHTSIMLIB_EXPORTED CBglFile final : IBglFile
{
public:
	CBglFile();
	explicit CBglFile(std::wstring file_name);
	~CBglFile() = default;
	// TODO - This is now move-only, not copyable
	CBglFile(const CBglFile&) = delete;
	CBglFile& operator= (const CBglFile&) = delete;
	CBglFile(CBglFile&&) noexcept = default;
	CBglFile& operator=(CBglFile&&) noexcept = delete;


	auto GetLayerCount() const -> int override;
	auto HasLayer(EBglLayerType type) const -> bool override;
	auto GetLayerAt(int index)->IBglLayer* override;
	auto GetIndirectQmidLayer(EBglLayerType type) -> IBglIndirectQmidLayer* override;
	auto GetDirectQmidLayer(EBglLayerType type) -> IBglDirectQmidLayer* override;
	auto GetNameListLayer() -> IBglNameListLayer* override;
	auto GetIcaoLayer(EBglLayerType type) -> IBglIcaoLayer* override;
	auto GetGuidLayer(EBglLayerType type) -> IBglGuidLayer* override;
	auto GetExclusionLayer() -> IBglExclusionLayer* override;
	auto GetTimeZoneLayer() -> IBglTimeZoneLayer* override;
	auto Open() -> bool override;
	auto Close() -> bool override;
	auto Write() -> bool override;
	auto Read() -> bool override;
	auto GetFileName() const -> const wchar_t* override;
	auto Rename(const wchar_t* file_name) -> void override;
	auto IsDirty() const -> bool override;
	auto GetFileSize() const -> int override;
	auto TryMergeLayer(IBglLayer* layer) -> bool override;
	auto RemoveLayer(EBglLayerType type) -> void override;

private:
	bool ReadAllLayers();
	bool WriteAllLayers();
	bool ReadHeader();
	bool WriteHeader();
	bool BuildHeader();
	bool ComputeHeaderQmids();

	static constexpr uint16_t Version()
	{
		return 0x0201; // FSX+
	}

	static constexpr uint32_t HeaderSize()
	{
		return sizeof(SBglHeader);
	}

	static constexpr uint16_t FileMagic()
	{
		return 0x1992;
	}

	static constexpr uint32_t QmidMagic()
	{
		return 0x08051803;
	}
	
	std::wstring m_file_name;
	int m_file_size;
	SBglHeader m_header;
	bool m_dirty;
	std::vector<std::unique_ptr<CBglLayer>> m_layers;
	std::map<EBglLayerType, int> m_layer_offsets;
	BinaryFileStream m_stream;
};

} // namespace io

} // namespace flightsimlib

#endif