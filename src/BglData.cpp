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

#include "BglData.h"

#include "BglDecompressor.h"
#include "BglFile.h"
#include "BinaryStream.h"

#include <type_traits>


// TODO Move to Util
template<typename T>
constexpr auto to_integral(T e) -> typename std::underlying_type<T>::type
{
	return static_cast<typename std::underlying_type<T>::type>(e);
}


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
		>> data.Longitude
		>> data.Latitude
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
	const auto size = static_cast<uint32_t>(CalculateSize());
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
		<< m_data->Longitude
		<< m_data->Latitude
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
	// TODO: This should be set by factory
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

bool flightsimlib::io::CBglRunway::CBglRunwayOffsetThreshold::IsEmpty() const
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

		switch (static_cast<EBglLayerType>(type))
		{
		case EBglLayerType::Runway: // runway
			{
				auto runway = CBglRunway{};
				runway.ReadBinary(in);
				m_runways.write().emplace_back(std::move(runway));
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

	for (auto& runway : m_runways.write())
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
		>> data.Longitude
		>> data.Latitude
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
		<< m_data->Longitude
		<< m_data->Latitude
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
	return static_cast<float>(ANGLE16::Value(m_data->Heading));
}

void flightsimlib::io::CBglMarker::SetHeading(float value)
{
	m_data.write().Heading = ANGLE16::FromDouble(static_cast<double>(value));
}

//******************************************************************************
// CBglGeopol
//******************************************************************************  

void flightsimlib::io::CBglGeopol::ReadBinary(BinaryFileStream& in)
{
	assert(m_vertices->empty());
	
	auto data = m_data.write();
	in >> data.SectionType
		>> data.Size
		>> data.GeopolType
		>> data.MinLongitude
		>> data.MinLatitude
		>> data.MaxLongitude
		>> data.MaxLatitude;

	const auto count = GetVertexCount();
	m_vertices.write().resize(count);

	for (auto& vertex : m_vertices.write())
	{
		in >> vertex.Longitude >> vertex.Latitude;
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

	for (const auto& vertex : m_vertices.read())
	{
		out << vertex.Longitude << vertex.Latitude;
	}
}

bool flightsimlib::io::CBglGeopol::Validate()
{
	return true;
}

int flightsimlib::io::CBglGeopol::CalculateSize() const
{
	return static_cast<int>(sizeof(SBglGeopolData) + GetVertexCount() * sizeof(SBglVertexLL));
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
	m_data.write().GeopolType = 
		static_cast<uint16_t>((m_data->GeopolType & 0x3FFF) | (static_cast<int>(value) << 14));
}

int flightsimlib::io::CBglGeopol::GetVertexCount() const
{
	return m_data->GeopolType & 0x3FFF;
}

const flightsimlib::io::SBglVertexLL* flightsimlib::io::CBglGeopol::GetVertexAt(int index) const
{
	return &(m_vertices.read()[index]);
}

void flightsimlib::io::CBglGeopol::AddVertex(const SBglVertexLL* vertex)
{
	// TODO Need validation, self check
	m_vertices.write().emplace_back(*vertex);
	SetVertexCount(m_vertices->size());
}

void flightsimlib::io::CBglGeopol::RemoveVertex(const SBglVertexLL* vertex)
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), vertex);
	m_vertices.write().erase(iter);
	
	SetVertexCount(m_vertices->size());
}

void flightsimlib::io::CBglGeopol::SetVertexCount(int value)
{
	m_data.write().GeopolType = 
		static_cast<uint16_t>((m_data->GeopolType & 0xC000) | value);
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
		>> data.ImageComplexity
		>> data.InstanceId;
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
		<< m_data->ImageComplexity
		<< m_data->InstanceId;
}

bool flightsimlib::io::CBglSceneryObject::Validate()
{
	return true;
}

int flightsimlib::io::CBglSceneryObject::CalculateSize() const
{
	return sizeof(SBglSceneryObjectData);
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
	return static_cast<float>(ANGLE16::Value(m_data->Pitch));
}

void flightsimlib::io::CBglSceneryObject::SetPitch(float value)
{
	m_data.write().Pitch = ANGLE16::FromDouble(static_cast<double>(value));
}

float flightsimlib::io::CBglSceneryObject::GetBank() const
{
	return static_cast<float>(ANGLE16::Value(m_data->Bank));
}

void flightsimlib::io::CBglSceneryObject::SetBank(float value)
{
	m_data.write().Bank = ANGLE16::FromDouble(static_cast<double>(value));
}

float flightsimlib::io::CBglSceneryObject::GetHeading() const
{
	return static_cast<float>(ANGLE16::Value(m_data->Heading));
}

void flightsimlib::io::CBglSceneryObject::SetHeading(float value)
{
	m_data.write().Heading = ANGLE16::FromDouble(static_cast<double>(value));
}

flightsimlib::io::CBglSceneryObject::EImageComplexity flightsimlib::io::CBglSceneryObject::GetImageComplexity() const
{
	return static_cast<EImageComplexity>(m_data->ImageComplexity);
}

void flightsimlib::io::CBglSceneryObject::SetImageComplexity(EImageComplexity value)
{
	m_data.write().ImageComplexity = static_cast<uint16_t>(value);
}

int flightsimlib::io::CBglSceneryObject::RecordSize() const
{
	return m_data->Size;
}


//******************************************************************************
// CBglGenericBuilding
//******************************************************************************  


auto flightsimlib::io::CBglGenericBuilding::ReadBinary(BinaryFileStream& in) -> void
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto data = m_data.write();
		
		in >> data.Scale
			>> data.SubrecordStart
			>> data.SubrecordSize
			>> data.SubrecordType;

		const auto type = static_cast<EType>(m_data->SubrecordType);

		if (type == EType::Multisided)
		{
			in >> data.BuildingSides;
		}

		in >> data.SizeX >> data.SizeZ;

		if (type == EType::Pyramidal)
		{
			in >> data.SizeTopX >> data.SizeTopZ;
		}

		in >> data.BottomTexture
			>> data.SizeBottomY
			>> data.TextureIndexBottomX;

		if (type != EType::Multisided)
		{
			in >> data.TextureIndexBottomZ;
		}

		in >> data.WindowTexture
			>> data.SizeWindowY
			>> data.TextureIndexWindowX
			>> data.TextureIndexWindowY;

		if (type != EType::Multisided)
		{
			in >> data.TextureIndexWindowZ;
		}

		in >> data.TopTexture
			>> data.SizeTopY
			>> data.TextureIndexTopX;

		if (type != EType::Multisided)
		{
			in >> data.TextureIndexTopZ;
		}

		in >> data.RoofTexture
			>> data.TextureIndexRoofX
			>> data.TextureIndexRoofZ;

		// At this point, Rectangular Flat is complete, as is Pyramidal.
		// Multisided is also complete, as TextureIndexRoofZ is not output

		if (type == EType::RectangularPeakedRoof)
		{
			in >> data.SizeRoofY >> data.TextureIndexRoofY;
		}

		if (type == EType::RectangularRidgeRoof || type == EType::RectangularSlantRoof)
		{
			in >> data.SizeRoofY
				>> data.GableTexture
				>> data.TextureIndexGableY
				>> data.TextureIndexGableZ;

			if (type == EType::RectangularSlantRoof)
			{
				in >> data.FaceTexture
					>> data.TextureIndexFaceX
					>> data.TextureIndexFaceY;
			}
		}

		in >> data.SubrecordEnd;
	}
}

auto flightsimlib::io::CBglGenericBuilding::WriteBinary(BinaryFileStream& out) -> void
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		const auto type = static_cast<EType>(m_data->SubrecordType);
		
		out << m_data->Scale
			<< m_data->SubrecordStart
			<< m_data->SubrecordSize
			<< m_data->SubrecordType;

		if (type == EType::Multisided)
		{
			out << m_data->BuildingSides;
		}

		out << m_data->SizeX << m_data->SizeZ;

		if (type == EType::Pyramidal)
		{
			out << m_data->SizeTopX << m_data->SizeTopZ;
		}

		out << m_data->BottomTexture
			<< m_data->SizeBottomY
			<< m_data->TextureIndexBottomX;

		if (type != EType::Multisided)
		{
			out << m_data->TextureIndexBottomZ;
		}

		out << m_data->WindowTexture
			<< m_data->SizeWindowY
			<< m_data->TextureIndexWindowX
			<< m_data->TextureIndexWindowY;

		if (type != EType::Multisided)
		{
			out << m_data->TextureIndexWindowZ;
		}

		out << m_data->TopTexture
			<< m_data->SizeTopY
			<< m_data->TextureIndexTopX;

		if (type != EType::Multisided)
		{
			out << m_data->TextureIndexTopZ;
		}

		out << m_data->RoofTexture
			<< m_data->TextureIndexRoofX
			<< m_data->TextureIndexRoofZ;

		if (type == EType::RectangularPeakedRoof)
		{
			out << m_data->SizeRoofY << m_data->TextureIndexRoofY;
		}

		if (type == EType::RectangularRidgeRoof || type == EType::RectangularSlantRoof)
		{
			out << m_data->SizeRoofY
				<< m_data->GableTexture
				<< m_data->TextureIndexGableY
				<< m_data->TextureIndexGableZ;

			if (type == EType::RectangularSlantRoof)
			{
				out << m_data->FaceTexture
					<< m_data->TextureIndexFaceX
					<< m_data->TextureIndexFaceY;
			}
		}

		out << m_data->SubrecordEnd;
	}
}

auto flightsimlib::io::CBglGenericBuilding::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglGenericBuilding::CalculateSize() const -> int
{
	return CBglSceneryObject::CalculateSize() + CalculateSubrecordSize();
}

auto flightsimlib::io::CBglGenericBuilding::GetScale() const -> float
{
	return m_data->Scale;
}

auto flightsimlib::io::CBglGenericBuilding::SetScale(float value) -> void
{
	m_data.write().Scale = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetType() const-> EType
{
	return static_cast<EType>(m_data->SubrecordType);
}

auto flightsimlib::io::CBglGenericBuilding::SetType(EType value) -> void
{
	m_data.write().SubrecordType = static_cast<uint16_t>(value);
	m_data.write().SubrecordSize = static_cast<uint16_t>(CalculateSubrecordSize());
}

auto flightsimlib::io::CBglGenericBuilding::GetBuildingSides() const -> uint16_t
{
	return m_data->BuildingSides;
}

auto flightsimlib::io::CBglGenericBuilding::SetBuildingSides(uint16_t value) -> void
{
	m_data.write().BuildingSides = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeX() const -> uint16_t
{
	return m_data->SizeX;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeX(uint16_t value) -> void
{
	m_data.write().SizeX = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeZ() const -> uint16_t
{
	return m_data->SizeZ;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeZ(uint16_t value) -> void
{
	m_data.write().SizeZ = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeTopX() const -> uint16_t
{
	return m_data->SizeTopX;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeTopX(uint16_t value) -> void
{
	m_data.write().SizeTopX = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeTopZ() const -> uint16_t
{
	return m_data->SizeTopZ;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeTopZ(uint16_t value) -> void
{
	m_data.write().SizeTopZ = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetBottomTexture() const -> uint16_t
{
	return m_data->BottomTexture;
}

auto flightsimlib::io::CBglGenericBuilding::SetBottomTexture(uint16_t value) -> void
{
	m_data.write().BottomTexture = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeBottomY() const -> uint16_t
{
	return m_data->SizeBottomY;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeBottomY(uint16_t value) -> void
{
	m_data.write().SizeBottomY = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexBottomX() const -> uint16_t
{
	return m_data->TextureIndexBottomX;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexBottomX(uint16_t value) -> void
{
	m_data.write().TextureIndexBottomX = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexBottomZ() const -> uint16_t
{
	return m_data->TextureIndexBottomZ;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexBottomZ(uint16_t value) -> void
{
	m_data.write().TextureIndexBottomZ = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetWindowTexture() const -> uint16_t
{
	return m_data->WindowTexture;
}

auto flightsimlib::io::CBglGenericBuilding::SetWindowTexture(uint16_t value) -> void
{
	m_data.write().WindowTexture = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeWindowY() const -> uint16_t
{
	return m_data->SizeWindowY;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeWindowY(uint16_t value) -> void
{
	m_data.write().SizeWindowY = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexWindowX() const -> uint16_t
{
	return m_data->TextureIndexWindowX;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexWindowX(uint16_t value) -> void
{
	m_data.write().TextureIndexWindowX = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexWindowY() const -> uint16_t
{
	return m_data->TextureIndexWindowY;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexWindowY(uint16_t value) -> void
{
	m_data.write().TextureIndexWindowY = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexWindowZ() const -> uint16_t
{
	return m_data->TextureIndexWindowZ;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexWindowZ(uint16_t value) -> void
{
	m_data.write().TextureIndexWindowZ = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTopTexture() const -> uint16_t
{
	return m_data->TopTexture;
}

auto flightsimlib::io::CBglGenericBuilding::SetTopTexture(uint16_t value) -> void
{
	m_data.write().TopTexture = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeTopY() const -> uint16_t
{
	return m_data->SizeTopY;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeTopY(uint16_t value) -> void
{
	m_data.write().SizeTopY = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexTopX() const -> uint16_t
{
	return m_data->TextureIndexTopX;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexTopX(uint16_t value) -> void
{
	m_data.write().TextureIndexTopX = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexTopZ() const -> uint16_t
{
	return m_data->TextureIndexTopZ;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexTopZ(uint16_t value) -> void
{
	m_data.write().TextureIndexTopZ = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetRoofTexture() const -> uint16_t
{
	return m_data->RoofTexture;
}

auto flightsimlib::io::CBglGenericBuilding::SetRoofTexture(uint16_t value) -> void
{
	m_data.write().RoofTexture = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexRoofX() const -> uint16_t
{
	return m_data->TextureIndexRoofX;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexRoofX(uint16_t value) -> void
{
	m_data.write().TextureIndexRoofX = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexRoofZ() const -> uint16_t
{
	return m_data->TextureIndexRoofZ;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexRoofZ(uint16_t value) -> void
{
	m_data.write().TextureIndexRoofZ = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetSizeRoofY() const -> uint16_t
{
	return m_data->SizeRoofY;
}

auto flightsimlib::io::CBglGenericBuilding::SetSizeRoofY(uint16_t value) -> void
{
	m_data.write().SizeRoofY = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexGableY() const -> uint16_t
{
	return m_data->TextureIndexGableY;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexGableY(uint16_t value) -> void
{
	m_data.write().TextureIndexGableY = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetGableTexture() const -> uint16_t
{
	return m_data->GableTexture;
}

auto flightsimlib::io::CBglGenericBuilding::SetGableTexture(uint16_t value) -> void
{
	m_data.write().GableTexture = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexGableZ() const -> uint16_t
{
	return m_data->TextureIndexGableZ;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexGableZ(uint16_t value) -> void
{
	m_data.write().TextureIndexGableZ = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetFaceTexture() const -> uint16_t
{
	return m_data->FaceTexture;
}

auto flightsimlib::io::CBglGenericBuilding::SetFaceTexture(uint16_t value) -> void
{
	m_data.write().FaceTexture = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexFaceX() const -> uint16_t
{
	return m_data->TextureIndexFaceX;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexFaceX(uint16_t value) -> void
{
	m_data.write().TextureIndexFaceX = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexFaceY() const -> uint16_t
{
	return m_data->TextureIndexFaceY;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexFaceY(uint16_t value) -> void
{
	m_data.write().TextureIndexFaceY = value;
}

auto flightsimlib::io::CBglGenericBuilding::GetTextureIndexRoofY() const -> uint16_t
{
	return m_data->TextureIndexRoofY;
}

auto flightsimlib::io::CBglGenericBuilding::SetTextureIndexRoofY(uint16_t value) -> void
{
	m_data.write().TextureIndexRoofY = value;
}

int flightsimlib::io::CBglGenericBuilding::CalculateSubrecordSize() const
{
	switch (static_cast<EType>(m_data->SubrecordType))
	{
	case EType::RectangularFlatRoof: 
		return 46;
	case EType::RectangularRidgeRoof:
		return 54;
	case EType::RectangularPeakedRoof:
		return 50;
	case EType::RectangularSlantRoof:
		return 60;
	case EType::Pyramidal:
		return 50;
	case EType::Multisided: 
		return 44;
	default:   // NOLINT(clang-diagnostic-covered-switch-default)
		return 0;
	}
}


//******************************************************************************
// CBglLibraryObject
//******************************************************************************  


void flightsimlib::io::CBglLibraryObject::ReadBinary(BinaryFileStream& in)
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto data = m_data.write();
		in >> data.Name
			>> data.Scale;
	}
}

void flightsimlib::io::CBglLibraryObject::WriteBinary(BinaryFileStream& out)
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		out << m_data->Name
			<< m_data->Scale;
	}
}

bool flightsimlib::io::CBglLibraryObject::Validate()
{
	return true;
}

int flightsimlib::io::CBglLibraryObject::CalculateSize() const
{
	return CBglSceneryObject::CalculateSize() + static_cast<int>(sizeof(SBglLibraryObjectData));
}

_GUID flightsimlib::io::CBglLibraryObject::GetName() const
{
	return m_data->Name;
}

void flightsimlib::io::CBglLibraryObject::SetName(_GUID value)
{
	m_data.write().Name = value;
}

float flightsimlib::io::CBglLibraryObject::GetScale() const
{
	return m_data->Scale;
}

void flightsimlib::io::CBglLibraryObject::SetScale(float value)
{
	m_data.write().Scale = value;
}


//******************************************************************************
// CBglEffect
//******************************************************************************  


void flightsimlib::io::CBglEffect::ReadBinary(BinaryFileStream& in)
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto data = m_data.write();

		data.Name.resize(s_name_size);
		char input[s_name_size];
		in.Read(&input, s_name_size);
		data.Name.assign(input, s_name_size);

		const auto param_size = RecordSize() - CBglSceneryObject::CalculateSize() - s_name_size;
		data.Params = in.ReadString(param_size);
	}
}

void flightsimlib::io::CBglEffect::WriteBinary(BinaryFileStream& out)
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		if (m_data->Name.size() != s_name_size)
		{
			m_data.write().Name.resize(s_name_size);
		}
		out.Write(m_data->Name.data(), s_name_size);

		const auto* const params = m_data->Params.c_str();
		out.Write(params, strlen(params));
	}
}

bool flightsimlib::io::CBglEffect::Validate()
{
	return true;
}

int flightsimlib::io::CBglEffect::CalculateSize() const
{
	return CBglSceneryObject::CalculateSize() + 
		s_name_size + static_cast<int>(strlen(m_data->Params.c_str()));
}

const char* flightsimlib::io::CBglEffect::GetName() const
{
	return m_data->Name.c_str();
}

void flightsimlib::io::CBglEffect::SetName(const char* value)
{
	m_data.write().Name = value;
}

const char* flightsimlib::io::CBglEffect::GetParams() const
{
	return m_data->Params.c_str();
}

void flightsimlib::io::CBglEffect::SetParams(const char* value)
{
	m_data.write().Params = value;
}


//******************************************************************************
// CBglTaxiwaySign
//****************************************************************************** 


auto flightsimlib::io::CBglTaxiwaySign::ReadBinary(BinaryFileStream& in) -> void
{
	auto data = m_data.write();
	in >> data.LongitudeBias
		>> data.LatitudeBias
		>> data.Heading
		>> data.Size
		>> data.Justification;
	
	const auto param_size = CalculateSize() - s_record_size;
	data.Label = in.ReadString(param_size);
}

auto flightsimlib::io::CBglTaxiwaySign::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->LongitudeBias
		<< m_data->LatitudeBias
		<< m_data->Heading
		<< m_data->Size
		<< m_data->Justification;

	const auto* const label = m_data->Label.c_str();
	const auto length = static_cast<int>(strlen(label));
	out.Write(label, length);

	// TODO - move pad to util
	const auto num_pad = 2 - length % 2;
	if (num_pad == 2)
	{
		return;
	}
	
	const auto pad = uint8_t{ 0 };
	for (auto i = 0; i < num_pad; ++i)
	{
		out << pad;
	}
}

auto flightsimlib::io::CBglTaxiwaySign::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwaySign::CalculateSize() const -> int
{
	const auto label_length = static_cast<int>(strlen(m_data->Label.c_str()));
	return s_record_size + label_length + label_length % 2 ? 1 : 0;
}

auto flightsimlib::io::CBglTaxiwaySign::GetLongitudeBias() const -> float
{
	return m_data->LongitudeBias;
}

auto flightsimlib::io::CBglTaxiwaySign::SetLongitudeBias(float value) -> void
{
	m_data.write().LongitudeBias = value;
}

auto flightsimlib::io::CBglTaxiwaySign::GetLatitudeBias() const -> float
{
	return m_data->LatitudeBias;
}

auto flightsimlib::io::CBglTaxiwaySign::SetLatitudeBias(float value) -> void
{
	m_data.write().LatitudeBias = value;
}

auto flightsimlib::io::CBglTaxiwaySign::GetHeading() const -> float
{
	return static_cast<float>(ANGLE16::Value(m_data->Heading));
}

auto flightsimlib::io::CBglTaxiwaySign::SetHeading(float value) -> void
{
	m_data.write().Heading = ANGLE16::FromDouble(static_cast<double>(value));
}

auto flightsimlib::io::CBglTaxiwaySign::GetSize() const -> ESize
{
	return static_cast<ESize>(m_data->Size);
}

auto flightsimlib::io::CBglTaxiwaySign::SetSize(ESize value) -> void
{
	m_data.write().Size = to_integral(value);
}

auto flightsimlib::io::CBglTaxiwaySign::GetJustification() const -> EJustification
{
	return static_cast<EJustification>(m_data->Justification);
}

auto flightsimlib::io::CBglTaxiwaySign::SetJustification(EJustification value) -> void
{
	m_data.write().Justification = to_integral(value);
}

auto flightsimlib::io::CBglTaxiwaySign::GetLabel() const -> const char*
{
	return m_data->Label .c_str();
}

auto flightsimlib::io::CBglTaxiwaySign::SetLabel(const char* value) -> void
{
	m_data.write().Label = value;
}


//******************************************************************************
// CBglTaxiwaySigns
//****************************************************************************** 


auto flightsimlib::io::CBglTaxiwaySigns::ReadBinary(BinaryFileStream& in) -> void
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		if (!m_signs.read().empty())
		{
			m_signs.write().clear();
		}
		auto sign_count = uint16_t{ 0 };
		in >> sign_count;
		m_signs.write().resize(sign_count);

		for (auto i = 0; i < sign_count; ++i)
		{
			if (!in)
			{
				break;
			}
			m_signs.write()[i].ReadBinary(in);
		}
	}
}

auto flightsimlib::io::CBglTaxiwaySigns::WriteBinary(BinaryFileStream& out) -> void
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		const auto sign_count = static_cast<uint16_t>(m_signs.read().size());
		out << sign_count;

		for (auto i = 0; i < sign_count; ++i)
		{
			if (!out)
			{
				break;
			}
			m_signs.write()[i].WriteBinary(out);
		}
	}
}

auto flightsimlib::io::CBglTaxiwaySigns::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwaySigns::CalculateSize() const -> int
{
	auto size = static_cast<int>(CBglSceneryObject::CalculateSize() + sizeof(uint16_t));
	for (const auto& sign : m_signs.read())
	{
		size += sign.CalculateSize();
	}
	return size;
}

auto flightsimlib::io::CBglTaxiwaySigns::GetSignCount() const -> int
{
	return static_cast<int>(m_signs.read().size());
}

auto flightsimlib::io::CBglTaxiwaySigns::GetSignAt(int index) const -> const IBglTaxiwaySign*
{
	return &(m_signs.read()[index]);
}

auto flightsimlib::io::CBglTaxiwaySigns::AddSign(const IBglTaxiwaySign* sign) -> void
{
	// TODO Need validation, self check
	m_signs.write().emplace_back(*static_cast<const CBglTaxiwaySign*>(sign));
}

auto flightsimlib::io::CBglTaxiwaySigns::RemoveSign(const IBglTaxiwaySign* sign) -> void
{
	const auto iter = m_signs.read().begin() + 
		std::distance(m_signs.read().data(), static_cast<const CBglTaxiwaySign*>(sign));
	m_signs.write().erase(iter);
}


//******************************************************************************
// CBglWindsock
//******************************************************************************  


void flightsimlib::io::CBglWindsock::ReadBinary(BinaryFileStream& in)
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto data = m_data.write();
		in >> data.InstanceId
			>> data.PoleHeight
			>> data.WindsockLength
			>> data.PoleColor
			>> data.SockColor
			>> data.Lighted;
	}
}

void flightsimlib::io::CBglWindsock::WriteBinary(BinaryFileStream& out)
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		out << m_data->InstanceId
			<< m_data->PoleHeight
			<< m_data->WindsockLength
			<< m_data->PoleColor
			<< m_data->SockColor
			<< m_data->Lighted;
	}
}

bool flightsimlib::io::CBglWindsock::Validate()
{
	return true;
}

int flightsimlib::io::CBglWindsock::CalculateSize() const
{
	return CBglSceneryObject::CalculateSize() + static_cast<int>(sizeof(SBglWindsockData));
}

_GUID flightsimlib::io::CBglWindsock::GetInstanceId() const
{
	return m_data->InstanceId;
}

void flightsimlib::io::CBglWindsock::SetInstanceId(_GUID value)
{
	m_data.write().InstanceId = value;
}

float flightsimlib::io::CBglWindsock::GetPoleHeight() const
{
	return m_data->PoleHeight;
}

void flightsimlib::io::CBglWindsock::SetPoleHeight(float value)
{
	m_data.write().PoleHeight = value;
}

float flightsimlib::io::CBglWindsock::GetSockLength() const
{
	return m_data->WindsockLength;
}

void flightsimlib::io::CBglWindsock::SetSockLength(float value)
{
	m_data.write().WindsockLength = value;
}

uint32_t flightsimlib::io::CBglWindsock::GetPoleColor() const
{
	return m_data->PoleColor;
}

void flightsimlib::io::CBglWindsock::SetPoleColor(uint32_t value)
{
	m_data.write().PoleColor = value;
}

uint32_t flightsimlib::io::CBglWindsock::GetSockColor() const
{
	return m_data->SockColor;
}

void flightsimlib::io::CBglWindsock::SetSockColor(uint32_t value)
{
	m_data.write().SockColor = value;
}

bool flightsimlib::io::CBglWindsock::IsLighted() const
{
	return m_data->Lighted == 1;
}

void flightsimlib::io::CBglWindsock::SetLighted(bool value)
{
	m_data.write().Lighted = static_cast<uint16_t>(value);
}


//******************************************************************************
// CBglBeacon
//****************************************************************************** 


void flightsimlib::io::CBglBeacon::ReadBinary(BinaryFileStream& in)
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto packed = uint16_t{};
		in >> packed;
		
		switch (packed)
		{
		case 501:
			m_data.write().Type = static_cast<uint8_t>(EType::Civilian);
			m_data.write().BaseType = static_cast<uint8_t>(EBaseType::Airport);
			break;
		case 502:
			m_data.write().Type = static_cast<uint8_t>(EType::Civilian);
			m_data.write().BaseType = static_cast<uint8_t>(EBaseType::Heliport);
			break;
		case 503:
			m_data.write().Type = static_cast<uint8_t>(EType::Civilian);
			m_data.write().BaseType = static_cast<uint8_t>(EBaseType::SeaBase);
			break;
		case 504:
			m_data.write().Type = static_cast<uint8_t>(EType::Military);
			m_data.write().BaseType = static_cast<uint8_t>(EBaseType::Airport);
			break;
		case 505:
			m_data.write().Type = static_cast<uint8_t>(EType::Military);
			m_data.write().BaseType = static_cast<uint8_t>(EBaseType::Heliport);
			break;
		case 506:
			m_data.write().Type = static_cast<uint8_t>(EType::Military);
			m_data.write().BaseType = static_cast<uint8_t>(EBaseType::SeaBase);
			break;
		default: 
			break;
		}
	}
}

void flightsimlib::io::CBglBeacon::WriteBinary(BinaryFileStream& out)
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		const auto type = static_cast<EType>(m_data->Type);
		const auto base_type = static_cast<EBaseType>(m_data->BaseType);
		auto data = uint16_t{};

		switch (base_type)
		{
		case EBaseType::Airport:
			data = type == EType::Civilian ? 501 : 504;
			break;
		case EBaseType::Heliport:
			data = type == EType::Civilian ? 502 : 505;
			break;
		case EBaseType::SeaBase:
			data = type == EType::Civilian ? 503 : 506;
			break;
		}

		out << data;
	}
}

bool flightsimlib::io::CBglBeacon::Validate()
{
	return true;
}

int flightsimlib::io::CBglBeacon::CalculateSize() const
{
	return CBglSceneryObject::CalculateSize() + static_cast<int>(sizeof(SBglBeaconData));
}

auto flightsimlib::io::CBglBeacon::GetBaseType() const -> EBaseType
{
	return static_cast<EBaseType>(m_data->BaseType);
}

void flightsimlib::io::CBglBeacon::SetBaseType(EBaseType value)
{
	m_data.write().BaseType = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglBeacon::GetType() const -> EType
{
	return static_cast<EType>(m_data->Type);
}

void flightsimlib::io::CBglBeacon::SetType(EType value)
{
	m_data.write().Type = static_cast<uint8_t>(value);
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
		m_data.write().DataOffset = in.GetPosition();
		m_data.write().DataLength = m_header->SizeData; // TODO consistent naming!
		if(m_header->SizeMask > 0)
		{
			m_data.write().MaskOffset = m_data->DataOffset + m_data->DataLength;
			m_data.write().MaskLength = m_header->SizeMask;
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
	case ERasterCompressionType::Max:
		throw std::exception("Unsupported Compression Type");
	}

	return uncompressed;
}
