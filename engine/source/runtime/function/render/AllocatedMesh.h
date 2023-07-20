//
// Created by beimingxianyu on 23-7-18.
//
#pragma once

#include "runtime/function/render/MeshBufferManager.h"
#include "runtime/function/render/RenderResourceDataBase.h"
#include "runtime/resource/asset_system/AssetSystem.h"

namespace MM {
namespace RenderSystem {
class AllocatedMesh : public RenderResourceDataBase {
  friend class MeshBufferManager;

 public:
  AllocatedMesh() = default;
  virtual ~AllocatedMesh() override;
  AllocatedMesh(const std::string& name,
                MM::RenderSystem::MeshBufferManager* mesh_buffer_manager,
                MM::AssetSystem::AssetManager::HandlerType mesh_asset);
  AllocatedMesh(const std::string& name,
                MM::RenderSystem::MeshBufferManager* mesh_buffer_manager,
                VkDeviceSize vertex_buffer_size,
                VkDeviceSize index_buffer_size);
  AllocatedMesh(const AllocatedMesh& other) = delete;
  AllocatedMesh(AllocatedMesh&& other) noexcept;
  AllocatedMesh& operator=(const AllocatedMesh& other) = delete;
  AllocatedMesh& operator=(AllocatedMesh&& other) noexcept;

 public:
  VkBuffer GetVertexBuffer();

  const VkBuffer_T* GetVertexBuffer() const;

  VkBuffer GetIndexBuffer();

  const VkBuffer_T* GetIndexBuffer() const;

  AllocatedMeshBuffer& GetAllocatedMeshBuffer();

  const AllocatedMeshBuffer& GetAllocatedMeshBuffer() const;

  const BufferChunkInfo& GetVertexChunkInfo() const;

  VkDeviceSize GetVertexOffset() const;

  VkDeviceSize GetVertexSize() const;

  QueueIndex GetVertexQueueIndex() const;

  const BufferChunkInfo& GetIndexChunkInfo() const;

  VkDeviceSize GetIndexOffset() const;

  VkDeviceSize GetIndexSize() const;

  QueueIndex GetIndexQueueIndex() const;

  ExecuteResult CopyAssetDataToBuffer(
      AssetSystem::AssetManager::HandlerType asset_handler);

  ResourceType GetResourceType() const override;

  VkDeviceSize GetSize() const override;

  bool IsArray() const override;

  bool CanWrite() const override;

  bool IsValid() const override;

  void Release() override;

 private:
  MeshBufferManager* mesh_buffer_manager_{nullptr};
  BufferSubResourceAttribute* sub_vertex_buffer_info_ptr_{nullptr};
  BufferSubResourceAttribute* sub_index_buffer_info_ptr_{nullptr};
};
}  // namespace RenderSystem
}  // namespace MM