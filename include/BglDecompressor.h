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


#ifndef FLIGHTSIMLIB_IO_BGLDECOMPRESSOR_H
#define FLIGHTSIMLIB_IO_BGLDECOMPRESSOR_H


#include <cstdint>


// Define FLIGHTSIMLIB_EXPORTED for any platform
#if defined _WIN32 || defined __CYGWIN__
	#ifdef FLIGHTSIMLIB_EXPORTS
		#ifdef __GNUC__
			#define FLIGHTSIMLIB_EXPORTED  __attribute__ ((dllexport))
		#else
			#define FLIGHTSIMLIB_EXPORTED  __declspec(dllexport) 
		#endif
	#else
		#ifdef __GNUC__
			#define FLIGHTSIMLIB_EXPORTED __attribute__ ((dllimport))
		#else
			#define FLIGHTSIMLIB_EXPORTED __declspec(dllimport)
		#endif
	#endif
	#define FLIGHTSIMLIB_NOT_EXPORTED
#else
	#if __GNUC__ >= 4
		#define FLIGHTSIMLIB_EXPORTED __attribute__ ((visibility ("default")))
		#define FLIGHTSIMLIB_NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
	#else
		#define FLIGHTSIMLIB_EXPORTED
		#define FLIGHTSIMLIB_NOT_EXPORTED
	#endif
#endif


#ifndef CDECL
	#if defined _WIN32 || defined __CYGWIN__
		#define CDECL __cdecl
	#else
		#define CDECL
	#endif
#endif


#define PTC_LIB // Use PTC codec from static lib


namespace flightsimlib
{
	namespace io
	{
		class CBglDecompressor
		{
		public:

			static FLIGHTSIMLIB_EXPORTED int CDECL DecompressLz1(
				uint8_t* p_uncompressed, 
				int uncompressed_size, 
				const uint8_t* p_compressed, 
				int compressed_size);
			
			static FLIGHTSIMLIB_EXPORTED int CDECL DecompressLz2(
				uint8_t* p_uncompressed, 
				int uncompressed_size, 
				const uint8_t* p_compressed, 
				int compressed_size);
			
			static FLIGHTSIMLIB_EXPORTED int CDECL DecompressDelta(
				uint8_t* p_uncompressed, 
				int uncompressed_size, 
				const uint8_t* p_compressed);
			
			static FLIGHTSIMLIB_EXPORTED int CDECL DecompressBitPack(
				uint8_t* p_uncompressed, 
				int uncompressed_size, 
				const uint8_t* p_compressed, 
				int compressed_size, 
				int rows, 
				int cols);
			
			static FLIGHTSIMLIB_EXPORTED int CDECL DecompressPtc(
				uint8_t* p_uncompressed, 
				int uncompressed_size, 
				const uint8_t* p_compressed, 
				int compressed_size, 
				int rows, 
				int cols, 
				int depth, 
				int bpp);

		private:
			
			// Forward Declarations
			struct SBitPoolBp;
			struct SBitPoolLz;

			
			static int LzReadNextBit(
				SBitPoolLz* p_pool);
			
			static int LzReadNextNBits(
				SBitPoolLz* p_pool, 
				int num_bits);
			
			static bool BpReadNextNBits(
				SBitPoolBp* p_pool, 
				int* o_dest, 
				int num_bits);
			
			static bool Bp16SetIdentical(
				uint8_t* p_uncompressed, 
				int total_cols, 
				int division_rows, 
				int division_cols, 
				int add_val);
			
			static bool Bp16ReadAndSet(
				SBitPoolBp* p_pool, 
				uint8_t* p_uncompressed, 
				int total_cols, 
				int division_rows, 
				int division_cols, 
				int add_value, 
				int num_bits, 
				int num_shifts);
			
			static bool Bp16DecompressSlice(
				SBitPoolBp* p_pool, 
				uint8_t* p_uncompressed, 
				int total_cols, 
				int add_value, 
				int num_bits, 
				int division_rows, 
				int division_cols, 
				int num_shifts, 
				int max_bits_read);
			
			static int Bp16Decompress(
				uint8_t* p_uncompressed, 
				int uncompressed_size, 
				const uint8_t* p_compressed, 
				int compressed_size, 
				int rows, 
				int cols);
			
			static bool Bp8SetIdentical(
				uint8_t* p_uncompressed, 
				int total_cols, 
				int division_rows, 
				int division_cols, 
				int add_val);
			
			static bool Bp8ReadAndSet(
				SBitPoolBp* p_pool, 
				uint8_t* p_uncompressed, 
				int total_cols, 
				int division_rows, 
				int division_cols, 
				int add_value, 
				int num_bits, 
				int num_shifts);
			
			static bool Bp8DecompressSlice(
				SBitPoolBp* p_pool, 
				uint8_t* p_uncompressed, 
				int total_cols, 
				int add_value, 
				int num_bits, 
				int division_rows, 
				int division_cols, 
				int num_shifts, 
				int max_bits_read);
			
			static int Bp8Decompress(
				uint8_t* p_uncompressed, 
				int uncompressed_size, 
				const uint8_t* p_compressed, 
				int compressed_size, 
				int rows, 
				int cols);

			
			static int s_shift_mask[];
			static constexpr int s_num_bp_slices = 4;
		};
	}
}


#endif