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

#ifndef FLIGHTSIMLIB_IO_BGLDATA_H
#define FLIGHTSIMLIB_IO_BGLDATA_H

#include "BglTypes.h"
#include "../external/stlab/copy_on_write.hpp"

#include <cstdint>
#include <memory>
#include <vector>


namespace flightsimlib
{
	
namespace io
{

//******************************************************************************
// IBglSerializable
//******************************************************************************  


// Forward Declarations
class BinaryFileStream;


class IBglSerializable
{
public:
	virtual ~IBglSerializable() = default;
	virtual void ReadBinary(BinaryFileStream& in) = 0;
	virtual void WriteBinary(BinaryFileStream& out) = 0;
	virtual bool Validate() = 0;
	virtual int CalculateSize() const = 0;
};


//******************************************************************************
// Common
//******************************************************************************  

#pragma pack(push)
#pragma pack(1)

struct SBglVertexLL
{
	uint32_t Longitude;
	uint32_t Latitude;
};
	
#pragma pack(pop)

// TODO: Move to Utility

class PackedAltitude
{
public:
	PackedAltitude(uint32_t value) : m_Value(value) {}

	static double Value(uint32_t value)
	{
		return value / 1000.0;
	}

	static uint32_t FromDouble(double value)
	{
		return static_cast<uint32_t>(value * 1000.0);
	}

	double Value() const
	{
		return Value(m_Value);
	}

private:
	uint32_t m_Value;
};

class ANGLE16
{
public:
	ANGLE16(uint16_t value) : m_Value(value){}

	static double Value(uint16_t value)
	{
		return value * 360.0 / 0x10000;
	}

	static uint16_t FromDouble(double value)
	{
		return static_cast<uint16_t>(0x10000 / 360.0 * value);
	}
	
	double Value() const
	{
		return Value(m_Value);
	}
	
private:
	uint16_t m_Value;
};


class Latitude
{
public:
	Latitude(uint32_t value) : m_Value(Value(value)) {}

	Latitude(double value) : m_Value(value) {}

	static double Value(uint32_t value)
	{
		return 90.0 - value * (180.0 / 0x20000000);
	}

	static uint32_t ToPacked(double value)
	{
		return static_cast<uint32_t>((90.0 - value) / (180.0 / 0x20000000));
	}

	double Value() const
	{
		return m_Value;
	}

	uint32_t ToPacked() const
	{
		return ToPacked(m_Value);
	}
	
	double m_Value;
};


class Longitude
{
public:
	Longitude(uint32_t value) : m_Value(Value(value)) {}

	Longitude(double value) : m_Value(value) {}

	static double Value(uint32_t value)
	{
		return (value * (360.0 / 0x30000000)) - 180.0;
	}

	static uint32_t ToPacked(double value)
	{
		return static_cast<uint32_t>((180.0 + value) / (360.0 / 0x30000000));
	}

	double Value() const
	{
		return m_Value;
	}

	uint32_t ToPacked() const
	{
		return ToPacked(m_Value);
	}

	double m_Value;
};
	
	
//******************************************************************************
// CBglRunway
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglRunwayData
{
	uint16_t Type;
	uint32_t Size;
	uint16_t SurfaceType;
	uint8_t NumberPrimary;
	uint8_t DesignatorPrimary;
	uint8_t NumberSecondary;
	uint8_t DesignatorSecondary;
	uint32_t IlsIcaoPrimary;
	uint32_t IlsIcaoSecondary;
	uint32_t Longitude;
	uint32_t Latitude;
	uint32_t Altitude;
	float Length;
	float Width;
	float Heading;
	float PatternAltitude;
	uint16_t MarkingFlags;
	uint8_t LightingFlags;
	uint8_t PatternFlags;
};

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
	
struct SBglRunwayPadData
{
	uint16_t Type;
	uint32_t Size;
	uint16_t SurfaceType;
	float Length;
	float Width;
};

#pragma pack(pop)
	
class CBglRunway final : public IBglSerializable, public IBglRunway
{
public:
	class CBglRunwayOffsetThreshold final : public IBglSerializable, public IBglRunwayOffsetThreshold
	{
	public:
		void ReadBinary(BinaryFileStream& in) override;
		void WriteBinary(BinaryFileStream& out) override;
		bool Validate() override;
		int CalculateSize() const override;
		
		ESurfaceType GetSurfaceType() override;
		void SetSurfaceType(ESurfaceType value) override;
		float GetLength() const override;
		void SetLength(float value) override;
		float GetWidth() const override;
		void SetWidth(float value) override;

		// public but not part of client interface:
		bool IsEmpty();
		void Clone(CBglRunwayOffsetThreshold& value);

	private:
		stlab::copy_on_write<SBglRunwayPadData> m_data;
	};
	
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	double GetLongitude() const override;
	void SetLongitude(double value) override;
	double GetLatitude() const override;
	void SetLatitude(double value) override;
	double GetAltitude() const override;
	void SetAltitude(double value) override;
	
	float GetLength() const override;
	void SetLength(float value) override;
	float GetWidth() const override;
	void SetWidth(float value) override;
	float GetHeading() const override;
	void SetHeading(float value) override;
	float GetPatternAltitude() const override;
	void SetPatternAltitude(float value) override;

	const IBglRunwayOffsetThreshold* GetPrimaryOffsetThreshold() override;
	void SetPrimaryOffsetThreshold(IBglRunwayOffsetThreshold* value) override;
	const IBglRunwayOffsetThreshold* GetSecondaryOffsetThreshold() override;
	void SetSecondaryOffsetThreshold(IBglRunwayOffsetThreshold* value) override;
	
private:
	stlab::copy_on_write<SBglRunwayData> m_data;

	CBglRunwayOffsetThreshold m_primary_offset_threshold;
	CBglRunwayOffsetThreshold m_secondary_offset_threshold;
};


//******************************************************************************
// CBglAirport
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglAirportData
{
	uint16_t Type;
	uint32_t Size;
	uint8_t RunwayCount;
	uint8_t FrequencyCount;
	uint8_t StartCount;
	uint8_t ApproachCount;
	uint8_t ApronCount;
	uint8_t HelipadCount;
	uint32_t ReferenceLon;
	uint32_t ReferenceLat;
	uint32_t ReferenceAlt;
	uint32_t TowerLongitude;
	uint32_t TowerLatitude;
	uint32_t TowerAltitude;
	float MagVar;
	uint32_t Icao;
	uint32_t RegionIdent;
	uint32_t FuelTypes;
	uint32_t Flags;
};

#pragma pack(pop)


class CBglAirport final : public IBglSerializable, public IBglAirport
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	float GetMagVar() const override;
	void SetMagVar(float value) override;

private:
	std::vector<CBglRunway> m_runways;
	stlab::copy_on_write<SBglAirportData> m_data;
};


//******************************************************************************
// CBglExclusion
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

// Exclusions are always 0,0 QMID!
struct SBglExclusionData
{
	uint16_t Type;
	uint16_t Size;
	uint32_t MinLongitude;
	uint32_t MinLatitude;
	uint32_t MaxLongitude;
	uint32_t MaxLatitude;
};

#pragma pack(pop)


class CBglExclusion final : public IBglSerializable, public IBglExclusion
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	double GetMinLongitude() const override;
	void SetMinLongitude(double value) override;
	double GetMaxLongitude() const override;
	void SetMaxLongitude(double value) override;
	double GetMinLatitude() const override;
	void SetMinLatitude(double value) override;
	double GetMaxLatitude() const override;
	void SetMaxLatitude(double value)  override;

	bool IsExcludeAll() const override;
	void SetExcludeAll(bool value) override;
	bool IsGenericBuilding() const override;
	void SetGenericBuilding(bool value) override;

private:
	enum EType : uint16_t {
		None = 0,
		All = 1 << 3,
		Beacon = 1 << 4,
		Effect = 1 << 5,
		GenericBuilding = 1 << 6,
		LibraryObject = 1 << 7,
		TaxiwaySign = 1 << 8,
		Trigger = 1 << 9,
		Windsock = 1 << 10,
		ExtrusionBridge = 1 << 11,
	};

	stlab::copy_on_write<SBglExclusionData> m_data;
};

//******************************************************************************
// CBglMarker
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglMarkerData
{
	uint16_t SectionType;
	uint16_t Size;
	uint8_t UnusedType;
	uint16_t Heading;
	uint8_t MarkerType;
	uint32_t Longitude;
	uint32_t Latitude;
	uint32_t Altitude;
	uint32_t Identifier;
	uint16_t Region;
	uint16_t Unknown;
};

#pragma pack(pop)


class CBglMarker final : public IBglSerializable, public IBglMarker
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	float GetHeading() const override;
	void SetHeading(float value) override;

private:
	enum EType : uint8_t {
		None = 0,
		All = 1,
		Beacon = 2,
		Effect = 3
	};

	stlab::copy_on_write<SBglMarkerData> m_data;
};

//******************************************************************************
// CBglGeopol
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglGeopolData
{
	uint16_t SectionType;
	uint32_t Size;
	uint16_t GeopolType;
	uint32_t MinLongitude;
	uint32_t MinLatitude;
	uint32_t MaxLongitude;
	uint32_t MaxLatitude;
	SBglVertexLL* Vertices;
};

#pragma pack(pop)

// TODO - Need mem management constructors around Vertices array
class CBglGeopol final : public IBglSerializable, public IBglGeopol
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	double GetMinLongitude() const override;
	void SetMinLongitude(double value) override;
	double GetMaxLongitude() const override;
	void SetMaxLongitude(double value) override;
	double GetMinLatitude() const override;
	void SetMinLatitude(double value) override;
	double GetMaxLatitude() const override;
	void SetMaxLatitude(double value) override;

	EType GetGeopolType() const override;
	void SetGeopolType(EType value) override;
	int GetNumVertices() const override;

private:
	void SetNumVertices(int value) override;
	
	stlab::copy_on_write<SBglGeopolData> m_data;
};

//******************************************************************************
// CBglSceneryObject
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglSceneryObjectData
{
	uint16_t SectionType;
	uint16_t Size;
	uint32_t Longitude;
	uint32_t Latitude;
	uint32_t Altitude;
	uint16_t Flags;
	uint16_t Pitch;
	uint16_t Bank;
	uint16_t Heading;
	uint16_t ImageComplexity;
};

#pragma pack(pop)


class CBglSceneryObject : public IBglSerializable, public IBglSceneryObject
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	double GetLongitude() const override;
	void SetLongitude(double value) override;
	double GetLatitude() const override;
	void SetLatitude(double value) override;
	double GetAltitude() const override;
	void SetAltitude(double value) override;
	bool IsAboveAgl() const override;
	void SetAboveAgl(bool value) override;
	bool IsNoAutogenSuppression() const override;
	void SetNoAutogenSuppression(bool value) override;
	bool IsNoCrash() const override;
	void SetNoCrash(bool value) override;
	bool IsNoFog() const override;
	void SetNoFog(bool value) override;
	bool IsNoShadow() const override;
	void SetNoShadow(bool value) override;
	bool IsNoZWrite() const override;
	void SetNoZWrite(bool value) override;
	bool IsNoZTest() const override;
	void SetNoZTest(bool value) override;
	float GetPitch() const override;
	void SetPitch(float value) override;
	float GetBank() const override;
	void SetBank(float value) override;
	float GetHeading() const override;
	void SetHeading(float value) override;
	EImageComplexity GetImageComplexity() const override;
	void SetImageComplexity(EImageComplexity value) override;

private:
	enum class Flags : uint16_t
	{
		None = 0,
		AboveAgl = 1 << 0,
		NoAutogenSuppression = 1 << 1,
		NoCrash = 1 << 2,
		NoFog = 1 << 3,
		NoShadow = 1 << 4,
		NoZWrite = 1 << 5,
		NoZTest = 1 << 6
	};

	stlab::copy_on_write<SBglSceneryObjectData> m_data;
};


//******************************************************************************
// CBglLibraryObject
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglLibraryObjectData
{
	_GUID InstanceId;
	_GUID Name;
	float Scale;
};

#pragma pack(pop)


class CBglLibraryObject : public CBglSceneryObject, public IBglLibraryObject
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	_GUID GetName() const override;
	void SetName(_GUID value) override;
	float GetScale() const override;
	void SetScale(float value) override;

private:
	stlab::copy_on_write<SBglLibraryObjectData> m_data;
};


//******************************************************************************
// CBglWindsock
//****************************************************************************** 


#pragma pack(push)
#pragma pack(1)

struct SBglWindsockData
{
	_GUID InstanceId;
	float PoleHeight;
	float WindsockLength;
	uint32_t PoleColor;
	uint32_t SockColor;
	uint16_t Lighted;
};

#pragma pack(pop)


class CBglWindsock : public CBglSceneryObject, public IBglWindsock
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	_GUID GetInstanceId() const override;
	void SetInstanceId(_GUID value) override;
	float GetPoleHeight() const override;
	void SetPoleHeight(float value) override;
	float GetSockLength() const override;
	void SetSockLength(float value) override;
	uint32_t GetPoleColor() const override;
	void SetPoleColor(uint32_t value) override;
	uint32_t GetSockColor() const override;
	void SetSockColor(uint32_t value) override;
	bool IsLighted() const override;
	void SetLighted(bool value) override;

private:
	stlab::copy_on_write<SBglWindsockData> m_data;
};


//******************************************************************************
// CTerrainRasterQuad1
//******************************************************************************  


enum class ERasterCompressionType : uint8_t
{
	None = 0,
	Lz1 = 1,
	Delta = 2,
	DeltaLz1 = 3,
	Lz2 = 4,
	DeltaLz2 = 5,
	BitPack = 6,
	BitPackLz1 = 7,
	SolidBlock = 8,
	BitPackLz2 = 9,
	Ptc = 10,
	Dxt1 = 11,
	Dxt3 = 12,
	Dxt5 = 13,
	Max = 14
};


enum class ERasterDataType : uint8_t
{
	None = 0,
	Photo = 1,
	Elevation = 2,
	LandClass = 3,
	WaterClass = 4,
	Region = 5,
	Season = 6,
	PopulationDensity = 7,
	Reserved = 8,
	TerrainIndex = 9,
	ModifiedElevation = 10,
	PhotoFlight = 11,
	Max = 12
};


#pragma pack(push)
#pragma pack(1)

// Exclusions are always 0,0 QMID!
struct SBglTerrainRasterQuad1Data
{
	uint32_t Version;
	uint32_t Size;
	ERasterDataType DataType;
	ERasterCompressionType CompressionTypeData;
	ERasterCompressionType CompressionTypeMask;
	uint32_t QmidLow;
	uint32_t QmidHigh;
	uint16_t Variations;
	uint32_t Rows;
	uint32_t Cols;
	uint32_t SizeData;
	uint32_t SizeMask;
};

#pragma pack(pop)


// Out-Of-Core compressed raster block
class CRasterBlock
{
public:
	int DataOffset;
	int DataLength;
	int MaskOffset;
	int MaskLength;

	std::shared_ptr<uint8_t[]> GetCompressedData();
};


class CTerrainRasterQuad1 : public IBglSerializable,  ITerrainRasterQuad1
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	int Rows() const override;
	int Cols() const override;
	ERasterDataType GetDataType() const
	{
		return m_header->DataType;
	}

private:

	int GetBpp() const
	{
		switch (GetDataType())
		{
		case ERasterDataType::PopulationDensity:
		case ERasterDataType::TerrainIndex:
		case ERasterDataType::LandClass:
		case ERasterDataType::Region:
		case ERasterDataType::Season:
		case ERasterDataType::WaterClass:
			return 1;

		case ERasterDataType::Elevation:
		case ERasterDataType::ModifiedElevation:
		case ERasterDataType::Photo:
			return 2;
		case ERasterDataType::PhotoFlight:
			return 4;
		default:
			return 0;
		}
	}
	
	bool GetImageFormat(int& bit_depth, int& num_channels) const
	{
		switch (GetDataType())
		{
		case ERasterDataType::LandClass:
		case ERasterDataType::WaterClass:
		case ERasterDataType::Region:
		case ERasterDataType::Season:
		case ERasterDataType::PopulationDensity:
			bit_depth = 8;
			num_channels = 1;
			break;
		case ERasterDataType::Photo:
			bit_depth = 16;
			num_channels = 4;
			break;
		case ERasterDataType::PhotoFlight:
			bit_depth = 32;
			num_channels = 4;
			break;
		case ERasterDataType::TerrainIndex:
		case ERasterDataType::Elevation:  // 32 is final float, but 16 compressed
		case ERasterDataType::ModifiedElevation:
			//bit_depth = 32;
			bit_depth = 16;
			num_channels = 1;
			break;
		default:
			bit_depth = 0;
			num_channels = 0;
			return false;
		}
		return true;
	}

	int CalculateLength(bool mask) const
	{
		if (mask)
		{
			return static_cast<int>(sizeof(uint8_t)) * Cols() * Rows();
		}

		return  GetBpp() * Cols() * Rows();
	}
	
	std::shared_ptr<uint8_t[]>DecompressData(ERasterCompressionType compression_type, int uncompressed_size);
	
	stlab::copy_on_write<SBglTerrainRasterQuad1Data> m_header;
	stlab::copy_on_write<CRasterBlock> m_data;
};

} // namespace io
	
} // namespace flightsimlib

#endif
