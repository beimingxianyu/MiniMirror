//
// Created by beimingxianyu on 23-7-1.
//

#include "runtime/function/render/RenderResourceTexture.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    MM::RenderSystem::AllocatedImage* allocated_image,
    MM::RenderSystem::Sampler&& sampler,
    MM::RenderSystem::ImageView&& image_view)
    : allocated_image_(allocated_image),
      sampler_(std::move(sampler)),
      image_view_(std::move(image_view)) {
  if (allocated_image_ == nullptr || !allocated_image_->IsValid()) {
    MM_LOG_ERROR("The input parameter allocated_image is invalid.");
    allocated_image_ = nullptr;
    sampler_.Reset();
    image_view_.Release();
    return;
  }

  if (sampler_.IsValid()) {
    MM_LOG_ERROR("The input parameter sampler is invalid.");
    allocated_image_ = nullptr;
    sampler_.Reset();
    image_view_.Release();
    return;
  }

  if (image_view_.IsValid()) {
    MM_LOG_ERROR("The input parameter image_view is invalid.");
    allocated_image_ = nullptr;
    sampler_.Reset();
    image_view_.Release();
    return;
  }
}

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    MM::RenderSystem::AllocatedImage* allocated_image,
    const MM::RenderSystem::SamplerCreateInfo& sampler_create_info,
    const MM::RenderSystem::ImageViewCreateInfo& image_view_create_info,
    VkAllocationCallbacks* sampler_allocator,
    VkAllocationCallbacks* image_view_allocator)
    : allocated_image_(allocated_image), sampler_(), image_view_() {
  if (allocated_image_ == nullptr || !allocated_image_->IsValid()) {
    MM_LOG_ERROR("The input parameter allocated_image is invalid.");
    return;
  }

  sampler_ = Sampler{allocated_image_->GetRenderEnginePtr()->GetDevice(),
                     sampler_allocator, sampler_create_info};
  if (!sampler_.IsValid()) {
    allocated_image_ = nullptr;
    MM_LOG_ERROR("Failed to create sampler_.");
    return;
  }

  image_view_ = ImageView{allocated_image_->GetRenderEnginePtr()->GetDevice(),
                          image_view_allocator, image_view_create_info};
  if (!image_view_.IsValid()) {
    allocated_image_ = nullptr;
    sampler_.Reset();
    MM_LOG_ERROR("Failed to create image_view_.");
    return;
  }
}

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    MM::RenderSystem::AllocatedImage* allocated_image,
    const VkSamplerCreateInfo& vk_sampler_create_info,
    const VkImageViewCreateInfo& vk_image_view_create_info,
    VkAllocationCallbacks* sampler_allocator,
    VkAllocationCallbacks* image_view_allocator) {
  if (allocated_image_ == nullptr || !allocated_image_->IsValid()) {
    MM_LOG_ERROR("The input parameter allocated_image is invalid.");
    return;
  }

  sampler_ = Sampler{allocated_image_->GetRenderEnginePtr()->GetDevice(),
                     sampler_allocator, vk_sampler_create_info};
  if (!sampler_.IsValid()) {
    allocated_image_ = nullptr;
    MM_LOG_ERROR("Failed to create sampler_.");
    return;
  }

  image_view_ = ImageView{allocated_image_->GetRenderEnginePtr()->GetDevice(),
                          image_view_allocator, vk_image_view_create_info};
  if (!image_view_.IsValid()) {
    allocated_image_ = nullptr;
    sampler_.Reset();
    MM_LOG_ERROR("Failed to create image_view_.");
    return;
  }
}

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    MM::RenderSystem::RenderResourceTexture&& other) noexcept
    : allocated_image_(other.allocated_image_),
      sampler_(std::move(other.sampler_)),
      image_view_(std::move(other.image_view_)) {
  other.allocated_image_ = nullptr;
}

MM::RenderSystem::RenderResourceTexture&
MM::RenderSystem::RenderResourceTexture::operator=(
    MM::RenderSystem::RenderResourceTexture&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  allocated_image_ = other.allocated_image_;
  sampler_ = std::move(other.sampler_);
  image_view_ = std::move(other.image_view_);

  other.allocated_image_ = nullptr;

  return *this;
}

const MM::RenderSystem::ImageDataInfo&
MM::RenderSystem::RenderResourceTexture::GetImageDataInfo() const {
  assert(IsValid());
  return allocated_image_->GetImageDataInfo();
}

const MM::RenderSystem::SamplerCreateInfo&
MM::RenderSystem::RenderResourceTexture::GetSamplerCreateInfo() const {
  assert(IsValid());
  return sampler_.GetSamplerCreateInfo();
}

const MM::RenderSystem::ImageViewCreateInfo&
MM::RenderSystem::RenderResourceTexture::GetImageViewCreateInfo() const {
  assert(IsValid());
  return image_view_.GetImageViewCreateInfo();
}

MM::RenderSystem::AllocatedImage&
MM::RenderSystem::RenderResourceTexture::GetAllocatedImage() {
  assert(IsValid());
  return *allocated_image_;
}

const MM::RenderSystem::AllocatedImage&
MM::RenderSystem::RenderResourceTexture::GetAllocatedImage() const {
  assert(IsValid());
  return *allocated_image_;
}

MM::RenderSystem::Sampler& MM::RenderSystem::RenderResourceTexture::GetSampler()
    const {
  assert(IsValid());
  return sampler_;
}

MM::RenderSystem::ImageView&
MM::RenderSystem::RenderResourceTexture::GetImageView() {
  assert(IsValid());
  return image_view_;
}

const MM::RenderSystem::ImageView&
MM::RenderSystem::RenderResourceTexture::GetImageView() const {
  assert(IsValid());
  return image_view_;
}

VkImage MM::RenderSystem::RenderResourceTexture::GetImage() {
  assert(IsValid());
  return allocated_image_->GetImage();
}

const VkImage_T* MM::RenderSystem::RenderResourceTexture::GetImage() const {
  assert(IsValid());
  return allocated_image_->GetImage();
}

VkSampler MM::RenderSystem::RenderResourceTexture::GetVkSampler() const {
  assert(IsValid());
  return sampler_.GetVkSampler();
}

VkImageView MM::RenderSystem::RenderResourceTexture::GetVkImageView() {
  assert(IsValid());
  return image_view_.GetVkImageView();
}

const VkImageView_T* MM::RenderSystem::RenderResourceTexture::GetVkImageView()
    const {
  assert(IsValid());
  return image_view_.GetVkImageView();
}

bool MM::RenderSystem::RenderResourceTexture::IsValid() const {
  return allocated_image_ != nullptr;
}

void MM::RenderSystem::RenderResourceTexture::Reset() {
  allocated_image_ = nullptr;
  sampler_.Reset();
  image_view_.Release();
}
