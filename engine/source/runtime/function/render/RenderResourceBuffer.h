//
// Created by beimingxianyu on 23-7-27.
//
#pragma once

#include "runtime/function/render/AllocatedBuffer.h"

namespace MM {
namespace RenderSystem {
class RenderResourceBuffer {
 public:
  RenderResourceBuffer() = default;
  ~RenderResourceBuffer() = default;
  RenderResourceBuffer(AllocatedBuffer* allocated_buffer, VkDeviceSize offset,
                       VkDeviceSize size);
  RenderResourceBuffer(const RenderResourceBuffer& other) = delete;
  RenderResourceBuffer(RenderResourceBuffer&& other) noexcept;
  RenderResourceBuffer& operator=(const RenderResourceBuffer& other) = delete;
  RenderResourceBuffer& operator=(RenderResourceBuffer&& other) noexcept;

 public:
  const BufferDataInfo& GetBufferDataInfo() const;

  VkDeviceSize GetOffset() const;

  VkDeviceSize GetSize() const;

  AllocatedBuffer& GetAllocatedBuffer();

  const AllocatedBuffer& GetAllocatedBuffer() const;

  VkBuffer GetBuffer();

  const VkBuffer_T* GetBuffer() const;

  bool IsValid() const;

  void Reset();

 private:
  AllocatedBuffer* allocated_buffer_{};
  VkDeviceSize offset_{UINT64_MAX};
  VkDeviceSize size_{0};
};
}  // namespace RenderSystem
}  // namespace MM
