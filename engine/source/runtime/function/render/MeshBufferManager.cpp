//
// Created by beimingxianyu on 23-7-10.
//

#include "runtime/function/render/MeshBufferManager.h"

#include <vulkan/vulkan_core.h>

#include "runtime/function/render/AllocatedMesh.h"
#include "runtime/function/render/vk_engine.h"
#include "runtime/function/render/vk_enum.h"
#include "runtime/function/render/vk_utils.h"
#include "utils/error.h"
#include "utils/type_utils.h"

namespace MM {
namespace RenderSystem {
MeshBufferManager::MeshBufferManager(
    AllocatedMeshBuffer&& allocated_mesh_buffer)
    : is_valid(true),
      managed_allocated_mesh_buffer_(std::move(allocated_mesh_buffer)),
      capacity_data_(0.9, 2, 0, 0),
      sub_index_buffer_list_(),
      sub_vertex_buffer_list_(),
      allocate_free_mutex_() {
  if (!managed_allocated_mesh_buffer_.IsValid()) {
    MM_LOG_ERROR("Manage an invalid mesh buffer.");
    managed_allocated_mesh_buffer_.Release();
    capacity_data_.Reset();
    is_valid = false;
    return;
  }

  capacity_data_.vertex_buffer_remaining_capacity_ =
      managed_allocated_mesh_buffer_.GetVertexBufferCreateInfo().size_;
  capacity_data_.index_buffer_remaining_capacity_ =
      managed_allocated_mesh_buffer_.GetIndexBufferCreateInfo().size_;
}

MeshBufferManager::MeshBufferManager(
    AllocatedMeshBuffer&& allocated_mesh_buffer, float capacity_coefficient,
    float expansion_coefficient)
    : is_valid(true),
      managed_allocated_mesh_buffer_(std::move(allocated_mesh_buffer)),
      capacity_data_(0.9, 2, capacity_coefficient, expansion_coefficient),
      sub_index_buffer_list_(),
      sub_vertex_buffer_list_(),
      allocate_free_mutex_() {
  if (!managed_allocated_mesh_buffer_.IsValid()) {
    MM_LOG_ERROR("Manage an invalid mesh buffer.");
    managed_allocated_mesh_buffer_.Release();
    capacity_data_.Reset();
    is_valid = false;
    return;
  }
}

MeshBufferManager::MeshBufferManager(MeshBufferManager&& other) noexcept
    : is_valid(),
      managed_allocated_mesh_buffer_(),
      capacity_data_(),
      sub_vertex_buffer_list_(),
      sub_index_buffer_list_() {
  std::lock(allocate_free_mutex_, other.allocate_free_mutex_);
  std::lock_guard guard1(allocate_free_mutex_, std::adopt_lock),
      guard2(other.allocate_free_mutex_, std::adopt_lock);

  is_valid = other.is_valid;
  managed_allocated_mesh_buffer_ =
      std::move(other.managed_allocated_mesh_buffer_);
  capacity_data_ = std::move(other.capacity_data_);
  sub_vertex_buffer_list_ = std::move(other.sub_vertex_buffer_list_);
  sub_index_buffer_list_ = std::move(other.sub_index_buffer_list_);

  other.is_valid = false;
}

MeshBufferManager& MeshBufferManager::operator=(
    MeshBufferManager&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  is_valid = other.is_valid;
  std::lock(allocate_free_mutex_, other.allocate_free_mutex_);
  std::lock_guard guard1(allocate_free_mutex_, std::adopt_lock),
      guard2(other.allocate_free_mutex_, std::adopt_lock);

  managed_allocated_mesh_buffer_ =
      std::move(other.managed_allocated_mesh_buffer_);
  capacity_data_ = std::move(other.capacity_data_);
  sub_vertex_buffer_list_ = std::move(other.sub_vertex_buffer_list_);
  sub_index_buffer_list_ = std::move(other.sub_index_buffer_list_);

  other.is_valid = false;

  return *this;
}

Result<Nil> MeshBufferManager::AllocateMeshBuffer(
    VkDeviceSize require_vertex_size, VkDeviceSize require_index_size,
    AllocatedMesh& allocated_mesh) {
  std::unique_lock guard(allocate_free_mutex_);
  if (!IsValid()) {
    MM_LOG_ERROR("MM::RenderSystem::MeshBufferManager is invalid.");
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  VkDeviceSize vertex_buffer_size = GetAllocatedMeshBuffer().GetVertexSize(),
               index_buffer_size = GetAllocatedMeshBuffer().GetIndexSize();

  if ((capacity_data_.vertex_buffer_remaining_capacity_ <
       vertex_buffer_size * capacity_data_.capacity_coefficient_) ||
      (capacity_data_.index_buffer_remaining_capacity_ <
       index_buffer_size * capacity_data_.capacity_coefficient_)) {
    ReserveStandard(
        std::ceil(vertex_buffer_size * capacity_data_.expansion_coefficient_),
        std::ceil(index_buffer_size * capacity_data_.expansion_coefficient_));
  }

  vertex_buffer_size = GetAllocatedMeshBuffer().GetVertexSize();
  index_buffer_size = GetAllocatedMeshBuffer().GetIndexSize();
  if (capacity_data_.vertex_buffer_remaining_capacity_ < require_vertex_size ||
      capacity_data_.index_buffer_remaining_capacity_ < require_index_size) {
    if (auto if_result = ReserveStandard(
            vertex_buffer_size -
                capacity_data_.vertex_buffer_remaining_capacity_ +
                require_vertex_size,
            index_buffer_size -
                capacity_data_.index_buffer_remaining_capacity_ +
                require_index_size);
        if_result
            .Exception(MM_ERROR_DESCRIPTION2("Failed to reserve mesh buffer."))
            .IsError()) {
      return ResultE<>{if_result.GetError().GetErrorCode()};
    }
  }

  vertex_buffer_size = GetAllocatedMeshBuffer().GetVertexSize();
  index_buffer_size = GetAllocatedMeshBuffer().GetIndexSize();
  VkDeviceSize pre_offset{0};
  std::list<BufferSubResourceAttribute>::iterator vertex_iter{}, index_iter{};
  QueueIndex graph_queue_index =
      managed_allocated_mesh_buffer_.GetRenderEnginePtr()->GetGraphQueueIndex();
  // find free sub vertex buffer
  const auto &vertex_back_item = sub_vertex_buffer_list_.back(),
             index_back_item = sub_index_buffer_list_.back();
  if (vertex_buffer_size -
          (vertex_back_item.GetOffset() + vertex_back_item.GetSize()) >=
      require_vertex_size) {
    vertex_iter = sub_vertex_buffer_list_.end();
  } else {
    for (vertex_iter = sub_vertex_buffer_list_.begin();
         vertex_iter != sub_vertex_buffer_list_.end(); ++vertex_iter) {
      if (vertex_iter->GetOffset() - pre_offset >= require_vertex_size) {
        break;
      }

      pre_offset = vertex_iter->GetOffset() + vertex_iter->GetSize();
    }

    if (auto if_result = RemoveBufferFragmentationWithoutLock();
        if_result.IgnoreException().IsError()) {
      return ResultE<>{if_result.GetError().GetErrorCode()};
    }

    assert(vertex_buffer_size -
               (vertex_back_item.GetOffset() + vertex_back_item.GetSize()) >=
           require_vertex_size);
    assert(index_buffer_size -
               (index_back_item.GetOffset() + index_back_item.GetSize()) >=
           require_index_size);

    sub_vertex_buffer_list_.emplace_back(
        vertex_back_item.GetOffset() + vertex_back_item.GetSize(),
        require_vertex_size, graph_queue_index);
    sub_index_buffer_list_.emplace_back(
        index_back_item.GetOffset() + index_back_item.GetSize(),
        require_index_size, graph_queue_index);
    BufferSubResourceAttribute* sub_vertex_buffer_info_ptr =
        &sub_vertex_buffer_list_.back();
    BufferSubResourceAttribute* sub_index_buffer_info_ptr =
        &sub_index_buffer_list_.back();
    capacity_data_.vertex_buffer_remaining_capacity_ -= require_vertex_size;
    capacity_data_.index_buffer_remaining_capacity_ -= require_index_size;
    guard.unlock();
    if (allocated_mesh.IsValid()) {
      allocated_mesh.mesh_buffer_manager_->FreeMeshBuffer(allocated_mesh);
    }
    allocated_mesh.mesh_buffer_manager_ = this;
    allocated_mesh.sub_vertex_buffer_info_ptr_ = sub_vertex_buffer_info_ptr;
    allocated_mesh.sub_index_buffer_info_ptr_ = sub_index_buffer_info_ptr;

    return ResultS<Nil>{};
  }

  // find free sub index buffer
  pre_offset = 0;
  if (index_buffer_size -
          (index_back_item.GetOffset() + index_back_item.GetSize()) >=
      require_index_size) {
    index_iter = sub_index_buffer_list_.end();
  } else {
    for (index_iter = sub_index_buffer_list_.begin();
         index_iter != sub_index_buffer_list_.end(); ++index_iter) {
      if (index_iter->GetOffset() - pre_offset >= require_index_size) {
        break;
      }

      pre_offset = index_iter->GetOffset() + index_iter->GetSize();
    }

    if (auto if_result = RemoveBufferFragmentationWithoutLock();
        if_result.IgnoreException().IsError()) {
      return ResultE<>{if_result.GetError().GetErrorCode()};
    }

    assert(vertex_buffer_size -
               (vertex_back_item.GetOffset() + vertex_back_item.GetSize()) >=
           require_vertex_size);
    assert(index_buffer_size -
               (index_back_item.GetOffset() + index_back_item.GetSize()) >=
           require_index_size);

    sub_vertex_buffer_list_.emplace_back(
        vertex_back_item.GetOffset() + vertex_back_item.GetSize(),
        require_vertex_size, graph_queue_index);
    sub_index_buffer_list_.emplace_back(
        index_back_item.GetOffset() + index_back_item.GetSize(),
        require_index_size, graph_queue_index);
    BufferSubResourceAttribute* sub_vertex_buffer_info_ptr =
        &sub_vertex_buffer_list_.back();
    BufferSubResourceAttribute* sub_index_buffer_info_ptr =
        &sub_index_buffer_list_.back();
    capacity_data_.vertex_buffer_remaining_capacity_ -= require_vertex_size;
    capacity_data_.index_buffer_remaining_capacity_ -= require_index_size;
    guard.unlock();
    if (allocated_mesh.IsValid()) {
      allocated_mesh.mesh_buffer_manager_->FreeMeshBuffer(allocated_mesh);
    }
    allocated_mesh.mesh_buffer_manager_ = this;
    allocated_mesh.sub_vertex_buffer_info_ptr_ = sub_vertex_buffer_info_ptr;
    allocated_mesh.sub_index_buffer_info_ptr_ = sub_index_buffer_info_ptr;

    return ResultS<Nil>{};
  }

  VkDeviceSize new_vertex_buffer_offset{0}, new_index_buffer_offset{0};
  if (vertex_iter != sub_vertex_buffer_list_.begin()) {
    std::list<BufferSubResourceAttribute>::iterator pre_vertex_iter{
        vertex_iter};
    --pre_vertex_iter;
    new_vertex_buffer_offset =
        pre_vertex_iter->GetOffset() + pre_vertex_iter->GetSize();
  }
  if (index_iter != sub_index_buffer_list_.begin()) {
    std::list<BufferSubResourceAttribute>::iterator pre_index_iter{index_iter};
    --pre_index_iter;
    new_index_buffer_offset =
        pre_index_iter->GetOffset() + pre_index_iter->GetSize();
  }
  vertex_iter =
      sub_vertex_buffer_list_.emplace(vertex_iter, new_vertex_buffer_offset,
                                      require_vertex_size, graph_queue_index);
  index_iter =
      sub_index_buffer_list_.emplace(index_iter, new_index_buffer_offset,
                                     require_index_size, graph_queue_index);
  capacity_data_.vertex_buffer_remaining_capacity_ -= require_vertex_size;
  capacity_data_.index_buffer_remaining_capacity_ -= require_index_size;
  guard.unlock();
  if (allocated_mesh.IsValid()) {
    allocated_mesh.mesh_buffer_manager_->FreeMeshBuffer(allocated_mesh);
  }
  allocated_mesh.mesh_buffer_manager_ = this;
  allocated_mesh.sub_vertex_buffer_info_ptr_ = &(*vertex_iter);
  allocated_mesh.sub_index_buffer_info_ptr_ = &(*index_iter);

  return ResultS<Nil>{};
}

Result<AllocatedMesh> MeshBufferManager::AllocateMeshBuffer(
    VkDeviceSize require_vertex_size, VkDeviceSize require_index_size) {
  AllocatedMesh allocated_mesh_buffer{};
  Result<Nil> allocate_result = AllocateMeshBuffer(
      require_vertex_size, require_index_size, allocated_mesh_buffer);
  if (allocate_result.IgnoreException().IsError()) {
    return ResultE<>{allocate_result.GetError().GetErrorCode()};
  }

  return ResultS{std::move(allocated_mesh_buffer)};
}

VkBuffer MeshBufferManager::GetVertexBuffer() {
  return managed_allocated_mesh_buffer_.GetVertexBuffer();
}

const VkBuffer_T* MeshBufferManager::GetVertexBuffer() const {
  return managed_allocated_mesh_buffer_.GetVertexBuffer();
}

VkBuffer MeshBufferManager::GetIndexBuffer() {
  return managed_allocated_mesh_buffer_.GetIndexBuffer();
}

const VkBuffer_T* MeshBufferManager::GetIndexBuffer() const {
  return managed_allocated_mesh_buffer_.GetIndexBuffer();
}

AllocatedMeshBuffer& MeshBufferManager::GetAllocatedMeshBuffer() {
  return managed_allocated_mesh_buffer_;
}

const AllocatedMeshBuffer& MeshBufferManager::GetAllocatedMeshBuffer() const {
  return managed_allocated_mesh_buffer_;
}

const MeshBufferCapacityData& MeshBufferManager::GetMeshBufferCapacityData()
    const {
  return capacity_data_;
}

float MeshBufferManager::GetCapacityCoefficient() const {
  return capacity_data_.capacity_coefficient_;
}

float MeshBufferManager::GetExpansionCoefficient() const {
  return capacity_data_.expansion_coefficient_;
}

VkDeviceSize MeshBufferManager::GetIndexBufferRemainingCapacity() const {
  return capacity_data_.index_buffer_remaining_capacity_;
}

VkDeviceSize MeshBufferManager::GetVertexBufferRemainingCapacity() const {
  return capacity_data_.vertex_buffer_remaining_capacity_;
}

void MeshBufferManager::SetCapacityCoefficient(float capacity_coefficient) {
  capacity_data_.capacity_coefficient_ = capacity_coefficient;
}

void MeshBufferManager::SetExpansionCoefficient(float expansion_coefficient) {
  capacity_data_.expansion_coefficient_ = expansion_coefficient;
}

Result<Nil> MeshBufferManager::RemoveBufferFragmentation() {
  std::lock_guard guard(allocate_free_mutex_);
  if (!IsValid()) {
    MM_LOG_ERROR("MM::RenderSystem::MeshBufferManager is invalid.");
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  RenderEngine* render_engine =
      managed_allocated_mesh_buffer_.GetRenderEnginePtr();
  CommandExecutorLockGuard command_executor_lock_guard =
      render_engine->GetCommandExecutorLockGuard();
  while (!render_engine->CommandExecutorIsFree()) {
    render_engine->GraphQueueWaitIdle();
    render_engine->ComputeQueueWaitIdle();
    render_engine->TransformQueueWaitIdle();
    render_engine->PresentQueueWaitIdle();
  }

  if (auto if_result = RemoveBufferFragmentationImp(
          managed_allocated_mesh_buffer_, sub_vertex_buffer_list_,
          sub_index_buffer_list_);
      if_result
          .Exception(
              MM_ERROR_DESCRIPTION2("Failed to remove buffer fragmentaion."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

Result<Nil> MeshBufferManager::Reserve(VkDeviceSize new_vertex_buffer_size,
                                       VkDeviceSize new_index_buffer_size) {
  std::lock_guard guard{allocate_free_mutex_};
  if (!IsValid()) {
    MM_LOG_ERROR("MM::RenderSystem::MeshBufferManager is invalid.");
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }
  const bool new_vertex_size_is_less =
      new_vertex_buffer_size <= managed_allocated_mesh_buffer_.GetVertexSize();
  const bool new_index_size_is_less =
      new_index_buffer_size <= managed_allocated_mesh_buffer_.GetIndexSize();
  if (new_vertex_size_is_less && new_index_size_is_less) {
    return ResultS<Nil>{};
  }
  if (new_vertex_size_is_less ^ new_index_size_is_less) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  if (auto if_result =
          ReserveImp(new_vertex_buffer_size, new_index_buffer_size);
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Failed to reserve mesh buffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

bool MeshBufferManager::IsValid() const { return is_valid; }

void MeshBufferManager::FreeMeshBuffer(AllocatedMesh& allocated_mesh) {
  assert(allocated_mesh.mesh_buffer_manager_ == this);

  if (!allocated_mesh.IsValid()) {
    return;
  }

  std::lock_guard guard{allocate_free_mutex_};

  capacity_data_.vertex_buffer_remaining_capacity_ +=
      allocated_mesh.GetVertexSize();
  capacity_data_.index_buffer_remaining_capacity_ +=
      allocated_mesh.GetIndexSize();

  bool is_erase = false;
  for (auto vertex_iter = sub_vertex_buffer_list_.begin();
       vertex_iter != sub_vertex_buffer_list_.end(); ++vertex_iter) {
    if (&(*vertex_iter) == allocated_mesh.sub_vertex_buffer_info_ptr_) {
      sub_vertex_buffer_list_.erase(vertex_iter);
      is_erase = true;
      break;
    }
  }
  assert(is_erase);
  is_erase = false;
  for (auto index_iter = sub_index_buffer_list_.begin();
       index_iter != sub_index_buffer_list_.end(); ++index_iter) {
    if (&(*index_iter) == allocated_mesh.sub_index_buffer_info_ptr_) {
      sub_index_buffer_list_.erase(index_iter);
      is_erase = true;
    }
  }
  assert(is_erase);

  allocated_mesh.mesh_buffer_manager_ = nullptr;
  allocated_mesh.sub_vertex_buffer_info_ptr_ = nullptr;
  allocated_mesh.sub_index_buffer_info_ptr_ = nullptr;
}

Result<Nil> MeshBufferManager::RemoveBufferFragmentationImp(
    AllocatedMeshBuffer& buffer,
    std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
    std::list<BufferSubResourceAttribute>& index_buffet_chunks_info) {
  RenderEngine* render_engine =
      managed_allocated_mesh_buffer_.GetRenderEnginePtr();
  assert(render_engine->IsValid());
#ifdef MM_CHECK_PARAMETERS
  if (!buffer.IsValid()) {
    MM_LOG_ERROR("buffer is invalid.");
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }
  if (vertex_buffer_chunks_info.empty() || index_buffet_chunks_info.empty()) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }
#endif

  // Used to mark buffer chunk that will be moved to the stage buffer
  VkDeviceSize vertex_stage_buffer_size = 0;
  std::vector<VkBufferCopy2> vertex_self_copy_regions;
  std::vector<VkBufferCopy2> vertex_self_copy_to_stage_regions;
  std::vector<VkBufferCopy2> vertex_stage_copy_to_self_regions;
  VkDeviceSize index_stage_buffer_size = 0;
  std::vector<VkBufferCopy2> index_self_copy_regions;
  std::vector<VkBufferCopy2> index_self_copy_to_stage_regions;
  std::vector<VkBufferCopy2> index_stage_copy_to_self_regions;
  GetRemoveBufferFragmentationBufferCopy(
      vertex_buffer_chunks_info, vertex_stage_buffer_size,
      vertex_self_copy_regions, vertex_self_copy_to_stage_regions,
      vertex_stage_copy_to_self_regions);
  GetRemoveBufferFragmentationBufferCopy(
      vertex_buffer_chunks_info, index_stage_buffer_size,
      index_self_copy_regions, index_self_copy_to_stage_regions,
      index_stage_copy_to_self_regions);

  QueueIndex transform_queue_index = render_engine->GetTransformQueueIndex();
  VkBufferCreateInfo buffer_create_info{GetVkBufferCreateInfo(
      nullptr, 0, vertex_stage_buffer_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_SHARING_MODE_EXCLUSIVE, 1, &transform_queue_index)};
  VmaAllocationCreateInfo vma_allocation_create_info{
      0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, 0, 0, nullptr, nullptr, 0.5};
  AllocatedBuffer vertex_stage_buffer, index_stage_buffer;
  if (auto if_result = render_engine->CreateBuffer(
          "", buffer_create_info, vma_allocation_create_info, nullptr);
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Failed to create stage buffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }
  buffer_create_info.size = index_stage_buffer_size;
  if (auto if_result = render_engine->CreateBuffer(
          "", buffer_create_info, vma_allocation_create_info, nullptr);
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Failed to create stage buffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  // vertex
  auto vertex_self_copy_info = GetVkCopyBufferInfo2(
      nullptr, buffer.GetVertexBuffer(), buffer.GetVertexBuffer(),
      vertex_self_copy_regions.size(), vertex_self_copy_regions.data());
  auto vertex_self_copy_to_stage_info = GetVkCopyBufferInfo2(
      nullptr, buffer.GetVertexBuffer(), vertex_stage_buffer.GetBuffer(),
      vertex_self_copy_to_stage_regions.size(),
      vertex_self_copy_to_stage_regions.data());
  auto vertex_stage_copy_to_self_info = GetVkCopyBufferInfo2(
      nullptr, vertex_stage_buffer.GetBuffer(), buffer.GetVertexBuffer(),
      vertex_stage_copy_to_self_regions.size(),
      vertex_stage_copy_to_self_regions.data());

  // vertex
  auto index_self_copy_info = GetVkCopyBufferInfo2(
      nullptr, buffer.GetIndexBuffer(), buffer.GetIndexBuffer(),
      index_self_copy_regions.size(), index_self_copy_regions.data());
  auto index_self_copy_to_stage_info = GetVkCopyBufferInfo2(
      nullptr, buffer.GetIndexBuffer(), index_stage_buffer.GetBuffer(),
      index_self_copy_to_stage_regions.size(),
      index_self_copy_to_stage_regions.data());
  auto index_stage_copy_to_self_info = GetVkCopyBufferInfo2(
      nullptr, index_stage_buffer.GetBuffer(), buffer.GetIndexBuffer(),
      index_stage_copy_to_self_regions.size(),
      index_stage_copy_to_self_regions.data());

  CommandExecutorGeneralCommandBufferGuard command_buffer =
      render_engine->GetGeneralCommandBufferGuard(CommandBufferType::TRANSFORM);

  if (auto if_result = AddRemoveBufferFragmentationCommands(
          render_engine, *command_buffer.GetGeneralCommandBuffer(),
          vertex_buffer_chunks_info, index_buffet_chunks_info,
          vertex_self_copy_info, vertex_self_copy_to_stage_info,
          vertex_stage_copy_to_self_info, index_self_copy_info,
          index_self_copy_to_stage_info, index_stage_copy_to_self_info);
      if_result.Exception(MM_ERROR_DESCRIPTION2("Failed recored commands."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  VkCommandBuffer command_buffer_ptr =
      command_buffer.GetGeneralCommandBuffer()->GetCommandBuffer();
  VkFence fence = command_buffer.GetGeneralCommandBuffer()->GetFence();
  VkSubmitInfo submit_info{GetVkSubmitInfo(nullptr, 0, nullptr, nullptr, 1,
                                           &command_buffer_ptr, 0, nullptr)};
  if (auto if_result = ConvertVkResultToMMResult(vkQueueSubmit(
          render_engine->GetTransformQueue(), 1, &submit_info, fence));
      if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to submit command."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }
  vkWaitForFences(render_engine->GetDevice(), 1, &fence, false, 1000000000);
  if (auto if_result = ConvertVkResultToMMResult(
          vkGetFenceStatus(render_engine->GetDevice(), fence));
      if_result.IgnoreException().IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  VkDeviceSize new_offset = 0;
  for (auto& buffer_chunk_info : vertex_buffer_chunks_info) {
    buffer_chunk_info.SetOffset(new_offset);
    new_offset += buffer_chunk_info.GetSize();
  }
  new_offset = 0;
  for (auto& buffer_chunk_info : index_buffet_chunks_info) {
    buffer_chunk_info.SetOffset(new_offset);
    new_offset += buffer_chunk_info.GetSize();
  }

  return ResultS<Nil>{};
}

void MeshBufferManager::GetRemoveBufferFragmentationBufferCopy(
    std::list<BufferSubResourceAttribute>& buffer_chunks_info,
    VkDeviceSize& stage_buffer_size,
    std::vector<VkBufferCopy2>& self_copy_regions,
    std::vector<VkBufferCopy2>& self_copy_to_stage_regions,
    std::vector<VkBufferCopy2>& stage_copy_to_self_regions) {
  stage_buffer_size = 0;

  std::vector<uint8_t> stage_flag(buffer_chunks_info.size(), 0);
  VkDeviceSize pre_valid_size = 0;

  std::uint32_t index = 0;
  for (auto buffer_chunk_info = buffer_chunks_info.begin();
       buffer_chunk_info != buffer_chunks_info.end();
       ++buffer_chunk_info, ++index) {
    if (pre_valid_size + buffer_chunk_info->GetSize() >=
        buffer_chunk_info->GetOffset()) {
      stage_flag[index] = 1;
      self_copy_to_stage_regions.push_back(
          GetVkBufferCopy2(buffer_chunk_info->GetSize(),
                           buffer_chunk_info->GetOffset(), stage_buffer_size));
      stage_copy_to_self_regions.push_back(GetVkBufferCopy2(
          buffer_chunk_info->GetSize(), stage_buffer_size, pre_valid_size));
      pre_valid_size += buffer_chunk_info->GetSize();
      stage_buffer_size += buffer_chunk_info->GetSize();
      continue;
    }

    // TODO Optimize to binary search.
    const VkDeviceSize new_offset = pre_valid_size;
    std::uint32_t index2 = 0;
    for (auto buffer_chunk_info2 = buffer_chunks_info.begin();
         buffer_chunk_info2 != buffer_chunk_info;
         ++buffer_chunk_info2, ++index2) {
      if (stage_flag[index2] != 1) {
        if (new_offset <
            buffer_chunk_info2->GetOffset() + buffer_chunk_info2->GetSize()) {
          if (new_offset + buffer_chunk_info->GetSize() >
              buffer_chunk_info2->GetOffset()) {
            stage_flag[index] = 1;
            self_copy_to_stage_regions.push_back(GetVkBufferCopy2(
                buffer_chunk_info->GetSize(), buffer_chunk_info->GetOffset(),
                stage_buffer_size));
            stage_copy_to_self_regions.push_back(
                GetVkBufferCopy2(buffer_chunk_info->GetSize(),
                                 stage_buffer_size, pre_valid_size));
            pre_valid_size += buffer_chunk_info->GetSize();
            stage_buffer_size += buffer_chunk_info->GetSize();
            continue;
          }
        }
      }
    }

    self_copy_regions.push_back(GetVkBufferCopy2(buffer_chunk_info->GetSize(),
                                                 buffer_chunk_info->GetOffset(),
                                                 pre_valid_size));
    pre_valid_size += buffer_chunk_info->GetSize();
  }
}

Result<Nil> MeshBufferManager::AddRemoveBufferFragmentationCommands(
    const RenderEngine* this_render_engine, AllocatedCommandBuffer& cmd,
    const std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
    const std::list<BufferSubResourceAttribute>& index_buffet_chunks_info,
    const VkCopyBufferInfo2& vertex_self_copy_info,
    const VkCopyBufferInfo2& vertex_self_copy_to_stage_info,
    const VkCopyBufferInfo2& vertex_stage_copy_to_self_info,
    const VkCopyBufferInfo2& index_self_copy_info,
    const VkCopyBufferInfo2& index_self_copy_to_stage_info,
    const VkCopyBufferInfo2& index_stage_copy_to_self_info) {
  if (auto if_result = BeginCommandBuffer(cmd);
      if_result
          .Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  const QueueIndex transform_queue_index =
      this_render_engine->GetTransformQueueIndex();
  std::vector<VkBufferMemoryBarrier2> barriers;
  barriers.reserve(vertex_buffer_chunks_info.size());
  for (const auto& buffer_chunk_info : vertex_buffer_chunks_info) {
    barriers.emplace_back(GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, buffer_chunk_info.GetQueueIndex(),
        transform_queue_index, vertex_self_copy_info.srcBuffer,
        buffer_chunk_info.GetChunkInfo().GetOffset(),
        buffer_chunk_info.GetChunkInfo().GetSize()));
  }
  for (const auto& buffer_chunk_info : index_buffet_chunks_info) {
    barriers.emplace_back(GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, buffer_chunk_info.GetQueueIndex(),
        transform_queue_index, index_self_copy_info.srcBuffer,
        buffer_chunk_info.GetChunkInfo().GetOffset(),
        buffer_chunk_info.GetChunkInfo().GetSize()));
  }

  const VkDependencyInfo dependency_info{GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &vertex_self_copy_to_stage_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &vertex_self_copy_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &vertex_stage_copy_to_self_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &index_self_copy_to_stage_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &index_self_copy_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &index_stage_copy_to_self_info);

  for (auto& barrier : barriers) {
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstQueueFamilyIndex = barrier.srcQueueFamilyIndex;
    barrier.srcQueueFamilyIndex = transform_queue_index;
  }

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  if (auto if_result = EndCommandBuffer(cmd);
      if_result
          .Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

Result<Nil> MeshBufferManager::AddReserveCommands(
    AllocatedCommandBuffer& cmd, const VkDependencyInfo& dependency_info,
    const VkCopyBufferInfo2& vertex_buffer_copy_info,
    const VkCopyBufferInfo2& index_buffer_copy_info) {
  if (auto if_result = BeginCommandBuffer(cmd);
      if_result
          .Exception(MM_FATAL_DESCRIPTION2("Failed to begine command buffer"))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &vertex_buffer_copy_info);
  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &index_buffer_copy_info);

  for (std::uint64_t i = 0; i != dependency_info.bufferMemoryBarrierCount;
       ++i) {
    VkBufferMemoryBarrier2& barrier = const_cast<VkBufferMemoryBarrier2&>(
        dependency_info.pBufferMemoryBarriers[i]);
    std::swap(barrier.srcQueueFamilyIndex, barrier.dstQueueFamilyIndex);
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  }
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  if (auto if_result = EndCommandBuffer(cmd);
      if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer"))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

RenderEngine* MeshBufferManager::GetRenderEnginePtr() {
  return managed_allocated_mesh_buffer_.GetRenderEnginePtr();
}

const RenderEngine* MeshBufferManager::GetRenderEnginePtr() const {
  return managed_allocated_mesh_buffer_.GetRenderEnginePtr();
}

Result<Nil> MeshBufferManager::Release() {
  std::lock_guard guard{allocate_free_mutex_};
  if ((!sub_vertex_buffer_list_.empty()) || (!sub_index_buffer_list_.empty())) {
    MM_LOG_ERROR(
        "There is a reference to the mesh buffer managed by this object and "
        "the object cannot be release.");
    return ResultE<>{ErrorCode::OPERATION_NOT_SUPPORTED};
  }

  managed_allocated_mesh_buffer_.Release();
  capacity_data_.Reset();

  is_valid = false;

  return ResultS<Nil>{};
}

Result<Nil> MeshBufferManager::ReserveStandard(
    VkDeviceSize require_vertex_size, VkDeviceSize require_index_size) {
  const VkDeviceSize vertex_buffer_size =
      GetAllocatedMeshBuffer().GetVertexSize();
  const VkDeviceSize index_buffer_size =
      GetAllocatedMeshBuffer().GetIndexSize();

  VkDeviceSize max_vertex_buffer_size = 0, max_index_buffer_size = 0;
  if (auto if_result = MM_CONFIG_SYSTEM->GetConfig("max_vertex_buffer_size",
                                                   max_vertex_buffer_size);
      if_result.IgnoreException().IsError()) {
    max_vertex_buffer_size = VK_WHOLE_SIZE;
  }
  if (auto if_result = MM_CONFIG_SYSTEM->GetConfig("max_index_buffer_size",
                                                   max_index_buffer_size);
      if_result.IgnoreException().IsError()) {
    max_index_buffer_size = VK_WHOLE_SIZE;
  }

  if (vertex_buffer_size >= max_vertex_buffer_size ||
      index_buffer_size >= max_vertex_buffer_size) {
    return ResultE<>{ErrorCode::OUT_OF_DEVICE_MEMORY};
  }

  if (require_vertex_size > max_vertex_buffer_size ||
      require_index_size > max_index_buffer_size) {
    return ResultE<>{ErrorCode::OUT_OF_DEVICE_MEMORY};
  }

  VkDeviceSize new_vertex_buffer_size = vertex_buffer_size,
               new_index_buffer_size = index_buffer_size;
  while (new_vertex_buffer_size < require_vertex_size) {
    new_vertex_buffer_size = std::ceil(new_vertex_buffer_size *
                                       capacity_data_.expansion_coefficient_);
  }
  while (new_index_buffer_size < require_index_size) {
    new_index_buffer_size = std::ceil(new_index_buffer_size *
                                      capacity_data_.expansion_coefficient_);
  }

  new_vertex_buffer_size =
      vertex_buffer_size * capacity_data_.expansion_coefficient_ <
              max_vertex_buffer_size
          ? vertex_buffer_size * capacity_data_.expansion_coefficient_
          : max_vertex_buffer_size;
  new_index_buffer_size =
      index_buffer_size * capacity_data_.expansion_coefficient_ <
              max_index_buffer_size
          ? index_buffer_size * capacity_data_.expansion_coefficient_
          : max_index_buffer_size;

  if (auto if_result =
          ReserveWithoutLock(new_vertex_buffer_size, new_index_buffer_size);
      if_result
          .Exception(MM_ERROR_DESCRIPTION2(
              "Failed to expand vertex buffer and index buffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

Result<Nil> MeshBufferManager::RemoveBufferFragmentationWithoutLock() {
  if (!IsValid()) {
    MM_LOG_ERROR("MM::RenderSystem::MeshBufferManager is invalid.");
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  RenderEngine* render_engine =
      managed_allocated_mesh_buffer_.GetRenderEnginePtr();
  CommandExecutorLockGuard command_executor_lock_guard =
      render_engine->GetCommandExecutorLockGuard();
  while (!render_engine->CommandExecutorIsFree()) {
    render_engine->GraphQueueWaitIdle();
    render_engine->ComputeQueueWaitIdle();
    render_engine->TransformQueueWaitIdle();
    render_engine->PresentQueueWaitIdle();
  }

  if (auto if_result = RemoveBufferFragmentationImp(
          managed_allocated_mesh_buffer_, sub_vertex_buffer_list_,
          sub_index_buffer_list_);
      if_result
          .Exception(
              MM_ERROR_DESCRIPTION2("Failed to remove buffer fragmentaion."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

Result<Nil> MeshBufferManager::ReserveImp(VkDeviceSize new_vertex_buffer_size,
                                          VkDeviceSize new_index_buffer_size) {
  RenderEngine* render_engine = managed_allocated_mesh_buffer_.GetRenderEnginePtr();

  AllocatedMeshBuffer new_mesh_buffer(render_engine, new_vertex_buffer_size,
                                      new_index_buffer_size);
  if (!new_mesh_buffer.IsValid()) {
    MM_LOG_ERROR("Failed to create a new mesh buffer.");
    return ResultE<>{ErrorCode::CREATE_OBJECT_FAILED};
  }

  CommandExecutorLockGuard command_executor_lock_guard =
      render_engine->GetCommandExecutorLockGuard();
  while (!render_engine->CommandExecutorIsFree()) {
    render_engine->GraphQueueWaitIdle();
    render_engine->ComputeQueueWaitIdle();
    render_engine->TransformQueueWaitIdle();
    render_engine->PresentQueueWaitIdle();
  }

  std::array<VkBufferMemoryBarrier2, 4> buffer_barriers{
      GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_READ_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          managed_allocated_mesh_buffer_.GetVertexBuffer(), 0,
          managed_allocated_mesh_buffer_.GetVertexSize()),
      GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_READ_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          managed_allocated_mesh_buffer_.GetIndexBuffer(), 0,
          managed_allocated_mesh_buffer_.GetIndexSize()),
      GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          new_mesh_buffer.GetVertexBuffer(), 0,
          new_mesh_buffer.GetVertexSize()),
      GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          new_mesh_buffer.GetIndexBuffer(), 0, new_mesh_buffer.GetIndexSize())};
  VkDependencyInfo dependency_info{GetVkDependencyInfo(
      0, nullptr, 4, buffer_barriers.data(), 0, nullptr, 0)};

  std::vector<VkBufferCopy2> regions{};
  regions.reserve(sub_vertex_buffer_list_.size() +
                  sub_index_buffer_list_.size());
  VkDeviceSize region_offset = 0;
  for (const auto& vertex_sub_resource : sub_vertex_buffer_list_) {
    regions.emplace_back(GetVkBufferCopy2(vertex_sub_resource.GetSize(),
                                          vertex_sub_resource.GetOffset(),
                                          region_offset));
    region_offset += vertex_sub_resource.GetSize();
  }
  for (const auto& index_sub_resource : sub_index_buffer_list_) {
    regions.emplace_back(GetVkBufferCopy2(index_sub_resource.GetSize(),
                                          index_sub_resource.GetOffset(),
                                          region_offset));
    region_offset += index_sub_resource.GetSize();
  }
  VkCopyBufferInfo2 vertex_copy_buffer_info{GetVkCopyBufferInfo2(
      nullptr, managed_allocated_mesh_buffer_.GetVertexBuffer(),
      new_mesh_buffer.GetVertexBuffer(), sub_vertex_buffer_list_.size(),
      regions.data())},
      index_copy_buffer_info{GetVkCopyBufferInfo2(
          nullptr, managed_allocated_mesh_buffer_.GetIndexBuffer(),
          new_mesh_buffer.GetIndexBuffer(), sub_index_buffer_list_.size(),
          regions.data() + sub_vertex_buffer_list_.size())};
  CommandExecutorGeneralCommandBufferGuard command_buffer =
      render_engine->GetGeneralCommandBufferGuard(CommandBufferType::TRANSFORM);

  if (auto if_result = AddReserveCommands(
          *command_buffer.GetGeneralCommandBuffer(), dependency_info,
          vertex_copy_buffer_info, index_copy_buffer_info);
      if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to record command."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  VkCommandBuffer command_buffer_ptr =
      command_buffer.GetGeneralCommandBuffer()->GetCommandBuffer();
  VkFence fence = command_buffer.GetGeneralCommandBuffer()->GetFence();
  VkSubmitInfo submit_info{GetVkSubmitInfo(nullptr, 0, nullptr, nullptr, 1,
                                           &command_buffer_ptr, 0, nullptr)};
  if (auto if_result = ConvertVkResultToMMResult(vkQueueSubmit(
          render_engine->GetTransformQueue(), 1, &submit_info, fence));
      if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to submit command."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }
  vkWaitForFences(render_engine->GetDevice(), 1, &fence, false, 100000000000);
  if (auto if_result = ConvertVkResultToMMResult(
          vkGetFenceStatus(render_engine->GetDevice(), fence));
      if_result.Exception(MM_FATAL_DESCRIPTION2("Gpu execute command timeout."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }
  vkResetFences(render_engine->GetDevice(), 1, &fence);

  VkDeviceSize new_offset = 0;
  for (auto& buffer_chunk_info : sub_vertex_buffer_list_) {
    buffer_chunk_info.SetOffset(new_offset);
    new_offset += buffer_chunk_info.GetSize();
  }
  new_offset = 0;
  for (auto& buffer_chunk_info : sub_index_buffer_list_) {
    buffer_chunk_info.SetOffset(new_offset);
    new_offset += buffer_chunk_info.GetSize();
  }

  managed_allocated_mesh_buffer_ = std::move(new_mesh_buffer);
  capacity_data_.vertex_buffer_remaining_capacity_ =
      new_vertex_buffer_size - capacity_data_.vertex_buffer_remaining_capacity_;
  capacity_data_.index_buffer_remaining_capacity_ =
      new_index_buffer_size - capacity_data_.index_buffer_remaining_capacity_;

  return ResultS<Nil>{};
}

Result<Nil> MeshBufferManager::ReserveWithoutLock(
    VkDeviceSize new_vertex_buffer_size, VkDeviceSize new_index_buffer_size) {
  const bool new_vertex_size_is_less =
      new_vertex_buffer_size <= managed_allocated_mesh_buffer_.GetVertexSize();
  const bool new_index_size_is_less =
      new_index_buffer_size <= managed_allocated_mesh_buffer_.GetIndexSize();
  if (new_vertex_size_is_less && new_index_size_is_less) {
    return ResultS<Nil>{};
  }
  if (new_vertex_size_is_less ^ new_index_size_is_less) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  if (auto if_result =
          ReserveImp(new_vertex_buffer_size, new_index_buffer_size);
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Failed to reserve mesh buffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}
}  // namespace RenderSystem
}  // namespace MM
