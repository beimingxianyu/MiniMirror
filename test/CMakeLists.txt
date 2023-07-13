project(MMTest)

set(CMAKE_BINARY_DIR "${engine_dir_test}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(config_dir_test "${engine_dir_test}/config")
set(source_test_file_dir_test "${CMAKE_CURRENT_SOURCE_DIR}/test_file")
set(test_file_dir_test "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../test_file")

#########  gtest  ########
# include_directories("${source_engine_dir}/3rd_party/gtest_x64-windows/include")

enable_testing()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
#####################  utils  ####################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utils")

#####################  runtime  ####################
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/runtime")
