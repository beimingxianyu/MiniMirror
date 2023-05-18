include("utils")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")

#####################################################
#####################  platform  ####################
#####################################################
########## base ##########
AddModule("base" "${CMAKE_CURRENT_SOURCE_DIR}/platform/base")
target_link_libraries(base PUBLIC utils)

##########  file_system  ##########
AddModule("file_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/file_system")
target_compile_definitions(file_system PUBLIC "BIN_DIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_libraries(file_system PUBLIC base)
else ()
    target_link_libraries(file_system PUBLIC base stdc++fs)
endif ()

##########  config_system  ##########
AddModule("config_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/config_system")
target_compile_definitions(config_system PUBLIC "-DCONFIG_DIR=\"${config_dir}\"")
target_link_libraries(config_system PUBLIC file_system base)


#################################################
#####################  core  ####################
#################################################
##########  log_system  ##########
AddModule("log_system" "${CMAKE_CURRENT_SOURCE_DIR}/core/log")
target_link_libraries(log_system PUBLIC spdlog::spdlog base)

##########  reflection  ##########
AddModule("reflection" "${CMAKE_CURRENT_SOURCE_DIR}/core/reflection")

##########  math  ##########
AddModule("math" "${CMAKE_CURRENT_SOURCE_DIR}/core/math")
target_link_libraries(math PUBLIC glm)

########## task_system ##########
AddModule("task_system" "${CMAKE_CURRENT_SOURCE_DIR}/core/task_system")
target_link_libraries(task_system PUBLIC Taskflow_lib)

########## manager ##########
AddModule("Manager" "${CMAKE_CURRENT_SOURCE_DIR}/core/manager")

#####################################################
#####################  resource  ####################
#####################################################
########## asset_type ##########
AddModule("asset_type" "${CMAKE_CURRENT_SOURCE_DIR}/resource/asset_type")
target_link_libraries(asset_type PUBLIC math assimp_lib stb task_system)

########## resource_system ##########
AddModule("asset_system" "${CMAKE_CURRENT_SOURCE_DIR}/resource/asset_system")
target_link_libraries(asset_system PUBLIC file_system config_system log_system asset_type)

#####################################################
#####################  function  ####################
#####################################################
##########  globel_context  ##########
# AddModule("globel_context" "${CMAKE_CURRENT_SOURCE_DIR}/function/global")
# target_link_libraries(globel_context PUBLIC config_system log_system file_system)

##########  render  ##########
AddModule("render" "${CMAKE_CURRENT_SOURCE_DIR}/function/render")
target_link_libraries(render PUBLIC math color log_system config_system file_system asset_system vulkan_lib glfw stb task_system)