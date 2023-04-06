set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# include(AddLibrary.cmake)

set(sources MainContext.cpp)
# list(APPEND sources dsl_dyn_cpp_generator.cpp)

starplat_add_libary(maincontext SOURCES ${sources})
