//
// Created by beimingxianyu on 23-7-8.
//

#pragma once

namespace MM {
namespace RenderSystem {
enum class ResourceType {
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
enum class MemoryOperate { READ, WRITE, READ_AND_WRITE, UNDEFINED };

enum class CommandBufferType { GRAPH, COMPUTE, TRANSFORM, UNDEFINED };

enum class DescriptorType {
  SAMPLER_TEXTURE2D,
  SAMPLER_TEXTURE3D,
  SAMPLER_TEXTURECUBE,
  STORAGE_TEXTURE2D,
  STORAGE_TEXTURE3D,
  STORAGE_TEXTURECUBE,
};

enum class DynamicState : std::uint64_t {
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
}  // namespace RenderSystem
}  // namespace MM