//
// Created by beimingxianyu on 23-7-18.
//

#include "runtime/function/render/AllocatedMesh.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::AllocatedMesh::~AllocatedMesh() {
  if (mesh_buffer_manager_ == nullptr) {
    return;
  }

  assert(sub_vertex_buffer_info_ptr_ != nullptr &&
         sub_index_buffer_info_ptr_ != nullptr);

  RenderResourceDataBase::Release();

  mesh_buffer_manager_->FreeMeshBuffer(*this);

  mesh_buffer_manager_ = nullptr;
  sub_vertex_buffer_info_ptr_ = nullptr;
  sub_index_buffer_info_ptr_ = nullptr;
}

MM::RenderSystem::AllocatedMesh::AllocatedMesh(
    const std::string& name, MeshBufferManager* mesh_buffer_manager,
    AssetSystem::AssetManager::HandlerType mesh_asset)
    : RenderResourceDataBase(name, RenderResourceDataID{}),
      mesh_buffer_manager_(mesh_buffer_manager),
      sub_vertex_buffer_info_ptr_(nullptr),
      sub_index_buffer_info_ptr_(nullptr) {
  if (mesh_buffer_manager == nullptr || !mesh_buffer_manager->IsValid()) {
    mesh_buffer_manager_ = nullptr;
    MM_LOG_ERROR("The mesh buffer manager is invalid.");
    return;
  }
  if (!mesh_asset.IsValid() || mesh_asset.GetAsset().GetAssetType() !=
                                   AssetSystem::AssetType::AssetType::MESH) {
    mesh_buffer_manager_ = nullptr;
    MM_LOG_ERROR("The mesh asset is invalid.");
    return;
  }
  const AssetSystem::AssetType::Mesh& mesh =
      static_cast<AssetSystem::AssetType::Mesh&>(mesh_asset.GetAsset());
  const VkDeviceSize vertex_buffer_size = mesh.GetVerticesCount() *
                                    sizeof(AssetSystem::AssetType::Vertex),
               index_buffer_size = mesh.GetIndexesCount() * sizeof(VertexIndex);

  if (auto if_result = mesh_buffer_manager_->AllocateMeshBuffer(vertex_buffer_size,
                                                    index_buffer_size, *this);
                                                    if_result.IgnoreException().IsError()) {
    RenderResourceDataBase::Release();
    mesh_buffer_manager_ = nullptr;
    return;
  }

  if (auto if_result = CopyAssetDataToBuffer(mesh_asset);
    if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to copy asset data to buffer.")).IsError()) {
    RenderResourceDataBase::Release();

    mesh_buffer_manager_->FreeMeshBuffer(*this);
    mesh_buffer_manager_ = nullptr;
    sub_vertex_buffer_info_ptr_ = nullptr;
    sub_index_buffer_info_ptr_ = nullptr;
    return;
  }

  MarkThisIsAssetResource();
}

MM::RenderSystem::AllocatedMesh::AllocatedMesh(
    const std::string& name, MeshBufferManager* mesh_buffer_manager,
    VkDeviceSize vertex_buffer_size, VkDeviceSize index_buffer_size)
    : RenderResourceDataBase(name, RenderResourceDataID()),
      mesh_buffer_manager_(mesh_buffer_manager),
      sub_vertex_buffer_info_ptr_(nullptr),
      sub_index_buffer_info_ptr_(nullptr) {
  if (mesh_buffer_manager == nullptr || !mesh_buffer_manager->IsValid()) {
    mesh_buffer_manager_ = nullptr;
    MM_LOG_ERROR("The mesh buffer manager is invalid.");
    return;
  }

  if (auto if_result = mesh_buffer_manager_->AllocateMeshBuffer(vertex_buffer_size,
                                                    index_buffer_size, *this);
    if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to allocated new MM::RenderSystem::AllocatedMesh.")).IsError()) {
    RenderResourceDataBase::Release();
    mesh_buffer_manager_ = nullptr;
    return;
  }

  SetRenderResourceDataID(RenderResourceDataID{
      GetObjectID().GetHash(), RenderResourceDataAttributeID{0, 0, 0}});
}

MM::RenderSystem::AllocatedMesh::AllocatedMesh(AllocatedMesh&& other) noexcept
    : RenderResourceDataBase(std::move(other)),
      mesh_buffer_manager_(other.mesh_buffer_manager_),
      sub_vertex_buffer_info_ptr_(other.sub_vertex_buffer_info_ptr_),
      sub_index_buffer_info_ptr_(other.sub_index_buffer_info_ptr_),
      index_count_(other.index_count_) {
  other.mesh_buffer_manager_ = nullptr;
  other.sub_vertex_buffer_info_ptr_ = nullptr;
  other.sub_index_buffer_info_ptr_ = nullptr;
  other.index_count_ = 0;
}

MM::RenderSystem::AllocatedMesh& MM::RenderSystem::AllocatedMesh::operator=(
    AllocatedMesh&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  Release();

  RenderResourceDataBase::operator=(std::move(other));
  mesh_buffer_manager_ = other.mesh_buffer_manager_;
  sub_vertex_buffer_info_ptr_ = other.sub_vertex_buffer_info_ptr_;
  sub_index_buffer_info_ptr_ = other.sub_index_buffer_info_ptr_;

  other.mesh_buffer_manager_ = nullptr;
  other.sub_vertex_buffer_info_ptr_ = nullptr;
  other.sub_index_buffer_info_ptr_ = nullptr;
  other.index_count_ = 0;

  return *this;
}

VkBuffer MM::RenderSystem::AllocatedMesh::GetVertexBuffer() {
  assert(mesh_buffer_manager_ != nullptr);
  return mesh_buffer_manager_->GetVertexBuffer();
}

const VkBuffer_T* MM::RenderSystem::AllocatedMesh::GetVertexBuffer() const {
  assert(mesh_buffer_manager_ != nullptr);
  return mesh_buffer_manager_->GetVertexBuffer();
}

VkBuffer MM::RenderSystem::AllocatedMesh::GetIndexBuffer() {
  assert(mesh_buffer_manager_ != nullptr);
  return mesh_buffer_manager_->GetIndexBuffer();
}

const VkBuffer_T* MM::RenderSystem::AllocatedMesh::GetIndexBuffer() const {
  assert(mesh_buffer_manager_ != nullptr);
  return mesh_buffer_manager_->GetIndexBuffer();
}

MM::RenderSystem::AllocatedMeshBuffer&
MM::RenderSystem::AllocatedMesh::GetAllocatedMeshBuffer() {
  assert(mesh_buffer_manager_ != nullptr);
  return mesh_buffer_manager_->GetAllocatedMeshBuffer();
}

const MM::RenderSystem::AllocatedMeshBuffer&
MM::RenderSystem::AllocatedMesh::GetAllocatedMeshBuffer() const {
  assert(mesh_buffer_manager_ != nullptr);
  return mesh_buffer_manager_->GetAllocatedMeshBuffer();
}

const MM::RenderSystem::BufferChunkInfo&
MM::RenderSystem::AllocatedMesh::GetVertexChunkInfo() const {
  assert(sub_vertex_buffer_info_ptr_ != nullptr);
  return sub_vertex_buffer_info_ptr_->GetChunkInfo();
}

VkDeviceSize MM::RenderSystem::AllocatedMesh::GetVertexOffset() const {
  assert(sub_vertex_buffer_info_ptr_ != nullptr);
  return sub_vertex_buffer_info_ptr_->GetOffset();
}

VkDeviceSize MM::RenderSystem::AllocatedMesh::GetVertexSize() const {
  assert(sub_vertex_buffer_info_ptr_ != nullptr);
  return sub_vertex_buffer_info_ptr_->GetSize();
}

MM::RenderSystem::QueueIndex
MM::RenderSystem::AllocatedMesh::GetVertexQueueIndex() const {
  assert(sub_vertex_buffer_info_ptr_ != nullptr);
  return sub_vertex_buffer_info_ptr_->GetQueueIndex();
}

const MM::RenderSystem::BufferChunkInfo&
MM::RenderSystem::AllocatedMesh::GetIndexChunkInfo() const {
  assert(sub_index_buffer_info_ptr_ != nullptr);
  return sub_index_buffer_info_ptr_->GetChunkInfo();
}

std::uint32_t MM::RenderSystem::AllocatedMesh::GetIndexCount() const {
  return index_count_;
}

VkDeviceSize MM::RenderSystem::AllocatedMesh::GetIndexOffset() const {
  assert(sub_index_buffer_info_ptr_ != nullptr);
  return sub_index_buffer_info_ptr_->GetOffset();
}

VkDeviceSize MM::RenderSystem::AllocatedMesh::GetIndexSize() const {
  assert(sub_index_buffer_info_ptr_ != nullptr);
  return sub_index_buffer_info_ptr_->GetSize();
}

MM::RenderSystem::QueueIndex
MM::RenderSystem::AllocatedMesh::GetIndexQueueIndex() const {
  assert(sub_index_buffer_info_ptr_ != nullptr);
  return sub_index_buffer_info_ptr_->GetQueueIndex();
}

MM::Result<MM::Nil> MM::RenderSystem::AllocatedMesh::CopyAssetDataToBuffer(
    AssetSystem::AssetManager::HandlerType asset_handler) {
  if (!IsValid() && !asset_handler.IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  if (IsAssetResource()) {
    MM_LOG_ERROR(
        "It is not supported to rewrite asset data to an AllocatedBuffer that "
        "has already written asset data.");
    return ResultE<>{ErrorCode::OPERATION_NOT_SUPPORTED};
  }

  if (asset_handler.GetAsset().GetAssetType() !=
      AssetSystem::AssetType::AssetType::MESH) {
    MM_LOG_ERROR("Asset is not a mesh.");
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  AssetSystem::AssetType::Mesh& asset_mesh =
      static_cast<AssetSystem::AssetType::Mesh&>(asset_handler.GetAsset());

  const VkDeviceSize buffer_vertex_size = GetVertexSize(),
               buffer_index_size = GetIndexSize();
  VkDeviceSize asset_vertex_size = asset_mesh.GetVerticesCount() *
                                   sizeof(AssetSystem::AssetType::Vertex),
               asset_index_size =
                   asset_mesh.GetIndexesCount() * sizeof(VertexIndex);
  if ((asset_vertex_size > buffer_vertex_size) ||
      (asset_index_size > buffer_index_size)) {
    MM_LOG_ERROR(
        "The vertex and index buffers are too small to hold all the data for "
        "the given asset.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  RenderEngine* render_engine =
      mesh_buffer_manager_->GetAllocatedMeshBuffer().GetRenderEnginePtr();
  Result<AllocatedBuffer> stage_buffer_result = render_engine->CreateStageBuffer(
               asset_vertex_size + asset_index_size,
               render_engine->GetTransformQueueIndex());
  if (stage_buffer_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create stage buffer.")).IsError()) {
    return ResultE<>{stage_buffer_result.GetError().GetErrorCode()};
  }
  AllocatedBuffer& stage_buffer = stage_buffer_result.GetResult();

  void* stage_data_void{nullptr};
  vmaMapMemory(stage_buffer.GetAllocator(), stage_buffer.GetAllocation(),
               &stage_data_void);
  memcpy(stage_data_void, asset_mesh.GetVertices().data(), asset_vertex_size);
  memcpy(static_cast<char*>(stage_data_void) + asset_vertex_size,
         asset_mesh.GetIndexes().data(), asset_index_size);
  vmaUnmapMemory(stage_buffer.GetAllocator(), stage_buffer.GetAllocation());

  if (auto if_result =
      render_engine->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, false,
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()},
          [this_mesh = this, &stage_buffer, asset_vertex_size, asset_index_size,
           &asset_mesh](AllocatedCommandBuffer& cmd) -> Result<Nil> {
            VkBuffer vertex_buffer = this_mesh->GetVertexBuffer(),
                     index_buffer = this_mesh->GetIndexBuffer();
            const BufferChunkInfo vertex_buffer_chunk_info =
                this_mesh->GetVertexChunkInfo();
            const BufferChunkInfo index_buffer_chunk_info =
                this_mesh->GetIndexChunkInfo();

            const QueueIndex transform_queue_index =
                this_mesh->mesh_buffer_manager_->GetRenderEnginePtr()
                    ->GetTransformQueueIndex();

            std::array<VkBufferMemoryBarrier2, 2> barriers{
                GetVkBufferMemoryBarrier2(
                    VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                    VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    VK_ACCESS_2_TRANSFER_WRITE_BIT,
                    this_mesh->sub_vertex_buffer_info_ptr_->GetQueueIndex(),
                    transform_queue_index, vertex_buffer,
                    vertex_buffer_chunk_info.GetOffset(),
                    vertex_buffer_chunk_info.GetSize()),
                GetVkBufferMemoryBarrier2(
                    VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                    VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    VK_ACCESS_2_TRANSFER_WRITE_BIT,
                    this_mesh->sub_index_buffer_info_ptr_->GetQueueIndex(),
                    transform_queue_index, index_buffer,
                    index_buffer_chunk_info.GetOffset(),
                    index_buffer_chunk_info.GetSize())};
            const VkDependencyInfo dependency_info{GetVkDependencyInfo(
                0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

            const VkBufferCopy2 vertex_buffer_copy{GetVkBufferCopy2(
                asset_vertex_size, 0, vertex_buffer_chunk_info.GetOffset())},
                index_buffer_copy{
                    GetVkBufferCopy2(asset_index_size, asset_vertex_size,
                                     index_buffer_chunk_info.GetOffset())};
            const VkCopyBufferInfo2 vertex_copy_buffer{
                GetVkCopyBufferInfo2(
                    nullptr, stage_buffer.GetBuffer(), vertex_buffer, 1,
                    &vertex_buffer_copy)},
                index_copy_buffer{GetVkCopyBufferInfo2(
                    nullptr, stage_buffer.GetBuffer(), index_buffer, 1,
                    &index_buffer_copy)};

            if (auto if_result2 = BeginCommandBuffer(cmd);
              if_result2.Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer.")).IsError()) {
              return ResultE<>{if_result2.GetError().GetErrorCode()};
            }

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &vertex_copy_buffer);
            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &index_copy_buffer);

            barriers[0].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barriers[1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            std::swap(barriers[0].srcQueueFamilyIndex,
                      barriers[0].dstQueueFamilyIndex);
            std::swap(barriers[1].srcQueueFamilyIndex,
                      barriers[1].dstQueueFamilyIndex);
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            if (auto if_result2 = EndCommandBuffer(cmd);
              if_result2.Exception(MM_ERROR_DESCRIPTION2("Failed to end command buffer.")).IsError()) {
              return ResultE<>{if_result2.GetError().GetErrorCode()};
            }

            this_mesh->MarkThisIsAssetResource();
            this_mesh->SetRenderResourceDataID(
                RenderResourceDataID(asset_mesh.GetAssetID(),
                                     RenderResourceDataAttributeID{0, 0, 0}));

            return ResultS<Nil>{};
          });
          if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to record commands.")).IsError() ||
          if_result.GetResult() != RenderFutureState::SUCCESS) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  index_count_ = asset_mesh.GetIndexesCount();

  return ResultS<Nil>{};
}

MM::RenderSystem::ResourceType
MM::RenderSystem::AllocatedMesh::GetResourceType() const {
  return ResourceType::MESH_BUFFER;
}

VkDeviceSize MM::RenderSystem::AllocatedMesh::GetSize() const {
  if (!IsValid()) {
    return 0;
  }

  return sub_vertex_buffer_info_ptr_->GetSize() +
         sub_index_buffer_info_ptr_->GetSize();
}

bool MM::RenderSystem::AllocatedMesh::IsArray() const { return false; }

bool MM::RenderSystem::AllocatedMesh::CanWrite() const { return true; }

bool MM::RenderSystem::AllocatedMesh::IsValid() const {
  return mesh_buffer_manager_ != nullptr &&
         sub_vertex_buffer_info_ptr_ != nullptr &&
         sub_index_buffer_info_ptr_ != nullptr && index_count_ != 0;
}

void MM::RenderSystem::AllocatedMesh::Release() {
  if (IsValid()) {
    RenderResourceDataBase::Release();

    mesh_buffer_manager_->FreeMeshBuffer(*this);

    mesh_buffer_manager_ = nullptr;
    sub_vertex_buffer_info_ptr_ = nullptr;
    sub_index_buffer_info_ptr_ = nullptr;
    index_count_ = 0;
  }
}
