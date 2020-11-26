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

#ifndef FLIGHTSIMLIB_IO_BGLTYPES_H
#define FLIGHTSIMLIB_IO_BGLTYPES_H

namespace flightsimlib
{

namespace io
{

class IBglRunway
{
public:
	virtual float GetLength() const = 0;
	virtual float GetWidth() const = 0;
	virtual float GetHeading() const = 0;
	virtual float GetPatternAltitude() const = 0;
};


class IBglAirport
{
public:
	virtual float GetMagVar() const = 0;
	virtual void SetMagVar(float value) = 0;
};


class IBglExclusion
{
public:
	virtual bool IsGenericBuilding() const = 0;
	virtual void SetGenericBuilding(bool value) = 0;
};

class IBglMarker
{
public:
	virtual float GetHeading() const = 0;
	virtual void SetHeading(float value) = 0;
};

class IBglGeopol
{
public:
	virtual double GetMinLongitude() const = 0;
	virtual void SetMinLongitude(double value) = 0;
	virtual double GetMaxLongitude() const = 0;
	virtual void SetMaxLongitude(double value) = 0;
	virtual double GetMinLatitude() const = 0;
	virtual void SetMinLatitude(double value) = 0;
	virtual double GetMaxLatitude() const = 0;
	virtual void SetMaxLatitude(double value) = 0;

	enum class EType : uint8_t {
		None = 0,
		Coastline = 64,
		BOUNDARY = 128,
		DASHED_BOUNDARY = 129, // 0x00000081
	};

	virtual EType GetGeopolType() const = 0;
	virtual void SetGeopolType(EType value) = 0;
	virtual int GetNumVertices() const = 0;

private:
	virtual void SetNumVertices(int value) = 0;
	// TODO - need Setter with realloc - need a better interface with this
};

class ITerrainRasterQuad1
{
public:
	virtual int Rows() const = 0;
	virtual int Cols() const = 0;
};

}

}

#endif