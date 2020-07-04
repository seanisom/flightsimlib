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


#include "Export.h"

#include "../external/stlab/copy_on_write.hpp"

#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>


namespace flightsimlib
{
	namespace io
	{
		enum class EBglLayerType
		{
			Unknown = -1,
			None = 0x0,
			Copyright = 0x1,
			Guid = 0x2,
			Airport = 0x3,
			IlsVor = 0x13,
			Ndb = 0x17,
			Marker = 0x18,
			Boundary = 0x20,
			Waypoint = 0x22,
			Geopol = 0x23,
			SceneryObject = 0x25,
			NameList = 0x27,
			VorIndex = 0x28,
			NdbIndex = 0x29,
			WaypointIndex = 0x2A,
			ModelData = 0x2B,
			AirportSummary = 0x2C,
			Exclusion = 0x2E,
			TimeZone = 0x2F,
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

		FLIGHTSIMLIB_EXPORTED bool IsTrq1BglLayer(EBglLayerType layer_type);
		FLIGHTSIMLIB_EXPORTED bool IsRcs1BglLayer(EBglLayerType layer_type);


		class BinaryFileStream
		{
		public:
			BinaryFileStream() = default;
			
			explicit BinaryFileStream(const std::wstring& filename) :
				m_stream(filename, std::fstream::out | std::fstream::in | std::fstream::binary) {}

			void Open(const std::wstring& filename, std::ios_base::openmode mode = 
				std::fstream::out | std::fstream::in | std::fstream::binary)
			{
				m_stream.open(filename, mode);
			}

			bool IsOpen() const
			{
				return m_stream.is_open();
			}

			void Close()
			{
				m_stream.close();
			}
			
			template <class T>
			BinaryFileStream& operator>> (T& val)
			{
				m_stream.read((char*)&val, sizeof(val));
				return *this;
			}

			template <class T>
			BinaryFileStream& operator<< (T& val)
			{
				m_stream.write((char*)&val, sizeof(val));
				return *this;
			}

			bool operator!() const
			{
				return !m_stream;
			}

			explicit operator bool() const
			{
				return m_stream.operator bool();
			}

			int GetPosition()
			{
				return m_stream.tellg();
			}

			void SetPosition(int pos, std::fstream::_Seekdir dir = std::fstream::beg)
			{
				m_stream.seekg(pos, dir);
				m_stream.seekp(pos, dir);
			}

			std::fstream m_stream;
		};

		
		class CPackedQmid
		{
		public:
			
			explicit CPackedQmid(uint64_t value) : m_value(value) { }

			explicit CPackedQmid(uint32_t a, uint32_t b) : m_value(static_cast<uint64_t>(b) << 32 | a) { }
			
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

		struct SBglAirportTileData
		{
			uint16_t Type;
			uint32_t Length;
			uint8_t RunwayCount;
			uint8_t FrequencyCount;
			uint8_t StartCount;
			uint8_t ApproachCount;
			uint8_t ApronCount;
			uint8_t HelipadCount;
			uint32_t ReferenceLat;
			uint32_t ReferenceLon;
			uint32_t ReferenceAlt;
			uint32_t TowerLat;
			uint32_t TowerLon;
			uint32_t TowerAlt;
			float MagVar;
			uint32_t Icao;
			uint32_t RegionIdent;
			uint32_t FuelTypes;
			uint32_t Flags;

			static SBglAirportTileData ReadBinary(BinaryFileStream& in)
			{
				auto data = SBglAirportTileData{};
				in >> data.Type
				   >> data.Length
				   >> data.RunwayCount
				   >> data.FrequencyCount
				   >> data.StartCount
				   >> data.ApproachCount
				   >> data.ApronCount
				   >> data.HelipadCount
				   >> data.ReferenceLat
				   >> data.ReferenceLon
				   >> data.ReferenceAlt
				   >> data.TowerLat
				   >> data.TowerLon
				   >> data.TowerAlt
				   >> data.MagVar
				   >> data.Icao
				   >> data.RegionIdent
				   >> data.FuelTypes
				   >> data.Flags;
				return data;
			}

			static void WriteBinary(BinaryFileStream& out, const SBglAirportTileData& data)
			{
				out << data.Type
				    << data.Length
				    << data.RunwayCount
				    << data.FrequencyCount
				    << data.StartCount
				    << data.ApproachCount
				    << data.ApronCount
				    << data.HelipadCount
				    << data.ReferenceLat
				    << data.ReferenceLon
				    << data.ReferenceAlt
				    << data.TowerLat
				    << data.TowerLon
				    << data.TowerAlt
				    << data.MagVar
				    << data.Icao
				    << data.RegionIdent
				    << data.FuelTypes
				    << data.Flags;
			}
		};

		class IBglTile
		{
		public:
			IBglTile(const SBglTilePointer& tile_pointer) : m_tile_pointer(tile_pointer) { }
			virtual ~IBglTile() = default;
			virtual bool ReadBinary(BinaryFileStream& in) = 0;
			virtual bool WriteBinary(BinaryFileStream& out) = 0;
			virtual int CalculateDataSize() = 0;
			virtual int GetRecordCount() const = 0;
			virtual int ReadInt() = 0; //test data
			virtual void SetInt(int val) = 0; //test data
			virtual std::unique_ptr<IBglTile> Clone() const = 0;
			virtual void PrintValue() const = 0;
			virtual EBglLayerType Type() = 0;

			const SBglTilePointer& Pointer() const
			{
				return m_tile_pointer.read();
			}

			// TODO - currently we just support moving offset within a file
			// This needs to be extended to allow  clones, merges, etc
			bool UpdateTilePointer(BinaryFileStream& out, const CPackedQmid& qmid)
			{
				m_tile_pointer.write().StreamOffset = out.GetPosition();
				m_tile_pointer.write().SizeBytes = CalculateDataSize();
				m_tile_pointer.write().RecordCount = GetRecordCount();
				m_tile_pointer.write().QmidLow = qmid.Low();
				m_tile_pointer.write().QmidHigh = qmid.High();
				return out ? true : false;
			}

		private:
			stlab::copy_on_write<SBglTilePointer> m_tile_pointer;
		};

		
		class CBglAirportTile : public IBglTile
		{
		public:
			CBglAirportTile() : IBglTile(SBglTilePointer{}), m_value(0) { }

			explicit CBglAirportTile(int value) : IBglTile(SBglTilePointer{}), m_value(value) { }

			explicit CBglAirportTile(const SBglTilePointer& pointer) : IBglTile(pointer), m_value(0) { }
			
			int ReadInt() override
			{
				return m_value;
			};
			
			void SetInt(int value) override
			{
				m_value = value;
			}
			
			std::unique_ptr<IBglTile> Clone() const override
			{
				return std::make_unique<CBglAirportTile>(m_value);
			}
			
			void PrintValue() const override
			{
				std::cout << m_value << std::endl;
			}
			
			EBglLayerType Type() override
			{
				return EBglLayerType::Airport;
			}

			bool ReadBinary(BinaryFileStream& in) override
			{
				const auto count = static_cast<int>(Pointer().RecordCount);
				if (count <= 0)
				{
					return false;
				}

				in.SetPosition(Pointer().StreamOffset);
				if(!in)
				{
					return false;
				}
				
				m_data.reserve(count);
				
				for (auto i = 0; i < count; ++i)
				{
					const auto pos = in.GetPosition();
					auto record = SBglAirportTileData::ReadBinary(in);
					if (!in || record.Type != 0x3C) // TODO Constant
					{
						return false;
					}
					m_data.emplace_back(record);
					in.SetPosition(pos + static_cast<int>(record.Length));
					if (!in)
					{
						return false;
					}
				}

				return true;
			}

			// Invariant - UpdateTilePointer shall have already been called
			bool WriteBinary(BinaryFileStream& out) override
			{
				for (const auto& data : m_data)
				{
					SBglAirportTileData::WriteBinary(out, data);
					if (!out)
					{
						return false;
					}
				}
				return true;
			}

			int CalculateDataSize() override
			{
				// TODO - this should be dynamic, based on children
				return static_cast<int>(sizeof(SBglAirportTileData)) * GetRecordCount();
			}

			int GetRecordCount() const override
			{
				return m_data.size();
			}
			
		private:
			int m_value;
			std::vector<stlab::copy_on_write<SBglAirportTileData>> m_data;
		};


		struct SBglRunway
		{
			uint32_t Id;
		};
		
		class CBglRunwayTile final : public IBglTile
		{
		public:
			CBglRunwayTile() : IBglTile(SBglTilePointer{}), m_value(0) { }
			
			explicit CBglRunwayTile(int value) : IBglTile(SBglTilePointer{}), m_value(value)
			{
				SetInt(value);
			}
			
			CBglRunwayTile(const CBglRunwayTile& other):
				IBglTile(SBglTilePointer{}),
				m_value(other.m_value),
				m_data(other.m_data) { }
			
			int ReadInt() override
			{
				return m_value;
			};
			
			void SetInt(int value) override
			{
				m_value = value;
				m_data.write().Id = value * 3;
			}
			
			std::unique_ptr<IBglTile> Clone() const override
			{
				return std::make_unique<CBglRunwayTile>(*this);
			}
			
			void PrintValue() const override
			{
				std::cout << m_value << " : " << m_data->Id << std::endl;
				std::cout << "Is Data Unique? " << m_data.unique() << std::endl;
			}
			
			EBglLayerType Type() override
			{
				return EBglLayerType::IcaoRunway;
			}

			bool ReadBinary(BinaryFileStream& in) override
			{
				return false;
			}

			bool WriteBinary(BinaryFileStream& out) override
			{
				return false;
			}

			int CalculateDataSize() override
			{
				return sizeof(SBglRunway);
			}

			int GetRecordCount() const override
			{
				return 1;
			}
			
		private:
			int m_value;
			stlab::copy_on_write<SBglRunway> m_data;
		};


		class CBglLayer
		{
		public:
			virtual ~CBglLayer() = default;

			explicit CBglLayer(EBglLayerType type, const SBglLayerPointer& data) : m_type(type), m_data(data) { }

			explicit CBglLayer(CPackedQmid qmid, std::unique_ptr<IBglTile> tile) : m_type(tile->Type())
			{
				AddData(qmid, std::move(tile));
			}

			CBglLayer(const CBglLayer& other) : m_type(other.m_type)
			{
				for (const auto& tile : other.m_tiles)
				{
					m_tiles[tile.first] = tile.second->Clone();
				}
			}

			bool AddData(CPackedQmid qmid, std::unique_ptr<IBglTile>&& tile)
			{
				if(tile->Type() != Type())
				{
					return false;
				}
				m_tiles[qmid] = std::move(tile);
				return true;
			}

			IBglTile& operator[](const CPackedQmid index)
			{
				return *m_tiles[index];
			}

			void Print() const 
			{
				for (const auto& tile : m_tiles)
				{
					tile.second->PrintValue();
				}
			}

			// Factory function
			static std::unique_ptr<CBglLayer> ReadBinary(BinaryFileStream& in)
			{
				const auto layer_pointer = SBglLayerPointer::ReadBinary(in);
				if (!in)
				{
					return nullptr;
				}
				const auto next_position = in.GetPosition();
				in.SetPosition(layer_pointer.StreamOffset);
				if (!in)
				{
					return nullptr;
				}
				
				// TODO - This needs to become a factory method
				// ReSharper disable once CppInitializedValueIsAlwaysRewritten
				auto layer_type = EBglLayerType::None;
				switch(layer_pointer.Type)
				{
				default:
					layer_type = layer_pointer.Type;
					break;
				}
				auto layer = std::make_unique<CBglLayer>(layer_type, layer_pointer);

				const auto count = static_cast<int>(layer_pointer.TileCount);
				std::vector<SBglTilePointer> tile_pointers(count);
				for (auto i = 0; i < count; ++i)
				{
					tile_pointers[i] = SBglTilePointer::ReadBinary(in, layer_pointer);
					if (!in)
					{
						return nullptr;
					}
				}
				if (in.GetPosition() != layer_pointer.StreamOffset + layer_pointer.SizeBytes)
				{
					return nullptr;
				}
				for (const auto& tile_pointer : tile_pointers)
				{
					std::unique_ptr<IBglTile> tile;
					switch (layer_type)
					{
					case EBglLayerType::Airport:
						tile = std::make_unique<CBglAirportTile>(tile_pointer);
						break;
					case EBglLayerType::IcaoRunway:
						tile = std::make_unique<CBglRunwayTile>();
					default:
						continue;
					}
					if (!tile->ReadBinary(in))
					{
						return nullptr;
					}
					layer->AddData(
						CPackedQmid(tile_pointer.QmidLow, tile_pointer.QmidHigh),
						std::move(tile));
				}
				
				in.SetPosition(next_position);
				if (!in)
				{
					return nullptr;
				}
				return layer;
			}

			static constexpr int CalculateLayerPointerSize()
			{
				return 20;
			}
			
			int CalculateLayerSize() const
			{
				auto data_size = 0;
				for (const auto& tile : m_tiles)
				{
					data_size += tile.second->CalculateDataSize();
				}
				return data_size;
			}

			int CalculateTilePointersSize() const
			{
				auto tile_pointer_size = 16;
				for (const auto& tile : m_tiles)
				{
					if (tile.second->Pointer().QmidHigh > 0)
					{
						tile_pointer_size = 20;
						break;
					}
				}
				return static_cast<int>(m_data->TileCount) * tile_pointer_size;
			}
			
			void UpdateLayerPointer(int offset_to_tile)
			{
				const auto tile_pointers_size = CalculateTilePointersSize();
				m_data.write().Type = m_type;
				m_data.write().TileCount = m_tiles.size();
				m_data.write().HasQmidLow = 1;
				if (tile_pointers_size / static_cast<int>(m_data->TileCount) == 20)
				{
					m_data.write().HasQmidHigh = 1;
				}
				else
				{
					m_data.write().HasQmidHigh = 0;
				}
				m_data.write().SizeBytes =  tile_pointers_size;
				m_data.write().StreamOffset = offset_to_tile;
			}
			
			bool WriteBinaryLayerPointer(BinaryFileStream& out, int offset_to_tile)
			{
				UpdateLayerPointer(offset_to_tile);
				SBglLayerPointer::WriteBinary(out, m_data);
				return true;
			};

			bool WriteBinaryLayerTiles(BinaryFileStream& out)
			{
				for (const auto& tile : m_tiles)
				{
					if (!tile.second->UpdateTilePointer(out, tile.first))
					{
						return false;
					}
					if (!tile.second->WriteBinary(out))
					{
						return false;
					}
				}
				return true;
			}
			
			bool WriteBinaryTilePointers(BinaryFileStream& out)
			{
				for (const auto& tile : m_tiles)
				{
					SBglTilePointer::WriteBinary(out, tile.second->Pointer(), m_data.read());
					if (!out)
					{
						return false;
					}
				}
				return true;
			}
			
			EBglLayerType Type() const
			{
				return m_type;
			}

		private:
			EBglLayerType m_type;
			std::map<CPackedQmid, std::unique_ptr<IBglTile>> m_tiles;
			stlab::copy_on_write<SBglLayerPointer> m_data;
		};

		
		struct SBglHeader
		{
			uint16_t Version;
			uint16_t Magic;
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
				   >> header.Magic
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
				    << header.Magic
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
		
		class CBglFile final
		{
		public:
			CBglFile() : CBglFile(L"") {}
			
			explicit CBglFile(std::wstring file_name) :
				m_file_name(std::move(file_name)),
				m_file_size(0),
				m_header(),
				m_dirty(false){ }

			~CBglFile() = default;
			
			// TODO - This is now move-only, not copyable
			// With the bgl document model, a copy should be deep and copy-on-write
			// to provide value semantics. It remains to be seen what we do with
			// the backing stream in this case.
			CBglFile(const CBglFile&) = delete;
			CBglFile& operator= (const CBglFile&) = delete;
			CBglFile(CBglFile&&) = default;
			CBglFile& operator=(CBglFile&&) = default;
			
			bool Open()
			{
				if (m_stream.IsOpen())
				{
					return true;
				}
				m_stream.Open(m_file_name);
				if (!m_stream)
				{
					return false;
				}
				m_stream.SetPosition(0, std::fstream::end);
				m_file_size = m_stream.GetPosition();
				m_stream.SetPosition(0, std::fstream::beg);
				return true;
			}

			bool Close()
			{
				m_stream.Close();
				return m_stream ? true : false;
			}
			
			bool Write()
			{
				if (m_stream.IsOpen())
				{
					m_stream.Close();
				}
				m_stream.Open(m_file_name,
					std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc);
				if (!m_stream)
				{
					return false;
				}
				if (!WriteHeader())
				{
					return false;
				}
				if (!WriteAllLayers())
				{
					return false;
				}
				m_dirty = false;
				return true;
			}

			bool Read()
			{
				if (!m_layers.empty())
				{
					return false;
				}
				if (!Open())
				{
					return false;
				}
				if (!ReadHeader())
				{
					return false;
				}
				return ReadAllLayers();
			}

			const std::wstring& GetFileName() const
			{
				return m_file_name;
			}

			void Rename(std::wstring file_name)
			{
				m_file_name = std::move(file_name);
				m_dirty = true;
			}
			
			// TODO - This currently fails if the layer exists
			// This needs to be expanded to a data-wise merge,
			// and only fail if the actual data conflicts
			bool TryMergeLayer(std::unique_ptr<CBglLayer>&& layer)
			{
				const auto it = m_layers.find(layer->Type());
				if (it == m_layers.end())
				{
					m_layers[layer->Type()] = std::move(layer);
					m_dirty = true;
					return true;
				}
				return false;
			}
			
			std::unique_ptr<CBglLayer> RemoveLayer(EBglLayerType type)
			{
				const auto it = m_layers.find(type);
				if (it == m_layers.end())
				{
					return nullptr;
				}
				auto result = std::move(it->second);
				m_layers.erase(it);
				m_dirty = true;
				return result;
			}

			bool IsDirty() const
			{
				return m_dirty;
			}
			
			int GetFileSize() const
			{
				return m_file_size;
			}
			
		private:

			bool ReadAllLayers()
			{
				if (m_stream.GetPosition() != static_cast<int>(m_header.HeaderSize))
				{
					m_stream.SetPosition(m_header.HeaderSize);
					if (!m_stream)
					{
						return false;
					}
				}
				const auto count = static_cast<int>(m_header.LayerCount);
				for (auto i = 0; i < count; ++i)
				{
					auto layer = CBglLayer::ReadBinary(m_stream);
					if (layer == nullptr)
					{
						return false;
					}
					m_layers[layer->Type()] = std::move(layer);
				}
				return true;
			}


			// File order for resample is currently all layers after header (required),
			// then all data, then all QMID pointers to tile data
			// Although this may just be a compiler constraint. bglcomp for example looks different
			bool WriteAllLayers()
			{
				if (m_layers.size() != m_header.LayerCount ||
					m_stream.GetPosition() != m_header.HeaderSize)
				{
					return false;
				}
				auto data_size = 0x38; // TODO - header data size
				data_size += CBglLayer::CalculateLayerPointerSize() * m_header.LayerCount;
				for (const auto& layer : m_layers)
				{
					data_size += layer.second->CalculateLayerSize();
				}
				for (const auto& layer : m_layers)
				{
					if (!layer.second->WriteBinaryLayerPointer(m_stream, data_size) && !m_stream)
					{
						return false;
					}
					data_size += layer.second->CalculateLayerPointerSize();
				}
				for (const auto& layer : m_layers)
				{
					if (!layer.second->WriteBinaryLayerTiles(m_stream) || !m_stream)
					{
						return false;
					}
				}
				for (const auto& layer : m_layers)
				{
					if (!layer.second->WriteBinaryTilePointers(m_stream) && !m_stream)
					{
						return false;
					}
				}
				return true;
			}

			
			bool ReadHeader()
			{
				if (!m_stream)
				{
					return false;
				}
				m_stream.SetPosition(0, std::fstream::beg);
				m_header = SBglHeader::ReadBinary(m_stream);
				if (!m_stream)
				{
					return false;
				}
				if (m_header.Version != 0x0201)
				{
					return false;
				}
				if (m_header.Magic != 0x1992)
				{
					return false;
				}
				if (m_header.HeaderSize != 0x38)
				{
					return false;
				}
				if (m_header.QmidMagic != 0x08051803)
				{
					return false;
				}
				const auto count = static_cast<int>(m_header.LayerCount);
				if (count < 0 || count > std::numeric_limits<int32_t>::max())
				{
					return false;
				}
				return true;
			}

			bool WriteHeader()
			{
				if (!m_stream)
				{
					return false;
				}
				if (m_dirty)
				{
					BuildHeader();
				}
				m_stream.SetPosition(0, std::fstream::beg);
				// TODO
				SBglHeader::WriteBinary(m_stream, m_header);
				return false;
			}

			bool BuildHeader()
			{
				m_header.Version = 0x0201;
				m_header.HeaderSize = 0x38;
				m_header.FileTime = 0; // TODO FILETIME library
				m_header.QmidMagic = 0x08051803;
				m_header.LayerCount = m_layers.size();
				return ComputeHeaderQmids();
			}
			
			bool ComputeHeaderQmids() // TODO Build Qmid Algorithm
			{
				return false;
			}
			

		private:
			std::wstring m_file_name;
			int m_file_size;
			SBglHeader m_header;
			bool m_dirty;
			std::map<EBglLayerType, std::unique_ptr<CBglLayer>> m_layers;
			BinaryFileStream m_stream;
			
		};
	}

}


#endif