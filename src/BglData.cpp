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


//******************************************************************************
// CBglRunway
//******************************************************************************  


void flightsimlib::io::CBglRunway::ReadBinary(BinaryFileStream& in)
{
	in >> m_data.write().Type
		>> m_data.write().Size
		>> m_data.write().SurfaceType
		>> m_data.write().NumberPrimary
		>> m_data.write().DesignatorPrimary
		>> m_data.write().NumberSecondary
		>> m_data.write().DesignatorSecondary
		>> m_data.write().IlsIcaoPrimary
		>> m_data.write().IlsIcaoSecondary
		>> m_data.write().Latitude
		>> m_data.write().Longitude
		>> m_data.write().Altitude
		>> m_data.write().Length
		>> m_data.write().Width
		>> m_data.write().Heading
		>> m_data.write().PatternAltitude
		>> m_data.write().MarkingFlags
		>> m_data.write().LightingFlags
		>> m_data.write().PatternFlags;
}

void flightsimlib::io::CBglRunway::WriteBinary(BinaryFileStream& out)
{
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

float flightsimlib::io::CBglRunway::GetLength() const
{
	return m_data->Length;
}

float flightsimlib::io::CBglRunway::GetWidth() const
{
	return m_data->Width;
}

float flightsimlib::io::CBglRunway::GetHeading() const
{
	return m_data->Heading;
}

float flightsimlib::io::CBglRunway::GetPatternAltitude() const
{
	return m_data->PatternAltitude;
}


//******************************************************************************
// CBglAirport
//******************************************************************************  


void flightsimlib::io::CBglAirport::ReadBinary(BinaryFileStream& in)
{
	const auto initial_pos = in.GetPosition();

	in >> m_data.write().Type
		>> m_data.write().Size
		>> m_data.write().RunwayCount
		>> m_data.write().FrequencyCount
		>> m_data.write().StartCount
		>> m_data.write().ApproachCount
		>> m_data.write().ApronCount
		>> m_data.write().HelipadCount
		>> m_data.write().ReferenceLat
		>> m_data.write().ReferenceLon
		>> m_data.write().ReferenceAlt
		>> m_data.write().TowerLatitude
		>> m_data.write().TowerLongitude
		>> m_data.write().TowerAltitude
		>> m_data.write().MagVar
		>> m_data.write().Icao
		>> m_data.write().RegionIdent
		>> m_data.write().FuelTypes
		>> m_data.write().Flags;

	const auto final_position = initial_pos + static_cast<int>(m_data->Size);
	while (in.GetPosition() < final_position)
	{
		const auto child_pos = in.GetPosition();
		uint16_t type = 0;
		uint32_t size = 0;
		in >> type >> size;
		in.SetPosition(child_pos);

		// TODO enum
		switch (type)
		{
		case 0x4: // runway
			{
				auto runway = CBglRunway{};
				runway.ReadBinary(in);
				m_runways.emplace_back(runway);
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
		<< m_data->ReferenceLat
		<< m_data->ReferenceLon
		<< m_data->ReferenceAlt
		<< m_data->TowerLatitude
		<< m_data->TowerLongitude
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
	in >> m_data.write().Type
		>> m_data.write().Size
		>> m_data.write().LonWest
		>> m_data.write().LatNorth
		>> m_data.write().LonEast
		>> m_data.write().LatSouth;
}

void flightsimlib::io::CBglExclusion::WriteBinary(BinaryFileStream& out)
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->LonWest
		<< m_data->LatNorth
		<< m_data->LonEast
		<< m_data->LatSouth;
}

bool flightsimlib::io::CBglExclusion::Validate()
{
	return true;
}

int flightsimlib::io::CBglExclusion::CalculateSize() const
{
	return sizeof(SBglExclusionData);
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
	in >> m_data.write().SectionType
		>> m_data.write().Size
		>> m_data.write().UnusedType
		>> m_data.write().Heading
		>> m_data.write().MarkerType
		>> m_data.write().Latitude
		>> m_data.write().Longitude
		>> m_data.write().Altitude
		>> m_data.write().Identifier
		>> m_data.write().Region
		>> m_data.write().Unknown;
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
	return sizeof(SBglExclusionData);
}

float flightsimlib::io::CBglMarker::GetHeading() const
{
	// TODO Heading Conversion
	return (float)m_data->Heading;
}

void flightsimlib::io::CBglMarker::SetHeading(float value)
{
	// TODO Heading Conversion
	m_data.write().Heading = (uint16_t)value;
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
	in >> m_header.write().Version
		>> m_header.write().Size
		>> m_header.write().DataType
		>> m_header.write().CompressionTypeData
		>> m_header.write().CompressionTypeMask
		>> m_header.write().QmidLow
		>> m_header.write().QmidHigh
		>> m_header.write().Variations
		>> m_header.write().Rows
		>> m_header.write().Cols
		>> m_header.write().SizeData
		>> m_header.write().SizeMask;
	
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
	//std::shared_ptr<int[]> sp(new int[10]);
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
