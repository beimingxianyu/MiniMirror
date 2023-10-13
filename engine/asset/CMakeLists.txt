include("utils")

CopyDir("${CMAKE_CURRENT_SOURCE_DIR}/standard" "${asset_dir}")
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/user")
    CopyDir("${CMAKE_CURRENT_SOURCE_DIR}/user" "${asset_dir}")
endif()