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
// File:     BglCompressor.cpp
//
// Summary:  Implementation of FSX BGL compression routines 
//
//           This is a work in progress
//
// Author:   Sean Isom
//
//******************************************************************************                                        


#include "BglCompressor.h"

#include <cmath>


using namespace flightsimlib::io;


// ReSharper disable CppInitializedValueIsAlwaysRewritten


// 140052430
int CBglCompressor::CompressDelta(
	uint8_t* p_compressed, 
	int compressed_size, 
	const uint8_t* p_uncompressed, 
	int uncompressed_size)
{
	const auto* p_original_compressed = p_compressed;
	if (uncompressed_size & 1)
	{
		p_compressed[0] = p_uncompressed[0];
		++p_compressed;
		++p_uncompressed;
		if (--uncompressed_size == 0)
		{
			return 1;
		}
	}

	const auto* p_src = reinterpret_cast<const short*>(p_uncompressed);
	auto previous = *p_src++;
	*reinterpret_cast<short*>(p_compressed) = previous;
	p_compressed += sizeof(short);
	const auto count = (uncompressed_size / 2) - 1;

	for (auto i = 0; i < count; ++i)
	{
		const auto current = *p_src;
		const auto delta = current - previous;
		if (delta + 125 > 0xFC)
		{
			if (delta + 381 > 0xFF)
			{
				if (delta - 128 > 0xFF)
				{
					*p_compressed++ = 0x80;
					*reinterpret_cast<short*>(p_compressed) = current;
					p_compressed += sizeof(short);
				}
				else
				{
					*p_compressed++ = 0x82;
					*p_compressed++ = delta - 128;
				}
			}
			else
			{
				*p_compressed++ = 0x81;
				*p_compressed++ = abs(delta) - 126; // abs?
			}
		}
		else
		{
			*p_compressed++ = delta;
		}
		previous = *p_src++;
	}
	
	const auto bytes_written = static_cast<int>(p_compressed - p_original_compressed);
	return bytes_written == compressed_size ? compressed_size : 0;
}
