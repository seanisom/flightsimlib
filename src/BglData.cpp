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
// File:     BglData.cpp
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

template <typename T>
T get_packed_bits(T src, int num_bits, int position)
{
	return (src >> position) & (1 << (num_bits - 1));
}


template <typename T>
void set_packed_bits(T& dest, int value, int num_bits, int position)
{
	dest &= ~((1 << (num_bits - 1)) << position);
	dest |= static_cast<uint32_t>(value) << position;
}


//******************************************************************************
// CBglFuelAvailability
//******************************************************************************  


template <typename T>
flightsimlib::io::CBglFuelAvailability<T>::CBglFuelAvailability(T& data) : m_data(data) { }

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Get73Octane() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Octane73)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Set73Octane(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Octane73));

	UpdateAvgasAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Get87Octane() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Octane87)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Set87Octane(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Octane87));

	UpdateAvgasAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Get100Octane() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Octane100)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Set100Octane(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Octane100));

	UpdateAvgasAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Get130Octane() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Octane130)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Set130Octane(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Octane130));

	UpdateAvgasAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Get145Octane() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Octane145)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::Set145Octane(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Octane145));

	UpdateAvgasAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetMogas() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Mogas)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetMogas(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Mogas));

	UpdateAvgasAvailability(value); // TODO - verify mogas counts as avgas?
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetJet() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Jet)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetJet(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Jet));

	UpdateJetFuelAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetJetA() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::JetA)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetJetA(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::JetA));

	UpdateJetFuelAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetJetA1() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::JetA1)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetJetA1(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::JetA1));

	UpdateJetFuelAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetJetAP() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::JetAP)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetJetAP(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::JetAP));

	UpdateJetFuelAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetJetB() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::JetB)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetJetB(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::JetB));

	UpdateJetFuelAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetJet4() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Jet4)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetJet4(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Jet4));

	UpdateJetFuelAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::GetJet5() const -> EFuelAvailability
{
	return static_cast<EFuelAvailability>(
		get_packed_bits(m_data->FuelAvailability, s_num_availability_bits, to_integral(EFuelBits::Jet5)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::SetJet5(EFuelAvailability value) -> void
{
	set_packed_bits(m_data.write().FuelAvailability, to_integral(value),
		s_num_availability_bits, to_integral(EFuelBits::Jet5));

	UpdateJetFuelAvailability(value);
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::HasAvgas() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->FuelAvailability, 1, to_integral(EFuelBits::Avgas)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::HasJetFuel() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->FuelAvailability, 1, to_integral(EFuelBits::JetFuel)));
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::UpdateAvgasAvailability(EFuelAvailability value)
{
	if (value == EFuelAvailability::Yes && !HasAvgas())
	{
		set_packed_bits(m_data.write().FuelAvailability, true,
			1, to_integral(EFuelBits::Avgas));
	}
	else if (value != EFuelAvailability::Yes && HasAvgas())
	{
		if (get_packed_bits(m_data->FuelAvailability,
			to_integral(EFuelBits::Jet), to_integral(EFuelBits::Octane73)) == 0)
		{
			set_packed_bits(m_data.write().FuelAvailability, false,
				1, to_integral(EFuelBits::Avgas));
		}
	}
}

template <typename T>
auto flightsimlib::io::CBglFuelAvailability<T>::UpdateJetFuelAvailability(EFuelAvailability value)
{
	if (value == EFuelAvailability::Yes && !HasJetFuel())
	{
		set_packed_bits(m_data.write().FuelAvailability, true,
			1, to_integral(EFuelBits::JetFuel));
	}
	else if (value != EFuelAvailability::Yes && HasJetFuel())
	{
		if (get_packed_bits(m_data->FuelAvailability,
			to_integral(EFuelBits::Reserved1), to_integral(EFuelBits::Jet)) == 0)
		{
			set_packed_bits(m_data.write().FuelAvailability, false,
				1, to_integral(EFuelBits::JetFuel));
		}
	}
}


//******************************************************************************
// CBglLLA
//******************************************************************************  


template <typename T>
flightsimlib::io::CBglLLA<T>::CBglLLA(T& data) : m_data(data) { }

template <typename T>
auto flightsimlib::io::CBglLLA<T>::GetLongitude() const -> double
{
	return Longitude::Value(m_data.get()->Longitude);
}

template <typename T>
auto flightsimlib::io::CBglLLA<T>::SetLongitude(double value) -> void
{
	m_data.get().write().Longitude = Longitude::ToPacked(value);
}

template <typename T>
auto flightsimlib::io::CBglLLA<T>::GetLatitude() const -> double
{
	return Latitude::Value(m_data.get()->Latitude);
}

template <typename T>
auto flightsimlib::io::CBglLLA<T>::SetLatitude(double value) -> void
{
	m_data.get().write().Latitude = Latitude::ToPacked(value);
}

template <typename T>
auto flightsimlib::io::CBglLLA<T>::GetAltitude() const -> double
{
	return PackedAltitude::Value(m_data.get()->Altitude);
}

template <typename T>
auto flightsimlib::io::CBglLLA<T>::SetAltitude(double value) -> void
{
	m_data.get().write().Altitude = PackedAltitude::FromDouble(value);
}

class CBglString
{
public:
	template <typename T>
	static auto CalculatePadSize(T& m_data, const int remaining_size, int pad_to = 4) -> int
	{
		const auto remainder = static_cast<int>(m_data->Name.size() +
			remaining_size) % pad_to;
		return pad_to - (remainder == 0 ? pad_to : remainder);
	}
	
	template <typename T>
	static auto ReadBinary(T& m_data, const int remaining_size, flightsimlib::io::BinaryFileStream& in) -> void
	{
		m_data.write().Name = in.ReadString(static_cast<int>(m_data->Size) - remaining_size);
		m_data.write().Name.erase(std::find(
			m_data->Name.begin(), m_data->Name.end(), '\0'), m_data->Name.end());
	}

	template <typename T>
	static auto WriteBinary(T& m_data, const int remaining_size, flightsimlib::io::BinaryFileStream& out) -> void
	{
		out.Write(m_data->Name.c_str(), static_cast<int>(m_data->Name.size()));

		const auto num_pad = CalculatePadSize(m_data, remaining_size);
		const auto pad = uint8_t{ 0 };
		for (auto i = 0; i < num_pad; ++i)
		{
			out << pad;
		}
	}
};


//******************************************************************************
// CBglName
//******************************************************************************  


auto flightsimlib::io::CBglName::ReadBinary(BinaryFileStream& in) -> void
{
	in >> m_data.write().Type
		>> m_data.write().Size;

	CBglString::ReadBinary(m_data, CalculateRemainingSize(), in);
}

auto flightsimlib::io::CBglName::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size;

	CBglString::WriteBinary(m_data, CalculateRemainingSize(), out);
}

auto flightsimlib::io::CBglName::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglName::CalculateSize() const -> int
{
	return static_cast<int>(CalculateRemainingSize() + m_data->Name.size() + 
		CBglString::CalculatePadSize(m_data, CalculateRemainingSize()));
}

auto flightsimlib::io::CBglName::GetName() const -> const char*
{
	return m_data->Name.c_str();
}

auto flightsimlib::io::CBglName::SetName(const char* value) -> void
{
	m_data.write().Name = value;
}

auto flightsimlib::io::CBglName::CalculateRemainingSize() const -> int
{
	return static_cast<int>(sizeof(m_data->Type) + sizeof(m_data->Size));
}


//******************************************************************************
// CBglNdb
//******************************************************************************  


auto flightsimlib::io::CBglNdb::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.NdbType
		>> data.Frequency
		>> data.Longitude
		>> data.Latitude
		>> data.Altitude
		>> data.Range
		>> data.MagVar
		>> data.Icao
		>> data.Region;
	
	CBglName::ReadBinary(in);
}

auto flightsimlib::io::CBglNdb::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->NdbType
		<< m_data->Frequency
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Altitude
		<< m_data->Range
		<< m_data->MagVar
		<< m_data->Icao
		<< m_data->Region;
	
	CBglName::WriteBinary(out);
}

auto flightsimlib::io::CBglNdb::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglNdb::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglNdbData)) + CBglName::CalculateSize();
}

auto flightsimlib::io::CBglNdb::GetType() const -> EType
{
	return static_cast<EType>(m_data->NdbType);
}

auto flightsimlib::io::CBglNdb::SetType(EType value) -> void
{
	m_data.write().NdbType = to_integral(value);
}

auto flightsimlib::io::CBglNdb::GetFrequency() const -> uint32_t
{
	return m_data->Frequency;
}

auto flightsimlib::io::CBglNdb::SetFrequency(uint32_t value) -> void
{
	m_data.write().Frequency = value;
}

auto flightsimlib::io::CBglNdb::GetRange() const -> float
{
	return m_data->Range;
}

auto flightsimlib::io::CBglNdb::SetRange(float value) -> void
{
	m_data.write().MagVar = value;
}

auto flightsimlib::io::CBglNdb::GetMagVar() const -> float
{
	return m_data->MagVar;
}

auto flightsimlib::io::CBglNdb::SetMagVar(float value) -> void
{
	m_data.write().MagVar = value;
}

auto flightsimlib::io::CBglNdb::GetIcao() const -> uint32_t
{
	return m_data->Icao;
}

auto flightsimlib::io::CBglNdb::SetIcao(uint32_t value) -> void
{
	m_data.write().Icao = value;
}

auto flightsimlib::io::CBglNdb::GetRegion() const -> uint32_t
{
	return m_data->Region;
}

auto flightsimlib::io::CBglNdb::SetRegion(uint32_t value) -> void
{
	m_data.write().Region = value;
}


//******************************************************************************
// CBglRunwayEnd
//******************************************************************************  


void flightsimlib::io::CBglRunway::CBglRunwayEnd::ReadBinary(BinaryFileStream& in)
{
	auto& data = m_data.write();
	in >> data.Position
		>> data.Size
		>> data.SurfaceType
		>> data.Length
		>> data.Width;
}

void flightsimlib::io::CBglRunway::CBglRunwayEnd::WriteBinary(BinaryFileStream& out)
{
	out << m_data->Position
		<< m_data->Size
		<< m_data->SurfaceType
		<< m_data->Length
		<< m_data->Width;
}

bool flightsimlib::io::CBglRunway::CBglRunwayEnd::Validate()
{
	return true;
}

int flightsimlib::io::CBglRunway::CBglRunwayEnd::CalculateSize() const
{
	return static_cast<int>(sizeof(SBglRunwayEndData));
}

auto flightsimlib::io::CBglRunway::CBglRunwayEnd::GetPosition() const -> EPosition
{
	return static_cast<EPosition>(m_data->Position);
}

flightsimlib::io::ESurfaceType flightsimlib::io::CBglRunway::CBglRunwayEnd::GetSurfaceType() const
{
	return static_cast<ESurfaceType>(m_data->SurfaceType);
}

void flightsimlib::io::CBglRunway::CBglRunwayEnd::SetSurfaceType(ESurfaceType value)
{
	m_data.write().SurfaceType = to_integral(value);
}

float flightsimlib::io::CBglRunway::CBglRunwayEnd::GetLength() const
{
	return m_data->Length;
}

void flightsimlib::io::CBglRunway::CBglRunwayEnd::SetLength(float value)
{
	m_data.write().Length = value;
}

float flightsimlib::io::CBglRunway::CBglRunwayEnd::GetWidth() const
{
	return m_data->Width;
}

void flightsimlib::io::CBglRunway::CBglRunwayEnd::SetWidth(float value)
{
	m_data.write().Width = value;
}

bool flightsimlib::io::CBglRunway::CBglRunwayEnd::IsEmpty() const
{
	if (m_data->Position == 0)
	{
		return true;
	}
	return false;
}

auto flightsimlib::io::CBglRunway::CBglRunwayEnd::SetPosition(EPosition value) -> void
{
	m_data.write().Position = to_integral(value);
}


//******************************************************************************
// CBglRunwayVasi
//****************************************************************************** 


auto flightsimlib::io::CBglRunway::CBglRunwayVasi::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Position
		>> data.Size
		>> data.Type
		>> data.BiasX
		>> data.BiasZ
		>> data.Spacing
		>> data.Pitch;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Position
		<< m_data->Size
		<< m_data->Type
		<< m_data->BiasX
		<< m_data->BiasZ
		<< m_data->Spacing
		<< m_data->Pitch;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglRunwayVasiData));
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::GetPosition() const -> EPosition
{
	return static_cast<EPosition>(m_data->Position);
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::GetType() const -> EType
{
	return static_cast<EType>(m_data->Type);
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::SetType(EType value) -> void
{
	m_data.write().Type = to_integral(value);
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::GetBiasX() const -> float
{
	return m_data->BiasX;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::SetBiasX(float value) -> void
{
	m_data.write().BiasX = value;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::GetBiasZ() const -> float
{
	return m_data->BiasZ;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::SetBiasZ(float value) -> void
{
	m_data.write().BiasZ = value;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::GetSpacing() const -> float
{
	return m_data->Spacing;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::SetSpacing(float value) -> void
{
	m_data.write().Spacing = value;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::GetPitch() const -> float
{
	return m_data->Pitch;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::SetPitch(float value) -> void
{
	m_data.write().Pitch = value;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::IsEmpty() const -> bool
{
	if (m_data->Position == 0)
	{
		return true;
	}
	return false;
}

auto flightsimlib::io::CBglRunway::CBglRunwayVasi::SetPosition(EPosition value) -> void
{
	m_data.write().Position = to_integral(value);
}


//******************************************************************************
// CBglRunwayApproachLights
//******************************************************************************  


auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Position
		>> data.Size
		>> data.Type
		>> data.Strobes;
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Position
		<< m_data->Size
		<< m_data->Type
		<< m_data->Strobes;
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglRunwayApproachLightsData));
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::GetPosition() const -> EPosition
{
	return static_cast<EPosition>(m_data->Position);
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::GetType() const -> EType
{
	return static_cast<EType>(get_packed_bits(m_data->Type, 5, 0));
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::SetType(EType value) -> void
{
	set_packed_bits(m_data.write().Type, to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::GetStrobeCount() const -> int
{
	return static_cast<int>(m_data->Strobes);
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::SetStrobeCount(int value) -> void
{
	m_data.write().Strobes = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::HasEndLights() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Type, 1, 5));
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::SetEndLights(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, 5);
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::HasReilLights() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Type, 1, 6));
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::SetReilLights(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, 6);
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::HasTouchdownLights() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Type, 1, 7));
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::SetTouchdownLights(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, 7);
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::IsEmpty() const -> bool
{
	if (m_data->Position == 0)
	{
		return true;
	}
	return false;
}

auto flightsimlib::io::CBglRunway::CBglRunwayApproachLights::SetPosition(EPosition value) -> void
{
	m_data.write().Position = to_integral(value);
}


//******************************************************************************
// CBglRunway
//******************************************************************************  


void flightsimlib::io::CBglRunway::ReadBinary(BinaryFileStream& in)
{
	auto& data = m_data.write();

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
		>> data.LightFlags
		>> data.PatternFlags;

	const auto final_position = initial_pos + static_cast<int>(m_data->Size);
	while (in.GetPosition() < final_position)
	{
		const auto child_pos = in.GetPosition();
		uint16_t type = 0;
		uint32_t size = 0;
		in >> type >> size;
		in.SetPosition(child_pos);

		switch (static_cast<EBglLayerType>(type))  // NOLINT(clang-diagnostic-switch-enum)
		{
		case EBglLayerType::PrimaryOffsetThreshold:
			m_primary_offset_threshold.write().ReadBinary(in);
			break;
		case EBglLayerType::SecondaryOffsetThreshold:
			m_secondary_offset_threshold.write().ReadBinary(in);
			break;
		case EBglLayerType::PrimaryBlastPad:
			m_primary_blast_pad.write().ReadBinary(in);
			break;
		case EBglLayerType::SecondaryBlastPad:
			m_secondary_blast_pad.write().ReadBinary(in);
			break;
		case EBglLayerType::PrimaryOverrun:
			m_primary_overrun.write().ReadBinary(in);
			break;
		case EBglLayerType::SecondaryOverrun:
			m_secondary_overrun.write().ReadBinary(in);
			break;
		case EBglLayerType::PrimaryLeftVasi:
			m_primary_left_vasi.write().ReadBinary(in);
			break;
		case EBglLayerType::PrimaryRightVasi:
			m_primary_right_vasi.write().ReadBinary(in);
			break;
		case EBglLayerType::SecondaryLeftVasi:
			m_secondary_left_vasi.write().ReadBinary(in);
			break;
		case EBglLayerType::SecondaryRightVasi:
			m_secondary_right_vasi.write().ReadBinary(in);
			break;
		case EBglLayerType::PrimaryApproachLights:
			m_primary_approach_lights.write().ReadBinary(in);
			break;
		case EBglLayerType::SecondaryApproachLights:
			m_secondary_approach_lights.write().ReadBinary(in);
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
		<< m_data->LightFlags
		<< m_data->PatternFlags;

	
	if (!m_primary_offset_threshold->IsEmpty())
	{
		m_primary_offset_threshold.write().WriteBinary(out);
	}
	if (!m_secondary_offset_threshold->IsEmpty())
	{
		m_secondary_offset_threshold.write().WriteBinary(out);
	}
	if (!m_primary_blast_pad->IsEmpty())
	{
		m_primary_blast_pad.write().WriteBinary(out);
	}
	if (!m_secondary_blast_pad->IsEmpty())
	{
		m_secondary_blast_pad.write().WriteBinary(out);
	}
	if (!m_primary_overrun->IsEmpty())
	{
		m_primary_overrun.write().WriteBinary(out);
	}
	if (!m_secondary_overrun->IsEmpty())
	{
		m_secondary_overrun.write().WriteBinary(out);
	}
	if (!m_primary_left_vasi->IsEmpty())
	{
		m_primary_left_vasi.write().WriteBinary(out);
	}
	if (!m_primary_right_vasi->IsEmpty())
	{
		m_primary_right_vasi.write().WriteBinary(out);
	}
	if (!m_secondary_left_vasi->IsEmpty())
	{
		m_secondary_left_vasi.write().WriteBinary(out);
	}
	if (!m_secondary_right_vasi->IsEmpty())
	{
		m_secondary_right_vasi.write().WriteBinary(out);
	}
	if (!m_primary_approach_lights->IsEmpty())
	{
		m_primary_approach_lights.write().WriteBinary(out);
	}
	if (!m_secondary_approach_lights->IsEmpty())
	{
		m_secondary_approach_lights.write().WriteBinary(out);
	}
}

bool flightsimlib::io::CBglRunway::Validate()
{
	auto count = static_cast<int>(sizeof(SBglRunwayData));

	if (!m_primary_offset_threshold->IsEmpty())
	{
		count += m_primary_offset_threshold->CalculateSize();
	}
	if (!m_secondary_offset_threshold->IsEmpty())
	{
		count += m_secondary_offset_threshold->CalculateSize();
	}
	if (!m_primary_blast_pad->IsEmpty())
	{
		count += m_primary_blast_pad->CalculateSize();
	}
	if (!m_secondary_blast_pad->IsEmpty())
	{
		count += m_secondary_blast_pad->CalculateSize();
	}
	if (!m_primary_overrun->IsEmpty())
	{
		count += m_primary_overrun->CalculateSize();
	}
	if (!m_secondary_overrun->IsEmpty())
	{
		count += m_secondary_overrun->CalculateSize();
	}
	if (!m_primary_left_vasi->IsEmpty())
	{
		count += m_primary_left_vasi->CalculateSize();
	}
	if (!m_primary_right_vasi->IsEmpty())
	{
		count += m_primary_right_vasi->CalculateSize();
	}
	if (!m_secondary_left_vasi->IsEmpty())
	{
		count += m_secondary_left_vasi->CalculateSize();
	}
	if (!m_secondary_right_vasi->IsEmpty())
	{
		count += m_secondary_right_vasi->CalculateSize();
	}
	if (!m_primary_approach_lights->IsEmpty())
	{
		count += m_primary_approach_lights->CalculateSize();
	}
	if (!m_secondary_approach_lights->IsEmpty())
	{
		count += m_secondary_approach_lights->CalculateSize();
	}
	
	m_data.write().Size = count;
	
	// TODO: This should be set by factory
	return m_data->Type == 0x4;
}

int flightsimlib::io::CBglRunway::CalculateSize() const
{
	return m_data->Size;
}

auto flightsimlib::io::CBglRunway::GetSurfaceType() const -> ESurfaceType
{
	return static_cast<ESurfaceType>(m_data->SurfaceType);
}

auto flightsimlib::io::CBglRunway::SetSurfaceType(ESurfaceType value) -> void
{
	m_data.write().SurfaceType = to_integral(value);
}

auto flightsimlib::io::CBglRunway::GetPrimaryRunwayNumber() const -> ERunwayNumber
{
	return static_cast<ERunwayNumber>(m_data->NumberPrimary);
}

auto flightsimlib::io::CBglRunway::SetPrimaryRunwayNumber(ERunwayNumber value) -> void
{
	m_data.write().NumberPrimary = to_integral(value);
}

auto flightsimlib::io::CBglRunway::GetPrimaryRunwayDesignator() const -> ERunwayDesignator
{
	return static_cast<ERunwayDesignator>(m_data->DesignatorPrimary);
}

auto flightsimlib::io::CBglRunway::SetPrimaryRunwayDesignator(ERunwayDesignator value) -> void
{
	m_data.write().DesignatorPrimary = to_integral(value);
}

auto flightsimlib::io::CBglRunway::GetSecondaryRunwayNumber() const -> ERunwayNumber
{
	return static_cast<ERunwayNumber>(m_data->NumberSecondary);
}

auto flightsimlib::io::CBglRunway::SetSecondaryRunwayNumber(ERunwayNumber value) -> void
{
	m_data.write().NumberSecondary = to_integral(value);
}

auto flightsimlib::io::CBglRunway::GetSecondaryRunwayDesignator() const -> ERunwayDesignator
{
	return static_cast<ERunwayDesignator>(m_data->DesignatorSecondary);
}

auto flightsimlib::io::CBglRunway::SetSecondaryRunwayDesignator(ERunwayDesignator value) -> void
{
	m_data.write().DesignatorSecondary = to_integral(value);
}

auto flightsimlib::io::CBglRunway::GetPrimaryIcaoIdent() const -> uint32_t
{
	return m_data->IlsIcaoPrimary;
}

auto flightsimlib::io::CBglRunway::SetPrimaryIcaoIdent(uint32_t value) -> void
{
	m_data.write().IlsIcaoPrimary = value;
}

auto flightsimlib::io::CBglRunway::GetSecondaryIcaoIdent() const -> uint32_t
{
	return m_data->IlsIcaoSecondary;
}

auto flightsimlib::io::CBglRunway::SetSecondaryIcaoIdent(uint32_t value) -> void
{
	m_data.write().IlsIcaoSecondary = value;
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

auto flightsimlib::io::CBglRunway::HasEdgeMarkings() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1, 
		to_integral(EMarkingFlags::Edges)));
}

auto flightsimlib::io::CBglRunway::SetEdgeMarkings(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::Edges));
}

auto flightsimlib::io::CBglRunway::HasThresholdMarkings() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::Threshold)));
}

auto flightsimlib::io::CBglRunway::SetThresholdMarkings(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::Threshold));
}

auto flightsimlib::io::CBglRunway::HasFixedDistanceMarkings() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::FixedDistance)));
}

auto flightsimlib::io::CBglRunway::SetFixedDistanceMarkings(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::FixedDistance));
}

auto flightsimlib::io::CBglRunway::HasTouchdownMarkings() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::Touchdown)));
}

auto flightsimlib::io::CBglRunway::SetTouchdownMarkings(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::Touchdown));
}

auto flightsimlib::io::CBglRunway::HasDashMarkings() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::Dashes)));
}

auto flightsimlib::io::CBglRunway::SetDashMarkings(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::Dashes));
}

auto flightsimlib::io::CBglRunway::HasIdentMarkings() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::Ident)));
}

auto flightsimlib::io::CBglRunway::SetIdentMarkings(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::Ident));
}

auto flightsimlib::io::CBglRunway::HasPrecisionMarkings() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::Precision)));
}

auto flightsimlib::io::CBglRunway::SetPrecisionMarkings(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::Precision));
}

auto flightsimlib::io::CBglRunway::HasEdgePavement() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::EdgePavement)));
}

auto flightsimlib::io::CBglRunway::SetEdgePavement(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::EdgePavement));
}

auto flightsimlib::io::CBglRunway::IsSingleEnd() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::SingleEnd)));
}

auto flightsimlib::io::CBglRunway::SetSingleEnd(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::SingleEnd));
}

auto flightsimlib::io::CBglRunway::IsPrimaryClosed() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::PrimaryClosed)));
}

auto flightsimlib::io::CBglRunway::SetPrimaryClosed(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::PrimaryClosed));
}

auto flightsimlib::io::CBglRunway::IsSecondaryClosed() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::SecondaryClosed)));
}

auto flightsimlib::io::CBglRunway::SetSecondaryClosed(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::SecondaryClosed));
}

auto flightsimlib::io::CBglRunway::IsPrimaryStol() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::PrimaryStol)));
}

auto flightsimlib::io::CBglRunway::SetPrimaryStol(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::PrimaryStol));
}

auto flightsimlib::io::CBglRunway::IsSecondaryStol() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::SecondaryStol)));
}

auto flightsimlib::io::CBglRunway::SetSecondaryStol(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::SecondaryStol));
}

auto flightsimlib::io::CBglRunway::HasAlternateThreshold() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::AlternateThreshold)));
}

auto flightsimlib::io::CBglRunway::SetAlternateThreshold(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::AlternateThreshold));
}

auto flightsimlib::io::CBglRunway::HasAlternateFixedDistance() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::AlternateFixedDistance)));
}

auto flightsimlib::io::CBglRunway::SetAlternateFixedDistance(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::AlternateFixedDistance));
}

auto flightsimlib::io::CBglRunway::HasAlternateTouchDown() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1,
		to_integral(EMarkingFlags::AlternateTouchdown)));
}

auto flightsimlib::io::CBglRunway::SetAlternateTouchDown(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1,
		to_integral(EMarkingFlags::AlternateTouchdown));
}

auto flightsimlib::io::CBglRunway::HasAlternatePrecision() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->LightFlags, 1,
		to_integral(ELightFlags::AlternatePrecision)));
}

auto flightsimlib::io::CBglRunway::SetAlternatePrecision(bool value) -> void
{
	set_packed_bits(m_data.write().LightFlags, value, 1,
		to_integral(ELightFlags::AlternatePrecision));
}

auto flightsimlib::io::CBglRunway::HasLeadingZeroIdent() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->LightFlags, 1,
		to_integral(ELightFlags::LeadingZeroIdent)));
}

auto flightsimlib::io::CBglRunway::SetLeadingZeroIdent(bool value) -> void
{
	set_packed_bits(m_data.write().LightFlags, value, 1,
		to_integral(ELightFlags::LeadingZeroIdent));
}

auto flightsimlib::io::CBglRunway::HasNoThresholdEndArrows() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->LightFlags, 1,
		to_integral(ELightFlags::NoThresholdEndArrows)));
}

auto flightsimlib::io::CBglRunway::SetNoThresholdEndArrows(bool value) -> void
{
	set_packed_bits(m_data.write().LightFlags, value, 1,
		to_integral(ELightFlags::NoThresholdEndArrows));
}

auto flightsimlib::io::CBglRunway::GetEdgeLights() const -> ELightIntensity
{
	return static_cast<ELightIntensity>(get_packed_bits(m_data->LightFlags, 2,
		to_integral(ELightFlags::Edge)));
}

auto flightsimlib::io::CBglRunway::SetEdgeLights(ELightIntensity value) -> void
{
	set_packed_bits(m_data.write().LightFlags, to_integral(value), 2,
		to_integral(ELightFlags::Edge));
}

auto flightsimlib::io::CBglRunway::GetCenterLights() const -> ELightIntensity
{
	return static_cast<ELightIntensity>(get_packed_bits(m_data->LightFlags, 2,
		to_integral(ELightFlags::Center)));
}

auto flightsimlib::io::CBglRunway::SetCenterLights(ELightIntensity value) -> void
{
	set_packed_bits(m_data.write().LightFlags, to_integral(value), 2,
		to_integral(ELightFlags::Center));
}

auto flightsimlib::io::CBglRunway::IsCenterRedLights() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->LightFlags, 1,
		to_integral(ELightFlags::CenterRed)));
}

auto flightsimlib::io::CBglRunway::SetCenterRedLights(bool value) -> void
{
	set_packed_bits(m_data.write().LightFlags, value, 1,
		to_integral(ELightFlags::CenterRed));
}

auto flightsimlib::io::CBglRunway::IsPrimaryTakeoff() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->PatternFlags, 1,
		to_integral(EPatternFlags::PrimaryTakeoff)));
}

auto flightsimlib::io::CBglRunway::SetPrimaryTakeoff(bool value) -> void
{
	set_packed_bits(m_data.write().PatternFlags, value, 1,
		to_integral(EPatternFlags::PrimaryTakeoff));
}

auto flightsimlib::io::CBglRunway::IsPrimaryLanding() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->PatternFlags, 1,
		to_integral(EPatternFlags::PrimaryLanding)));
}

auto flightsimlib::io::CBglRunway::SetPrimaryLanding(bool value) -> void
{
	set_packed_bits(m_data.write().PatternFlags, value, 1,
		to_integral(EPatternFlags::PrimaryLanding));
}

auto flightsimlib::io::CBglRunway::IsPrimaryRightPattern() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->PatternFlags, 1,
		to_integral(EPatternFlags::PrimaryPattern)));
}

auto flightsimlib::io::CBglRunway::SetPrimaryRightPattern(bool value) -> void
{
	set_packed_bits(m_data.write().PatternFlags, value, 1,
		to_integral(EPatternFlags::PrimaryPattern));
}

auto flightsimlib::io::CBglRunway::IsSecondaryTakeoff() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->PatternFlags, 1,
		to_integral(EPatternFlags::SecondaryTakeoff)));
}

auto flightsimlib::io::CBglRunway::SetSecondaryTakeoff(bool value) -> void
{
	set_packed_bits(m_data.write().PatternFlags, value, 1,
		to_integral(EPatternFlags::SecondaryTakeoff));
}

auto flightsimlib::io::CBglRunway::IsSecondaryLanding() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->PatternFlags, 1,
		to_integral(EPatternFlags::SecondaryLanding)));
}

auto flightsimlib::io::CBglRunway::SetSecondaryLanding(bool value) -> void
{
	set_packed_bits(m_data.write().PatternFlags, value, 1,
		to_integral(EPatternFlags::SecondaryLanding));
}

auto flightsimlib::io::CBglRunway::IsSecondaryRightPattern() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->PatternFlags, 1,
		to_integral(EPatternFlags::SecondaryPattern)));
}

auto flightsimlib::io::CBglRunway::SetSecondaryRightPattern(bool value) -> void
{
	set_packed_bits(m_data.write().PatternFlags, value, 1,
		to_integral(EPatternFlags::SecondaryPattern));
}

const flightsimlib::io::IBglRunwayEnd* flightsimlib::io::CBglRunway::GetPrimaryOffsetThreshold()
{
	if (m_primary_offset_threshold->IsEmpty())
	{
		return nullptr;
	}
	return m_primary_offset_threshold.operator->();
}

void flightsimlib::io::CBglRunway::SetPrimaryOffsetThreshold(IBglRunwayEnd* value)
{
	// TODO - There has to be a better way to deal with the multiple interface issue for clients
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_primary_offset_threshold = { *static_cast<CBglRunwayEnd*>(value) };
	m_primary_offset_threshold.write().SetPosition(IBglRunwayEnd::EPosition::PrimaryOffsetThreshold);
}

const flightsimlib::io::IBglRunwayEnd* flightsimlib::io::CBglRunway::GetSecondaryOffsetThreshold()
{
	if (m_secondary_offset_threshold->IsEmpty())
	{
		return nullptr;
	}
	return m_secondary_offset_threshold.operator->();
}

void flightsimlib::io::CBglRunway::SetSecondaryOffsetThreshold(IBglRunwayEnd* value)
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_secondary_offset_threshold = { *static_cast<CBglRunwayEnd*>(value) };
	m_secondary_offset_threshold.write().SetPosition(IBglRunwayEnd::EPosition::SecondaryOffsetThreshold);
}

auto flightsimlib::io::CBglRunway::GetPrimaryBlastPad() -> const IBglRunwayEnd*
{
	if (m_primary_blast_pad->IsEmpty())
	{
		return nullptr;
	}
	return m_primary_blast_pad.operator->();
}

auto flightsimlib::io::CBglRunway::SetPrimaryBlastPad(IBglRunwayEnd* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_primary_blast_pad = { *static_cast<CBglRunwayEnd*>(value) };
	m_primary_blast_pad.write().SetPosition(IBglRunwayEnd::EPosition::PrimaryBlastPad);
}

auto flightsimlib::io::CBglRunway::GetSecondaryBlastPad() -> const IBglRunwayEnd*
{
	if (m_secondary_blast_pad->IsEmpty())
	{
		return nullptr;
	}
	return m_secondary_blast_pad.operator->();
}

auto flightsimlib::io::CBglRunway::SetSecondaryBlastPad(IBglRunwayEnd* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_secondary_blast_pad = { *static_cast<CBglRunwayEnd*>(value) };
	m_secondary_blast_pad.write().SetPosition(IBglRunwayEnd::EPosition::SecondaryBlastPad);
}

auto flightsimlib::io::CBglRunway::GetPrimaryOverrun() -> const IBglRunwayEnd*
{
	if (m_primary_overrun->IsEmpty())
	{
		return nullptr;
	}
	return m_primary_overrun.operator->();
}

auto flightsimlib::io::CBglRunway::SetPrimaryOverrun(IBglRunwayEnd* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_primary_overrun = { *static_cast<CBglRunwayEnd*>(value) };
	m_primary_overrun.write().SetPosition(IBglRunwayEnd::EPosition::PrimaryOverrun);
}

auto flightsimlib::io::CBglRunway::GetSecondaryOverrun() -> const IBglRunwayEnd*
{
	if (m_secondary_overrun->IsEmpty())
	{
		return nullptr;
	}
	return m_secondary_overrun.operator->();
}

auto flightsimlib::io::CBglRunway::SetSecondaryOverrun(IBglRunwayEnd* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_secondary_overrun = { *static_cast<CBglRunwayEnd*>(value) };
	m_secondary_overrun.write().SetPosition(IBglRunwayEnd::EPosition::SecondaryOverrun);
}

auto flightsimlib::io::CBglRunway::GetPrimaryLeftVasi() -> const IBglRunwayVasi*
{
	if (m_primary_left_vasi->IsEmpty())
	{
		return nullptr;
	}
	return m_primary_left_vasi.operator->();
}

auto flightsimlib::io::CBglRunway::SetPrimaryLeftVasi(IBglRunwayVasi* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_primary_left_vasi = { *static_cast<CBglRunwayVasi*>(value) };
	m_primary_left_vasi.write().SetPosition(IBglRunwayVasi::EPosition::PrimaryLeftVasi);
}

auto flightsimlib::io::CBglRunway::GetPrimaryRightVasi() -> const IBglRunwayVasi*
{
	if (m_primary_right_vasi->IsEmpty())
	{
		return nullptr;
	}
	return m_primary_right_vasi.operator->();
}

auto flightsimlib::io::CBglRunway::SetPrimaryRightVasi(IBglRunwayVasi* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_primary_right_vasi = { *static_cast<CBglRunwayVasi*>(value) };
	m_primary_right_vasi.write().SetPosition(IBglRunwayVasi::EPosition::PrimaryRightVasi);
}

auto flightsimlib::io::CBglRunway::GetSecondaryLeftVasi() -> const IBglRunwayVasi*
{
	if (m_secondary_left_vasi->IsEmpty())
	{
		return nullptr;
	}
	return m_secondary_left_vasi.operator->();
}

auto flightsimlib::io::CBglRunway::SetSecondaryLeftVasi(IBglRunwayVasi* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_secondary_left_vasi = { *static_cast<CBglRunwayVasi*>(value) };
	m_secondary_left_vasi.write().SetPosition(IBglRunwayVasi::EPosition::SecondaryLeftVasi);
}

auto flightsimlib::io::CBglRunway::GetSecondaryRightVasi() -> const IBglRunwayVasi*
{
	if (m_secondary_right_vasi->IsEmpty())
	{
		return nullptr;
	}
	return m_secondary_right_vasi.operator->();
}

auto flightsimlib::io::CBglRunway::SetSecondaryRightVasi(IBglRunwayVasi* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_secondary_right_vasi = { *static_cast<CBglRunwayVasi*>(value) };
	m_secondary_right_vasi.write().SetPosition(IBglRunwayVasi::EPosition::SecondaryRightVasi);
}

auto flightsimlib::io::CBglRunway::GetPrimaryApproachLights() -> const IBglRunwayApproachLights*
{
	if (m_primary_approach_lights->IsEmpty())
	{
		return nullptr;
	}
	return m_primary_approach_lights.operator->();
}

auto flightsimlib::io::CBglRunway::SetPrimaryApproachLights(IBglRunwayApproachLights* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_primary_approach_lights = { *static_cast<CBglRunwayApproachLights*>(value) };
	m_primary_approach_lights.write().SetPosition(IBglRunwayApproachLights::EPosition::PrimaryApproachLights);
}

auto flightsimlib::io::CBglRunway::GetSecondaryApproachLights() -> const IBglRunwayApproachLights*
{
	if (m_secondary_approach_lights->IsEmpty())
	{
		return nullptr;
	}
	return m_secondary_approach_lights.operator->();
}

auto flightsimlib::io::CBglRunway::SetSecondaryApproachLights(IBglRunwayApproachLights* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_secondary_approach_lights = { *static_cast<CBglRunwayApproachLights*>(value) };
	m_secondary_approach_lights.write().SetPosition(IBglRunwayApproachLights::EPosition::SecondaryApproachLights);
}


//******************************************************************************
// CBglStart
//****************************************************************************** 


auto flightsimlib::io::CBglStart::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.Number
		>> data.Designator
		>> data.Longitude
		>> data.Latitude
		>> data.Altitude
		>> data.Heading;
}

auto flightsimlib::io::CBglStart::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->Number
		<< m_data->Designator
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Altitude
		<< m_data->Heading;
}

auto flightsimlib::io::CBglStart::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglStart::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglStartData));
}

auto flightsimlib::io::CBglStart::GetRunwayNumber() const -> IBglRunway::ERunwayNumber
{
	return static_cast<IBglRunway::ERunwayNumber>(m_data->Number);
}

auto flightsimlib::io::CBglStart::SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void
{
	m_data.write().Number = to_integral(value);
}

auto flightsimlib::io::CBglStart::GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator
{
	return static_cast<IBglRunway::ERunwayDesignator>(get_packed_bits(m_data->Designator, 4, 0));
}

auto flightsimlib::io::CBglStart::SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void
{
	set_packed_bits(m_data.write().Designator, to_integral(value), 4,0);
}

auto flightsimlib::io::CBglStart::GetType() const -> EType
{
	return static_cast<EType>(get_packed_bits(m_data->Designator, 4, 4));
}

auto flightsimlib::io::CBglStart::SetType(EType value) -> void
{
	set_packed_bits(m_data.write().Designator, to_integral(value), 4, 4);
}

auto flightsimlib::io::CBglStart::GetHeading() const -> float
{
	return m_data->Heading;
}

auto flightsimlib::io::CBglStart::SetHeading(float value) -> void
{
	m_data.write().Heading = value;
}


//******************************************************************************
// CBglCom
//******************************************************************************


auto flightsimlib::io::CBglCom::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.ComType
		>> data.Frequency;

	const auto size = CalculateRemainingSize();
	if (static_cast<int>(m_data->Size) != size)
	{
		CBglString::ReadBinary(m_data, size, in);
	}
}

auto flightsimlib::io::CBglCom::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->ComType
		<< m_data->Frequency;

	const auto size = CalculateRemainingSize();
	if (static_cast<int>(m_data->Size) != size)
	{
		CBglString::WriteBinary(m_data, size, out);
	}
}

auto flightsimlib::io::CBglCom::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglCom::CalculateSize() const -> int
{
	return static_cast<int>(CalculateRemainingSize() + m_data->Name.size() +
		CBglString::CalculatePadSize(m_data, CalculateRemainingSize()));
}

auto flightsimlib::io::CBglCom::GetType() const -> EType
{
	return static_cast<EType>(m_data->Type);
}

auto flightsimlib::io::CBglCom::SetType(EType value) -> void
{
	m_data.write().ComType = to_integral(value);
}

auto flightsimlib::io::CBglCom::GetFrequency() const -> uint32_t
{
	return m_data->Frequency;
}

auto flightsimlib::io::CBglCom::SetFrequency(uint32_t value) -> void
{
	m_data.write().Frequency = value;
}

auto flightsimlib::io::CBglCom::GetName() const -> const char*
{
	return m_data->Name.c_str();
}

auto flightsimlib::io::CBglCom::SetName(const char* value) -> void
{
	m_data.write().Name = value;
}

auto flightsimlib::io::CBglCom::CalculateRemainingSize() const -> int
{
	return static_cast<int>(sizeof(m_data->Type) + sizeof(m_data->Size) + 
		sizeof(m_data->ComType) + sizeof(m_data->Frequency));
}


//******************************************************************************
// CBglHelipad
//******************************************************************************  


auto flightsimlib::io::CBglHelipad::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.SurfaceType
		>> data.HelipadType
		>> data.Color
		>> data.Longitude
		>> data.Latitude
		>> data.Altitude
		>> data.Length
		>> data.Width
		>> data.Heading;
}

auto flightsimlib::io::CBglHelipad::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->SurfaceType
		<< m_data->HelipadType
		<< m_data->Color
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Altitude
		<< m_data->Length
		<< m_data->Width
		<< m_data->Heading;
}

auto flightsimlib::io::CBglHelipad::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglHelipad::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglHelipadData));
}

auto flightsimlib::io::CBglHelipad::GetSurfaceType() const -> ESurfaceType
{
	return static_cast<ESurfaceType>(m_data->SurfaceType);
}

auto flightsimlib::io::CBglHelipad::SetSurfaceType(ESurfaceType value) -> void
{
	m_data.write().SurfaceType = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglHelipad::GetType() const -> EType
{
	return static_cast<EType>(get_packed_bits(m_data->HelipadType, 4, 0));
}

auto flightsimlib::io::CBglHelipad::SetType(EType value) -> void
{
	set_packed_bits(m_data.write().HelipadType, to_integral(value), 4, 0);
}

auto flightsimlib::io::CBglHelipad::IsTransparent() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->HelipadType, 1, 4));
}

auto flightsimlib::io::CBglHelipad::SetTransparent(bool value) -> void
{
	set_packed_bits(m_data.write().HelipadType, value, 1, 4);
}

auto flightsimlib::io::CBglHelipad::IsClosed() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->HelipadType, 1, 5));
}

auto flightsimlib::io::CBglHelipad::SetClosed(bool value) -> void
{
	set_packed_bits(m_data.write().HelipadType, value, 1, 5);
}

auto flightsimlib::io::CBglHelipad::GetColor() const -> uint32_t
{
	return m_data->Color;
}

auto flightsimlib::io::CBglHelipad::SetColor(uint32_t value) -> void
{
	m_data.write().Color = value;
}

auto flightsimlib::io::CBglHelipad::GetLength() const -> float
{
	return m_data->Length;
}

auto flightsimlib::io::CBglHelipad::SetLength(float value) -> void
{
	m_data.write().Length = value;
}

auto flightsimlib::io::CBglHelipad::GetWidth() const -> float
{
	return m_data->Width;
}

auto flightsimlib::io::CBglHelipad::SetWidth(float value) -> void
{
	m_data.write().Width = value;
}

auto flightsimlib::io::CBglHelipad::GetHeading() const -> float
{
	return m_data->Heading;
}

auto flightsimlib::io::CBglHelipad::SetHeading(float value) -> void
{
	m_data.write().Heading = value;
}


//******************************************************************************
// CBglRunwayDelete
//******************************************************************************


auto flightsimlib::io::CBglRunwayDelete::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.SurfaceType
		>> data.NumberPrimary
		>> data.NumberSecondary
		>> data.Designator;
}

auto flightsimlib::io::CBglRunwayDelete::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->SurfaceType
		<< m_data->NumberPrimary
		<< m_data->NumberSecondary
		<< m_data->Designator;
}

auto flightsimlib::io::CBglRunwayDelete::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglRunwayDelete::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglRunwayDeleteData));
}

auto flightsimlib::io::CBglRunwayDelete::GetSurfaceType() const -> ESurfaceType
{
	return static_cast<ESurfaceType>(m_data->SurfaceType);
}

auto flightsimlib::io::CBglRunwayDelete::SetSurfaceType(ESurfaceType value) -> void
{
	m_data.write().SurfaceType = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglRunwayDelete::GetPrimaryRunwayNumber() const -> IBglRunway::ERunwayNumber
{
	return static_cast<IBglRunway::ERunwayNumber>(m_data->NumberPrimary);
}

auto flightsimlib::io::CBglRunwayDelete::SetPrimaryRunwayNumber(IBglRunway::ERunwayNumber value) -> void
{
	m_data.write().NumberPrimary = to_integral(value);
}

auto flightsimlib::io::CBglRunwayDelete::GetPrimaryRunwayDesignator() const -> IBglRunway::ERunwayDesignator
{
	return static_cast<IBglRunway::ERunwayDesignator>(get_packed_bits(m_data->Designator, 4, 0));
}

auto flightsimlib::io::CBglRunwayDelete::SetPrimaryRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void
{
	set_packed_bits(m_data.write().Designator, to_integral(value), 4, 0);
}

auto flightsimlib::io::CBglRunwayDelete::GetSecondaryRunwayNumber() const -> IBglRunway::ERunwayNumber
{
	return static_cast<IBglRunway::ERunwayNumber>(m_data->NumberPrimary);
}

auto flightsimlib::io::CBglRunwayDelete::SetSecondaryRunwayNumber(IBglRunway::ERunwayNumber value) -> void
{
	m_data.write().NumberPrimary = to_integral(value);
}

auto flightsimlib::io::CBglRunwayDelete::GetSecondaryRunwayDesignator() const -> IBglRunway::ERunwayDesignator
{
	return static_cast<IBglRunway::ERunwayDesignator>(get_packed_bits(m_data->Designator, 4, 4));
}

auto flightsimlib::io::CBglRunwayDelete::SetSecondaryRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void
{
	set_packed_bits(m_data.write().Designator, to_integral(value), 4, 0);
}


//******************************************************************************
// CBglStartDelete
//******************************************************************************


auto flightsimlib::io::CBglStartDelete::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Number
		>> data.Designator
		>> data.Type
		>> data.Pad;
}

auto flightsimlib::io::CBglStartDelete::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Number
		<< m_data->Designator
		<< m_data->Type
		<< m_data->Pad;
}

auto flightsimlib::io::CBglStartDelete::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglStartDelete::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglStartDeleteData));
}

auto flightsimlib::io::CBglStartDelete::GetRunwayNumber() const -> IBglRunway::ERunwayNumber
{
	return static_cast<IBglRunway::ERunwayNumber>(m_data->Number);
}

auto flightsimlib::io::CBglStartDelete::SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void
{
	m_data.write().Number = to_integral(value);
}

auto flightsimlib::io::CBglStartDelete::GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator
{
	return static_cast<IBglRunway::ERunwayDesignator>(m_data->Designator);
}

auto flightsimlib::io::CBglStartDelete::SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void
{
	m_data.write().Designator = to_integral(value);
}

auto flightsimlib::io::CBglStartDelete::GetType() const -> IBglStart::EType
{
	return static_cast<IBglStart::EType>(m_data->Type);
}

auto flightsimlib::io::CBglStartDelete::SetType(IBglStart::EType value) -> void
{
	m_data.write().Type = to_integral(value);
}


//******************************************************************************
// CBglComDelete
//******************************************************************************


auto flightsimlib::io::CBglComDelete::ReadBinary(BinaryFileStream& in) -> void
{
	in >> m_data.write().Frequency;
}

auto flightsimlib::io::CBglComDelete::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Frequency;
}

auto flightsimlib::io::CBglComDelete::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglComDelete::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglComDeleteData));
}

auto flightsimlib::io::CBglComDelete::GetType() const -> IBglCom::EType
{
	return static_cast<IBglCom::EType>(get_packed_bits(m_data->Frequency, 4, 28));
}

auto flightsimlib::io::CBglComDelete::SetType(IBglCom::EType value) -> void
{
	set_packed_bits(m_data.write().Frequency, to_integral(value), 4, 28);
}

auto flightsimlib::io::CBglComDelete::GetFrequency() const -> uint32_t
{
	return get_packed_bits(m_data->Frequency, 28, 0);
}

auto flightsimlib::io::CBglComDelete::SetFrequency(uint32_t value) -> void
{
	set_packed_bits(m_data.write().Frequency, value, 28, 0);
}


//******************************************************************************
// CBglAirportDelete
//******************************************************************************


auto flightsimlib::io::CBglAirportDelete::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.Flags
		>> data.RunwayCount
		>> data.StartCount
		>> data.ComCount
		>> data.Pad;

	m_runway_deletes.write().reserve(data.RunwayCount);
	m_start_deletes.write().reserve(data.StartCount);
	m_com_deletes.write().reserve(data.ComCount);
	
	for (auto& runway : m_runway_deletes.write())
	{
		runway.ReadBinary(in);
	}

	for (auto& start : m_start_deletes.write())
	{
		start.ReadBinary(in);
	}

	for (auto& com : m_com_deletes.write())
	{
		com.ReadBinary(in);
	}
}

auto flightsimlib::io::CBglAirportDelete::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->Flags
		<< m_data->RunwayCount
		<< m_data->StartCount
		<< m_data->ComCount
		<< m_data->Pad;

	for (auto& runway : m_runway_deletes.write())
	{
		runway.WriteBinary(out);
	}

	for (auto& start : m_start_deletes.write())
	{
		start.WriteBinary(out);
	}

	for (auto& com : m_com_deletes.write())
	{
		com.WriteBinary(out);
	}
}

auto flightsimlib::io::CBglAirportDelete::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglAirportDelete::CalculateSize() const -> int
{
	auto count = static_cast<int>(sizeof(SBglAirportDeleteData));
	
	for (const auto& runway : m_runway_deletes.read())
	{
		count += runway.CalculateSize();
	}

	for (const auto& start : m_start_deletes.read())
	{
		count += start.CalculateSize();
	}

	for (const auto& com : m_com_deletes.read())
	{
		count += com.CalculateSize();
	}
	
	return count;
}

auto flightsimlib::io::CBglAirportDelete::IsAllApproaches() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::Approaches));
}

auto flightsimlib::io::CBglAirportDelete::SetAllApproaches(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::Approaches));
}

auto flightsimlib::io::CBglAirportDelete::IsAllApronLights() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::ApronLights));
}

auto flightsimlib::io::CBglAirportDelete::SetAllApronLights(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::ApronLights));
}

auto flightsimlib::io::CBglAirportDelete::IsAllAprons() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::Aprons));
}

auto flightsimlib::io::CBglAirportDelete::SetAllAprons(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::Aprons));
}

auto flightsimlib::io::CBglAirportDelete::IsAllFrequencies() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::Aprons));
}

auto flightsimlib::io::CBglAirportDelete::SetAllFrequencies(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::Frequencies));
}

auto flightsimlib::io::CBglAirportDelete::IsAllHelipads() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::Helipads));
}

auto flightsimlib::io::CBglAirportDelete::SetAllHelipads(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::Helipads));
}

auto flightsimlib::io::CBglAirportDelete::IsAllRunways() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::Runways));
}

auto flightsimlib::io::CBglAirportDelete::SetAllRunways(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::Runways));
}

auto flightsimlib::io::CBglAirportDelete::IsAllStarts() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::Starts));
}

auto flightsimlib::io::CBglAirportDelete::SetAllStarts(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::Starts));
}

auto flightsimlib::io::CBglAirportDelete::IsAllTaxiways() const -> bool
{
	return get_packed_bits(m_data->Flags, 1, to_integral(EFlags::Taxiways));
}

auto flightsimlib::io::CBglAirportDelete::SetAllTaxiways(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, to_integral(EFlags::Taxiways));
}

auto flightsimlib::io::CBglAirportDelete::GetRunwayDeleteCount() const -> int
{
	return static_cast<int>(m_data->RunwayCount);
}

auto flightsimlib::io::CBglAirportDelete::GetStartDeleteCount() const -> int
{
	return static_cast<int>(m_data->StartCount);
}

auto flightsimlib::io::CBglAirportDelete::GetComDeleteCount() const -> int
{
	return static_cast<int>(m_data->ComCount);
}

auto flightsimlib::io::CBglAirportDelete::GetRunwayDeleteAt(int index) -> IBglRunwayDelete*
{
	return &(m_runway_deletes.write()[index]);
}

auto flightsimlib::io::CBglAirportDelete::AddRunwayDelete(const IBglRunwayDelete* runway) -> void
{
	m_runway_deletes.write().emplace_back(*static_cast<const CBglRunwayDelete*>(runway));
}

auto flightsimlib::io::CBglAirportDelete::RemoveRunwayDelete(const IBglRunwayDelete* runway) -> void
{
	const auto iter = m_runway_deletes.read().begin() +
		std::distance(m_runway_deletes.read().data(), static_cast<const CBglRunwayDelete*>(runway));
	m_runway_deletes.write().erase(iter);
}

auto flightsimlib::io::CBglAirportDelete::GetStartDeleteAt(int index) -> IBglStartDelete*
{
	return &(m_start_deletes.write()[index]);
}

auto flightsimlib::io::CBglAirportDelete::AddStartDelete(const IBglStartDelete* start) -> void
{
	m_start_deletes.write().emplace_back(*static_cast<const CBglStartDelete*>(start));
}

auto flightsimlib::io::CBglAirportDelete::RemoveStartDelete(const IBglStartDelete* start) -> void
{
	const auto iter = m_start_deletes.read().begin() +
		std::distance(m_start_deletes.read().data(), static_cast<const CBglStartDelete*>(start));
	m_start_deletes.write().erase(iter);
}

auto flightsimlib::io::CBglAirportDelete::GetComDeleteAt(int index) -> IBglComDelete*
{
	return &(m_com_deletes.write()[index]);
}

auto flightsimlib::io::CBglAirportDelete::AddComDelete(const IBglComDelete* com) -> void
{
	m_com_deletes.write().emplace_back(*static_cast<const CBglComDelete*>(com));
}

auto flightsimlib::io::CBglAirportDelete::RemoveComDelete(const IBglComDelete* com) -> void
{
	const auto iter = m_com_deletes.read().begin() +
		std::distance(m_com_deletes.read().data(), static_cast<const CBglComDelete*>(com));
	m_com_deletes.write().erase(iter);
}

auto flightsimlib::io::CBglAirportDelete::IsEmpty() const -> bool
{
	if (m_data->Type == 0)
	{
		return true;
	}
	return false;
}


//******************************************************************************
// CBglApronEdgeLights
//******************************************************************************


auto flightsimlib::io::CBglApronEdgeLights::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.Flags
		>> data.VertexCount
		>> data.EdgeCount
		>> data.Color
		>> data.Brightness
		>> data.MaxAltitude;

	m_vertices.write().resize(m_data->VertexCount);
	m_edges.write().resize(m_data->EdgeCount);
	
	for (auto& vertex : m_vertices.write())
	{
		in >> vertex.Longitude >> vertex.Latitude;
	}

	for (auto& edge : m_edges.write())
	{
		in >> edge.Spacing >> edge.Start >> edge.End;
	}
}

auto flightsimlib::io::CBglApronEdgeLights::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->Flags
		<< m_data->VertexCount
		<< m_data->EdgeCount
		<< m_data->Color
		<< m_data->Brightness
		<< m_data->MaxAltitude;
	
	for (const auto& vertex : m_vertices.read())
	{
		out << vertex.Longitude << vertex.Latitude;
	}

	for (const auto& edge : m_edges.read())
	{
		out << edge.Spacing << edge.Start << edge.End;
	}
}

auto flightsimlib::io::CBglApronEdgeLights::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglApronEdgeLights::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglApronEdgeLightsData) + 
		m_data->VertexCount * sizeof(SBglVertexBias) +
		m_data->EdgeCount * sizeof(SBglEdge));
}

auto flightsimlib::io::CBglApronEdgeLights::GetVertexCount() const -> int
{
	return m_data->VertexCount;
}

auto flightsimlib::io::CBglApronEdgeLights::GetEdgeCount() const -> int
{
	return m_data->EdgeCount;
}

auto flightsimlib::io::CBglApronEdgeLights::GetColor() const -> uint32_t
{
	return m_data->Color;
}

auto flightsimlib::io::CBglApronEdgeLights::SetColor(uint32_t value) -> void
{
	m_data.write().Color = value;
}

auto flightsimlib::io::CBglApronEdgeLights::GetBrightness() const -> float
{
	return m_data->Brightness;
}

auto flightsimlib::io::CBglApronEdgeLights::SetBrightness(float value) -> void
{
	m_data.write().Brightness = value;
}

auto flightsimlib::io::CBglApronEdgeLights::GetMaxAltitude() const -> float
{
	return m_data->MaxAltitude;
}

auto flightsimlib::io::CBglApronEdgeLights::SetMaxAltitude(float value) -> void
{
	m_data.write().MaxAltitude = value;
}

auto flightsimlib::io::CBglApronEdgeLights::GetVertexAt(int index) -> SBglVertexLL*
{
	return &(m_vertices.write()[index]);
}

auto flightsimlib::io::CBglApronEdgeLights::AddVertex(const SBglVertexLL* vertex) -> void
{
	// TODO Need validation, self check, update count
	m_vertices.write().emplace_back(*vertex);
}

auto flightsimlib::io::CBglApronEdgeLights::RemoveVertex(const SBglVertexLL* vertex) -> void
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), vertex);
	m_vertices.write().erase(iter);
}

auto flightsimlib::io::CBglApronEdgeLights::GetEdgeAt(int index) -> SBglEdge*
{
	return &(m_edges.write()[index]);
}

auto flightsimlib::io::CBglApronEdgeLights::AddEdge(const SBglEdge* edge) -> void
{
	// TODO Need validation, self check, update count
	m_edges.write().emplace_back(*edge);
}

auto flightsimlib::io::CBglApronEdgeLights::RemoveEdge(const SBglEdge* edge) -> void
{
	const auto iter = m_edges.read().begin() +
		std::distance(m_edges.read().data(), edge);
	m_edges.write().erase(iter);
}

auto flightsimlib::io::CBglApronEdgeLights::IsEmpty() const -> bool
{
	return m_data->Type == 0;
}


//******************************************************************************
// CBglApron
//******************************************************************************


auto flightsimlib::io::CBglApron::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.Surface
		>> data.VertexCount;

	m_vertices.write().resize(m_data->VertexCount);

	for (auto& vertex : m_vertices.write())
	{
		in >> vertex.Longitude >> vertex.Latitude;
	}

	auto pad = uint8_t{ 0 };
	
	for (auto i = 0; i < s_num_pad; ++i)
	{
		in >> pad;
	}
}

auto flightsimlib::io::CBglApron::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->Surface
		<< m_data->VertexCount;

	for (const auto& vertex : m_vertices.read())
	{
		out << vertex.Longitude << vertex.Latitude;
	}

	const auto pad = uint8_t{ 0 };

	for (auto i = 0; i < s_num_pad; ++i)
	{
		out << pad;
	}
}

auto flightsimlib::io::CBglApron::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglApron::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglApronData) + s_num_pad + 
		m_data->VertexCount * sizeof(SBglVertexLL));
}

auto flightsimlib::io::CBglApron::GetSurfaceType() const -> ESurfaceType
{
	return static_cast<ESurfaceType>(m_data->Surface);
}

auto flightsimlib::io::CBglApron::SetSurfaceType(ESurfaceType value) -> void
{
	m_data.write().Surface = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglApron::GetVertexCount() const -> int
{
	return m_data->VertexCount;
}

auto flightsimlib::io::CBglApron::GetVertexAt(int index) -> SBglVertexLL*
{
	return &(m_vertices.write()[index]);
}

auto flightsimlib::io::CBglApron::AddVertex(const SBglVertexLL* vertex) -> void
{
	// TODO Need validation, self check, update count
	m_vertices.write().emplace_back(*vertex);
}

auto flightsimlib::io::CBglApron::RemoveVertex(const SBglVertexLL* vertex) -> void
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), vertex);
	m_vertices.write().erase(iter);
}


//******************************************************************************
// CBglApronPolygons
//******************************************************************************  


auto flightsimlib::io::CBglApronPolygons::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.Surface
		>> data.Flags
		>> data.VertexCount
		>> data.IndexCount;

	m_vertices.write().resize(m_data->VertexCount);
	m_indices.write().resize(m_data->IndexCount);

	for (auto& vertex : m_vertices.write())
	{
		in >> vertex.Longitude >> vertex.Latitude;
	}

	for (auto& index : m_indices.write())
	{
		in >> index.I0 >> index.I1 >> index.I2;
	}

	if (m_data->IndexCount % 2)
	{
		auto pad = uint16_t{ 0 };
		in >> pad;
	}
}

auto flightsimlib::io::CBglApronPolygons::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->Surface
		<< m_data->Flags
		<< m_data->VertexCount
		<< m_data->IndexCount;

	for (const auto& vertex : m_vertices.read())
	{
		out << vertex.Longitude << vertex.Latitude;
	}

	for (const auto& index : m_indices.read())
	{
		out << index.I0 << index.I1 << index.I2;
	}

	if (m_data->IndexCount % 2)
	{
		const auto pad = uint16_t{ 0 };
		out << pad;
	}
}

auto flightsimlib::io::CBglApronPolygons::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglApronPolygons::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglApronPolygonsData) +
		m_data->VertexCount * sizeof(SBglVertexLL) + 
		m_data->IndexCount * sizeof(SBglIndex) + 
		(m_data->IndexCount % 2 ? sizeof(uint16_t) : 0));
}

auto flightsimlib::io::CBglApronPolygons::GetSurfaceType() const -> ESurfaceType
{
	return static_cast<ESurfaceType>(m_data->Surface);
}

auto flightsimlib::io::CBglApronPolygons::SetSurfaceType(ESurfaceType value) -> void
{
	m_data.write().Surface = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglApronPolygons::IsDrawSurface() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Flags, 1, 0));
}

auto flightsimlib::io::CBglApronPolygons::SetDrawSurface(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, 0);
}

auto flightsimlib::io::CBglApronPolygons::IsDrawDetail() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Flags, 1, 1));
}

auto flightsimlib::io::CBglApronPolygons::SetDrawDetail(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, 1);
}

auto flightsimlib::io::CBglApronPolygons::GetVertexCount() const -> int
{
	return static_cast<int>(m_data->VertexCount);
}

auto flightsimlib::io::CBglApronPolygons::GetIndexCount() const -> int
{
	return static_cast<int>(m_data->IndexCount);
}

auto flightsimlib::io::CBglApronPolygons::GetVertexAt(int index) -> SBglVertexLL*
{
	return &(m_vertices.write()[index]);
}

auto flightsimlib::io::CBglApronPolygons::AddVertex(const SBglVertexLL* vertex) -> void
{
	// TODO Need validation, self check, update count
	m_vertices.write().emplace_back(*vertex);
}

auto flightsimlib::io::CBglApronPolygons::RemoveVertex(const SBglVertexLL* vertex) -> void
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), vertex);
	m_vertices.write().erase(iter);
}

auto flightsimlib::io::CBglApronPolygons::GetIndexAt(int index) -> SBglIndex*
{
	return &(m_indices.write()[index]);
}

auto flightsimlib::io::CBglApronPolygons::AddIndex(const SBglIndex* index) -> void
{
	// TODO Need validation, self check, update count
	m_indices.write().emplace_back(*index);
}

auto flightsimlib::io::CBglApronPolygons::RemoveIndex(const SBglIndex* index) -> void
{
	const auto iter = m_indices.read().begin() +
		std::distance(m_indices.read().data(), index);
	m_indices.write().erase(iter);
}


//******************************************************************************
// CBglTaxiwayPoint
//****************************************************************************** 


auto flightsimlib::io::CBglTaxiwayPoint::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Orientation
		>> data.Pad
		>> data.Vertex.Longitude
		>> data.Vertex.Latitude;
}

auto flightsimlib::io::CBglTaxiwayPoint::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Orientation
		<< m_data->Pad
		<< m_data->Vertex.Longitude
		<< m_data->Vertex.Latitude;
}

auto flightsimlib::io::CBglTaxiwayPoint::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwayPoint::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTaxiwayPointData));
}

auto flightsimlib::io::CBglTaxiwayPoint::GetType() const -> EType
{
	return static_cast<EType>(m_data->Type);
}

auto flightsimlib::io::CBglTaxiwayPoint::SetType(EType value) -> void
{
	m_data.write().Type = to_integral(value);
}

auto flightsimlib::io::CBglTaxiwayPoint::GetOrientation() const -> EOrientation
{
	return static_cast<EOrientation>(m_data->Orientation);
}

auto flightsimlib::io::CBglTaxiwayPoint::SetOrientation(EOrientation value) -> void
{
	m_data.write().Orientation = to_integral(value);
}

auto flightsimlib::io::CBglTaxiwayPoint::GetVertex() -> SBglVertexLL*
{
	return &(m_data.write().Vertex); // TODO - do we want a const form for readers?
}

auto flightsimlib::io::CBglTaxiwayPoint::SetVertex(SBglVertexLL* vertex) -> void
{
	m_data.write().Vertex = *vertex;
}


//******************************************************************************
// CBglTaxiwayPoints
//****************************************************************************** 


auto flightsimlib::io::CBglTaxiwayPoints::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.PointCount;

	m_points.write().resize(m_data->PointCount);

	for (auto& point : m_points.write())
	{
		point.ReadBinary(in);
	}
}

auto flightsimlib::io::CBglTaxiwayPoints::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->PointCount;

	for (auto& point : m_points.write())
	{
		point.WriteBinary(out);
	}
}

auto flightsimlib::io::CBglTaxiwayPoints::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwayPoints::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTaxiwayPointsData) + // could sum CalculateSize() but this
		m_data->PointCount * sizeof(SBglTaxiwayPointData)); // seems like unnecessary performance hit 
}

auto flightsimlib::io::CBglTaxiwayPoints::GetPointCount() const -> int
{
	return static_cast<int>(m_data->PointCount);
}

auto flightsimlib::io::CBglTaxiwayPoints::GetPointAt(int index) -> IBglTaxiwayPoint*
{
	return &(m_points.write()[index]);
}

auto flightsimlib::io::CBglTaxiwayPoints::AddPoint(const IBglTaxiwayPoint* point) -> void
{
	m_points.write().emplace_back(*static_cast<const CBglTaxiwayPoint*>(point));
}

auto flightsimlib::io::CBglTaxiwayPoints::RemovePoint(const IBglTaxiwayPoint* point) -> void
{
	const auto iter = m_points.read().begin() +
		std::distance(m_points.read().data(), static_cast<const CBglTaxiwayPoint*>(point));
	m_points.write().erase(iter);
}

auto flightsimlib::io::CBglTaxiwayPoints::IsEmpty() const -> bool
{
	return m_data->Type == 0;
}


//******************************************************************************
// CBglTaxiwayParking
//******************************************************************************  


auto flightsimlib::io::CBglTaxiwayParking::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Flags
		>> data.Radius
		>> data.Heading
		>> data.TeeOffset1
		>> data.TeeOffset2
		>> data.TeeOffset3
		>> data.TeeOffset4
		>> data.Vertex.Longitude
		>> data.Vertex.Latitude;

	m_codes.write().resize(GetAirlineCodeCount());

	for (auto& code : m_codes.write())
	{
		code.assign(in.ReadString(static_cast<int>(sizeof(uint32_t))));
		code.erase(std::find(
			code.begin(), code.end(), '\0'), code.end());
	}
}

auto flightsimlib::io::CBglTaxiwayParking::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Flags
		<< m_data->Radius
		<< m_data->Heading
		<< m_data->TeeOffset1
		<< m_data->TeeOffset2
		<< m_data->TeeOffset3
		<< m_data->TeeOffset4
		<< m_data->Vertex.Longitude
		<< m_data->Vertex.Latitude;

	for (const auto& code : m_codes.read())
	{
		const auto size = static_cast<int>(code.size());
		out.Write(code.c_str(), size);

		// TODO - Pad utility
		const auto pad_size = 4 - size % 4;
		if (pad_size == 4 && size != 0)
		{
			continue;
		}

		for (auto i = 0; i < pad_size; ++i)
		{
			auto pad = uint8_t{ 0 };
			out << pad;
		}
	}
}

auto flightsimlib::io::CBglTaxiwayParking::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwayParking::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTaxiwayParkingData) +
		m_codes->size() * sizeof(uint32_t));
}

auto flightsimlib::io::CBglTaxiwayParking::GetAirlineCodeCount() const -> int
{
	// TODO - these offsets should be flags
	return static_cast<bool>(get_packed_bits(m_data->Flags, 12, 24));
}

auto flightsimlib::io::CBglTaxiwayParking::GetNumber() const -> uint16_t
{
	return static_cast<bool>(get_packed_bits(m_data->Flags, 12, 12));
}

auto flightsimlib::io::CBglTaxiwayParking::SetNumber(uint16_t value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 12, 12);
}

auto flightsimlib::io::CBglTaxiwayParking::GetType() const -> EType
{
	return static_cast<EType>(get_packed_bits(m_data->Flags, 4, 8));
}

auto flightsimlib::io::CBglTaxiwayParking::SetType(EType value) -> void
{
	set_packed_bits(m_data.write().Flags, to_integral(value), 4, 8);
}

auto flightsimlib::io::CBglTaxiwayParking::GetPushback() const -> EPushback
{
	return static_cast<EPushback>(get_packed_bits(m_data->Flags, 2, 6));
}

auto flightsimlib::io::CBglTaxiwayParking::SetPushback(EPushback value) -> void
{
	set_packed_bits(m_data.write().Flags, to_integral(value), 2, 6);
}

auto flightsimlib::io::CBglTaxiwayParking::GetName() const -> EName
{
	return static_cast<EName>(get_packed_bits(m_data->Flags, 6, 0));
}

auto flightsimlib::io::CBglTaxiwayParking::SetName(EName value) -> void
{
	set_packed_bits(m_data.write().Flags, to_integral(value), 6, 0);
}

auto flightsimlib::io::CBglTaxiwayParking::GetRadius() const -> float
{
	return m_data->Radius;
}

auto flightsimlib::io::CBglTaxiwayParking::SetRadius(float value) -> void
{
	m_data.write().Radius = value;
}

auto flightsimlib::io::CBglTaxiwayParking::GetHeading() const -> float
{
	return m_data->Heading;
}

auto flightsimlib::io::CBglTaxiwayParking::SetHeading(float value) -> void
{
	m_data.write().Heading = value;
}

auto flightsimlib::io::CBglTaxiwayParking::GetTeeOffset1() const -> float
{
	return m_data->TeeOffset1;
}

auto flightsimlib::io::CBglTaxiwayParking::SetTeeOffset1(float value) -> void
{
	m_data.write().TeeOffset1 = value;
}

auto flightsimlib::io::CBglTaxiwayParking::GetTeeOffset2() const -> float
{
	return m_data->TeeOffset2;
}

auto flightsimlib::io::CBglTaxiwayParking::SetTeeOffset2(float value) -> void
{
	m_data.write().TeeOffset2 = value;
}

auto flightsimlib::io::CBglTaxiwayParking::GetTeeOffset3() const -> float
{
	return m_data->TeeOffset3;
}

auto flightsimlib::io::CBglTaxiwayParking::SetTeeOffset3(float value) -> void
{
	m_data.write().TeeOffset3 = value;
}

auto flightsimlib::io::CBglTaxiwayParking::GetTeeOffset4() const -> float
{
	return m_data->TeeOffset4;
}

auto flightsimlib::io::CBglTaxiwayParking::SetTeeOffset4(float value) -> void
{
	m_data.write().TeeOffset4 = value;
}

auto flightsimlib::io::CBglTaxiwayParking::GetAirlineCodeAt(int index) const -> const char*
{
	return m_codes.read()[index].c_str();
}

auto flightsimlib::io::CBglTaxiwayParking::AddAirlineCode(const char* code) -> void
{
	m_codes.write().emplace_back(code);
	SetAirlineCodeCount(GetAirlineCodeCount() + 1);
}

auto flightsimlib::io::CBglTaxiwayParking::RemoveAirlineCode(const char* code) -> void
{
	const auto str_code = std::string{ code };
	const auto iter = std::find(m_codes->begin(), m_codes->end(), str_code);
	if (iter != m_codes->end())
	{
		m_codes.write().erase(iter);
		SetAirlineCodeCount(GetAirlineCodeCount() - 1);
	}
}

auto flightsimlib::io::CBglTaxiwayParking::SetAirlineCodeCount(int value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 12, 24);
}


//******************************************************************************
// CBglTaxiwayParkings
//****************************************************************************** 


auto flightsimlib::io::CBglTaxiwayParkings::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.ParkingCount;

	m_parkings.write().resize(m_data->ParkingCount);

	for (auto& parking : m_parkings.write())
	{
		parking.ReadBinary(in);
	}
}

auto flightsimlib::io::CBglTaxiwayParkings::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->ParkingCount;

	for (auto& parking : m_parkings.write())
	{
		parking.WriteBinary(out);
	}
}

auto flightsimlib::io::CBglTaxiwayParkings::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwayParkings::CalculateSize() const -> int
{
	auto count = static_cast<int>(sizeof(SBglTaxiwayParkingsData));

	for (const auto& parking : m_parkings.read())
	{
		count += parking.CalculateSize();
	}
	
	return count;
}

auto flightsimlib::io::CBglTaxiwayParkings::GetParkingCount() const -> int
{
	return static_cast<int>(m_data->ParkingCount);
}

auto flightsimlib::io::CBglTaxiwayParkings::GetParkingAt(int index) -> IBglTaxiwayParking*
{
	return &(m_parkings.write()[index]);
}

auto flightsimlib::io::CBglTaxiwayParkings::AddParking(const IBglTaxiwayParking* parking) -> void
{
	m_parkings.write().emplace_back(*static_cast<const CBglTaxiwayParking*>(parking));
}

auto flightsimlib::io::CBglTaxiwayParkings::RemoveParking(const IBglTaxiwayParking* parking) -> void
{
	const auto iter = m_parkings.read().begin() +
		std::distance(m_parkings.read().data(), static_cast<const CBglTaxiwayParking*>(parking));
	m_parkings.write().erase(iter);
}

auto flightsimlib::io::CBglTaxiwayParkings::IsEmpty() const -> bool
{
	return m_data->Type == 0;
}


//******************************************************************************
// CBglTaxiwayPath
//****************************************************************************** 


auto flightsimlib::io::CBglTaxiwayPath::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	
	in >> data.StartIndex
		>> data.EndIndex
		>> data.TypeFlags
		>> data.NameIndex
		>> data.MarkingFlags
		>> data.Surface
		>> data.Width
		>> data.WeightLimit
		>> data.Unknown;
}

auto flightsimlib::io::CBglTaxiwayPath::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->StartIndex
		<< m_data->EndIndex
		<< m_data->TypeFlags
		<< m_data->NameIndex
		<< m_data->MarkingFlags
		<< m_data->Surface
		<< m_data->Width
		<< m_data->WeightLimit
		<< m_data->Unknown;
}

auto flightsimlib::io::CBglTaxiwayPath::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwayPath::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTaxiwayPathData));
}

auto flightsimlib::io::CBglTaxiwayPath::GetStartIndex() const -> int
{
	return static_cast<int>(m_data->StartIndex);
}

auto flightsimlib::io::CBglTaxiwayPath::SetStartIndex(int value) -> void
{
	m_data.write().StartIndex = static_cast<uint16_t>(value);
}

auto flightsimlib::io::CBglTaxiwayPath::GetEndIndex() const -> int
{
	return static_cast<int>(get_packed_bits(m_data->EndIndex, 12, 0));
}

auto flightsimlib::io::CBglTaxiwayPath::SetEndIndex(int value) -> void
{
	set_packed_bits(m_data.write().EndIndex, value, 12, 0);
}

auto flightsimlib::io::CBglTaxiwayPath::GetType() const -> EType
{
	return static_cast<EType>(get_packed_bits(m_data->TypeFlags, 5, 0));
}

auto flightsimlib::io::CBglTaxiwayPath::SetType(EType value) -> void
{
	set_packed_bits(m_data.write().TypeFlags, to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglTaxiwayPath::GetRunwayNumber() const -> IBglRunway::ERunwayNumber
{
	return static_cast<IBglRunway::ERunwayNumber>(m_data->NameIndex);
}

auto flightsimlib::io::CBglTaxiwayPath::SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void
{
	m_data.write().NameIndex = to_integral(value);
}

auto flightsimlib::io::CBglTaxiwayPath::GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator
{
	return static_cast<IBglRunway::ERunwayDesignator>(get_packed_bits(m_data->EndIndex, 4, 12));
}

auto flightsimlib::io::CBglTaxiwayPath::SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void
{
	set_packed_bits(m_data.write().EndIndex, to_integral(value), 4, 12);
}

auto flightsimlib::io::CBglTaxiwayPath::GetNameIndex() const -> int
{
	return static_cast<int>(m_data->NameIndex);
}

auto flightsimlib::io::CBglTaxiwayPath::SetNameIndex(int value) -> void
{
	m_data.write().NameIndex = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglTaxiwayPath::IsDrawSurface() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->TypeFlags, 1, 5));
}

auto flightsimlib::io::CBglTaxiwayPath::SetDrawSurface(bool value) -> void
{
	set_packed_bits(m_data.write().TypeFlags, value, 1, 5);
}

auto flightsimlib::io::CBglTaxiwayPath::IsDrawDetail() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->TypeFlags, 1, 6));
}

auto flightsimlib::io::CBglTaxiwayPath::SetDrawDetail(bool value) -> void
{
	set_packed_bits(m_data.write().TypeFlags, value, 1, 6);
}

auto flightsimlib::io::CBglTaxiwayPath::HasCenterLine() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1, 0));
}

auto flightsimlib::io::CBglTaxiwayPath::SetCenterLine(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1, 0);
}

auto flightsimlib::io::CBglTaxiwayPath::IsCenterLineLighted() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1, 1));
}

auto flightsimlib::io::CBglTaxiwayPath::SetCenterLineLighted(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1, 1);
}

auto flightsimlib::io::CBglTaxiwayPath::IsLeftEdgeLighted() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1, 4));
}

auto flightsimlib::io::CBglTaxiwayPath::SetsLeftEdgeLighted(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1, 4);
}

auto flightsimlib::io::CBglTaxiwayPath::IsRightEdgeLighted() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->MarkingFlags, 1, 7));
}

auto flightsimlib::io::CBglTaxiwayPath::SetsRightEdgeLighted(bool value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, value, 1, 7);
}

auto flightsimlib::io::CBglTaxiwayPath::GetLeftEdge() const -> ELightType
{
	return static_cast<ELightType>(get_packed_bits(m_data->MarkingFlags, 2, 2));
}

auto flightsimlib::io::CBglTaxiwayPath::SetLeftEdge(ELightType value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, to_integral(value), 2, 2);
}

auto flightsimlib::io::CBglTaxiwayPath::GetRightEdge() const -> ELightType
{
	return static_cast<ELightType>(get_packed_bits(m_data->MarkingFlags, 2, 5));
}

auto flightsimlib::io::CBglTaxiwayPath::SetRightEdge(ELightType value) -> void
{
	set_packed_bits(m_data.write().MarkingFlags, to_integral(value), 2, 5);
}

auto flightsimlib::io::CBglTaxiwayPath::GetSurfaceType() const -> ESurfaceType
{
	return static_cast<ESurfaceType>(m_data->Surface);
}

auto flightsimlib::io::CBglTaxiwayPath::SetSurfaceType(ESurfaceType value) -> void
{
	m_data.write().Surface = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglTaxiwayPath::GetWidth() const -> float
{
	return m_data->Width;
}

auto flightsimlib::io::CBglTaxiwayPath::SetWidth(float value) -> void
{
	m_data.write().Width = value;
}

auto flightsimlib::io::CBglTaxiwayPath::GetWeightLimit() const -> float
{
	return m_data->WeightLimit;
}

auto flightsimlib::io::CBglTaxiwayPath::SetWeightLimit(float value) -> void
{
	m_data.write().WeightLimit = value;
}


//******************************************************************************
// CBglTaxiwayPaths
//******************************************************************************  


auto flightsimlib::io::CBglTaxiwayPaths::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.PathCount;

	m_paths.write().resize(m_data->PathCount);

	for (auto& path : m_paths.write())
	{
		path.ReadBinary(in);
	}
}

auto flightsimlib::io::CBglTaxiwayPaths::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->PathCount;

	for (auto& path : m_paths.write())
	{
		path.WriteBinary(out);
	}
}

auto flightsimlib::io::CBglTaxiwayPaths::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwayPaths::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTaxiwayPathsData) + // could sum CalculateSize() but this
		m_data->PathCount * sizeof(SBglTaxiwayPathData)); // seems like unnecessary performance hit 
}

auto flightsimlib::io::CBglTaxiwayPaths::GetPathCount() const -> int
{
	return static_cast<int>(m_data->PathCount);
}

auto flightsimlib::io::CBglTaxiwayPaths::GetPathAt(int index) -> IBglTaxiwayPath*
{
	return &(m_paths.write()[index]);
}

auto flightsimlib::io::CBglTaxiwayPaths::AddPath(const IBglTaxiwayPath* path) -> void
{
	m_paths.write().emplace_back(*static_cast<const CBglTaxiwayPath*>(path));
}

auto flightsimlib::io::CBglTaxiwayPaths::RemovePath(const IBglTaxiwayPath* path) -> void
{
	const auto iter = m_paths.read().begin() +
		std::distance(m_paths.read().data(), static_cast<const CBglTaxiwayPath*>(path));
	m_paths.write().erase(iter);
}

auto flightsimlib::io::CBglTaxiwayPaths::IsEmpty() const -> bool
{
	return m_data->Type == 0;
}


//******************************************************************************
// CBglTaxiwayNames
//****************************************************************************** 


auto flightsimlib::io::CBglTaxiwayNames::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.NameCount;

	m_names.write().resize(m_data->NameCount);

	for (auto& name : m_names.write())
	{
		name.assign(in.ReadString(static_cast<int>(sizeof(uint64_t))));
		name.erase(std::find(
			name.begin(), name.end(), '\0'), name.end());
	}
}

auto flightsimlib::io::CBglTaxiwayNames::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->NameCount;

	for (const auto& name : m_names.read())
	{
		//TODO Pad Util
		const auto size = static_cast<int>(name.size());
		out.Write(name.c_str(), size);

		const auto pad_size = 8 - size % 8;
		if (pad_size == 8 && size != 0)
		{
			continue;
		}

		for (auto i = 0; i < pad_size; ++i)
		{
			auto pad = uint8_t{ 0 };
			out << pad;
		}
	}
}

auto flightsimlib::io::CBglTaxiwayNames::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTaxiwayNames::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTaxiwayNamesData) +
		m_names->size() * sizeof(uint64_t));
}

auto flightsimlib::io::CBglTaxiwayNames::GetNameCount() const -> int
{
	return static_cast<int>(m_data->NameCount);
}

auto flightsimlib::io::CBglTaxiwayNames::GetNameAt(int index) const -> const char*
{
	return m_names.read()[index].c_str();
}

auto flightsimlib::io::CBglTaxiwayNames::AddName(const char* name) -> void
{
	m_names.write().emplace_back(name);
	++(m_data.write().NameCount);
}

auto flightsimlib::io::CBglTaxiwayNames::RemoveName(const char* name) -> void
{
	const auto name_str = std::string{ name };
	const auto iter = std::find(m_names->begin(), m_names->end(), name_str);
	if (iter != m_names->end())
	{
		m_names.write().erase(iter);
		--(m_data.write().NameCount);
	}
}

auto flightsimlib::io::CBglTaxiwayNames::IsEmpty() const -> bool
{
	return m_data->Type == 0;
}


//******************************************************************************
// CBglJetway
//****************************************************************************** 


flightsimlib::io::CBglJetway::CBglJetway(CBglJetway const& other)
{
	m_scenery_object = m_scenery_object->Clone();
}

flightsimlib::io::CBglJetway& flightsimlib::io::CBglJetway::operator=(CBglJetway const& other)
{
	m_scenery_object = other.m_scenery_object->Clone();
	return *this;
}

auto flightsimlib::io::CBglJetway::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();

	in >> data.Type
		>> data.Size
		>> data.Number
		>> data.Name
		>> data.SceneryObjectSize;
	
	const auto pos = in.GetPosition();
	auto child_type = uint16_t{};
	auto child_size = uint16_t{};
	in >> child_type;
	in >> child_size;
	in.SetPosition(pos);

	switch (static_cast<IBglSceneryObject::ESceneryObjectType>(child_type))
	{
	case IBglSceneryObject::ESceneryObjectType::GenericBuilding:
		m_scenery_object = std::make_unique<CBglGenericBuilding>();
		break;
	case IBglSceneryObject::ESceneryObjectType::LibraryObject:
		m_scenery_object = std::make_unique<CBglLibraryObject>();
		break;
	case IBglSceneryObject::ESceneryObjectType::Windsock:
		m_scenery_object = std::make_unique<CBglWindsock>();
		break;
	case IBglSceneryObject::ESceneryObjectType::Effect:
		m_scenery_object = std::make_unique<CBglEffect>();
		break;
	case IBglSceneryObject::ESceneryObjectType::TaxiwaySigns:
		m_scenery_object = std::make_unique<CBglTaxiwaySigns>();
		break;
	case IBglSceneryObject::ESceneryObjectType::Trigger:
		m_scenery_object = std::make_unique<CBglTrigger>();
		break;
	case IBglSceneryObject::ESceneryObjectType::Beacon:
		m_scenery_object = std::make_unique<CBglBeacon>();
		break;
	case IBglSceneryObject::ESceneryObjectType::ExtrusionBridge:
		m_scenery_object = std::make_unique<CBglExtrusionBridge>();
		break;
	default:
		in.SetPosition(pos + static_cast<int>(child_size));
		return;
	}

	m_scenery_object->ReadBinary(in);
	
	const auto pad_size = CalculatePadSize();
	auto pad = uint8_t{ 0 };
	
	for (auto i = 0; i < pad_size; ++i)
	{	
		in >> pad;
	}
}

auto flightsimlib::io::CBglJetway::WriteBinary(BinaryFileStream& out) -> void
{
	assert(m_scenery_object != nullptr);
	
	out << m_data->Type
		<< m_data->Size
		<< m_data->Number
		<< m_data->Name
		<< m_data->SceneryObjectSize;
	
	m_scenery_object->WriteBinary(out);
	
	const auto pad_size = CalculatePadSize();
	for (auto i = 0; i < pad_size; ++i)
	{
		auto pad = uint8_t{ 0 };
		out << pad;
	}
}

auto flightsimlib::io::CBglJetway::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglJetway::CalculateSize() const -> int
{
	assert(m_scenery_object != nullptr);
	return static_cast<int>(sizeof(SBglJetwayData) + 
		m_scenery_object->CalculateSize() + CalculatePadSize());
}

auto flightsimlib::io::CBglJetway::GetNumber() const -> uint16_t
{
	return m_data->Number;
}

auto flightsimlib::io::CBglJetway::SetNumber(uint16_t value) -> void
{
	m_data.write().Number = value;
}

auto flightsimlib::io::CBglJetway::GetName() const -> IBglTaxiwayParking::EName
{
	return static_cast<IBglTaxiwayParking::EName>(m_data->Name);
}

auto flightsimlib::io::CBglJetway::SetName(IBglTaxiwayParking::EName value) -> void
{
	m_data.write().Name = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglJetway::GetSceneryObject() -> IBglSceneryObject*
{
	return m_scenery_object.get();
}

auto flightsimlib::io::CBglJetway::GetSceneryObject() const -> const IBglSceneryObject*
{
	return m_scenery_object.get();
}

auto flightsimlib::io::CBglJetway::SetSceneryObject(IBglSceneryObject* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_scenery_object = dynamic_cast<CBglSceneryObject*>(value)->Clone();
}

auto flightsimlib::io::CBglJetway::CalculatePadSize() const -> int
{
	// TODO - Pad utility
	assert(m_scenery_object != nullptr);
	const auto size = static_cast<int>(sizeof(SBglJetwayData) + m_scenery_object->CalculateSize());
	const auto pad_size = 4 - size % 4;
	if (pad_size == 4)
	{
		return 0;
	}
	return pad_size;
}


//******************************************************************************
// CBglLeg
//****************************************************************************** 


auto flightsimlib::io::CBglLeg::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();

	in >> data.LegType
		>> data.AltitudeDescriptor
		>> data.TurnDirection
		>> data.Flags
		>> data.IcaoIdent
		>> data.RegionIdent
		>> data.RecommendedIcaoIdent
		>> data.RecommendedRegionIdent
		>> data.Theta
		>> data.Rho
		>> data.Course
		>> data.DistanceTime
		>> data.Altitude1
		>> data.Altitude2;
}

auto flightsimlib::io::CBglLeg::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->LegType
		<< m_data->AltitudeDescriptor
		<< m_data->TurnDirection
		<< m_data->Flags
		<< m_data->IcaoIdent
		<< m_data->RegionIdent
		<< m_data->RecommendedIcaoIdent
		<< m_data->RecommendedRegionIdent
		<< m_data->Theta
		<< m_data->Rho
		<< m_data->Course
		<< m_data->DistanceTime
		<< m_data->Altitude1
		<< m_data->Altitude2;
}

auto flightsimlib::io::CBglLeg::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglLeg::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglLegData));
}

auto flightsimlib::io::CBglLeg::GetType() const -> EType
{
	return static_cast<EType>(m_data->LegType);
}

auto flightsimlib::io::CBglLeg::SetType(EType value) -> void
{
	m_data.write().LegType = to_integral(value);
}

auto flightsimlib::io::CBglLeg::GetAltitudeDescriptor() const -> EAltitudeDescriptor
{
	return static_cast<EAltitudeDescriptor>(m_data->AltitudeDescriptor);
}

auto flightsimlib::io::CBglLeg::SetAltitudeDescriptor(EAltitudeDescriptor value) -> void
{
	m_data.write().AltitudeDescriptor = to_integral(value);
}

auto flightsimlib::io::CBglLeg::GetTurnDirection() const -> ETurnDirection
{
	return static_cast<ETurnDirection>(m_data->TurnDirection);
}

auto flightsimlib::io::CBglLeg::SetTurnDirection(ETurnDirection value) -> void
{
	m_data.write().TurnDirection = to_integral(value);
}

auto flightsimlib::io::CBglLeg::IsTrueCourse() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Flags, 1, 0));
}

auto flightsimlib::io::CBglLeg::SetTrueCourse(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, 0);
}

auto flightsimlib::io::CBglLeg::IsTime() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Flags, 1, 1));
}

auto flightsimlib::io::CBglLeg::SetIsTime(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, 1);
}

auto flightsimlib::io::CBglLeg::IsFlyover() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->Flags, 1, 2));
}

auto flightsimlib::io::CBglLeg::SetFlyover(bool value) -> void
{
	set_packed_bits(m_data.write().Flags, value, 1, 2);
}

auto flightsimlib::io::CBglLeg::GetFixType() const -> EFixType
{
	return static_cast<EFixType>(get_packed_bits(m_data->IcaoIdent, 5, 0));
}

auto flightsimlib::io::CBglLeg::SetFixType(EFixType value) -> void
{
	set_packed_bits(m_data.write().IcaoIdent, to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglLeg::GetIcaoIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->IcaoIdent, 27, 5));
}

auto flightsimlib::io::CBglLeg::SetIcaoIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().IcaoIdent, value, 27, 5);
}

auto flightsimlib::io::CBglLeg::GetRegionIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 11, 0));
}

auto flightsimlib::io::CBglLeg::SetRegionIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 11, 0);
}

auto flightsimlib::io::CBglLeg::GetIcaoAirport() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 21, 11));
}

auto flightsimlib::io::CBglLeg::SetIcaoAirport(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 21, 11);
}

auto flightsimlib::io::CBglLeg::GetRecommendedFixType() const -> EFixType
{
	return static_cast<EFixType>(get_packed_bits(
		m_data->RecommendedIcaoIdent, 5, 0));
}

auto flightsimlib::io::CBglLeg::SetRecommendedFixType(EFixType value) -> void
{
	set_packed_bits(m_data.write().RecommendedIcaoIdent, 
		to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglLeg::GetRecommendedIcaoIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(
		m_data->RecommendedIcaoIdent, 27, 5));
}

auto flightsimlib::io::CBglLeg::SetRecommendedIcaoIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RecommendedIcaoIdent,
		value, 27, 5);
}

auto flightsimlib::io::CBglLeg::GetRecommendedRegionIdent() const -> uint32_t
{
	return m_data->RecommendedRegionIdent;
}

auto flightsimlib::io::CBglLeg::SetRecommendedRegionIdent(uint32_t value) -> void
{
	m_data.write().RecommendedRegionIdent = value;
}

auto flightsimlib::io::CBglLeg::GetTheta() const -> float
{
	return m_data->Theta;
}

auto flightsimlib::io::CBglLeg::SetTheta(float value) -> void
{
	m_data.write().Theta = value;
}

auto flightsimlib::io::CBglLeg::GetRho() const -> float
{
	return m_data->Rho;
}

auto flightsimlib::io::CBglLeg::SetRho(float value) -> void
{
	m_data.write().Rho = value;
}

auto flightsimlib::io::CBglLeg::GetTrueCourse() const -> float
{
	// TODO - Validation
	if (!IsTrueCourse())
	{
		return 0.0f;
	}
	return m_data->Course;
}

auto flightsimlib::io::CBglLeg::SetTrueCourse(float value) -> void
{
	if (IsTrueCourse())
	{
		m_data.write().Course = value;
	}
}

auto flightsimlib::io::CBglLeg::GetMagneticCourse() const -> float
{
	if (IsTrueCourse())
	{
		return 0.0f;
	}
	return m_data->Course;
}

auto flightsimlib::io::CBglLeg::SetMagneticCourse(float value) -> void
{
	if (!IsTrueCourse())
	{
		m_data.write().Course = value;
	}
}

auto flightsimlib::io::CBglLeg::GetDistance() const -> float
{
	if (IsTime())
	{
		return 0.0f;
	}
	return m_data->DistanceTime;
}

auto flightsimlib::io::CBglLeg::SetDistance(float value) -> void
{
	if (!IsTime())
	{
		m_data.write().DistanceTime = value;
	}
}

auto flightsimlib::io::CBglLeg::GetTime() const -> float
{
	if (!IsTime())
	{
		return 0.0f;
	}
	return m_data->DistanceTime;
}

auto flightsimlib::io::CBglLeg::SetTime(float value) -> void
{
	if (IsTime())
	{
		m_data.write().DistanceTime = value;
	}
}

auto flightsimlib::io::CBglLeg::GetAltitude1() const -> float
{
	return m_data->Altitude1;
}

auto flightsimlib::io::CBglLeg::SetAltitude1(float value) -> void
{
	m_data.write().Altitude1 = value;
}

auto flightsimlib::io::CBglLeg::GetAltitude2() const -> float
{
	return m_data->Altitude2;
}

auto flightsimlib::io::CBglLeg::SetAltitude2(float value) -> void
{
	m_data.write().Altitude2 = value;
}


//******************************************************************************
// CBglLegs
//******************************************************************************  


auto flightsimlib::io::CBglLegs::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.LegCount;

	m_legs.write().resize(m_data->LegCount);

	for (auto& leg : m_legs.write())
	{
		leg.ReadBinary(in);
	}
}

auto flightsimlib::io::CBglLegs::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->LegCount;
	
	for (auto& leg : m_legs.write())
	{
		leg.WriteBinary(out);
	}
}

auto flightsimlib::io::CBglLegs::Validate() -> bool
{
	m_data.write().Size = static_cast<int>(CalculateSize());
	return true;
}

auto flightsimlib::io::CBglLegs::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglLegsData) +
		m_data->LegCount * sizeof(SBglLegData));
}

auto flightsimlib::io::CBglLegs::GetLegCount() const -> int
{
	return static_cast<int>(m_data->LegCount);
}

auto flightsimlib::io::CBglLegs::GetLegAt(int index) -> IBglLeg*
{
	return &(m_legs.write()[index]);
}

auto flightsimlib::io::CBglLegs::AddLeg(const IBglLeg* leg) -> void
{
	m_legs.write().emplace_back(*static_cast<const CBglLeg*>(leg));
}

auto flightsimlib::io::CBglLegs::RemoveLeg(const IBglLeg* leg) -> void
{
	const auto iter = m_legs.read().begin() +
		std::distance(m_legs.read().data(), static_cast<const CBglLeg*>(leg));
	m_legs.write().erase(iter);
}

auto flightsimlib::io::CBglLegs::IsEmpty() const -> bool
{
	if (m_data->Type == 0)
	{
		return true;
	}
	return false;
}


//******************************************************************************
// CBglDmeArc
//****************************************************************************** 


auto flightsimlib::io::CBglDmeArc::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.IcaoIdent
		>> data.RegionIdent
		>> data.Radial
		>> data.Distance;
}

auto flightsimlib::io::CBglDmeArc::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->IcaoIdent
		<< m_data->RegionIdent
		<< m_data->Radial
		<< m_data->Radial;
}

auto flightsimlib::io::CBglDmeArc::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglDmeArc::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglDmeArcData));
}

auto flightsimlib::io::CBglDmeArc::GetIcaoIdent() const -> uint32_t
{
	return m_data->IcaoIdent;
}

auto flightsimlib::io::CBglDmeArc::SetIcaoIdent(uint32_t value) -> void
{
	m_data.write().IcaoIdent = value;
}

auto flightsimlib::io::CBglDmeArc::GetRegionIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 11, 0));
}

auto flightsimlib::io::CBglDmeArc::SetRegionIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 11, 0);
}

auto flightsimlib::io::CBglDmeArc::GetIcaoAirport() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 21, 11));
}

auto flightsimlib::io::CBglDmeArc::SetIcaoAirport(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 21, 11);
}

auto flightsimlib::io::CBglDmeArc::GetRadial() const -> int
{
	// Validate - 1-360
	return static_cast<int>(m_data->Radial);
}

auto flightsimlib::io::CBglDmeArc::SetRadial(int value) -> void
{
	m_data.write().Radial = static_cast<uint32_t>(value);
}

auto flightsimlib::io::CBglDmeArc::GetDistance() const -> float
{
	return m_data->Distance;
}

auto flightsimlib::io::CBglDmeArc::SetDistance(float value) -> void
{
	m_data.write().Distance = value;
}

auto flightsimlib::io::CBglDmeArc::IsEmpty() const -> bool
{
	if (m_data->IcaoIdent == 0 && m_data->RegionIdent == 0 && 
		m_data->Radial == 0 && m_data->Distance == 0.0f)
	{
		return true;
	}
	return false;
}


//******************************************************************************
// CBglTransition
//****************************************************************************** 


auto flightsimlib::io::CBglTransition::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	
	in >> data.Type
		>> data.Size
		>> data.TransitionType
		>> data.LegCount
		>> data.IcaoIdent
		>> data.RegionIdent
		>> data.Altitude;

	if (GetType() == EType::Dme)
	{
		m_dme_arc.write().ReadBinary(in);
	}
	
	if (m_data->LegCount > 0u)
	{
		const auto child_pos = in.GetPosition();
		uint16_t type = 0;
		in >> type;
		in.SetPosition(child_pos);

		if (type != s_transition_legs_type)
		{
			return;
		}
		
		m_legs.write().ReadBinary(in);
		// TODO - error handling
		if (!m_legs.write().Validate())
		{
			return;
		}
	}
}

auto flightsimlib::io::CBglTransition::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->TransitionType
		<< m_data->LegCount
		<< m_data->IcaoIdent
		<< m_data->RegionIdent
		<< m_data->Altitude;

	if (!m_dme_arc->IsEmpty())
	{
		m_dme_arc.write().WriteBinary(out);
	}

	if (!m_legs->IsEmpty())
	{
		m_legs.write().WriteBinary(out);
	}
}

auto flightsimlib::io::CBglTransition::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTransition::CalculateSize() const -> int
{
	auto count = static_cast<int>(sizeof(SBglTransitionData));
	if (!m_legs->IsEmpty())
	{
		count += m_legs->CalculateSize();
	}
	if (!m_dme_arc->IsEmpty())
	{
		count += m_dme_arc->CalculateSize();
	}
	return count;
}

auto flightsimlib::io::CBglTransition::GetType() const -> EType
{
	return static_cast<EType>(m_data->TransitionType);
}

auto flightsimlib::io::CBglTransition::SetType(EType value) -> void
{
	m_data.write().TransitionType = to_integral(value);
}

auto flightsimlib::io::CBglTransition::GetLegCount() const -> int
{
	return static_cast<int>(m_data->LegCount);
}

auto flightsimlib::io::CBglTransition::GetFixType() const -> IBglLeg::EFixType
{
	return static_cast<IBglLeg::EFixType>(get_packed_bits(m_data->IcaoIdent, 5, 0));
}

auto flightsimlib::io::CBglTransition::SetFixType(IBglLeg::EFixType value) -> void
{
	set_packed_bits(m_data.write().IcaoIdent, to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglTransition::GetIcaoIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->IcaoIdent, 27, 5));
}

auto flightsimlib::io::CBglTransition::SetIcaoIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().IcaoIdent, value, 27, 5);
}

auto flightsimlib::io::CBglTransition::GetRegionIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 11, 0));
}

auto flightsimlib::io::CBglTransition::SetRegionIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 11, 0);
}

auto flightsimlib::io::CBglTransition::GetIcaoAirport() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 21, 11));
}

auto flightsimlib::io::CBglTransition::SetIcaoAirport(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 21, 11);
}

auto flightsimlib::io::CBglTransition::GetAltitude() const -> float
{
	return m_data->Altitude;
}

auto flightsimlib::io::CBglTransition::SetAltitude(float value) -> void
{
	m_data.write().Altitude = value;
}

auto flightsimlib::io::CBglTransition::GetDmeArc() -> IBglDmeArc*
{
	if (m_dme_arc->IsEmpty())
	{
		return nullptr;
	}
	return &m_dme_arc.write();
}

auto flightsimlib::io::CBglTransition::SetDmeArc(IBglDmeArc* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_dme_arc = { *static_cast<CBglDmeArc*>(value) };
}

auto flightsimlib::io::CBglTransition::GetTransitionLegs() -> IBglLegs*
{
	if (m_legs->IsEmpty())
	{
		return nullptr;
	}
	return &m_legs.write();
}

auto flightsimlib::io::CBglTransition::SetTransitionLegs(IBglLegs* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_legs = { *static_cast<CBglLegs*>(value) };
}


//******************************************************************************
// CBglApproach
//****************************************************************************** 


auto flightsimlib::io::CBglApproach::ReadBinary(BinaryFileStream& in) -> void
{
	const auto initial_pos = in.GetPosition();
	
	auto& data = m_data.write();

	in >> data.Type
		>> data.Size
		>> data.Suffix
		>> data.Number
		>> data.TypeDesignator
		>> data.TransitionCount
		>> data.ApproachLegCount
		>> data.MissedApproachLegCount
		>> data.IcaoIdent
		>> data.RegionIdent
		>> data.Altitude
		>> data.Heading
		>> data.MissedAltitude;

	m_transitions.write().resize(m_data->TransitionCount);
	auto transition_index = 0;
	
	const auto final_position = initial_pos + static_cast<int>(m_data->Size);
	while (in.GetPosition() < final_position)
	{
		const auto child_pos = in.GetPosition();
		uint16_t type = 0;
		uint32_t size = 0;
		in >> type >> size;
		in.SetPosition(child_pos);
		const auto type_enum = static_cast<EChildType>(type);

		switch (type_enum)
		{
		case EChildType::ApproachLegs:
			m_approach_legs.write().ReadBinary(in);
			// TODO - error handling
			if (!m_approach_legs.write().Validate())
			{
				return;
			}
			break;
		case EChildType::MissedApproachLegs:
			m_missed_approach_legs.write().ReadBinary(in);
			// TODO - error handling
			if (!m_missed_approach_legs.write().Validate())
			{
				return;
			}
			break;
		case EChildType::Transition:
			m_transitions.write()[transition_index++].ReadBinary(in);
			break;
		case EChildType::TransitionLegs: 
			[[fallthrough]]
		case EChildType::None:
			return;
		}
	}
}

auto flightsimlib::io::CBglApproach::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->Suffix
		<< m_data->Number
		<< m_data->TypeDesignator
		<< m_data->TransitionCount
		<< m_data->ApproachLegCount
		<< m_data->MissedApproachLegCount
		<< m_data->IcaoIdent
		<< m_data->RegionIdent
		<< m_data->Altitude
		<< m_data->Heading
		<< m_data->MissedAltitude;

	if (!m_approach_legs->IsEmpty())
	{
		m_approach_legs.write().WriteBinary(out);
	}

	if (!m_missed_approach_legs->IsEmpty())
	{
		m_missed_approach_legs.write().WriteBinary(out);
	}
	
	for (auto& transition : m_transitions.write())
	{
		transition.WriteBinary(out);
	}
}

auto flightsimlib::io::CBglApproach::Validate() -> bool
{
	m_data.write().Size = static_cast<int>(CalculateSize());
	return true;
}

auto flightsimlib::io::CBglApproach::CalculateSize() const -> int
{
	auto count = static_cast<int>(sizeof(SBglApproachData));
	if (!m_approach_legs->IsEmpty())
	{
		count += m_approach_legs->CalculateSize();
	}
	if (!m_missed_approach_legs->IsEmpty())
	{
		count += m_missed_approach_legs->CalculateSize();
	}
	for (const auto& transition : m_transitions.read())
	{
		count += transition.CalculateSize();
	}
	return count;
}

auto flightsimlib::io::CBglApproach::GetSuffix() const -> char
{
	return static_cast<char>(m_data->Suffix);
}

auto flightsimlib::io::CBglApproach::SetSuffix(char value) -> void
{
	m_data.write().Suffix = value;
}

auto flightsimlib::io::CBglApproach::GetRunwayNumber() const -> IBglRunway::ERunwayNumber
{
	return static_cast<IBglRunway::ERunwayNumber>(m_data->Number);
}

auto flightsimlib::io::CBglApproach::SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void
{
	m_data.write().Number = to_integral(value);
}

auto flightsimlib::io::CBglApproach::GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator
{
	return static_cast<IBglRunway::ERunwayDesignator>(
		get_packed_bits(m_data->TypeDesignator, 3, 4));
}

auto flightsimlib::io::CBglApproach::SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void
{
	set_packed_bits(m_data.write().TypeDesignator, to_integral(value), 3, 4);
}

auto flightsimlib::io::CBglApproach::GetType() const -> EType
{
	return static_cast<EType>(get_packed_bits(m_data->TypeDesignator, 4, 0));
}

auto flightsimlib::io::CBglApproach::SetType(EType value) -> void
{
	set_packed_bits(m_data.write().TypeDesignator, to_integral(value), 4, 0);
}

auto flightsimlib::io::CBglApproach::HasGpsOverlay() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->TypeDesignator, 1, 7));
}

auto flightsimlib::io::CBglApproach::SetGpsOverlay(bool value) -> void
{
	set_packed_bits(m_data.write().TypeDesignator, value, 1, 7);
}

auto flightsimlib::io::CBglApproach::GetFixType() const -> IBglLeg::EFixType
{
	return static_cast<IBglLeg::EFixType>(get_packed_bits(m_data->IcaoIdent, 5, 0));
}

auto flightsimlib::io::CBglApproach::SetFixType(IBglLeg::EFixType value) -> void
{
	set_packed_bits(m_data.write().IcaoIdent, to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglApproach::GetIcaoIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->IcaoIdent, 27, 5));
}

auto flightsimlib::io::CBglApproach::SetIcaoIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().IcaoIdent, value, 27, 5);
}

auto flightsimlib::io::CBglApproach::GetRegionIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 11, 0));
}

auto flightsimlib::io::CBglApproach::SetRegionIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 11, 0);
}

auto flightsimlib::io::CBglApproach::GetIcaoAirport() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 21, 11));
}

auto flightsimlib::io::CBglApproach::SetIcaoAirport(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 21, 11);
}

auto flightsimlib::io::CBglApproach::GetAltitude() const -> float
{
	return m_data->Altitude;
}

auto flightsimlib::io::CBglApproach::SetAltitude(float value) -> void
{
	m_data.write().Altitude = value;
}

auto flightsimlib::io::CBglApproach::GetHeading() const -> float
{
	return m_data->Heading;
}

auto flightsimlib::io::CBglApproach::SetHeading(float value) -> void
{
	m_data.write().Heading = value;
}

auto flightsimlib::io::CBglApproach::GetMissedAltitude() const -> float
{
	return m_data->MissedAltitude;
}

auto flightsimlib::io::CBglApproach::SetMissedAltitude(float value) -> void
{
	m_data.write().MissedAltitude = value;
}

auto flightsimlib::io::CBglApproach::GetTransitionCount() const -> int
{
	return static_cast<int>(m_data->TransitionCount);
}

auto flightsimlib::io::CBglApproach::GetApproachLegCount() const -> int
{
	return static_cast<int>(m_data->ApproachLegCount);
}

auto flightsimlib::io::CBglApproach::GetMissedApproachLegCount() const -> int
{
	return static_cast<int>(m_data->MissedApproachLegCount);
}

auto flightsimlib::io::CBglApproach::GetApproachLegs() -> IBglLegs*
{
	if (m_approach_legs->IsEmpty())
	{
		return nullptr;
	}
	return &m_approach_legs.write();
}

auto flightsimlib::io::CBglApproach::SetApproachLegs(IBglLegs* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_approach_legs = { *static_cast<CBglLegs*>(value) };
}

auto flightsimlib::io::CBglApproach::GetMissedApproachLegs() -> IBglLegs*
{
	if (m_missed_approach_legs->IsEmpty())
	{
		return nullptr;
	}
	return &m_missed_approach_legs.write();
}

auto flightsimlib::io::CBglApproach::SetMissedApproachLegs(IBglLegs* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_missed_approach_legs = { *static_cast<CBglLegs*>(value) };
}

auto flightsimlib::io::CBglApproach::GetTransitionAt(int index) -> IBglTransition*
{
	return &(m_transitions.write()[index]);
}

auto flightsimlib::io::CBglApproach::AddTransition(const IBglTransition* transition) -> void
{
	m_transitions.write().emplace_back(*static_cast<const CBglTransition*>(transition));
}

auto flightsimlib::io::CBglApproach::RemoveTransition(const IBglTransition* transition) -> void
{
	const auto iter = m_transitions.read().begin() +
		std::distance(m_transitions.read().data(), static_cast<const CBglTransition*>(transition));
	m_transitions.write().erase(iter);
}


//******************************************************************************
// CBglFence
//******************************************************************************  


auto flightsimlib::io::CBglFence::ReadBinary(BinaryFileStream& in) -> void
{
	assert(m_vertices->empty());

	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.VertexCount
		>> data.InstanceId
		>> data.Profile;

	const auto count = GetVertexCount();
	m_vertices.write().resize(count);

	for (auto& vertex : m_vertices.write())
	{
		in >> vertex.Longitude >> vertex.Latitude;
	}
}

auto flightsimlib::io::CBglFence::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->VertexCount
		<< m_data->InstanceId
		<< m_data->Profile;

	for (const auto& vertex : m_vertices.read())
	{
		out << vertex.Longitude << vertex.Latitude;
	}
}

auto flightsimlib::io::CBglFence::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglFence::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglFenceData) +
		m_data->VertexCount * sizeof(SBglVertexLL));
}

auto flightsimlib::io::CBglFence::GetType() const -> EType
{
	return static_cast<EType>(m_data->Type);
}

auto flightsimlib::io::CBglFence::SetType(EType value) -> void
{
	m_data.write().Type = to_integral(value);
}

auto flightsimlib::io::CBglFence::GetVertexCount() const -> int
{
	return static_cast<int>(m_data->VertexCount);
}

auto flightsimlib::io::CBglFence::GetInstanceId() const -> _GUID
{
	return m_data->InstanceId;
}

auto flightsimlib::io::CBglFence::SetInstanceId(_GUID value) -> void
{
	m_data.write().InstanceId = value;
}

auto flightsimlib::io::CBglFence::GetProfile() const -> _GUID
{
	return m_data->Profile;
}

auto flightsimlib::io::CBglFence::SetProfile(_GUID value) -> void
{
	m_data.write().Profile = value;
}

auto flightsimlib::io::CBglFence::GetVertexAt(int index) -> SBglVertexLL*
{
	return &(m_vertices.write()[index]);
}

auto flightsimlib::io::CBglFence::AddVertex(const SBglVertexLL* vertex) -> void
{
	m_vertices.write().emplace_back(*vertex);
	++m_data.write().VertexCount;
}

auto flightsimlib::io::CBglFence::RemoveVertex(const SBglVertexLL* vertex) -> void
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), vertex);
	m_vertices.write().erase(iter);
	--m_data.write().VertexCount;
}


//******************************************************************************
// CBglAirport
//******************************************************************************


auto flightsimlib::io::CBglAirport::ReadBinary(BinaryFileStream& in) -> void
{
	const auto initial_pos = in.GetPosition();

	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.RunwayCount
		>> data.FrequencyCount
		>> data.StartCount
		>> data.ApproachCount
		>> data.ApronCount
		>> data.HelipadCount
		>> data.Longitude
		>> data.Latitude
		>> data.Altitude
		>> data.TowerLongitude
		>> data.TowerLatitude
		>> data.TowerAltitude
		>> data.MagVar
		>> data.IcaoIdent
		>> data.RegionIdent
		>> data.FuelAvailability
		>> data.TrafficScalar
		>> data.Pad;

	m_runways.write().reserve(m_data->RunwayCount);
	m_starts.write().reserve(m_data->StartCount);
	
	const auto final_position = initial_pos + static_cast<int>(m_data->Size);
	while (in.GetPosition() < final_position)
	{
		const auto child_pos = in.GetPosition();
		uint16_t type = 0;
		uint32_t size = 0;
		in >> type >> size;
		in.SetPosition(child_pos);
		const auto type_enum = static_cast<EBglLayerType>(type);

		switch (type_enum)
		{
		case EBglLayerType::Runway:
			{
				auto runway = CBglRunway{};
				runway.ReadBinary(in);
				if (!runway.Validate())
				{
					return; // TODO - error handling?
				}
				m_runways.write().emplace_back(std::move(runway));
			}
			break;
		case EBglLayerType::Start:
			{
				auto start = CBglStart{};
				start.ReadBinary(in);
				if (!start.Validate())
				{
					return; // TODO - error handling?
				}
				m_starts.write().emplace_back(std::move(start));
			}
			break;
		case EBglLayerType::Com:
			{
				auto com = CBglCom{};
				com.ReadBinary(in);
				if (!com.Validate())
				{
					return; 
				}
				m_coms.write().emplace_back(std::move(com));
			}
			break;
		case EBglLayerType::Helipad:
			{
				auto helipad = CBglHelipad{};
				helipad.ReadBinary(in);
				if (!helipad.Validate())
				{
					return;
				}
				m_helipads.write().emplace_back(std::move(helipad));
			}
			break;
		case EBglLayerType::AirportDelete:
			m_delete.write().ReadBinary(in);
			if (!m_delete.write().Validate())
			{
				return;
			}
			break;
		case EBglLayerType::ApronEdgeLights:
			m_apron_edge_lights.write().ReadBinary(in);
			if (!m_apron_edge_lights.write().Validate())
			{
				return;
			}
			break;
		case EBglLayerType::Apron:
			{
				auto apron = CBglApron{};
				apron.ReadBinary(in);
				if (!apron.Validate())
				{
					return;
				}
				m_aprons.write().emplace_back(std::move(apron));
			}
			break;
		case EBglLayerType::ApronPolygons:
			{
				auto apron_polygons = CBglApronPolygons{};
				apron_polygons.ReadBinary(in);
				if (!apron_polygons.Validate())
				{
					return;
				}
				m_apron_polygons.write().emplace_back(std::move(apron_polygons));
			}
			break;
		case EBglLayerType::TaxiwayPoint:
			m_taxiway_points.write().ReadBinary(in);
			if (!m_taxiway_points.write().Validate())
			{
				return;
			}
			break;
		case EBglLayerType::TaxiwayParking:
			m_taxiway_parkings.write().ReadBinary(in);
			if (!m_taxiway_parkings.write().Validate())
			{
				return;
			}
			break;
		case EBglLayerType::TaxiwayPath:
			m_taxiway_paths.write().ReadBinary(in);
			if (!m_taxiway_paths.write().Validate())
			{
				return;
			}
			break;
		case EBglLayerType::TaxiName:
			m_taxiway_names.write().ReadBinary(in);
			if (!m_taxiway_names.write().Validate())
			{
				return;
			}
			break;
		case EBglLayerType::Jetway:
			{
				auto jetway = CBglJetway{};
				jetway.ReadBinary(in);
				if (!jetway.Validate())
				{
					return;
				}
				m_jetways.write().emplace_back(std::move(jetway));
			}
			break;
		case EBglLayerType::BlastFence:
			[[fallthrough]]
		case EBglLayerType::BoundaryFence:
			{
				auto fence = CBglFence{};
				fence.ReadBinary(in);
				if (!fence.Validate())
				{
					return;
				}
				if (type_enum == EBglLayerType::BlastFence)
				{
					m_blast_fences.write().emplace_back(std::move(fence));
				}
				else
				{
					m_boundary_fences.write().emplace_back(std::move(fence));
				}
			}
			break;
		case EBglLayerType::Approach:
			{
				auto approach = CBglApproach{};
				approach.ReadBinary(in);
				if (!approach.Validate())
				{
					return;
				}
				m_approaches.write().emplace_back(std::move(approach));
			}
			break;
		case EBglLayerType::Name:
			CBglName::ReadBinary(in);
			break;
		default:
			break;
		}

		in.SetPosition(child_pos + static_cast<int>(size));
	}
}

auto flightsimlib::io::CBglAirport::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->RunwayCount
		<< m_data->FrequencyCount
		<< m_data->StartCount
		<< m_data->ApproachCount
		<< m_data->ApronCount
		<< m_data->HelipadCount
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Altitude
		<< m_data->TowerLongitude
		<< m_data->TowerLatitude
		<< m_data->TowerAltitude
		<< m_data->MagVar
		<< m_data->IcaoIdent
		<< m_data->RegionIdent
		<< m_data->FuelAvailability
		<< m_data->TrafficScalar
		<< m_data->Pad;

	CBglName::WriteBinary(out);
	
	for (auto& runway : m_runways.write())
	{
		runway.WriteBinary(out);
	}

	for (auto& start : m_starts.write())
	{
		start.WriteBinary(out);
	}

	for (auto& com : m_coms.write())
	{
		com.WriteBinary(out);
	}

	for (auto& helipad : m_helipads.write())
	{
		helipad.WriteBinary(out);
	}

	if (!m_delete->IsEmpty())
	{
		m_delete.write().WriteBinary(out);
	}

	if (!m_apron_edge_lights->IsEmpty())
	{
		m_apron_edge_lights.write().WriteBinary(out);
	}

	for (auto i = 0; i < m_data->ApronCount; ++i)
	{
		m_aprons.write()[i].WriteBinary(out);
		m_apron_polygons.write()[i].WriteBinary(out);
	}

	if (!m_taxiway_points->IsEmpty())
	{
		m_taxiway_points.write().WriteBinary(out);
	}

	if (!m_taxiway_parkings->IsEmpty())
	{
		m_taxiway_parkings.write().WriteBinary(out);
	}

	if (!m_taxiway_paths->IsEmpty())
	{
		m_taxiway_paths.write().WriteBinary(out);
	}

	if (!m_taxiway_names->IsEmpty())
	{
		m_taxiway_names.write().WriteBinary(out);
	}

	for (auto& jetway : m_jetways.write())
	{
		jetway.WriteBinary(out);
	}

	for (auto& fence : m_blast_fences.write())
	{
		fence.WriteBinary(out);
	}

	for (auto& fence : m_boundary_fences.write())
	{
		fence.WriteBinary(out);
	}

	for (auto& approach : m_approaches.write())
	{
		approach.WriteBinary(out);
	}
	
}

auto flightsimlib::io::CBglAirport::Validate() -> bool
{
	auto count = static_cast<int>(sizeof(SBglAirportData)) +
		CBglName::CalculateSize();

	for (const auto& runway : m_runways.read())
	{
		count += runway.CalculateSize();
	}

	for (const auto& start : m_starts.read())
	{
		count += start.CalculateSize();
	}

	for (const auto& com : m_coms.read())
	{
		count += com.CalculateSize();
	}

	for (const auto& helipad : m_helipads.read())
	{
		count += helipad.CalculateSize();
	}

	if (!m_delete->IsEmpty())
	{
		count += m_delete->CalculateSize();
	}

	if (!m_apron_edge_lights->IsEmpty())
	{
		count += m_apron_edge_lights->CalculateSize();
	}

	for (const auto& apron : m_aprons.read())
	{
		count += apron.CalculateSize();
	}

	for (const auto& apron_polygons : m_apron_polygons.read())
	{
		count += apron_polygons.CalculateSize();
	}

	if (!m_taxiway_points->IsEmpty())
	{
		count +=  m_taxiway_points->CalculateSize();
	}

	if (!m_taxiway_parkings->IsEmpty())
	{
		count += m_taxiway_parkings->CalculateSize();
	}

	if (!m_taxiway_paths->IsEmpty())
	{
		count += m_taxiway_paths->CalculateSize();
	}

	if (!m_taxiway_names->IsEmpty())
	{
		count += m_taxiway_names->CalculateSize();
	}

	for (const auto& jetway : m_jetways.read())
	{
		count += jetway.CalculateSize();
	}

	for (const auto& fence : m_blast_fences.read())
	{
		count += fence.CalculateSize();
	}

	for (const auto& fence : m_boundary_fences.read())
	{
		count += fence.CalculateSize();
	}

	for (const auto& approach : m_approaches.read())
	{
		count += approach.CalculateSize();
	}
	
	m_data.write().Size = count;
	
	return m_data->Type == 0x3C;
}

auto flightsimlib::io::CBglAirport::CalculateSize() const -> int
{
	return m_data->Size;
}

auto flightsimlib::io::CBglAirport::GetRunwayCount() const -> int
{
	return m_data->RunwayCount;
}

auto flightsimlib::io::CBglAirport::GetFrequencyCount() const -> int
{
	return m_data->FrequencyCount;
}

auto flightsimlib::io::CBglAirport::GetStartCount() const -> int
{
	return m_data->StartCount;
}

auto flightsimlib::io::CBglAirport::GetApproachCount() const -> int
{
	return m_data->ApproachCount;
}

auto flightsimlib::io::CBglAirport::GetApronCount() const -> int
{
	return static_cast<int>(get_packed_bits(m_data->ApronCount, 7, 0));
}

auto flightsimlib::io::CBglAirport::IsDeleteAirport() const -> bool
{
	return static_cast<bool>(get_packed_bits(m_data->ApronCount, 1, 7));
}

auto flightsimlib::io::CBglAirport::SetDeleteAirport(bool value) -> void
{
	set_packed_bits(m_data.write().ApronCount, value, 1, 7);
}

auto flightsimlib::io::CBglAirport::GetHelipadCount() const -> int
{
	return m_data->HelipadCount;
}

auto flightsimlib::io::CBglAirport::GetTowerLongitude() const -> double
{
	return Longitude::Value(m_data->TowerLongitude);
}

auto flightsimlib::io::CBglAirport::SetTowerLongitude(double value) -> void
{
	m_data.write().TowerLongitude = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglAirport::GetTowerLatitude() const -> double
{
	return Latitude::Value(m_data->TowerLatitude);
}

auto flightsimlib::io::CBglAirport::SetTowerLatitude(double value) -> void
{
	m_data.write().TowerLatitude = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglAirport::GetTowerAltitude() const -> double
{
	return PackedAltitude::Value(m_data->TowerAltitude);
}

auto flightsimlib::io::CBglAirport::SetTowerAltitude(double value) -> void
{
	m_data.write().TowerAltitude = PackedAltitude::FromDouble(value);
}

auto flightsimlib::io::CBglAirport::GetMagVar() const -> float
{
	return m_data->MagVar;
}

auto flightsimlib::io::CBglAirport::SetMagVar(float value) -> void
{
	m_data.write().MagVar = value;
}

auto flightsimlib::io::CBglAirport::GetIcaoIdent() const -> uint32_t
{
	return m_data->IcaoIdent;
}

auto flightsimlib::io::CBglAirport::SetIcaoIdent(uint32_t value) -> void
{
	m_data.write().IcaoIdent = value;
}

auto flightsimlib::io::CBglAirport::GetRegionIdent() const -> uint32_t
{
	return m_data->RegionIdent;
}

auto flightsimlib::io::CBglAirport::SetRegionIdent(uint32_t value) -> void
{
	m_data.write().RegionIdent = value;
}

auto flightsimlib::io::CBglAirport::GetTrafficScalar() const -> float
{
	return static_cast<float>(m_data->TrafficScalar) / 
		static_cast<float>(std::numeric_limits<uint16_t>::max());
}

auto flightsimlib::io::CBglAirport::SetTrafficScalar(float value) -> void
{
	m_data.write().TrafficScalar = static_cast<uint16_t>(
		value * std::numeric_limits<uint16_t>::max());
}

auto flightsimlib::io::CBglAirport::GetRunwayAt(int index) -> IBglRunway*
{
	return &(m_runways.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddRunway(const IBglRunway* runway) -> void
{
	// TODO Need validation, self check
	m_runways.write().emplace_back(*static_cast<const CBglRunway*>(runway));
}

auto flightsimlib::io::CBglAirport::RemoveRunway(const IBglRunway* runway) -> void
{
	const auto iter = m_runways.read().begin() +
		std::distance(m_runways.read().data(), static_cast<const CBglRunway*>(runway));
	m_runways.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetStartAt(int index) -> IBglStart*
{
	return &(m_starts.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddStart(const IBglStart* start) -> void
{
	m_starts.write().emplace_back(*static_cast<const CBglStart*>(start));
}

auto flightsimlib::io::CBglAirport::RemoveStart(const IBglStart* start) -> void
{
	const auto iter = m_starts.read().begin() +
		std::distance(m_starts.read().data(), static_cast<const CBglStart*>(start));
	m_starts.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetComAt(int index) -> IBglCom*
{
	return &(m_coms.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddCom(const IBglCom* com) -> void
{
	m_coms.write().emplace_back(*static_cast<const CBglCom*>(com));
}

auto flightsimlib::io::CBglAirport::RemoveCom(const IBglCom* com) -> void
{
	const auto iter = m_coms.read().begin() +
		std::distance(m_coms.read().data(), static_cast<const CBglCom*>(com));
	m_coms.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetHelipadAt(int index) -> IBglHelipad*
{
	return &(m_helipads.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddHelipad(const IBglHelipad* helipad) -> void
{
	m_helipads.write().emplace_back(*static_cast<const CBglHelipad*>(helipad));
}

auto flightsimlib::io::CBglAirport::RemoveHelipad(const IBglHelipad* helipad) -> void
{
	const auto iter = m_helipads.read().begin() +
		std::distance(m_helipads.read().data(), static_cast<const CBglHelipad*>(helipad));
	m_helipads.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetDelete() -> IBglAirportDelete*
{
	if (m_delete->IsEmpty())
	{
		return nullptr;
	}
	return &m_delete.write();
}

auto flightsimlib::io::CBglAirport::SetDelete(IBglAirportDelete* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_delete = { *static_cast<CBglAirportDelete*>(value) };
}

auto flightsimlib::io::CBglAirport::GetApronEdgeLights() -> IBglApronEdgeLights*
{
	if (m_apron_edge_lights->IsEmpty())
	{
		return nullptr;
	}
	return &m_apron_edge_lights.write();
}

auto flightsimlib::io::CBglAirport::SetApronEdgeLights(IBglApronEdgeLights* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_apron_edge_lights = { *static_cast<CBglApronEdgeLights*>(value) };
}

auto flightsimlib::io::CBglAirport::GetApronAt(int index) -> IBglApron*
{
	return &(m_aprons.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddApron(const IBglApron* apron) -> void
{
	m_aprons.write().emplace_back(*static_cast<const CBglApron*>(apron));
}

auto flightsimlib::io::CBglAirport::RemoveApron(const IBglApron* apron) -> void
{
	const auto iter = m_aprons.read().begin() +
		std::distance(m_aprons.read().data(), static_cast<const CBglApron*>(apron));
	m_aprons.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetApronPolygonsAt(int index) -> IBglApronPolygons*
{
	return &(m_apron_polygons.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddApronPolygons(const IBglApronPolygons* polygons) -> void
{
	m_apron_polygons.write().emplace_back(*static_cast<const CBglApronPolygons*>(polygons));
}

auto flightsimlib::io::CBglAirport::RemoveApronPolygons(const IBglApronPolygons* polygons) -> void
{
	const auto iter = m_apron_polygons.read().begin() +
		std::distance(m_apron_polygons.read().data(), static_cast<const CBglApronPolygons*>(polygons));
	m_apron_polygons.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetTaxiwayPoints() -> IBglTaxiwayPoints*
{
	if (m_taxiway_points->IsEmpty())
	{
		return nullptr;
	}
	return &m_taxiway_points.write();
}

auto flightsimlib::io::CBglAirport::SetTaxiwayPoints(IBglTaxiwayPoints* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_taxiway_points = { *static_cast<CBglTaxiwayPoints*>(value) };
}

auto flightsimlib::io::CBglAirport::GetTaxiwayParkings() -> IBglTaxiwayParkings*
{
	if (m_taxiway_parkings->IsEmpty())
	{
		return nullptr;
	}
	return &m_taxiway_parkings.write();
}

auto flightsimlib::io::CBglAirport::SetTaxiwayParkings(IBglTaxiwayParkings* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_taxiway_parkings = { *static_cast<CBglTaxiwayParkings*>(value) };
}

auto flightsimlib::io::CBglAirport::GetTaxiwayPaths() -> IBglTaxiwayPaths*
{
	if (m_taxiway_paths->IsEmpty())
	{
		return nullptr;
	}
	return &m_taxiway_paths.write();
}

auto flightsimlib::io::CBglAirport::SetTaxiwayPaths(IBglTaxiwayPaths* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_taxiway_paths = { *static_cast<CBglTaxiwayPaths*>(value) };
}

auto flightsimlib::io::CBglAirport::GetTaxiwayNames() -> IBglTaxiwayNames*
{
	if (m_taxiway_names->IsEmpty())
	{
		return nullptr;
	}
	return &m_taxiway_names.write();
}

auto flightsimlib::io::CBglAirport::SetTaxiwayNames(IBglTaxiwayNames* value) -> void
{
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_taxiway_names = { *static_cast<CBglTaxiwayNames*>(value) };
}

auto flightsimlib::io::CBglAirport::GetJetwayCount() const -> int
{
	return static_cast<int>(m_jetways->size());
}

auto flightsimlib::io::CBglAirport::GetJetwayAt(int index) -> IBglJetway*
{
	return &(m_jetways.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddJetway(const IBglJetway* jetway) -> void
{
	m_jetways.write().emplace_back(*static_cast<const CBglJetway*>(jetway));
}

auto flightsimlib::io::CBglAirport::RemoveJetway(const IBglJetway* jetway) -> void
{
	const auto iter = m_jetways.read().begin() +
		std::distance(m_jetways.read().data(), static_cast<const CBglJetway*>(jetway));
	m_jetways.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetApproachAt(int index) -> IBglApproach*
{
	return &(m_approaches.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddApproach(const IBglApproach* approach) -> void
{
	m_approaches.write().emplace_back(*static_cast<const CBglApproach*>(approach));
}

auto flightsimlib::io::CBglAirport::RemoveApproach(const IBglApproach* approach) -> void
{
	const auto iter = m_approaches.read().begin() +
		std::distance(m_approaches.read().data(), static_cast<const CBglApproach*>(approach));
	m_approaches.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetBlastFenceCount() const -> int
{
	return static_cast<int>(m_blast_fences->size());
}

auto flightsimlib::io::CBglAirport::GetBlastFenceAt(int index) -> IBglFence*
{
	return &(m_blast_fences.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddBlastFence(const IBglFence* fence) -> void
{
	m_blast_fences.write().emplace_back(*static_cast<const CBglFence*>(fence));
}

auto flightsimlib::io::CBglAirport::RemoveBlastFence(const IBglFence* fence) -> void
{
	const auto iter = m_blast_fences.read().begin() +
		std::distance(m_blast_fences.read().data(), static_cast<const CBglFence*>(fence));
	m_blast_fences.write().erase(iter);
}

auto flightsimlib::io::CBglAirport::GetBoundaryFenceCount() const -> int
{
	return static_cast<int>(m_boundary_fences->size());
}

auto flightsimlib::io::CBglAirport::GetBoundaryFenceAt(int index) -> IBglFence*
{
	return &(m_boundary_fences.write()[index]);
}

auto flightsimlib::io::CBglAirport::AddBoundaryFence(const IBglFence* fence) -> void
{
	m_boundary_fences.write().emplace_back(*static_cast<const CBglFence*>(fence));
}

auto flightsimlib::io::CBglAirport::RemoveBoundaryFence(const IBglFence* fence) -> void
{
	const auto iter = m_boundary_fences.read().begin() +
		std::distance(m_boundary_fences.read().data(), static_cast<const CBglFence*>(fence));
	m_boundary_fences.write().erase(iter);
}


//******************************************************************************
// CBglAirportSummary
//******************************************************************************  


auto flightsimlib::io::CBglAirportSummary::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.ApproachAvailability
		>> data.Longitude
		>> data.Latitude
		>> data.Altitude
		>> data.IcaoIdent
		>> data.RegionIdent
		>> data.MagVar
		>> data.LongestRunwayLength
		>> data.LongestRunwayHeading
		>> data.FuelAvailability;
}

auto flightsimlib::io::CBglAirportSummary::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->ApproachAvailability
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Altitude
		<< m_data->IcaoIdent
		<< m_data->RegionIdent
		<< m_data->MagVar
		<< m_data->LongestRunwayLength
		<< m_data->LongestRunwayHeading
		<< m_data->FuelAvailability;
}

auto flightsimlib::io::CBglAirportSummary::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglAirportSummary::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglAirportSummaryData));
}

auto flightsimlib::io::CBglAirportSummary::HasCom() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::Com)));
}

auto flightsimlib::io::CBglAirportSummary::SetCom(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::Com));
}

auto flightsimlib::io::CBglAirportSummary::HasPavedRunway() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::PavedRunway)));
}

auto flightsimlib::io::CBglAirportSummary::SetPavedRunway(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::PavedRunway));
}

auto flightsimlib::io::CBglAirportSummary::HasOnlyWaterRunway() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::OnlyWaterRunway)));
}

auto flightsimlib::io::CBglAirportSummary::SetOnlyWaterRunway(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::OnlyWaterRunway));
}

auto flightsimlib::io::CBglAirportSummary::HasGpsApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::GpsApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetGpsApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::GpsApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasVorApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::VorApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetVorApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::VorApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasNdbApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::NdbApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetNdbApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::NdbApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasIlsApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::IlsApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetIlsApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::IlsApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasLocApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::LocApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetLocApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::LocApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasSdfApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::SdfApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetSdfApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::SdfApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasLdaApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::LdaApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetLdaApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::LdaApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasVorDmeApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::VorDmeApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetVorDmeApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::VorDmeApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasNdbDmeApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::NdbDmeApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetNdbDmeApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::NdbDmeApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasRnavApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::RnavApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetRnavApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::RnavApproach));
}

auto flightsimlib::io::CBglAirportSummary::HasLocBcApproach() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->ApproachAvailability, 1, to_integral(EFlags::LocBcApproach)));
}

auto flightsimlib::io::CBglAirportSummary::SetLocBcApproach(bool value) -> void
{
	set_packed_bits(m_data.write().ApproachAvailability, value, 1, to_integral(EFlags::LocBcApproach));
}

auto flightsimlib::io::CBglAirportSummary::GetIcaoIdent() const -> uint32_t
{
	return m_data->IcaoIdent;
}

auto flightsimlib::io::CBglAirportSummary::SetIcaoIdent(uint32_t value) -> void
{
	m_data.write().IcaoIdent = value;
}

auto flightsimlib::io::CBglAirportSummary::GetRegionIdent() const -> uint32_t
{
	return m_data->RegionIdent;
}

auto flightsimlib::io::CBglAirportSummary::SetRegionIdent(uint32_t value) -> void
{
	m_data.write().RegionIdent = value;
}

auto flightsimlib::io::CBglAirportSummary::GetMagVar() const -> float
{
	return m_data->MagVar;
}

auto flightsimlib::io::CBglAirportSummary::SetMagVar(float value) -> void
{
	m_data.write().MagVar = value;
}

auto flightsimlib::io::CBglAirportSummary::GetLongestRunwayLength() const -> float
{
	return m_data->LongestRunwayLength;
}

auto flightsimlib::io::CBglAirportSummary::SetLongestRunwayLength(float value) -> void
{
	m_data.write().LongestRunwayLength = value;
}

auto flightsimlib::io::CBglAirportSummary::GetLongestRunwayHeading() const -> float
{
	return m_data->LongestRunwayHeading;
}

auto flightsimlib::io::CBglAirportSummary::SetLongestRunwayHeading(float value) -> void
{
	m_data.write().LongestRunwayHeading = value;
}


//******************************************************************************
// CBglRoute
//******************************************************************************  


auto flightsimlib::io::CBglRoute::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.RouteType;

	data.Name.assign(in.ReadString(static_cast<int>(sizeof(uint64_t))));
	data.Name.erase(std::find(
		data.Name.begin(), data.Name.end(), '\0'), data.Name.end());
	
	in >> data.Previous.IcaoIdent
		>> data.Previous.RegionIdent
		>> data.Previous.AltitudeMinimum
		>> data.Next.IcaoIdent
		>> data.Next.RegionIdent
		>> data.Next.AltitudeMinimum;
}

auto flightsimlib::io::CBglRoute::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->RouteType;

	//TODO Pad Util
	const auto size = static_cast<int>(m_data->Name.size());
	out.Write(m_data->Name.c_str(), size);

	auto pad_size = 8 - size % 8;
	if (pad_size == 8 && size != 0)
	{
		pad_size = 0;
	}

	for (auto i = 0; i < pad_size; ++i)
	{
		auto pad = uint8_t{ 0 };
		out << pad;
	}
	
	out << m_data->Previous.IcaoIdent
		<< m_data->Previous.RegionIdent
		<< m_data->Previous.AltitudeMinimum
		<< m_data->Next.IcaoIdent
		<< m_data->Next.RegionIdent
		<< m_data->Next.AltitudeMinimum;
}

auto flightsimlib::io::CBglRoute::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglRoute::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(m_data->RouteType) + sizeof(uint64_t) +
		2 * sizeof(SBglConnectionData));
}

auto flightsimlib::io::CBglRoute::GetType() const -> EType
{
	return static_cast<EType>(m_data->RouteType);
}

auto flightsimlib::io::CBglRoute::SetType(EType value) -> void
{
	m_data.write().RouteType = to_integral(value);
}

auto flightsimlib::io::CBglRoute::GetName() const -> const char*
{
	return m_data->Name.c_str();
}

auto flightsimlib::io::CBglRoute::SetName(const char* value) -> void
{
	m_data.write().Name = value;
}

auto flightsimlib::io::CBglRoute::GetPreviousType() const -> EConnectionType
{
	return static_cast<EConnectionType>(get_packed_bits(m_data->Previous.IcaoIdent, 5, 0));
}

auto flightsimlib::io::CBglRoute::SetPreviousType(EConnectionType value) -> void
{
	set_packed_bits(m_data.write().Previous.IcaoIdent, to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglRoute::GetPreviousIcaoIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->Previous.IcaoIdent, 27, 5));
}

auto flightsimlib::io::CBglRoute::SetPreviousIcaoIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().Previous.IcaoIdent, value, 27, 5);
}

auto flightsimlib::io::CBglRoute::GetPreviousRegionIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->Previous.RegionIdent, 11, 0));
}

auto flightsimlib::io::CBglRoute::SetPreviousRegionIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().Previous.RegionIdent, value, 11, 0);
}

auto flightsimlib::io::CBglRoute::GetPreviousIcaoAirport() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->Previous.RegionIdent, 21, 11));
}

auto flightsimlib::io::CBglRoute::SetPreviousIcaoAirport(uint32_t value) -> void
{
	set_packed_bits(m_data.write().Previous.RegionIdent, value, 21, 11);
}

auto flightsimlib::io::CBglRoute::GetPreviousAltitudeMinimum() const -> float
{
	return m_data->Previous.AltitudeMinimum;
}

auto flightsimlib::io::CBglRoute::SetPreviousAltitudeMinimum(float value) -> void
{
	m_data.write().Previous.AltitudeMinimum = value;
}

auto flightsimlib::io::CBglRoute::GetNextType() const -> EConnectionType
{
	return static_cast<EConnectionType>(get_packed_bits(m_data->Next.IcaoIdent, 5, 0));
}

auto flightsimlib::io::CBglRoute::SetNextType(EConnectionType value) -> void
{
	set_packed_bits(m_data.write().Next.IcaoIdent, to_integral(value), 5, 0);
}

auto flightsimlib::io::CBglRoute::GetNextIcaoIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->Next.IcaoIdent, 27, 5));
}

auto flightsimlib::io::CBglRoute::SetNextIcaoIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().Next.IcaoIdent, value, 27, 5);
}

auto flightsimlib::io::CBglRoute::GetNextRegionIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->Next.RegionIdent, 11, 0));
}

auto flightsimlib::io::CBglRoute::SetNextRegionIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().Next.RegionIdent, value, 11, 0);
}

auto flightsimlib::io::CBglRoute::GetNextIcaoAirport() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->Next.RegionIdent, 21, 11));
}

auto flightsimlib::io::CBglRoute::SetNextIcaoAirport(uint32_t value) -> void
{
	set_packed_bits(m_data.write().Next.RegionIdent, value, 21, 11);
}

auto flightsimlib::io::CBglRoute::GetNextAltitudeMinimum() const -> float
{
	return m_data->Next.AltitudeMinimum;
}

auto flightsimlib::io::CBglRoute::SetNextAltitudeMinimum(float value) -> void
{
	m_data.write().Next.AltitudeMinimum = value;
}


//******************************************************************************
// CBglWaypoint
//******************************************************************************  


auto flightsimlib::io::CBglWaypoint::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();

	in >> data.Type
		>> data.Size
		>> data.WaypointType
		>> data.RouteCount
		>> data.Longitude
		>> data.Latitude
		>> data.Magvar
		>> data.IcaoIdent
		>> data.RegionIdent;
	
	m_routes.write().resize(m_data->RouteCount);

	for (auto& route : m_routes.write())
	{
		route.ReadBinary(in);
	}

	const auto pad_size = CalculatePadSize();
	auto pad = uint8_t{ 0 };

	for (auto i = 0; i < pad_size; ++i)
	{
		in >> pad;
	}
}

auto flightsimlib::io::CBglWaypoint::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->WaypointType
		<< m_data->RouteCount
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Magvar
		<< m_data->IcaoIdent
		<< m_data->RegionIdent;
	
	for (auto& route : m_routes.write())
	{
		route.WriteBinary(out);
	}

	const auto pad_size = CalculatePadSize();
	for (auto i = 0; i < pad_size; ++i)
	{
		auto pad = uint8_t{ 0 };
		out << pad;
	}
}

auto flightsimlib::io::CBglWaypoint::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglWaypoint::CalculateSize() const -> int
{
	auto count = static_cast<int>(sizeof(SBglWaypointData));
	for (const auto& route : m_routes.read())
	{
		count += route.CalculateSize();
	}
	return count + CalculatePadSize();
}

auto flightsimlib::io::CBglWaypoint::GetType() const -> EType
{
	return static_cast<EType>(m_data->WaypointType);
}

auto flightsimlib::io::CBglWaypoint::SetType(EType value) -> void
{
	m_data.write().WaypointType = to_integral(value);
}

auto flightsimlib::io::CBglWaypoint::GetRouteCount() const -> int
{
	return static_cast<int>(m_data->RouteCount);
}

auto flightsimlib::io::CBglWaypoint::GetLongitude() const -> double
{
	return Longitude::Value(m_data->Longitude);
}

auto flightsimlib::io::CBglWaypoint::SetLongitude(double value) -> void
{
	m_data.write().Longitude = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglWaypoint::GetLatitude() const -> double
{
	return Latitude::Value(m_data->Latitude);
}

auto flightsimlib::io::CBglWaypoint::SetLatitude(double value) -> void
{
	m_data.write().Latitude = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglWaypoint::GetMagVar() const -> float
{
	return m_data->Magvar;
}

auto flightsimlib::io::CBglWaypoint::SetMagVar(float value) -> void
{
	m_data.write().Magvar = value;
}

auto flightsimlib::io::CBglWaypoint::GetIcaoIdent() const -> uint32_t
{
	return m_data->IcaoIdent;
}

auto flightsimlib::io::CBglWaypoint::SetIcaoIdent(uint32_t value) -> void
{
	m_data.write().IcaoIdent = value;
}

auto flightsimlib::io::CBglWaypoint::GetRegionIdent() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 11, 0));
}

auto flightsimlib::io::CBglWaypoint::SetRegionIdent(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 11, 0);
}

auto flightsimlib::io::CBglWaypoint::GetIcaoAirport() const -> uint32_t
{
	return static_cast<uint32_t>(get_packed_bits(m_data->RegionIdent, 21, 11));
}

auto flightsimlib::io::CBglWaypoint::SetIcaoAirport(uint32_t value) -> void
{
	set_packed_bits(m_data.write().RegionIdent, value, 21, 11);
}

auto flightsimlib::io::CBglWaypoint::GetRouteAt(int index) -> IBglRoute*
{
	return &(m_routes.write()[index]);
}

auto flightsimlib::io::CBglWaypoint::AddRoute(const IBglRoute* route) -> void
{
	m_routes.write().emplace_back(*static_cast<const CBglRoute*>(route));
}

auto flightsimlib::io::CBglWaypoint::RemoveRoute(const IBglRoute* route) -> void
{
	const auto iter = m_routes.read().begin() +
		std::distance(m_routes.read().data(), static_cast<const CBglRoute*>(route));
	m_routes.write().erase(iter);
}

auto flightsimlib::io::CBglWaypoint::CalculatePadSize() const -> int
{
	// TODO - Pad utility
	const auto size = static_cast<int>(sizeof(SBglWaypointData) + 33 * m_data->RouteCount);
	const auto pad_size = 4 - size % 4;
	if (pad_size == 4)
	{
		return 0;
	}
	return pad_size;
}


//******************************************************************************
// CBglExclusion
//******************************************************************************  


auto flightsimlib::io::CBglExclusion::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Size
		>> data.MinLongitude
		>> data.MinLatitude
		>> data.MaxLongitude
		>> data.MaxLatitude;
}

auto flightsimlib::io::CBglExclusion::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Size
		<< m_data->MinLongitude
		<< m_data->MinLatitude
		<< m_data->MaxLongitude
		<< m_data->MaxLatitude;
}

auto flightsimlib::io::CBglExclusion::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglExclusion::CalculateSize() const -> int
{
	return sizeof(SBglExclusionData);
}

auto flightsimlib::io::CBglExclusion::GetMinLongitude() const -> double
{
	return Longitude::Value(m_data->MinLongitude);
}

auto flightsimlib::io::CBglExclusion::SetMinLongitude(double value) -> void
{
	m_data.write().MinLongitude = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglExclusion::GetMaxLongitude() const -> double
{
	return Longitude::Value(m_data->MaxLongitude);
}

auto flightsimlib::io::CBglExclusion::SetMaxLongitude(double value) -> void
{
	m_data.write().MaxLongitude = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglExclusion::GetMinLatitude() const -> double
{
	return Latitude::Value(m_data->MinLatitude);
}

auto flightsimlib::io::CBglExclusion::SetMinLatitude(double value) -> void
{
	m_data.write().MinLatitude = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglExclusion::GetMaxLatitude() const -> double
{
	return Latitude::Value(m_data->MaxLatitude);
}

auto flightsimlib::io::CBglExclusion::SetMaxLatitude(double value) -> void
{
	m_data.write().MaxLatitude = Latitude::ToPacked(value);
}

bool flightsimlib::io::CBglExclusion::IsExcludeAll() const
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::All)));
}

void flightsimlib::io::CBglExclusion::SetAll(bool value)
{
	m_data.write().Type = 1 << to_integral(EType::All); // Clear other flags
}

auto flightsimlib::io::CBglExclusion::IsBeacon() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::Beacon)));
}

auto flightsimlib::io::CBglExclusion::SetBeacon(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::Beacon));
}

auto flightsimlib::io::CBglExclusion::IsGenericBuilding() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::GenericBuilding)));
}

auto flightsimlib::io::CBglExclusion::SetGenericBuilding(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::GenericBuilding));
}

auto flightsimlib::io::CBglExclusion::IsEffect() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::Effect)));
}

auto flightsimlib::io::CBglExclusion::SetEffect(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::Effect));
}

auto flightsimlib::io::CBglExclusion::IsLibraryObject() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::LibraryObject)));
}

auto flightsimlib::io::CBglExclusion::SetLibraryObject(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::LibraryObject));
}

auto flightsimlib::io::CBglExclusion::IsTaxiwaySigns() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::TaxiwaySigns)));
}

auto flightsimlib::io::CBglExclusion::SetTaxiwaySigns(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::TaxiwaySigns));
}

auto flightsimlib::io::CBglExclusion::IsTrigger() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::Trigger)));
}

auto flightsimlib::io::CBglExclusion::SetTrigger(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::Trigger));
}

auto flightsimlib::io::CBglExclusion::IsWindsock() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::Windsock)));
}

auto flightsimlib::io::CBglExclusion::SetWindsock(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::Windsock));
}

auto flightsimlib::io::CBglExclusion::IsExtrusionBridge() const -> bool
{
	return static_cast<bool>(
		get_packed_bits(m_data->Type, 1, to_integral(EType::ExtrusionBridge)));
}

auto flightsimlib::io::CBglExclusion::SetExtrusionBridge(bool value) -> void
{
	set_packed_bits(m_data.write().Type, value, 1, to_integral(EType::ExtrusionBridge));
}


//******************************************************************************
// CBglMarker
//******************************************************************************  


auto flightsimlib::io::CBglMarker::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.SectionType
		>> data.Size
		>> data.UnusedType
		>> data.Heading
		>> data.MarkerType
		>> data.Longitude
		>> data.Latitude
		>> data.Altitude
		>> data.Icao
		>> data.Region;
}

auto flightsimlib::io::CBglMarker::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->SectionType
		<< m_data->Size
		<< m_data->UnusedType
		<< m_data->Heading
		<< m_data->MarkerType
		<< m_data->Longitude
		<< m_data->Latitude
		<< m_data->Altitude
		<< m_data->Icao
		<< m_data->Region;
}

auto flightsimlib::io::CBglMarker::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglMarker::CalculateSize() const -> int
{
	return sizeof(SBglMarkerData);
}

auto flightsimlib::io::CBglMarker::GetHeading() const -> float
{
	return static_cast<float>(ANGLE16::Value(m_data->Heading));
}

auto flightsimlib::io::CBglMarker::SetHeading(float value) -> void
{
	m_data.write().Heading = ANGLE16::FromDouble(static_cast<double>(value));
}

auto flightsimlib::io::CBglMarker::GetType() const -> EType
{
	return static_cast<EType>(m_data->MarkerType);
}

auto flightsimlib::io::CBglMarker::SetType(EType value) -> void
{
	m_data.write().MarkerType = to_integral(value);
}

auto flightsimlib::io::CBglMarker::GetLongitude() const -> double
{
	return Longitude::Value(m_data->Longitude);
}

auto flightsimlib::io::CBglMarker::SetLongitude(double value) -> void
{
	m_data.write().Longitude = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglMarker::GetLatitude() const -> double
{
	return Latitude::Value(m_data->Latitude);
}

auto flightsimlib::io::CBglMarker::SetLatitude(double value) -> void
{
	m_data.write().Latitude = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglMarker::GetAltitude() const -> double
{
	return PackedAltitude::Value(m_data->Altitude);
}

auto flightsimlib::io::CBglMarker::SetAltitude(double value) -> void
{
	m_data.write().Altitude = PackedAltitude::FromDouble(value);
}

auto flightsimlib::io::CBglMarker::GetIcaoIdent() const -> uint32_t
{
	return m_data->Icao;
}

auto flightsimlib::io::CBglMarker::SetIcaoIdent(uint32_t value) -> void
{
	m_data.write().Icao = value;
}

auto flightsimlib::io::CBglMarker::GetRegionIdent() const -> uint32_t
{
	return m_data->Region;
}

auto flightsimlib::io::CBglMarker::SetRegionIdent(uint32_t value) -> void
{
	m_data.write().Region = value;
}

//******************************************************************************
// CBglGeopol
//******************************************************************************  

auto flightsimlib::io::CBglGeopol::ReadBinary(BinaryFileStream& in) -> void
{
	assert(m_vertices->empty());
	
	auto& data = m_data.write();
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

auto flightsimlib::io::CBglGeopol::WriteBinary(BinaryFileStream& out) -> void
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

auto flightsimlib::io::CBglGeopol::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglGeopol::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglGeopolData) + GetVertexCount() * sizeof(SBglVertexLL));
}

auto flightsimlib::io::CBglGeopol::GetMinLongitude() const -> double
{
	return Longitude::Value(m_data->MinLongitude);
}

auto flightsimlib::io::CBglGeopol::GetType() const -> EType
{
	return static_cast<EType>((m_data->GeopolType >> 14) & 0xF);
}

auto flightsimlib::io::CBglGeopol::SetType(EType value) -> void
{
	m_data.write().GeopolType = 
		static_cast<uint16_t>((m_data->GeopolType & 0x3FFF) | (to_integral(value) << 14));
}

auto flightsimlib::io::CBglGeopol::SetMinLongitude(double value) -> void
{
	m_data.write().MinLongitude = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglGeopol::GetMaxLongitude() const -> double
{
	return Longitude::Value(m_data->MaxLongitude);
}

auto flightsimlib::io::CBglGeopol::SetMaxLongitude(double value) -> void
{
	m_data.write().MaxLongitude = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglGeopol::GetMinLatitude() const -> double
{
	return Latitude::Value(m_data->MinLatitude);
}

auto flightsimlib::io::CBglGeopol::SetMinLatitude(double value) -> void
{
	m_data.write().MinLatitude = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglGeopol::GetMaxLatitude() const -> double
{
	return Latitude::Value(m_data->MaxLatitude);
}

auto flightsimlib::io::CBglGeopol::SetMaxLatitude(double value) -> void
{
	m_data.write().MaxLatitude = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglGeopol::GetVertexCount() const -> int
{
	return m_data->GeopolType & 0x3FFF;
}

auto flightsimlib::io::CBglGeopol::GetVertexAt(int index) -> SBglVertexLL*
{
	return &(m_vertices.write()[index]);
}

auto flightsimlib::io::CBglGeopol::AddVertex(const SBglVertexLL* vertex) -> void
{
	// TODO Need validation, self check
	m_vertices.write().emplace_back(*vertex);
	SetVertexCount(static_cast<int>(m_vertices->size()));
}

auto flightsimlib::io::CBglGeopol::RemoveVertex(const SBglVertexLL* vertex) -> void
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), vertex);
	m_vertices.write().erase(iter);
	
	SetVertexCount(static_cast<int>(m_vertices->size()));
}

auto flightsimlib::io::CBglGeopol::SetVertexCount(int value) -> void
{
	m_data.write().GeopolType = 
		static_cast<uint16_t>((m_data->GeopolType & 0xC000) | value);
}


//******************************************************************************
// CBglSceneryObject
//****************************************************************************** 


void flightsimlib::io::CBglSceneryObject::ReadBinary(BinaryFileStream& in)
{
	auto& data = m_data.write();
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

flightsimlib::io::IBglSceneryObject::ESceneryObjectType flightsimlib::io::CBglSceneryObject::GetSceneryObjectType() const
{
	return static_cast<ESceneryObjectType>(m_data->SectionType);
}

void flightsimlib::io::CBglSceneryObject::SetSceneryObjectType(ESceneryObjectType value)
{
	m_data.write().SectionType = to_integral(value);
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

_GUID flightsimlib::io::CBglSceneryObject::GetInstanceId() const
{
	return m_data->InstanceId;
}

void flightsimlib::io::CBglSceneryObject::SetInstanceId(_GUID value)
{
	m_data.write().InstanceId = value;
}

flightsimlib::io::IBglGenericBuilding* flightsimlib::io::CBglSceneryObject::GetGenericBuilding()
{
	return dynamic_cast<IBglGenericBuilding*>(this);
}

flightsimlib::io::IBglLibraryObject* flightsimlib::io::CBglSceneryObject::GetLibraryObject()
{
	return dynamic_cast<IBglLibraryObject*>(this);
}

flightsimlib::io::IBglWindsock* flightsimlib::io::CBglSceneryObject::GetWindsock()
{
	return dynamic_cast<IBglWindsock*>(this);
}

flightsimlib::io::IBglEffect* flightsimlib::io::CBglSceneryObject::GetEffect()
{
	return dynamic_cast<IBglEffect*>(this);
}

flightsimlib::io::IBglTaxiwaySigns* flightsimlib::io::CBglSceneryObject::GetTaxiwaySigns()
{
	return dynamic_cast<IBglTaxiwaySigns*>(this);
}

flightsimlib::io::IBglTrigger* flightsimlib::io::CBglSceneryObject::GetTrigger()
{
	return dynamic_cast<IBglTrigger*>(this);
}

flightsimlib::io::IBglBeacon* flightsimlib::io::CBglSceneryObject::GetBeacon()
{
	return dynamic_cast<IBglBeacon*>(this);
}

flightsimlib::io::IBglExtrusionBridge* flightsimlib::io::CBglSceneryObject::GetExtrusionBridge()
{
	return dynamic_cast<IBglExtrusionBridge*>(this);
}

auto flightsimlib::io::CBglSceneryObject::IsEmpty() const -> bool
{
	if (m_data->SectionType == 0)
	{
		return true;
	}
	return false;
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
		auto& data = m_data.write();
		
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
		auto& data = m_data.write();
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
// CBglWindsock
//******************************************************************************  


void flightsimlib::io::CBglWindsock::ReadBinary(BinaryFileStream& in)
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto& data = m_data.write();
		in >> data.PoleHeight
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
		out << m_data->PoleHeight
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
// CBglEffect
//******************************************************************************  


void flightsimlib::io::CBglEffect::ReadBinary(BinaryFileStream& in)
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto& data = m_data.write();

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
		out.Write(params, static_cast<int>(strlen(params)));
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
	auto& data = m_data.write();
	in >> data.LongitudeBias
		>> data.LatitudeBias
		>> data.Heading
		>> data.Size
		>> data.Justification;
	
	data.Label = in.ReadCString();
	auto pad = uint8_t{ 0 };
	if (data.Label.size() % 2 == 0) // if it's even, we pad, as the c_str() is odd
	{
		in >> pad;
	}
}

auto flightsimlib::io::CBglTaxiwaySign::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->LongitudeBias
		<< m_data->LatitudeBias
		<< m_data->Heading
		<< m_data->Size
		<< m_data->Justification;

	const auto* const label = m_data->Label.c_str();
	const auto length = static_cast<int>(strlen(label)) + 1;
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
	const auto label_length = static_cast<int>(m_data->Label.size());
	return s_record_size + label_length + 1 + (label_length % 2 ? 0 : 1);
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
		auto sign_count = uint32_t{ 0 };
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
		const auto sign_count = static_cast<uint32_t>(m_signs.read().size());
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
	auto size = static_cast<int>(CBglSceneryObject::CalculateSize() + sizeof(uint32_t));
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

auto flightsimlib::io::CBglTaxiwaySigns::GetSignAt(int index) -> IBglTaxiwaySign*
{
	return &(m_signs.write()[index]);
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
// CBglTriggerRefuelRepair
//****************************************************************************** 


auto flightsimlib::io::CBglTriggerRefuelRepair::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.FuelAvailability
		>> data.PointCount;

	m_vertices.write().resize(data.PointCount);

	for (auto& vertex : m_vertices.write())
	{
		in >> vertex.X >> vertex.Z;
	}
}

auto flightsimlib::io::CBglTriggerRefuelRepair::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->FuelAvailability
		<< m_data->PointCount;

	for (const auto& vertex : m_vertices.write())
	{
		out << vertex.X << vertex.Z;
	}
}

auto flightsimlib::io::CBglTriggerRefuelRepair::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTriggerRefuelRepair::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTriggerRefuelRepairData) +
		sizeof(SBglVertexBias) * m_data->PointCount);
}

auto flightsimlib::io::CBglTriggerRefuelRepair::GetVertexCount() const -> int
{
	return m_data->PointCount;
}

auto flightsimlib::io::CBglTriggerRefuelRepair::GetVertexAt(int index) -> SBglVertexBias*
{
	return &(m_vertices.write()[index]);
}

auto flightsimlib::io::CBglTriggerRefuelRepair::AddVertex(const SBglVertexBias* point) -> void
{
	m_vertices.write().emplace_back(*point);
	++m_data.write().PointCount;
}

auto flightsimlib::io::CBglTriggerRefuelRepair::RemoveVertex(const SBglVertexBias* point) -> void
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), point);
	m_vertices.write().erase(iter);
	--m_data.write().PointCount;
}


//******************************************************************************
// CBglTriggerWeather
//****************************************************************************** 


auto flightsimlib::io::CBglTriggerWeather::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Type
		>> data.Heading
		>> data.Scalar
		>> data.PointCount;

	m_vertices.write().resize(data.PointCount);

	for (auto& vertex : m_vertices.write())
	{
		in >> vertex.X >> vertex.Z;
	}
}

auto flightsimlib::io::CBglTriggerWeather::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Type
		<< m_data->Heading
		<< m_data->Scalar
		<< m_data->PointCount;

	for (const auto& vertex : m_vertices.write())
	{
		out << vertex.X << vertex.Z;
	}
}

auto flightsimlib::io::CBglTriggerWeather::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTriggerWeather::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglTriggerWeatherData) +
		sizeof(SBglVertexBias) * m_data->PointCount);
}

auto flightsimlib::io::CBglTriggerWeather::GetTriggerHeading() const -> float
{
	return m_data->Heading;
}

auto flightsimlib::io::CBglTriggerWeather::SetTriggerHeading(float value) -> void
{
	m_data.write().Heading = value;
}

auto flightsimlib::io::CBglTriggerWeather::GetScalar() const -> float
{
	return m_data->Scalar;
}

auto flightsimlib::io::CBglTriggerWeather::SetScalar(float value) -> void
{
	m_data.write().Scalar = value;
}

auto flightsimlib::io::CBglTriggerWeather::GetVertexCount() const -> int
{
	return m_data->PointCount;
}

auto flightsimlib::io::CBglTriggerWeather::GetVertexAt(int index) -> SBglVertexBias*
{
	return &(m_vertices.write()[index]);
}

auto flightsimlib::io::CBglTriggerWeather::AddVertex(const SBglVertexBias* point) -> void
{
	m_vertices.write().emplace_back(*point);
	++m_data.write().PointCount;
}

auto flightsimlib::io::CBglTriggerWeather::RemoveVertex(const SBglVertexBias* point) -> void
{
	const auto iter = m_vertices.read().begin() +
		std::distance(m_vertices.read().data(), point);
	m_vertices.write().erase(iter);
	--m_data.write().PointCount;
}


//******************************************************************************
// CBglTrigger
//****************************************************************************** 


auto flightsimlib::io::CBglTrigger::ReadBinary(BinaryFileStream& in) -> void
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto& data = m_data.write();
		in >> data.Type
			>> data.Height;
		
		if (GetType() == EType::RefuelRepair)
		{
			m_refuel.write().ReadBinary(in);
		}
		else
		{
			m_weather.write().ReadBinary(in);
		}
	}
}

auto flightsimlib::io::CBglTrigger::WriteBinary(BinaryFileStream& out) -> void
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		out << m_data->Type
			<< m_data->Height;

		if (GetType() == EType::RefuelRepair)
		{
			m_refuel.write().WriteBinary(out);
		}
		else
		{
			m_weather.write().WriteBinary(out);
		}
	}
}

auto flightsimlib::io::CBglTrigger::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglTrigger::CalculateSize() const -> int
{
	return CBglSceneryObject::CalculateSize() + static_cast<int>(sizeof(SBglTriggerData)) +
		(GetType() == EType::RefuelRepair ? m_refuel->CalculateSize() : m_weather->CalculateSize());
}

auto flightsimlib::io::CBglTrigger::GetType() const -> EType
{
	return static_cast<EType>(m_data->Type);
}

auto flightsimlib::io::CBglTrigger::SetType(EType value) -> void
{
	m_data.write().Type = to_integral(value);
}

auto flightsimlib::io::CBglTrigger::GetHeight() const -> float
{
	return m_data->Height;
}

auto flightsimlib::io::CBglTrigger::SetHeight(float value) -> void
{
	m_data.write().Height = value;
}

auto flightsimlib::io::CBglTrigger::GetRepairRefuel() const -> const IBglTriggerRefuelRepair*
{
	if (GetType() == EType::RefuelRepair)
	{
		return m_refuel.operator->();
	}
	return nullptr;
}

auto flightsimlib::io::CBglTrigger::SetRepairRefuel(const IBglTriggerRefuelRepair* value) -> void
{
	// TODO - Do we need a clone function to steal the cow pointer, and not construct a new one?
	// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	m_refuel = { *static_cast<const CBglTriggerRefuelRepair*>(value) };
}

auto flightsimlib::io::CBglTrigger::GetWeather() const -> const IBglTriggerWeather*
{
	if (GetType() == EType::Weather)
	{
		return m_weather.operator->();
	}
	return nullptr;
}

auto flightsimlib::io::CBglTrigger::SetWeather(const IBglTriggerWeather* value) -> void
{
	m_weather = { *static_cast<const CBglTriggerWeather*>(value) };
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
// CBglExtrusionBridge
//****************************************************************************** 


auto flightsimlib::io::CBglExtrusionBridge::ReadBinary(BinaryFileStream& in) -> void
{
	CBglSceneryObject::ReadBinary(in);
	if (in)
	{
		auto& data = m_data.write();
		in >> data.ExtrusionProfile
			>> data.MaterialSet
			>> data.LongitudeSample1
			>> data.LatitudeSample1
			>> data.AltitudeSample1
			>> data.LongitudeSample2
			>> data.LatitudeSample2
			>> data.AltitudeSample2
			>> data.RoadWidth
			>> data.Probability
			>> data.SuppressPlatform
			>> data.PlacementCount
			>> data.PointCount;

		m_placements.write().resize(data.PlacementCount);

		for (auto& placement : m_placements.write())
		{
			in >> placement;
		}

		m_points.write().resize(data.PointCount);

		for (auto& point : m_points.write())
		{
			in >> point.Longitude >> point.Latitude >> point.Altitude;
		}
	}
}

auto flightsimlib::io::CBglExtrusionBridge::WriteBinary(BinaryFileStream& out) -> void
{
	CBglSceneryObject::WriteBinary(out);
	if (out)
	{
		out << m_data->ExtrusionProfile
			<< m_data->MaterialSet
			<< m_data->LongitudeSample1
			<< m_data->LatitudeSample1
			<< m_data->AltitudeSample1
			<< m_data->LongitudeSample2
			<< m_data->LatitudeSample2
			<< m_data->AltitudeSample2
			<< m_data->RoadWidth
			<< m_data->Probability
			<< m_data->SuppressPlatform
			<< m_data->PlacementCount
			<< m_data->PointCount;

		for (const auto& placement : m_placements.read())
		{
			out << placement;
		}

		for (const auto& point : m_points.read())
		{
			out << point.Longitude << point.Latitude << point.Altitude;
		}
	}
}

auto flightsimlib::io::CBglExtrusionBridge::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglExtrusionBridge::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglExtrusionBridgeData) + 
		sizeof(_GUID) * m_data->PlacementCount +
		sizeof(SBglVertexLLA) * m_data->PointCount);
}

auto flightsimlib::io::CBglExtrusionBridge::GetExtrusionProfile() const -> _GUID
{
	return m_data->ExtrusionProfile;
}

auto flightsimlib::io::CBglExtrusionBridge::SetExtrusionProfile(_GUID value) -> void
{
	m_data.write().ExtrusionProfile = value;
}

auto flightsimlib::io::CBglExtrusionBridge::GetMaterialSet() const -> _GUID
{
	return m_data->MaterialSet;
}

void flightsimlib::io::CBglExtrusionBridge::SetMaterialSet(_GUID value)
{
	m_data.write().MaterialSet = value;
}

auto flightsimlib::io::CBglExtrusionBridge::GetLongitudeSample1() const -> double
{
	return Longitude::Value(m_data->LongitudeSample1);
}

void flightsimlib::io::CBglExtrusionBridge::SetLongitudeSample1(double value)
{
	m_data.write().LongitudeSample1 = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglExtrusionBridge::GetLatitudeSample1() const -> double
{
	return Latitude::Value(m_data->LatitudeSample1);
}

auto flightsimlib::io::CBglExtrusionBridge::SetLatitudeSample1(double value) -> void
{
	m_data.write().LatitudeSample1 = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglExtrusionBridge::GetAltitudeSample1() const -> double
{
	return PackedAltitude::Value(m_data->AltitudeSample1);
}

auto flightsimlib::io::CBglExtrusionBridge::SetAltitudeSample1(double value) -> void
{
	m_data.write().AltitudeSample1 = PackedAltitude::FromDouble(value);
}

auto flightsimlib::io::CBglExtrusionBridge::GetLongitudeSample2() const -> double
{
	return Longitude::Value(m_data->LongitudeSample2);
}

auto flightsimlib::io::CBglExtrusionBridge::SetLongitudeSample2(double value) -> void
{
	m_data.write().LongitudeSample2 = Longitude::ToPacked(value);
}

auto flightsimlib::io::CBglExtrusionBridge::GetLatitudeSample2() const -> double
{
	return Latitude::Value(m_data->LatitudeSample2);
}

auto flightsimlib::io::CBglExtrusionBridge::SetLatitudeSample2(double value) -> void
{
	m_data.write().LatitudeSample2 = Latitude::ToPacked(value);
}

auto flightsimlib::io::CBglExtrusionBridge::GetAltitudeSample2() const -> double
{
	return PackedAltitude::Value(m_data->AltitudeSample2);
}

auto flightsimlib::io::CBglExtrusionBridge::SetAltitudeSample2(double value) -> void
{
	m_data.write().AltitudeSample2 = PackedAltitude::FromDouble(value);
}

auto flightsimlib::io::CBglExtrusionBridge::GetRoadWidth() const -> float
{
	return m_data->RoadWidth;
}

auto flightsimlib::io::CBglExtrusionBridge::SetRoadWidth(float value) -> void
{
	m_data.write().RoadWidth = value;
}

auto flightsimlib::io::CBglExtrusionBridge::GetProbability() const -> float
{
	return m_data->Probability;
}

auto flightsimlib::io::CBglExtrusionBridge::SetProbability(float value) -> void
{
	m_data.write().Probability = value;
}

auto flightsimlib::io::CBglExtrusionBridge::IsSuppressPlatform() const -> bool
{
	return static_cast<bool>(m_data->SuppressPlatform);
}

auto flightsimlib::io::CBglExtrusionBridge::SetSuppressPlatform(bool value) -> void
{
	m_data.write().SuppressPlatform = static_cast<uint8_t>(value);
}

auto flightsimlib::io::CBglExtrusionBridge::GetPlacementCount() const -> int
{
	return static_cast<int>(m_data->PlacementCount);
}

auto flightsimlib::io::CBglExtrusionBridge::GetPointCount() const -> int
{
	return static_cast<int>(m_data->PointCount);
}

auto flightsimlib::io::CBglExtrusionBridge::GetPlacementAt(int index) ->  _GUID*
{
	return &(m_placements.write()[index]);
}

auto flightsimlib::io::CBglExtrusionBridge::AddPlacement(const _GUID* placement) -> void
{
	// TODO Need validation, self check
	m_placements.write().emplace_back(*placement);
	++m_data.write().PlacementCount;
}

auto flightsimlib::io::CBglExtrusionBridge::RemovePlacement(const _GUID* placement) -> void
{
	const auto iter = m_placements.read().begin() +
		std::distance(m_placements.read().data(), placement);
	m_placements.write().erase(iter);
	--m_data.write().PlacementCount;
}

auto flightsimlib::io::CBglExtrusionBridge::GetPointAt(int index) -> SBglVertexLLA*
{
	return &(m_points.write()[index]);
}

auto flightsimlib::io::CBglExtrusionBridge::AddPoint(const SBglVertexLLA* point) -> void
{
	m_points.write().emplace_back(*point);
	++m_data.write().PointCount;
}

auto flightsimlib::io::CBglExtrusionBridge::RemovePoint(const SBglVertexLLA* point) -> void
{
	const auto iter = m_points.read().begin() +
		std::distance(m_points.read().data(), point);
	m_points.write().erase(iter);
	--m_data.write().PointCount;
}


//******************************************************************************
// CBglModelData
//******************************************************************************  


auto flightsimlib::io::CBglModelData::ReadBinary(BinaryFileStream& in) -> void
{
	auto& data = m_data.write();
	in >> data.Name
		>> data.Offset
		>> data.Length;

	m_model.write().resize(m_data->Length);
	in.Read(m_model.write().data(), m_data->Length);
}

auto flightsimlib::io::CBglModelData::WriteBinary(BinaryFileStream& out) -> void
{
	out << m_data->Name
		<< m_data->Offset
		<< m_data->Length;

	// TODO - Writing Output model is not yet supported, as they are in a block!
	// Needs support in CBglTile::WriteBinary, which is going to take an interface refactor
}

auto flightsimlib::io::CBglModelData::Validate() -> bool
{
	return true;
}

auto flightsimlib::io::CBglModelData::CalculateSize() const -> int
{
	return static_cast<int>(sizeof(SBglModelData) + m_data->Length);
}

auto flightsimlib::io::CBglModelData::GetName() const -> _GUID
{
	return m_data->Name;
}

auto flightsimlib::io::CBglModelData::SetName(_GUID value) -> void
{
	m_data.write().Name = value;
}

auto flightsimlib::io::CBglModelData::GetData() const -> const uint8_t*
{
	return m_model->data();
}

auto flightsimlib::io::CBglModelData::SetData(const uint8_t* value, int length) -> void
{
	m_model.write().clear();
	m_model.write().insert(m_model->end(), value, value + length);
	m_data.write().Length = length;
}

auto flightsimlib::io::CBglModelData::GetLength() const -> int
{
	return static_cast<int>(m_data->Length);
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

std::unique_ptr<uint8_t[]> flightsimlib::io::CTerrainRasterQuad1::DecompressData(
	ERasterCompressionType compression_type,
	uint8_t p_compressed[],
	int compressed_size,
	int uncompressed_size) const
{
	auto p_uncompressed = std::make_unique<uint8_t[]>(uncompressed_size);
	const auto bpp = GetBpp();
	auto bit_depth = 0, num_channels = 0;
	if (!GetImageFormat(bit_depth, num_channels))
	{
		return nullptr;
	}

	auto intermediate_size = 0;
	std::unique_ptr<uint8_t[]> p_intermediate = nullptr;
	if (compression_type == ERasterCompressionType::DeltaLz1 || 
		compression_type == ERasterCompressionType::DeltaLz2 ||
		compression_type == ERasterCompressionType::BitPackLz1 ||
		compression_type == ERasterCompressionType::BitPackLz2)
	{
		intermediate_size = *reinterpret_cast<int*>(p_compressed);
		p_intermediate = std::make_unique<uint8_t[]>(intermediate_size);
	}

	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	auto bytes_read = 0;
	
	switch (compression_type)
	{
	case ERasterCompressionType::Delta:
		bytes_read = CBglDecompressor::DecompressDelta(
			p_uncompressed.get(), 
			uncompressed_size, 
			p_compressed);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::BitPack:
		bytes_read = CBglDecompressor::DecompressBitPack(
			p_uncompressed.get(), 
			uncompressed_size, 
			p_compressed, 
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
			p_uncompressed.get(), 
			uncompressed_size, 
			p_compressed, 
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		break;
		
	case ERasterCompressionType::Lz2:
		bytes_read = CBglDecompressor::DecompressLz2(
			p_uncompressed.get(), 
			uncompressed_size, 
			p_compressed, 
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
			p_compressed + sizeof(int), 
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressDelta(
			p_uncompressed.get(), 
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
			p_compressed + sizeof(int),
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressDelta(
			p_uncompressed.get(), 
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
			p_compressed + sizeof(int),
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressBitPack(
			p_uncompressed.get(), 
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
			p_compressed + sizeof(int),
			compressed_size);
		if (bytes_read != compressed_size)
		{
			return nullptr;
		}
		bytes_read = CBglDecompressor::DecompressBitPack(
			p_uncompressed.get(), 
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
			p_uncompressed.get(), 
			uncompressed_size, 
			p_compressed, 
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

	return p_uncompressed;
}

template class flightsimlib::io::CBglFuelAvailability<stlab::copy_on_write<flightsimlib::io::SBglTriggerRefuelRepairData>>;
template class flightsimlib::io::CBglFuelAvailability<stlab::copy_on_write<flightsimlib::io::SBglAirportData>>;
template class flightsimlib::io::CBglFuelAvailability<stlab::copy_on_write<flightsimlib::io::SBglAirportSummaryData>>;

template class flightsimlib::io::CBglLLA<stlab::copy_on_write<flightsimlib::io::SBglNdbData>>;
template class flightsimlib::io::CBglLLA<stlab::copy_on_write<flightsimlib::io::SBglRunwayData>>;
template class flightsimlib::io::CBglLLA<stlab::copy_on_write<flightsimlib::io::SBglStartData>>;
template class flightsimlib::io::CBglLLA<stlab::copy_on_write<flightsimlib::io::SBglHelipadData>>;
template class flightsimlib::io::CBglLLA<stlab::copy_on_write<flightsimlib::io::SBglAirportData>>;
template class flightsimlib::io::CBglLLA<stlab::copy_on_write<flightsimlib::io::SBglAirportSummaryData>>;