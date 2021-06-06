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


namespace flightsimlib::io
{
	
class CByteArrayStreambuf final : public std::streambuf
{
public:
	CByteArrayStreambuf(const uint8_t* begin, size_t size);
	~CByteArrayStreambuf() override = default;
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

	const uint8_t* const m_begin;
	const uint8_t* const m_end;
	const uint8_t* m_current;
};


class IBinaryStream
{
protected:
	explicit IBinaryStream(std::iostream& stream) : m_stream(stream) { }

public:
	template <typename T>
	IBinaryStream& operator>> (T& val)
	{
		m_stream.read(reinterpret_cast<char*>(&val), sizeof(val));
		return *this;
	}

	template <typename T>
	T ReadType()
	{
		T t{};
		m_stream.read(reinterpret_cast<char*>(&t), sizeof(t));
		return t;
	}
	
	template <typename T>
	IBinaryStream& operator<< (T& val)
	{
		m_stream.write(reinterpret_cast<const char*>(&val), sizeof(val));
		return *this;
	}

	IBinaryStream& Read(void* val, int size)
	{
		m_stream.read(static_cast<char*>(val), size);
		return *this;
	}

	IBinaryStream& Write(const void* val, int size)
	{
		m_stream.write(static_cast<const char*>(val), size);
		return *this;
	}

	[[nodiscard]] std::string ReadString(int count) const
	{
		const auto bytes = std::unique_ptr<char[]>(new char[count]);

		m_stream.read(bytes.get(), count);
		return std::string(bytes.get(), count);
	}

	[[nodiscard]] std::string ReadCString() const
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

	[[nodiscard]] int GetPosition() const
	{
		// This is a problem with ReSharper - no issue here
		// ReSharper disable once CppRedundantCastExpression
		return static_cast<int>(m_stream.tellg());
	}

	void SetPosition(int pos, std::fstream::_Seekdir dir = std::fstream::beg) const
	{
		m_stream.seekg(pos, dir);
		m_stream.seekp(pos, dir);
	}

protected:
	std::iostream& m_stream;
};


// TODO - fstream leaky abstractions!
class BinaryFileStream final : public IBinaryStream
{
public:
	explicit BinaryFileStream(const std::wstring& filename) : IBinaryStream(m_fstream),
		m_fstream(filename, std::fstream::out | std::fstream::in | std::fstream::binary) {}

	void Open(const std::wstring& filename, std::ios_base::openmode mode =
		          std::fstream::out | std::fstream::in | std::fstream::binary)
	{
		m_fstream.open(filename, mode);
	}

	[[nodiscard]] bool IsOpen() const
	{
		return m_fstream.is_open();
	}

	void Close()
	{
		m_fstream.close();
	}

private:
	std::fstream m_fstream;
};


class BinaryMemoryStream final : public IBinaryStream
{
public:
	explicit BinaryMemoryStream(uint8_t* pointer, int length) : IBinaryStream(m_iostream),
		m_buf(pointer, length), m_iostream(&m_buf), m_pointer(pointer), m_length(length) {}

	[[nodiscard]] uint8_t* GetPointer() const
	{
		return m_pointer;
	}

	[[nodiscard]] int GetLength() const
	{
		return m_length;
	}

private:
	CByteArrayStreambuf m_buf;
	std::iostream m_iostream;
	uint8_t* m_pointer;
	int m_length;
};


}

#endif
