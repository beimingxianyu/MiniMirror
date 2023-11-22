#pragma once

#include "runtime/function/render/AllocatedMeshBuffer.h"
#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {
class AllocatedMesh;

class MeshBufferManager {
  friend class AllocatedMesh;

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
  RenderEngine* GetRenderEnginePtr();

  const RenderEngine* GetRenderEnginePtr() const;

  Result<AllocatedMesh> AllocateMeshBuffer(VkDeviceSize require_vertex_size,
                                           VkDeviceSize require_index_size);

  Result<Nil> AllocateMeshBuffer(VkDeviceSize require_vertex_size,
                                 VkDeviceSize require_index_size,
                                 AllocatedMesh& allocated_mesh);

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

  Result<Nil> RemoveBufferFragmentation();

  Result<Nil> Reserve(VkDeviceSize new_vertex_buffer_size,
                      VkDeviceSize new_index_buffer_size);

  bool IsValid() const;

  Result<Nil> Release();

 private:
  Result<Nil> AddRemoveBufferFragmentationCommands(
      const RenderEngine* this_render_engine, AllocatedCommandBuffer& cmd,
      const std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
      const std::list<BufferSubResourceAttribute>& index_buffet_chunks_info,
      const VkCopyBufferInfo2& vertex_self_copy_info,
      const VkCopyBufferInfo2& vertex_self_copy_to_stage_info,
      const VkCopyBufferInfo2& vertex_stage_copy_to_self_info,
      const VkCopyBufferInfo2& index_self_copy_info,
      const VkCopyBufferInfo2& index_self_copy_to_stage_info,
      const VkCopyBufferInfo2& index_stage_copy_to_self_info);

  void GetRemoveBufferFragmentationBufferCopy(
      std::list<BufferSubResourceAttribute>& buffer_chunks_info,
      VkDeviceSize& stage_buffer_size,
      std::vector<VkBufferCopy2>& self_copy_regions,
      std::vector<VkBufferCopy2>& self_copy_to_stage_regions,
      std::vector<VkBufferCopy2>& stage_copy_to_self_regions);

  Result<Nil> RemoveBufferFragmentationImp(
      AllocatedMeshBuffer& buffer,
      std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
      std::list<BufferSubResourceAttribute>& index_buffet_chunks_info);

  Result<Nil> AddReserveCommands(AllocatedCommandBuffer& cmd, const VkDependencyInfo& dependency_info,
      const VkCopyBufferInfo2& vertex_buffer_copy_info,
      const VkCopyBufferInfo2& index_buffer_copy_info);

  void FreeMeshBuffer(AllocatedMesh& allocated_mesh);

  Result<Nil> ReserveStandard(VkDeviceSize require_vertex_size,
                              VkDeviceSize require_index_size);

  Result<Nil> RemoveBufferFragmentationWithoutLock();

  Result<Nil> ReserveImp(VkDeviceSize new_vertex_buffer_size,
                         VkDeviceSize new_index_buffer_size);

  Result<Nil> ReserveWithoutLock(VkDeviceSize new_vertex_buffer_size,
                                 VkDeviceSize new_index_buffer_size);

 private:
  bool is_valid{false};

  AllocatedMeshBuffer managed_allocated_mesh_buffer_{};
  MeshBufferCapacityData capacity_data_{};
  std::list<BufferSubResourceAttribute> sub_vertex_buffer_list_{};
  std::list<BufferSubResourceAttribute> sub_index_buffer_list_{};

  std::mutex allocate_free_mutex_;
};

}  // namespace RenderSystem
}  // namespace MM
