#########  glm  #########
set(glm_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glm/cmake/glm")
find_package(glm CONFIG REQUIRED)
if(NOT TARGET glm)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glm")
endif()

#########  spdlog  ########
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include")

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
endif()

if(NOT TARGET vulkan)
    set(vulkan_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/include")
    if (WIN32)
        set(vulkan_lib "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib")
        file(GLOB_RECURSE bin_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "VulkanSDK/bin/Win32/*")
        foreach(bin_file IN LISTS bin_files)
            file(COPY ${bin_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        endforeach()
    elseif (UNIX)
        set(vulkan_lib "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/Linux/libvulkan.so.1")
        file(GLOB_RECURSE bin_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "VulkanSDK/bin/Linux/*")
        foreach(bin_file IN LISTS bin_files)
            file(COPY ${bin_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        endforeach()
    elseif (APPLE)
        set(vulkan_lib "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/MacOs/libvulkan.1.dylib")
        file(GLOB_RECURSE bin_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "VulkanSDK/bin/MacOs/*")
        foreach(bin_file IN LISTS bin_files)
            file(COPY ${bin_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        endforeach()
    else()
        message(FATAL_ERROR "The platform is not supported.")
    endif()
endif()

if(NOT TARGET imgui)
    set(imgui_SOURCE_DIR_ "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
    file(GLOB imgui_sources CONFIGURE_DEPENDS  "${imgui_SOURCE_DIR_}/*.cpp")
    file(GLOB imgui_impl CONFIGURE_DEPENDS  
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.h")
    add_library(imgui STATIC ${imgui_sources} ${imgui_impl})
    target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR_}")
    target_include_directories(imgui PUBLIC "${vulkan_include_dir}")
    target_link_libraries(imgui PUBLIC glfw ${vulkan_lib})
endif()

if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
endif()

if(NOT TARGET stb)
    include(stb.cmake)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
endif()

#if(NOT TARGET Jolt)
#    option(TARGET_HELLO_WORLD "" OFF)
#    option(TARGET_PERFORMANCE_TEST "" OFF)
#    option(TARGET_SAMPLES "" OFF)
#    option(TARGET_UNIT_TESTS "" OFF)
#    option(TARGET_VIEWER "" OFF)
#
#    if(ENABLE_PHYSICS_DEBUG_RENDERER)
#        option(TARGET_TEST_FRAMEWORK "" ON)
#    else()
#        option(TARGET_TEST_FRAMEWORK "" OFF)
#    endif()
#
#    add_subdirectory(JoltPhysics/Build)
#
#    if(ENABLE_PHYSICS_DEBUG_RENDERER)
#       set_target_properties(Jolt TestFramework
#            PROPERTIES 
#            FOLDER ${third_party_folder}/JoltPhysics
#            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
#   else()
#        set_target_properties(Jolt
#            PROPERTIES 
#            FOLDER ${third_party_folder}/JoltPhysics
#            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
#    endif()

#endif()

if(NOT TARGET json11) 
    option(JSON11_BUILD_TESTS "" OFF)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/json11")
endif()