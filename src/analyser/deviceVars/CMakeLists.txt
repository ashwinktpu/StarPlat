set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

#include(AddLibrary.cmake)

set(sources deviceVarsAnalyser.cpp deviceVarsInit.cpp
            deviceVarsPrint.cpp deviceVarsTransfer.cpp getUsedVars.cpp)

set(headers deviceVarsAnalyser.h)

starplat_add_libary(deviceVars SOURCES ${sources} HEADERS ${headers})
