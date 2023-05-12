project(MMTest)

set(CMAKE_BINARY_DIR "${engine_dir_test}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(config_dir_test "${engine_dir_test}/config")
set(source_test_file_dir_test "${CMAKE_CURRENT_SOURCE_DIR}/test_file")
set(test_file_dir_test "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../test_file")

#########  gtest  ########
if (WIN32)
    set(GTest_DIR "${source_engine_dir}/3rd_party/gtest/x64-windows/share/gtest")
elseif (UNIX)
    set(GTest_DIR "${source_engine_dir}/3rd_party/gtest/linux/lib/cmake/GTest")
endif ()

find_package(GTest CONFIG REQUIRED)

# include_directories("${source_engine_dir}/3rd_party/gtest_x64-windows/include")

enable_testing()
#####################  utils  ####################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utils")

#####################  runtime  ####################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/runtime")
