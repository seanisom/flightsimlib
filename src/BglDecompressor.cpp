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
// Summary:  Implementation of FSX BGL decompression routines through
//           a DLL interface.
//
//           This will be expanded to a more generic decompression interface
//           in future builds, with a full api and doxygen docs
//
// Author:   Sean Isom
//
//******************************************************************************                                        


#include "BglDecompressor.h"

#include "PTC/PTCLib.h"

#include <cstring>
#include <memory>


using namespace flightsimlib::io;


// ReSharper disable CppInitializedValueIsAlwaysRewritten


static void memset32(void* p_dest, unsigned int value, int count) 
{
	auto* p = static_cast<unsigned int*>(p_dest);
	for (auto i = 0; i < count; i++)
	{
		*p++ = value;
	}
}


int CBglDecompressor::s_shift_mask[] =
{
	 0x0,
	 0x1,
	 0x3,
	 0x7,
	 0xF,
	 0x1F,
	 0x3F,
	 0x7F,
	 0xFF,
	 0x01FF,
	 0x03FF,
	 0x07FF,
	 0x0FFF,
	 0x1FFF,
	 0x3FFF,
	 0x7FFF,
	 0xFFFF
};


struct CBglDecompressor::SBitPoolLz
{
	uint8_t chunk_value;
	int chunk_bits;
	const uint8_t* p_chunk;
	int pool_bytes;
};


struct CBglDecompressor::SBitPoolBp
{
	const uint8_t* p_chunk;
	int num_chunks;
	int chunk_bits;
	int total_bits;
};


int CBglDecompressor::DecompressLz1(
	uint8_t* p_uncompressed, 
	int uncompressed_size, 
	const uint8_t* p_compressed,
	int compressed_size)
{
	SBitPoolLz pool
	{
		0,
		0,
		(p_compressed + 4),
		compressed_size - 4
	};

	if (compressed_size <= 4 || *p_compressed != 68 || *(p_compressed + 1) != 83)
	{
		return -1;
	}

	auto* p_dest = p_uncompressed;
	while (p_dest - p_uncompressed < uncompressed_size)
	{
		auto sequence_offset = 0;
		const auto sequence_type = LzReadNextNBits(&pool, 2);
		if (sequence_type == 1)
		{
			*p_dest++ = LzReadNextNBits(&pool, 7) | 0x80;
			continue;
		}
		if (sequence_type == 2)
		{
			*p_dest++ = LzReadNextNBits(&pool, 7);
			continue;
		}
		if (sequence_type == 3)
		{
			if (LzReadNextBit(&pool))
			{
				sequence_offset = LzReadNextNBits(&pool, 12) + 0x140;
			}
			else
			{
				sequence_offset = LzReadNextNBits(&pool, 8) + 0x40;
			} 
		}
		else
		{
			sequence_offset = LzReadNextNBits(&pool, 6);
		}
		if (sequence_offset != 0x113F)
		{
			auto num_bits = 0;
			auto sequence_length = 0;
			while (!LzReadNextBit(&pool))
			{
				++num_bits;
			}
			if (num_bits > 0xF)
			{
				return 0;
			}
			if (num_bits)
			{
				sequence_length = (1 << num_bits) + LzReadNextNBits(&pool, num_bits) + 1;
			}
			else
			{
				sequence_length = 2;
			}
			auto* dict_ptr = p_dest - sequence_offset;
			while (sequence_length--)
			{
				*p_dest++ = *dict_ptr++;
			}
		}
	}
	return static_cast<int>(p_dest - p_uncompressed);
}


int CBglDecompressor::DecompressLz2(
	uint8_t* p_uncompressed, 
	int uncompressed_size, 
	const uint8_t* p_compressed, 
	int compressed_size)
{
	SBitPoolLz pool
	{
		0,
		0,
		(p_compressed + 4),
		compressed_size - 4
	};

	if (compressed_size <= 4 || *p_compressed != 74 || *(p_compressed + 1) != 77)
	{
		return -1;
	}

	auto* p_dest = p_uncompressed;
	while (p_dest - p_uncompressed < uncompressed_size)
	{
		auto sequence_offset = 0;
		if (!LzReadNextBit(&pool))
		{
			*p_dest++ = LzReadNextNBits(&pool, 7);
			continue;
		}
		if (LzReadNextBit(&pool))
		{
			*p_dest++ = LzReadNextNBits(&pool, 7) | 0x80;
			continue;
		}
		if (LzReadNextBit(&pool))
		{
			if (LzReadNextBit(&pool))
			{
				sequence_offset = LzReadNextNBits(&pool, 12) + 0x140;
			}
			else
			{
				sequence_offset = LzReadNextNBits(&pool, 8) + 0x40;
			}
		}
		else
		{
			sequence_offset = LzReadNextNBits(&pool, 6);
		}
		if (sequence_offset != 0x113F)
		{
			auto num_bits = 0;
			auto sequence_length = 0;
			while (!LzReadNextBit(&pool))
			{
				++num_bits;
			}
			if (num_bits > 0xF)
			{
				return 0;
			}
			if (num_bits)
			{
				sequence_length = (1 << num_bits) + LzReadNextNBits(&pool, num_bits) + 2;
			}
			else
			{
				sequence_length = 3;
			}
			auto* dict_ptr = p_dest - sequence_offset;
			while (sequence_length--)
			{
				*p_dest++ = *dict_ptr++;
			}
		}
	}
	return static_cast<int>(p_dest - p_uncompressed);
}


int CBglDecompressor::DecompressDelta(
	uint8_t* p_uncompressed, 
	int uncompressed_size, 
	const uint8_t* p_compressed)
{
	if (uncompressed_size & 1)
	{
		p_uncompressed[0] = p_compressed[0];
		++p_compressed;
		++p_uncompressed;
	}
	
	if (uncompressed_size == 0 || uncompressed_size == 1)
	{
		return uncompressed_size;
	}
	
	auto previous = *reinterpret_cast<const short*>(p_compressed);
	p_compressed += 2;
	auto* p_dest = reinterpret_cast<short*>(p_uncompressed);
	*p_dest++ = previous;
	const auto count = (uncompressed_size / 2) - 1;

	for (auto i = 0; i < count; ++i)
	{
		if (*p_compressed == 0x80) // -128
		{
			*p_dest = *reinterpret_cast<const short*>(p_compressed + 1);
			p_compressed += 3;
		}
		else if (*p_compressed == 0x81) // -127
		{
			*p_dest = previous - *(p_compressed + 1) - 126;
			p_compressed += 2;
		}
		else if (*p_compressed == 0x82) // -126
		{
			*p_dest = previous + *(p_compressed + 1) + 128;
			p_compressed += 2;
		}
		else
		{
			*p_dest = previous + *reinterpret_cast<const char*>(p_compressed);
			p_compressed += 1;
		}
		previous = *p_dest++;
	}

	return uncompressed_size;
}


int CBglDecompressor::DecompressBitPack(
	uint8_t* p_uncompressed, 
	int uncompressed_size, 
	const uint8_t* p_compressed, 
	int compressed_size, 
	int rows, 
	int cols)
{
	if (uncompressed_size == 2 * cols * rows)
	{
		return Bp16Decompress(
			p_uncompressed, 
			uncompressed_size, 
			p_compressed, 
			compressed_size, 
			rows, 
			cols);
	}
	if (uncompressed_size == cols * rows)
	{
		return Bp8Decompress(
			p_uncompressed, 
			uncompressed_size, 
			p_compressed, 
			compressed_size, 
			rows, 
			cols);
	}
	return 0;
}



int numChannelsForFormat(int format)
{
	switch (format)
	{
	case 1:
	case 3:
	case 5:
		return 3;
	case 2:
	case 4:
	case 6:
		return 4;
	case 7:
	case 8:
	case 9:
		return 1;
	default:
		return 0;
	}
}


struct PTCElevationHeader5
{
	unsigned char Version;
	unsigned char SkipRow  : 1;
	unsigned char SkipCol  : 1;
	unsigned char Reserved : 6;
	short Bias;
};


struct PTCImageHeader1
{
	unsigned int Version  : 8;
	unsigned int Reserved : 24;
};


struct PTCBlockHeader
{
	char Magic[4];
	uint8_t Version;
	uint8_t PixelFormat;
	uint16_t MipLevels;
	uint32_t Width;
	uint32_t Height;
	float    Scale;
	float    Bias;
	uint32_t Length; // Header + Data
} ;


struct PTCMipHeader
{
	uint32_t Offset;
	uint32_t Size;
	uint8_t  MipLevel;
	uint8_t  Reserved1;
	uint16_t Reserved2;
	uint32_t Reserved3;
};


// TODO - There's still a lot of code from PTCImage1 that needs ported
int CBglDecompressor::DecompressPtc(
	uint8_t* p_uncompressed, 
	int uncompressed_size, 
	const uint8_t* p_compressed, 
	int compressed_size, 
	int rows, 
	int cols, 
	int channels, 
	int bpp)
{
	auto length = bpp * 0x100;
	auto p_dest = std::make_unique<uint8_t[]>(static_cast<size_t>(bpp) * 0x100 * 0x100);

	auto format = 8; // U16
	if (channels == 4)
	{
		format = 4; // 1555
	}
	if (bpp == 4)
	{
		format = 2; // 8888
	}

	PTCElevationHeader5 elevation_header{};
	PTCImageHeader1 image_header{};

	if (format == 8)
	{
		memcpy(&elevation_header, p_compressed, sizeof(elevation_header));
		if (elevation_header.Version != 5U)
		{
			return -1;
		}
	}
	else
	{
		memcpy(&image_header, p_compressed, sizeof(image_header));
		if (image_header.Version != 1U)
		{
			return -1;
		}
	}
	
	compressed_size -= 4;
	p_compressed += 4;

	WriteRowParams decodeObjects;
	memset(&decodeObjects, 0, sizeof(WriteRowParams));
	DecodeParams params;
	memset(&params, 0, sizeof(DecodeParams));
	
	PTCBlockHeader header{};
	PTCMipHeader mip_header{};

	int numBytesRead = 0;

	memcpy(&header, p_compressed, 28);
	if(header.Magic[0] != 'P' || header.Magic[1] != 'T' || header.Magic[2] != 'C' || header.Magic[3] != '\0')
		return 14;

	const int num_channels = numChannelsForFormat(format);
	if (num_channels == 0 || format != static_cast<int>(header.PixelFormat))
		return 7;
	
	if (header.Height != 256 || header.Width != 256)
		return 11;

	params.LP2 = 4;
	params.LP4 = 7;
	params.HasSubregion = 0;

	memcpy(&mip_header, p_compressed + 28, 16);
	if (mip_header.Offset != 44)
		return 14;

	if (mip_header.MipLevel != 0)
		return 6;

	constexpr auto delta = sizeof(PTCBlockHeader) + sizeof(PTCMipHeader);
	p_compressed += delta;
	compressed_size -= delta;

	params.MipGenerate[0] = 1;
	params.HasMipmaps = 0;
	decodeObjects.Dest = p_dest.get();
	decodeObjects.Type = static_cast<PixelType>(format);
	decodeObjects.RowWidth = static_cast<int>(header.Width);
	decodeObjects.StrideBytes = length;
	if (decodeObjects.Type == PT32)
	{
		decodeObjects.PixelParams.Bias = header.Bias;
		decodeObjects.PixelParams.Scale = header.Scale;
	}
	else if (decodeObjects.Type == PT1555)
	{
		decodeObjects.PixelParams.NumBitsColor = 5;
		decodeObjects.PixelParams.NumBitsAlpha = 1;
	}
	else if (decodeObjects.Type == PT8888)
	{
		decodeObjects.PixelParams.NumBitsColor = 8;
		decodeObjects.PixelParams.NumBitsAlpha = 8;
	}
	else
	{
		decodeObjects.PixelParams.NumBitsColor = 16;
		decodeObjects.PixelParams.NumBitsAlpha = 0;
	}
	params.RowParams[0] = &decodeObjects;

	numBytesRead = PTCDecompress(&params, p_compressed, compressed_size);
	if (numBytesRead != compressed_size)
		return 6;

	//TODO Check this for TRQ2
	if (decodeObjects.Type == PT16)
	{
		auto idx_norm = 0;
		auto idx_denorm = cols * elevation_header.SkipRow * bpp;
		const auto bias = elevation_header.Bias;
		for (auto i = 0U; i < header.Height; i++)
		{
			if (elevation_header.SkipCol)
			{
				const auto var = *reinterpret_cast<short*>(p_dest.get() + idx_norm);
				*reinterpret_cast<short*>(p_uncompressed + idx_denorm) = static_cast<short>(var - bias);
				idx_denorm += bpp;
			}
			
			for (auto col = 0U; col < header.Width; col++)
			{
				const auto var = *reinterpret_cast<short*>(p_dest.get() + idx_norm);
				*reinterpret_cast<short*>(p_uncompressed + idx_denorm) = static_cast<short>(var - bias);
				idx_denorm += bpp;
				idx_norm += bpp;
			}
		}

		if (elevation_header.SkipRow)
		{
			memcpy(p_uncompressed, p_uncompressed + cols * bpp, static_cast<size_t>(cols)* bpp);
		}
	}
	else
	{
		memcpy(p_uncompressed, p_dest.get(), uncompressed_size);
	}

	return 0;
}


int CBglDecompressor::LzReadNextBit(
	SBitPoolLz* p_pool)
{
	if (!p_pool->pool_bytes)
	{
		return -1;
	}

	if (!p_pool->chunk_bits)
	{
		p_pool->chunk_value = *p_pool->p_chunk;
		p_pool->chunk_bits = 8;
		++p_pool->p_chunk;
		--p_pool->pool_bytes;
	}

	const auto result = p_pool->chunk_value & 1;
	--p_pool->chunk_bits;
	p_pool->chunk_value >>= 1;
	return result;
}


int CBglDecompressor::LzReadNextNBits(
	SBitPoolLz* p_pool, 
	int num_bits)
{
	if (!p_pool->pool_bytes)
	{
		return -1;
	}

	auto result = 0;
	if (num_bits <= p_pool->chunk_bits)
	{
		result = p_pool->chunk_value;
		p_pool->chunk_value >>= num_bits;
		p_pool->chunk_bits -= num_bits;
	}
	else
	{
		auto num_shift = 0;
		auto num_bits_remaining = num_bits;
		while (num_bits_remaining > 0)
		{
			if (!p_pool->chunk_bits)
			{
				p_pool->chunk_value = *p_pool->p_chunk;
				p_pool->chunk_bits = 8;
				++p_pool->p_chunk;
				--p_pool->pool_bytes;
			}

			result |= p_pool->chunk_value << num_shift;

			// Not enough bits, run again
			if (p_pool->chunk_bits < num_bits_remaining)
			{
				num_shift += p_pool->chunk_bits;
				num_bits_remaining -= p_pool->chunk_bits;
				p_pool->chunk_bits = 0;
			}
			else // sufficient bits, terminate
			{
				p_pool->chunk_value >>= num_bits_remaining;
				p_pool->chunk_bits -= num_bits_remaining;
				num_bits_remaining = 0;
			}
		}
	}

	return result & ((1 << num_bits) - 1);
}


bool CBglDecompressor::BpReadNextNBits(
	SBitPoolBp* p_pool, 
	int* o_dest, 
	int num_bits)
{
	if (p_pool->total_bits < num_bits)
	{
		return false;
	}
		
	if (num_bits == 0)
	{
		return true;
	}
	
	p_pool->total_bits -= num_bits;
	auto num_shift = 8 - p_pool->chunk_bits;
	
	if (p_pool->chunk_bits > num_bits)
	{
		*o_dest = s_shift_mask[num_bits] & (*p_pool->p_chunk >> num_shift);
		p_pool->chunk_bits -= num_bits;
		return true;
	}
	
	*o_dest = *p_pool->p_chunk >> num_shift;
	num_shift = p_pool->chunk_bits;
	auto num_bits_to_get = num_bits - num_shift;
	++p_pool->p_chunk;
	++p_pool->num_chunks;
	p_pool->chunk_bits = 8;

	if (num_bits == num_shift)
	{
		return true;
	}
	while (num_bits_to_get > 0)
	{
		if (num_bits_to_get < 8)
		{
			*o_dest |= (*p_pool->p_chunk & s_shift_mask[num_bits_to_get]) << num_shift;
			p_pool->chunk_bits -= num_bits_to_get;
			return true;
		}
		
		*o_dest |= *p_pool->p_chunk << num_shift;
		num_shift += p_pool->chunk_bits;
		num_bits_to_get -= p_pool->chunk_bits;
		++p_pool->p_chunk;
		++p_pool->num_chunks;
		p_pool->chunk_bits = 8;
	}
	return true;
}


bool CBglDecompressor::Bp16SetIdentical(
	uint8_t*p_uncompressed, 
	int total_cols, 
	int division_rows, 
	int division_cols, 
	int add_val)
{
	if (!division_cols)
	{
		return true;
	}
	
	for (auto row = 0; row < division_rows; ++row)
	{
		memset32(p_uncompressed, (add_val << 16) | add_val, division_cols / 2);
		if (division_cols % 2)
		{
			*reinterpret_cast<uint16_t*>(p_uncompressed + 2 * (division_cols - 1)) = add_val;
		}
		p_uncompressed += 2 * total_cols;
	}
	
	return true;
}


bool CBglDecompressor::Bp16ReadAndSet(
	SBitPoolBp* p_pool, 
	uint8_t* p_uncompressed, 
	int total_cols, 
	int division_rows, 
	int division_cols, 
	int add_value, 
	int num_bits, 
	int num_shifts)
{
	auto result = true;
	
	if (division_cols == 0)
	{
		return result;
	}
	
	for (auto row = 0; row < division_rows; ++row)
	{
		auto *p_source = reinterpret_cast<uint16_t*>(p_uncompressed);
		for (auto col = 0; col < division_cols; ++col)
		{
			auto read_value = 0;
			result &= BpReadNextNBits(p_pool, &read_value, num_bits); 
			*p_source++ = add_value + (read_value << num_shifts);
		}
		p_uncompressed += 2 * total_cols;
	}
	
	return result;
}


bool CBglDecompressor::Bp16DecompressSlice(
	SBitPoolBp* p_pool, 
	uint8_t* p_uncompressed, 
	int total_cols, 
	int add_value, 
	int num_bits, 
	int division_rows, 
	int division_cols, 
	int num_shifts, 
	int max_bits_read)
{
	auto extra_shifts = 0;
	if (num_bits > 8)
	{
		extra_shifts = num_bits - 8;
	}	

	auto value_read = 0;
	auto result = true;
	result &= BpReadNextNBits(p_pool, &value_read, num_bits > 8 ? 8 : num_bits);
	result &= BpReadNextNBits(p_pool, &num_bits, 4);

	const auto new_add_value = add_value + (value_read << (extra_shifts + num_shifts));
	
	if (num_bits == 0)
	{
		return result & Bp16SetIdentical(
			p_uncompressed, 
			total_cols, 
			division_rows, 
			division_cols, 
			new_add_value);
	}
	
	if (division_cols < 8 || division_rows < 8)
	{
		if (num_bits > max_bits_read)
		{
			num_bits = max_bits_read;
		}
			
		return result & Bp16ReadAndSet(
			p_pool, 
			p_uncompressed, 
			total_cols, 
			division_rows, 
			division_cols, 
			new_add_value, 
			num_bits, 
			num_shifts);
	}
	
	const auto row_division_size = division_rows / s_num_bp_slices;
	const auto col_division_size = division_cols / s_num_bp_slices;
	const auto total_row_size = 2 * total_cols * row_division_size;
	for (auto row = 0; row < s_num_bp_slices; ++row)
	{
		auto num_rows = row_division_size;
		if (row == s_num_bp_slices - 1)
		{
			num_rows += division_rows % s_num_bp_slices;
		}
			
		auto *p_source = p_uncompressed;
		for (auto col = 0; col < s_num_bp_slices; ++col)
		{
			auto num_cols = col_division_size;
			if (col == s_num_bp_slices - 1)
			{
				num_cols += division_cols % s_num_bp_slices;
			}
				
			result &= Bp16DecompressSlice(
				p_pool, 
				p_source, 
				total_cols, 
				new_add_value, 
				num_bits, 
				num_rows, 
				num_cols, 
				num_shifts, 
				max_bits_read);
			
			p_source += 2 * col_division_size;
		}
		p_uncompressed += total_row_size;
	}
	
	return result;
}


int CBglDecompressor::Bp16Decompress(
	uint8_t* p_uncompressed, 
	int uncompressed_size, 
	const uint8_t* p_compressed, 
	int compressed_size, 
	int rows, 
	int cols)
{
	if (2 * cols * rows != uncompressed_size)
	{
		return 0;
	}
	
	SBitPoolBp pool
	{
		p_compressed,
		0,
		8,
		compressed_size * 8
	};

	auto num_bits_add_value = 0;
	auto num_shifts = 0;
	auto initial_add_value = 0;
	auto num_bits = 0;
	auto max_bits_read = 0;
	auto result = true;
	result &= BpReadNextNBits(&pool, &num_bits_add_value, 8);
	result &= BpReadNextNBits(&pool, &num_shifts, 8);
	result &= BpReadNextNBits(&pool, &initial_add_value, 8 * num_bits_add_value);
	result &= BpReadNextNBits(&pool, &num_bits, 4);
	result &= BpReadNextNBits(&pool, &max_bits_read, 4);

	if (!max_bits_read)
	{
		max_bits_read = 16;
	}

	const auto row_division_size = rows / s_num_bp_slices;
	const auto col_division_size = cols / s_num_bp_slices;
	const auto total_row_size = 2 * cols * row_division_size;
	for (auto row = 0; row < s_num_bp_slices; ++row)
	{
		auto num_rows = row_division_size;
		if (row == s_num_bp_slices - 1)
		{
			num_rows += rows % s_num_bp_slices;
		}
			
		auto *p_source = p_uncompressed;
		for (auto col = 0; col < s_num_bp_slices; ++col)
		{
			auto num_cols = col_division_size;
			if (col == s_num_bp_slices - 1)
			{
				num_cols += cols % s_num_bp_slices;
			}
				
			result &= Bp16DecompressSlice(
				&pool, 
				p_source, 
				cols, 
				initial_add_value, 
				num_bits, 
				num_rows, 
				num_cols, 
				num_shifts, 
				max_bits_read);
			
			p_source += 2 * col_division_size;
		}
		p_uncompressed += total_row_size;
	}
	return result ? uncompressed_size : result;
}


bool CBglDecompressor::Bp8SetIdentical(
	uint8_t* p_uncompressed, 
	int total_cols, 
	int division_rows, 
	int division_cols, 
	int add_val)
{
	if (!division_cols)
	{
		return true;
	}
	
	for (auto i = 0; i < division_rows; ++i)
	{
		memset(p_uncompressed, add_val, division_cols);
		p_uncompressed += total_cols;
	}
	
	return true;
}


bool CBglDecompressor::Bp8ReadAndSet(
	SBitPoolBp* p_pool, 
	uint8_t* p_uncompressed, 
	int total_cols, 
	int division_rows, 
	int division_cols, 
	int add_value, 
	int num_bits, 
	int num_shifts)
{
	auto result = true;
	
	if (!division_rows)
	{
		return result;
	}
	
	for (auto row = 0; row < division_rows; ++row)
	{
		auto* p_source = p_uncompressed;
		for (auto col = 0; col < division_cols; ++col)
		{
			auto read_value = 0;
			result &= BpReadNextNBits(p_pool, &read_value, num_bits);
			*p_source++ = add_value + (read_value << num_shifts);
		}
		p_uncompressed += total_cols;
	}
	
	return result;
}


bool CBglDecompressor::Bp8DecompressSlice(
	SBitPoolBp* p_pool, 
	uint8_t* p_uncompressed, 
	int total_cols, 
	int add_value, 
	int num_bits, 
	int division_rows, 
	int division_cols, 
	int num_shifts, 
	int max_bits_read)
{
	auto extra_shifts = 0;
	if (num_bits > 8)
	{
		extra_shifts = num_bits - 8;
	}	

	auto value_read = 0;
	auto result = true;
	result &= BpReadNextNBits(p_pool, &value_read, num_bits > 8 ? 8 : num_bits);
	result &= BpReadNextNBits(p_pool, &num_bits, 4);

	const auto new_add_value = add_value + (value_read << (extra_shifts + num_shifts));
	
	if (num_bits == 0)
	{
		return result & Bp8SetIdentical(
			p_uncompressed, 
			total_cols, 
			division_rows, 
			division_cols, 
			new_add_value);
	}
	
	if (division_cols < 8 || division_rows < 8)
	{
		if (num_bits >max_bits_read)
		{
			num_bits = max_bits_read;
		}
			
		return result & Bp8ReadAndSet(
			p_pool, 
			p_uncompressed, 
			total_cols, 
			division_rows, 
			division_cols, 
			new_add_value, 
			num_bits, 
			num_shifts);
	}
	
	const auto row_division_size = division_rows / s_num_bp_slices;
	const auto col_division_size = division_cols / s_num_bp_slices;
	const auto total_row_size = total_cols * row_division_size;
	for (auto row = 0; row < s_num_bp_slices; ++row)
	{
		auto num_rows = row_division_size;
		if (row == s_num_bp_slices - 1)
		{
			num_rows += division_rows % s_num_bp_slices;
		}
			
		auto *p_source = p_uncompressed;
		for (auto col = 0; col < s_num_bp_slices; ++col)
		{
			auto num_cols = col_division_size;
			if (col == s_num_bp_slices - 1)
			{
				num_cols += division_cols % s_num_bp_slices;
			}
				
			result &= Bp8DecompressSlice(
				p_pool, 
				p_source, 
				total_cols, 
				new_add_value, 
				num_bits, 
				num_rows, 
				num_cols, 
				num_shifts, 
				max_bits_read);
			
			p_source += col_division_size;
		}
		p_uncompressed += total_row_size;
	}
	
	return result;
}


int CBglDecompressor::Bp8Decompress(
	uint8_t* p_uncompressed, 
	int uncompressed_size, 
	const uint8_t* p_compressed, 
	int compressed_size, 
	int rows, 
	int cols)
{
	if (cols * rows != uncompressed_size)
	{
		return 0;
	}

	SBitPoolBp pool
	{
		p_compressed,
		0,
		8,
		compressed_size * 8
	};

	auto num_bits_add_value = 0;
	auto num_shifts = 0;
	auto initial_add_value = 0;
	auto num_bits = 0;
	auto max_bits_read = 0;
	auto result = true;
	result &= BpReadNextNBits(&pool, &num_bits_add_value, 8);
	result &= BpReadNextNBits(&pool, &num_shifts, 8);
	result &= BpReadNextNBits(&pool, &initial_add_value, 8 * num_bits_add_value);
	result &= BpReadNextNBits(&pool, &num_bits, 4);
	result &= BpReadNextNBits(&pool, &max_bits_read, 4);
	
	if (!max_bits_read)
	{
		max_bits_read = 16;
	}
		
	const auto row_division_size = rows / s_num_bp_slices;
	const auto col_division_size = cols / s_num_bp_slices;
	const auto total_row_size = cols * row_division_size;
	//initial_add_value = compressed_size << 8 * (4 - num_bits_add_value) >> 8 * (4 - num_bits_add_value);
	for (auto row = 0; row < s_num_bp_slices; ++row)
	{
		auto num_rows = row_division_size;
		if (row == s_num_bp_slices - 1)
		{
			num_rows += rows % s_num_bp_slices;
		}
			
		auto *p_source = p_uncompressed;
		for (auto col = 0; col < s_num_bp_slices; ++col)
		{
			auto num_cols = col_division_size;
			if (col == s_num_bp_slices - 1)
			{
				num_cols += cols % s_num_bp_slices;
			}
				
			result &= Bp8DecompressSlice(
				&pool, 
				p_source, 
				cols, 
				initial_add_value, 
				num_bits, 
				num_rows, 
				num_cols, 
				num_shifts, 
				max_bits_read);
			
			p_source += col_division_size;
		}
		p_uncompressed += total_row_size;
	}
	return result ? uncompressed_size : result;
}