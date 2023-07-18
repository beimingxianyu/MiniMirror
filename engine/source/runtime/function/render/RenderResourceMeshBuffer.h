//
// Created by beimingxianyu on 23-7-18.
//
#pragma once

#include "runtime/function/render/MeshBufferManager.h"

namespace MM {
namespace RenderSystem {
class RenderResourceMeshBuffer {
 public:
  RenderResourceMeshBuffer() = default;
  ~RenderResourceMeshBuffer();
  RenderResourceMeshBuffer(const std::string& name);
  RenderResourceMeshBuffer(const RenderResourceMeshBuffer& other) = delete;
  RenderResourceMeshBuffer(RenderResourceMeshBuffer&& other) noexcept = default;
  RenderResourceMeshBuffer& operator=(const RenderResourceMeshBuffer& other) =
      delete;
  RenderResourceMeshBuffer& operator=(
      RenderResourceMeshBuffer&& other) noexcept = default;

 private:
  MeshBufferManager* mesh_buffer_manager_{nullptr};
  BufferSubResourceAttribute* sub_vertex_buffer_info_ptr_{nullptr};
  BufferSubResourceAttribute* sub_index_buffer_info_ptr_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM