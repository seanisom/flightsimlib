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
	uint32_t Latitude;
	uint32_t Longitude;
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


class CBglRunway final : public IBglSerializable, public IBglRunway
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	float GetLength() const override;
	float GetWidth() const override;
	float GetHeading() const override;
	float GetPatternAltitude() const override;
	
private:
	stlab::copy_on_write<SBglRunwayData> m_data;
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
	uint32_t ReferenceLat;
	uint32_t ReferenceLon;
	uint32_t ReferenceAlt;
	uint32_t TowerLatitude;
	uint32_t TowerLongitude;
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
	uint32_t LonWest;
	uint32_t LatNorth;
	uint32_t LonEast;
	uint32_t LatSouth;
};

#pragma pack(pop)


class CBglExclusion final : public IBglSerializable, public IBglExclusion
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

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
// CTerrainRasterQuad1
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

// Exclusions are always 0,0 QMID!
struct SBglTerrainRasterQuad1Data
{
	uint32_t Version;
	uint32_t Size;
	uint16_t DataType;
	uint8_t CompressionTypeData;
	uint8_t CompressionTypeMask;
	uint32_t QmidLow;
	uint32_t QmidHigh;
	uint32_t Variations;
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

private:
	std::shared_ptr<uint8_t[]>DecompressData(uint8_t compression_type, int UncompressedLength);
	
	stlab::copy_on_write<SBglTerrainRasterQuad1Data> m_header;
	stlab::copy_on_write<CRasterBlock> m_data;
};

} // namespace io
	
} // namespace flightsimlib

#endif
