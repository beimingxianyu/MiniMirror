include("utils")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")

#####################################################
#####################  platform  ####################
#####################################################
##########  file_system  ##########
AddMoudle("file_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/file_system")
target_compile_definitions(file_system PUBLIC "-DBIN_DIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"")

##########  config_system  ##########
AddMoudle("config_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/config_system")
target_compile_definitions(config_system PUBLIC "-DCONFIG_DIR=\"${config_dir}\"")
target_link_libraries(config_system PUBLIC file_system)


#################################################
#####################  core  ####################
#################################################
##########  log_system  ##########
AddMoudle("log_system" "${CMAKE_CURRENT_SOURCE_DIR}/core/log")
target_link_libraries(log_system PUBLIC spdlog::spdlog)

##########  reflection  ##########
AddMoudle("reflection" "${CMAKE_CURRENT_SOURCE_DIR}/core/reflection")

##########  math  ##########
AddMoudle("math" "${CMAKE_CURRENT_SOURCE_DIR}/core/math")
target_link_libraries(math PUBLIC glm)

########## task_flow ##########
add_library(task_flow INTERFACE IMPORTED)
set_target_properties(task_flow PROPERTIES
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_INCLUDE_DIRECTORIES "${source_third_party_dir}/TaskFlow/include"
  INTERFACE_LINK_LIBRARIES "Threads::Threads"
)

#####################################################
#####################  resource  ####################
#####################################################
########## asset_type ##########
AddMoudle("asset_type" "${CMAKE_CURRENT_SOURCE_DIR}/resource/asset_type")
target_link_libraries(asset_type PUBLIC math assimp_lib stb)

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
target_link_libraries(render PUBLIC math color log_system config_system file_system asset_system vulkan_lib glfw stb)