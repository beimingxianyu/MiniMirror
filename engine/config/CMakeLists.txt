set(source_config_dir "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}")

configure_file("${source_config_dir}/init_config.ini.in" "${config_dir}/init_config.ini")
