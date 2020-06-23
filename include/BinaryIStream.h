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

#ifndef FLIGHTSIMLIB_IO_BINARYISTREAM_H
#define FLIGHTSIMLIB_IO_BINARYISTREAM_H


#include <cstdint>
#include <istream>


namespace flightsimlib
{

namespace io
{
	
class CByteArrayStreambuf final : public std::streambuf
{
public:

	CByteArrayStreambuf(const uint8_t* begin, const size_t size);
	~CByteArrayStreambuf() = default;
	// copying not allowed
	CByteArrayStreambuf(const CByteArrayStreambuf&) = delete;
	CByteArrayStreambuf& operator= (const CByteArrayStreambuf&) = delete;
	CByteArrayStreambuf(CByteArrayStreambuf&&) = delete;
	CByteArrayStreambuf& operator=(CByteArrayStreambuf&&) = delete;

private:

	int_type underflow() override;
	int_type uflow()  override;
	int_type pbackfail(int_type ch)  override;
	std::streamsize showmanyc() override;
	std::streampos seekoff(
		std::streamoff off,
		std::ios_base::seekdir way,
		std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
		override;
	std::streampos seekpos(
		std::streampos sp,
		std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
		override;

private:

	const uint8_t* const m_begin;
	const uint8_t* const m_end;
	const uint8_t* m_current;
};


class CBinaryIStream final : public std::istream
{
public:
	explicit CBinaryIStream(std::streambuf* sb) : std::istream(sb) {}

	template <class T>
	std::istream& operator>> (T& val)
	{
		return read(reinterpret_cast<char*>(&val), sizeof(val));
	}
};

	
}

}


#endif