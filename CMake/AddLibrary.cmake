function(starplat_add_libary modulename)
  set(options OPTIONAL FAST)
  set(multi_value_args SOURCES DEPENDENCIES HEADERS)
  set(one_value_args LIB_TYPE)
  cmake_parse_arguments(${modulename} "${options}" "${one_value_args}"
                        "${multi_value_args}" ${ARGN})

  message(
    WARNING "SOURCES : ${${modulename}_SOURCES} ${${modulename}_DEPENDENCIES}")
  add_library(starplat_${modulename} ${${modulename}_SOURCES}
                                     ${${modulename}_DEPENDENCIES})
  set_target_properties(starplat_${modulename} PROPERTIES LINKER_LANGUAGE CXX)

endfunction()
