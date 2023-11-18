//
// Created by beimingxianyu on 23-7-27.
//

#include "runtime/function/render/RenderResourceBuffer.h"

MM::RenderSystem::RenderResourceBuffer::RenderResourceBuffer(
    AllocatedBuffer *allocated_buffer, VkDeviceSize offset,
    VkDeviceSize size)
    : allocated_buffer_(allocated_buffer), offset_(offset), size_(size) {
  if (allocated_buffer_ == nullptr || !allocated_buffer_->IsValid()) {
    Reset();
    MM_LOG_ERROR("The input parameter allocated_buffer is invalid.");
    return;
  }

  if (offset_ > allocated_buffer_->GetSize() ||
      size_ > allocated_buffer_->GetSize() - offset_) {
    Reset();
    MM_LOG_ERROR("The input parameters offset/size is invalid.");
  }
}

MM::RenderSystem::RenderResourceBuffer::RenderResourceBuffer(
    RenderResourceBuffer &&other) noexcept
    : allocated_buffer_(other.allocated_buffer_),
      offset_(other.offset_),
      size_(other.size_) {
  other.Reset();
}

MM::RenderSystem::RenderResourceBuffer &
MM::RenderSystem::RenderResourceBuffer::operator=(
    RenderResourceBuffer &&other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  allocated_buffer_ = other.allocated_buffer_;
  offset_ = other.offset_;
  size_ = other.size_;

  other.Reset();

  return *this;
}

const MM::RenderSystem::BufferDataInfo &
MM::RenderSystem::RenderResourceBuffer::GetBufferDataInfo() const {
  assert(IsValid());
  return allocated_buffer_->GetBufferDataInfo();
}

VkDeviceSize MM::RenderSystem::RenderResourceBuffer::GetOffset() const {
  assert(IsValid());
  return offset_;
}

VkDeviceSize MM::RenderSystem::RenderResourceBuffer::GetSize() const {
  assert(IsValid());
  return size_;
}

MM::RenderSystem::AllocatedBuffer &
MM::RenderSystem::RenderResourceBuffer::GetAllocatedBuffer() {
  assert(IsValid());
  return *allocated_buffer_;
}
const MM::RenderSystem::AllocatedBuffer &
MM::RenderSystem::RenderResourceBuffer::GetAllocatedBuffer() const {
  assert(IsValid());
  return *allocated_buffer_;
}

VkBuffer MM::RenderSystem::RenderResourceBuffer::GetBuffer() {
  assert(IsValid());
  return allocated_buffer_->GetBuffer();
}

const VkBuffer_T *MM::RenderSystem::RenderResourceBuffer::GetBuffer() const {
  assert(IsValid());
  return allocated_buffer_->GetBuffer();
}

bool MM::RenderSystem::RenderResourceBuffer::IsValid() const {
  return allocated_buffer_ != nullptr;
}

void MM::RenderSystem::RenderResourceBuffer::Reset() {
  allocated_buffer_ = nullptr;
  offset_ = UINT64_MAX;
  size_ = 0;
}
