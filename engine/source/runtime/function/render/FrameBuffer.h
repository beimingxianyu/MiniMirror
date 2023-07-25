//
// Created by beimingxianyu on 23-7-24.
//

#include "runtime/function/render/vk_type_define.h"
#include "utils/ID.h"

namespace MM {
namespace RenderSystem {

using FrameBufferID = MM::Utils::ID2;

class FrameBuffer {
 public:
  FrameBuffer() = default;
  ~FrameBuffer();
  FrameBuffer(VkDevice device, VkAllocationCallbacks* allocator,
              const VkFramebufferCreateInfo& vk_frame_buffer_create_parameters);
  FrameBuffer(VkDevice device, VkAllocationCallbacks* allocator,
              const FrameBufferCreateInfo& frame_buffer_create_parameters);
  FrameBuffer(VkDevice device, VkAllocationCallbacks* allocator,
              FrameBufferCreateInfo&& frame_buffer_create_parameters);
  FrameBuffer(const FrameBuffer& other) = delete;
  FrameBuffer(FrameBuffer&& other) noexcept;
  FrameBuffer& operator=(const FrameBuffer& other) = delete;
  FrameBuffer& operator=(FrameBuffer&& other) noexcept;

 public:
  const FrameBufferCreateInfo& GetFrameBufferCreateInfo() const;

  const std::vector<VkImageView> GetImageVIew() const;

  VkDevice GetDevice() const;

  VkAllocationCallbacks* GetAlloctor() const;

  VkFramebuffer GetFrameBuffere();

  const VkFramebuffer_T* GetFrameBuffer() const;

  FrameBufferID GetFrameBUfferID() const;

  static FrameBufferID GetFrameBUfferID(
      const FrameBufferCreateInfo& frame_buffer_create_info);

 private:
  ExecuteResult CheckInitParameters(
      VkDevice device, const FrameBufferCreateInfo& frame_buffer_create_info);

 private:
  FrameBufferCreateInfo frame_buffer_create_info_{};

  VkDevice device_{nullptr};
  VkAllocationCallbacks* allocator_{nullptr};
  VkFramebuffer frame_buffer_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM
