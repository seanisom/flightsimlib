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
// File:     BglDecompressor.cpp
//
// Summary:  Implementation of actual stored data types from an FSX+ Bgl file
//
//           These are for internal consumption by clients of BglFile.h
//           
// Author:   Sean Isom
//
//******************************************************************************          


#include "BinaryStream.h"
#include "BglData.h"

#include "BglDecompressor.h"
#include "BglFile.h"


//******************************************************************************
// CBglRunway
//******************************************************************************  


void flightsimlib::io::CBglRunway::ReadBinary(BinaryFileStream& in)
{
	auto data = m_data.write();

	const auto initial_pos = in.GetPosition();
	
	in >> data.Type
		>> data.Size
		>> data.SurfaceType
		>> data.NumberPrimary
		>> data.DesignatorPrimary
		>> data.NumberSecondary
		>> data.DesignatorSecondary
		>> data.IlsIcaoPrimary
		>> data.IlsIcaoSecondary
		>> data.Latitude
		>> data.Longitude
		>> data.Altitude
		>> data.Length
		>> data.Width
		>> data.Heading
		>> data.PatternAltitude
		>> data.MarkingFlags
		>> data.LightingFlags
		>> data.PatternFlags;

	const auto final_position = initial_pos + static_cast<int>(m_data->Size);
	while (in.GetPosition() < final_position)
	{
		const auto child_pos = in.GetPosition();
		uint16_t type = 0;
		uint32_t size = 0;
		in >> type >> size;
		in.SetPosition(child_pos);

		// TODO enum
		switch (static_cast<EBglLayerType>(type))
		{
		case EBglLayerType::PrimaryOffsetThreshold:
			{
				m_primary_offset_threshold.ReadBinary(in);
			}
			break;
		case EBglLayerType::SecondaryOffsetThreshold:
			{
				m_secondary_offset_threshold.ReadBinary(in);
			}
			break;
			
		default:
			break;
		}

		in.SetPosition(child_pos + static_cast<int>(size));
	}
}

void flightsimlib::io::CBglRunway::WriteBinary(BinaryFileStream& out)
{
	const auto size = CalculateSize();
	if (size != m_data->Size)
	{
		m_data.write().Size = size;
	}
	
	out << m_data->Type
		<< m_data->Size
		<< m_data->SurfaceType
		<< m_data->NumberPrimary
		<< m_data->DesignatorPrimary
		<< m_data->NumberSecondary
		<< m_data->DesignatorSecondary
		<< m_data->IlsIcaoPrimary
		<< m_data->IlsIcaoSecondary
		<< m_data->Latitude
		<< m_data->Longitude
		<< m_data->Altitude
		<< m_data->Length
		<< m_data->Width
		<< m_data->Heading
		<< m_data->PatternAltitude
		<< m_data->MarkingFlags
		<< m_data->LightingFlags
		<< m_data->PatternFlags;

	m_primary_offset_threshold.WriteBinary(out);
	m_secondary_offset_threshold.WriteBinary(out);
}

bool flightsimlib::io::CBglRunway::Validate()
{
	m_data.write().Size = sizeof(SBglRunwayData);

	return m_data->Type == 0x4;
}

int flightsimlib::io::CBglRunway::CalculateSize() const
{
	return m_data->Size;
}

double flightsimlib::io::CBglRunway::GetLongitude() const
{
	return Longitude::Value(m_data->Longitude);
}

void flightsimlib::io::CBglRunway::SetLongitude(double value)
{
	m_data.write().Longitude = Longitude::ToPacked(value);
}

double flightsimlib::io::CBglRunway::GetLatitude() const
{
	return Latitude::Value(m_data->Latitude);
}

void flightsimlib::io::CBglRunway::SetLatitude(double value)
{
	m_data.write().Latitude = Latitude::ToPacked(value);
}

double flightsimlib::io::CBglRunway::GetAltitude() const
{
	return PackedAltitude::Value(m_data->Altitude);
}

void flightsimlib::io::CBglRunway::SetAltitude(double value)
{
	m_data.write().Altitude = PackedAltitude::FromDouble(value);
}

float flightsimlib::io::CBglRunway::GetLength() const
{
	return m_data->Length;
}

void flightsimlib::io::CBglRunway::SetLength(float value)
{
	m_data.write().Length = value;
}

float flightsimlib::io::CBglRunway::GetWidth() const
{
	return m_data->Width;
}

void flightsimlib::io::CBglRunway::SetWidth(float value)
{
	m_data.write().Width = value;
}

float flightsimlib::io::CBglRunway::GetHeading() const
{
	return m_data->Heading;
}

void flightsimlib::io::CBglRunway::SetHeading(float value)
{
	m_data.write().Heading = value;
}

float flightsimlib::io::CBglRunway::GetPatternAltitude() const
{
	return m_data->PatternAltitude;
}

void flightsimlib::io::CBglRunway::SetPatternAltitude(float value)
{
	m_data.write().PatternAltitude = value;
}

const flightsimlib::io::IBglRunwayOffsetThreshold* flightsimlib::io::CBglRunway::GetPrimaryOffsetThreshold()
{
	if (m_primary_offset_threshold.IsEmpty())
	{
		return nullptr;
	}
	return &m_primary_offset_threshold;
}

void flightsimlib::io::CBglRunway::SetPrimaryOffsetThreshold(IBglRunwayOffsetThreshold* value)
{
	// TODO - There has to be a better way to deal with the multiple interface issue for clients
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_primary_offset_threshold.Clone(*static_cast<CBglRunwayOffsetThreshold*>(value));
}

const flightsimlib::io::IBglRunwayOffsetThreshold* flightsimlib::io::CBglRunway::GetSecondaryOffsetThreshold()
{
	if (m_secondary_offset_threshold.IsEmpty())
	{
		return nullptr;
	}
	return &m_secondary_offset_threshold;
}

void flightsimlib::io::CBglRunway::SetSecondaryOffsetThreshold(IBglRunwayOffsetThreshold* value)
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_secondary_offset_threshold.Clone(*static_cast<CBglRunwayOffsetThreshold*>(value));
}


//******************************************************************************
// CBglRunwayOffsetThreshold
//******************************************************************************  

void flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::ReadBinary(BinaryFileStream& in)
{
	auto data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.SurfaceType
		>> data.Length
		>> data.Width;
}

void flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::WriteBinary(BinaryFileStream& out)
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->SurfaceType
		<< m_data->Length
		<< m_data->Width;
}

bool flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::Validate()
{
	return true;
}

int flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::CalculateSize() const
{
	return sizeof(SBglRunwayPadData);
}

flightsimlib::io::ESurfaceType flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::GetSurfaceType()
{
	return static_cast<ESurfaceType>(m_data->SurfaceType);
}

void flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::SetSurfaceType(ESurfaceType value)
{
	m_data.write().SurfaceType = static_cast<uint16_t>(value);
}

float flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::GetLength() const
{
	return m_data->Length;
}

void flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::SetLength(float value)
{
	m_data.write().Length = value;
}

float flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::GetWidth() const
{
	return m_data->Width;
}

void flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::SetWidth(float value)
{
	m_data.write().Width = value;
}

bool flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::IsEmpty()
{
	if (m_data->Type == 0)
	{
		return false;
	}
	return true;
}

void flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::Clone(CBglRunwayOffsetThreshold& value)
{
	// Value semantics, is this even necessary?
	m_data = value.m_data;
}


//******************************************************************************
// CBglAirport
//******************************************************************************  


void flightsimlib::io::CBglAirport::ReadBinary(BinaryFileStream& in)
{
	const auto initial_pos = in.GetPosition();

	auto data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.RunwayCount
		>> data.FrequencyCount
		>> data.StartCount
		>> data.ApproachCount
		>> data.ApronCount
		>> data.HelipadCount
		>> data.ReferenceLon
		>> data.ReferenceLat
		>> data.ReferenceAlt
		>> data.TowerLongitude
		>> data.TowerLatitude
		>> data.TowerAltitude
		>> data.MagVar
		>> data.Icao
		>> data.RegionIdent
		>> data.FuelTypes
		>> data.Flags;

	const auto final_position = initial_pos + static_cast<int>(m_data->Size);
	while (in.GetPosition() < final_position)
	{
		const auto child_pos = in.GetPosition();
		uint16_t type = 0;
		uint32_t size = 0;
		in >> type >> size;
		in.SetPosition(child_pos);

		// TODO enum
		switch (static_cast<EBglLayerType>(type))
		{
		case EBglLayerType::Runway: // runway
			{
				auto runway = CBglRunway{};
				runway.ReadBinary(in);
				m_runways.emplace_back(std::move(runway));
			}
			break;
		default:
			break;
		}

		in.SetPosition(child_pos + static_cast<int>(size));
	}
}

void flightsimlib::io::CBglAirport::WriteBinary(BinaryFileStream& out)
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->RunwayCount
		<< m_data->FrequencyCount
		<< m_data->StartCount
		<< m_data->ApproachCount
		<< m_data->ApronCount
		<< m_data->HelipadCount
		<< m_data->ReferenceLon
		<< m_data->ReferenceLat
		<< m_data->ReferenceAlt
		<< m_data->TowerLongitude
		<< m_data->TowerLatitude
		<< m_data->TowerAltitude
		<< m_data->MagVar
		<< m_data->Icao
		<< m_data->RegionIdent
		<< m_data->FuelTypes
		<< m_data->Flags;

	for (auto& runway : m_runways)
	{
		runway.WriteBinary(out);
	}
}

bool flightsimlib::io::CBglAirport::Validate()
{
	// TODO - static size
	m_data.write().Size = sizeof(SBglAirportData) +
		sizeof(SBglRunwayData) * m_data->RunwayCount;

	return m_data->Type == 0x3C;
}

int flightsimlib::io::CBglAirport::CalculateSize() const
{
	return m_data->Size;
}

float flightsimlib::io::CBglAirport::GetMagVar() const
{
	return m_data->MagVar;
}

void flightsimlib::io::CBglAirport::SetMagVar(float value)
{
	m_data.write().MagVar = value;
}


//******************************************************************************
// CBglExclusion
//******************************************************************************  


void flightsimlib::io::CBglExclusion::ReadBinary(BinaryFileStream& in)
{
	auto data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.MinLongitude
		>> data.MinLatitude
		>> data.MaxLongitude
		>> data.MaxLatitude;
}

void flightsimlib::io::CBglExclusion::WriteBinary(BinaryFileStream& out)
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->MinLongitude
		<< m_data->MinLatitude
		<< m_data->MaxLongitude
		<< m_data->MaxLatitude;
}

bool flightsimlib::io::CBglExclusion::Validate()
{
	return true;
}

int flightsimlib::io::CBglExclusion::CalculateSize() const
{
	return sizeof(SBglExclusionData);
}

double flightsimlib::io::CBglExclusion::GetMinLongitude() const
{
	return Longitude::Value(m_data->MinLongitude);
}

void flightsimlib::io::CBglExclusion::SetMinLongitude(double value)
{
	m_data.write().MinLongitude = Longitude::ToPacked(value);
}

double flightsimlib::io::CBglExclusion::GetMaxLongitude() const
{
	return Longitude::Value(m_data->MaxLongitude);
}

void flightsimlib::io::CBglExclusion::SetMaxLongitude(double value)
{
	m_data.write().MaxLongitude = Longitude::ToPacked(value);
}

double flightsimlib::io::CBglExclusion::GetMinLatitude() const
{
	return Latitude::Value(m_data->MinLatitude);
}

void flightsimlib::io::CBglExclusion::SetMinLatitude(double value)
{
	m_data.write().MinLatitude = Latitude::ToPacked(value);
}

double flightsimlib::io::CBglExclusion::GetMaxLatitude() const
{
	return Latitude::Value(m_data->MaxLatitude);
}

void flightsimlib::io::CBglExclusion::SetMaxLatitude(double value)
{
	m_data.write().MaxLatitude = Latitude::ToPacked(value);
}

bool flightsimlib::io::CBglExclusion::IsExcludeAll() const
{
	return (m_data->Type & 0xF) == 0x8;
}

void flightsimlib::io::CBglExclusion::SetExcludeAll(bool value)
{
	m_data.write().Type = 0x8; // Clear other flags
}

bool flightsimlib::io::CBglExclusion::IsGenericBuilding() const
{
	return m_data->Type & GenericBuilding;
}

void flightsimlib::io::CBglExclusion::SetGenericBuilding(bool value)
{
	if (value)
	{
		m_data.write().Type |= GenericBuilding;
	}
	else
	{
		m_data.write().Type &= ~GenericBuilding;
	}
}


//******************************************************************************
// CBglMarker
//******************************************************************************  


void flightsimlib::io::CBglMarker::ReadBinary(BinaryFileStream& in)
{
	auto data = m_data.write();
	in >> data.SectionType
		>> data.Size
		>> data.UnusedType
		>> data.Heading
		>> data.MarkerType
		>> data.Latitude
		>> data.Longitude
		>> data.Altitude
		>> data.Identifier
		>> data.Region
		>> data.Unknown;
}

void flightsimlib::io::CBglMarker::WriteBinary(BinaryFileStream& out)
{
	out << m_data->SectionType
		<< m_data->Size
		<< m_data->UnusedType
		<< m_data->Heading
		<< m_data->MarkerType
		<< m_data->Latitude
		<< m_data->Longitude
		<< m_data->Altitude
		<< m_data->Identifier
		<< m_data->Region
		<< m_data->Unknown;
}

bool flightsimlib::io::CBglMarker::Validate()
{
	return true;
}

int flightsimlib::io::CBglMarker::CalculateSize() const
{
	return sizeof(SBglMarkerData);
}

float flightsimlib::io::CBglMarker::GetHeading() const
{
	return ANGLE16::Value(m_data->Heading);
}

void flightsimlib::io::CBglMarker::SetHeading(float value)
{
	m_data.write().Heading = ANGLE16::FromDouble(value);
}

//******************************************************************************
// CBglGeopol
//******************************************************************************  

void flightsimlib::io::CBglGeopol::ReadBinary(BinaryFileStream& in)
{
	auto data = m_data.write();
	in >> data.SectionType
		>> data.Size
		>> data.GeopolType
		>> data.MinLongitude
		>> data.MinLatitude
		>> data.MaxLongitude
		>> data.MaxLatitude;

	const auto count = GetNumVertices();
	m_data.write().Vertices = new SBglVertexLL[count];

	for (auto i = 0; i < count; ++i)
	{
		auto& vert = m_data->Vertices[i];
		in >> vert.Longitude >> vert.Latitude;
	}
}

void flightsimlib::io::CBglGeopol::WriteBinary(BinaryFileStream& out)
{
	out << m_data->SectionType
		<< m_data->Size
		<< m_data->GeopolType
		<< m_data->MinLongitude
		<< m_data->MinLatitude
		<< m_data->MaxLongitude
		<< m_data->MaxLatitude;

	const auto count = GetNumVertices();
	for (auto i = 0; i < count; ++i)
	{
		const auto& vert = m_data->Vertices[i];
		out << vert.Longitude << vert.Longitude;
	}
}

bool flightsimlib::io::CBglGeopol::Validate()
{
	return true;
}

int flightsimlib::io::CBglGeopol::CalculateSize() const
{
	return static_cast<int>(sizeof(SBglGeopolData) + GetNumVertices() * sizeof(SBglVertexLL));
}

double flightsimlib::io::CBglGeopol::GetMinLongitude() const
{
	return Longitude::Value(m_data->MinLongitude);
}

void flightsimlib::io::CBglGeopol::SetMinLongitude(double value)
{
	m_data.write().MinLongitude = Longitude::ToPacked(value);
}

double flightsimlib::io::CBglGeopol::GetMaxLongitude() const
{
	return Longitude::Value(m_data->MaxLongitude);
}

void flightsimlib::io::CBglGeopol::SetMaxLongitude(double value)
{
	m_data.write().MaxLongitude = Longitude::ToPacked(value);
}

double flightsimlib::io::CBglGeopol::GetMinLatitude() const
{
	return Latitude::Value(m_data->MinLatitude);
}

void flightsimlib::io::CBglGeopol::SetMinLatitude(double value)
{
	m_data.write().MinLatitude = Latitude::ToPacked(value);
}

double flightsimlib::io::CBglGeopol::GetMaxLatitude() const
{
	return Latitude::Value(m_data->MaxLatitude);
}

void flightsimlib::io::CBglGeopol::SetMaxLatitude(double value)
{
	m_data.write().MaxLatitude = Latitude::ToPacked(value);
}

flightsimlib::io::IBglGeopol::EType flightsimlib::io::CBglGeopol::GetGeopolType() const
{
	return static_cast<EType>((m_data->GeopolType >> 14) & 0xF);
}

void flightsimlib::io::CBglGeopol::SetGeopolType(EType value)
{
	m_data.write().GeopolType = (m_data->GeopolType & 0x3FFF) | (static_cast<int>(value) << 14);
}

int flightsimlib::io::CBglGeopol::GetNumVertices() const
{
	return m_data->GeopolType & 0x3FFF;
}

void flightsimlib::io::CBglGeopol::SetNumVertices(int value)
{
	m_data.write().GeopolType = (m_data->GeopolType & 0xC000) | value;
}


//******************************************************************************
// CBglSceneryObject
//****************************************************************************** 


void flightsimlib::io::CBglSceneryObject::ReadBinary(BinaryFileStream& in)
{
	auto data = m_data.write();
	in >> data.SectionType
		>> data.Size
		>> data.Longitude
		>> data.Latitude
		>> data.Altitude
		>> data.Flags
		>> data.Pitch
		>> data.Bank
		>> data.Heading
		>> data.ImageComplexity;
}

void flightsimlib::io::CBglSceneryObject::WriteBinary(BinaryFileStream& out)
{
	out << m_data->SectionType
		<< m_data->Size
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Altitude
		<< m_data->Flags
		<< m_data->Pitch
		<< m_data->Bank
		<< m_data->Heading
		<< m_data->ImageComplexity;
}

bool flightsimlib::io::CBglSceneryObject::Validate()
{
	return true;
}

int flightsimlib::io::CBglSceneryObject::CalculateSize() const
{
	return sizeof(SBglSceneryObjectData); // TODO how are subclasses handled
}

double flightsimlib::io::CBglSceneryObject::GetLongitude() const
{
	return Longitude::Value(m_data->Longitude);
}

void flightsimlib::io::CBglSceneryObject::SetLongitude(double value)
{
	m_data.write().Longitude = Longitude::ToPacked(value);
}

double flightsimlib::io::CBglSceneryObject::GetLatitude() const
{
	return Latitude::Value(m_data->Latitude);
}

void flightsimlib::io::CBglSceneryObject::SetLatitude(double value)
{
	m_data.write().Latitude = Latitude::ToPacked(value);
}

double flightsimlib::io::CBglSceneryObject::GetAltitude() const
{
	return PackedAltitude::Value(m_data->Altitude);
}

void flightsimlib::io::CBglSceneryObject::SetAltitude(double value)
{
	m_data.write().Altitude = PackedAltitude::FromDouble(value);
}

bool flightsimlib::io::CBglSceneryObject::IsAboveAgl() const
{
	return m_data->Flags & static_cast<uint32_t>(Flags::AboveAgl);
}

void flightsimlib::io::CBglSceneryObject::SetAboveAgl(bool value)
{
	if (value)
	{
		m_data.write().Flags |= static_cast<uint32_t>(Flags::AboveAgl);
	}
	else
	{
		m_data.write().Flags &= ~static_cast<uint32_t>(Flags::AboveAgl);
	}
}

bool flightsimlib::io::CBglSceneryObject::IsNoAutogenSuppression() const
{
	return m_data->Flags & static_cast<uint32_t>(Flags::NoAutogenSuppression);
}

void flightsimlib::io::CBglSceneryObject::SetNoAutogenSuppression(bool value)
{
	if (value)
	{
		m_data.write().Flags |= static_cast<uint32_t>(Flags::NoAutogenSuppression);
	}
	else
	{
		m_data.write().Flags &= ~static_cast<uint32_t>(Flags::NoAutogenSuppression);
	}
}

bool flightsimlib::io::CBglSceneryObject::IsNoCrash() const
{
	return m_data->Flags & static_cast<uint32_t>(Flags::NoCrash);
}

void flightsimlib::io::CBglSceneryObject::SetNoCrash(bool value)
{
	if (value)
	{
		m_data.write().Flags |= static_cast<uint32_t>(Flags::NoCrash);
	}
	else
	{
		m_data.write().Flags &= ~static_cast<uint32_t>(Flags::NoCrash);
	}
}

bool flightsimlib::io::CBglSceneryObject::IsNoFog() const
{
	return m_data->Flags & static_cast<uint32_t>(Flags::NoFog);
}

void flightsimlib::io::CBglSceneryObject::SetNoFog(bool value)
{
	if (value)
	{
		m_data.write().Flags |= static_cast<uint32_t>(Flags::NoFog);
	}
	else
	{
		m_data.write().Flags &= ~static_cast<uint32_t>(Flags::NoFog);
	}
}

bool flightsimlib::io::CBglSceneryObject::IsNoShadow() const
{
	return m_data->Flags & static_cast<uint32_t>(Flags::NoShadow);
}

void flightsimlib::io::CBglSceneryObject::SetNoShadow(bool value)
{
	if (value)
	{
		m_data.write().Flags |= static_cast<uint32_t>(Flags::NoShadow);
	}
	else
	{
		m_data.write().Flags &= ~static_cast<uint32_t>(Flags::NoShadow);
	}
}

bool flightsimlib::io::CBglSceneryObject::IsNoZWrite() const
{
	return m_data->Flags & static_cast<uint32_t>(Flags::NoZWrite);
}

void flightsimlib::io::CBglSceneryObject::SetNoZWrite(bool value)
{
	if (value)
	{
		m_data.write().Flags |= static_cast<uint32_t>(Flags::NoZWrite);
	}
	else
	{
		m_data.write().Flags &= ~static_cast<uint32_t>(Flags::NoZWrite);
	}
}

bool flightsimlib::io::CBglSceneryObject::IsNoZTest() const
{
	return m_data->Flags & static_cast<uint32_t>(Flags::NoZTest);
}

void flightsimlib::io::CBglSceneryObject::SetNoZTest(bool value)
{
	if (value)
	{
		m_data.write().Flags |= static_cast<uint32_t>(Flags::NoZTest);
	}
	else
	{
		m_data.write().Flags &= ~static_cast<uint32_t>(Flags::NoZTest);
	}
}

float flightsimlib::io::CBglSceneryObject::GetPitch() const
{
	return ANGLE16::Value(m_data->Pitch);
}

void flightsimlib::io::CBglSceneryObject::SetPitch(float value)
{
	m_data.write().Pitch = ANGLE16::FromDouble(value);
}

float flightsimlib::io::CBglSceneryObject::GetBank() const
{
	return ANGLE16::Value(m_data->Bank);
}

void flightsimlib::io::CBglSceneryObject::SetBank(float value)
{
	m_data.write().Bank = ANGLE16::FromDouble(value);
}

float flightsimlib::io::CBglSceneryObject::GetHeading() const
{
	return ANGLE16::Value(m_data->Heading);
}

void flightsimlib::io::CBglSceneryObject::SetHeading(float value)
{
	m_data.write().Heading = ANGLE16::FromDouble(value);
}

flightsimlib::io::CBglSceneryObject::EImageComplexity flightsimlib::io::CBglSceneryObject::GetImageComplexity() const
{
	return static_cast<EImageComplexity>(m_data->ImageComplexity);
}

void flightsimlib::io::CBglSceneryObject::SetImageComplexity(EImageComplexity value)
{
	m_data.write().ImageComplexity = static_cast<uint16_t>(value);
}


//******************************************************************************
// CTerrainRasterQuad1
//******************************************************************************  


std::shared_ptr<uint8_t[]> flightsimlib::io::CRasterBlock::GetCompressedData()
{
	return nullptr;
}

void flightsimlib::io::CTerrainRasterQuad1::ReadBinary(BinaryFileStream& in)
{
	auto header = m_header.write();
	in >> header.Version
		>> header.Size
		>> header.DataType
		>> header.CompressionTypeData
		>> header.CompressionTypeMask
		>> header.QmidLow
		>> header.QmidHigh
		>> header.Variations
		>> header.Rows
		>> header.Cols
		>> header.SizeData
		>> header.SizeMask;
	
	if (in)
	{
		auto data = m_data.write();
		data.DataOffset = in.GetPosition();
		data.DataLength = m_header->SizeData; // TODO consistent naming!
		if(m_header->SizeMask > 0)
		{
			data.MaskOffset = m_data->DataOffset + m_data->DataLength;
			data.MaskLength = m_header->SizeMask;
		}
	}
}

void flightsimlib::io::CTerrainRasterQuad1::WriteBinary(BinaryFileStream& out)
{
	// if dirty
	m_header.write().SizeData = m_data->DataLength;
	m_header.write().SizeMask = m_data->MaskLength;
	out << m_header->Version
		<< m_header->Size
		<< m_header->DataType
		<< m_header->CompressionTypeData
		<< m_header->CompressionTypeMask
		<< m_header->QmidLow
		<< m_header->QmidHigh
		<< m_header->Variations
		<< m_header->Rows
		<< m_header->Cols
		<< m_header->SizeData
		<< m_header->SizeMask;
	
	m_data.write().DataOffset = out.GetPosition();
	if (m_data->MaskLength)
	{
		m_data.write().MaskOffset = m_data->DataOffset + m_data->DataLength;
	}
	else
	{
		m_data.write().MaskOffset = 0;
	}
}

bool flightsimlib::io::CTerrainRasterQuad1::Validate()
{
	return m_header->Version == 0x31515254; // TRQ1
}

int flightsimlib::io::CTerrainRasterQuad1::CalculateSize() const
{
	return static_cast<int>(m_header->Size) + m_data->DataLength + m_data->MaskLength;
}

int flightsimlib::io::CTerrainRasterQuad1::Rows() const
{
	return m_header->Rows;
}

int flightsimlib::io::CTerrainRasterQuad1::Cols() const
{
	return m_header->Cols;
}

std::shared_ptr<uint8_t[]> flightsimlib::io::CTerrainRasterQuad1::DecompressData(
	ERasterCompressionType compression_type, 
	int uncompressed_size)
{
	auto uncompressed = std::shared_ptr<uint8_t[]>(new uint8_t[uncompressed_size]);
	const auto compressed_size = m_data->DataLength;
	const auto compressed = m_data.write().GetCompressedData();
	const auto bpp = GetBpp();
	auto bit_depth = 0, num_channels = 0;
	if (!GetImageFormat(bit_depth, num_channels))
	{
		return nullptr;
	}

	auto intermediate_size = 0;
	std::shared_ptr<uint8_t[]> p_intermediate = nullptr;
	if (compression_type == ERasterCompressionType::DeltaLz1 || 
		compression_type == ERasterCompressionType::DeltaLz2 ||
		compression_type == ERasterCompressionType::BitPackLz1 ||
		compression_type == ERasterCompressionType::BitPackLz2)
	{
		intermediate_size = *reinterpret_cast<int*>(compressed.get());
		p_intermediate = std::shared_ptr<uint8_t[]>(new uint8_t[intermediate_size]);
	}

	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	auto bytes_read = 0;
	
	switch (compression_type)
	{
	case ERasterCompressionType::Delta:
		bytes_read = CBglDecompressor::DecompressDelta(
			uncompressed.get(), 
			uncompressed_size, 
			compressed.get());
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::BitPack:
		bytes_read = CBglDecompressor::DecompressBitPack(
			uncompressed.get(), 
			uncompressed_size, 
			compressed.get(), 
			compressed_size,
			Rows(), 
			Cols());
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::Lz1:
		bytes_read = CBglDecompressor::DecompressLz1(
			uncompressed.get(), 
			uncompressed_size, 
			compressed.get(), 
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::Lz2:
		bytes_read = CBglDecompressor::DecompressLz2(
			uncompressed.get(), 
			uncompressed_size, 
			compressed.get(), 
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::DeltaLz1:
		bytes_read = CBglDecompressor::DecompressLz1(
			p_intermediate.get(), 
			intermediate_size, 
			compressed.get() + sizeof(int), 
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressDelta(
			uncompressed.get(), 
			uncompressed_size,
			p_intermediate.get());
		if (bytes_read != intermediate_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::DeltaLz2:
		bytes_read = CBglDecompressor::DecompressLz2(
			p_intermediate.get(),
			intermediate_size, 
			compressed.get() + sizeof(int),
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressDelta(
			uncompressed.get(), 
			uncompressed_size, 
			p_intermediate.get());
		if (bytes_read != intermediate_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::BitPackLz1:
		bytes_read = CBglDecompressor::DecompressLz1(
			p_intermediate.get(),
			intermediate_size, 
			compressed.get() + sizeof(int),
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressBitPack(
			uncompressed.get(), 
			uncompressed_size, 
			p_intermediate.get(),
			intermediate_size, 
			Rows(), 
			Cols());
		if (bytes_read != intermediate_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::BitPackLz2:
		bytes_read = CBglDecompressor::DecompressLz2(
			p_intermediate.get(),
			intermediate_size, 
			compressed.get() + sizeof(int),
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressBitPack(
			uncompressed.get(), 
			uncompressed_size, 
			p_intermediate.get(),
			intermediate_size, 
			Rows(), 
			Cols());
		if (bytes_read != intermediate_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::Ptc:
		bytes_read = CBglDecompressor::DecompressPtc(
			uncompressed.get(), 
			uncompressed_size, 
			compressed.get(), 
			compressed_size, 
			Rows(), 
			Cols(), 
			num_channels, 
			bpp);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::Dxt1:
	case ERasterCompressionType::Dxt3:
	case ERasterCompressionType::Dxt5:
	case ERasterCompressionType::None: // TODO - this should be implemented
	case ERasterCompressionType::SolidBlock:
	default:
		throw std::exception("Unsupported Compression Type");
	}

	return uncompressed;
}
