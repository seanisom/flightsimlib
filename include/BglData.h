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
#include <string>
#include <vector>

// This is because of the mixins for SceneryObject
#pragma warning( disable : 4250 )


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
// CBglFuelAvailability
//******************************************************************************  


template <typename T>
class CBglFuelAvailability : virtual public IBglFuelAvailability
{
public:
	explicit CBglFuelAvailability(T& data);
	
	auto Get73Octane() const -> EFuelAvailability override;
	auto Set73Octane(EFuelAvailability value) -> void override;
	auto Get87Octane() const -> EFuelAvailability override;
	auto Set87Octane(EFuelAvailability value) -> void override;
	auto Get100Octane() const -> EFuelAvailability override;
	auto Set100Octane(EFuelAvailability value) -> void override;
	auto Get130Octane() const -> EFuelAvailability override;
	auto Set130Octane(EFuelAvailability value) -> void override;
	auto Get145Octane() const -> EFuelAvailability override;
	auto Set145Octane(EFuelAvailability value) -> void override;
	auto GetMogas() const -> EFuelAvailability override;
	auto SetMogas(EFuelAvailability value) -> void override;
	auto GetJet() const -> EFuelAvailability override;
	auto SetJet(EFuelAvailability value) -> void override;
	auto GetJetA() const -> EFuelAvailability override;
	auto SetJetA(EFuelAvailability value) -> void override;
	auto GetJetA1() const -> EFuelAvailability override;
	auto SetJetA1(EFuelAvailability value) -> void override;
	auto GetJetAP() const -> EFuelAvailability override;
	auto SetJetAP(EFuelAvailability value) -> void override;
	auto GetJetB() const -> EFuelAvailability override;
	auto SetJetB(EFuelAvailability value) -> void override;
	auto GetJet4() const -> EFuelAvailability override;
	auto SetJet4(EFuelAvailability value) -> void override;
	auto GetJet5() const -> EFuelAvailability override;
	auto SetJet5(EFuelAvailability value) -> void override;
	auto HasAvgas() const -> bool override;
	auto HasJetFuel() const -> bool override;
	
private:
	auto UpdateAvgasAvailability(EFuelAvailability value);
	auto UpdateJetFuelAvailability(EFuelAvailability value);
	
	enum class EFuelBits
	{
		Octane73 = 0,
		Octane87 = 2,
		Octane100 = 4,
		Octane130 = 6,
		Octane145 = 8,
		Mogas = 10,
		Jet = 12,
		JetA = 14,
		JetA1 = 16,
		JetAP = 18,
		JetB = 20,
		Jet4 = 22,
		Jet5 = 24,
		Reserved1 = 26,
		Reserved2 = 28,
		Avgas = 30,
		JetFuel = 31
	};

	static constexpr int s_num_availability_bits = 2;
	
	T& m_data;
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
		bool IsEmpty() const;
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
	stlab::copy_on_write<std::vector<CBglRunway>> m_runways;
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
};

#pragma pack(pop)


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
	int GetVertexCount() const override;
	const SBglVertexLL* GetVertexAt(int index) const override;
	void AddVertex(const SBglVertexLL* vertex) override;
	void RemoveVertex(const SBglVertexLL* vertex) override;

private:
	void SetVertexCount(int value);
	
	stlab::copy_on_write<SBglGeopolData> m_data;
	stlab::copy_on_write<std::vector<SBglVertexLL>> m_vertices;
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
	_GUID InstanceId; // TODO: Assumes FSX, need to add FS9 types
};

#pragma pack(pop)


class CBglSceneryObject : public IBglSerializable, virtual public IBglSceneryObject
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
	_GUID GetInstanceId() const override;
	void SetInstanceId(_GUID value) override;

protected:
	int RecordSize() const;

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
// CBglGenericBuilding
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)


struct SBglGenericBuildingData
{
	float Scale;
	uint16_t SubrecordStart;
	uint16_t SubrecordSize;
	uint16_t SubrecordType;
	uint16_t BuildingSides;
	uint16_t SizeX;
	uint16_t SizeZ;
	uint16_t SizeTopX;
	uint16_t SizeTopZ;
	uint16_t BottomTexture;
	uint16_t SizeBottomY;
	uint16_t TextureIndexBottomX;
	uint16_t TextureIndexBottomZ;
	uint16_t WindowTexture;
	uint16_t SizeWindowY;
	uint16_t TextureIndexWindowX;
	uint16_t TextureIndexWindowY;
	uint16_t TextureIndexWindowZ;
	uint16_t TopTexture;
	uint16_t SizeTopY;
	uint16_t TextureIndexTopX;
	uint16_t TextureIndexTopZ;
	uint16_t RoofTexture;
	uint16_t TextureIndexRoofX;
	uint16_t TextureIndexRoofZ;
	uint16_t SizeRoofY;
	uint16_t TextureIndexGableY;
	uint16_t GableTexture;
	uint16_t TextureIndexGableZ;
	uint16_t FaceTexture;
	uint16_t TextureIndexFaceX;
	uint16_t TextureIndexFaceY;
	uint16_t TextureIndexRoofY;
	uint16_t SubrecordEnd;
	// Smoothing not implemented
};

#pragma pack(pop)


class CBglGenericBuilding final : public CBglSceneryObject, public IBglGenericBuilding
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	float GetScale() const override;
	void SetScale(float value) override;
	EType GetType() const override;
	void SetType(EType value) override;
	uint16_t GetBuildingSides() const override;
	void SetBuildingSides(uint16_t value) override;
	uint16_t GetSizeX() const override;
	void SetSizeX(uint16_t value) override;
	uint16_t GetSizeZ() const override;
	void SetSizeZ(uint16_t value) override;
	uint16_t GetSizeTopX() const override;
	void SetSizeTopX(uint16_t value) override;
	uint16_t GetSizeTopZ() const override;
	void SetSizeTopZ(uint16_t value) override;
	uint16_t GetBottomTexture() const override;
	void SetBottomTexture(uint16_t value) override;
	uint16_t GetSizeBottomY() const override;
	void SetSizeBottomY(uint16_t value) override;
	uint16_t GetTextureIndexBottomX() const override;
	void SetTextureIndexBottomX(uint16_t value) override;
	uint16_t GetTextureIndexBottomZ() const override;
	void SetTextureIndexBottomZ(uint16_t value) override;
	uint16_t GetWindowTexture() const override;
	void SetWindowTexture(uint16_t value) override;
	uint16_t GetSizeWindowY() const override;
	void SetSizeWindowY(uint16_t value) override;
	uint16_t GetTextureIndexWindowX() const override;
	void SetTextureIndexWindowX(uint16_t value) override;
	uint16_t GetTextureIndexWindowY() const override;
	void SetTextureIndexWindowY(uint16_t value) override;
	uint16_t GetTextureIndexWindowZ() const override;
	void SetTextureIndexWindowZ(uint16_t value) override;
	uint16_t GetTopTexture() const override;
	void SetTopTexture(uint16_t value) override;
	uint16_t GetSizeTopY() const override;
	void SetSizeTopY(uint16_t value) override;
	uint16_t GetTextureIndexTopX() const override;
	void SetTextureIndexTopX(uint16_t value) override;
	uint16_t GetTextureIndexTopZ() const override;
	void SetTextureIndexTopZ(uint16_t value) override;
	uint16_t GetRoofTexture() const override;
	void SetRoofTexture(uint16_t value) override;
	uint16_t GetTextureIndexRoofX() const override;
	void SetTextureIndexRoofX(uint16_t value) override;
	uint16_t GetTextureIndexRoofZ() const override;
	void SetTextureIndexRoofZ(uint16_t value) override;
	uint16_t GetSizeRoofY() const override;
	void SetSizeRoofY(uint16_t value) override;
	uint16_t GetTextureIndexGableY() const override;
	void SetTextureIndexGableY(uint16_t value) override;
	uint16_t GetGableTexture() const override;
	void SetGableTexture(uint16_t value) override;
	uint16_t GetTextureIndexGableZ() const override;
	void SetTextureIndexGableZ(uint16_t value) override;
	uint16_t GetFaceTexture() const override;
	void SetFaceTexture(uint16_t value) override;
	uint16_t GetTextureIndexFaceX() const override;
	void SetTextureIndexFaceX(uint16_t value) override;
	uint16_t GetTextureIndexFaceY() const override;
	void SetTextureIndexFaceY(uint16_t value) override;
	uint16_t GetTextureIndexRoofY() const override;
	void SetTextureIndexRoofY(uint16_t value) override;

private:
	int CalculateSubrecordSize() const;

	stlab::copy_on_write<SBglGenericBuildingData> m_data;
};


//******************************************************************************
// CBglLibraryObject
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglLibraryObjectData
{
	_GUID Name;
	float Scale;
};

#pragma pack(pop)


class CBglLibraryObject final : public CBglSceneryObject, public IBglLibraryObject
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
	float PoleHeight;
	float WindsockLength;
	uint32_t PoleColor;
	uint32_t SockColor;
	uint16_t Lighted;
};

#pragma pack(pop)


class CBglWindsock final : public CBglSceneryObject, public IBglWindsock
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

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
// CBglEffect
//****************************************************************************** 


#pragma pack(push)
#pragma pack(1)

struct SBglEffectData
{
	std::string Name;
	std::string Params;
};

#pragma pack(pop)


class CBglEffect final : public CBglSceneryObject, public IBglEffect
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	const char* GetName() const override;
	void SetName(const char* value) override;
	const char* GetParams() const override;
	void SetParams(const char* value) override;

private:
	stlab::copy_on_write<SBglEffectData> m_data;
	static constexpr int s_name_size = 80;
};


//******************************************************************************
// CBglTaxiwaySign
//****************************************************************************** 
	

#pragma pack(push)
#pragma pack(1)

struct SBglTaxiwaySignData
{
	float LongitudeBias;
	float LatitudeBias;
	uint16_t Heading;
	uint8_t Size;
	uint8_t Justification;
	std::string Label;
};

#pragma pack(pop)


class CBglTaxiwaySign final : public IBglSerializable, public IBglTaxiwaySign
{
public:
	auto ReadBinary(BinaryFileStream& in) -> void override;
	auto WriteBinary(BinaryFileStream& out) -> void override;
	auto Validate() -> bool override;
	auto CalculateSize() const -> int override;

	auto GetLongitudeBias() const -> float override;
	auto SetLongitudeBias(float value) -> void override;
	auto GetLatitudeBias() const -> float override;
	auto SetLatitudeBias(float value) -> void override;
	auto GetHeading() const -> float override;
	auto SetHeading(float value) -> void override;
	auto GetSize() const -> ESize override;
	auto SetSize(ESize value) -> void override;
	auto GetJustification() const -> EJustification override;
	auto SetJustification(EJustification value) -> void override;
	auto GetLabel() const -> const char* override;
	auto SetLabel(const char* value) -> void override;

private:
	stlab::copy_on_write<SBglTaxiwaySignData> m_data;
	static constexpr int s_record_size = 12;
};


//******************************************************************************
// CBglTaxiwaySigns
//****************************************************************************** 


class CBglTaxiwaySigns final : public CBglSceneryObject, public IBglTaxiwaySigns
{
public:
	auto ReadBinary(BinaryFileStream& in) -> void override;
	auto WriteBinary(BinaryFileStream& out) -> void override;
	auto Validate() -> bool override;
	auto CalculateSize() const -> int override;

	auto GetSignCount() const -> int override;
	auto GetSignAt(int index) const -> const IBglTaxiwaySign* override;
	auto AddSign(const IBglTaxiwaySign* sign) -> void override;
	auto RemoveSign(const IBglTaxiwaySign* sign) -> void override;

private:
	stlab::copy_on_write<std::vector<CBglTaxiwaySign>> m_signs;
};


//******************************************************************************
// CBglTriggerRefuelRepair
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglTriggerRefuelRepairData
{
	uint32_t FuelAvailability;
	uint32_t PointCount;
};

#pragma pack(pop)


class CBglTriggerRefuelRepair final : public CBglFuelAvailability<stlab::copy_on_write<SBglTriggerRefuelRepairData>>,
	public IBglSerializable, public IBglTriggerRefuelRepair
{
public:
	CBglTriggerRefuelRepair() : CBglFuelAvailability<stlab::copy_on_write<SBglTriggerRefuelRepairData>>(m_data) { }

	auto ReadBinary(BinaryFileStream& in) -> void override;
	auto WriteBinary(BinaryFileStream& out) -> void override;
	auto Validate() -> bool override;
	auto CalculateSize() const -> int override;

	auto GetVertexCount() const -> int override;
	auto GetVertexAt(int index) const -> const SBglVertexBias* override;
	auto AddVertex(const SBglVertexBias* point) -> void override;
	auto RemoveVertex(const SBglVertexBias* point) -> void override;
	
private:
	stlab::copy_on_write<SBglTriggerRefuelRepairData> m_data;
	stlab::copy_on_write<std::vector<SBglVertexBias>> m_vertices;
};


//******************************************************************************
// CBglTriggerWeather
//******************************************************************************  


#pragma pack(push)
#pragma pack(1)

struct SBglTriggerWeatherData
{
	uint16_t Type;
	float Heading;
	float Scalar;
	uint32_t PointCount;
};

#pragma pack(pop)


class CBglTriggerWeather final : public IBglSerializable, public IBglTriggerWeather
{
public:
	auto ReadBinary(BinaryFileStream& in) -> void override;
	auto WriteBinary(BinaryFileStream& out) -> void override;
	auto Validate() -> bool override;
	auto CalculateSize() const -> int override;

	auto GetTriggerHeading() const -> float override;
	auto SetTriggerHeading(float value) -> void override;
	auto GetScalar() const -> float override;
	auto SetScalar(float value) -> void override;
	auto GetVertexCount() const -> int override;
	auto GetVertexAt(int index) const -> const SBglVertexBias* override;
	auto AddVertex(const SBglVertexBias* point) -> void override;
	auto RemoveVertex(const SBglVertexBias* point) -> void override;

private:
	stlab::copy_on_write<SBglTriggerWeatherData> m_data;
	stlab::copy_on_write<std::vector<SBglVertexBias>> m_vertices;
};


//******************************************************************************
// CBglTrigger
//****************************************************************************** 


#pragma pack(push)
#pragma pack(1)

struct SBglTriggerData
{
	uint16_t Type;
	float Height;
};

#pragma pack(pop)


class CBglTrigger final : public CBglSceneryObject, public IBglTrigger
{
public:
	auto ReadBinary(BinaryFileStream& in) -> void override;
	auto WriteBinary(BinaryFileStream& out) -> void override;
	auto Validate() -> bool override;
	auto CalculateSize() const -> int override;

	auto GetType() const -> EType override;
	auto SetType(EType value) -> void override;
	auto GetHeight() const -> float override;
	auto SetHeight(float value) -> void override;
	auto GetRepairRefuel() const -> const IBglTriggerRefuelRepair* override;
	auto SetRepairRefuel(const IBglTriggerRefuelRepair* value) -> void override;
	auto GetWeather() const -> const IBglTriggerWeather* override;
	auto SetWeather(const IBglTriggerWeather* value) -> void override;

private:
	stlab::copy_on_write<SBglTriggerData> m_data;
	stlab::copy_on_write<CBglTriggerRefuelRepair> m_refuel;
	stlab::copy_on_write<CBglTriggerWeather> m_weather;
};


//******************************************************************************
// CBglBeacon
//****************************************************************************** 


#pragma pack(push)
#pragma pack(1)

struct SBglBeaconData
{
	uint8_t BaseType;
	uint8_t Type;
};

#pragma pack(pop)


class CBglBeacon final : public CBglSceneryObject, public IBglBeacon
{
public:
	void ReadBinary(BinaryFileStream& in) override;
	void WriteBinary(BinaryFileStream& out) override;
	bool Validate() override;
	int CalculateSize() const override;

	EBaseType GetBaseType() const override;
	void SetBaseType(EBaseType value) override;
	EType GetType() const override;
	void SetType(EType value) override;

private:
	stlab::copy_on_write<SBglBeaconData> m_data;
};


//******************************************************************************
// CBglExtrusionBridge
//****************************************************************************** 


#pragma pack(push)
#pragma pack(1)

struct SBglExtrusionBridgeData
{
	_GUID ExtrusionProfile;
	_GUID MaterialSet;
	uint32_t LongitudeSample1;
	uint32_t LatitudeSample1;
	uint32_t AltitudeSample1;
	uint32_t LongitudeSample2;
	uint32_t LatitudeSample2;
	uint32_t AltitudeSample2;
	float RoadWidth;
	float Probability;
	uint8_t SuppressPlatform;
	uint8_t PlacementCount;
	uint16_t PointCount;
};

#pragma pack(pop)


class CBglExtrusionBridge final : public CBglSceneryObject, public IBglExtrusionBridge
{
public:
	auto ReadBinary(BinaryFileStream& in) -> void override;
	auto WriteBinary(BinaryFileStream& out) -> void override;
	auto Validate() -> bool override;
	auto CalculateSize() const -> int override;
	
	auto GetExtrusionProfile() const -> _GUID  override;
	auto SetExtrusionProfile(_GUID value) -> void  override;
	auto GetMaterialSet() const -> _GUID  override;
	void SetMaterialSet(_GUID value) override;
	auto GetLongitudeSample1() const -> double  override;
	void SetLongitudeSample1(double value) override;
	auto GetLatitudeSample1() const -> double  override;
	auto SetLatitudeSample1(double value) -> void  override;
	auto GetAltitudeSample1() const -> double  override;
	auto SetAltitudeSample1(double value) -> void  override;
	auto GetLongitudeSample2() const -> double  override;
	auto SetLongitudeSample2(double value) -> void  override;
	auto GetLatitudeSample2() const -> double  override;
	auto SetLatitudeSample2(double value) -> void  override;
	auto GetAltitudeSample2() const -> double  override;
	auto SetAltitudeSample2(double value) -> void  override;
	auto GetRoadWidth() const -> float  override;
	auto SetRoadWidth(float value) -> void  override;
	auto GetProbability() const -> float  override;
	auto SetProbability(float value) -> void  override;
	auto IsSuppressPlatform() const -> bool  override;
	auto SetSuppressPlatform(bool value) -> void  override;
	auto GetPlacementCount() const -> int  override;
	auto GetPointCount() const -> int  override;
	auto GetPlacementAt(int index) const -> const _GUID*  override;
	auto AddPlacement(const _GUID* placement) -> void  override;
	auto RemovePlacement(const _GUID* placement) -> void  override;
	auto GetPointAt(int index) const -> const SBglVertexLLA*  override;
	auto AddPoint(const SBglVertexLLA* point) -> void  override;
	auto RemovePoint(const SBglVertexLLA* point) -> void  override;

private:
	stlab::copy_on_write<SBglExtrusionBridgeData> m_data;
	stlab::copy_on_write<std::vector<_GUID>> m_placements;
	stlab::copy_on_write<std::vector<SBglVertexLLA>> m_points;
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

	struct STerrainRasterQuadData
	{
		
	};
	
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
	
	std::unique_ptr<uint8_t[]>DecompressData(
		ERasterCompressionType compression_type,
		uint8_t p_compressed[],
		int compressed_size,
		int uncompressed_size) const;
	
	stlab::copy_on_write<SBglTerrainRasterQuad1Data> m_header;
	stlab::copy_on_write<CRasterBlock> m_data;
};

} // namespace io
	
} // namespace flightsimlib

#endif
