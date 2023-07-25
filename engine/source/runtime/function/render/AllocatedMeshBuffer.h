#pragma once

#include "runtime/function/render/AllocatedBuffer.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

class AllocatedMeshBuffer : public MMObject {
 public:
  AllocatedMeshBuffer() = default;
  ~AllocatedMeshBuffer() = default;
  explicit AllocatedMeshBuffer(RenderEngine* render_engine);
  AllocatedMeshBuffer(RenderEngine* render_engine, std::uint32_t vertex_count);
  AllocatedMeshBuffer(RenderEngine* render_engine,
                      VkDeviceSize vertex_buffer_size,
                      VkDeviceSize index_buffer_size);
  AllocatedMeshBuffer(const AllocatedMeshBuffer& other) = delete;
  AllocatedMeshBuffer(AllocatedMeshBuffer&& other) noexcept;
  AllocatedMeshBuffer& operator=(const AllocatedMeshBuffer& other) = delete;
  AllocatedMeshBuffer& operator=(AllocatedMeshBuffer&& other) noexcept;

 public:
  VkDeviceSize GetVertexSize() const;

  VkDeviceSize GetIndexSize() const;

  RenderEngine* GetRenderEnginePtr();

  const RenderEngine* GetRenderEnginePtr() const;

  const MeshVertexInfo& GetMeshVertexInfo() const;

  const MeshIndexInfo& GetMeshIndexInfo() const;

  const BufferCreateInfo& GetVertexBufferCreateInfo() const;

  const BufferCreateInfo& GetIndexBufferCreateInfo() const;

  const AllocationCreateInfo& GetVertexAllocationCreateInfo() const;

  const AllocationCreateInfo& GetIndexAllocationCreateInfo() const;

  VmaAllocator GetAllocator() const;

  VkBuffer GetVertexBuffer();

  VmaAllocation GetVertexAllocation();

  const VkBuffer_T* GetVertexBuffer() const;

  const VmaAllocation_T* GetVertexAllocation() const;

  VkBuffer GetIndexBuffer();

  VmaAllocation GetIndexAllocation();

  const VkBuffer_T* GetIndexBuffer() const;

  const VmaAllocation_T* GetIndexAllocation() const;

  bool IsValid() const;

  void Release();

 private:
  ExecuteResult InitMeshBuffer(std::uint64_t vertex_buffer_size,
                               std::uint64_t index_buffer_size);

 private:
  RenderEngine* render_engine_{nullptr};
  MeshVertexInfo vertex_info_{};
  MeshIndexInfo index_info_{};
  AllocatedBufferWrapper vertex_buffer_{};
  AllocatedBufferWrapper index_buffer_{};
};

}  // namespace RenderSystem
}  // namespace MM
