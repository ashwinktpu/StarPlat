set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# include(AddLibrary.cmake)

set(sources SymbolTableBuilder.cpp SymbolTableNew.cpp) 
set(headers SymbolTable.h SymbolTableBuilder.h)
starplat_add_libary(symbolutil SOURCES ${sources} HEADERS ${headers})
