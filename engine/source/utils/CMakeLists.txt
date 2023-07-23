include("utils")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

AddModule("utils" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(utils PUBLIC rapidjson shaderc)
target_compile_definitions(utils PUBLIC "MM_HARDWARE_INFO_MAC_ADDRESS=${hardware_info_mac_address}" "MM_HARDWARE_INFO_MAC_ADDRESS_16=${hardware_info_mac_address_16}")