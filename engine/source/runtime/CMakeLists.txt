include("utils")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")

#####################################################
#####################  platform  ####################
#####################################################
########## base ##########
AddMoudle("base" "${CMAKE_CURRENT_SOURCE_DIR}/platform/base")

##########  file_system  ##########
AddMoudle("file_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/file_system")
target_compile_definitions(file_system PUBLIC "BIN_DIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"")
target_link_libraries(file_system PUBLIC base)

##########  config_system  ##########
AddMoudle("config_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/config_system")
target_compile_definitions(config_system PUBLIC "-DCONFIG_DIR=\"${config_dir}\"")
target_link_libraries(config_system PUBLIC file_system base)


#################################################
#####################  core  ####################
#################################################
##########  log_system  ##########
AddMoudle("log_system" "${CMAKE_CURRENT_SOURCE_DIR}/core/log")
target_link_libraries(log_system PUBLIC spdlog::spdlog base)

##########  reflection  ##########
AddMoudle("reflection" "${CMAKE_CURRENT_SOURCE_DIR}/core/reflection")

##########  math  ##########
AddMoudle("math" "${CMAKE_CURRENT_SOURCE_DIR}/core/math")
target_link_libraries(math PUBLIC glm)

########## task_system ##########
AddMoudle("task_system" "${CMAKE_CURRENT_SOURCE_DIR}/core/task_system")
target_link_libraries(task_system PUBLIC Taskflow_lib)

#####################################################
#####################  resource  ####################
#####################################################
########## asset_type ##########
AddMoudle("asset_type" "${CMAKE_CURRENT_SOURCE_DIR}/resource/asset_type")
target_link_libraries(asset_type PUBLIC math assimp_lib stb task_system)

########## resource_system ##########
AddMoudle("asset_system" "${CMAKE_CURRENT_SOURCE_DIR}/resource/asset_system")
target_link_libraries(asset_system PUBLIC file_system config_system log_system asset_type)

#####################################################
#####################  function  ####################
#####################################################
##########  globel_context  ##########
# AddMoudle("globel_context" "${CMAKE_CURRENT_SOURCE_DIR}/function/global")
# target_link_libraries(globel_context PUBLIC config_system log_system file_system)

##########  render  ##########
AddMoudle("render" "${CMAKE_CURRENT_SOURCE_DIR}/function/render")
target_link_libraries(render PUBLIC math color log_system config_system file_system asset_system vulkan_lib glfw stb task_system)