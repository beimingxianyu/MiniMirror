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

  ExecuteResult AllocateMeshBuffer(VkDeviceSize vertex_size,
                                   VkDeviceSize index_size,
                                   AllocatedMesh& render_resource_mesh_buffer);

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

  ExecuteResult Reserve(VkDeviceSize new_vertex_buffer_size,
                        VkDeviceSize new_index_buffer_size);

  bool IsValid() const;

  ExecuteResult Release();

 private:
  ExecuteResult AddRemoveBufferFragmentationCommands(
      RenderEngine* this_render_engine, AllocatedCommandBuffer& cmd,
      std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
      std::list<BufferSubResourceAttribute>& index_buffet_chunks_info,
      VkCopyBufferInfo2& vertex_self_copy_info,
      VkCopyBufferInfo2& vertex_self_copy_to_stage_info,
      VkCopyBufferInfo2& vertex_stage_copy_to_self_info,
      VkCopyBufferInfo2& index_self_copy_info,
      VkCopyBufferInfo2& index_self_copy_to_stage_info,
      VkCopyBufferInfo2& index_stage_copy_to_self_info);

  void GetRemoveBufferFragmentationBufferCopy(
      std::list<BufferSubResourceAttribute>& buffer_chunks_info,
      VkDeviceSize& stage_buffer_size,
      std::vector<VkBufferCopy2>& self_copy_regions,
      std::vector<VkBufferCopy2>& self_copy_to_stage_regions,
      std::vector<VkBufferCopy2>& stage_copy_to_self_regions);

  ExecuteResult RemoveBufferFragmentationImp(
      MM::RenderSystem::AllocatedMeshBuffer& buffer,
      std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
      std::list<BufferSubResourceAttribute>& index_buffet_chunks_info);

  ExecuteResult AddReserveCommands(AllocatedCommandBuffer& cmd,
                                   VkDependencyInfo& dependency_info,
                                   VkCopyBufferInfo2& vertex_buffer_copy_info,
                                   VkCopyBufferInfo2& index_buffer_copy_info);

  void FreeMeshBuffer(BufferSubResourceAttribute* sub_vertex_buffer_info_ptr,
                      BufferSubResourceAttribute* sub_index_buffer_info_ptr);

 private:
  AllocatedMeshBuffer managed_allocated_mesh_buffer_{};
  MeshBufferCapacityData capacity_data_{};
  std::list<BufferSubResourceAttribute> sub_vertex_buffer_list_{};
  std::list<BufferSubResourceAttribute> sub_index_buffer_list_{};

  std::mutex allocate_free_mutex_;
};

}  // namespace RenderSystem
}  // namespace MM
