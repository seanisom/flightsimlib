# flightsimlib
Flight Simulator C++ Common libs


## Introduction
I have been building utilities and addons for Microsoft Flight Simulator for the past 10 years.

Over this time I have built quite a bit of useful library code spread across many products and langauges.

This is my attempt to refactor what I can into a lightweight common C++ library for others to build on top of.


## Features
This library currently provides a thin cross-platform abstraction for BGL File Decompression. 
This allows one to read raw compressed data from a ESP-based (FSX, ESP, P3D) resample-compressed BGL file.

More features are coming, starting with BGL-file functions and structures.


## Documentation
Please [see the wiki](https://github.com/seanisom/flightsimlib/wiki) for basic usage.

You can grab a precompiled binary release with headers from the `releases` tab, or clone and build from source.

Minimal sample code is provided in the `examples` folder.


## Roadmap
* More Sample code / integration
* Doxygen docs
* Factory methods with C API wrapper to ease integration with other languages
* Open-source more functionality of my Flight Sim projects into this library:
	* BGL File Parsing
	* MDL Files and supporting objects
	* Simprop
	* Autogen
	* Texture Compositing
	* 2D and 3D Graphics
	* Terrain Tiles
	* etc


## License
Permissive MIT License - See LICENSE