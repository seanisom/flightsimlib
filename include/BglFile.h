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
	TaxiwayPath = 0x1C,
	TaxiName = 0x1D,
	Boundary = 0x20,
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
	Exclusion = 0x2E,
	TimeZone = 0x2F,
	ApronPolygons = 0x30,
	ApronEdgeLights = 0x31,
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


class CPackedQmid
{
public:
	
	explicit CPackedQmid(uint64_t value) : m_value(value) { }

	explicit CPackedQmid(uint32_t low, uint32_t high) : m_value(static_cast<uint64_t>(high) << 32 | low) { }
	
	bool operator<(const CPackedQmid& rhs) const noexcept
	{
		return this->m_value < rhs.m_value;
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
	uint16_t HasQmidLow;
	uint16_t HasQmidHigh;
	uint32_t TileCount;
	uint32_t StreamOffset;
	uint32_t SizeBytes;

	static SBglLayerPointer ReadBinary(BinaryFileStream& in)
	{
		auto pointer = SBglLayerPointer{};
		int32_t type = 0;
		in >> type
			>> pointer.HasQmidLow
		    >> pointer.HasQmidHigh
			>> pointer.TileCount
			>> pointer.StreamOffset
			>> pointer.SizeBytes;
		if (type <= static_cast<int>(EBglLayerType::None) || type >= static_cast<int>(EBglLayerType::Max))
		{
			pointer.Type = EBglLayerType::None;
		}
		else
		{
			pointer.Type = static_cast<EBglLayerType>(type);
		}
		return pointer;
	}

	static void WriteBinary(BinaryFileStream& out, const SBglLayerPointer& pointer)
	{
		out << pointer.Type
			<< pointer.HasQmidLow
		    << pointer.HasQmidHigh
			<< pointer.TileCount
			<< pointer.StreamOffset
			<< pointer.SizeBytes;
	}
};


struct SBglTilePointer
{
	uint32_t QmidLow;
	uint32_t QmidHigh;
	uint32_t RecordCount;
	uint32_t StreamOffset;
	uint32_t SizeBytes;
	
	static SBglTilePointer ReadBinary(BinaryFileStream& in, const SBglLayerPointer& layer)
	{
		auto pointer = SBglTilePointer{};
		in >> pointer.QmidLow;
		if (layer.HasQmidHigh)
		{
			in >> pointer.QmidHigh;
		}
		in >> pointer.RecordCount
		   >> pointer.StreamOffset
		   >> pointer.SizeBytes;
		return pointer;
	}

	static void WriteBinary(BinaryFileStream& out, const SBglTilePointer& pointer, const SBglLayerPointer& layer)
	{
		out << pointer.QmidLow;
		if (layer.HasQmidHigh)
		{
			out << pointer.QmidHigh;
		}
		out << pointer.RecordCount
		    << pointer.StreamOffset
		    << pointer.SizeBytes;
	}
};


class FLIGHTSIMLIB_EXPORTED CBglTile final
{
public:
	explicit CBglTile(EBglLayerType type, const SBglTilePointer& pointer);

	// TODO - non,copyable, non-moveable
	CBglTile(const CBglTile& other) = delete;
	CBglTile& operator= (const CBglTile&) = delete;
	CBglTile(CBglTile&&) = delete;
	CBglTile& operator=(CBglTile&&) = delete;

	std::shared_ptr<CBglTile> Clone() const;
	const SBglTilePointer& Pointer() const;
	bool UpdateTilePointer(BinaryFileStream& out, const CPackedQmid& qmid);
	EBglLayerType Type() const;
	bool ReadBinary(BinaryFileStream& in);
	bool WriteBinary(BinaryFileStream& out);
	int CalculateDataSize() const;
	int GetRecordCount() const;
	template <typename T>
	T* GetTileDataAt(int index) const;

private:
	std::vector<std::unique_ptr<IBglSerializable>> m_data;
	stlab::copy_on_write<SBglTilePointer> m_tile_pointer;
	EBglLayerType m_type;
};


class FLIGHTSIMLIB_EXPORTED CBglLayer
{
public:
	explicit CBglLayer(EBglLayerType type, const SBglLayerPointer& data);
	explicit CBglLayer(CPackedQmid qmid, std::shared_ptr<CBglTile> tile);
	virtual ~CBglLayer() = default;
	CBglLayer(const CBglLayer& other);

	bool AddTile(CPackedQmid qmid, std::shared_ptr<CBglTile> tile);
	CBglTile& operator[](const CPackedQmid index);
	static std::unique_ptr<CBglLayer> ReadBinary(
		BinaryFileStream& in,                                      
		const std::map<EBglLayerType, std::unique_ptr<CBglLayer>>& layers);
	int CalculateLayerSize() const;
	int CalculateTilePointersSize() const;
	void UpdateLayerPointer(int offset_to_tile);
	bool WriteBinaryLayerPointer(BinaryFileStream& out, int offset_to_tile);;
	bool WriteBinaryLayerTiles(BinaryFileStream& out);
	bool WriteBinaryTilePointers(BinaryFileStream& out);
	int Offset() const;
	EBglLayerType Type() const;
	const std::map<CPackedQmid, std::shared_ptr<CBglTile>>& Tiles() const;

	static bool IsTrq1BglLayer(EBglLayerType layer_type);
	static bool IsRcs1BglLayer(EBglLayerType layer_type);

	static constexpr int CalculateLayerPointerSize()
	{
		return 20;
	}

private:
	EBglLayerType m_type;
	std::map<CPackedQmid, std::shared_ptr<CBglTile>> m_tiles;
	stlab::copy_on_write<SBglLayerPointer> m_data;
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

class FLIGHTSIMLIB_EXPORTED CBglFile final
{
public:
	CBglFile();
	explicit CBglFile(std::wstring file_name);
	~CBglFile() = default;
	
	// TODO - This is now move-only, not copyable
	// With the bgl document model, a copy should be deep and copy-on-write
	// to provide value semantics. It remains to be seen what we do with
	// the backing stream in this case.
	CBglFile(const CBglFile&) = delete;
	CBglFile& operator= (const CBglFile&) = delete;
	CBglFile(CBglFile&&) = default;
	CBglFile& operator=(CBglFile&&) = default;

	bool Open();
	bool Close();
	bool Write();
	bool Read();
	const std::wstring& GetFileName() const;
	void Rename(std::wstring file_name);
	bool TryMergeLayer(std::unique_ptr<CBglLayer>&& layer);
	std::unique_ptr<CBglLayer> RemoveLayer(EBglLayerType type);
	bool IsDirty() const;
	int GetFileSize() const;
	CBglLayer* GetLayer(EBglLayerType type) const;

	// Test method!
	std::vector<const IBglExclusion*> GetExclusions();

private:
	bool ReadAllLayers();
	bool WriteAllLayers();
	bool ReadHeader();
	bool WriteHeader();
	bool BuildHeader();
	bool ComputeHeaderQmids();

private:
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
	std::map<EBglLayerType, std::unique_ptr<CBglLayer>> m_layers;
	BinaryFileStream m_stream;
};

} // namespace io

} // namespace flightsimlib

#endif