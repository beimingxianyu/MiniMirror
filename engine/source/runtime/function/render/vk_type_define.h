//
// Created by beimingxianyu on 23-6-28.
//

#pragma once

namespace MM {
namespace RenderSystem {
enum class ImageTransferMode {
  INIT_TO_ATTACHMENT,
  INIT_TO_TRANSFER_DESTINATION,
  TRANSFER_DESTINATION_TO_SHARED_READABLE,
  TRANSFER_DESTINATION_TO_SHARED_PRESENT,
  ATTACHMENT_TO_PRESENT,
  INIT_TO_DEPTH_TEST,
  ATTACHMENT_TO_TRANSFER_SOURCE,
  TRANSFER_DESTINATION_TO_TRANSFER_SOURCE
};

enum class ResourceType {
  Texture,
  BUFFER,
  VERTEX_BUFFER,
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
