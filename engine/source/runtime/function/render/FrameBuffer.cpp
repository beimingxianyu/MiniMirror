//
// Created by beimingxianyu on 23-7-24.
//

#include "runtime/function/render/FrameBuffer.h"

MM::RenderSystem::FrameBuffer::~FrameBuffer() {
  vkDestroyFramebuffer(device_, frame_buffer_, allocator_);
}

MM::RenderSystem::FrameBuffer::FrameBuffer(
    VkDevice device, VkAllocationCallbacks* allocator,
    const VkFramebufferCreateInfo& vk_frame_buffer_create_parameters)
    : frame_buffer_create_info_(
          vk_frame_buffer_create_parameters.pNext,
          vk_frame_buffer_create_parameters.flags,
          vk_frame_buffer_create_parameters.renderPass,
          vk_frame_buffer_create_parameters.attachmentCount,
          vk_frame_buffer_create_parameters.pAttachments,
          vk_frame_buffer_create_parameters.width,
          vk_frame_buffer_create_parameters.height,
          vk_frame_buffer_create_parameters.layers),
      device_(device),
      allocator_(allocator),
      frame_buffer_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, frame_buffer_create_info_),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  VkFramebufferCreateInfo temp_vk_frame_buffer_create_info =
      frame_buffer_create_info_.GetVkFrameBufferCreateInfo();
  MM_VK_CHECK(vkCreateFramebuffer(device_, &temp_vk_frame_buffer_create_info,
                                  allocator, &frame_buffer_),
              MM_LOG_ERROR("Filed to create VkFramebuffer.");
              return;)
}

MM::RenderSystem::FrameBuffer::FrameBuffer(
    VkDevice device, VkAllocationCallbacks* allocator,
    const FrameBufferCreateInfo& frame_buffer_create_info)
    : frame_buffer_create_info_(frame_buffer_create_info),
      device_(device),
      allocator_(allocator),
      frame_buffer_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, frame_buffer_create_info_),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  VkFramebufferCreateInfo temp_vk_frame_buffer_create_info =
      frame_buffer_create_info_.GetVkFrameBufferCreateInfo();
  MM_VK_CHECK(vkCreateFramebuffer(device_, &temp_vk_frame_buffer_create_info,
                                  allocator, &frame_buffer_),
              MM_LOG_ERROR("Filed to create VkFramebuffer.");
              return;)
}

MM::RenderSystem::FrameBuffer::FrameBuffer(
    VkDevice device, VkAllocationCallbacks* allocator,
    MM::RenderSystem::FrameBufferCreateInfo&& frame_buffer_create_parameters)
    : frame_buffer_create_info_(std::move(frame_buffer_create_parameters)),
      device_(device),
      allocator_(allocator),
      frame_buffer_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, frame_buffer_create_parameters),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  VkFramebufferCreateInfo temp_vk_frame_buffer_create_info =
      frame_buffer_create_info_.GetVkFrameBufferCreateInfo();
  MM_VK_CHECK(vkCreateFramebuffer(device_, &temp_vk_frame_buffer_create_info,
                                  allocator, &frame_buffer_),
              MM_LOG_ERROR("Filed to create VkFramebuffer.");
              return;)
}

MM::RenderSystem::FrameBuffer::FrameBuffer(
    MM::RenderSystem::FrameBuffer&& other) noexcept
    : frame_buffer_create_info_(std::move(other.frame_buffer_create_info_)),
      device_(other.device_),
      allocator_(other.allocator_),
      frame_buffer_(other.frame_buffer_) {
  other.device_ = nullptr;
  other.allocator_ = nullptr;
  other.frame_buffer_ = nullptr;
}

MM::RenderSystem::FrameBuffer& MM::RenderSystem::FrameBuffer::operator=(
    MM::RenderSystem::FrameBuffer&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  frame_buffer_create_info_ = std::move(other.frame_buffer_create_info_);
  device_ = other.device_;
  allocator_ = other.allocator_;
  frame_buffer_ = other.frame_buffer_;

  other.device_ = nullptr;
  other.allocator_ = nullptr;
  other.frame_buffer_ = nullptr;

  return *this;
}

const MM::RenderSystem::FrameBufferCreateInfo&
MM::RenderSystem::FrameBuffer::GetFrameBufferCreateInfo() const {
  return frame_buffer_create_info_;
}

const std::vector<VkImageView> MM::RenderSystem::FrameBuffer::GetImageVIew()
    const {
  return frame_buffer_create_info_.attachments_;
}

VkDevice MM::RenderSystem::FrameBuffer::GetDevice() const { return device_; }

VkAllocationCallbacks* MM::RenderSystem::FrameBuffer::GetAlloctor() const {
  return allocator_;
}

VkFramebuffer MM::RenderSystem::FrameBuffer::GetFrameBuffere() {
  return frame_buffer_;
}

const VkFramebuffer_T* MM::RenderSystem::FrameBuffer::GetFrameBuffer() const {
  return frame_buffer_;
}

MM::RenderSystem::FrameBufferID
MM::RenderSystem::FrameBuffer::GetFrameBUfferID() const {
  return FrameBuffer::GetFrameBUfferID(frame_buffer_create_info_);
}

MM::RenderSystem::FrameBufferID MM::RenderSystem::FrameBuffer::GetFrameBUfferID(
    const MM::RenderSystem::FrameBufferCreateInfo& frame_buffer_create_info) {
  FrameBufferID frame_buffer_ID{0, 0};

  std::uint64_t sub_ID = 0;

  // sub_ID1
  sub_ID |=
      reinterpret_cast<std::uint64_t>(frame_buffer_create_info.next_) & 0xFFFF;
  sub_ID |= frame_buffer_create_info.flags_ << 16;
  sub_ID |=
      (reinterpret_cast<std::uint64_t>(frame_buffer_create_info.render_pass_) &
       0xFFFF)
      << 17;
  sub_ID |= frame_buffer_create_info.attachments_.size() << 33;
  sub_ID |= static_cast<std::uint64_t>(frame_buffer_create_info.width_) << 38;
  sub_ID |= static_cast<std::uint64_t>(frame_buffer_create_info.height_) << 51;
  frame_buffer_ID.SetSubID1(sub_ID);
  sub_ID = 0;

  std::uint32_t cycle_offset = 6;
  // sub_ID2
  sub_ID |= frame_buffer_create_info.layers_;
  for (auto* attachment : frame_buffer_create_info.attachments_) {
    sub_ID |= (reinterpret_cast<std::uint64_t>(attachment) & 0xFFFF)
              << cycle_offset;
    cycle_offset += 16;
    if (cycle_offset > 63) cycle_offset -= 64;
  }
  frame_buffer_ID.SetSubID2(sub_ID);

  return frame_buffer_ID;
}

MM::ExecuteResult MM::RenderSystem::FrameBuffer::CheckInitParameters(
    VkDevice device,
    const MM::RenderSystem::FrameBufferCreateInfo& frame_buffer_create_info) {
  if (device == nullptr) {
    MM_LOG_ERROR("The input parameter device must invalid.");
    return MM::ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (frame_buffer_create_info.flags_ == 0x7FFFFFFF) {
    MM_LOG_ERROR("The flags of VkFramebufferCreateInfo is error.");
    return MM::ExecuteResult::INITIALIZATION_FAILED;
  }
  if (frame_buffer_create_info.render_pass_ == nullptr) {
    MM_LOG_ERROR("The renderpass of VkFramebufferCreateInfo is error.");
    return MM::ExecuteResult::INITIALIZATION_FAILED;
  }
  for (auto* attachment : frame_buffer_create_info.attachments_) {
    if (attachment == nullptr) {
      MM_LOG_ERROR("The attachment point is invalid.");
      return MM::ExecuteResult::INITIALIZATION_FAILED;
    }
  }
  if (frame_buffer_create_info.width_ == 0 ||
      frame_buffer_create_info.width_ > 8192) {
    MM_LOG_ERROR(
        "The width of the image must be not equal to 0 and not greater than "
        "8192.");
    return MM::ExecuteResult::INITIALIZATION_FAILED;
  }
  if (frame_buffer_create_info.height_ == 0 ||
      frame_buffer_create_info.height_ > 8192) {
    MM_LOG_ERROR(
        "The height of the image must be not equal to 0 and not greater than "
        "8192.");
    return MM::ExecuteResult::INITIALIZATION_FAILED;
  }
  if (frame_buffer_create_info.layers_ != 1) {
    MM_LOG_ERROR("The layers of image must 1.");
    return MM::ExecuteResult::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}
