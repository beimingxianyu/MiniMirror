#########  glm  #########
set(glm_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glm/cmake/glm")
find_package(glm CONFIG REQUIRED)
if (NOT TARGET glm)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glm")
endif ()

#########  spdlog  ########
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include")

if (NOT TARGET glfw)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
  add_subdirectory(glfw)
endif ()

if (NOT TARGET vulkan_lib)
  set(vulkan_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/include")
  set(vma_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/vulkanmemoryallocator/include")
  file(MAKE_DIRECTORY "${vulkan_include_dir}")
  file(MAKE_DIRECTORY "${vma_include_dir}")
  if (WIN32)
    # set(vulkan_lib "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib")
    add_library(vulkan_lib STATIC IMPORTED GLOBAL)
    set_target_properties(vulkan_lib PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib"
                )
    target_include_directories(vulkan_lib INTERFACE "${vma_include_dir};${vulkan_include_dir}")
    file(GLOB_RECURSE bin_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "VulkanSDK/bin/Win32/*")
    foreach (bin_file IN LISTS bin_files)
      file(COPY ${bin_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endforeach ()
  elseif (UNIX)
    # set(vulkan_lib "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/Linux/libvulkan.so.1")
    add_library(vulkan_lib STATIC IMPORTED GLOBAL)
    set_target_properties(vulkan_lib PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/Linux/libvulkan.so.1"
                )
    target_include_directories(vulkan_lib INTERFACE "${vma_include_dir};${vulkan_include_dir}")
    file(GLOB_RECURSE bin_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "VulkanSDK/bin/Linux/*")
    foreach (bin_file IN LISTS bin_files)
      file(COPY ${bin_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endforeach ()
  elseif (APPLE)
    # set(vulkan_lib "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/MacOs/libvulkan.1.dylib")
    add_library(vulkan_lib STATIC IMPORTED GLOBAL)
    set_target_properties(vulkan_lib PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/VulkanSDK/lib/MacOs/libvulkan.1.dylib"
                )
    target_include_directories(vulkan_lib INTERFACE "${vma_include_dir};${vulkan_include_dir}")
    file(GLOB_RECURSE bin_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "VulkanSDK/bin/MacOs/*")
    foreach (bin_file IN LISTS bin_files)
      file(COPY ${bin_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endforeach ()
  else ()
    message(FATAL_ERROR "The platform is not supported.")
  endif ()
endif ()

if (NOT TARGET imgui)
  set(imgui_SOURCE_DIR_ "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
  file(GLOB imgui_sources CONFIGURE_DEPENDS "${imgui_SOURCE_DIR_}/*.cpp")
  file(GLOB imgui_impl CONFIGURE_DEPENDS
            "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp"
            "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.h"
            "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.cpp"
            "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.h")
  add_library(imgui STATIC ${imgui_sources} ${imgui_impl})
  target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR_}")
  target_include_directories(imgui PUBLIC "${vulkan_include_dir}")
  target_link_libraries(imgui PUBLIC glfw ${vulkan_lib})
endif ()

if (NOT TARGET spdlog)
  set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
  set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
  add_subdirectory(spdlog)
endif ()

if (NOT TARGET stb)
  file(GLOB stb_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h")
  add_library(stb INTERFACE)
  target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
endif ()

if (NOT TARGET assimp::assimp)
  set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/assimp")
endif ()
#if(NOT TARGET Jolt)
#    option(TARGET_HELLO_WORLD OFF CACHE BOOL "" FORCE)
#    option(TARGET_PERFORMANCE_TEST OFF CACHE BOOL "" FORCE)
#    option(TARGET_SAMPLES OFF CACHE BOOL "" FORCE)
#    option(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
#    option(TARGET_VIEWER OFF CACHE BOOL "" FORCE)
#
#    if(ENABLE_PHYSICS_DEBUG_RENDERER)
#        option(TARGET_TEST_FRAMEWORK ON CACHE BOOL "" FORCE)
#    else()
#        option(TARGET_TEST_FRAMEWORK OFF CACHE BOOL "" FORCE)
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

if (NOT TARGET rapidjson)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/rapidjson")
endif ()

if (NOT TARGET Taskflow::Taskflow)
  set(Taskflow_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TaskFlow/lib/cmake/Taskflow")
  find_package(Taskflow REQUIRED)
  add_library(Taskflow_lib INTERFACE)
  target_link_libraries(Taskflow_lib INTERFACE Taskflow::Taskflow)
endif ()

if (NOT TARGET shaderc)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shaderc")
endif ()

if (NOT TARGET spirv_reflect)
  add_library(spirv_reflect STATIC ${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect/spirv_reflect.c ${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect/spirv_reflect.h)
  target_include_directories(spirv_reflect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect)
endif()

if (MM_ENABLE_TEST)
  if (NOT TARGET gtest_main)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gtest")
  endif ()
endif ()


