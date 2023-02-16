include("utils")

include_directories("${source_engine_dir}/source")

#####################  platform  ####################
##########  config_system  ##########
add_executable(config_system_test "${CMAKE_CURRENT_SOURCE_DIR}/platform/config_system/config_system_test.cpp")
target_include_directories(config_system_test PRIVATE "${source_engine_dir}/source/runtime/platform/config_system")
target_link_libraries(config_system_test PRIVATE GTest::gtest_main config_system)
target_compile_definitions(config_system_test PRIVATE "CONFIG_DIR=\"${config_dir_test}\"")
# add_definitions("-DCONFIG_DIR=\"${config_dir_test}\"")
CopyDirSpecialFile("${source_test_file_dir_test}/config" "${config_dir_test}")
CopyFileToDestDirPostBuild(config_system_test ${engine_dir}/bin/config_system.dll ${engine_dir_test}/bin)

##########  file_system  ##########
add_executable(file_system_test "${CMAKE_CURRENT_SOURCE_DIR}/platform/file_system/file_system_test.cpp")
target_include_directories(file_system_test PRIVATE "${source_engine_dir}/source/runtime/platform/file_system")
target_include_directories(file_system_test PRIVATE "${source_engine_dir}/source")
target_link_libraries(file_system_test PRIVATE GTest::gtest_main file_system)
target_compile_definitions(file_system_test PRIVATE "ORIGINE_DIR=\"${test_file_dir_test}/file_system/origine\"")
CopyDir("${source_test_file_dir_test}/file_system" "${test_file_dir_test}")
CopyFileToDestDirPostBuild(file_system_test ${engine_dir}/bin/file_system.dll ${engine_dir_test}/bin)

include(GoogleTest)
gtest_discover_tests(config_system_test)