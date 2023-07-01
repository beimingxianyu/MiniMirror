//
// Created by beimingxianyu on 23-6-29.
//

#include "runtime/function/render/AllocatedImage.h"

#include "vk_utils.h"

MM::RenderSystem::AllocatedImage::AllocatedImage(
    const std::string& name,
    const RenderResourceDataID& render_resource_data_ID,
    const VmaAllocator& allocator, const VkImage& image,
    const VmaAllocation& allocation, const ImageDataInfo& image_data_info)
    : RenderResourceDataBase(name, render_resource_data_ID),
      image_data_info_(image_data_info),
      wrapper_(allocator, image, allocation) {}

MM::RenderSystem::AllocatedImage::AllocatedImage(
    AllocatedImage&& other) noexcept
    : RenderResourceDataBase(std::move(other)),
      image_data_info_(std::move(other.image_data_info_)),
      wrapper_(std::move(other.wrapper_)) {
  image_data_info_.Reset();
}

MM::RenderSystem::AllocatedImage& MM::RenderSystem::AllocatedImage::operator=(
    AllocatedImage&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  RenderResourceDataBase::operator=(std::move(other));
  image_data_info_ = std::move(other.image_data_info_);
  wrapper_ = std::move(other.wrapper_);

  other.image_data_info_.Reset();

  return *this;
}

const VkExtent3D& MM::RenderSystem::AllocatedImage::GetImageExtent() const {
  return image_data_info_.image_create_info_.extent_;
}

const VkDeviceSize& MM::RenderSystem::AllocatedImage::GetImageSize() const {
  return image_data_info_.image_create_info_.image_size_;
}

const VkFormat& MM::RenderSystem::AllocatedImage::GetImageFormat() const {
  return image_data_info_.image_create_info_.format_;
}

const VkImageLayout& MM::RenderSystem::AllocatedImage::GetImageLayout() const {
  return image_data_info_.image_create_info_.initial_layout_;
}

const uint32_t& MM::RenderSystem::AllocatedImage::GetMipmapLevels() const {
  return image_data_info_.image_create_info_.miplevels_;
}

const uint32_t& MM::RenderSystem::AllocatedImage::GetArrayLayers() const {
  return image_data_info_.image_create_info_.array_levels_;
}

bool MM::RenderSystem::AllocatedImage::CanMapped() const {
  return Utils::CanBeMapped(image_data_info_.allocation_create_info_.usage_,
                            image_data_info_.allocation_create_info_.flags_);
}

bool MM::RenderSystem::AllocatedImage::IsTransformSrc() const {
  return Utils::IsTransformSrcImage(image_data_info_.image_create_info_.usage_);
}

bool MM::RenderSystem::AllocatedImage::IsTransformDest() const {
  return Utils::IsTransformDestImage(
      image_data_info_.image_create_info_.usage_);
}

const MM::RenderSystem::ImageCreateInfo&
MM::RenderSystem::AllocatedImage::GetImageCreateInfo() const {
  return GetImageInfo().image_create_info_;
}

const MM::RenderSystem::AllocationCreateInfo&
MM::RenderSystem::AllocatedImage::GetAllocationCreateInfo() const {
  return GetImageInfo().allocation_create_info_;
}

const MM::RenderSystem::ImageDataInfo&
MM::RenderSystem::AllocatedImage::GetImageInfo() const {
  return image_data_info_;
}

VmaAllocator MM::RenderSystem::AllocatedImage::GetAllocator() const {
  return wrapper_.GetAllocator();
}

VkImage MM::RenderSystem::AllocatedImage::GetImage() const {
  return wrapper_.GetImage();
}

VmaAllocation MM::RenderSystem::AllocatedImage::GetAllocation() const {
  return wrapper_.GetAllocation();
}

const std::vector<std::uint32_t>&
MM::RenderSystem::AllocatedImage::GetQueueIndexes() const {
  return image_data_info_.image_create_info_.queue_family_indices_;
}

void MM::RenderSystem::AllocatedImage::Release() { wrapper_.Release(); }

bool MM::RenderSystem::AllocatedImage::IsValid() const {
  return wrapper_.IsValid() && image_data_info_.IsValid();
}

MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::AllocatedImageWrapper(
    const VmaAllocator& allocator, const VkImage& image,
    const VmaAllocation& allocation)
    : allocator_(allocator), image_(image), allocation_(allocation) {
  if (allocator_ == nullptr || image_ == nullptr || allocation_ == nullptr) {
    allocator_ = nullptr;
    image_ = nullptr;
    allocation_ = nullptr;
  }
}

MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::
    ~AllocatedImageWrapper() {
  Release();
}

MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::AllocatedImageWrapper(
    MM::RenderSystem::AllocatedImage::AllocatedImageWrapper&& other) noexcept
    : allocator_(other.allocator_),
      image_(other.image_),
      allocation_(other.allocation_) {
  other.allocator_ = nullptr;
  other.image_ = nullptr;
  other.allocation_ = nullptr;
}

MM::RenderSystem::AllocatedImage::AllocatedImageWrapper&
MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::operator=(
    MM::RenderSystem::AllocatedImage::AllocatedImageWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  allocator_ = other.allocator_;
  image_ = other.image_;
  allocation_ = other.allocation_;

  other.allocator_ = nullptr;
  other.image_ = nullptr;
  other.allocation_ = nullptr;

  return *this;
}

VmaAllocator
MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::GetAllocator() const {
  return allocator_;
}

VkImage MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::GetImage()
    const {
  return image_;
}

VmaAllocation
MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::GetAllocation() const {
  return allocation_;
}

bool MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::IsValid() const {
  return allocator_ != nullptr && image_ != nullptr && allocation_ != nullptr;
}

void MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::Release() {
  if (!IsValid()) {
    return;
  }

  vmaDestroyImage(allocator_, image_, allocation_);

  allocator_ = nullptr;
  image_ = nullptr;
  allocation_ = nullptr;
}
