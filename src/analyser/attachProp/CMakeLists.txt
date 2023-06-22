set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# include(AddLibrary.cmake)

set(sources attachPropAnalyser.cpp)
set(headers attachPropAnalyser.h)

starplat_add_libary(attachProp SOURCES ${sources} HEADERS ${headers})
