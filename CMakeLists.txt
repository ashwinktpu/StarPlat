cmake_minimum_required(VERSION 3.25)

project(starplat)
add_subdirectory(src)

add_executable(starplat src/temp.cpp)
link_directories(starplat startplat_analyser startplat_ast startplat_backends startplat_maincontext startplat_symbolutil startplat_parser)
set_target_properties(starplat PROPERTIES LINKER_LANGUAGE CXX)
