//
// Created by beimingxianyu on 23-7-27.
//
#pragma once

#include "runtime/function/render/AllocatedMesh.h"

namespace MM {
namespace RenderSystem {

class RenderResourceMesh {
 public:
  RenderResourceMesh() = default;
  ~RenderResourceMesh() = default;
  explicit RenderResourceMesh(AllocatedMesh* allocated_mesh);
  RenderResourceMesh(const RenderResourceMesh& other) = delete;
  RenderResourceMesh(RenderResourceMesh&& other) noexcept;
  RenderResourceMesh& operator=(const RenderResourceMesh& other) = delete;
  RenderResourceMesh& operator=(RenderResourceMesh&& other) noexcept;

 public:
  VkBuffer GetVertexBuffer();

  const VkBuffer_T* GetVertexBuffer() const;

  VkBuffer GetIndexBuffer();

  const VkBuffer_T* GetIndexBuffer() const;

  const BufferChunkInfo& GetVertexChunkInfo() const;

  const BufferChunkInfo& GetIndexChunkInfo() const;

  std::uint32_t GetIndexCount() const;

  std::uint32_t GetIndexOffset() const;

  std::int32_t GetVertexOffset() const;

  bool IsValid() const;

  void Reset();

 private:
  AllocatedMesh* allocated_mesh_{};
};

}  // namespace RenderSystem
}  // namespace MM