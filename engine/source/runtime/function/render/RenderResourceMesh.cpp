//
// Created by beimingxianyu on 23-7-27.
//

#include "runtime/function/render/RenderResourceMesh.h"

MM::RenderSystem::RenderResourceMesh::RenderResourceMesh(
    AllocatedMesh *allocated_mesh)
    : allocated_mesh_(allocated_mesh) {
  if (allocated_mesh_ == nullptr || allocated_mesh_->IsValid()) {
    MM_LOG_ERROR("The input parameter allocated_mesh is invalid.");
    allocated_mesh_ = nullptr;
    return;
  }
}

MM::RenderSystem::RenderResourceMesh::RenderResourceMesh(
    RenderResourceMesh &&other) noexcept
    : allocated_mesh_(other.allocated_mesh_) {
  other.Reset();
}
MM::RenderSystem::RenderResourceMesh &
MM::RenderSystem::RenderResourceMesh::operator=(
    RenderResourceMesh &&other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  allocated_mesh_ = other.allocated_mesh_;

  return *this;
}

VkBuffer MM::RenderSystem::RenderResourceMesh::GetVertexBuffer() {
  assert(IsValid());
  return allocated_mesh_->GetVertexBuffer();
}

const VkBuffer_T *MM::RenderSystem::RenderResourceMesh::GetVertexBuffer()
    const {
  assert(IsValid());
  return allocated_mesh_->GetVertexBuffer();
}

VkBuffer MM::RenderSystem::RenderResourceMesh::GetIndexBuffer() {
  assert(IsValid());
  return allocated_mesh_->GetIndexBuffer();
}

const VkBuffer_T *MM::RenderSystem::RenderResourceMesh::GetIndexBuffer() const {
  assert(IsValid());
  return allocated_mesh_->GetIndexBuffer();
}

const MM::RenderSystem::BufferChunkInfo &
MM::RenderSystem::RenderResourceMesh::GetVertexChunkInfo() const {
  assert(IsValid());
  return allocated_mesh_->GetVertexChunkInfo();
}

const MM::RenderSystem::BufferChunkInfo &
MM::RenderSystem::RenderResourceMesh::GetIndexChunkInfo() const {
  assert(IsValid());
  return allocated_mesh_->GetIndexChunkInfo();
}

std::uint32_t MM::RenderSystem::RenderResourceMesh::GetIndexCount() const {
  assert(IsValid());
  return allocated_mesh_->GetIndexCount();
}

std::uint32_t MM::RenderSystem::RenderResourceMesh::GetIndexOffset() const {
  assert(IsValid());
  return allocated_mesh_->GetIndexOffset() / sizeof(VertexIndex);
}

std::int32_t MM::RenderSystem::RenderResourceMesh::GetVertexOffset() const {
  assert(IsValid());
  return allocated_mesh_->GetVertexOffset() /
         sizeof(AssetSystem::AssetType::Vertex);
}

bool MM::RenderSystem::RenderResourceMesh::IsValid() const {
  return allocated_mesh_ != nullptr;
}

void MM::RenderSystem::RenderResourceMesh::Reset() {
  allocated_mesh_ = nullptr;
}
