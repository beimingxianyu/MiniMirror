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
  AllocatedMeshBuffer(const AllocatedMeshBuffer& other) = delete;
  AllocatedMeshBuffer(AllocatedMeshBuffer&& other) noexcept;
  AllocatedMeshBuffer& operator=(const AllocatedMeshBuffer& other) = delete;
  AllocatedMeshBuffer& operator=(AllocatedMeshBuffer&& other) noexcept;

 public:
  VkDeviceSize GetVertexSize() const;

  VkDeviceSize GetIndexSize() const;

  RenderEngine* GetRenderEnginePtr();

  const RenderEngine* GetRenderEnginePtr() const;

  const MeshBufferCapacityData& GetMeshBufferCapacityData() const;

  float GetCapacityCoefficient() const;

  float GetExpansionCoefficient() const;

  VkDeviceSize GetIndexBufferRemainingCapacity() const;

  VkDeviceSize GetVertexBufferRemainingCapacity() const;

  void SetCapacityCoefficient(float capacity_coefficient);

  void SetExpansionCoefficient(float expansion_coefficient);

  const MeshVertexInfo& GetMeshVertexInfo() const;

  const MeshIndexInfo& GetMeshIndexInfo() const;

  const BufferCreateInfo& GetVertexBufferCreateInfo() const;

  const BufferCreateInfo& GetIndexBufferCreateInfo() const;

  const AllocationCreateInfo& GetVertexAllocationCreateInfo() const;

  const AllocationCreateInfo& GetIndexAllocationCreateInfo() const;

  VmaAllocator GetAllocator();

  const VmaAllocator_T* GetAllocator() const;

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
  RenderEngine* render_engine_{nullptr};
  MeshBufferCapacityData capacity_data_{};
  MeshVertexInfo vertex_info_{};
  MeshIndexInfo index_info_{};
  AllocatedBufferWrapper vertex_buffer_{};
  AllocatedBufferWrapper index_buffer_{};
};

}  // namespace RenderSystem
}  // namespace MM
