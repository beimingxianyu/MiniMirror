//
// Created by beimingxianyu on 23-7-24.
//

#include "runtime/function/render/FrameBuffer.h"

#include "runtime/function/render/vk_engine.h"
#include "runtime/function/render/vk_type_define.h"
#include "runtime/function/render/vk_utils.h"
#include "utils/error.h"

MM::RenderSystem::FrameBuffer::~FrameBuffer() { Release(); }

MM::RenderSystem::FrameBuffer::FrameBuffer(
    RenderEngine* render_engine, VkAllocationCallbacks* allocator,
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
      render_engine_(render_engine),
      allocator_(allocator),
      frame_buffer_(nullptr) {
#ifdef MM_CHECK_PARAMETERS
  if (auto if_result =
          CheckInitParameters(render_engine, frame_buffer_create_info_);
      if_result.Exception(MM_ERROR_DESCRIPTION2("Input parameters is error."))
          .IsError()) {
    render_engine_ = nullptr;
    return;
  }
#endif

  const VkFramebufferCreateInfo temp_vk_frame_buffer_create_info =
      frame_buffer_create_info_.GetVkFrameBufferCreateInfo();
  if (auto if_result = ConvertVkResultToMMResult(vkCreateFramebuffer(
          render_engine_->GetDevice(), &temp_vk_frame_buffer_create_info,
          allocator, &frame_buffer_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkFramebuffer."))
          .IsError()) {
    return;
  }
}

MM::RenderSystem::FrameBuffer::FrameBuffer(
    RenderEngine* render_engine, VkAllocationCallbacks* allocator,
    const FrameBufferCreateInfo& frame_buffer_create_parameters)
    : frame_buffer_create_info_(frame_buffer_create_parameters),
      render_engine_(render_engine),
      allocator_(allocator),
      frame_buffer_(nullptr) {
#ifdef MM_CHECK_PARAMETERS
  if (auto if_result =
          CheckInitParameters(render_engine, frame_buffer_create_info_);
      if_result.Exception(MM_ERROR_DESCRIPTION2("Input parameters is error."))
          .IsError()) {
    return;
  }
#endif

  const VkFramebufferCreateInfo temp_vk_frame_buffer_create_info =
      frame_buffer_create_info_.GetVkFrameBufferCreateInfo();
  if (auto if_result = ConvertVkResultToMMResult(vkCreateFramebuffer(
          render_engine_->GetDevice(), &temp_vk_frame_buffer_create_info,
          allocator, &frame_buffer_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkFramebuffer."))
          .IsError()) {
    return;
  }
}

MM::RenderSystem::FrameBuffer::FrameBuffer(
    RenderEngine* render_Engine, VkAllocationCallbacks* allocator,
    FrameBufferCreateInfo&& frame_buffer_create_parameters)
    : frame_buffer_create_info_(std::move(frame_buffer_create_parameters)),
      render_engine_(render_Engine),
      allocator_(allocator),
      frame_buffer_(nullptr) {
#ifdef MM_CHECK_PARAMETERS
  if (auto if_result =
          CheckInitParameters(render_Engine, frame_buffer_create_parameters);
      if_result.Exception(MM_ERROR_DESCRIPTION2("Input parameters is error."))
          .IsError()) {
    return;
  }
#endif

  const VkFramebufferCreateInfo temp_vk_frame_buffer_create_info =
      frame_buffer_create_info_.GetVkFrameBufferCreateInfo();
  if (auto if_result = ConvertVkResultToMMResult(vkCreateFramebuffer(
          render_engine_->GetDevice(), &temp_vk_frame_buffer_create_info,
          allocator, &frame_buffer_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkFramebuffer."))
          .IsError()) {
    return;
  }
}

MM::RenderSystem::FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : frame_buffer_create_info_(std::move(other.frame_buffer_create_info_)),
      render_engine_(other.render_engine_),
      allocator_(other.allocator_),
      frame_buffer_(other.frame_buffer_) {
  other.render_engine_ = nullptr;
  other.allocator_ = nullptr;
  other.frame_buffer_ = nullptr;
}

MM::RenderSystem::FrameBuffer& MM::RenderSystem::FrameBuffer::operator=(
    FrameBuffer&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  frame_buffer_create_info_ = std::move(other.frame_buffer_create_info_);
  render_engine_ = other.render_engine_;
  allocator_ = other.allocator_;
  frame_buffer_ = other.frame_buffer_;

  other.render_engine_ = nullptr;
  other.allocator_ = nullptr;
  other.frame_buffer_ = nullptr;

  return *this;
}

const MM::RenderSystem::FrameBufferCreateInfo&
MM::RenderSystem::FrameBuffer::GetFrameBufferCreateInfo() const {
  return frame_buffer_create_info_;
}

std::vector<VkImageView> MM::RenderSystem::FrameBuffer::GetImageVIew() const {
  return frame_buffer_create_info_.attachments_;
}

VkDevice MM::RenderSystem::FrameBuffer::GetDevice() const {
  return render_engine_->GetDevice();
}

VkAllocationCallbacks* MM::RenderSystem::FrameBuffer::GetAlloctor() const {
  return allocator_;
}

VkFramebuffer MM::RenderSystem::FrameBuffer::GetFrameBuffere() {
  return frame_buffer_;
}

const VkFramebuffer_T* MM::RenderSystem::FrameBuffer::GetFrameBuffer() const {
  return frame_buffer_;
}

MM::Result<MM::RenderSystem::FrameBufferID>
MM::RenderSystem::FrameBuffer::GetFrameBufferID() const {
  return frame_buffer_create_info_.GetRenderFrameID();
}

MM::Result<MM::Nil> MM::RenderSystem::FrameBuffer::CheckInitParameters(
    const RenderEngine* render_engine,
    const FrameBufferCreateInfo& frame_buffer_create_info) {
  if (render_engine == nullptr || render_engine->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine must invalid.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }

  if (frame_buffer_create_info.flags_ == 0x7FFFFFFF) {
    MM_LOG_ERROR("The flags of VkFramebufferCreateInfo is error.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }
  if (frame_buffer_create_info.render_pass_ == nullptr) {
    MM_LOG_ERROR("The renderpass of VkFramebufferCreateInfo is error.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }
  for (const auto* attachment : frame_buffer_create_info.attachments_) {
    if (attachment == nullptr) {
      MM_LOG_ERROR("The attachment point is invalid.");
      return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
    }
  }
  if (frame_buffer_create_info.width_ == 0 ||
      frame_buffer_create_info.width_ > 8192) {
    MM_LOG_ERROR(
        "The width of the image must be not equal to 0 and not greater than "
        "8192.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }
  if (frame_buffer_create_info.height_ == 0 ||
      frame_buffer_create_info.height_ > 8192) {
    MM_LOG_ERROR(
        "The height of the image must be not equal to 0 and not greater than "
        "8192.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }
  if (frame_buffer_create_info.layers_ != 1) {
    MM_LOG_ERROR("The layers of image must 1.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }

  return ResultS<Nil>{};
}

bool MM::RenderSystem::FrameBuffer::IsValid() const {
  return render_engine_ != nullptr && allocator_ != nullptr &&
         frame_buffer_ != nullptr;
}

void MM::RenderSystem::FrameBuffer::Release() {
  if (IsValid()) {
    vkDestroyFramebuffer(render_engine_->GetDevice(), frame_buffer_,
                         allocator_);

    frame_buffer_create_info_.Reset();
    render_engine_ = nullptr;
    allocator_ = nullptr;
    frame_buffer_ = nullptr;
  }
}
