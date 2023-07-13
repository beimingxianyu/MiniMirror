//
// Created by beimingxianyu on 23-7-9.
//

#include "runtime/function/render/AllocatedMeshBuffer.h"

#include "runtime/function/render/vk_engine.h"

namespace MM {
namespace RenderSystem {
AllocatedMeshBuffer::AllocatedMeshBuffer(RenderEngine *render_engine)
    : MMObject(),
      render_engine_(render_engine),
      capacity_data_{0.9f, 2.0f, 0, 0},
      vertex_info_(),
      index_info_(),
      vertex_buffer_(),
      index_buffer_() {
  if (render_engine_ == nullptr) {
    LOG_ERROR("Failed to create allocated mesh buffer.");
    return;
  }

  VkBufferCreateInfo vertex_buffer_create_info = Utils::GetVkBufferCreateInfo();
  VkBufferCreateInfo index_buffer_create_info = Utils::GetVkBufferCreateInfo();

  VmaAllocationCreateInfo vma_allocation_create_info =
      Utils::GetVmaAllocationCreateInfo();

  VkBuffer temp_vertex_buffer{nullptr}, temp_index_buffer{nullptr};
  VmaAllocation temp_vertex_allocation{nullptr}, temp_index_allocation{nullptr};

  VK_CHECK(
      vmaCreateBuffer(render_engine_->GetAllocator(), &index_buffer_create_info,
                      &vma_allocation_create_info, &temp_index_buffer,
                      &temp_index_allocation, nullptr),
      LOG_ERROR("Failed to create index buffer.");
      return;)

  VK_CHECK(
      vmaCreateBuffer(render_engine_->GetAllocator(),
                      &vertex_buffer_create_info, &vma_allocation_create_info,
                      &temp_vertex_buffer, &temp_vertex_allocation, nullptr),
      LOG_ERROR("Failed to create vertex buffer.");
      vmaDestroyBuffer(render_engine_->GetAllocator(), temp_index_buffer,
                       temp_index_allocation);
      return;)

  vertex_info_.SetBufferCreateInfo(vertex_buffer_create_info);
  vertex_info_.SetAllocationCreateInfo(vma_allocation_create_info);
  index_info_.SetBufferCreateInfo(index_buffer_create_info);
  index_info_.SetAllocationCreateInfo(vma_allocation_create_info);

  vertex_buffer_ =
      AllocatedBufferWrapper(render_engine_->GetAllocator(), temp_vertex_buffer,
                             temp_vertex_allocation);
  index_buffer_ = AllocatedBufferWrapper(
      render_engine->GetAllocator(), temp_index_buffer, temp_index_allocation);
}

AllocatedMeshBuffer::AllocatedMeshBuffer(AllocatedMeshBuffer &&other) noexcept
    : render_engine_(other.render_engine_),
      capacity_data_(std::move(other.capacity_data_)),
      vertex_info_(std::move(other.vertex_info_)),
      index_info_(std::move(other.index_info_)),
      vertex_buffer_(std::move(other.vertex_buffer_)),
      index_buffer_(std::move(other.index_buffer_)) {
  other.render_engine_ = nullptr;
}

AllocatedMeshBuffer &AllocatedMeshBuffer::operator=(
    AllocatedMeshBuffer &&other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }
  render_engine_ = other.render_engine_;
  capacity_data_ = std::move(other.capacity_data_);
  vertex_info_ = std::move(other.vertex_info_);
  index_info_ = std::move(other.index_info_);
  vertex_buffer_ = std::move(other.vertex_buffer_);
  index_buffer_ = std::move(other.index_buffer_);

  other.render_engine_ = nullptr;

  return *this;
}

VkDeviceSize AllocatedMeshBuffer::GetVertexSize() const {
  return GetVertexBufferCreateInfo().size_;
}

VkDeviceSize AllocatedMeshBuffer::GetIndexSize() const {
  return GetIndexBufferCreateInfo().size_;
}

RenderEngine *AllocatedMeshBuffer::GetRenderEnginePtr() {
  return render_engine_;
}

const RenderEngine *AllocatedMeshBuffer::GetRenderEnginePtr() const {
  return render_engine_;
}

const MeshBufferCapacityData &AllocatedMeshBuffer::GetMeshBufferCapacityData()
    const {
  return capacity_data_;
}

float AllocatedMeshBuffer::GetCapacityCoefficient() const {
  return GetMeshBufferCapacityData().capacity_coefficient_;
}

float AllocatedMeshBuffer::GetExpansionCoefficient() const {
  return GetMeshBufferCapacityData().expansion_coefficient_;
}

VkDeviceSize AllocatedMeshBuffer::GetIndexBufferRemainingCapacity() const {
  return GetMeshBufferCapacityData().index_buffer_remaining_capacity_;
}

VkDeviceSize AllocatedMeshBuffer::GetVertexBufferRemainingCapacity() const {
  return GetMeshBufferCapacityData().vertex_buffer_remaining_capacity_;
}

void AllocatedMeshBuffer::SetCapacityCoefficient(float capacity_coefficient) {
  capacity_data_.capacity_coefficient_ = capacity_coefficient;
}

void AllocatedMeshBuffer::SetExpansionCoefficient(float expansion_coefficient) {
  capacity_data_.expansion_coefficient_ = expansion_coefficient;
}

const MeshVertexInfo &AllocatedMeshBuffer::GetMeshVertexInfo() const {
  return vertex_info_;
}

const MeshIndexInfo &AllocatedMeshBuffer::GetMeshIndexInfo() const {
  return index_info_;
}

const BufferCreateInfo &AllocatedMeshBuffer::GetVertexBufferCreateInfo() const {
  return GetMeshVertexInfo().buffer_create_info_;
}

const BufferCreateInfo &AllocatedMeshBuffer::GetIndexBufferCreateInfo() const {
  return GetMeshIndexInfo().buffer_create_info_;
}

const AllocationCreateInfo &AllocatedMeshBuffer::GetVertexAllocationCreateInfo()
    const {
  return GetMeshVertexInfo().allocation_create_info_;
}

const AllocationCreateInfo &AllocatedMeshBuffer::GetIndexAllocationCreateInfo()
    const {
  return GetMeshIndexInfo().allocation_create_info_;
}

VmaAllocator AllocatedMeshBuffer::GetAllocator() {
  return vertex_buffer_.GetAllocator();
  // or return index_buffer_.GetAllocator();
}

const VmaAllocator_T *AllocatedMeshBuffer::GetAllocator() const {
  return vertex_buffer_.GetAllocator();
  // or return index_buffer_.GetAllocator();
}

VkBuffer AllocatedMeshBuffer::GetVertexBuffer() {
  return vertex_buffer_.GetBuffer();
}

VmaAllocation AllocatedMeshBuffer::GetVertexAllocation() {
  return vertex_buffer_.GetAllocation();
}

const VkBuffer_T *AllocatedMeshBuffer::GetVertexBuffer() const {
  return vertex_buffer_.GetBuffer();
}

const VmaAllocation_T *AllocatedMeshBuffer::GetVertexAllocation() const {
  return vertex_buffer_.GetAllocation();
}

VkBuffer AllocatedMeshBuffer::GetIndexBuffer() {
  return index_buffer_.GetBuffer();
}

VmaAllocation AllocatedMeshBuffer::GetIndexAllocation() {
  return index_buffer_.GetAllocation();
}

const VkBuffer_T *AllocatedMeshBuffer::GetIndexBuffer() const {
  return index_buffer_.GetBuffer();
}

const VmaAllocation_T *AllocatedMeshBuffer::GetIndexAllocation() const {
  return index_buffer_.GetAllocation();
}

bool AllocatedMeshBuffer::IsValid() const {
  return render_engine_ != nullptr && capacity_data_.IsValid() &&
         vertex_info_.IsValid() && index_info_.IsValid() &&
         vertex_buffer_.IsValid() && index_buffer_.IsValid();
}

void AllocatedMeshBuffer::Release() {
  vertex_buffer_.Release();
  index_buffer_.Release();
  vertex_info_.Reset();
  index_info_.Reset();
  capacity_data_.Reset();
  render_engine_ = nullptr;
}
}  // namespace RenderSystem
}  // namespace MM