include("utils")

include_directories("${source_engine_dir}/source")

#####################  platform  ####################
##########  config_system  ##########
add_executable(config_system_test "${CMAKE_CURRENT_SOURCE_DIR}/platform/config_system/config_system_test.cpp")
target_include_directories(config_system_test PRIVATE "${source_engine_dir}/source/runtime/platform/config_system")
target_link_libraries(config_system_test PRIVATE GTest::gtest_main config_system)
target_compile_definitions(config_system_test PRIVATE "CONFIG_DIR=\"${config_dir}\"" "CONFIG_DIR_TEST=\"${config_dir_test}\"" "ENGINE_DIR=\"${engine_dir}\"" "BIN_DIR\"${bin_dir}\"")
# add_definitions("-DCONFIG_DIR=\"${config_dir_test}\"")
CopyDirSpecialFile("${source_test_file_dir_test}/config" "${config_dir}")

##########  file_system  ##########
add_executable(file_system_test "${CMAKE_CURRENT_SOURCE_DIR}/platform/file_system/file_system_test.cpp")
target_include_directories(file_system_test PRIVATE "${source_engine_dir}/source/runtime/platform/file_system")
target_include_directories(file_system_test PRIVATE "${source_engine_dir}/source")
target_link_libraries(file_system_test PRIVATE GTest::gtest_main file_system)
target_compile_definitions(file_system_test PRIVATE "ORIGINE_DIR=\"${test_file_dir_test}/file_system/origine\"")
CopyDir("${source_test_file_dir_test}/file_system" "${test_file_dir_test}")

#####################  core  ####################
##########  manager  ##########
AddExecutable("manager_test" "${CMAKE_CURRENT_SOURCE_DIR}/core/manager")
# add_executable(manager_test "${CMAKE_CURRENT_SOURCE_DIR}/core/manager/set_test.cpp")
target_link_libraries(manager_test PRIVATE GTest::gtest_main manager)
CopySharedLibraryToDestDirPostBuild(manager_test ${engine_dir}/bin manager ${engine_dir_test}/bin)

##########  asset_system ##########
AddExecutable("asset_system_test" "${CMAKE_CURRENT_SOURCE_DIR}/resource/asset_system")
target_link_libraries(asset_system_test PRIVATE asset_system GTest::gtest_main)
target_compile_definitions(asset_system_test PRIVATE "TEST_FILE_DIR_TEST=\"${test_file_dir_test}\"")
CopyDir("${source_test_file_dir_test}/asset_system" "${test_file_dir_test}")

include(GoogleTest)
gtest_discover_tests(config_system_test)
gtest_discover_tests(file_system_test)
gtest_discover_tests(manager_test)
gtest_discover_tests(asset_system_test)