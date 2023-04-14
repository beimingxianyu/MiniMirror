if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_compile_definitions("CHECK_PARAMETERS")
endif()

#shader config
set(shader_config_version "430")
set(shader_config_global_set "0")
set(shader_config_material_set "1")
set(shader_config_sampler_texture2D_binding "0")
set(shader_config_sampler_texture3D_binding "1")
set(shader_config_sampler_textureCube_binding "2")
set(shader_config_storage_texture2D_binding "3")
set(shader_config_storage_texture3D_binding "4")
set(shader_config_storage_textureCube_binding "5")
set(shader_config_storage_texture_format "rgba8")
add_compile_definitions(SHADER_VERSION=${shader_config_version} 
						SHADER_GLOBAL_SET=${shader_config_global_set}
						SHADER_MATERIAL_SET=${shader_config_material_set}
						SHADER_SAMPLER_TEXTURE2D_BINDING=${shader_config_sampler_texture2D_binding}
						SHADER_SAMPLER_TEXTURE3D_BINDING=${shader_config_sampler_texture3D_binding}
						SHADER_SAMPLER_TEXTURECUBE_BINDING=${shader_config_sampler_textureCube_binding}
						SHADER_STORAGE_TEXTURE2D_BINDING=${shader_config_storage_texture2D_binding}
						SHADER_STORAGE_TEXTURE3D_BINDING=${shader_config_storage_texture3D_binding}
						SHADER_STORAGE_TEXTURECUBE_BINDING=${shader_config_storage_textureCube_binding})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/config")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/asset")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rd_party")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/source")
