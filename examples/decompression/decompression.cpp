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

// 
// This example takes a couple of compressed sections from a BGL file and demonstrates how to use
// the decompression routines from the flightsimlib dll.
//
// This BGL File is compiled from the FSX SDK resample examples. You can rebuild this yourself if desired. 
//
// Note! This does not parse the BGL or TRQ1 structure - for the sake of this demo the compressed offsets
// are already known. Safe file parsing capabilities are being added in a future release.
//
// See https://docs.microsoft.com/en-us/previous-versions/microsoft-esp/cc707102(v=msdn.10) for more resample info.
// See https://github.com/seanisom/flightsimlib/wiki for flightsimlib documentation
//
// This is built with Visual Studio and linked with the dll link lib for convenience. Instead, you could:
// 
// 1. Link Dynamically on Windows, no .lib: (Currently you will need to use the C++ mangled name!)
// typedef int (*LP_DECOMPRESSBITPACK)(char *p_uncompressed, int uncompressed_size,
//     const char *p_compressed, int compressed_size, int rows, int cols);
// HINSTANCE hDLL = NULL;
// LP_DECOMPRESSBITPACK DecompressBitPack = nullptr;
// hDLL = LoadLibrary(L"flightsimlib.dll");
// DecompressBitPack = (LP_DECOMPRESSBITPACK)GetProcAddress((HMODULE)hDLL,
//     "?DecompressBitPack@CBglDecompressor@io@flightsimlib@@SAHPEAEHPEBEHHH@Z");
//
// Then you can DecompressBitPack(foo, bar, etc..); like normal with no namespace.
//
// 2. You could build under a different compiler or OS manually, such as:
// g++ -c -fPIC BglDecompressor.cpp
// g++ -shared -o flightsimlib.so -fPIC BglDecompressor.o
// g++ decompression.cpp -o decompression-L. -lflightsimlib
//
// NOTE - if you are missing the header or the .lib to link when you open this solution,
// build the parent flightsimlib.sln first - it will xcopy these to the examples folder.

#include "BglDecompressor.h"

#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>


#ifdef _WIN64
#pragma comment(lib, "../lib/x64/flightsimlib.lib")
#else
#pragma comment(lib, "../lib/x86/flightsimlib.lib")
#endif


using namespace std;
using flightsimlib::io::CBglDecompressor;

// This is all raw static data! To be enhanced by BGLReader
int main()
{
	const string filename = "DeathValley_Elevations";
	
	cout << "Reading input bgl file: " << filename << ".bgl" << endl;

	std::ifstream input_file(filename + ".bgl", std::ifstream::binary);
	if (!input_file.good())
	{
		cout << "Error opening input bgl file!" << endl;
		return 1;
	}

	const auto file_contents_size = 1901403;
	const auto file_contents = std::make_unique<uint8_t[]>(file_contents_size);
	input_file.read(reinterpret_cast<char*>(file_contents.get()), file_contents_size);
	if (!input_file.good())
	{
		cout << "Error reading input bgl file!" << endl;
		return 2;
	}

	// static input file constants for verification
	const auto offset = 1897828;
	const auto input_size = 1748;
	const auto intermediate_size = 2093;
	const auto dimensions = 257;
	const auto final_size = static_cast<int>(dimensions * dimensions * sizeof(int16_t));

	// Make output dynamic for easier future reuse
	const auto intermediate_data = std::make_unique<uint8_t[]>(intermediate_size);
	const auto final_data = std::make_unique<uint8_t[]>(final_size);
	const auto* p_input = &file_contents[offset];

	// This demonstrates a multi-step decompression
	// See details at:  https://www.fsdeveloper.com/wiki/index.php?title=BGL_File_Format#TRQ1_Record
	//
	auto bytes_written = ::CBglDecompressor::DecompressLz1(
		intermediate_data.get(), 
		intermediate_size, 
		p_input, 
		input_size);

	assert(bytes_written == intermediate_size);
	
	bytes_written = ::CBglDecompressor::DecompressBitPack(
		final_data.get(), 
		final_size, 
		intermediate_data.get(), 
		intermediate_size, 
		dimensions, 
		dimensions);

	assert(bytes_written == final_size);

	// verify data
	std::ifstream verification_file(filename + ".bin", std::ifstream::binary);
	if (!verification_file.good())
	{
		cout << "Error opening verification bin file!" << endl;
		return 3;
	}

	auto verification_data = std::make_unique<uint8_t[]>(final_size);
	verification_file.read(reinterpret_cast<char*>(verification_data.get()), final_size);
	if (!verification_file.good())
	{
		cout << "Error reading verification bin file!" << endl;
		return 4;
	}

	for (auto i = 0; i < final_size; ++i)
	{
		assert(final_data[i] == verification_data[i]);
	}

	cout << "Decompressed data verified!" << endl;
	
	return 0;
}
