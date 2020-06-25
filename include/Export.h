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


#ifndef FLIGHTSIMLIB_EXPORT_H
#define FLIGHTSIMLIB_EXPORT_H


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


#endif