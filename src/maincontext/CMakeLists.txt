set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# include(AddLibrary.cmake)

set(sources MainContext.cpp)
set(headers MainContext.hpp)
# enum_def.hpp not part of build

starplat_add_libary(maincontext SOURCES ${sources} HEADERS ${headers})
