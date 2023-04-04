cmake_minimum_required(VERSION 3.25)

project(starplat)
add_subdirectory(src)

add_executable(starplat src/temp.cpp)
target_link_libraries(starplat_analyser)
set_target_properties(starplat PROPERTIES LINKER_LANGUAGE CXX)
