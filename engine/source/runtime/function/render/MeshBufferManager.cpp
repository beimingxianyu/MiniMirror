//
// Created by beimingxianyu on 23-7-10.
//

#include "runtime/function/render/MeshBufferManager.h"

#include "runtime/function/render/vk_engine.h"

namespace MM {
namespace RenderSystem {
MeshBufferManager::MeshBufferManager(
    AllocatedMeshBuffer&& allocated_mesh_buffer)
    : managed_allocated_mesh_buffer_(std::move(allocated_mesh_buffer)),
      capacity_data_(0.9, 2, 0, 0),
      sub_index_buffer_list_(),
      sub_vertex_buffer_list_(),
      allocate_free_mutex_() {
  if (!managed_allocated_mesh_buffer_.IsValid()) {
    LOG_ERROR("Manage an invalid mesh buffer.");
    managed_allocated_mesh_buffer_.Release();
    capacity_data_.Reset();
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
    : managed_allocated_mesh_buffer_(std::move(allocated_mesh_buffer)),
      capacity_data_(0.9, 2, capacity_coefficient, expansion_coefficient),
      sub_index_buffer_list_(),
      sub_vertex_buffer_list_(),
      allocate_free_mutex_() {
  if (!managed_allocated_mesh_buffer_.IsValid()) {
    LOG_ERROR("Manage an invalid mesh buffer.");
    managed_allocated_mesh_buffer_.Release();
    capacity_data_.Reset();
    return;
  }
}

MeshBufferManager::MeshBufferManager(MeshBufferManager&& other) noexcept
    : managed_allocated_mesh_buffer_(),
      capacity_data_(),
      sub_vertex_buffer_list_(),
      sub_index_buffer_list_() {
  std::lock(allocate_free_mutex_, other.allocate_free_mutex_);
  std::lock_guard guard1(allocate_free_mutex_, std::adopt_lock),
      guard2(other.allocate_free_mutex_, std::adopt_lock);

  managed_allocated_mesh_buffer_ =
      std::move(other.managed_allocated_mesh_buffer_);
  capacity_data_ = std::move(other.capacity_data_);
  sub_vertex_buffer_list_ = std::move(other.sub_vertex_buffer_list_);
  sub_index_buffer_list_ = std::move(other.sub_index_buffer_list_);
}

MeshBufferManager& MeshBufferManager::operator=(
    MeshBufferManager&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  std::lock(allocate_free_mutex_, other.allocate_free_mutex_);
  std::lock_guard guard1(allocate_free_mutex_, std::adopt_lock),
      guard2(other.allocate_free_mutex_, std::adopt_lock);

  managed_allocated_mesh_buffer_ =
      std::move(other.managed_allocated_mesh_buffer_);
  capacity_data_ = std::move(other.capacity_data_);
  sub_vertex_buffer_list_ = std::move(other.sub_vertex_buffer_list_);
  sub_index_buffer_list_ = std::move(other.sub_index_buffer_list_);

  return *this;
}

ExecuteResult MeshBufferManager::AllocateMeshBuffer(
    VkDeviceSize vertex_size, VkDeviceSize index_size,
    AllocatedMesh& render_resource_mesh_buffer) {
  // TODO
  std::lock_guard guard(allocate_free_mutex_);

  VkDeviceSize vertex_buffer_size = GetAllocatedMeshBuffer().GetVertexSize(),
               index_buffer_size = GetAllocatedMeshBuffer().GetIndexSize();
  if ((capacity_data_.vertex_buffer_remaining_capacity_ <
       vertex_buffer_size * capacity_data_.capacity_coefficient_) ||
      (capacity_data_.index_buffer_remaining_capacity_ <
       index_buffer_size * capacity_data_.capacity_coefficient_)) {
    MM_CHECK(Reserve(vertex_buffer_size * capacity_data_.expansion_coefficient_,
                     index_buffer_size * capacity_data_.expansion_coefficient_),
             LOG_ERROR("Failed to expand vertex buffer and index buffer.");)
  }

  if (capacity_data_.vertex_buffer_remaining_capacity_ < vertex_size ||
      capacity_data_.index_buffer_remaining_capacity_ < index_size) {
    return ExecuteResult ::OUT_OF_DEVICE_MEMORY;
  }

  return ExecuteResult::SUCCESS;
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

ExecuteResult MeshBufferManager::RemoveBufferFragmentation() {
  if (!IsValid()) {
    LOG_ERROR("MM::RenderSystem::MeshBufferManager is invalid.");
    return MM::Utils::ExecuteResult ::OBJECT_IS_INVALID;
  }
  std::lock_guard guard(allocate_free_mutex_);

  RenderEngine* render_engine;
  CommandExecutorLockGuard command_executor_lock_guard =
      render_engine->GetCommandExecutorLockGuard();
  while (!render_engine->CommandExecutorIsFree()) {
    render_engine->GraphQueueWaitIdle();
    render_engine->ComputeQueueWaitIdle();
    render_engine->TransformQueueWaitIdle();
    render_engine->PresentQueueWaitIdle();
  }

  MM_CHECK(RemoveBufferFragmentationImp(managed_allocated_mesh_buffer_,
                                        sub_vertex_buffer_list_,
                                        sub_index_buffer_list_),
           LOG_ERROR("Failed to remove buffer fragmentaion.");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

ExecuteResult MeshBufferManager::Reserve(VkDeviceSize new_vertex_buffer_size,
                                         VkDeviceSize new_index_buffer_size) {
  std::lock_guard guard{allocate_free_mutex_};
  bool new_vertex_size_is_less =
      new_vertex_buffer_size <= managed_allocated_mesh_buffer_.GetVertexSize();
  bool new_index_size_is_less =
      new_index_buffer_size <= managed_allocated_mesh_buffer_.GetIndexSize();
  if (new_vertex_size_is_less && new_index_size_is_less) {
    return ExecuteResult ::SUCCESS;
  }
  if (new_vertex_size_is_less & new_index_buffer_size) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  RenderEngine* render_engine;

  AllocatedMeshBuffer new_mesh_buffer(render_engine, new_vertex_buffer_size,
                                      new_index_buffer_size);
  if (!new_mesh_buffer.IsValid()) {
    LOG_ERROR("Failed to create a new mesh buffer.");
    return ExecuteResult::CREATE_OBJECT_FAILED;
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
      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_READ_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          managed_allocated_mesh_buffer_.GetVertexBuffer(), 0,
          managed_allocated_mesh_buffer_.GetVertexSize()),
      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_READ_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          managed_allocated_mesh_buffer_.GetIndexBuffer(), 0,
          managed_allocated_mesh_buffer_.GetIndexSize()),
      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          new_mesh_buffer.GetVertexBuffer(), 0,
          new_mesh_buffer.GetVertexSize()),
      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, render_engine->GetComputeQueueIndex(),
          render_engine->GetTransformQueueIndex(),
          new_mesh_buffer.GetIndexBuffer(), 0, new_mesh_buffer.GetIndexSize())};
  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, 4, buffer_barriers.data(), 0, nullptr, 0)};

  std::vector<VkBufferCopy2> regions{};
  regions.reserve(sub_vertex_buffer_list_.size() +
                  sub_index_buffer_list_.size());
  VkDeviceSize region_offset = 0;
  for (const auto& vertex_sub_resource : sub_vertex_buffer_list_) {
    regions.emplace_back(MM::RenderSystem::Utils::GetVkBufferCopy2(
        vertex_sub_resource.GetSize(), vertex_sub_resource.GetOffset(),
        region_offset));
    region_offset += vertex_sub_resource.GetSize();
  }
  for (const auto& index_sub_resource : sub_index_buffer_list_) {
    regions.emplace_back(MM::RenderSystem::Utils::GetVkBufferCopy2(
        index_sub_resource.GetSize(), index_sub_resource.GetOffset(),
        region_offset));
    region_offset += index_sub_resource.GetSize();
  }
  VkCopyBufferInfo2 vertex_copy_buffer_info{
      MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
          nullptr, managed_allocated_mesh_buffer_.GetVertexBuffer(),
          new_mesh_buffer.GetVertexBuffer(), sub_vertex_buffer_list_.size(),
          regions.data())},
      index_copy_buffer_info{MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
          nullptr, managed_allocated_mesh_buffer_.GetIndexBuffer(),
          new_mesh_buffer.GetIndexBuffer(), sub_index_buffer_list_.size(),
          regions.data() + sub_vertex_buffer_list_.size())};
  CommandExecutorGeneralCommandBufferGuard command_buffer =
      render_engine->GetGeneralCommandBufferGuard(CommandBufferType::TRANSFORM);

  MM_CHECK(AddReserveCommands(*command_buffer.GetGeneralCommandBuffer(),
                              dependency_info, vertex_copy_buffer_info,
                              index_copy_buffer_info),
           LOG_ERROR("Failed to record command.");
           return MM_RESULT_CODE;)

  VkCommandBuffer command_buffer_ptr =
      command_buffer.GetGeneralCommandBuffer()->GetCommandBuffer();
  VkFence fence = command_buffer.GetGeneralCommandBuffer()->GetFence();
  VkSubmitInfo submit_info{MM::RenderSystem::Utils::GetVkSubmitInfo(
      nullptr, 0, nullptr, nullptr, 1, &command_buffer_ptr, 0, nullptr)};
  VK_CHECK(
      vkQueueSubmit(render_engine->GetTransformQueue(), 1, &submit_info, fence),
      LOG_ERROR("Failed to submit command.");
      return MM::RenderSystem::Utils::VkResultToMMResult(VK_RESULT_CODE);)
  vkWaitForFences(render_engine->GetDevice(), 1, &fence, false, 100000000000);
  VK_CHECK(vkGetFenceStatus(render_engine->GetDevice(), fence),
           LOG_FATAL("Gpu execute command timeout.");
           return MM::RenderSystem::Utils::VkResultToMMResult(VK_RESULT_CODE);)
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

  return ExecuteResult ::SUCCESS;
}

bool MeshBufferManager::IsValid() const {
  return managed_allocated_mesh_buffer_.IsValid() && capacity_data_.IsValid();
}

void MeshBufferManager::FreeMeshBuffer(
    BufferSubResourceAttribute* sub_vertex_buffer_info,
    BufferSubResourceAttribute* sub_index_buffer_info) {
  // TODO
}

ExecuteResult MeshBufferManager::RemoveBufferFragmentationImp(
    AllocatedMeshBuffer& buffer,
    std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
    std::list<BufferSubResourceAttribute>& index_buffet_chunks_info) {
  RenderEngine* render_engine =
      managed_allocated_mesh_buffer_.GetRenderEnginePtr();
  assert(render_engine->IsValid());
#ifdef CHECK_PARAMETERS
  if (!buffer.IsValid()) {
    LOG_ERROR("buffer is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }
  if (vertex_buffer_chunks_info.empty() || index_buffet_chunks_info.empty()) {
    return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
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
  VkBufferCreateInfo buffer_create_info{
      MM::RenderSystem::Utils::GetVkBufferCreateInfo(
          nullptr, 0, vertex_stage_buffer_size,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_SHARING_MODE_EXCLUSIVE, 1, &transform_queue_index)};
  VmaAllocationCreateInfo vma_allocation_create_info{
      0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, 0, 0, nullptr, nullptr, 0.5};
  AllocatedBuffer vertex_stage_buffer, index_stage_buffer;
  MM_CHECK(render_engine->CreateBuffer(buffer_create_info,
                                       vma_allocation_create_info, nullptr,
                                       vertex_stage_buffer),
           LOG_ERROR("Failed to create stage buffer.");
           return MM_RESULT_CODE;)
  buffer_create_info.size = index_stage_buffer_size;
  MM_CHECK(render_engine->CreateBuffer(buffer_create_info,
                                       vma_allocation_create_info, nullptr,
                                       index_stage_buffer),
           LOG_ERROR("Failed to create stage buffer.");
           return MM_RESULT_CODE;)

  // vertex
  auto vertex_self_copy_info = MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
      nullptr, buffer.GetVertexBuffer(), buffer.GetVertexBuffer(),
      vertex_self_copy_regions.size(), vertex_self_copy_regions.data());
  auto vertex_self_copy_to_stage_info = Utils::GetVkCopyBufferInfo2(
      nullptr, buffer.GetVertexBuffer(), vertex_stage_buffer.GetBuffer(),
      vertex_self_copy_to_stage_regions.size(),
      vertex_self_copy_to_stage_regions.data());
  auto vertex_stage_copy_to_self_info = Utils::GetVkCopyBufferInfo2(
      nullptr, vertex_stage_buffer.GetBuffer(), buffer.GetVertexBuffer(),
      vertex_stage_copy_to_self_regions.size(),
      vertex_stage_copy_to_self_regions.data());

  // vertex
  auto index_self_copy_info = MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
      nullptr, buffer.GetIndexBuffer(), buffer.GetIndexBuffer(),
      index_self_copy_regions.size(), index_self_copy_regions.data());
  auto index_self_copy_to_stage_info = Utils::GetVkCopyBufferInfo2(
      nullptr, buffer.GetIndexBuffer(), index_stage_buffer.GetBuffer(),
      index_self_copy_to_stage_regions.size(),
      index_self_copy_to_stage_regions.data());
  auto index_stage_copy_to_self_info = Utils::GetVkCopyBufferInfo2(
      nullptr, index_stage_buffer.GetBuffer(), buffer.GetIndexBuffer(),
      index_stage_copy_to_self_regions.size(),
      index_stage_copy_to_self_regions.data());

  CommandExecutorGeneralCommandBufferGuard command_buffer =
      render_engine->GetGeneralCommandBufferGuard(CommandBufferType::TRANSFORM);

  MM_CHECK(AddRemoveBufferFragmentationCommands(
               render_engine, *command_buffer.GetGeneralCommandBuffer(),
               vertex_buffer_chunks_info, index_buffet_chunks_info,
               vertex_self_copy_info, vertex_self_copy_to_stage_info,
               vertex_stage_copy_to_self_info, index_self_copy_info,
               index_self_copy_to_stage_info, index_stage_copy_to_self_info),
           LOG_ERROR("Failed recored commands.");
           return MM_RESULT_CODE;)

  VkCommandBuffer command_buffer_ptr =
      command_buffer.GetGeneralCommandBuffer()->GetCommandBuffer();
  VkFence fence = command_buffer.GetGeneralCommandBuffer()->GetFence();
  VkSubmitInfo submit_info{MM::RenderSystem::Utils::GetVkSubmitInfo(
      nullptr, 0, nullptr, nullptr, 1, &command_buffer_ptr, 0, nullptr)};
  VK_CHECK(
      vkQueueSubmit(render_engine->GetTransformQueue(), 1, &submit_info, fence),
      LOG_ERROR("Failed to submit command.");
      return MM::RenderSystem::Utils::VkResultToMMResult(VK_RESULT_CODE);)
  vkWaitForFences(render_engine->GetDevice(), 1, &fence, false, 1000000000);
  VK_CHECK(vkGetFenceStatus(render_engine->GetDevice(), fence),
           return MM::RenderSystem::Utils::VkResultToMMResult(VK_RESULT_CODE);)

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

  return ExecuteResult::SUCCESS;
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
      self_copy_to_stage_regions.push_back(Utils::GetVkBufferCopy2(
          buffer_chunk_info->GetSize(), buffer_chunk_info->GetOffset(),
          stage_buffer_size));
      stage_copy_to_self_regions.push_back(Utils::GetVkBufferCopy2(
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
            self_copy_to_stage_regions.push_back(Utils::GetVkBufferCopy2(
                buffer_chunk_info->GetSize(), buffer_chunk_info->GetOffset(),
                stage_buffer_size));
            stage_copy_to_self_regions.push_back(
                Utils::GetVkBufferCopy2(buffer_chunk_info->GetSize(),
                                        stage_buffer_size, pre_valid_size));
            pre_valid_size += buffer_chunk_info->GetSize();
            stage_buffer_size += buffer_chunk_info->GetSize();
            continue;
          }
        }
      }
    }

    self_copy_regions.push_back(Utils::GetVkBufferCopy2(
        buffer_chunk_info->GetSize(), buffer_chunk_info->GetOffset(),
        pre_valid_size));
    pre_valid_size += buffer_chunk_info->GetSize();
  }
}

MM::Utils::ExecuteResult
MeshBufferManager::AddRemoveBufferFragmentationCommands(
    RenderEngine* this_render_engine, AllocatedCommandBuffer& cmd,
    std::list<BufferSubResourceAttribute>& vertex_buffer_chunks_info,
    std::list<BufferSubResourceAttribute>& index_buffet_chunks_info,
    VkCopyBufferInfo2& vertex_self_copy_info,
    VkCopyBufferInfo2& vertex_self_copy_to_stage_info,
    VkCopyBufferInfo2& vertex_stage_copy_to_self_info,
    VkCopyBufferInfo2& index_self_copy_info,
    VkCopyBufferInfo2& index_self_copy_to_stage_info,
    VkCopyBufferInfo2& index_stage_copy_to_self_info) {
  MM_CHECK(Utils::BeginCommandBuffer(cmd),
           LOG_FATAL("Failed to begin command buffer.");
           return MM_RESULT_CODE;)

  QueueIndex transform_queue_index =
      this_render_engine->GetTransformQueueIndex();
  std::vector<VkBufferMemoryBarrier2> barriers;
  barriers.reserve(vertex_buffer_chunks_info.size());
  for (const auto& buffer_chunk_info : vertex_buffer_chunks_info) {
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, buffer_chunk_info.GetQueueIndex(),
        transform_queue_index, vertex_self_copy_info.srcBuffer,
        buffer_chunk_info.GetChunkInfo().GetOffset(),
        buffer_chunk_info.GetChunkInfo().GetSize()));
  }
  for (const auto& buffer_chunk_info : index_buffet_chunks_info) {
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, buffer_chunk_info.GetQueueIndex(),
        transform_queue_index, index_self_copy_info.srcBuffer,
        buffer_chunk_info.GetChunkInfo().GetOffset(),
        buffer_chunk_info.GetChunkInfo().GetSize()));
  }

  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
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

  MM_CHECK(Utils::EndCommandBuffer(cmd),
           LOG_FATAL("Failed to end command buffer.");
           return MM_RESULT_CODE;)

  return ExecuteResult::SUCCESS;
}

ExecuteResult MeshBufferManager::AddReserveCommands(
    AllocatedCommandBuffer& cmd, VkDependencyInfo& dependency_info,
    VkCopyBufferInfo2& vertex_buffer_copy_info,
    VkCopyBufferInfo2& index_buffer_copy_info) {
  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           LOG_FATAL("Failed to begine command buffer");
           return MM_RESULT_CODE;)

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

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           LOG_FATAL("Failed to end command buffer");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

RenderEngine* MeshBufferManager::GetRenderEnginePtr() {
  return managed_allocated_mesh_buffer_.GetRenderEnginePtr();
}

const RenderEngine* MeshBufferManager::GetRenderEnginePtr() const {
  return managed_allocated_mesh_buffer_.GetRenderEnginePtr();
}

ExecuteResult MeshBufferManager::Release() {
  // TODO
}
}  // namespace RenderSystem
}  // namespace MM