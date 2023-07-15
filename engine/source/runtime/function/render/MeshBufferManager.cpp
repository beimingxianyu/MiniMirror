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

MeshBufferManager::MeshBufferManager(MeshBufferManager&& other) noexcept {
  std::lock(allocate_free_mutex_, other.allocate_free_mutex_);
  std::lock_guard guard1(allocate_free_mutex_, std::adopt_lock),
      guard2(other.allocate_free_mutex_, std::adopt_lock);

  managed_allocated_mesh_buffer_ =
      std::move(other.managed_allocated_mesh_buffer_);
  capacity_data_ = std::move(other.capacity_data_);
  sub_vertex_buffer_list_ = std::move(other.sub_vertex_buffer_list_);
  sub_index_buffer_list_ = std::move(other.sub_index_buffer_list_);
}

ExecuteResult MeshBufferManager::AllocateMeshBuffer(
    RenderResourceMeshBuffer& render_resource_mesh_buffer) {
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
}

ExecuteResult MeshBufferManager::Reserve() {
  // TODO
}

bool MeshBufferManager::IsValid() const {
  return managed_allocated_mesh_buffer_.IsValid() && capacity_data_.IsValid();
}

ExecuteResult MeshBufferManager::Release() {
  // TODO
}

void MeshBufferManager::FreeMeshBuffer() {
  // TODO
}
}  // namespace RenderSystem
}  // namespace MM