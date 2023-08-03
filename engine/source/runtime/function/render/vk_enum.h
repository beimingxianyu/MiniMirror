//
// Created by beimingxianyu on 23-7-8.
//

#pragma once

#include <cstdint>

namespace MM {
namespace RenderSystem {
enum class ResourceType {
  UNDEFINE,
  Texture,
  BUFFER,
  MESH_BUFFER,
  FRAME_BUFFER,
  CONSTANTS,
  STAGE_BUFFER,
  UNDEFINED
};

/**
 * \brief Memory operations allowed for rendering resources.
 */
enum class MemoryOperate { UNDEFINE, READ, WRITE, READ_AND_WRITE, UNDEFINED };

enum class CommandBufferType { UNDEFINE, GRAPH, COMPUTE, TRANSFORM, UNDEFINED };

enum class DescriptorType {
  UNDEFINE,

  SAMPLER_TEXTURE2D,
  SAMPLER_TEXTURE3D,
  SAMPLER_TEXTURECUBE,
  STORAGE_TEXTURE2D,
  STORAGE_TEXTURE3D,
  STORAGE_TEXTURECUBE,
};

enum class DynamicState : std::uint64_t {
  UNDEFINE,

  DYNAMIC_STATE_VIEWPORT = static_cast<std::uint64_t>(0x1) << 0,
  DYNAMIC_STATE_SCISSOR = static_cast<std::uint64_t>(0x1) << 1,
  DYNAMIC_STATE_LINE_WIDTH = static_cast<std::uint64_t>(0x1) << 2,
  DYNAMIC_STATE_DEPTH_BIAS = static_cast<std::uint64_t>(0x1) << 3,
  DYNAMIC_STATE_BLEND_CONSTANTS = static_cast<std::uint64_t>(0x1) << 4,
  DYNAMIC_STATE_DEPTH_BOUNDS = static_cast<std::uint64_t>(0x1) << 5,
  DYNAMIC_STATE_STENCIL_COMPARE_MASK = static_cast<std::uint64_t>(0x1) << 6,
  DYNAMIC_STATE_STENCIL_WRITE_MASK = static_cast<std::uint64_t>(0x1) << 7,
  DYNAMIC_STATE_STENCIL_REFERENCE = static_cast<std::uint64_t>(0x1) << 8,
  DYNAMIC_STATE_CULL_MODE = static_cast<std::uint64_t>(0x1) << 9,
  DYNAMIC_STATE_FRONT_FACE = static_cast<std::uint64_t>(0x1) << 10,
  DYNAMIC_STATE_PRIMITIVE_TOPOLOGY = static_cast<std::uint64_t>(0x1) << 11,
  DYNAMIC_STATE_VIEWPORT_WITH_COUNT = static_cast<std::uint64_t>(0x1) << 12,
  DYNAMIC_STATE_SCISSOR_WITH_COUNT = static_cast<std::uint64_t>(0x1) << 13,
  DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE = static_cast<std::uint64_t>(0x1)
                                              << 14,
  DYNAMIC_STATE_DEPTH_TEST_ENABLE = static_cast<std::uint64_t>(0x1) << 15,
  DYNAMIC_STATE_DEPTH_WRITE_ENABLE = static_cast<std::uint64_t>(0x1) << 16,
  DYNAMIC_STATE_DEPTH_COMPARE_OP = static_cast<std::uint64_t>(0x1) << 17,
  DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE = static_cast<std::uint64_t>(0x1)
                                           << 18,
  DYNAMIC_STATE_STENCIL_TEST_ENABLE = static_cast<std::uint64_t>(0x1) << 19,
  DYNAMIC_STATE_STENCIL_OP = static_cast<std::uint64_t>(0x1) << 20,
  DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE = static_cast<std::uint64_t>(0x1)
                                            << 21,
  DYNAMIC_STATE_DEPTH_BIAS_ENABLE = static_cast<std::uint64_t>(0x1) << 22,
  DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE = static_cast<std::uint64_t>(0x1)
                                           << 23,
  DYNAMIC_STATE_VIEWPORT_W_SCALING_NV = static_cast<std::uint64_t>(0x1) << 24,
  DYNAMIC_STATE_DISCARD_RECTANGLE_EXT = static_cast<std::uint64_t>(0x1) << 25,
  DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT = static_cast<std::uint64_t>(0x1) << 26,
  DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR =
      static_cast<std::uint64_t>(0x1) << 27,
  DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV =
      static_cast<std::uint64_t>(0x1) << 28,
  DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV =
      static_cast<std::uint64_t>(0x1) << 29,
  DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV = static_cast<std::uint64_t>(0x1) << 30,
  DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR = static_cast<std::uint64_t>(0x1)
                                            << 31,
  DYNAMIC_STATE_LINE_STIPPLE_EXT = static_cast<std::uint64_t>(0x1) << 32,
  DYNAMIC_STATE_VERTEX_INPUT_EXT = static_cast<std::uint64_t>(0x1) << 33,
  DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT = static_cast<std::uint64_t>(0x1)
                                           << 34,
  DYNAMIC_STATE_LOGIC_OP_EXT = static_cast<std::uint64_t>(0x1) << 35,
  DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT = static_cast<std::uint64_t>(0x1) << 36,
  DYNAMIC_STATE_CULL_MODE_EXT = DYNAMIC_STATE_CULL_MODE,
  DYNAMIC_STATE_FRONT_FACE_EXT = DYNAMIC_STATE_FRONT_FACE,
  DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT = DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
  DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT = DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
  DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT = DYNAMIC_STATE_SCISSOR_WITH_COUNT,
  DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT =
      DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE,
  DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT = DYNAMIC_STATE_DEPTH_TEST_ENABLE,
  DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT = DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
  DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT = DYNAMIC_STATE_DEPTH_COMPARE_OP,
  DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT =
      DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
  DYNAMIC_STATE_STENCIL_TEST_ENABLE_EXT = DYNAMIC_STATE_STENCIL_TEST_ENABLE,
  DYNAMIC_STATE_STENCIL_OP_EXT = DYNAMIC_STATE_STENCIL_OP,
  DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE_EXT =
      DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE,
  DYNAMIC_STATE_DEPTH_BIAS_ENABLE_EXT = DYNAMIC_STATE_DEPTH_BIAS_ENABLE,
  DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE_EXT =
      DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE,
  DYNAMIC_STATE_MAX_ENUM = static_cast<std::uint64_t>(0x1) << 37
};

DynamicState operator|(const DynamicState& lhs, const DynamicState& rhs);

DynamicState& operator|=(DynamicState& lhs, const DynamicState& rhs);

DynamicState operator&(const DynamicState& lhs, const DynamicState& rhs);

DynamicState& operator&=(DynamicState& lhs, const DynamicState& rhs);

// DEFAULT == ALL, but if
// MM::RenderSystem::PipelineLayout::pipeline_layout_type_ is DEFAULT, the
// MM::RenderSystem::PipelineLayout::pipeline_layout_ managed by
// MM::RenderSystem::DescriptorManager.
enum class PipelineLayoutType {
  UNDEFINE,
  GRAPHICS,
  COMPUTE,
  RAY,
  SUBPASS,
  ALL,
  DEFAULT
};

enum class ShaderStage {
  // GRAPHICS
  SHADER_STAGE_VERTEX_BIT = 0x00000001,
  SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
  SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
  SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
  SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
  SHADER_STAGE_TASK_BIT = 0x00000040,
  SHADER_STAGE_MESH_BIT = 0x00000080,
  SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,

  // COMPUTE
  SHADER_STAGE_COMPUTE_BIT = 0x00000020,

  // RAY
  SHADER_STAGE_RAYGEN_BIT = 0x00000100,
  SHADER_STAGE_ANY_HIT_BIT = 0x00000200,
  SHADER_STAGE_CLOSEST_HIT_BIT = 0x00000400,
  SHADER_STAGE_MISS_BIT = 0x00000800,
  SHADER_STAGE_INTERSECTION_BIT = 0x00001000,
  SHADER_STAGE_CALLABLE_BIT = 0x00002000,

  // SUBPASS
  SHADER_STAGE_SUBPASS_SHADING_BIT = 0x00004000,

  SHADER_STAGE_ALL = 0x7FFFFFFF,
};

ShaderStage operator|(const ShaderStage& lhs, const ShaderStage& rhs);

ShaderStage& operator|=(ShaderStage& lhs, const ShaderStage& rhs);

ShaderStage operator&(const ShaderStage& lhs, const ShaderStage& rhs);

ShaderStage& operator&=(ShaderStage& lhs, const ShaderStage& rhs);

enum class ShaderSlotCount : std::uint64_t {
  UNDEFINE = 0,

  // GRAPHICS
  SHADER_STAGE_VERTEX_0_SLOT_BIT = (0x1 << 0) + (0x0 << 0),
  SHADER_STAGE_VERTEX_1_SLOT_BIT = (0x1 << 0) + (0x1 << 0),
  SHADER_STAGE_VERTEX_2_SLOT_BIT = (0x1 << 0) + (0x2 << 0),
  SHADER_STAGE_VERTEX_3_SLOT_BIT = (0x1 << 0) + (0x3 << 0),
  SHADER_STAGE_VERTEX_4_SLOT_BIT = (0x1 << 0) + (0x4 << 0),
  SHADER_STAGE_VERTEX_5_SLOT_BIT = (0x1 << 0) + (0x5 << 0),
  SHADER_STAGE_VERTEX_6_SLOT_BIT = (0x1 << 0) + (0x6 << 0),
  SHADER_STAGE_VERTEX_7_SLOT_BIT = (0x1 << 0) + (0x7 << 0),
  SHADER_STAGE_VERTEX_8_SLOT_BIT = (0x1 << 0) + (0x8 << 0),
  SHADER_STAGE_VERTEX_9_SLOT_BIT = (0x1 << 0) + (0x9 << 0),
  SHADER_STAGE_VERTEX_10_SLOT_BIT = (0x1 << 0) + (0xA << 0),
  SHADER_STAGE_VERTEX_11_SLOT_BIT = (0x1 << 0) + (0xB << 0),
  SHADER_STAGE_VERTEX_12_SLOT_BIT = (0x1 << 0) + (0xC << 0),
  SHADER_STAGE_VERTEX_13_SLOT_BIT = (0x1 << 0) + (0xD << 0),
  SHADER_STAGE_VERTEX_14_SLOT_BIT = (0x1 << 0) + (0xE << 0),
  SHADER_STAGE_VERTEX_15_SLOT_BIT = (0x1 << 0) + (0xF << 0),
  SHADER_STAGE_VERTEX_16_SLOT_BIT = (0x1 << 0) + (0x10 << 0),
  SHADER_STAGE_TESSELLATION_CONTROL_0_SLOT_BIT = (0x1 << 5) + (0x0 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_1_SLOT_BIT = (0x1 << 5) + (0x1 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_2_SLOT_BIT = (0x1 << 5) + (0x2 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_3_SLOT_BIT = (0x1 << 5) + (0x3 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_4_SLOT_BIT = (0x1 << 5) + (0x4 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_5_SLOT_BIT = (0x1 << 5) + (0x5 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_6_SLOT_BIT = (0x1 << 5) + (0x6 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_7_SLOT_BIT = (0x1 << 5) + (0x7 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_8_SLOT_BIT = (0x1 << 5) + (0x8 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_9_SLOT_BIT = (0x1 << 5) + (0x9 << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_10_SLOT_BIT = (0x1 << 5) + (0xA << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_11_SLOT_BIT = (0x1 << 5) + (0xB << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_12_SLOT_BIT = (0x1 << 5) + (0xC << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_13_SLOT_BIT = (0x1 << 5) + (0xD << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_14_SLOT_BIT = (0x1 << 5) + (0xE << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_15_SLOT_BIT = (0x1 << 5) + (0xF << 5),
  SHADER_STAGE_TESSELLATION_CONTROL_16_SLOT_BIT = (0x1 << 5) + (0x10 << 5),
  SHADER_STAGE_TESSELLATION_EVALUATION_0_SLOT_BIT = (0x1 << 10) + (0x0 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_1_SLOT_BIT = (0x1 << 10) + (0x1 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_2_SLOT_BIT = (0x1 << 10) + (0x2 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_3_SLOT_BIT = (0x1 << 10) + (0x3 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_4_SLOT_BIT = (0x1 << 10) + (0x4 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_5_SLOT_BIT = (0x1 << 10) + (0x5 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_6_SLOT_BIT = (0x1 << 10) + (0x6 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_7_SLOT_BIT = (0x1 << 10) + (0x7 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_8_SLOT_BIT = (0x1 << 10) + (0x8 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_9_SLOT_BIT = (0x1 << 10) + (0x9 << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_10_SLOT_BIT = (0x1 << 10) + (0xA << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_11_SLOT_BIT = (0x1 << 10) + (0xB << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_12_SLOT_BIT = (0x1 << 10) + (0xC << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_13_SLOT_BIT = (0x1 << 10) + (0xD << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_14_SLOT_BIT = (0x1 << 10) + (0xE << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_15_SLOT_BIT = (0x1 << 10) + (0xF << 10),
  SHADER_STAGE_TESSELLATION_EVALUATION_16_SLOT_BIT = (0x1 << 10) + (0x10 << 10),
  SHADER_STAGE_GEOMETRY_0_SLOT_BIT = (0x1 << 15) + (0x0 << 15),
  SHADER_STAGE_GEOMETRY_1_SLOT_BIT = (0x1 << 15) + (0x1 << 15),
  SHADER_STAGE_GEOMETRY_2_SLOT_BIT = (0x1 << 15) + (0x2 << 15),
  SHADER_STAGE_GEOMETRY_3_SLOT_BIT = (0x1 << 15) + (0x3 << 15),
  SHADER_STAGE_GEOMETRY_4_SLOT_BIT = (0x1 << 15) + (0x4 << 15),
  SHADER_STAGE_GEOMETRY_5_SLOT_BIT = (0x1 << 15) + (0x5 << 15),
  SHADER_STAGE_GEOMETRY_6_SLOT_BIT = (0x1 << 15) + (0x6 << 15),
  SHADER_STAGE_GEOMETRY_7_SLOT_BIT = (0x1 << 15) + (0x7 << 15),
  SHADER_STAGE_GEOMETRY_8_SLOT_BIT = (0x1 << 15) + (0x8 << 15),
  SHADER_STAGE_GEOMETRY_9_SLOT_BIT = (0x1 << 15) + (0x9 << 15),
  SHADER_STAGE_GEOMETRY_10_SLOT_BIT = (0x1 << 15) + (0xA << 15),
  SHADER_STAGE_GEOMETRY_11_SLOT_BIT = (0x1 << 15) + (0xB << 15),
  SHADER_STAGE_GEOMETRY_12_SLOT_BIT = (0x1 << 15) + (0xC << 15),
  SHADER_STAGE_GEOMETRY_13_SLOT_BIT = (0x1 << 15) + (0xD << 15),
  SHADER_STAGE_GEOMETRY_14_SLOT_BIT = (0x1 << 15) + (0xE << 15),
  SHADER_STAGE_GEOMETRY_15_SLOT_BIT = (0x1 << 15) + (0xF << 15),
  SHADER_STAGE_GEOMETRY_16_SLOT_BIT = (0x1 << 15) + (0x10 << 15),
  SHADER_STAGE_FRAGMENT_0_SLOT_BIT = (0x1 << 20) + (0x0 << 20),
  SHADER_STAGE_FRAGMENT_1_SLOT_BIT = (0x1 << 20) + (0x1 << 20),
  SHADER_STAGE_FRAGMENT_2_SLOT_BIT = (0x1 << 20) + (0x2 << 20),
  SHADER_STAGE_FRAGMENT_3_SLOT_BIT = (0x1 << 20) + (0x3 << 20),
  SHADER_STAGE_FRAGMENT_4_SLOT_BIT = (0x1 << 20) + (0x4 << 20),
  SHADER_STAGE_FRAGMENT_5_SLOT_BIT = (0x1 << 20) + (0x5 << 20),
  SHADER_STAGE_FRAGMENT_6_SLOT_BIT = (0x1 << 20) + (0x6 << 20),
  SHADER_STAGE_FRAGMENT_7_SLOT_BIT = (0x1 << 20) + (0x7 << 20),
  SHADER_STAGE_FRAGMENT_8_SLOT_BIT = (0x1 << 20) + (0x8 << 20),
  SHADER_STAGE_FRAGMENT_9_SLOT_BIT = (0x1 << 20) + (0x9 << 20),
  SHADER_STAGE_FRAGMENT_10_SLOT_BIT = (0x1 << 20) + (0xA << 20),
  SHADER_STAGE_FRAGMENT_11_SLOT_BIT = (0x1 << 20) + (0xB << 20),
  SHADER_STAGE_FRAGMENT_12_SLOT_BIT = (0x1 << 20) + (0xC << 20),
  SHADER_STAGE_FRAGMENT_13_SLOT_BIT = (0x1 << 20) + (0xD << 20),
  SHADER_STAGE_FRAGMENT_14_SLOT_BIT = (0x1 << 20) + (0xE << 20),
  SHADER_STAGE_FRAGMENT_15_SLOT_BIT = (0x1 << 20) + (0xF << 20),
  SHADER_STAGE_FRAGMENT_16_SLOT_BIT = (0x1 << 20) + (0x10 << 20),
  SHADER_STAGE_TASK_0_SLOT_BIT = (0x1 << 25) + (0x0 << 25),
  SHADER_STAGE_TASK_1_SLOT_BIT = (0x1 << 25) + (0x1 << 25),
  SHADER_STAGE_TASK_2_SLOT_BIT = (0x1 << 25) + (0x2 << 25),
  SHADER_STAGE_TASK_3_SLOT_BIT = (0x1 << 25) + (0x3 << 25),
  SHADER_STAGE_TASK_4_SLOT_BIT = (0x1 << 25) + (0x4 << 25),
  SHADER_STAGE_TASK_5_SLOT_BIT = (0x1 << 25) + (0x5 << 25),
  SHADER_STAGE_TASK_6_SLOT_BIT = (0x1 << 25) + (0x6 << 25),
  SHADER_STAGE_TASK_7_SLOT_BIT = (0x1 << 25) + (0x7 << 25),
  SHADER_STAGE_TASK_8_SLOT_BIT = (0x1 << 25) + (0x8 << 25),
  SHADER_STAGE_TASK_9_SLOT_BIT = (0x1 << 25) + (0x9 << 25),
  SHADER_STAGE_TASK_10_SLOT_BIT = (0x1 << 25) + (0xA << 25),
  SHADER_STAGE_TASK_11_SLOT_BIT = (0x1 << 25) + (0xB << 25),
  SHADER_STAGE_TASK_12_SLOT_BIT = (0x1 << 25) + (0xC << 25),
  SHADER_STAGE_TASK_13_SLOT_BIT = (0x1 << 25) + (0xD << 25),
  SHADER_STAGE_TASK_14_SLOT_BIT = (0x1 << 25) + (0xE << 25),
  SHADER_STAGE_TASK_15_SLOT_BIT = (0x1 << 25) + (0xF << 25),
  SHADER_STAGE_TASK_16_SLOT_BIT = (0x1 << 25) + (0x10 << 25),
  SHADER_STAGE_MESH_0_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x0) << 30),
  SHADER_STAGE_MESH_1_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x1) << 30),
  SHADER_STAGE_MESH_2_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x2) << 30),
  SHADER_STAGE_MESH_3_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x3) << 30),
  SHADER_STAGE_MESH_4_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x4) << 30),
  SHADER_STAGE_MESH_5_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x5) << 30),
  SHADER_STAGE_MESH_6_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x6) << 30),
  SHADER_STAGE_MESH_7_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x7) << 30),
  SHADER_STAGE_MESH_8_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x8) << 30),
  SHADER_STAGE_MESH_9_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                 (static_cast<std::uint64_t>(0x9) << 30),
  SHADER_STAGE_MESH_10_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                  (static_cast<std::uint64_t>(0xA) << 30),
  SHADER_STAGE_MESH_11_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                  (static_cast<std::uint64_t>(0xB) << 30),
  SHADER_STAGE_MESH_12_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                  (static_cast<std::uint64_t>(0xC) << 30),
  SHADER_STAGE_MESH_13_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                  (static_cast<std::uint64_t>(0xD) << 30),
  SHADER_STAGE_MESH_14_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                  (static_cast<std::uint64_t>(0xE) << 30),
  SHADER_STAGE_MESH_15_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                  (static_cast<std::uint64_t>(0xF) << 30),
  SHADER_STAGE_MESH_16_SLOT_BIT = (static_cast<std::uint64_t>(0x1) << 30) +
                                  (static_cast<std::uint64_t>(0x10) << 30),
  SHADER_STAGE_ALL_GRAPHICS_0_SLOT_BIT =
      SHADER_STAGE_VERTEX_0_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_0_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_0_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_0_SLOT_BIT | SHADER_STAGE_FRAGMENT_0_SLOT_BIT |
      SHADER_STAGE_TASK_0_SLOT_BIT | SHADER_STAGE_MESH_0_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT =
      SHADER_STAGE_VERTEX_1_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_1_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_1_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_1_SLOT_BIT | SHADER_STAGE_FRAGMENT_1_SLOT_BIT |
      SHADER_STAGE_TASK_1_SLOT_BIT | SHADER_STAGE_MESH_1_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT =
      SHADER_STAGE_VERTEX_2_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_2_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_2_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_2_SLOT_BIT | SHADER_STAGE_FRAGMENT_2_SLOT_BIT |
      SHADER_STAGE_TASK_2_SLOT_BIT | SHADER_STAGE_MESH_2_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT =
      SHADER_STAGE_VERTEX_3_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_3_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_3_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_3_SLOT_BIT | SHADER_STAGE_FRAGMENT_3_SLOT_BIT |
      SHADER_STAGE_TASK_3_SLOT_BIT | SHADER_STAGE_MESH_3_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT =
      SHADER_STAGE_VERTEX_4_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_4_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_4_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_4_SLOT_BIT | SHADER_STAGE_FRAGMENT_4_SLOT_BIT |
      SHADER_STAGE_TASK_4_SLOT_BIT | SHADER_STAGE_MESH_4_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT =
      SHADER_STAGE_VERTEX_5_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_5_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_5_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_5_SLOT_BIT | SHADER_STAGE_FRAGMENT_5_SLOT_BIT |
      SHADER_STAGE_TASK_5_SLOT_BIT | SHADER_STAGE_MESH_5_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT =
      SHADER_STAGE_VERTEX_6_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_6_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_6_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_6_SLOT_BIT | SHADER_STAGE_FRAGMENT_6_SLOT_BIT |
      SHADER_STAGE_TASK_6_SLOT_BIT | SHADER_STAGE_MESH_6_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT =
      SHADER_STAGE_VERTEX_7_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_7_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_7_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_7_SLOT_BIT | SHADER_STAGE_FRAGMENT_7_SLOT_BIT |
      SHADER_STAGE_TASK_7_SLOT_BIT | SHADER_STAGE_MESH_7_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT =
      SHADER_STAGE_VERTEX_8_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_8_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_8_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_8_SLOT_BIT | SHADER_STAGE_FRAGMENT_8_SLOT_BIT |
      SHADER_STAGE_TASK_8_SLOT_BIT | SHADER_STAGE_MESH_8_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT =
      SHADER_STAGE_VERTEX_9_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_9_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_9_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_9_SLOT_BIT | SHADER_STAGE_FRAGMENT_9_SLOT_BIT |
      SHADER_STAGE_TASK_9_SLOT_BIT | SHADER_STAGE_MESH_9_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT =
      SHADER_STAGE_VERTEX_10_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_10_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_10_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_10_SLOT_BIT | SHADER_STAGE_FRAGMENT_10_SLOT_BIT |
      SHADER_STAGE_TASK_10_SLOT_BIT | SHADER_STAGE_MESH_10_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT =
      SHADER_STAGE_VERTEX_11_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_11_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_11_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_11_SLOT_BIT | SHADER_STAGE_FRAGMENT_11_SLOT_BIT |
      SHADER_STAGE_TASK_11_SLOT_BIT | SHADER_STAGE_MESH_11_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT =
      SHADER_STAGE_VERTEX_12_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_12_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_12_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_12_SLOT_BIT | SHADER_STAGE_FRAGMENT_12_SLOT_BIT |
      SHADER_STAGE_TASK_12_SLOT_BIT | SHADER_STAGE_MESH_12_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT =
      SHADER_STAGE_VERTEX_13_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_13_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_13_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_13_SLOT_BIT | SHADER_STAGE_FRAGMENT_13_SLOT_BIT |
      SHADER_STAGE_TASK_13_SLOT_BIT | SHADER_STAGE_MESH_13_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT =
      SHADER_STAGE_VERTEX_14_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_14_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_14_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_14_SLOT_BIT | SHADER_STAGE_FRAGMENT_14_SLOT_BIT |
      SHADER_STAGE_TASK_14_SLOT_BIT | SHADER_STAGE_MESH_14_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT =
      SHADER_STAGE_VERTEX_15_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_15_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_15_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_15_SLOT_BIT | SHADER_STAGE_FRAGMENT_15_SLOT_BIT |
      SHADER_STAGE_TASK_15_SLOT_BIT | SHADER_STAGE_MESH_15_SLOT_BIT,
  SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT =
      SHADER_STAGE_VERTEX_16_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_CONTROL_16_SLOT_BIT |
      SHADER_STAGE_TESSELLATION_EVALUATION_16_SLOT_BIT |
      SHADER_STAGE_GEOMETRY_16_SLOT_BIT | SHADER_STAGE_FRAGMENT_16_SLOT_BIT |
      SHADER_STAGE_TASK_16_SLOT_BIT | SHADER_STAGE_MESH_16_SLOT_BIT,

  // COMPUTE
  SHADER_STAGE_COMPUTE_0_SLOT_BIT = SHADER_STAGE_VERTEX_0_SLOT_BIT,
  SHADER_STAGE_COMPUTE_1_SLOT_BIT = SHADER_STAGE_VERTEX_1_SLOT_BIT,
  SHADER_STAGE_COMPUTE_2_SLOT_BIT = SHADER_STAGE_VERTEX_2_SLOT_BIT,
  SHADER_STAGE_COMPUTE_3_SLOT_BIT = SHADER_STAGE_VERTEX_3_SLOT_BIT,
  SHADER_STAGE_COMPUTE_4_SLOT_BIT = SHADER_STAGE_VERTEX_4_SLOT_BIT,
  SHADER_STAGE_COMPUTE_5_SLOT_BIT = SHADER_STAGE_VERTEX_5_SLOT_BIT,
  SHADER_STAGE_COMPUTE_6_SLOT_BIT = SHADER_STAGE_VERTEX_6_SLOT_BIT,
  SHADER_STAGE_COMPUTE_7_SLOT_BIT = SHADER_STAGE_VERTEX_7_SLOT_BIT,
  SHADER_STAGE_COMPUTE_8_SLOT_BIT = SHADER_STAGE_VERTEX_8_SLOT_BIT,
  SHADER_STAGE_COMPUTE_9_SLOT_BIT = SHADER_STAGE_VERTEX_9_SLOT_BIT,
  SHADER_STAGE_COMPUTE_10_SLOT_BIT = SHADER_STAGE_VERTEX_10_SLOT_BIT,
  SHADER_STAGE_COMPUTE_11_SLOT_BIT = SHADER_STAGE_VERTEX_11_SLOT_BIT,
  SHADER_STAGE_COMPUTE_12_SLOT_BIT = SHADER_STAGE_VERTEX_12_SLOT_BIT,
  SHADER_STAGE_COMPUTE_13_SLOT_BIT = SHADER_STAGE_VERTEX_13_SLOT_BIT,
  SHADER_STAGE_COMPUTE_14_SLOT_BIT = SHADER_STAGE_VERTEX_14_SLOT_BIT,
  SHADER_STAGE_COMPUTE_15_SLOT_BIT = SHADER_STAGE_VERTEX_15_SLOT_BIT,
  SHADER_STAGE_COMPUTE_16_SLOT_BIT = SHADER_STAGE_VERTEX_16_SLOT_BIT,

  // Ray
  SHADER_STAGE_RAYGEN_0_SLOT_BIT = SHADER_STAGE_VERTEX_0_SLOT_BIT,
  SHADER_STAGE_RAYGEN_1_SLOT_BIT = SHADER_STAGE_VERTEX_1_SLOT_BIT,
  SHADER_STAGE_RAYGEN_2_SLOT_BIT = SHADER_STAGE_VERTEX_2_SLOT_BIT,
  SHADER_STAGE_RAYGEN_3_SLOT_BIT = SHADER_STAGE_VERTEX_3_SLOT_BIT,
  SHADER_STAGE_RAYGEN_4_SLOT_BIT = SHADER_STAGE_VERTEX_4_SLOT_BIT,
  SHADER_STAGE_RAYGEN_5_SLOT_BIT = SHADER_STAGE_VERTEX_5_SLOT_BIT,
  SHADER_STAGE_RAYGEN_6_SLOT_BIT = SHADER_STAGE_VERTEX_6_SLOT_BIT,
  SHADER_STAGE_RAYGEN_7_SLOT_BIT = SHADER_STAGE_VERTEX_7_SLOT_BIT,
  SHADER_STAGE_RAYGEN_8_SLOT_BIT = SHADER_STAGE_VERTEX_8_SLOT_BIT,
  SHADER_STAGE_RAYGEN_9_SLOT_BIT = SHADER_STAGE_VERTEX_9_SLOT_BIT,
  SHADER_STAGE_RAYGEN_10_SLOT_BIT = SHADER_STAGE_VERTEX_10_SLOT_BIT,
  SHADER_STAGE_RAYGEN_11_SLOT_BIT = SHADER_STAGE_VERTEX_11_SLOT_BIT,
  SHADER_STAGE_RAYGEN_12_SLOT_BIT = SHADER_STAGE_VERTEX_12_SLOT_BIT,
  SHADER_STAGE_RAYGEN_13_SLOT_BIT = SHADER_STAGE_VERTEX_13_SLOT_BIT,
  SHADER_STAGE_RAYGEN_14_SLOT_BIT = SHADER_STAGE_VERTEX_14_SLOT_BIT,
  SHADER_STAGE_RAYGEN_15_SLOT_BIT = SHADER_STAGE_VERTEX_15_SLOT_BIT,
  SHADER_STAGE_RAYGEN_16_SLOT_BIT = SHADER_STAGE_VERTEX_16_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_0_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_0_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_1_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_1_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_2_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_2_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_3_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_3_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_4_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_4_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_5_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_5_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_6_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_6_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_7_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_7_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_8_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_8_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_9_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_9_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_10_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_10_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_11_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_11_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_12_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_12_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_13_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_13_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_14_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_14_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_15_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_15_SLOT_BIT,
  SHADER_STAGE_ANY_HIT_16_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_CONTROL_16_SLOT_BIT,
  SHADER_STAGE_CLOSEST_0_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_0_SLOT_BIT,
  SHADER_STAGE_CLOSEST_1_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_1_SLOT_BIT,
  SHADER_STAGE_CLOSEST_2_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_2_SLOT_BIT,
  SHADER_STAGE_CLOSEST_3_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_3_SLOT_BIT,
  SHADER_STAGE_CLOSEST_4_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_4_SLOT_BIT,
  SHADER_STAGE_CLOSEST_5_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_5_SLOT_BIT,
  SHADER_STAGE_CLOSEST_6_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_6_SLOT_BIT,
  SHADER_STAGE_CLOSEST_7_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_7_SLOT_BIT,
  SHADER_STAGE_CLOSEST_8_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_8_SLOT_BIT,
  SHADER_STAGE_CLOSEST_9_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_9_SLOT_BIT,
  SHADER_STAGE_CLOSEST_10_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_10_SLOT_BIT,
  SHADER_STAGE_CLOSEST_11_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_11_SLOT_BIT,
  SHADER_STAGE_CLOSEST_12_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_12_SLOT_BIT,
  SHADER_STAGE_CLOSEST_13_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_13_SLOT_BIT,
  SHADER_STAGE_CLOSEST_14_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_14_SLOT_BIT,
  SHADER_STAGE_CLOSEST_15_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_15_SLOT_BIT,
  SHADER_STAGE_CLOSEST_16_SLOT_BIT =
      SHADER_STAGE_TESSELLATION_EVALUATION_16_SLOT_BIT,
  SHADER_STAGE_MISS_0_SLOT_BIT = SHADER_STAGE_GEOMETRY_0_SLOT_BIT,
  SHADER_STAGE_MISS_1_SLOT_BIT = SHADER_STAGE_GEOMETRY_1_SLOT_BIT,
  SHADER_STAGE_MISS_2_SLOT_BIT = SHADER_STAGE_GEOMETRY_2_SLOT_BIT,
  SHADER_STAGE_MISS_3_SLOT_BIT = SHADER_STAGE_GEOMETRY_3_SLOT_BIT,
  SHADER_STAGE_MISS_4_SLOT_BIT = SHADER_STAGE_GEOMETRY_4_SLOT_BIT,
  SHADER_STAGE_MISS_5_SLOT_BIT = SHADER_STAGE_GEOMETRY_5_SLOT_BIT,
  SHADER_STAGE_MISS_6_SLOT_BIT = SHADER_STAGE_GEOMETRY_6_SLOT_BIT,
  SHADER_STAGE_MISS_7_SLOT_BIT = SHADER_STAGE_GEOMETRY_7_SLOT_BIT,
  SHADER_STAGE_MISS_8_SLOT_BIT = SHADER_STAGE_GEOMETRY_8_SLOT_BIT,
  SHADER_STAGE_MISS_9_SLOT_BIT = SHADER_STAGE_GEOMETRY_9_SLOT_BIT,
  SHADER_STAGE_MISS_10_SLOT_BIT = SHADER_STAGE_GEOMETRY_10_SLOT_BIT,
  SHADER_STAGE_MISS_11_SLOT_BIT = SHADER_STAGE_GEOMETRY_11_SLOT_BIT,
  SHADER_STAGE_MISS_12_SLOT_BIT = SHADER_STAGE_GEOMETRY_12_SLOT_BIT,
  SHADER_STAGE_MISS_13_SLOT_BIT = SHADER_STAGE_GEOMETRY_13_SLOT_BIT,
  SHADER_STAGE_MISS_14_SLOT_BIT = SHADER_STAGE_GEOMETRY_14_SLOT_BIT,
  SHADER_STAGE_MISS_15_SLOT_BIT = SHADER_STAGE_GEOMETRY_15_SLOT_BIT,
  SHADER_STAGE_MISS_16_SLOT_BIT = SHADER_STAGE_GEOMETRY_16_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_0_SLOT_BIT = SHADER_STAGE_FRAGMENT_0_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_1_SLOT_BIT = SHADER_STAGE_FRAGMENT_1_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_2_SLOT_BIT = SHADER_STAGE_FRAGMENT_2_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_3_SLOT_BIT = SHADER_STAGE_FRAGMENT_3_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_4_SLOT_BIT = SHADER_STAGE_FRAGMENT_4_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_5_SLOT_BIT = SHADER_STAGE_FRAGMENT_5_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_6_SLOT_BIT = SHADER_STAGE_FRAGMENT_6_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_7_SLOT_BIT = SHADER_STAGE_FRAGMENT_7_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_8_SLOT_BIT = SHADER_STAGE_FRAGMENT_8_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_9_SLOT_BIT = SHADER_STAGE_FRAGMENT_9_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_10_SLOT_BIT = SHADER_STAGE_FRAGMENT_10_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_11_SLOT_BIT = SHADER_STAGE_FRAGMENT_11_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_12_SLOT_BIT = SHADER_STAGE_FRAGMENT_12_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_13_SLOT_BIT = SHADER_STAGE_FRAGMENT_13_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_14_SLOT_BIT = SHADER_STAGE_FRAGMENT_14_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_15_SLOT_BIT = SHADER_STAGE_FRAGMENT_15_SLOT_BIT,
  SHADER_STAGE_INTERSECTION_16_SLOT_BIT = SHADER_STAGE_FRAGMENT_16_SLOT_BIT,
  SHADER_STAGE_CALLABLE_0_SLOT_BIT = SHADER_STAGE_TASK_0_SLOT_BIT,
  SHADER_STAGE_CALLABLE_1_SLOT_BIT = SHADER_STAGE_TASK_1_SLOT_BIT,
  SHADER_STAGE_CALLABLE_2_SLOT_BIT = SHADER_STAGE_TASK_2_SLOT_BIT,
  SHADER_STAGE_CALLABLE_3_SLOT_BIT = SHADER_STAGE_TASK_3_SLOT_BIT,
  SHADER_STAGE_CALLABLE_4_SLOT_BIT = SHADER_STAGE_TASK_4_SLOT_BIT,
  SHADER_STAGE_CALLABLE_5_SLOT_BIT = SHADER_STAGE_TASK_5_SLOT_BIT,
  SHADER_STAGE_CALLABLE_6_SLOT_BIT = SHADER_STAGE_TASK_6_SLOT_BIT,
  SHADER_STAGE_CALLABLE_7_SLOT_BIT = SHADER_STAGE_TASK_7_SLOT_BIT,
  SHADER_STAGE_CALLABLE_8_SLOT_BIT = SHADER_STAGE_TASK_8_SLOT_BIT,
  SHADER_STAGE_CALLABLE_9_SLOT_BIT = SHADER_STAGE_TASK_9_SLOT_BIT,
  SHADER_STAGE_CALLABLE_10_SLOT_BIT = SHADER_STAGE_TASK_10_SLOT_BIT,
  SHADER_STAGE_CALLABLE_11_SLOT_BIT = SHADER_STAGE_TASK_11_SLOT_BIT,
  SHADER_STAGE_CALLABLE_12_SLOT_BIT = SHADER_STAGE_TASK_12_SLOT_BIT,
  SHADER_STAGE_CALLABLE_13_SLOT_BIT = SHADER_STAGE_TASK_13_SLOT_BIT,
  SHADER_STAGE_CALLABLE_14_SLOT_BIT = SHADER_STAGE_TASK_14_SLOT_BIT,
  SHADER_STAGE_CALLABLE_15_SLOT_BIT = SHADER_STAGE_TASK_15_SLOT_BIT,
  SHADER_STAGE_CALLABLE_16_SLOT_BIT = SHADER_STAGE_TASK_16_SLOT_BIT,

  // SUBPASS
  SHADER_STAGE_SUBPASS_SHADING_0_SLOT_BIT = SHADER_STAGE_VERTEX_0_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_1_SLOT_BIT = SHADER_STAGE_VERTEX_1_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_2_SLOT_BIT = SHADER_STAGE_VERTEX_2_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_3_SLOT_BIT = SHADER_STAGE_VERTEX_3_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_4_SLOT_BIT = SHADER_STAGE_VERTEX_4_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_5_SLOT_BIT = SHADER_STAGE_VERTEX_5_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_6_SLOT_BIT = SHADER_STAGE_VERTEX_6_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_7_SLOT_BIT = SHADER_STAGE_VERTEX_7_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_8_SLOT_BIT = SHADER_STAGE_VERTEX_8_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_9_SLOT_BIT = SHADER_STAGE_VERTEX_9_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_10_SLOT_BIT = SHADER_STAGE_VERTEX_10_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_11_SLOT_BIT = SHADER_STAGE_VERTEX_11_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_12_SLOT_BIT = SHADER_STAGE_VERTEX_12_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_13_SLOT_BIT = SHADER_STAGE_VERTEX_13_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_14_SLOT_BIT = SHADER_STAGE_VERTEX_14_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_15_SLOT_BIT = SHADER_STAGE_VERTEX_15_SLOT_BIT,
  SHADER_STAGE_SUBPASS_SHADING_16_SLOT_BIT = SHADER_STAGE_VERTEX_16_SLOT_BIT,

  // All stage
  SHADER_STAGE_ALL_STAGE_0_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_0_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_1_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_2_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_3_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_4_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_5_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_6_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_7_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_8_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_9_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_10_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_11_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_12_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_13_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_14_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_15_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT,
  SHADER_STAGE_ALL_STAGE_16_SLOT_BIT = SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT,
};

ShaderSlotCount operator|(const ShaderSlotCount& lhs,
                          const ShaderSlotCount& rhs);

ShaderSlotCount& operator|=(ShaderSlotCount& lhs, const ShaderSlotCount& rhs);

ShaderSlotCount operator&(const ShaderSlotCount& lhs,
                          const ShaderSlotCount& rhs);

ShaderSlotCount& operator&=(ShaderSlotCount& lhs, const ShaderSlotCount& rhs);
}  // namespace RenderSystem
}  // namespace MM