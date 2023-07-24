#version 430

#extension GL_EXT_buffer_reference : require

#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_ARB_gpu_shader_int64: enable

#define VIRSION 430

#define STD_LAYOUT_IMP_IMP(std, version) std##version

#define STD_LAYOUT_IMP(version) STD_LAYOUT_IMP_IMP(std, version)

#define STD_LAYOUT STD_LAYOUT_IMP(VIRSION)

#define GLOBAL_SET 0

#define MATERIAL_SET 1

#define SAMPLER_TEXTURE2D_BINDING 0

#define SAMPLER_TEXTURE3D_BINDING 1

#define SAMPLER_TEXTURECUBE_BINDING 2

#define STORAGE_TEXTURE2D_BINDING 3

#define STORAGE_TEXTURE3D_BINDING 4

#define STORAGE_TEXTURECUBE_BINDING 5

#define STORAGE_TEXTURE_FORMAT rgba8

#define SAMPLER_TEXTURE2D_SET_AND_BINDING set = MATERIAL_SET, binding = SAMPLER_TEXTURE2D_BINDING

#define SAMPLER_TEXTURE3D_SET_AND_BINDING set = MATERIAL_SET, binding = SAMPLER_TEXTURE3D_BINDING

#define SAMPLER_TEXTURECUBE_SET_AND_BINDING set = MATERIAL_SET, binding = SAMPLER_TEXTURECUBE_BINDING

#define STORAGE_TEXTURE2D_SET_AND_BINDING set = MATERIAL_SET, binding = STORAGE_TEXTURE2D_BINDING

#define STORAGE_TEXTURE3D_SET_AND_BINDING set = MATERIAL_SET, binding = STORAGE_TEXTURE3D_BINDING

#define STORAGE_TEXTURECUBE_SET_AND_BINDING set = MATERIAL_SET, binding = STORAGE_TEXTURECUBE_BINDING

#define GET_UNIFORM_BUFFER_NAME(type_name) __uniform_buffer_register_##type_name##__

#define GET_STORAGE_BUFFER_NAME(type_name) __storage_buffer_register_##type_name##__

#define GET_TEXTURE2D_NAME(variable_name) __texture2D_buffer_register_##variable_name##__

#define GET_TEXTURE3D_NAME(variable_name) __texture3D_buffer_register_##variable_name##__

#define GET_TEXTURECUBE_NAME(variable_name) __texturecube_buffer_register_##variable_name##__

#define GET_STORAGE_TEXTURE2D_NAME(variable_name) __storage_texture2D_register_##variable_name##__

#define GET_STORAGE_TEXTURE3D_NAME(variable_name) __storage_texture3D_register_##variable_name##__

#define GET_STORAGE_TEXTURECUBE_NAME(variable_name) __storage_textureCUBE_register_##variable_name##__

#define __CONSTANT_TYPE_NAME__ Registers

#define __CONSTANT_VARIABLE_NAME__ __push_constant_registers__

#define __SAMPLER_TEXTURE2D_VECTOR_NAME__ __SamplerTexture2DVector__

#define __SAMPLER_TEXTURE3D_VECTOR_NAME__ __SamplerTexture3DVector__

#define __SAMPLER_TEXTURECUBE_VECTOR_NAME__ __SamplerTextureCUBEVector__

#define __STORAGE_TEXTURE2D_VECTOR_NAME__ __StorageImage2DVector__

#define __STORAGE_TEXTURE3D_VECTOR_NAME__ __StorageImage3DVector__

#define __STORAGE_TEXTURECUBE_VECTOR_NAME__ __StorageImageCUBEVector__

#define IMPORT_RESOURCE_START layout(push_constant) uniform __CONSTANT_TYPE_NAME__ {

#define IMPORT_RESOURCE_END } __CONSTANT_VARIABLE_NAME__;                                                           \
	layout(SAMPLER_TEXTURE2D_SET_AND_BINDING) uniform sampler2D __SAMPLER_TEXTURE2D_VECTOR_NAME__[];                         \
	layout(SAMPLER_TEXTURE3D_SET_AND_BINDING) uniform sampler3D __SAMPLER_TEXTURE3D_VECTOR_NAME__[];                         \
	layout(SAMPLER_TEXTURECUBE_SET_AND_BINDING) uniform samplerCube __SAMPLER_TEXTURECUBE_VECTOR_NAME__[];                   \
	layout(STORAGE_TEXTURE2D_SET_AND_BINDING, STORAGE_TEXTURE_FORMAT) uniform image2D __STORAGE_TEXTURE2D_VECTOR_NAME__[];       \
	layout(STORAGE_TEXTURE3D_SET_AND_BINDING, STORAGE_TEXTURE_FORMAT) uniform image3D __STORAGE_TEXTURE3D_VECTOR_NAME__[];       \
	layout(STORAGE_TEXTURECUBE_SET_AND_BINDING, STORAGE_TEXTURE_FORMAT) uniform imageCube __STORAGE_TEXTURECUBE_VECTOR_NAME__[]; \

#define IMPORT_UNIFORM_BUFFER(type_name) uint64_t GET_UNIFORM_BUFFER_NAME(type_name);

#define IMPORT_STORAGE_BUFFER(type_name) uint64_t GET_STORAGE_BUFFER_NAME(type_name);

#define IMPORT_TEXTURE2D(variable_name) uint64_t GET_TEXTURE2D_NAME(variable_name);

#define IMPORT_TEXTURE3D(variable_name) uint64_t GET_TEXTURE3D_NAME(variable_name);

#define IMPORT_TEXTURECUBE(variable_name) uint64_t GET_TEXTURECUBE_NAME(variable_name);

#define IMPORT_STORAGE_TEXTURE2D(variable_name) uint64_t GET_STORAGE_TEXTURE2D_NAME(variable_name);

#define IMPORT_STORAGE_TEXTURE3D(variable_name) uint64_t GET_STORAGE_TEXTURE2D_NAME(variable_name);

#define IMPORT_STORAGE_TEXTURECUBE(variable_name) uint64_t GET_STORAGE_TEXTURECUBE_NAME(variable_name);

#define REGISTER_UNIFORM_BUFFER(type_name, registe_struct) layout(STD_LAYOUT, buffer_reference) uniform type_name registe_struct GET_UNIFORM_BUFFER_NAME(type_name);

#define REGISTER_STORAGE_BUFFER(buffer_accsess, type_name, registe_struct) layout(STD_LAYOUT, buffer_reference) buffer_accsess buffer type_name registe_struct GET_STORAGE_BUFFER_NAME(type_name);

//#define REGISTER_TEXTURE2D(variable_name) layout(SAMPLER_TEXTURE_SET_AND_BINDING) uniform sampler2D GET_TEXTURE2D_NAME(variable_name);
//
//#define REGISTER_TEXTURE3D(variable_name) layout(SAMPLER_TEXTURE_SET_AND_BINDING) uniform sampler3D GET_TEXTURE3D_NAME(variable_name);
//
//#define REGISTER_TEXTURECUBE(variable_name) layout(SAMPLER_TEXTURE_SET_AND_BINDING) uniform samplerCube GET_TEXTURECUBE_NAME(variable_name);
//
//#define REGISTER_STORAGE_TEXTURE2D(buffer_accsess, variable_name) uniform buffer_accsess image2D GET_STORAGE_TEXTURE2D_NAME(variable_name)
//
//#define REGISTER_STORAGE_TEXTURE3D(buffer_accsess, variable_name) uniform buffer_accsess image3D GET_STORAGE_TEXTURE3D_NAME(variable_name)
//
//#define REGISTER_STORAGE_TEXTURECUBE(buffer_accsess, variable_name) uniform buffer_accsess imageCube GET_STORAGE_TEXTURECUBE_NAME(variable_name)

#define GET_UNIFORM_BUFFER(type_name) __CONSTANT_VARIABLE_NAME__.GET_UNIFORM_BUFFER_NAME(type_name)

#define GET_STORAGE_BUFFER(type_name) __CONSTANT_VARIABLE_NAME__.GET_STORAGE_BUFFER_NAME(type_name)

#define GET_TEXTURE2D(variable_name) __SAMPLER_TEXTURE2D_VECTOR_NAME__[uint(__CONSTANT_VARIABLE_NAME__.GET_TEXTURE2D(variable_name))]

#define GET_TEXTURE3D(varable_name) __SAMPLER_TEXTURE3D_VECTOR_NAME__[uint(__CONSTANT_VARIABLE_NAME__.GET_TEXTURE3D(variable_name))]

#define GET_TEXTURECUBE(varable_name) __SAMPLER_TEXTURECUBE_VECTOR_NAME__[uint(__CONSTANT_VARIABLE_NAME__.GET_TEXTURE3D(variable_name))]

#define GET_STORAGE_TEXTURE2D(variable_name) __STORAGE_TEXTURE2D_VECTOR_NAME__[uint(__CONSTANT_VARIABLE_NAME__.GET_STORAGE_TEXTURE2D_NAME(varable_name))]

#define GET_STORAGE_TEXTURE3D(variable_name) __STORAGE_TEXTURE3D_VECTOR_NAME__[uint(__CONSTANT_VARIABLE_NAME__.GET_STORAGE_TEXTURE3D_NAME(varable_name))]

#define GET_STORAGE_TEXTURECUBE(variable_name) __STORAGE_TEXTURECUBE_VECTOR_NAME__[uint(__CONSTANT_VARIABLE_NAME__.GET_STORAGE_TEXTURECUBE_NAME(varable_name))]

