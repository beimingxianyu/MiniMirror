include("utils")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

AddModule("utils" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(utils PUBLIC rapidjson)