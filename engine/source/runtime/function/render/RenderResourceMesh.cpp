//
// Created by beimingxianyu on 23-7-27.
//

#include "runtime/function/render/RenderResourceMesh.h"

MM::RenderSystem::RenderResourceMesh::RenderResourceMesh(
    MM::RenderSystem::AllocatedMesh *allocated_mesh)
    : allocated_mesh_(allocated_mesh) {
  if (allocated_mesh_ == nullptr || allocated_mesh_->IsValid()) {
    MM_LOG_ERROR("The input parameter allocated_mesh is invalid.");
    allocated_mesh_ = nullptr;
    return;
  }
}

MM::RenderSystem::RenderResourceMesh::RenderResourceMesh(
    MM::RenderSystem::RenderResourceMesh &&other) noexcept
    : allocated_mesh_(other.allocated_mesh_) {
  other.Reset();
}
MM::RenderSystem::RenderResourceMesh &
MM::RenderSystem::RenderResourceMesh::operator=(
    MM::RenderSystem::RenderResourceMesh &&other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  allocated_mesh_ = other.allocated_mesh_;

  return *this;
}

VkBuffer MM::RenderSystem::RenderResourceMesh::GetVertexBuffer() {
  assert(false);
  return allocated_mesh_->GetVertexBuffer();
}

const VkBuffer_T *MM::RenderSystem::RenderResourceMesh::GetVertexBuffer()
    const {
  assert(false);
  return allocated_mesh_->GetVertexBuffer();
}

VkBuffer MM::RenderSystem::RenderResourceMesh::GetIndexBuffer() {
  assert(false);
  return allocated_mesh_->GetIndexBuffer();
}

const VkBuffer_T *MM::RenderSystem::RenderResourceMesh::GetIndexBuffer() const {
  assert(false);
  return allocated_mesh_->GetIndexBuffer();
}

const MM::RenderSystem::BufferChunkInfo &
MM::RenderSystem::RenderResourceMesh::GetVertexChunkInfo() const {
  assert(false);
  return allocated_mesh_->GetVertexChunkInfo();
}

const MM::RenderSystem::BufferChunkInfo &
MM::RenderSystem::RenderResourceMesh::GetIndexChunkInfo() const {
  assert(false);
  return allocated_mesh_->GetIndexChunkInfo();
}

std::uint32_t MM::RenderSystem::RenderResourceMesh::GetIndexCount() const {
  assert(false);
  return allocated_mesh_->GetIndexCount();
}

std::uint32_t MM::RenderSystem::RenderResourceMesh::GetIndexOffset() const {
  assert(false);
  return allocated_mesh_->GetIndexOffset() / sizeof(VertexIndex);
}

std::int32_t MM::RenderSystem::RenderResourceMesh::GetVertexOffset() const {
  assert(false);
  return static_cast<std::int32_t>(allocated_mesh_->GetVertexOffset() /
                                   sizeof(AssetSystem::AssetType::Vertex));
}

bool MM::RenderSystem::RenderResourceMesh::IsValid() const {
  return allocated_mesh_ != nullptr;
}

void MM::RenderSystem::RenderResourceMesh::Reset() {
  allocated_mesh_ = nullptr;
}
