# Standalone build for the LCLookup (0x6F) round-trip example / test.
#
# Self-contained: compiles the flightsimlib sources it needs directly into the
# example executable so it can be built without the rest of the project. From
# this directory:
#
#   cmake -S . -B build && cmake --build build && ./build/lclookup_roundtrip
#
# Not wired into newdawn's build (see LANDCLASS_SYNTHESIS.md M1).

cmake_minimum_required(VERSION 3.16)
project(lclookup_roundtrip LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLIGHTSIMLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(lclookup_roundtrip
  lclookup_roundtrip.cpp
  ${FLIGHTSIMLIB_DIR}/src/BglCompressor.cpp
  ${FLIGHTSIMLIB_DIR}/src/BglData.cpp
  ${FLIGHTSIMLIB_DIR}/src/BglDecompressor.cpp
  ${FLIGHTSIMLIB_DIR}/src/BglFile.cpp
  ${FLIGHTSIMLIB_DIR}/src/BinaryStream.cpp
  ${FLIGHTSIMLIB_DIR}/src/CglModule.cpp
  ${FLIGHTSIMLIB_DIR}/src/FlightSimLib.cpp
  ${FLIGHTSIMLIB_DIR}/src/IObject.cpp
  ${FLIGHTSIMLIB_DIR}/src/VectorTile.cpp
  ${FLIGHTSIMLIB_DIR}/src/VectorTileBuilder.cpp
  ${FLIGHTSIMLIB_DIR}/external/PTC/PTC.c
  ${FLIGHTSIMLIB_DIR}/external/PTC/PTCAdaptiveDecoder.c
  ${FLIGHTSIMLIB_DIR}/external/PTC/PTCColorMapper.c
  ${FLIGHTSIMLIB_DIR}/external/PTC/PTCRow.c
  ${FLIGHTSIMLIB_DIR}/external/PTC/PTCTransform.c
)

target_compile_definitions(lclookup_roundtrip PRIVATE FLIGHTSIMLIB_LIB FLIGHTSIMLIB_EXPORTS)

target_include_directories(lclookup_roundtrip PRIVATE
  ${FLIGHTSIMLIB_DIR}/include
  ${FLIGHTSIMLIB_DIR}/external
  ${FLIGHTSIMLIB_DIR}/external/PTC
)

if(NOT WIN32)
  target_compile_definitions(lclookup_roundtrip PRIVATE __stdcall= __cdecl= __thiscall=)
endif()
