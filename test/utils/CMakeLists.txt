include("utils")
include("hardware_info")


AddExecutable(utils_test "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(utils_test PRIVATE "${source_engine_dir}/source")
# target_link_libraries(utils_test PUBLIC utils GTest::gtest_main)
target_link_libraries(utils_test PUBLIC utils gtest_main)
target_compile_definitions(utils_test PUBLIC "HARDWARE_INFO_MAC_ADDRESS_16=${hardware_info_mac_address_16}")