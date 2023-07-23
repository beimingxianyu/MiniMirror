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
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_libraries(file_system PUBLIC base)
else ()
    target_link_libraries(file_system PUBLIC base stdc++fs)
endif ()

##########  config_system  ##########
AddModule("config_system" "${CMAKE_CURRENT_SOURCE_DIR}/platform/config_system")
target_compile_definitions(config_system PUBLIC "MM_RELATIVE_CONFIG_DIR=${relative_config_dir}")
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
AddModule("manager" "${CMAKE_CURRENT_SOURCE_DIR}/core/manager")
target_link_libraries(manager PUBLIC base log_system)

#####################################################
#####################  resource  ####################
#####################################################
########## resource_system ##########
AddModule("asset_system" "${CMAKE_CURRENT_SOURCE_DIR}/resource/asset_system")
target_link_libraries(asset_system PUBLIC file_system config_system log_system math assimp::assimp stb task_system manager utils)
target_compile_definitions(asset_system PUBLIC "MM_RELATIVE_ASSET_DIR=${relative_asset_dir}" "MM_RELATIVE_ASSET_DIR_STD=${relative_asset_dir_std}"
        "MM_RELATIVE_ASSET_DIR_USER=${relative_asset_dir_user}" "MM_RELATIVE_ASSET_DIR_CACHE=${relative_asset_dir_cache}")

#####################################################
#####################  function  ####################
#####################################################
##########  render  ##########
AddModule("render" "${CMAKE_CURRENT_SOURCE_DIR}/function/render")
target_link_libraries(render PUBLIC math color log_system config_system file_system asset_system vulkan_lib glfw task_system)