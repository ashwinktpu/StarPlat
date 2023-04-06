set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(AddLibrary)

set(starplat_analyser_modules attachProp dataRace deviceVars)
# list(APPEND starplat_analyser_modules getUsedVars)

foreach(module ${starplat_analyser_modules})
  add_subdirectory(${module})
endforeach()

starplat_add_libary(analyser DEPENDENCIES ${starplat_analyser_modules})
