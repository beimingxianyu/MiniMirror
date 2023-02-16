cmake_minimum_required (VERSION 3.8)

project ("MiniMirror")

option(enable_test "Whether to open unit test." ON)

if(WIN32) 
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/script/cmake")
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/bin")

set(version_major "0")
set(version_minor "1")
set(version_patch "1")

set(source_engine_dir "${CMAKE_CURRENT_SOURCE_DIR}/engine")
set(source_third_party_dir "${source_engine_dir}/3rd_party")
set(engine_dir "${CMAKE_BINARY_DIR}")
set(script_dir "${CMAKE_CURRENT_SOURCE_DIR}/script")
set(asset_dir "${engine_dir}/asset")
set(asset_dir_std "${asset_dir}/standard")
set(asset_dir_user "${asset_dir}/user")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/engine")

if (enable_test)
	set(engine_dir_test "${CMAKE_CURRENT_SOURCE_DIR}/build_test/${CMAKE_BUILD_TYPE}")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()