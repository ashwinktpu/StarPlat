set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# include(AddLibrary.cmake)

set(sources SymbolTableBuilder.cpp SymbolTableNew.cpp) 
# list(APPEND sources dsl_dyn_cpp_generator.cpp)

starplat_add_libary(symbolutil SOURCES ${sources})
