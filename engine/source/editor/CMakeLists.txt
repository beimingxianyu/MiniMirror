include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")

add_executable(MiniMirror "${CMAKE_CURRENT_SOURCE_DIR}/MiniMirror.cpp")
target_link_libraries(MiniMirror PUBLIC log_system file_system config_system render)
