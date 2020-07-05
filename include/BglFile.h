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


		// TODO - fstream leaky abstractions!
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
				m_stream.read(reinterpret_cast<char*>(&val), sizeof(val));
				return *this;
			}

			template <class T>
			BinaryFileStream& operator<< (T& val)
			{
				m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
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
		
		class IBglTile
		{
		public:
			IBglTile(const SBglTilePointer& tile_pointer) : m_tile_pointer(tile_pointer) { }
			virtual ~IBglTile() = default;
			virtual bool ReadBinary(BinaryFileStream& in) = 0;
			virtual bool WriteBinary(BinaryFileStream& out) = 0;
			virtual int CalculateDataSize() const = 0;
			virtual int GetRecordCount() const = 0;
			virtual std::unique_ptr<IBglTile> Clone() const = 0;
			virtual EBglLayerType Type() const = 0;

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


		class IBglData
		{
		public:
			virtual ~IBglData() = default;
			virtual void ReadBinary(BinaryFileStream& in) = 0;
			virtual void WriteBinary(BinaryFileStream& out) = 0;
			virtual bool Validate() const = 0;
			virtual int CalculateSize() const = 0;
		};

#pragma pack(push)
#pragma pack(1)
		struct SBglAirportData
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
		};
#pragma pack(pop)

		class CBglAirportData final : public IBglData
		{
		public:
			void ReadBinary(BinaryFileStream& in) override
			{
				in >> m_data.write().Type
					>> m_data.write().Length
					>> m_data.write().RunwayCount
					>> m_data.write().FrequencyCount
					>> m_data.write().StartCount
					>> m_data.write().ApproachCount
					>> m_data.write().ApronCount
					>> m_data.write().HelipadCount
					>> m_data.write().ReferenceLat
					>> m_data.write().ReferenceLon
					>> m_data.write().ReferenceAlt
					>> m_data.write().TowerLat
					>> m_data.write().TowerLon
					>> m_data.write().TowerAlt
					>> m_data.write().MagVar
					>> m_data.write().Icao
					>> m_data.write().RegionIdent
					>> m_data.write().FuelTypes
					>> m_data.write().Flags;
			}

			void WriteBinary(BinaryFileStream& out) override
			{
				m_data.write().Length = sizeof(SBglAirportData);
				out << m_data->Type
					<< m_data->Length
					<< m_data->RunwayCount
					<< m_data->FrequencyCount
					<< m_data->StartCount
					<< m_data->ApproachCount
					<< m_data->ApronCount
					<< m_data->HelipadCount
					<< m_data->ReferenceLat
					<< m_data->ReferenceLon
					<< m_data->ReferenceAlt
					<< m_data->TowerLat
					<< m_data->TowerLon
					<< m_data->TowerAlt
					<< m_data->MagVar
					<< m_data->Icao
					<< m_data->RegionIdent
					<< m_data->FuelTypes
					<< m_data->Flags;
			}

			bool Validate() const override
			{
				return m_data->Type == 0x3C;
			}

			int CalculateSize() const override
			{
				return m_data->Length;
			}

		private:
			stlab::copy_on_write<SBglAirportData> m_data;
		};

		
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

		class CBglExclusionData final : public IBglData
		{
		public:
			void ReadBinary(BinaryFileStream& in) override
			{
				in >> m_data.write().Type
					>> m_data.write().Size
					>> m_data.write().LonWest
					>> m_data.write().LatNorth
					>> m_data.write().LonEast
					>> m_data.write().LatSouth;
			}

			void WriteBinary(BinaryFileStream& out) override
			{
				out << m_data->Type
					<< m_data->Size
					<< m_data->LonWest
					<< m_data->LatNorth
					<< m_data->LonEast
					<< m_data->LatSouth;
			}

			bool IsGenericBuilding() const
			{
				return m_data->Type & GenericBuilding;
			}

			void SetGenericBuilding(bool value)
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

			bool Validate() const override
			{
				return true;
			}

			int CalculateSize() const override
			{
				return sizeof(SBglExclusionData);
			}

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
		
		
		class CBglTile final : public IBglTile
		{
		public:
			explicit CBglTile(EBglLayerType type, const SBglTilePointer& pointer) :
				 IBglTile(pointer), m_type(type)  { }

			//CBglTile(const CBglTile& other) = default;
			
			std::unique_ptr<IBglTile> Clone() const override
			{
				//return std::make_unique<CBglTile>(*this);
				// TODO copying?
				return nullptr;
			}
			
			EBglLayerType Type() const override
			{
				return m_type;
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
					std::unique_ptr<IBglData> record;
					switch (m_type)
					{
					case EBglLayerType::Airport:
						record = std::make_unique<CBglAirportData>();
						break;
					case EBglLayerType::Exclusion:
						record = std::make_unique<CBglExclusionData>();
						break;
					default:
						continue;
					}
					record->ReadBinary(in);
					if (!record->Validate())
					{
						return false;
					}
					const auto size = record->CalculateSize();
					m_data.push_back(std::move(record));
					in.SetPosition(pos + static_cast<int>(size));
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
				for (auto& data : m_data)
				{
					data->WriteBinary(out);
					if (!out)
					{
						return false;
					}
				}
				return true;
			}

			int CalculateDataSize() const override
			{
				return static_cast<int>(sizeof(SBglAirportData)) * GetRecordCount();
			}

			int GetRecordCount() const override
			{
				return m_data.size();
			}
			
		private:
			std::vector<std::unique_ptr<IBglData>> m_data;
			EBglLayerType m_type;
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
				if (in.GetPosition() != static_cast<int>(layer_pointer.StreamOffset + layer_pointer.SizeBytes))
				{
					return nullptr;
				}
				for (const auto& tile_pointer : tile_pointers)
				{
					std::unique_ptr<IBglTile> tile;
					tile = std::make_unique<CBglTile>(layer_type, tile_pointer);
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
				if (m_data->TileCount && tile_pointers_size / static_cast<int>(m_data->TileCount) == 20)
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

			FLIGHTSIMLIB_EXPORTED static bool IsTrq1BglLayer(EBglLayerType layer_type);
			FLIGHTSIMLIB_EXPORTED static bool IsRcs1BglLayer(EBglLayerType layer_type);

		private:
			EBglLayerType m_type;
			std::map<CPackedQmid, std::unique_ptr<IBglTile>> m_tiles;
			stlab::copy_on_write<SBglLayerPointer> m_data;
		};

		
		struct SBglHeader
		{
			uint16_t Version;
			uint16_t FileMagic;
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
				   >> header.FileMagic
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
				    << header.FileMagic
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
					m_stream.GetPosition() != static_cast<int>(m_header.HeaderSize))
				{
					return false;
				}
				auto data_size = HeaderSize();
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
					data_size += layer.second->CalculateTilePointersSize();
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
				if (m_header.Version != Version())
				{
					return false;
				}
				if (m_header.FileMagic != FileMagic())
				{
					return false;
				}
				if (m_header.HeaderSize != HeaderSize())
				{
					return false;
				}
				if (m_header.QmidMagic != QmidMagic())
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
				m_stream.SetPosition(0, std::fstream::beg);
				if (!m_stream)
				{
					return false;
				}
				if (m_dirty)
				{
					BuildHeader();
				}
				SBglHeader::WriteBinary(m_stream, m_header);
				return m_stream ? true : false;
			}

			bool BuildHeader()
			{
				m_header.Version = Version();
				m_header.FileMagic = FileMagic();
				m_header.HeaderSize = HeaderSize();
				m_header.FileTime = 0; // TODO FILETIME library
				m_header.QmidMagic = QmidMagic();
				m_header.LayerCount = m_layers.size();
				return ComputeHeaderQmids();
			}
			
			bool ComputeHeaderQmids() // TODO Build Qmid Algorithm
			{
				m_header.PackedQMIDParent0 = 0;
				m_header.PackedQMIDParent1 = 0;
				m_header.PackedQMIDParent2 = 0;
				m_header.PackedQMIDParent3 = 0;
				m_header.PackedQMIDParent4 = 0;
				m_header.PackedQMIDParent5 = 0;
				m_header.PackedQMIDParent6 = 0;
				m_header.PackedQMIDParent7 = 0;
				return true;
			}
			

		private:
			std::wstring m_file_name;
			int m_file_size;
			SBglHeader m_header;
			bool m_dirty;
			std::map<EBglLayerType, std::unique_ptr<CBglLayer>> m_layers;
			BinaryFileStream m_stream;

			static constexpr uint16_t Version()
			{
				return 0x0201; // FSX+
			}
			
			static constexpr uint32_t HeaderSize()
			{
				return sizeof(SBglHeader);
			}
			
			static constexpr uint16_t FileMagic()
			{
				return 0x1992;
			}
			
			static constexpr uint32_t QmidMagic()
			{
				return 0x08051803;
			}
			
		};
	}

}


#endif