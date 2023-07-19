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
}  // namespace RenderSystem
}  // namespace MM