include("utils")
include("hardware_info")

add_executable(utils_test "${CMAKE_CURRENT_SOURCE_DIR}/utils_test.cpp")
target_include_directories(utils_test PRIVATE "${source_engine_dir}/source")
target_link_libraries(utils_test PUBLIC GTest::gtest_main utils)
target_compile_definitions(utils_test PUBLIC "HARDWARE_INFO_MAC_ADDRESS_16=${hardware_info_mac_address_16}")