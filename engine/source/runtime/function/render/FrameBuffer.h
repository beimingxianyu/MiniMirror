//
// Created by beimingxianyu on 23-7-24.
//
#pragma once

#include "runtime/function/render/vk_type_define.h"
#include "utils/ID.h"

namespace MM {
namespace RenderSystem {

class FrameBuffer {
 public:
  FrameBuffer() = default;
  ~FrameBuffer();
  FrameBuffer(RenderEngine* render_engine, VkAllocationCallbacks* allocator,
              const VkFramebufferCreateInfo& vk_frame_buffer_create_parameters);
  FrameBuffer(RenderEngine* render_engine, VkAllocationCallbacks* allocator,
              const FrameBufferCreateInfo& frame_buffer_create_parameters);
  FrameBuffer(RenderEngine* render_Engine, VkAllocationCallbacks* allocator,
              FrameBufferCreateInfo&& frame_buffer_create_parameters);
  FrameBuffer(const FrameBuffer& other) = delete;
  FrameBuffer(FrameBuffer&& other) noexcept;
  FrameBuffer& operator=(const FrameBuffer& other) = delete;
  FrameBuffer& operator=(FrameBuffer&& other) noexcept;

 public:
  const FrameBufferCreateInfo& GetFrameBufferCreateInfo() const;

  std::vector<VkImageView> GetImageVIew() const;

  VkDevice GetDevice() const;

  VkAllocationCallbacks* GetAlloctor() const;

  VkFramebuffer GetFrameBuffere();

  const VkFramebuffer_T* GetFrameBuffer() const;

  Result<FrameBufferID> GetFrameBufferID() const;

  bool IsValid() const;

  void Release();

 private:
  static Result<Nil> CheckInitParameters(
      const RenderEngine* render_engine,
      const FrameBufferCreateInfo& frame_buffer_create_info);

 private:
  FrameBufferCreateInfo frame_buffer_create_info_{};

  RenderEngine* render_engine_{nullptr};
  VkAllocationCallbacks* allocator_{nullptr};
  VkFramebuffer frame_buffer_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM
