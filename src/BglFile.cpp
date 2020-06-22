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
//******************************************************************************       


#include "BglFile.h"


using namespace flightsimlib::io;


bool IsTrq1BglLayer(EBglLayerType layer_type)
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


bool IsRcs1BglLayer(EBglLayerType layer_type)
{
    if (layer_type == EBglLayerType::TerrainElevation)
    {
        return true;
    }
    return false;
}
