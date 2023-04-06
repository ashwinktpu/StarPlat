set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

#include(AddLibrary.cmake)

set(sources analyserUtil.cpp getUsedVars.cpp)

starplat_add_libary(getUsedVars SOURCES ${sources})
