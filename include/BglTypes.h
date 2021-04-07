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

#ifndef FLIGHTSIMLIB_IO_BGLTYPES_H
#define FLIGHTSIMLIB_IO_BGLTYPES_H

// TODO: We need a cross platform library for this
#include <cstdint>
#include <guiddef.h>

namespace flightsimlib
{

namespace io
{

class IBglSceneryObject;
	
enum class ESurfaceType : uint16_t
{ 
	Concrete = 0, // There are missing values, find them?
	Grass = 1,
	Water = 2,
	Asphalt = 4,
	Clay = 7,
	Snow = 8,
	Ice = 9,
	Dirt = 0xC,
	Coral = 0xD,
	Gravel = 0xE,
	OilTreated = 0xF,
	SteelMats = 0x10,
	Bituminous = 0x11,
	Brick = 0x12,
	Macadam = 0x13,
	Planks = 0x14,
	Sand = 0x15,
	Shale = 0x16,
	Tarmac = 0x17,
	Unknown = 0xFE
};


#pragma pack(push)
#pragma pack(1)

struct SBglVertexLL
{
	uint32_t Longitude;
	uint32_t Latitude;
};

#pragma pack(pop)

	
#pragma pack(push)
#pragma pack(1)

struct SBglVertexLLA
{
	uint32_t Longitude;
	uint32_t Latitude;
	uint32_t Altitude;
};

#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)

struct SBglVertexBias
{
	float X;
	float Z;
};

#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)

struct SBglEdge
{
	float Spacing;
	uint16_t Start;
	uint16_t End;
};

#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)

struct SBglIndex
{
	uint16_t I0;
	uint16_t I1;
	uint16_t I2;
};

#pragma pack(pop)


class IBglTerrainVectorDb
{
public:
	
};
	
class IBglTerrainElevation
{
public:

};

class IBglTerrainLandClass
{
public:

};

class IBglTerrainWaterClass
{
public:

};

class IBglTerrainRegion
{
public:

};

class IBglPopulationDensity
{
public:

};

class IBglAutogenAnnotation
{
public:

};

class IBglTerrainIndex
{
public:

};

class IBglTerrainTextureLookup
{
public:

};

class IBglTerrainSeason
{
public:

};

class IBglTerrainPhoto
{
public:

};

class IBglTerrainPhoto32
{
public:

};


	
class IBglFuelAvailability
{
public:

	enum class EFuelAvailability
	{
		No = 0,
		Unknown = 1,
		PriorRequest = 2,
		Yes = 3
	};

	// TODO - There is an "Unknown" type in the XML compiler
	virtual EFuelAvailability Get73Octane() const = 0;
	virtual void Set73Octane(EFuelAvailability value) = 0;
	virtual EFuelAvailability Get87Octane() const = 0;
	virtual void Set87Octane(EFuelAvailability value) = 0;
	virtual EFuelAvailability Get100Octane() const = 0;
	virtual void Set100Octane(EFuelAvailability value) = 0;
	virtual EFuelAvailability Get130Octane() const = 0;
	virtual void Set130Octane(EFuelAvailability value) = 0;
	virtual EFuelAvailability Get145Octane() const = 0;
	virtual void Set145Octane(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetMogas() const = 0;
	virtual void SetMogas(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetJet() const = 0;
	virtual void SetJet(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetJetA() const = 0;
	virtual void SetJetA(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetJetA1() const = 0;
	virtual void SetJetA1(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetJetAP() const = 0;
	virtual void SetJetAP(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetJetB() const = 0;
	virtual void SetJetB(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetJet4() const = 0;
	virtual void SetJet4(EFuelAvailability value) = 0;
	virtual EFuelAvailability GetJet5() const = 0;
	virtual void SetJet5(EFuelAvailability value) = 0;
	virtual bool HasAvgas() const = 0;
	virtual bool HasJetFuel() const = 0;
};


class IBglName
{
public:
	virtual auto GetName() const -> const char* = 0;
	virtual auto SetName(const char* value) -> void = 0;
};


class IBglLLA
{
public:
	virtual auto GetLongitude() const -> double = 0;
	virtual auto SetLongitude(double value) -> void = 0;
	virtual auto GetLatitude() const -> double = 0;
	virtual auto SetLatitude(double value) -> void = 0;
	virtual auto GetAltitude() const -> double = 0;
	virtual auto SetAltitude(double value) -> void = 0;
};


class IBglNdb : virtual public IBglLLA, virtual public IBglName
{
public:
	enum class EType : uint16_t
	{
		CompassPoint = 0,
		MH = 1,
		H = 2,
		HH = 3
	};

	virtual auto GetType() const->EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetFrequency() const -> uint32_t = 0;
	virtual auto SetFrequency(uint32_t value) -> void = 0;
	virtual auto GetRange() const -> float = 0;
	virtual auto SetRange(float value) -> void = 0;
	virtual auto GetMagVar() const -> float = 0;
	virtual auto SetMagVar(float value) -> void = 0;
	virtual auto GetIcao() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcao(uint32_t value) -> void = 0;
	virtual auto GetRegion() const->uint32_t = 0;
	virtual auto SetRegion(uint32_t value) -> void = 0;
};


class IBglRunwayEnd
{
public:
	enum class EPosition : uint16_t
	{
		PrimaryOffsetThreshold = 0x5,
		SecondaryOffsetThreshold = 0x6,
		PrimaryBlastPad = 0x7,
		SecondaryBlastPad = 0x8,
		PrimaryOverrun = 0x9,
		SecondaryOverrun = 0xA
	};
	
	virtual auto GetPosition() const -> EPosition = 0;
	virtual auto GetSurfaceType() const -> ESurfaceType = 0;
	virtual auto SetSurfaceType(ESurfaceType value) -> void = 0;
	virtual auto GetLength() const -> float = 0;
	virtual auto SetLength(float value) -> void = 0;
	virtual auto GetWidth() const -> float = 0;
	virtual auto SetWidth(float value) -> void = 0;
};


class IBglRunwayVasi
{
public:
	enum class EPosition : uint16_t
	{
		PrimaryLeftVasi = 0xB,
		PrimaryRightVasi = 0xC,
		SecondaryLeftVasi = 0xD,
		SecondaryRightVasi = 0xE
	};

	enum class EType : uint16_t
	{
		None = 0, // invalid
		Vasi21 = 1,
		Vasi31 = 2,
		Vasi22 = 3,
		Vasi32 = 4,
		Vasi23 = 5,
		Vasi33 = 6,
		Papi2 = 7,
		Papi4 = 8,
		Tricolor = 9,
		PVasi = 10,
		TVasi = 11,
		Ball = 12,
		Apap = 13
		// Panels remapped to Papi2
	};
	
	virtual auto GetPosition() const -> EPosition = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetBiasX() const -> float = 0;
	virtual auto SetBiasX(float value) -> void = 0;
	virtual auto GetBiasZ() const -> float = 0;
	virtual auto SetBiasZ(float value) -> void = 0;
	virtual auto GetSpacing() const -> float = 0;
	virtual auto SetSpacing(float value) -> void = 0;
	virtual auto GetPitch() const -> float = 0;
	virtual auto SetPitch(float value) -> void = 0;
};


class IBglRunwayApproachLights
{
public:
	enum class EPosition : uint16_t
	{
		PrimaryApproachLights = 0xF,
		SecondaryApproachLights = 0x10
	};

	enum class EType : uint8_t
	{
		None = 0,
		Odals = 1,
		Malsf = 2,
		Malsr = 3,
		Ssalf = 4,
		Ssalr = 5,
		Alsf1 = 6,
		Alsf2 = 7,
		Rail = 8,
		Calvert = 9,
		Calvert2 = 10,
		Mals = 11,
		Sals = 12, // 13 missing
		Ssals = 14
	};

	virtual auto GetPosition() const -> EPosition = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetStrobeCount() const -> int = 0;
	virtual auto SetStrobeCount(int value) -> void = 0;
	virtual auto HasEndLights() const -> bool = 0;
	virtual auto SetEndLights(bool value) -> void = 0;
	virtual auto HasReilLights() const -> bool = 0;
	virtual auto SetReilLights(bool value) -> void = 0;
	virtual auto HasTouchdownLights() const -> bool = 0;
	virtual auto SetTouchdownLights(bool value) -> void = 0;
};


class IBglRunway : virtual public IBglLLA
{
public:

	enum class ERunwayNumber : uint8_t
	{
		None = 0,
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
	
	enum class ELightIntensity : uint8_t
	{
		None = 0,
		Low = 1,
		Medium = 2,
		High = 3
	};

	virtual auto GetSurfaceType() const -> ESurfaceType = 0;
	virtual auto SetSurfaceType(ESurfaceType value) -> void = 0;
	virtual auto GetPrimaryRunwayNumber() const -> ERunwayNumber = 0;
	virtual auto SetPrimaryRunwayNumber(ERunwayNumber value) -> void = 0;
	virtual auto GetPrimaryRunwayDesignator() const -> ERunwayDesignator = 0;
	virtual auto SetPrimaryRunwayDesignator(ERunwayDesignator value) -> void = 0;
	virtual auto GetSecondaryRunwayNumber() const -> ERunwayNumber = 0;
	virtual auto SetSecondaryRunwayNumber(ERunwayNumber value) -> void = 0;
	virtual auto GetSecondaryRunwayDesignator() const -> ERunwayDesignator = 0;
	virtual auto SetSecondaryRunwayDesignator(ERunwayDesignator value) -> void = 0;
	virtual auto GetPrimaryIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetPrimaryIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetSecondaryIcaoIdent() const -> uint32_t = 0;
	virtual auto SetSecondaryIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetLength() const -> float = 0;
	virtual auto SetLength(float value) -> void = 0;
	virtual auto GetWidth() const -> float = 0;
	virtual auto SetWidth(float value) -> void = 0;
	virtual auto GetHeading() const -> float = 0;
	virtual auto SetHeading(float value) -> void = 0;
	virtual auto GetPatternAltitude() const -> float = 0;
	virtual auto SetPatternAltitude(float value) -> void = 0;
	virtual auto HasEdgeMarkings() const -> bool = 0;
	virtual auto SetEdgeMarkings(bool value) -> void = 0;
	virtual auto HasThresholdMarkings() const -> bool = 0;
	virtual auto SetThresholdMarkings(bool value) -> void = 0;
	virtual auto HasFixedDistanceMarkings() const -> bool = 0;
	virtual auto SetFixedDistanceMarkings(bool value) -> void = 0;
	virtual auto HasTouchdownMarkings() const -> bool = 0;
	virtual auto SetTouchdownMarkings(bool value) -> void = 0;
	virtual auto HasDashMarkings() const -> bool = 0;
	virtual auto SetDashMarkings(bool value) -> void = 0;
	virtual auto HasIdentMarkings() const -> bool = 0;
	virtual auto SetIdentMarkings(bool value) -> void = 0;
	virtual auto HasPrecisionMarkings() const -> bool = 0;
	virtual auto SetPrecisionMarkings(bool value) -> void = 0;
	virtual auto HasEdgePavement() const -> bool = 0;
	virtual auto SetEdgePavement(bool value) -> void = 0;
	virtual auto IsSingleEnd() const -> bool = 0;
	virtual auto SetSingleEnd(bool value) -> void = 0;
	virtual auto IsPrimaryClosed() const -> bool = 0;
	virtual auto SetPrimaryClosed(bool value) -> void = 0;
	virtual auto IsSecondaryClosed() const -> bool = 0;
	virtual auto SetSecondaryClosed(bool value) -> void = 0;
	virtual auto IsPrimaryStol() const -> bool = 0;
	virtual auto SetPrimaryStol(bool value) -> void = 0;
	virtual auto IsSecondaryStol() const -> bool = 0;
	virtual auto SetSecondaryStol(bool value) -> void = 0;
	virtual auto HasAlternateThreshold() const -> bool = 0;
	virtual auto SetAlternateThreshold(bool value) -> void = 0;
	virtual auto HasAlternateFixedDistance() const -> bool = 0;
	virtual auto SetAlternateFixedDistance(bool value) -> void = 0;
	virtual auto HasAlternateTouchDown() const -> bool = 0;
	virtual auto SetAlternateTouchDown(bool value) -> void = 0;
	virtual auto HasAlternatePrecision() const -> bool = 0;
	virtual auto SetAlternatePrecision(bool value) -> void = 0;
	virtual auto HasLeadingZeroIdent() const -> bool = 0;
	virtual auto SetLeadingZeroIdent(bool value) -> void = 0;
	virtual auto HasNoThresholdEndArrows() const -> bool = 0;
	virtual auto SetNoThresholdEndArrows(bool value) -> void = 0;
	virtual auto GetEdgeLights() const -> ELightIntensity = 0;
	virtual auto SetEdgeLights(ELightIntensity value) -> void = 0;
	virtual auto GetCenterLights() const -> ELightIntensity = 0;
	virtual auto SetCenterLights(ELightIntensity value) -> void = 0;
	virtual auto IsCenterRedLights() const -> bool = 0;
	virtual auto SetCenterRedLights(bool value) -> void = 0;
	virtual auto IsPrimaryTakeoff() const -> bool = 0;
	virtual auto SetPrimaryTakeoff(bool value) -> void = 0;
	virtual auto IsPrimaryLanding() const -> bool = 0;
	virtual auto SetPrimaryLanding(bool value) -> void = 0;
	virtual auto IsPrimaryRightPattern() const -> bool = 0;
	virtual auto SetPrimaryRightPattern(bool value) -> void = 0;
	virtual auto IsSecondaryTakeoff() const -> bool = 0;
	virtual auto SetSecondaryTakeoff(bool value) -> void = 0;
	virtual auto IsSecondaryLanding() const -> bool = 0;
	virtual auto SetSecondaryLanding(bool value) -> void = 0;
	virtual auto IsSecondaryRightPattern() const -> bool = 0;
	virtual auto SetSecondaryRightPattern(bool value) -> void = 0;
	
	virtual auto GetPrimaryOffsetThreshold() -> const IBglRunwayEnd* = 0;
	virtual auto SetPrimaryOffsetThreshold(IBglRunwayEnd* value) -> void = 0;
	virtual auto GetSecondaryOffsetThreshold() -> const IBglRunwayEnd* = 0;
	virtual auto SetSecondaryOffsetThreshold(IBglRunwayEnd* value) -> void = 0;
	virtual auto GetPrimaryBlastPad() -> const IBglRunwayEnd* = 0;
	virtual auto SetPrimaryBlastPad(IBglRunwayEnd* value) -> void = 0;
	virtual auto GetSecondaryBlastPad() -> const IBglRunwayEnd* = 0;
	virtual auto SetSecondaryBlastPad(IBglRunwayEnd* value) -> void = 0;
	virtual auto GetPrimaryOverrun() -> const IBglRunwayEnd* = 0;
	virtual auto SetPrimaryOverrun(IBglRunwayEnd* value) -> void = 0;
	virtual auto GetSecondaryOverrun() -> const IBglRunwayEnd* = 0;
	virtual auto SetSecondaryOverrun(IBglRunwayEnd* value) -> void = 0;
	virtual auto GetPrimaryLeftVasi() -> const IBglRunwayVasi* = 0;
	virtual auto SetPrimaryLeftVasi(IBglRunwayVasi* value) -> void = 0;
	virtual auto GetPrimaryRightVasi() -> const IBglRunwayVasi* = 0;
	virtual auto SetPrimaryRightVasi(IBglRunwayVasi* value) -> void = 0;
	virtual auto GetSecondaryLeftVasi() -> const IBglRunwayVasi* = 0;
	virtual auto SetSecondaryLeftVasi(IBglRunwayVasi* value) -> void = 0;
	virtual auto GetSecondaryRightVasi() -> const IBglRunwayVasi* = 0;
	virtual auto SetSecondaryRightVasi(IBglRunwayVasi* value) -> void = 0;
	virtual auto GetPrimaryApproachLights() -> const IBglRunwayApproachLights* = 0;
	virtual auto SetPrimaryApproachLights(IBglRunwayApproachLights* value) -> void = 0;
	virtual auto GetSecondaryApproachLights() -> const IBglRunwayApproachLights* = 0;
	virtual auto SetSecondaryApproachLights(IBglRunwayApproachLights* value) -> void = 0;
};


class IBglStart : virtual public IBglLLA
{
public:
	enum class EType : uint8_t
	{
		None = 0,
		Runway = 1,
		Water = 2,
		Helipad = 3
	};
	
	virtual auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber = 0;
	virtual auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void = 0;
	virtual auto GetRunwayDesignator() const->IBglRunway::ERunwayDesignator = 0;
	virtual auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetHeading() const -> float = 0;
	virtual auto SetHeading(float value) -> void = 0;
};


class IBglCom
{
public:
	enum class EType : uint16_t
	{
		None = 0,
		Atis = 1,
		Multicom = 2,
		Unicom = 3,
		Ctaf = 4,
		Ground = 5,
		Tower = 6,
		Clearance = 7,
		Approach = 8,
		Departure = 9,
		Center = 10,
		Fss = 11,
		Awos = 12,
		Asos = 13,
		ClearancePreTaxi = 14,
		RemoteClearanceDelivery = 15
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetFrequency() const -> uint32_t = 0;
	virtual auto SetFrequency(uint32_t value) -> void = 0;
	virtual auto GetName() const -> const char* = 0;
	virtual auto SetName(const char* value) -> void = 0;
};


class IBglHelipad : virtual public IBglLLA
{
public:
	enum class EType : uint8_t
	{
		None = 0,
		H = 1,
		Square = 2,
		Circle = 3,
		Medical = 4
	};

	virtual auto GetSurfaceType() const -> ESurfaceType = 0;
	virtual auto SetSurfaceType(ESurfaceType value) -> void = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto IsTransparent() const -> bool = 0;
	virtual auto SetTransparent(bool value) -> void = 0;
	virtual auto IsClosed() const -> bool = 0;
	virtual auto SetClosed(bool value) -> void = 0;
	virtual auto GetColor() const -> uint32_t = 0;
	virtual auto SetColor(uint32_t value) -> void = 0;
	virtual auto GetLength() const -> float = 0;
	virtual auto SetLength(float value) -> void = 0;
	virtual auto GetWidth() const -> float = 0;
	virtual auto SetWidth(float value) -> void = 0;
	virtual auto GetHeading() const -> float = 0;
	virtual auto SetHeading(float value) -> void = 0;
};


class IBglRunwayDelete
{
public: // TODO - shared interface?
	virtual auto GetSurfaceType() const -> ESurfaceType = 0;
	virtual auto SetSurfaceType(ESurfaceType value) -> void = 0;
	virtual auto GetPrimaryRunwayNumber() const -> IBglRunway::ERunwayNumber = 0;
	virtual auto SetPrimaryRunwayNumber(IBglRunway::ERunwayNumber value) -> void = 0;
	virtual auto GetPrimaryRunwayDesignator() const -> IBglRunway::ERunwayDesignator = 0;
	virtual auto SetPrimaryRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void = 0;
	virtual auto GetSecondaryRunwayNumber() const -> IBglRunway::ERunwayNumber = 0;
	virtual auto SetSecondaryRunwayNumber(IBglRunway::ERunwayNumber value) -> void = 0;
	virtual auto GetSecondaryRunwayDesignator() const -> IBglRunway::ERunwayDesignator = 0;
	virtual auto SetSecondaryRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void = 0;
};

	
class IBglStartDelete
{
public:
	virtual auto GetRunwayNumber() const->IBglRunway::ERunwayNumber = 0;
	virtual auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void = 0;
	virtual auto GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator = 0;
	virtual auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void = 0;
	virtual auto GetType() const -> IBglStart::EType = 0;
	virtual auto SetType(IBglStart::EType value) -> void = 0;
};

	
class IBglComDelete
{
public:
	virtual auto GetType() const -> IBglCom::EType = 0;
	virtual auto SetType(IBglCom::EType value) -> void = 0;
	virtual auto GetFrequency() const -> uint32_t = 0;
	virtual auto SetFrequency(uint32_t value) -> void = 0;
};


class IBglAirportDelete
{
public:
	virtual auto IsAllApproaches() const -> bool = 0;
	virtual auto SetAllApproaches(bool value) -> void = 0;
	virtual auto IsAllApronLights() const -> bool = 0;
	virtual auto SetAllApronLights(bool value) -> void = 0;
	virtual auto IsAllAprons() const -> bool = 0;
	virtual auto SetAllAprons(bool value) -> void = 0;
	virtual auto IsAllFrequencies() const -> bool = 0;
	virtual auto SetAllFrequencies(bool value) -> void = 0;
	virtual auto IsAllHelipads() const -> bool = 0;
	virtual auto SetAllHelipads(bool value) -> void = 0;
	virtual auto IsAllRunways() const -> bool = 0;
	virtual auto SetAllRunways(bool value) -> void = 0;
	virtual auto IsAllStarts() const -> bool = 0;
	virtual auto SetAllStarts(bool value) -> void = 0;
	virtual auto IsAllTaxiways() const -> bool = 0;
	virtual auto SetAllTaxiways(bool value) -> void = 0;

	virtual auto GetRunwayDeleteCount() const -> int = 0;
	virtual auto GetStartDeleteCount() const -> int = 0;
	virtual auto GetComDeleteCount() const -> int = 0;
	virtual auto GetRunwayDeleteAt(int index) -> IBglRunwayDelete* = 0;
	virtual auto AddRunwayDelete(const IBglRunwayDelete* runway) -> void = 0;
	virtual auto RemoveRunwayDelete(const IBglRunwayDelete* runway) -> void = 0;
	virtual auto GetStartDeleteAt(int index) -> IBglStartDelete* = 0;
	virtual auto AddStartDelete(const IBglStartDelete* start) -> void = 0;
	virtual auto RemoveStartDelete(const IBglStartDelete* start) -> void = 0;
	virtual auto GetComDeleteAt(int index) -> IBglComDelete* = 0;
	virtual auto AddComDelete(const IBglComDelete* com) -> void = 0;
	virtual auto RemoveComDelete(const IBglComDelete* com) -> void = 0;
};


class IBglApronEdgeLights
{
public:
	virtual auto GetVertexCount() const -> int = 0;
	virtual auto GetEdgeCount() const -> int = 0;
	virtual auto GetColor() const -> uint32_t = 0;
	virtual auto SetColor(uint32_t value) -> void = 0;
	virtual auto GetBrightness() const -> float = 0;
	virtual auto SetBrightness(float value) -> void = 0;
	virtual auto GetMaxAltitude() const -> float = 0;
	virtual auto SetMaxAltitude(float value) -> void = 0;
	virtual auto GetVertexAt(int index) -> SBglVertexLL* = 0;
	virtual auto AddVertex(const SBglVertexLL* vertex) -> void = 0;
	virtual auto RemoveVertex(const SBglVertexLL* vertex) -> void = 0;
	virtual auto GetEdgeAt(int index) -> SBglEdge* = 0;
	virtual auto AddEdge(const SBglEdge* edge) -> void = 0;
	virtual auto RemoveEdge(const SBglEdge* edge) -> void = 0;
};


class IBglApron
{
public:
	virtual auto GetSurfaceType() const -> ESurfaceType = 0;
	virtual auto SetSurfaceType(ESurfaceType value) -> void = 0;
	virtual auto GetVertexCount() const -> int = 0;
	virtual auto GetVertexAt(int index) -> SBglVertexLL* = 0;
	virtual auto AddVertex(const SBglVertexLL* vertex) -> void = 0;
	virtual auto RemoveVertex(const SBglVertexLL* vertex) -> void = 0;
};

	
class IBglApronPolygons
{
public:
	virtual auto GetSurfaceType() const -> ESurfaceType = 0;
	virtual auto SetSurfaceType(ESurfaceType value) -> void = 0;
	virtual auto IsDrawSurface() const -> bool = 0;
	virtual auto SetDrawSurface(bool value) -> void = 0;
	virtual auto IsDrawDetail() const -> bool = 0;
	virtual auto SetDrawDetail(bool value) -> void = 0;
	virtual auto GetVertexCount() const -> int = 0;
	virtual auto GetIndexCount() const -> int = 0;
	virtual auto GetVertexAt(int index) -> SBglVertexLL* = 0;
	virtual auto AddVertex(const SBglVertexLL* vertex) -> void = 0;
	virtual auto RemoveVertex(const SBglVertexLL* vertex) -> void = 0;
	virtual auto GetIndexAt(int index) -> SBglIndex* = 0;
	virtual auto AddIndex(const SBglIndex* index) -> void = 0;
	virtual auto RemoveIndex(const SBglIndex* index) -> void = 0;
};


// TODO - Handle XML Index (and adjust)
class IBglTaxiwayPoint
{
public:
	enum class EType : uint8_t
	{
		None = 0,
		Normal = 1,
		HoldShort = 2,
		IlsHoldShort = 4,
		HoldShortNoDraw = 5,
		IlsHoldShortNoDraw = 6
	};

	enum class EOrientation : uint8_t
	{
		Forward = 0,
		Reverse = 1
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetOrientation() const -> EOrientation = 0;
	virtual auto SetOrientation(EOrientation value) -> void = 0;
	virtual auto GetVertex() -> SBglVertexLL* = 0;
	virtual auto SetVertex(SBglVertexLL* vertex) -> void = 0;
};
	

class IBglTaxiwayPoints
{
public:
	virtual auto GetPointCount() const -> int = 0;
	virtual auto GetPointAt(int index) -> IBglTaxiwayPoint* = 0;
	virtual auto AddPoint(const IBglTaxiwayPoint* point) -> void = 0;
	virtual auto RemovePoint(const IBglTaxiwayPoint* point) -> void = 0;
};


// TODO - Handle XML Index (and adjust)
class IBglTaxiwayParking
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		RampGa = 0x1,
		RampGaSmall = 0x2,
		RampGaMedium = 0x3,
		RampGaLarge = 0x4,
		RampCargo = 0x5,
		RampMilCargo = 0x6,
		RampMilCombat = 0x7,
		GateSmall = 0x8,
		GateMedium = 0x9,
		HateHeavy = 0xA,
		DockGa = 0xB,
		Fuel = 0xC,
		Vehicle = 0xD
	};

	enum class EPushback : uint8_t
	{
		None = 0,
		Left = 0x1,
		Right = 0x2,
		Both = 0x3
	};

	enum class EName : uint8_t
	{
		None = 0x0,
		Parking = 0x1,
		ParkingN = 0x2,
		ParkingNe = 0x3,
		ParkingE = 0x4,
		ParkingSe = 0x5,
		ParkingS = 0x6,
		ParkingSw = 0x7,
		ParkingW = 0x8,
		ParkingNw = 0x9,
		Gate = 0xA,
		Dock = 0xB,
		GateA = 0xC,
		GateB = 0xD,
		GateC = 0xE,
		GateD = 0xF,
		GateE = 0x10,
		GateF = 0x11,
		GateG = 0x12,
		GateH = 0x13,
		GateI = 0x14,
		GateJ = 0x15,
		GateK = 0x16,
		GateL = 0x17,
		GateM = 0x18,
		GateN = 0x19,
		GateO = 0x1A,
		GateP = 0x1B,
		GateQ = 0x1C,
		GateR = 0x1D,
		GateS = 0x1E,
		GateT = 0x1F,
		GateU = 0x20,
		GateV = 0x21,
		GateW = 0x22,
		GateX = 0x23,
		GateY = 0x24,
		GateZ = 0x25
	};
	
	virtual auto GetAirlineCodeCount() const -> int = 0;
	virtual auto GetNumber() const -> uint16_t = 0;
	virtual auto SetNumber(uint16_t value) -> void = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetPushback() const -> EPushback = 0;
	virtual auto SetPushback(EPushback value) -> void = 0;
	virtual auto GetName() const -> EName = 0;
	virtual auto SetName(EName value) -> void = 0;
	virtual auto GetRadius() const -> float = 0;
	virtual auto SetRadius(float value) -> void = 0;
	virtual auto GetHeading() const -> float = 0;
	virtual auto SetHeading(float value) -> void = 0;
	virtual auto GetTeeOffset1() const -> float = 0;
	virtual auto SetTeeOffset1(float value) -> void = 0;
	virtual auto GetTeeOffset2() const -> float = 0;
	virtual auto SetTeeOffset2(float value) -> void = 0;
	virtual auto GetTeeOffset3() const -> float = 0;
	virtual auto SetTeeOffset3(float value) -> void = 0;
	virtual auto GetTeeOffset4() const -> float = 0;
	virtual auto SetTeeOffset4(float value) -> void = 0;
	virtual auto GetAirlineCodeAt(int index) const -> const char* = 0;
	virtual auto AddAirlineCode(const char* code) -> void = 0;
	virtual auto RemoveAirlineCode(const char* code) -> void = 0;
};

	
class IBglTaxiwayParkings
{
public:
	virtual auto GetParkingCount() const -> int = 0;
	virtual auto GetParkingAt(int index) -> IBglTaxiwayParking* = 0;
	virtual auto AddParking(const IBglTaxiwayParking* parking) -> void = 0;
	virtual auto RemoveParking(const IBglTaxiwayParking* parking) -> void = 0;
};


class IBglTaxiwayPath
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		Taxi = 0x1,
		Runway = 0x2,
		Parking = 0x3,
		Path = 0x4,
		Closed = 0x5,
		Vehicle = 0x6
	};

	enum class ELightType : uint8_t
	{
		None = 0x0,
		Solid = 0x1,
		Dashed = 0x2,
		SolidDashed = 0x3
	};
	
	virtual auto GetStartIndex() const -> int = 0;
	virtual auto SetStartIndex(int value) -> void = 0;
	virtual auto GetEndIndex() const -> int = 0;
	virtual auto SetEndIndex(int value) -> void = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber = 0;
	virtual auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void = 0;
	virtual auto GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator = 0;
	virtual auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void = 0;
	virtual auto GetNameIndex() const -> int = 0;
	virtual auto SetNameIndex(int value) -> void = 0;
	virtual auto IsDrawSurface() const -> bool = 0;
	virtual auto SetDrawSurface(bool value) -> void = 0;
	virtual auto IsDrawDetail() const -> bool = 0;
	virtual auto SetDrawDetail(bool value) -> void = 0;
	virtual auto HasCenterLine() const -> bool = 0;
	virtual auto SetCenterLine(bool value) -> void = 0;
	virtual auto IsCenterLineLighted() const -> bool = 0;
	virtual auto SetCenterLineLighted(bool value) -> void = 0;
	virtual auto IsLeftEdgeLighted() const -> bool = 0;
	virtual auto SetsLeftEdgeLighted(bool value) -> void = 0;
	virtual auto IsRightEdgeLighted() const -> bool = 0;
	virtual auto SetsRightEdgeLighted(bool value) -> void = 0;
	virtual auto GetLeftEdge() const->ELightType = 0;
	virtual auto SetLeftEdge(ELightType value) -> void = 0;
	virtual auto GetRightEdge() const->ELightType = 0;
	virtual auto SetRightEdge(ELightType value) -> void = 0;
	virtual auto GetSurfaceType() const -> ESurfaceType = 0;
	virtual auto SetSurfaceType(ESurfaceType value) -> void = 0;
	virtual auto GetWidth() const -> float = 0;
	virtual auto SetWidth(float value) -> void = 0;
	virtual auto GetWeightLimit() const -> float = 0;
	virtual auto SetWeightLimit(float value) -> void = 0;
};
	
class IBglTaxiwayPaths
{
public:
	virtual auto GetPathCount() const -> int = 0;
	virtual auto GetPathAt(int index) -> IBglTaxiwayPath* = 0;
	virtual auto AddPath(const IBglTaxiwayPath* path) -> void = 0;
	virtual auto RemovePath(const IBglTaxiwayPath* path) -> void = 0;
};
	
	
class IBglTaxiwayNames
{
public:
	virtual auto GetNameCount() const -> int = 0;
	virtual auto GetNameAt(int index) const -> const char* = 0;
	virtual auto AddName(const char* name) -> void = 0;
	virtual auto RemoveName(const char* name) -> void = 0;
};


class IBglJetway
{
public:
	virtual auto GetNumber() const -> uint16_t = 0;
	virtual auto SetNumber(uint16_t value) -> void = 0;
	virtual auto GetName() const -> IBglTaxiwayParking::EName = 0;
	virtual auto SetName(IBglTaxiwayParking::EName value) -> void = 0;
	virtual auto GetSceneryObject() -> IBglSceneryObject* = 0;
	virtual auto GetSceneryObject() const -> const IBglSceneryObject* = 0;
	virtual auto SetSceneryObject(IBglSceneryObject* value) -> void = 0;
};


class IBglLeg
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		AF = 0x1,
		CA = 0x2,
		CD = 0x3,
		CF = 0x4,
		CI = 0x5,
		CR = 0x6,
		DF = 0x7,
		FA = 0x8,
		FC = 0x9,
		FD = 0xA,
		FM = 0xB,
		HA = 0xC,
		HF = 0xD,
		HM = 0xE,
		IF = 0xF,
		PI = 0x10,
		RF = 0x11,
		TF = 0x12,
		VA = 0x13,
		VD = 0x14,
		VI = 0x15,
		VM = 0x16,
		VR = 0x17
	};
	
	enum class EAltitudeDescriptor : uint8_t
	{
		None = 0x0,
		A = 0x1,
		Plus = 0x2,
		Minus = 0x3,
		B = 0x4,
		C = 0x5,
		G = 0x6,
		H = 0x7,
		I = 0x8,
		J = 0x9,
		V = 0xA,
		Space = 0xB
	};

	enum class ETurnDirection : uint8_t
	{
		None = 0x0,
		Left = 0x1,
		Right = 0x2,
		Either = 0x3
	};

	enum class EFixType : uint8_t
	{
		// Missing 0x1, 0x7, and 0xQ
		None = 0x0,
		Vor = 0x2,
		Ndb = 0x3,
		TerminalNdb = 0x4,
		Waypoint = 0x5,
		TerminalWaypoint = 0x6,
		Localizer = 8,
		Runway = 0x9,
		ManualTermination = 0xB,
		CourseToAlt = 0xC,
		CourseToDist = 0xD,
		HeadingToAlt = 0xE
	};
	
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetAltitudeDescriptor() const ->EAltitudeDescriptor = 0;
	virtual auto SetAltitudeDescriptor(EAltitudeDescriptor value) -> void = 0;
	virtual auto GetTurnDirection() const -> ETurnDirection = 0;
	virtual auto SetTurnDirection(ETurnDirection value) -> void = 0;
	virtual auto IsTrueCourse() const -> bool = 0;
	virtual auto SetTrueCourse(bool value) -> void = 0;
	virtual auto IsTime() const -> bool = 0;
	virtual auto SetIsTime(bool value) -> void = 0;
	virtual auto IsFlyover() const -> bool = 0;
	virtual auto SetFlyover(bool value) -> void = 0;
	virtual auto GetFixType() const -> EFixType = 0;
	virtual auto SetFixType(EFixType value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetIcaoAirport() const -> uint32_t = 0;
	virtual auto SetIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetRecommendedFixType() const -> EFixType = 0;
	virtual auto SetRecommendedFixType(EFixType value) -> void = 0;
	virtual auto GetRecommendedIcaoIdent() const -> uint32_t = 0;
	virtual auto SetRecommendedIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRecommendedRegionIdent() const -> uint32_t = 0;
	virtual auto SetRecommendedRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetTheta() const -> float = 0;
	virtual auto SetTheta(float value) -> void = 0;
	virtual auto GetRho() const -> float = 0;
	virtual auto SetRho(float value) -> void = 0;
	virtual auto GetTrueCourse() const -> float = 0;
	virtual auto SetTrueCourse(float value) -> void = 0;
	virtual auto GetMagneticCourse() const -> float = 0;
	virtual auto SetMagneticCourse(float value) -> void = 0;
	virtual auto GetDistance() const -> float = 0;
	virtual auto SetDistance(float value) -> void = 0;
	virtual auto GetTime() const -> float = 0;
	virtual auto SetTime(float value) -> void = 0;
	virtual auto GetAltitude1() const -> float = 0;
	virtual auto SetAltitude1(float value) -> void = 0;
	virtual auto GetAltitude2() const -> float = 0;
	virtual auto SetAltitude2(float value) -> void = 0;
};


class IBglLegs
{
public:
	virtual auto GetLegCount() const -> int = 0;
	virtual auto GetLegAt(int index) -> IBglLeg* = 0;
	virtual auto AddLeg(const IBglLeg* leg) -> void = 0;
	virtual auto RemoveLeg(const IBglLeg* leg) -> void = 0;
};


class IBglDmeArc
{
public:
	virtual auto GetIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetIcaoAirport() const -> uint32_t = 0;
	virtual auto SetIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetRadial() const -> int = 0;
	virtual auto SetRadial(int value) -> void = 0;
	virtual auto GetDistance() const -> float = 0;
	virtual auto SetDistance(float value) -> void = 0;
};

	
class IBglTransition
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		Full = 0x1,
		Dme = 0x2
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetLegCount() const -> int = 0;
	virtual auto GetFixType() const ->IBglLeg::EFixType = 0;
	virtual auto SetFixType(IBglLeg::EFixType value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetIcaoAirport() const -> uint32_t = 0;
	virtual auto SetIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetAltitude() const -> float = 0;
	virtual auto SetAltitude(float value) -> void = 0;
	virtual auto GetDmeArc() -> IBglDmeArc* = 0;
	virtual auto SetDmeArc(IBglDmeArc* value) -> void = 0;
	virtual auto GetTransitionLegs() -> IBglLegs* = 0;
	virtual auto SetTransitionLegs(IBglLegs* value) -> void = 0;
};
	
	
class IBglApproach
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		Gps = 0x1,
		Vor = 0x2,
		Ndb = 0x3,
		Ils = 0x4,
		Localizer = 0x5,
		Sdf = 0x6,
		Lda = 0x7,
		VorDme = 0x8,
		NdbDme = 0x9,
		Rnav = 0xA,
		LocalizerBackcourse = 0xB
	};
	
	virtual auto GetSuffix() const -> char = 0;
	virtual auto SetSuffix(char value) -> void = 0;
	virtual auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber = 0;
	virtual auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void = 0;
	virtual auto GetRunwayDesignator() const->IBglRunway::ERunwayDesignator = 0;
	virtual auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto HasGpsOverlay() const -> bool = 0;
	virtual auto SetGpsOverlay(bool value) -> void = 0;
	virtual auto GetFixType() const -> IBglLeg::EFixType = 0;
	virtual auto SetFixType(IBglLeg::EFixType value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetIcaoAirport() const -> uint32_t = 0;
	virtual auto SetIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetAltitude() const -> float = 0;
	virtual auto SetAltitude(float value) -> void = 0;
	virtual auto GetHeading() const -> float = 0;
	virtual auto SetHeading(float value) -> void = 0;
	virtual auto GetMissedAltitude() const -> float = 0;
	virtual auto SetMissedAltitude(float value) -> void = 0;
	virtual auto GetTransitionCount() const -> int = 0;
	virtual auto GetApproachLegCount() const -> int = 0;
	virtual auto GetMissedApproachLegCount() const -> int = 0;
	virtual auto GetApproachLegs() -> IBglLegs* = 0;
	virtual auto SetApproachLegs(IBglLegs* value) -> void = 0;
	virtual auto GetMissedApproachLegs() -> IBglLegs* = 0;
	virtual auto SetMissedApproachLegs(IBglLegs* value) -> void = 0;
	virtual auto GetTransitionAt(int index) -> IBglTransition* = 0;
	virtual auto AddTransition(const IBglTransition* transition) -> void = 0;
	virtual auto RemoveTransition(const IBglTransition* transition) -> void = 0;
};
	
	
class IBglFence
{
public:
	enum class EType : uint16_t
	{
		BlastFence = 0x38,
		BoundaryFence = 0x39,
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetVertexCount() const -> int = 0;
	virtual auto GetInstanceId() const -> _GUID = 0;
	virtual auto SetInstanceId(_GUID value) -> void = 0;
	virtual auto GetProfile() const -> _GUID = 0;
	virtual auto SetProfile(_GUID value) -> void = 0;
	virtual auto GetVertexAt(int index) -> SBglVertexLL* = 0;
	virtual auto AddVertex(const SBglVertexLL* vertex) -> void = 0;
	virtual auto RemoveVertex(const SBglVertexLL* vertex) -> void = 0;
};

	
// TODO ! Handle P3D5 (and FS9 and P20)
class IBglAirport : virtual public IBglFuelAvailability, virtual public IBglLLA, virtual public IBglName
{
public:
	virtual auto GetRunwayCount() const -> int = 0;
	virtual auto GetFrequencyCount() const -> int = 0;
	virtual auto GetStartCount() const -> int = 0;
	virtual auto GetApproachCount() const -> int = 0;
	virtual auto GetApronCount() const -> int = 0;
	virtual auto IsDeleteAirport() const -> bool = 0;
	virtual auto SetDeleteAirport(bool value) -> void = 0;
	virtual auto GetHelipadCount() const -> int = 0;
	virtual auto GetTowerLongitude() const -> double = 0;
	virtual auto SetTowerLongitude(double value) -> void = 0;
	virtual auto GetTowerLatitude() const -> double = 0;
	virtual auto SetTowerLatitude(double value) -> void = 0;
	virtual auto GetTowerAltitude() const -> double = 0;
	virtual auto SetTowerAltitude(double value) -> void = 0;
	virtual auto GetMagVar() const -> float = 0;
	virtual auto SetMagVar(float value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetTrafficScalar() const -> float = 0;
	virtual auto SetTrafficScalar(float value) -> void = 0;

	virtual auto GetRunwayAt(int index) -> IBglRunway* = 0;
	virtual auto AddRunway(const IBglRunway* runway) -> void = 0;
	virtual auto RemoveRunway(const IBglRunway* runway) -> void = 0;
	virtual auto GetStartAt(int index) -> IBglStart* = 0;
	virtual auto AddStart(const IBglStart* start) -> void = 0;
	virtual auto RemoveStart(const IBglStart* start) -> void = 0;
	virtual auto GetComAt(int index) -> IBglCom* = 0;
	virtual auto AddCom(const IBglCom* com) -> void = 0;
	virtual auto RemoveCom(const IBglCom* com) -> void = 0;
	virtual auto GetHelipadAt(int index) -> IBglHelipad* = 0;
	virtual auto AddHelipad(const IBglHelipad* helipad) -> void = 0;
	virtual auto RemoveHelipad(const IBglHelipad* helipad) -> void = 0;
	virtual auto GetDelete() -> IBglAirportDelete* = 0;
	virtual auto SetDelete(IBglAirportDelete* value) -> void = 0;
	virtual auto GetApronEdgeLights() -> IBglApronEdgeLights* = 0;
	virtual auto SetApronEdgeLights(IBglApronEdgeLights* value) -> void = 0;
	virtual auto GetApronAt(int index) -> IBglApron* = 0; // TODO - should these be collapsed
	virtual auto AddApron(const IBglApron* apron) -> void = 0; // into a single apron interface? 
	virtual auto RemoveApron(const IBglApron* apron) -> void = 0; // AddApron(apron, polygons) 
	virtual auto GetApronPolygonsAt(int index) -> IBglApronPolygons* = 0;
	virtual auto AddApronPolygons(const IBglApronPolygons* polygons) -> void = 0;
	virtual auto RemoveApronPolygons(const IBglApronPolygons* polygons) -> void = 0;
	virtual auto GetTaxiwayPoints() -> IBglTaxiwayPoints* = 0;
	virtual auto SetTaxiwayPoints(IBglTaxiwayPoints* value) -> void = 0;
	virtual auto GetTaxiwayParkings() -> IBglTaxiwayParkings* = 0;
	virtual auto SetTaxiwayParkings(IBglTaxiwayParkings* value) -> void = 0;
	virtual auto GetTaxiwayPaths() -> IBglTaxiwayPaths* = 0;
	virtual auto SetTaxiwayPaths(IBglTaxiwayPaths* value) -> void = 0;
	virtual auto GetTaxiwayNames() -> IBglTaxiwayNames* = 0;
	virtual auto SetTaxiwayNames(IBglTaxiwayNames* value) -> void = 0;
	virtual auto GetJetwayCount() const -> int = 0;
	virtual auto GetJetwayAt(int index) -> IBglJetway* = 0;
	virtual auto AddJetway(const IBglJetway* jetway) -> void = 0;
	virtual auto RemoveJetway(const IBglJetway* jetway) -> void = 0;
	virtual auto GetApproachAt(int index)->IBglApproach* = 0;
	virtual auto AddApproach(const IBglApproach* approach) -> void = 0;
	virtual auto RemoveApproach(const IBglApproach* approach) -> void = 0;
	virtual auto GetBlastFenceCount() const -> int = 0;
	virtual auto GetBlastFenceAt(int index) -> IBglFence* = 0;
	virtual auto AddBlastFence(const IBglFence* fence) -> void = 0;
	virtual auto RemoveBlastFence(const IBglFence* fence) -> void = 0;
	virtual auto GetBoundaryFenceCount() const -> int = 0;
	virtual auto GetBoundaryFenceAt(int index) -> IBglFence* = 0;
	virtual auto AddBoundaryFence(const IBglFence* fence) -> void = 0;
	virtual auto RemoveBoundaryFence(const IBglFence* fence) -> void = 0;
};
	

class IBglAirportSummary : virtual public IBglFuelAvailability, virtual public IBglLLA
{
public:
	virtual auto HasCom() const -> bool = 0;
	virtual auto SetCom(bool value) -> void = 0;
	virtual auto HasPavedRunway() const -> bool = 0;
	virtual auto SetPavedRunway(bool value) -> void = 0;
	virtual auto HasOnlyWaterRunway() const -> bool = 0;
	virtual auto SetOnlyWaterRunway(bool value) -> void = 0;
	virtual auto HasGpsApproach() const -> bool = 0;
	virtual auto SetGpsApproach(bool value) -> void = 0;
	virtual auto HasVorApproach() const -> bool = 0;
	virtual auto SetVorApproach(bool value) -> void = 0;
	virtual auto HasNdbApproach() const -> bool = 0;
	virtual auto SetNdbApproach(bool value) -> void = 0;
	virtual auto HasIlsApproach() const -> bool = 0;
	virtual auto SetIlsApproach(bool value) -> void = 0;
	virtual auto HasLocApproach() const -> bool = 0;
	virtual auto SetLocApproach(bool value) -> void = 0;
	virtual auto HasSdfApproach() const -> bool = 0;
	virtual auto SetSdfApproach(bool value) -> void = 0;
	virtual auto HasLdaApproach() const -> bool = 0;
	virtual auto SetLdaApproach(bool value) -> void = 0;
	virtual auto HasVorDmeApproach() const -> bool = 0;
	virtual auto SetVorDmeApproach(bool value) -> void = 0;
	virtual auto HasNdbDmeApproach() const -> bool = 0;
	virtual auto SetNdbDmeApproach(bool value) -> void = 0;
	virtual auto HasRnavApproach() const -> bool = 0;
	virtual auto SetRnavApproach(bool value) -> void = 0;
	virtual auto HasLocBcApproach() const -> bool = 0;
	virtual auto SetLocBcApproach(bool value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const->uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetMagVar() const -> float = 0;
	virtual auto SetMagVar(float value) -> void = 0;
	virtual auto GetLongestRunwayLength() const -> float = 0;
	virtual auto SetLongestRunwayLength(float value) -> void = 0;
	virtual auto GetLongestRunwayHeading() const -> float = 0;
	virtual auto SetLongestRunwayHeading(float value) -> void = 0;
};
	

class IBglRoute
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		Victor = 0x1,
		Jet = 0x2,
		Both = 0x3
	};

	enum class EConnectionType : uint8_t
	{
		Unnamed = 0x0,
		Vor = 0x2,
		Ndb = 0x3,
		Named = 0x5
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetName() const -> const char* = 0;
	virtual auto SetName(const char* value) -> void = 0;
	virtual auto GetPreviousType() const -> EConnectionType = 0;
	virtual auto SetPreviousType(EConnectionType value) -> void = 0;
	virtual auto GetPreviousIcaoIdent() const -> uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetPreviousIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetPreviousRegionIdent() const -> uint32_t = 0;
	virtual auto SetPreviousRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetPreviousIcaoAirport() const -> uint32_t = 0;
	virtual auto SetPreviousIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetPreviousAltitudeMinimum() const -> float = 0;
	virtual auto SetPreviousAltitudeMinimum(float value) -> void = 0;
	virtual auto GetNextType() const -> EConnectionType = 0;
	virtual auto SetNextType(EConnectionType value) -> void = 0;
	virtual auto GetNextIcaoIdent() const -> uint32_t = 0;
	virtual auto SetNextIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetNextRegionIdent() const -> uint32_t = 0;
	virtual auto SetNextRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetNextIcaoAirport() const -> uint32_t = 0;
	virtual auto SetNextIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetNextAltitudeMinimum() const -> float = 0;
	virtual auto SetNextAltitudeMinimum(float value) -> void = 0;
};

	
class IBglWaypoint
{
public:
	enum class EType : uint8_t
	{
		Unknown = 0x0,
		Named = 0x1,
		Unnamed = 0x2,
		Vor = 0x3,
		Ndb = 0x4,
		OffRoute = 0x5,
		Iaf = 0x6,
		Faf = 0x7
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetRouteCount() const -> int = 0;
	virtual auto GetLongitude() const -> double = 0;
	virtual auto SetLongitude(double value) -> void = 0;
	virtual auto GetLatitude() const -> double = 0;
	virtual auto SetLatitude(double value) -> void = 0;
	virtual auto GetMagVar() const -> float = 0;
	virtual auto SetMagVar(float value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0;
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetIcaoAirport() const -> uint32_t = 0;
	virtual auto SetIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetRouteAt(int index) -> IBglRoute* = 0;
	virtual auto AddRoute(const IBglRoute* route) -> void = 0;
	virtual auto RemoveRoute(const IBglRoute* route) -> void = 0;
};

	
class IBglLocalizer
{
public:
	virtual auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber = 0;
	virtual auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void = 0;
	virtual auto GetRunwayDesignator() const->IBglRunway::ERunwayDesignator = 0;
	virtual auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void = 0;
	virtual auto GetHeading() const -> float = 0;
	virtual auto SetHeading(float value) -> void = 0;
	virtual auto GetWidth() const -> float = 0;
	virtual auto SetWidth(float value) -> void = 0;
};

	
class IBglGlideSlope : virtual public IBglLLA
{
public:
	virtual auto GetPitch() const -> float = 0;
	virtual auto SetPitch(float value) -> void = 0;
	virtual auto GetRange() const -> float = 0;
	virtual auto SetRange(float value) -> void = 0;
};


class IBglDme : virtual public IBglLLA
{
public:
	virtual auto GetRange() const -> float = 0;
	virtual auto SetRange(float value) -> void = 0;
};
	
	
class IBglNav : virtual public IBglLLA, virtual public IBglName
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		VorTerminal = 0x1,
		VorLow = 0x2,
		VorHigh = 0x3,
		Ils = 0x4,
		VorVot = 0x5
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto IsDmeOnly() const -> bool = 0;
	virtual auto SetDmeOnly(bool value) -> void = 0;
	virtual auto HasBackCourse() const -> bool = 0;
	virtual auto SetBackCourse(bool value) -> void = 0;
	virtual auto HasGlideSlope() const -> bool = 0;
	virtual auto SetGlideSlope(bool value) -> void = 0;
	virtual auto HasDme() const -> bool = 0;
	virtual auto SetDme(bool value) -> void = 0;
	virtual auto HasNav() const -> bool = 0;
	virtual auto SetNav(bool value) -> void = 0;
	virtual auto GetFrequency() const -> uint32_t = 0;
	virtual auto SetFrequency(uint32_t value) -> void = 0;
	virtual auto GetRange() const -> float = 0;
	virtual auto SetRange(float value) -> void = 0;
	virtual auto GetMagVar() const -> float = 0;
	virtual auto SetMagVar(float value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0;
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetIcaoAirport() const -> uint32_t = 0;
	virtual auto SetIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetLocalizer() -> IBglLocalizer* = 0;
	virtual auto SetLocalizer(IBglLocalizer* value) -> void = 0;
	virtual auto GetGlideSlope() -> IBglGlideSlope* = 0;
	virtual auto SetGlideSlope(IBglGlideSlope* value) -> void = 0;
	virtual auto GetDmeRecord() -> IBglDme* = 0;
	virtual auto SetDmeRecord(IBglDme* value) -> void = 0;
};


class IBglTacan : virtual public IBglLLA, virtual public IBglName
{
public:
	virtual auto IsTypeY() const -> bool = 0;
	virtual auto SetTypeY(bool value) -> void = 0;
	virtual auto IsDmeOnly() const -> bool = 0;
	virtual auto SetDmeOnly(bool value) -> void = 0;
	virtual auto GetChannel() const -> uint32_t = 0;
	virtual auto SetChannel(uint32_t value) -> void = 0;
	virtual auto GetRange() const -> float = 0;
	virtual auto SetRange(float value) -> void = 0;
	virtual auto GetMagVar() const -> float = 0;
	virtual auto SetMagVar(float value) -> void = 0;
	virtual auto GetIcaoIdent() const -> uint32_t = 0;
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const -> uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
	virtual auto GetIcaoAirport() const -> uint32_t = 0;
	virtual auto SetIcaoAirport(uint32_t value) -> void = 0;
	virtual auto GetDmeRecord() -> IBglDme* = 0;
	virtual auto SetDmeRecord(IBglDme* value) -> void = 0;
};


class IBglBoundaryEdge
{
public:
	enum class EType : uint16_t
	{
		None = 0x0,
		Start = 0x1,
		Line = 0x2,
		Origin = 0x3,
		ArcClockwise = 0x4,
		ArcCounterClockwise = 0x5,
		Circle = 0x6
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetLongitude() const -> double = 0;
	virtual auto SetLongitude(double value) -> void = 0;
	virtual auto GetLatitude() const -> double = 0;
	virtual auto SetLatitude(double value) -> void = 0;
	virtual auto GetRadius() const -> float = 0;
	virtual auto SetRadius(float value) -> void = 0;
};
	

class IBglBoundaryEdges
{
public:
	virtual auto GetEdgeCount() const -> int = 0;
	virtual auto GetEdgeAt(int index) -> IBglBoundaryEdge* = 0;
	virtual auto AddEdge(const IBglBoundaryEdge* edge) -> void = 0;
	virtual auto RemoveEdge(const IBglBoundaryEdge* edge) -> void = 0;
};

	
class IBglBoundary : virtual public IBglName
{
public:
	enum class EType : uint8_t
	{
		None = 0x0,
		Center = 0x1,
		ClassA = 0x2,
		ClassB = 0x3,
		ClassC = 0x4,
		ClassD = 0x5,
		ClassE = 0x6,
		ClassF = 0x7,
		ClassG = 0x8,
		Tower = 0x9,
		Clearance = 0xA,
		Ground = 0xB,
		Departure = 0xC,
		Approach = 0xD,
		Moa = 0xE,
		Restricted = 0xF,
		Prohibited = 0x10,
		Warning = 0x11,
		Alert = 0x12,
		Danger = 0x13,
		NationalPark = 0x14,
		Modec = 0x15,
		Radar = 0x16,
		Training = 0x17
	};

	enum class EAltitudeType : uint8_t
	{
		None = 0x0,
		MeanSeaLevel = 0x1, // Unknown maps to this
		AboveGroundLevel = 0x2,
		Unlimited = 0x3
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetMaxAltitudeType() const -> EAltitudeType = 0;
	virtual auto SetMaxAltitudeType(EAltitudeType value) -> void = 0;
	virtual auto GetMinAltitudeType() const -> EAltitudeType = 0;
	virtual auto SetMinAltitudeType(EAltitudeType value) -> void = 0;
	virtual auto GetMinLongitude() const -> double = 0;
	virtual auto SetMinLongitude(double value) -> void = 0;
	virtual auto GetMinLatitude() const -> double = 0;
	virtual auto SetMinLatitude(double value) -> void = 0;
	virtual auto GetMinAltitude() const -> double = 0;
	virtual auto SetMinAltitude(double value) -> void = 0;
	virtual auto GetMaxLongitude() const -> double = 0;
	virtual auto SetMaxLongitude(double value) -> void = 0;
	virtual auto GetMaxLatitude() const -> double = 0;
	virtual auto SetMaxLatitude(double value) -> void = 0;
	virtual auto GetMaxAltitude() const -> double = 0;
	virtual auto SetMaxAltitude(double value) -> void = 0;
	virtual auto GetCom() -> IBglCom* = 0;
	virtual auto SetCom(IBglCom* value) -> void = 0;
	virtual auto GetEdges() -> IBglBoundaryEdges* = 0;
	virtual auto SetEdges(IBglBoundaryEdges* value) -> void = 0;
};
	
	
class IBglExclusion
{
public:
	virtual auto IsExcludeAll() const -> bool = 0;
	virtual auto SetAll(bool value) -> void = 0;
	virtual auto IsBeacon() const -> bool = 0;
	virtual auto SetBeacon(bool value) -> void = 0;
	virtual auto IsGenericBuilding() const -> bool = 0;
	virtual auto SetGenericBuilding(bool value) -> void = 0;
	virtual auto IsEffect() const -> bool = 0;
	virtual auto SetEffect(bool value) -> void = 0;
	virtual auto IsLibraryObject() const -> bool = 0;
	virtual auto SetLibraryObject(bool value) -> void = 0;
	virtual auto IsTaxiwaySigns() const -> bool = 0;
	virtual auto SetTaxiwaySigns(bool value) -> void = 0;
	virtual auto IsTrigger() const -> bool = 0;
	virtual auto SetTrigger(bool value) -> void = 0;
	virtual auto IsWindsock() const -> bool = 0;
	virtual auto SetWindsock(bool value) -> void = 0;
	virtual auto IsExtrusionBridge() const -> bool = 0;
	virtual auto SetExtrusionBridge(bool value) -> void = 0;
	virtual auto GetMinLongitude() const -> double = 0;
	virtual auto SetMinLongitude(double value) -> void = 0;
	virtual auto GetMaxLongitude() const -> double = 0;
	virtual auto SetMaxLongitude(double value) -> void = 0;
	virtual auto GetMinLatitude() const -> double = 0;
	virtual auto SetMinLatitude(double value) -> void = 0;
	virtual auto GetMaxLatitude() const -> double = 0;
	virtual auto SetMaxLatitude(double value) -> void = 0;
};


class IBglMarker
{
public:
	enum class EType : uint8_t
	{
		Inner = 0,
		Middle = 1,
		Outer = 2,
		BackCourse = 3
	};

	virtual auto GetHeading() const -> float = 0;
	virtual auto SetHeading(float value) -> void = 0;
	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetLongitude() const -> double = 0;
	virtual auto SetLongitude(double value) -> void = 0;
	virtual auto GetLatitude() const -> double = 0;
	virtual auto SetLatitude(double value) -> void = 0;
	virtual auto GetAltitude() const -> double = 0;
	virtual auto SetAltitude(double value) -> void = 0;
	virtual auto GetIcaoIdent() const->uint32_t = 0; // TODO, we need to parse and wrap this
	virtual auto SetIcaoIdent(uint32_t value) -> void = 0;
	virtual auto GetRegionIdent() const->uint32_t = 0;
	virtual auto SetRegionIdent(uint32_t value) -> void = 0;
};


class IBglGeopol
{
public:
	enum class EType : uint16_t
	{
		None = 0,
		Coastline = 64,
		Boundary = 128,
		DashedBoundary = 129
	};

	virtual auto GetType() const -> EType = 0;
	virtual auto SetType(EType value) -> void = 0;
	virtual auto GetMinLongitude() const -> double = 0;
	virtual auto SetMinLongitude(double value) -> void = 0;
	virtual auto GetMaxLongitude() const -> double = 0;
	virtual auto SetMaxLongitude(double value) -> void = 0;
	virtual auto GetMinLatitude() const -> double = 0;
	virtual auto SetMinLatitude(double value) -> void = 0;
	virtual auto GetMaxLatitude() const -> double = 0;
	virtual auto SetMaxLatitude(double value) -> void = 0;
	virtual auto GetVertexCount() const -> int = 0;
	virtual auto GetVertexAt(int index) -> SBglVertexLL* = 0;
	virtual auto AddVertex(const SBglVertexLL* vertex) -> void = 0;
	virtual auto RemoveVertex(const SBglVertexLL* vertex) -> void = 0;
};


class IBglGenericBuilding;
class IBglLibraryObject;
class IBglWindsock;
class IBglEffect;
class IBglTaxiwaySigns;
class IBglTrigger;
class IBglBeacon;
class IBglExtrusionBridge;

	
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

	enum class ESceneryObjectType : uint16_t
	{
		Unknown = 0x0,
		GenericBuilding = 0xA,
		LibraryObject = 0xB,
		Windsock = 0xC,
		Effect = 0xD,
		TaxiwaySigns = 0xE,
		Trigger = 0x10,
		Beacon = 0x11,
		ExtrusionBridge = 0x12,
		AttachedObjectEnd = 0x1002,
		AttachedObjectStart = 0x1003
	};
	
	virtual ESceneryObjectType GetSceneryObjectType() const = 0;
	virtual void SetSceneryObjectType(ESceneryObjectType value) = 0;
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
	virtual _GUID GetInstanceId() const = 0;
	virtual void SetInstanceId(_GUID value) = 0;

	// TODO - this is super unclean. Need to find a better way to expose nested layer data types
	// Overload on const...
	virtual IBglGenericBuilding* GetGenericBuilding() = 0;
	virtual IBglLibraryObject* GetLibraryObject() = 0;
	virtual IBglWindsock* GetWindsock() = 0;
	virtual IBglEffect* GetEffect() = 0;
	virtual IBglTaxiwaySigns* GetTaxiwaySigns() = 0;
	virtual IBglTrigger* GetTrigger() = 0;
	virtual IBglBeacon* GetBeacon() = 0;
	virtual IBglExtrusionBridge* GetExtrusionBridge() = 0;
};


class IBglGenericBuilding : virtual public IBglSceneryObject
{
public:
	enum class EType : uint16_t
	{
		RectangularFlatRoof = 4,
		RectangularRidgeRoof = 6,
		RectangularPeakedRoof = 7,
		RectangularSlantRoof = 8,
		Pyramidal = 9,
		Multisided = 10
	};

	virtual float GetScale() const = 0;
	virtual void SetScale(float value) = 0;
	virtual EType GetType() const = 0;
	virtual void SetType(EType value) = 0;
	virtual uint16_t GetBuildingSides() const = 0;
	virtual void SetBuildingSides(uint16_t value) = 0;
	virtual uint16_t GetSizeX() const = 0;
	virtual void SetSizeX(uint16_t value) = 0;
	virtual uint16_t GetSizeZ() const = 0;
	virtual void SetSizeZ(uint16_t value) = 0;
	virtual uint16_t GetSizeTopX() const = 0;
	virtual void SetSizeTopX(uint16_t value) = 0;
	virtual uint16_t GetSizeTopZ() const = 0;
	virtual void SetSizeTopZ(uint16_t value) = 0;
	virtual uint16_t GetBottomTexture() const = 0;
	virtual void SetBottomTexture(uint16_t value) = 0;
	virtual uint16_t GetSizeBottomY() const = 0;
	virtual void SetSizeBottomY(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexBottomX() const = 0;
	virtual void SetTextureIndexBottomX(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexBottomZ() const = 0;
	virtual void SetTextureIndexBottomZ(uint16_t value) = 0;
	virtual uint16_t GetWindowTexture() const = 0;
	virtual void SetWindowTexture(uint16_t value) = 0;
	virtual uint16_t GetSizeWindowY() const = 0;
	virtual void SetSizeWindowY(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexWindowX() const = 0;
	virtual void SetTextureIndexWindowX(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexWindowY() const = 0;
	virtual void SetTextureIndexWindowY(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexWindowZ() const = 0;
	virtual void SetTextureIndexWindowZ(uint16_t value) = 0;
	virtual uint16_t GetTopTexture() const = 0;
	virtual void SetTopTexture(uint16_t value) = 0;
	virtual uint16_t GetSizeTopY() const = 0;
	virtual void SetSizeTopY(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexTopX() const = 0;
	virtual void SetTextureIndexTopX(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexTopZ() const = 0;
	virtual void SetTextureIndexTopZ(uint16_t value) = 0;
	virtual uint16_t GetRoofTexture() const = 0;
	virtual void SetRoofTexture(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexRoofX() const = 0;
	virtual void SetTextureIndexRoofX(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexRoofZ() const = 0;
	virtual void SetTextureIndexRoofZ(uint16_t value) = 0;
	virtual uint16_t GetSizeRoofY() const = 0;
	virtual void SetSizeRoofY(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexGableY() const = 0;
	virtual void SetTextureIndexGableY(uint16_t value) = 0;
	virtual uint16_t GetGableTexture() const = 0;
	virtual void SetGableTexture(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexGableZ() const = 0;
	virtual void SetTextureIndexGableZ(uint16_t value) = 0;
	virtual uint16_t GetFaceTexture() const = 0;
	virtual void SetFaceTexture(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexFaceX() const = 0;
	virtual void SetTextureIndexFaceX(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexFaceY() const = 0;
	virtual void SetTextureIndexFaceY(uint16_t value) = 0;
	virtual uint16_t GetTextureIndexRoofY() const = 0;
	virtual void SetTextureIndexRoofY(uint16_t value) = 0;
};


class IBglLibraryObject : virtual public IBglSceneryObject
{
public:
	virtual _GUID GetName() const = 0;
	virtual void SetName(_GUID value) = 0;
	virtual float GetScale() const = 0;
	virtual void SetScale(float value) = 0;
};


class IBglWindsock : virtual public IBglSceneryObject
{
public:
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


class IBglEffect : virtual public IBglSceneryObject
{
public:
	virtual const char* GetName() const = 0;
	virtual void SetName(const char* value) = 0;
	virtual const char* GetParams() const = 0;
	virtual void SetParams(const char* value) = 0;
};


class IBglTaxiwaySign
{
public:
	enum class ESize : uint8_t
	{
		VerySparse = 0,
		Size1 = 1,
		Size2 = 2,
		Size3 = 3,
		Size4 = 4,
		Size5 = 5
	};

	enum class EJustification : uint8_t
	{
		None = 0,
		Right = 1,
		Left = 2
	};
	
	virtual float GetLongitudeBias() const = 0;
	virtual void SetLongitudeBias(float value) = 0;
	virtual float GetLatitudeBias() const = 0;
	virtual void SetLatitudeBias(float value) = 0;
	virtual float GetHeading() const = 0;
	virtual void SetHeading(float value) = 0;
	virtual ESize GetSize() const = 0;
	virtual void SetSize(ESize value) = 0;
	virtual EJustification GetJustification() const = 0;
	virtual void SetJustification(EJustification value) = 0;
	virtual const char* GetLabel() const = 0;
	virtual void SetLabel(const char* value) = 0;
};
	

class IBglTaxiwaySigns : virtual public IBglSceneryObject
{
public:
	virtual int GetSignCount() const = 0;
	virtual IBglTaxiwaySign* GetSignAt(int index) = 0;
	virtual void AddSign(const IBglTaxiwaySign* sign) = 0;
	virtual void RemoveSign(const IBglTaxiwaySign* sign) = 0;
};


class IBglTriggerRefuelRepair : virtual public IBglFuelAvailability
{
public:
	virtual int GetVertexCount() const = 0;
	virtual SBglVertexBias* GetVertexAt(int index) = 0;
	virtual void AddVertex(const SBglVertexBias* point) = 0;
	virtual void RemoveVertex(const SBglVertexBias* point) = 0;
};


class IBglTriggerWeather
{
public:
	enum class EType : uint16_t
	{
		RidgeLift = 1,
		UnidirectionalTurbulence = 2,
		DirectionalTurbulence = 3,
		Thermal = 4
	};

	virtual float GetTriggerHeading() const = 0;
	virtual void SetTriggerHeading(float value) = 0;
	virtual float GetScalar() const = 0;
	virtual void SetScalar(float value) = 0;
	virtual int GetVertexCount() const = 0;
	virtual SBglVertexBias* GetVertexAt(int index) = 0;
	virtual void AddVertex(const SBglVertexBias* point) = 0;
	virtual void RemoveVertex(const SBglVertexBias* point) = 0;
};


class IBglTrigger : virtual public IBglSceneryObject
{
public:

	enum class EType : uint16_t
	{
		RefuelRepair = 0,
		Weather = 1
	};

	virtual EType GetType() const = 0;
	virtual void SetType(EType value) = 0;
	virtual float GetHeight() const = 0;
	virtual void SetHeight(float value) = 0;
	virtual const IBglTriggerRefuelRepair* GetRepairRefuel() const = 0;
	virtual void SetRepairRefuel(const IBglTriggerRefuelRepair* value) = 0;
	virtual const IBglTriggerWeather* GetWeather() const = 0;
	virtual void SetWeather(const IBglTriggerWeather* value) = 0;
};


class IBglBeacon : virtual public IBglSceneryObject
{
public:

	enum class EType : uint8_t
	{
		Civilian,
		Military
	};

	enum class EBaseType : uint8_t
	{
		Airport,
		SeaBase,
		Heliport
	};

	virtual EBaseType GetBaseType() const = 0;
	virtual void SetBaseType(EBaseType value) = 0;
	virtual EType GetType() const = 0;
	virtual void SetType(EType value) = 0;
};


class IBglExtrusionBridge : virtual public IBglSceneryObject
{
public:
	virtual _GUID GetExtrusionProfile() const = 0;
	virtual void SetExtrusionProfile(_GUID value) = 0;
	virtual _GUID GetMaterialSet() const = 0;
	virtual void SetMaterialSet(_GUID value) = 0;
	virtual double GetLongitudeSample1() const = 0;
	virtual void SetLongitudeSample1(double value) = 0;
	virtual double GetLatitudeSample1() const = 0;
	virtual void SetLatitudeSample1(double value) = 0;
	virtual double GetAltitudeSample1() const = 0;
	virtual void SetAltitudeSample1(double value) = 0;
	virtual double GetLongitudeSample2() const = 0;
	virtual void SetLongitudeSample2(double value) = 0;
	virtual double GetLatitudeSample2() const = 0;
	virtual void SetLatitudeSample2(double value) = 0;
	virtual double GetAltitudeSample2() const = 0;
	virtual void SetAltitudeSample2(double value) = 0;
	virtual float GetRoadWidth() const = 0;
	virtual void SetRoadWidth(float value) = 0;
	virtual float GetProbability() const = 0;
	virtual void SetProbability(float value) = 0;
	virtual bool IsSuppressPlatform() const = 0;
	virtual void SetSuppressPlatform(bool value) = 0;
	virtual int GetPlacementCount() const = 0;
	virtual int GetPointCount() const = 0;
	virtual _GUID* GetPlacementAt(int index) = 0;
	virtual void AddPlacement(const _GUID* placement) = 0;
	virtual void RemovePlacement(const _GUID* placement) = 0;
	virtual SBglVertexLLA* GetPointAt(int index) = 0;
	virtual void AddPoint(const SBglVertexLLA* point) = 0;
	virtual void RemovePoint(const SBglVertexLLA* point) = 0;
};


class IBglModelData
{
public:
	virtual auto GetData() const ->const uint8_t* = 0;
	virtual auto SetData(const uint8_t* value, int length) -> void = 0;
	virtual auto GetLength() const -> int = 0;
};


class ITerrainRasterQuad1
{
public:
	virtual int Rows() const = 0;
	virtual int Cols() const = 0;
};



class IBglTimeZone
{
public:
	enum class EStartDayOfWeek : uint8_t
	{
		Any = 0,
		Sunday = 1,
		Monday = 2,
		Tuesday = 3,
		Wednesday = 4,
		Thursday = 5,
		Friday = 6,
		Saturday = 7
	};
	
	virtual auto GetMinLongitude() const -> double = 0;
	virtual auto SetMinLongitude(double value) -> void = 0;
	virtual auto GetMaxLatitude() const -> double = 0;
	virtual auto SetMaxLatitude(double value) -> void = 0;
	virtual auto GetMaxLongitude() const -> double = 0;
	virtual auto SetMaxLongitude(double value) -> void = 0;
	virtual auto GetMinLatitude() const -> double = 0;
	virtual auto SetMinLatitude(double value) -> void = 0;
	virtual auto GetTimeDeviation() const -> int16_t = 0;
	virtual auto SetTimeDeviation(int16_t value) -> void = 0;
	virtual auto GetPriority() const -> uint8_t = 0;
	virtual auto SetPriority(uint8_t value) -> void = 0;
	virtual auto GetDaylightSavingsTimeShift() const -> int8_t = 0;
	virtual auto SetDaylightSavingsTimeShift(int8_t value) -> void = 0;
	virtual auto GetDaylightSavingsStartDayOfYear() const -> uint16_t = 0;
	virtual auto SetDaylightSavingsStartDayOfYear(uint16_t value) -> void = 0;
	virtual auto GetDaylightSavingsStartDayOfWeek() const -> EStartDayOfWeek = 0;
	virtual auto SetDaylightSavingsStartDayOfWeek(EStartDayOfWeek value) -> void = 0;
	virtual auto GetDaylightSavingsEndDayOfYear() const -> uint16_t = 0;
	virtual auto SetDaylightSavingsEndDayOfYear(uint16_t value) -> void = 0;
	virtual auto GetDaylightSavingsEndDayOfWeek() const -> EStartDayOfWeek = 0;
	virtual auto SetDaylightSavingsEndDayOfWeek(EStartDayOfWeek value) -> void = 0;
};
	
	
}

}

#endif