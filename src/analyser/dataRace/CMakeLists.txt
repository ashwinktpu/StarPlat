set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

#include(AddLibrary.cmake)

set(sources dataRaceAnalyser.cpp dataRaceAnalyser.h)

starplat_add_libary(dataRace SOURCES ${sources})

