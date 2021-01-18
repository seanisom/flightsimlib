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
#include <fstream>
#include <istream>
#include <memory>
#include <string>


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

	BinaryFileStream& Read(void* val, int size)
	{
		m_stream.read(static_cast<char*>(val), size);
		return *this;
	}

	BinaryFileStream& Write(const void* val, int size)
	{
		m_stream.write(static_cast<const char*>(val), size);
		return *this;
	}

	std::string ReadString(int count)
	{
		const auto bytes = std::unique_ptr<char[]>(new char[count]);

		m_stream.read(bytes.get(), count);

		return std::string(bytes.get(), count);
	}

	std::string ReadCString()
	{
		std::string buffer;
		std::getline(m_stream, buffer, '\0');
		return buffer;
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
		return static_cast<int>(m_stream.tellg());
	}

	void SetPosition(int pos, std::fstream::_Seekdir dir = std::fstream::beg)
	{
		m_stream.seekg(pos, dir);
		m_stream.seekp(pos, dir);
	}

	std::fstream m_stream;
};

	
}

}


#endif