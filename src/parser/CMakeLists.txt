set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(AddLibrary)

find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.0 REQUIRED)

FLEX_TARGET(LEXER lexer.l LEXER_OUT)
BISON_TARGET(PARSER lrparser.y PARSER_OUT)
add_flex_bison_dependency(LEXER PARSER)

set(headers includeHeader.hpp)

starplat_add_libary(parser DEPENDENCIES LEXER_OUT PARSER_OUT HEADERS ${headers})
