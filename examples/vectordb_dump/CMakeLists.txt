# Standalone build for the TerrainVectorDb (CVX, 0x65) dump tool.
#
# Self-contained: compiles the flightsimlib sources it needs directly into the
# example executable so it can be built without the rest of the project. From
# this directory:
#
#   cmake -S . -B build && cmake --build build && ./build/vectordb_dump
#
# Ground-truth validation aid; point it at a local FSX install's cvx files.

cmake_minimum_required(VERSION 3.16)
project(vectordb_dump LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLIGHTSIMLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(vectordb_dump
  vectordb_dump.cpp
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

target_compile_definitions(vectordb_dump PRIVATE FLIGHTSIMLIB_LIB FLIGHTSIMLIB_EXPORTS)

target_include_directories(vectordb_dump PRIVATE
  ${FLIGHTSIMLIB_DIR}/include
  ${FLIGHTSIMLIB_DIR}/external
  ${FLIGHTSIMLIB_DIR}/external/PTC
)

if(NOT WIN32)
  target_compile_definitions(vectordb_dump PRIVATE __stdcall= __cdecl= __thiscall=)
endif()
