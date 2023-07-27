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
}  // namespace RenderSystem
}  // namespace MM