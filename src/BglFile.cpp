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
// File:     BglFile.cpp
//
// Summary:  BGL File structure parsing and utilities
//
// Author:   Sean Isom
//
// TODO list:
// * Factories need to be refactored to standalone
// * Iteration, copy, and move semantics for Layers and Tiles
//    - Should allow seamless reversible edits and moves across files
// * Layers and Tiles should conform to IBglSerializable
// * Observable model
// * History Stack
//
//******************************************************************************       


#include "BglFile.h"
#include "BglData.h"


namespace flightsimlib
{

namespace io
{


//******************************************************************************
// CBglTile
//******************************************************************************  


CBglTile::CBglTile(EBglLayerType type, const SBglTilePointer& pointer):
	m_tile_pointer(pointer), m_type(type)
{
}

std::shared_ptr<CBglTile> CBglTile::Clone() const
{
	//return std::make_unique<CBglTile>(*this);
	// TODO copying?
	return nullptr;
}

const SBglTilePointer& CBglTile::Pointer() const
{
	return m_tile_pointer.read();
}

// TODO - currently we just support moving offset within a file
// This needs to be extended to allow  clones, merges, etc
bool CBglTile::UpdateTilePointer(BinaryFileStream& out, const CPackedQmid& qmid)
{
	m_tile_pointer.write().StreamOffset = out.GetPosition();
	m_tile_pointer.write().SizeBytes = CalculateDataSize();
	m_tile_pointer.write().RecordCount = GetRecordCount();
	m_tile_pointer.write().QmidLow = qmid.Low();
	m_tile_pointer.write().QmidHigh = qmid.High();
	return out ? true : false;
}

EBglLayerType CBglTile::Type() const
{
	return m_type;
}

bool CBglTile::ReadBinary(BinaryFileStream& in)
{
	const auto count = static_cast<int>(Pointer().RecordCount);
	if (count <= 0)
	{
		return false;
	}

	in.SetPosition(Pointer().StreamOffset);
	if (!in)
	{
		return false;
	}

	m_data.reserve(count);

	for (auto i = 0; i < count; ++i)
	{
		const auto pos = in.GetPosition();
		std::unique_ptr<IBglSerializable> record;
		switch (m_type)
		{
		case EBglLayerType::Airport:
			record = std::make_unique<CBglAirport>();
			break;
		case EBglLayerType::Exclusion:
			record = std::make_unique<CBglExclusion>();
			break;
		default:
			continue;
		}
		record->ReadBinary(in);
		const auto size = record->CalculateSize();
		if (!record->Validate())
		{
			return false;
		}
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
bool CBglTile::WriteBinary(BinaryFileStream& out)
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

int CBglTile::CalculateDataSize() const
{
	auto size = 0;
	for (const auto& data : m_data)
	{
		size += data->CalculateSize();
	}
	return size;
}

int CBglTile::GetRecordCount() const
{
	return static_cast<int>(m_data.size());
}


//******************************************************************************
// CBglLayer
//******************************************************************************  


CBglLayer::CBglLayer(EBglLayerType type, const SBglLayerPointer& data): m_type(type), m_data(data)
{
}

CBglLayer::CBglLayer(CPackedQmid qmid, std::shared_ptr<CBglTile> tile): m_type(tile->Type())
{
	AddTile(qmid, std::move(tile));
}

CBglLayer::CBglLayer(const CBglLayer& other): m_type(other.m_type)
{
	for (const auto& tile : other.m_tiles)
	{
		m_tiles[tile.first] = tile.second->Clone();
	}
}

bool CBglLayer::AddTile(CPackedQmid qmid, std::shared_ptr<CBglTile> tile)
{
	if (tile->Type() != Type())
	{
		return false;
	}
	m_tiles[qmid] = std::move(tile);
	return true;
}

CBglTile& CBglLayer::operator[](const CPackedQmid index)
{
	return *m_tiles[index];
}

// Factory function
std::unique_ptr<CBglLayer> CBglLayer::ReadBinary(
	BinaryFileStream& in,							 
	const std::map<EBglLayerType, std::unique_ptr<CBglLayer>>& layers)
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
	switch (layer_pointer.Type)
	{
	case EBglLayerType::TerrainPhoto32Jan:
		layer_type = EBglLayerType::Tacan;
		break;
	case EBglLayerType::TerrainPhoto32Feb:
		layer_type = EBglLayerType::TacanIndex;
		break;
	default:
		layer_type = layer_pointer.Type;
		break;
	}
	auto layer = std::make_unique<CBglLayer>(layer_type, layer_pointer);
	const auto count = static_cast<int>(layer_pointer.TileCount);

	auto existing = false;
	for (const auto& other_layer : layers)
	{
		if (other_layer.second->Offset() == static_cast<int>(layer_pointer.StreamOffset))
		{
			existing = true;
			for (const auto& tile : other_layer.second->Tiles())
			{
				const auto tile_pointer(tile.second);
				layer->AddTile(tile.first, tile_pointer);
			}
			break;
		}
	}

	if (existing == false)
	{
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
			auto tile = std::make_shared<CBglTile>(layer_type, tile_pointer);
			if (!tile->ReadBinary(in))
			{
				return nullptr;
			}
			layer->AddTile(
				CPackedQmid(tile_pointer.QmidLow, tile_pointer.QmidHigh),
				tile);
		}
	}

	in.SetPosition(next_position);
	if (!in)
	{
		return nullptr;
	}
	return layer;
}

int CBglLayer::CalculateLayerSize() const
{
	auto data_size = 0;
	for (const auto& tile : m_tiles)
	{
		data_size += tile.second->CalculateDataSize();
	}
	return data_size;
}

int CBglLayer::CalculateTilePointersSize() const
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

void CBglLayer::UpdateLayerPointer(int offset_to_tile)
{
	const auto tile_pointers_size = CalculateTilePointersSize();
	m_data.write().Type = m_type;
	m_data.write().TileCount = static_cast<uint32_t>(m_tiles.size());
	m_data.write().HasQmidLow = 1;
	if (m_data->TileCount && tile_pointers_size / static_cast<int>(m_data->TileCount) == 20)
	{
		m_data.write().HasQmidHigh = 1;
	}
	else
	{
		m_data.write().HasQmidHigh = 0;
	}
	m_data.write().SizeBytes = tile_pointers_size;
	m_data.write().StreamOffset = offset_to_tile;
}

bool CBglLayer::WriteBinaryLayerPointer(BinaryFileStream& out, int offset_to_tile)
{
	UpdateLayerPointer(offset_to_tile);
	SBglLayerPointer::WriteBinary(out, m_data);
	return true;
}

bool CBglLayer::WriteBinaryLayerTiles(BinaryFileStream& out)
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

bool CBglLayer::WriteBinaryTilePointers(BinaryFileStream& out)
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

int CBglLayer::Offset() const
{
	return static_cast<int>(m_data->StreamOffset);
}

EBglLayerType CBglLayer::Type() const
{
	return m_type;
}

const std::map<CPackedQmid, std::shared_ptr<CBglTile>>& CBglLayer::Tiles() const
{
	return m_tiles;
}

bool CBglLayer::IsTrq1BglLayer(EBglLayerType layer_type)
{
	if (layer_type == EBglLayerType::TerrainIndex ||
		layer_type == EBglLayerType::TerrainLandClass ||
		layer_type == EBglLayerType::TerrainWaterClass ||
		layer_type == EBglLayerType::TerrainRegion ||
		layer_type == EBglLayerType::PopulationDensity ||
		(layer_type >= EBglLayerType::TerrainSeasonJan && layer_type <= EBglLayerType::TerrainSeasonDec) ||
		(layer_type >= EBglLayerType::TerrainPhotoJan && layer_type <= EBglLayerType::TerrainPhotoNight) ||
		(layer_type >= EBglLayerType::TerrainPhoto32Jan && layer_type <= EBglLayerType::TerrainPhoto32Night))
	{
		return true;
	}
	return false;
}

bool CBglLayer::IsRcs1BglLayer(EBglLayerType layer_type)
{
	if (layer_type == EBglLayerType::TerrainElevation)
	{
		return true;
	}
	return false;
}


//******************************************************************************
// CBglFile
//******************************************************************************  


CBglFile::CBglFile(): CBglFile(L"")
{
}

CBglFile::CBglFile(std::wstring file_name):
	m_file_name(std::move(file_name)),
	m_file_size(0),
	m_header(),
	m_dirty(false)
{
}

bool CBglFile::Open()
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

bool CBglFile::Close()
{
	m_stream.Close();
	return m_stream ? true : false;
}

bool CBglFile::Write()
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

bool CBglFile::Read()
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

const std::wstring& CBglFile::GetFileName() const
{
	return m_file_name;
}

void CBglFile::Rename(std::wstring file_name)
{
	m_file_name = std::move(file_name);
	m_dirty = true;
}

// TODO - This currently fails if the layer exists
// This needs to be expanded to a data-wise merge,
// and only fail if the actual data conflicts
bool CBglFile::TryMergeLayer(std::unique_ptr<CBglLayer>&& layer)
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

std::unique_ptr<CBglLayer> CBglFile::RemoveLayer(EBglLayerType type)
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

bool CBglFile::IsDirty() const
{
	return m_dirty;
}

int CBglFile::GetFileSize() const
{
	return m_file_size;
}

// Test method!
std::vector<const IBglExclusion*> CBglFile::GetExclusions()
{
	return { nullptr };
}

bool CBglFile::ReadAllLayers()
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
		auto layer = CBglLayer::ReadBinary(m_stream, m_layers);
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
bool CBglFile::WriteAllLayers()
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

bool CBglFile::ReadHeader()
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

bool CBglFile::WriteHeader()
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

bool CBglFile::BuildHeader()
{
	m_header.Version = Version();
	m_header.FileMagic = FileMagic();
	m_header.HeaderSize = HeaderSize();
	m_header.FileTime = 0; // TODO FILETIME library
	m_header.QmidMagic = QmidMagic();
	m_header.LayerCount = static_cast<uint32_t>(m_layers.size());
	return ComputeHeaderQmids();
}

// TODO Build Qmid Algorithm
bool CBglFile::ComputeHeaderQmids()
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


} // namespace io

} // namespace flightsimlib