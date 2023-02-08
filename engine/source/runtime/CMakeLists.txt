include("utils")


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")


#####################  platform  ####################
##########  config_system  ##########
AddMoudle("config_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/config_system")
target_compile_definitions(config_system PRIVATE "-DCONFIG_DIR=\"${config_dir}\"")

##########  file_system  ##########
AddMoudle("file_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/file_system")
target_compile_definitions(file_system PRIVATE "-DBIN_DIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"")

#####################  core  ####################
##########  log_system  ##########
AddMoudle("log_system" "${CMAKE_CURRENT_SOURCE_DIR}/core/log")
target_link_libraries(log_system PRIVATE spdlog::spdlog)

##########  reflection  ##########
# AddMoudle("reflection" "${CMAKE_CURRENT_SOURCE_DIR}/core/reflection")
# target_link_libraries(reflection PUBLIC json11)

##########  color  ##########
AddMoudle("color" "${CMAKE_CURRENT_SOURCE_DIR}/core/color")
target_link_libraries(color PUBLIC glm)

##########  math  ##########
AddMoudle("math" "${CMAKE_CURRENT_SOURCE_DIR}/core/math")
target_link_libraries(math PUBLIC glm)
