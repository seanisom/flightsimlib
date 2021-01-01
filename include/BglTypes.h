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

// TODO: We need a cross platform library for this
#include <guiddef.h>

#ifndef FLIGHTSIMLIB_IO_BGLTYPES_H
#define FLIGHTSIMLIB_IO_BGLTYPES_H

namespace flightsimlib
{

namespace io
{

enum class ESurfaceType : uint16_t
{
	CONCRETE = 0x0000,
	GRASS = 0x0001,
	WATER = 0x0002,
	ASPHALT = 0x0004,
	CLAY = 0x0007,
	SNOW = 0x0008,
	ICE = 0x0009,
	DIRT = 0x000C,
	CORAL = 0x000D,
	GRAVEL = 0x000E,
	OIL_TREATED = 0x000F,
	STEEL_MATS = 0x0010,
	BITUMINOUS = 0x0011,
	BRICK = 0x0012,
	MACADAM = 0x0013,
	PLANKS = 0x0014,
	SAND = 0x0015,
	SHALE = 0x0016,
	TARMAC = 0x0017,
	UNKNOWN = 0x00FE
};
	
class IBglRunwayOffsetThreshold
{
public:
	virtual ESurfaceType GetSurfaceType() = 0;
	virtual void SetSurfaceType(ESurfaceType value) = 0;
	virtual float GetLength() const = 0;
	virtual void SetLength(float value) = 0;
	virtual float GetWidth() const = 0;
	virtual void SetWidth(float value) = 0;
};

class IBglRunway
{
public:

	enum class ERunwayNumber : uint8_t
	{
		Runway1 = 1,
		Runway2 = 2,
		Runway3 = 3,
		Runway4 = 4,
		Runway5 = 5,
		Runway6 = 6,
		Runway7 = 7,
		Runway8 = 8,
		Runway9 = 9,
		Runway10 = 10,
		Runway11 = 11,
		Runway12 = 12,
		Runway13 = 13,
		Runway14 = 14,
		Runway15 = 15,
		Runway16 = 16,
		Runway17 = 17,
		Runway18 = 18,
		Runway19 = 19,
		Runway20 = 20,
		Runway21 = 21,
		Runway22 = 22,
		Runway23 = 23,
		Runway24 = 24,
		Runway25 = 25,
		Runway26 = 26,
		Runway27 = 27,
		Runway28 = 28,
		Runway29 = 29,
		Runway30 = 30,
		Runway31 = 31,
		Runway32 = 32,
		Runway33 = 33,
		Runway34 = 34,
		Runway35 = 35,
		Runway36 = 36,
		RunwayN = 37,
		RunwayNE = 38,
		RunwayE = 39,
		RunwaySE = 40,
		RunwayS = 41,
		RunwaySW = 42,
		RunwayW = 43,
		RunwayNW = 44
	};

	enum class ERunwayDesignator : uint8_t
	{
		None = 0,
		Left = 1,
		Right = 2,
		Center = 3,
		Water = 4,
		A = 5,
		B = 6
	};

	enum EMarkingFlags : uint16_t {
		None = 0,
		Edges = 1 << 0,
		Threshold = 1 << 1,
		FixedDistance = 1 << 2,
		Touchdown = 1 << 3,
		Dashes = 1 << 4,
		Ident = 1 << 5,
		Precision = 1 << 6,
		EdgePavement = 1 << 7,
		SingleEnd = 1 << 8,
		PrimaryClosed = 1 << 9,
		SecondaryClosed = 1 << 10,
		PrimaryStol = 1 << 11,
		SecondaryStol = 1 << 12,
		AlternateThreshold = 1 << 13,
		AlternateFixedThreshold = 1 << 14,
		AlternateTouchdown = 1 << 15,
	};
	
	enum class ELightFlags : uint8_t
	{
		None = 0,
		EdgeLow = 1 << 0,
		EdgeMedium = 1 << 1,
		EdgeHigh = (1 << 0) & (1 << 1),
		CenterLow = 1 << 2,
		CenterMedium = 1 << 3,
		CenterHigh = (1 << 2) & (1 << 3),
		CenterRed = 1 << 4,
		AlternatePrecision = 1 << 5,
		Leading0Ident = 1 << 6,
		NoThresholdEndArrows = 1 << 7
	};

	enum class EPatternFlags : uint8_t
	{
		None = 0,
		PrimaryTakeoff = 1 << 0,
		PrimaryLanding = 1 << 1,
		PrimaryPattern = 1 << 2,
		SecondaryTakeoff = 1 << 3,
		SecondaryLanding = 1 << 4,
		SecondaryPattern = 1 << 5,
	};
	
public:
	virtual double GetLongitude() const = 0;
	virtual void SetLongitude(double value) = 0;
	virtual double GetLatitude() const = 0;
	virtual void SetLatitude(double value) = 0;
	virtual double GetAltitude() const = 0;
	virtual void SetAltitude(double value) = 0;
	
	virtual float GetLength() const = 0;
	virtual void SetLength(float value) = 0;
	virtual float GetWidth() const = 0;
	virtual void SetWidth(float value) = 0;
	virtual float GetHeading() const = 0;
	virtual void SetHeading(float value) = 0;
	virtual float GetPatternAltitude() const = 0;
	virtual void SetPatternAltitude(float value) = 0;

	virtual const IBglRunwayOffsetThreshold* GetPrimaryOffsetThreshold() = 0;
	virtual void SetPrimaryOffsetThreshold(IBglRunwayOffsetThreshold* value) = 0;
	virtual const IBglRunwayOffsetThreshold* GetSecondaryOffsetThreshold() = 0;
	virtual void SetSecondaryOffsetThreshold(IBglRunwayOffsetThreshold* value) = 0;
};


class IBglAirport
{
public:
	virtual float GetMagVar() const = 0;
	virtual void SetMagVar(float value) = 0;
};


class IBglExclusion
{
public:
	virtual double GetMinLongitude() const = 0;
	virtual void SetMinLongitude(double value) = 0;
	virtual double GetMaxLongitude() const = 0;
	virtual void SetMaxLongitude(double value) = 0;
	virtual double GetMinLatitude() const = 0;
	virtual void SetMinLatitude(double value) = 0;
	virtual double GetMaxLatitude() const = 0;
	virtual void SetMaxLatitude(double value) = 0;

	virtual bool IsExcludeAll() const = 0;
	virtual void SetExcludeAll(bool value) = 0;
	virtual bool IsGenericBuilding() const = 0;
	virtual void SetGenericBuilding(bool value) = 0;
};

class IBglMarker
{
public:
	virtual float GetHeading() const = 0;
	virtual void SetHeading(float value) = 0;
};

class IBglGeopol
{
public:
	virtual double GetMinLongitude() const = 0;
	virtual void SetMinLongitude(double value) = 0;
	virtual double GetMaxLongitude() const = 0;
	virtual void SetMaxLongitude(double value) = 0;
	virtual double GetMinLatitude() const = 0;
	virtual void SetMinLatitude(double value) = 0;
	virtual double GetMaxLatitude() const = 0;
	virtual void SetMaxLatitude(double value) = 0;

	enum class EType : uint8_t {
		None = 0,
		Coastline = 64,
		BOUNDARY = 128,
		DASHED_BOUNDARY = 129, // 0x00000081
	};

	virtual EType GetGeopolType() const = 0;
	virtual void SetGeopolType(EType value) = 0;
	virtual int GetNumVertices() const = 0;

private:
	virtual void SetNumVertices(int value) = 0;
	// TODO - need Setter with realloc - need a better interface with this
};

class ITerrainRasterQuad1
{
public:
	virtual int Rows() const = 0;
	virtual int Cols() const = 0;
};

class IModelData
{
	virtual void* Data() = 0;
	virtual int Length() = 0;
};


class IBglSceneryObject
{
public:
	enum class EImageComplexity : uint32_t
	{
		VerySparse = 0,
		Sparse = 1,
		Normal = 2,
		Dense = 3,
		VeryDense = 4
	};

	virtual double GetLongitude() const = 0;
	virtual void SetLongitude(double value) = 0;
	virtual double GetLatitude() const = 0;
	virtual void SetLatitude(double value) = 0;
	virtual double GetAltitude() const = 0;
	virtual void SetAltitude(double value) = 0;
	virtual bool IsAboveAgl() const = 0;
	virtual void SetAboveAgl(bool value) = 0;
	virtual bool IsNoAutogenSuppression() const = 0;
	virtual void SetNoAutogenSuppression(bool value) = 0;
	virtual bool IsNoCrash() const = 0;
	virtual void SetNoCrash(bool value) = 0;
	virtual bool IsNoFog() const = 0;
	virtual void SetNoFog(bool value) = 0;
	virtual bool IsNoShadow() const = 0;
	virtual void SetNoShadow(bool value) = 0;
	virtual bool IsNoZWrite() const = 0;
	virtual void SetNoZWrite(bool value) = 0;
	virtual bool IsNoZTest() const = 0;
	virtual void SetNoZTest(bool value) = 0;
	virtual float GetPitch() const = 0;
	virtual void SetPitch(float value) = 0;
	virtual float GetBank() const = 0;
	virtual void SetBank(float value) = 0;
	virtual float GetHeading() const = 0;
	virtual void SetHeading(float value) = 0;
	virtual EImageComplexity GetImageComplexity() const = 0;
	virtual void SetImageComplexity(EImageComplexity value) = 0;
};


class IBglLibraryObject : virtual public IBglSceneryObject
{
public:
	virtual _GUID GetName() const = 0;
	virtual void SetName(_GUID value) = 0;
	virtual float GetScale() const = 0;
	virtual void SetScale(float value) = 0;
};


class IBglEffect : virtual public IBglSceneryObject
{
public:
	virtual const char* GetName() const = 0;
	virtual void SetName(const char* value) = 0;
	virtual const char* GetParams() const = 0;
	virtual void SetParams(const char* value) = 0;
};


class IBglWindsock : virtual public IBglSceneryObject
{
public:
	virtual _GUID GetInstanceId() const = 0;
	virtual void SetInstanceId(_GUID value) = 0;
	virtual float GetPoleHeight() const = 0;
	virtual void SetPoleHeight(float value) = 0;
	virtual float GetSockLength() const = 0;
	virtual void SetSockLength(float value) = 0;
	virtual uint32_t GetPoleColor() const = 0;
	virtual void SetPoleColor(uint32_t value) = 0;
	virtual uint32_t GetSockColor() const = 0;
	virtual void SetSockColor(uint32_t value) = 0;
	virtual bool IsLighted() const = 0;
	virtual void SetLighted(bool value) = 0;
};

}

}

#endif