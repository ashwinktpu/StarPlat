set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

#include(AddLibrary.cmake)

set(sources deviceVarsAnalyser.cpp deviceVarsAnalyser.h deviceVarsInit.cpp
            deviceVarsPrint.cpp deviceVarsTransfer.cpp getUsedVars.cpp)

starplat_add_libary(deviceVars SOURCES ${sources})
