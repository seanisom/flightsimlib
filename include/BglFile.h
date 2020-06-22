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


namespace flightsimlib
{
	namespace io
	{
		enum class EBglLayerType
		{
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
			FakeTypes = 0x2710,
			IcaoRunway = 0x2711,
			Unknown = -1
		};

		bool IsTrq1BglLayer(EBglLayerType layer_type);
		bool IsRcs1BglLayer(EBglLayerType layer_type);
	}
}


#endif