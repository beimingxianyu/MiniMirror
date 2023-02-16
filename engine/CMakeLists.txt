set(config_dir "${engine_dir}/config")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/asset")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/config")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rd_party")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/source")
