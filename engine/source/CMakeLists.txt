if (MM_CHECK_PARAMETERS)
    add_compile_definitions("MM_CHECK_PARAMETERS")
endif ()

if (MM_CHECK_ALL_EXCEPTION_PROCESS)
    add_compile_definitions("MM_CHECK_ALL_EXCEPTION_PROCESS")
endif ()

#####################  utils  ####################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utils")

#####################  runtime  ####################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/runtime")

#####################  editor  ####################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/editor")