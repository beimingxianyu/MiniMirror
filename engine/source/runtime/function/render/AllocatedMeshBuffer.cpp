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
      vertex_info_(),
      index_info_(),
      vertex_buffer_(),
      index_buffer_() {
  if (render_engine_ == nullptr && render_engine_->IsValid()) {
    render_engine_ = nullptr;
    LOG_ERROR("Failed to create allocated mesh buffer.");
    return;
  }

  const auto vertex_buffer_size =
      std::stoull(CONFIG_SYSTEM->GetConfig("init_vertex_buffer_size"));

  const auto index_buffer_size =
      std::stoull(CONFIG_SYSTEM->GetConfig("init_index_buffer_size"));
  InitMeshBuffer(vertex_buffer_size, index_buffer_size);
}

AllocatedMeshBuffer::AllocatedMeshBuffer(RenderEngine *render_engine,
                                         std::uint32_t vertex_count)
    : MMObject(),
      render_engine_(render_engine),
      vertex_info_(),
      index_info_(),
      vertex_buffer_(),
      index_buffer_() {
  if (render_engine_ == nullptr && render_engine_->IsValid()) {
    render_engine_ = nullptr;
    LOG_ERROR("Failed to create allocated mesh buffer.");
    return;
  }

  std::uint64_t vertex_buffer_size =
                    vertex_count * sizeof(AssetSystem::AssetType::Vertex),
                index_buffer_size = vertex_buffer_size *
                                    sizeof(AssetSystem::AssetType::Vertex) /
                                    sizeof(std::uint32_t) * 4;
  InitMeshBuffer(vertex_buffer_size, index_buffer_size);
}

AllocatedMeshBuffer::AllocatedMeshBuffer(AllocatedMeshBuffer &&other) noexcept
    : render_engine_(other.render_engine_),
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
  return render_engine_ != nullptr && vertex_info_.IsValid() &&
         index_info_.IsValid() && vertex_buffer_.IsValid() &&
         index_buffer_.IsValid();
}

void AllocatedMeshBuffer::Release() {
  vertex_buffer_.Release();
  index_buffer_.Release();
  vertex_info_.Reset();
  index_info_.Reset();
  render_engine_ = nullptr;
}

ExecuteResult AllocatedMeshBuffer::InitMeshBuffer(
    std::uint64_t vertex_buffer_size, std::uint64_t index_buffer_size) {
  std::uint32_t graph_index = render_engine_->GetGraphQueueIndex();
  VkBufferCreateInfo vertex_buffer_create_info = Utils::GetVkBufferCreateInfo(
      nullptr, 0, vertex_buffer_size,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE, 1, &graph_index);
  VkBufferCreateInfo index_buffer_create_info = Utils::GetVkBufferCreateInfo(
      nullptr, 0, index_buffer_size,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE, 1, &graph_index);

  VmaAllocationCreateInfo vma_allocation_create_info =
      Utils::GetVmaAllocationCreateInfo(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
                                        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0,
                                        0, nullptr, nullptr, 0.5);

  VkBuffer temp_vertex_buffer{nullptr}, temp_index_buffer{nullptr};
  VmaAllocation temp_vertex_allocation{nullptr}, temp_index_allocation{nullptr};

  VK_CHECK(
      vmaCreateBuffer(render_engine_->GetAllocator(), &index_buffer_create_info,
                      &vma_allocation_create_info, &temp_index_buffer,
                      &temp_index_allocation, nullptr),
      LOG_ERROR("Failed to create index buffer.");
      render_engine_ = nullptr;
      return MM::Utils::ExecuteResult ::INITIALIZATION_FAILED;)

  VK_CHECK(
      vmaCreateBuffer(render_engine_->GetAllocator(),
                      &vertex_buffer_create_info, &vma_allocation_create_info,
                      &temp_vertex_buffer, &temp_vertex_allocation, nullptr),
      LOG_ERROR("Failed to create vertex buffer.");
      render_engine_ = nullptr;
      vmaDestroyBuffer(render_engine_->GetAllocator(), temp_index_buffer,
                       temp_index_allocation);
      return MM::Utils::ExecuteResult ::INITIALIZATION_FAILED;)

  vertex_info_.SetBufferCreateInfo(vertex_buffer_create_info);
  vertex_info_.SetAllocationCreateInfo(vma_allocation_create_info);
  index_info_.SetBufferCreateInfo(index_buffer_create_info);
  index_info_.SetAllocationCreateInfo(vma_allocation_create_info);

  vertex_buffer_ =
      AllocatedBufferWrapper(render_engine_->GetAllocator(), temp_vertex_buffer,
                             temp_vertex_allocation);
  index_buffer_ = AllocatedBufferWrapper(
      render_engine_->GetAllocator(), temp_index_buffer, temp_index_allocation);
}
}  // namespace RenderSystem
}  // namespace MM