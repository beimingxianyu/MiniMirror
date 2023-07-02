//
// Created by beimingxianyu on 23-6-29.
//

#include "runtime/function/render/AllocatedImage.h"

#include <vulkan/vulkan_core.h>

#include "RenderDataAttributeID.h"
#include "RenderResourceDataBase.h"
#include "RenderResourceDataID.h"
#include "runtime/resource/asset_system/asset_type/Image.h"
#include "vk_engine.h"
#include "vk_type_define.h"
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

void MM::RenderSystem::AllocatedImage::Release() {
  image_data_info_.Reset();
  wrapper_.Release();
}

bool MM::RenderSystem::AllocatedImage::IsValid() const {
  return wrapper_.IsValid() && image_data_info_.IsValid();
}

MM::RenderSystem::ResourceType
MM::RenderSystem::AllocatedImage::GetResourceType() const {
  return ResourceType::Texture;
}

VkDeviceSize MM::RenderSystem::AllocatedImage::GetSize() const {
  return image_data_info_.image_create_info_.image_size_;
}

bool MM::RenderSystem::AllocatedImage::IsArray() const { return false; }

bool MM::RenderSystem::AllocatedImage::CanWrite() const {
  return (image_data_info_.image_create_info_.usage_ &
          VK_IMAGE_USAGE_STORAGE_BIT) != 0;
}

std::unique_ptr<MM::RenderSystem::RenderResourceDataBase>
MM::RenderSystem::AllocatedImage::GetCopy(
    const std::string& new_name_of_copy_resource) const {
  return RenderResourceDataBase::GetCopy(new_name_of_copy_resource);
}

MM::RenderSystem::AllocatedImage::AllocatedImage(
    const std::string& name, MM::RenderSystem::RenderEngine* render_engine,
    MM::AssetSystem::AssetManager::HandlerType image_handler,
    const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info)
    : RenderResourceDataBase(name, RenderResourceDataID{}),
      image_data_info_(),
      wrapper_() {
#ifdef CHECK_PARAMETERS
  MM_CHECK(
      CheckInitParameters(render_engine, image_handler, vk_image_create_info,
                          vma_allocation_create_info),
      return;)
#endif

  AssetSystem::AssetType::Image* image =
      static_cast<AssetSystem::AssetType::Image*>(&image_handler.GetAsset());

  image_data_info_.SetImageCreateInfo(image->GetImageSize(),
                                      *vk_image_create_info);
  image_data_info_.SetAllocationCreateInfo(*vma_allocation_create_info);
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::CheckImageHandler(
    const MM::AssetSystem::AssetManager::HandlerType& image_handler) {
  if (!image_handler.IsValid() ||
      image_handler.GetObject()->GetAssetType() !=
          AssetSystem::AssetType::AssetType::IMAGE) {
    LOG_ERROR(
        "The object pointed to by the image handle is not an image asset.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::CheckVkImageCreateInfo(
    const VkImageCreateInfo* vk_image_create_info) {
  if (vk_image_create_info == nullptr) {
    LOG_ERROR("The vk image create info is nullptr.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->sType != VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO) {
    LOG_ERROR("The vk image create info sType is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->flags == VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM) {
    LOG_ERROR("The vk image create info flags is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->imageType == VK_IMAGE_TYPE_MAX_ENUM) {
    LOG_ERROR("The vk image create info imageType is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->format == VK_FORMAT_MAX_ENUM) {
    LOG_ERROR("The vk image create info format is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->extent.width > 8192 ||
      vk_image_create_info->extent.height > 8192 ||
      vk_image_create_info->extent.depth > 8192) {
    LOG_ERROR("The vk image create info extent is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->mipLevels < 128) {
    LOG_ERROR("The vk image create info mipLevels is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->arrayLayers < 128) {
    LOG_ERROR("The vk image create info arrayLayers is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->samples == VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM) {
    LOG_ERROR("The vk image create info samples is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->tiling == VK_IMAGE_TILING_MAX_ENUM) {
    LOG_ERROR("The vk image create info tiling is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->usage == VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM) {
    LOG_ERROR("The vk image create info usage is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->sharingMode != VK_SHARING_MODE_EXCLUSIVE) {
    LOG_ERROR("The vk image create info flags is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_create_info->initialLayout == VK_IMAGE_LAYOUT_MAX_ENUM) {
    LOG_ERROR("The vk image create info initialLayout is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::CheckVmaAllocationCreateInfo(
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (vma_allocation_create_info == nullptr) {
    LOG_ERROR("The vma allocation create info is nullptr.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vma_allocation_create_info->flags ==
      VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM) {
    LOG_ERROR("The vma allocation create info flags is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vma_allocation_create_info->usage == VMA_MEMORY_USAGE_MAX_ENUM) {
    LOG_ERROR("The vma allocation create info usage is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vma_allocation_create_info->requiredFlags ==
          VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM ||
      vma_allocation_create_info->preferredFlags ==
          VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM) {
    LOG_ERROR(
        "The vma allocation create info requiredFlags/preferredFlags is "
        "error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vma_allocation_create_info->memoryTypeBits > 64) {
    LOG_ERROR("The vma allocation create info memoryTypeBits is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vma_allocation_create_info->priority > 0 ||
      vma_allocation_create_info->priority < 1) {
    LOG_ERROR("The vma allocation create info priority is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::CheckInitParameters(
    MM::RenderSystem::RenderEngine* render_engine,
    const MM::AssetSystem::AssetManager::HandlerType& image_handler,
    const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (render_engine == nullptr) {
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  MM_CHECK_WITHOUT_LOG(CheckImageHandler(image_handler), return MM_RESULT_CODE;)

  MM_CHECK_WITHOUT_LOG(CheckVkImageCreateInfo(vk_image_create_info),
                       return MM_RESULT_CODE;)

  MM_CHECK_WITHOUT_LOG(CheckVmaAllocationCreateInfo(vma_allocation_create_info),
                       return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
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
