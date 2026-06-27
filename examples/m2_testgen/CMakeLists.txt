# Standalone build for the M2 reverse-engineering test-data generators.
#
# Builds two tools (see LANDCLASS_SYNTHESIS.md M2):
#   - m2_testgen : authors an LCLookup (0x6F) + land-class (0x68) test .bgl
#                  (links the flightsimlib sources directly, like the M1
#                  lclookup_roundtrip example).
#   - m2_texgen  : writes numbered/colored placeholder land-class textures
#                  (BMP; no flightsimlib / external image dependency).
#
# From this directory:
#   cmake -S . -B build && cmake --build build
#   ./build/m2_testgen --help
#   ./build/m2_texgen  --help
#
# Not wired into newdawn's build (M2 is local-FSX experiment prep).

cmake_minimum_required(VERSION 3.16)
project(m2_testgen LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLIGHTSIMLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(m2_testgen
  m2_testgen.cpp
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

target_compile_definitions(m2_testgen PRIVATE FLIGHTSIMLIB_LIB FLIGHTSIMLIB_EXPORTS)

target_include_directories(m2_testgen PRIVATE
  ${FLIGHTSIMLIB_DIR}/include
  ${FLIGHTSIMLIB_DIR}/external
  ${FLIGHTSIMLIB_DIR}/external/PTC
)

if(NOT WIN32)
  target_compile_definitions(m2_testgen PRIVATE __stdcall= __cdecl= __thiscall=)
endif()

# Texture generator is fully standalone (no flightsimlib).
add_executable(m2_texgen m2_texgen.cpp)
