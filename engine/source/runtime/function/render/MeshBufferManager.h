#pragma once

#include "runtime/function/render/AllocatedMeshBuffer.h"
#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {
class RenderResourceMeshBuffer;

class MeshBufferManager {
 public:
  MeshBufferManager() = delete;
  ~MeshBufferManager() = default;
  explicit MeshBufferManager(AllocatedMeshBuffer&& allocated_mesh_buffer);
  MeshBufferManager(AllocatedMeshBuffer&& allocated_mesh_buffer,
                    float capacity_coefficient, float expansion_coefficient);
  MeshBufferManager(const MeshBufferManager& other) = delete;
  MeshBufferManager(MeshBufferManager&& other) noexcept;
  MeshBufferManager& operator=(const MeshBufferManager& other) = delete;
  MeshBufferManager& operator=(MeshBufferManager&& other) noexcept;

 public:
  ExecuteResult AllocateMeshBuffer(
      RenderResourceMeshBuffer& render_resource_mesh_buffer);

  VkBuffer GetVertexBuffer();

  const VkBuffer_T* GetVertexBuffer() const;

  VkBuffer GetIndexBuffer();

  const VkBuffer_T* GetIndexBuffer() const;

  AllocatedMeshBuffer& GetAllocatedMeshBuffer();

  const AllocatedMeshBuffer& GetAllocatedMeshBuffer() const;

  const MeshBufferCapacityData& GetMeshBufferCapacityData() const;

  float GetCapacityCoefficient() const;

  float GetExpansionCoefficient() const;

  VkDeviceSize GetIndexBufferRemainingCapacity() const;

  VkDeviceSize GetVertexBufferRemainingCapacity() const;

  void SetCapacityCoefficient(float capacity_coefficient);

  void SetExpansionCoefficient(float expansion_coefficient);

  ExecuteResult RemoveBufferFragmentation();

  ExecuteResult Reserve();

  bool IsValid() const;

  ExecuteResult Release();

 private:
  void FreeMeshBuffer();

 private:
  AllocatedMeshBuffer managed_allocated_mesh_buffer_{};
  MeshBufferCapacityData capacity_data_{};
  std::list<BufferSubResourceAttribute> sub_vertex_buffer_list_{};
  std::list<BufferSubResourceAttribute> sub_index_buffer_list_{};

  std::mutex allocate_free_mutex_;
};

}  // namespace RenderSystem
}  // namespace MM
