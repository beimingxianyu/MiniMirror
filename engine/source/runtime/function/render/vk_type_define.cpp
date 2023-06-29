//
// Created by beimingxianyu on 23-6-29.
//
#include "runtime/function/render/vk_type_define.h"

#include <vulkan/vulkan_core.h>

#include "runtime/function/render/vk_engine.h"

bool MM::RenderSystem::QueueFamilyIndices::isComplete() const {
  return graphics_family_.has_value() && present_family_.has_value() &&
         compute_family_.has_value() && transform_family_.has_value();
}

void MM::RenderSystem::SemaphoreDestructor::operator()(VkSemaphore* value) {
  if (value == nullptr) {
    return;
  }

  vkDestroySemaphore(render_engine_->GetDevice(), *value, nullptr);

  value = nullptr;
}

MM::RenderSystem::ImageBindInfo::ImageBindInfo(
    const VkDescriptorSetLayoutBinding& bind,
    std::unique_ptr<VkImageView>&& image_view,
    std::unique_ptr<VkSampler>&& sampler)
    : bind_(bind),
      image_view_(std::move(image_view)),
      sampler_(std::move(sampler)) {}

MM::RenderSystem::ImageBindInfo::ImageBindInfo(ImageBindInfo&& other) noexcept
    : bind_(other.bind_),
      image_view_(std::move(other.image_view_)),
      sampler_(std::move(other.sampler_)) {
  other.Reset();
}

MM::RenderSystem::ImageBindInfo& MM::RenderSystem::ImageBindInfo::operator=(
    ImageBindInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  bind_ = other.bind_;
  image_view_ = std::move(other.image_view_);
  sampler_ = std::move(other.sampler_);

  other.Reset();

  return *this;
}

bool MM::RenderSystem::ImageBindInfo::IsValid() const {
  if (bind_.descriptorCount == 0 || image_view_ == nullptr ||
      sampler_ == nullptr) {
    return false;
  }

  return true;
}

void MM::RenderSystem::ImageBindInfo::Reset() {
  bind_.binding = 0;
  bind_.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
  bind_.descriptorCount = 0;
  bind_.pImmutableSamplers = nullptr;

  image_view_.reset();
  sampler_.reset();
}
bool MM::RenderSystem::ImageCreateInfo::IsValid() const {
  return image_size_ != 0;
}

void MM::RenderSystem::ImageCreateInfo::Reset() {
  image_size_ = 0;
  next_ = nullptr;
  flags_ = VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM;
  image_type_ = VK_IMAGE_TYPE_MAX_ENUM;
  format_ = VK_FORMAT_MAX_ENUM;
  extent_.width = 0;
  extent_.height = 0;
  extent_.depth = 0;
  miplevels_ = 0;
  array_levels_ = 0;
  samples_ = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
  tiling_ = VK_IMAGE_TILING_MAX_ENUM;
  usage_ = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
  sharing_mode_ = VK_SHARING_MODE_EXCLUSIVE;
  queue_family_indices_.clear();
  initial_layout_ = VK_IMAGE_LAYOUT_MAX_ENUM;
}
MM::RenderSystem::ImageCreateInfo::ImageCreateInfo(
    ImageCreateInfo&& other) noexcept
    : image_size_(other.image_size_),
      next_(other.next_),
      flags_(other.flags_),
      image_type_(other.image_type_),
      format_(other.format_),
      extent_(other.extent_),
      miplevels_(other.miplevels_),
      array_levels_(other.array_levels_),
      samples_(other.samples_),
      tiling_(other.tiling_),
      usage_(other.usage_),
      sharing_mode_(other.sharing_mode_),
      queue_family_indices_(std::move(other.queue_family_indices_)),
      initial_layout_(other.initial_layout_) {}

MM::RenderSystem::ImageCreateInfo& MM::RenderSystem::ImageCreateInfo::operator=(
    const MM::RenderSystem::ImageCreateInfo& other) {
  if (&other == this) {
    return *this;
  }

  image_size_ = other.image_size_;
  next_ = other.next_;
  flags_ = other.flags_;
  image_type_ = other.image_type_;
  format_ = other.format_;
  extent_ = other.extent_;
  miplevels_ = other.miplevels_;
  array_levels_ = other.array_levels_;
  samples_ = other.samples_;
  tiling_ = other.tiling_;
  usage_ = other.usage_;
  sharing_mode_ = other.sharing_mode_;
  queue_family_indices_ = other.queue_family_indices_;
  initial_layout_ = other.initial_layout_;

  return *this;
}

MM::RenderSystem::ImageCreateInfo& MM::RenderSystem::ImageCreateInfo::operator=(
    MM::RenderSystem::ImageCreateInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  image_size_ = other.image_size_;
  next_ = other.next_;
  flags_ = other.flags_;
  image_type_ = other.image_type_;
  format_ = other.format_;
  extent_ = other.extent_;
  miplevels_ = other.miplevels_;
  array_levels_ = other.array_levels_;
  samples_ = other.samples_;
  tiling_ = other.tiling_;
  usage_ = other.usage_;
  sharing_mode_ = other.sharing_mode_;
  queue_family_indices_ = std::move(other.queue_family_indices_);
  initial_layout_ = other.initial_layout_;

  other.Reset();

  return *this;
}

bool MM::RenderSystem::AllocationCreateInfo::IsValid() const {
  return flags_ != VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM &&
         usage_ != VMA_MEMORY_USAGE_MAX_ENUM;
}

void MM::RenderSystem::AllocationCreateInfo::Reset() {
  flags_ = VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM;
  usage_ = VMA_MEMORY_USAGE_MAX_ENUM;
  required_flags_ = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
  preferred_flags_ = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
  priority_ = 0;
}

bool MM::RenderSystem::ImageDataInfo::IsValid() const {
  return image_create_info_.IsValid() && allocation_create_info_.IsValid();
}

void MM::RenderSystem::ImageDataInfo::Reset() {
  image_create_info_.Reset();
  allocation_create_info_.Reset();
}
