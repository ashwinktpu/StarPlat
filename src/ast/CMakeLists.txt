set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(AddLibrary)

set(starplat_ast_modules)

foreach(module ${starplat_ast_modules})
  add_subdirectory(${module})
endforeach()

set(sources ASTHelper.cpp)
set(headers ASTNode.hpp ASTNodeTypes.hpp)

starplat_add_libary(ast DEPENDENCIES ${starplat_analyser_modules} SOURCES ${sources} HEADERS ${headers})
