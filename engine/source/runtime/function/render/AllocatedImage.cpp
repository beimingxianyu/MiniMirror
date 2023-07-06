//
// Created by beimingxianyu on 23-6-29.
//

#include "runtime/function/render/AllocatedImage.h"

#include <vulkan/vulkan_core.h>

#include "RenderDataAttributeID.h"
#include "RenderResourceDataBase.h"
#include "RenderResourceDataID.h"
#include "vk_engine.h"
#include "vk_type_define.h"
#include "vk_utils.h"

MM::RenderSystem::AllocatedImage::AllocatedImage(
    const std::string& name,
    const RenderResourceDataID& render_resource_data_ID,
    RenderEngine* render_engine, const ImageDataInfo& image_data_info,
    const VmaAllocator& allocator, const VkImage& image,
    const VmaAllocation& allocation, bool have_data)
    : RenderResourceDataBase(name, render_resource_data_ID),
      render_engine_(render_engine),
      image_data_info_(image_data_info),
      wrapper_(allocator, image, allocation) {}

MM::RenderSystem::AllocatedImage::AllocatedImage(
    AllocatedImage&& other) noexcept
    : RenderResourceDataBase(std::move(other)),
      render_engine_(other.render_engine_),
      image_data_info_(std::move(other.image_data_info_)),
      wrapper_(std::move(other.wrapper_)) {
  other.render_engine_ = nullptr;
}

MM::RenderSystem::AllocatedImage& MM::RenderSystem::AllocatedImage::operator=(
    AllocatedImage&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  RenderResourceDataBase::operator=(std::move(other));
  render_engine_ = other.render_engine_;
  image_data_info_ = std::move(other.image_data_info_);
  wrapper_ = std::move(other.wrapper_);

  other.render_engine_ = nullptr;

  return *this;
}

const VkExtent3D& MM::RenderSystem::AllocatedImage::GetImageExtent() const {
  return image_data_info_.image_create_info_.extent_;
}

VkDeviceSize MM::RenderSystem::AllocatedImage::GetImageSize() const {
  return image_data_info_.image_create_info_.image_size_;
}

VkFormat MM::RenderSystem::AllocatedImage::GetImageFormat() const {
  return image_data_info_.image_create_info_.format_;
}

VkImageLayout MM::RenderSystem::AllocatedImage::GetImageLayout() const {
  return image_data_info_.image_create_info_.initial_layout_;
}

std::uint32_t MM::RenderSystem::AllocatedImage::GetMipmapLevels() const {
  return image_data_info_.image_create_info_.miplevels_;
}

std::uint32_t MM::RenderSystem::AllocatedImage::GetArrayLayers() const {
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

const std::vector<MM::RenderSystem::ImageSubResourceAttribute>&
MM::RenderSystem::AllocatedImage::GetQueueIndexes() const {
  return image_data_info_.image_sub_resource_attributes_;
}

void MM::RenderSystem::AllocatedImage::Release() {
  render_engine_ = nullptr;
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

MM::RenderSystem::AllocatedImage::AllocatedImage(
    const std::string& name, MM::RenderSystem::RenderEngine* render_engine,
    MM::AssetSystem::AssetManager::HandlerType image_handler,
    VkImageLayout image_layout, const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info)
    : RenderResourceDataBase(name, RenderResourceDataID{}),
      render_engine_(render_engine),
      image_data_info_(),
      wrapper_() {
#ifdef CHECK_PARAMETERS
  MM_CHECK(
      CheckInitParameters(render_engine, image_handler, image_layout,
                          vk_image_create_info, vma_allocation_create_info),
      render_engine_ = nullptr;
      return;)
#endif

  AssetSystem::AssetType::Image* image =
      static_cast<AssetSystem::AssetType::Image*>(&image_handler.GetAsset());

  const std::uint32_t recommend_mipmap_level =
      static_cast<uint32_t>(std::floor(
          std::log2(std::max(vk_image_create_info->extent.width,
                             vk_image_create_info->extent.height)))) +
      1;

  image_data_info_.SetImageCreateInfo(image->GetImageSize(), image_layout,
                                      *vk_image_create_info);
  if (recommend_mipmap_level < vk_image_create_info->mipLevels) {
    image_data_info_.image_create_info_.miplevels_ = recommend_mipmap_level;
  }
  image_data_info_.SetAllocationCreateInfo(*vma_allocation_create_info);
  image_data_info_.image_sub_resource_attributes_.emplace_back(
      ImageSubresourceRangeInfo{
          0, image_data_info_.image_create_info_.miplevels_, 0,
          image_data_info_.image_create_info_.array_levels_},
      image_data_info_.image_create_info_.queue_family_indices_[0],
      image_data_info_.image_create_info_.image_layout_);

  AllocatedBuffer stage_allocated_buffer;
  MM_CHECK_WITHOUT_LOG(
      LoadImageDataToStageBuffer(image, stage_allocated_buffer),
      render_engine_ = nullptr;
      image_data_info_.Reset(); return;)
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

MM::ExecuteResult MM::RenderSystem::AllocatedImage::CheckInitParameters(
    RenderEngine* render_engine,
    const AssetSystem::AssetManager::HandlerType& image_handler,
    VkImageLayout image_layout, const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  MM_CHECK_WITHOUT_LOG(CheckImageHandler(image_handler),
                       return ExecuteResult ::INITIALIZATION_FAILED;)

  MM_CHECK_WITHOUT_LOG(Utils::CheckVkImageCreateInfo(vk_image_create_info),
                       return ExecuteResult ::INITIALIZATION_FAILED;)

  MM_CHECK_WITHOUT_LOG(
      Utils::CheckVmaAllocationCreateInfo(vma_allocation_create_info),
      return ExecuteResult ::INITIALIZATION_FAILED;)

  if (*vk_image_create_info->pQueueFamilyIndices !=
          render_engine->GetGraphQueueIndex() &&
      *vk_image_create_info->pQueueFamilyIndices !=
          render_engine->GetTransformQueueIndex() &&
      *vk_image_create_info->pQueueFamilyIndices !=
          render_engine->GetPresentQueueIndex() &&
      *vk_image_create_info->pQueueFamilyIndices !=
          render_engine->GetComputeQueueIndex()) {
    LOG_ERROR("The queue family index is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (image_layout == VK_IMAGE_LAYOUT_MAX_ENUM) {
    LOG_ERROR("The image layout is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

const std::vector<MM::RenderSystem::ImageSubResourceAttribute>&
MM::RenderSystem::AllocatedImage::GetImageSubResourceAttributes() const {
  return image_data_info_.image_sub_resource_attributes_;
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::TransformQueueFamily(
    std::uint32_t new_queue_family_index) {
  if (new_queue_family_index == GetImageSubResourceAttributes()) {
    return ExecuteResult ::SUCCESS;
  }

  CommandBufferType command_buffer_type;
  std::uint32_t current_queue_index = GetImageSubResourceAttributes();
  if (current_queue_index == render_engine_->GetGraphQueueIndex()) {
    command_buffer_type = CommandBufferType::GRAPH;
  } else if (current_queue_index == render_engine_->GetTransformQueueIndex()) {
    command_buffer_type = CommandBufferType::TRANSFORM;
  } else if (current_queue_index == render_engine_->GetComputeQueueIndex()) {
    command_buffer_type = CommandBufferType::COMPUTE;
  }

  MM_CHECK(render_engine_->RunSingleCommandAndWait(
               command_buffer_type,
               [this_image = this, new_queue_family_index,
                command_buffer_type](AllocatedCommandBuffer& cmd) {
                 MM_CHECK(Utils::BeginCommandBuffer(cmd),
                          LOG_ERROR("Failed to begin command buffer.");
                          return MM_RESULT_CODE;)

                 VkBufferMemoryBarrier2 buffer_memory_barrier;
                 if (command_buffer_type == CommandBufferType::TRANSFORM) {
                   buffer_memory_barrier = Utils::GetVkImageMemoryBarrier2(
                       VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                       VK_ACCESS_2_TRANSFER_READ_BIT,
                       VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                       VK_ACCESS_2_TRANSFER_READ_BIT,
                       this_image->GetImageSubResourceAttributes(),
                       new_queue_family_index, *this_image, 0,
                       this_image->GetSize());
                 } else if (command_buffer_type == CommandBufferType::GRAPH) {
                   buffer_memory_barrier = Utils::GetVkBufferMemoryBarrier2(
                       VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, 0,
                       VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, 0,
                       this_image->GetImageSubResourceAttributes(),
                       new_queue_family_index, *this_image, 0,
                       this_image->GetSize());
                 } else if (command_buffer_type == CommandBufferType::COMPUTE) {
                   buffer_memory_barrier = Utils::GetVkBufferMemoryBarrier2(
                       VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, 0,
                       VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, 0,
                       this_image->GetImageSubResourceAttributes(),
                       new_queue_family_index, *this_image, 0,
                       this_image->GetSize());
                 } else {
                   return ExecuteResult ::UNDEFINED_ERROR;
                 }

                 VkDependencyInfo dependency_info = Utils::GetVkDependencyInfo(
                     0, nullptr, 0, &buffer_memory_barrier, 0, nullptr, 0);

                 vkCmdPipelineBarrier2(cmd.GetCommandBuffer(),
                                       &dependency_info);

                 MM_CHECK(Utils::EndCommandBuffer(cmd),
                          LOG_ERROR("Failed to end command buffer.");
                          return MM_RESULT_CODE;)

                 return ExecuteResult::SUCCESS;
               }),
           return MM_RESULT_CODE;)

  buffer_data_info_.queue_index_ = new_queue_family_index;

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::LoadImageDataToStageBuffer(
    MM::AssetSystem::AssetType::Image* image_data,
    MM::RenderSystem::AllocatedBuffer& stage_allocated_buffer) {
  std::uint32_t transform_queue_index =
      render_engine_->GetTransformQueueIndex();
  MM_CHECK(render_engine_->CreateBuffer(
               Utils::GetVkBufferCreateInfo(
                   nullptr, 0, image_data_info_.image_create_info_.image_size_,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
                   1, &transform_queue_index),
               VmaAllocationCreateInfo{
                   VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                   VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, 0, 0, nullptr,
                   nullptr, 0},
               nullptr, stage_allocated_buffer),
           LOG_ERROR("Failed to create stage VkBuffer.");
           return MM_RESULT_CODE;)

  void* stage_data;
  VK_CHECK(vmaMapMemory(render_engine_->GetAllocator(),
                        stage_allocated_buffer.GetAllocation(), &stage_data),
           LOG_ERROR("The vmaMapMemory operation failed, unable to complete "
                     "the pointer mapping operation.");
           return ExecuteResult::UNDEFINED_ERROR;)
  memcpy(stage_data, image_data->GetPixelsData(),
         image_data_info_.image_create_info_.image_size_);
  vmaUnmapMemory(render_engine_->GetAllocator(),
                 stage_allocated_buffer.GetAllocation());

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::InitImage(
    const MM::RenderSystem::AllocatedBuffer& stage_allocated_buffer,
    const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  VkImage created_image;
  VmaAllocation created_allocation;
  if (vk_image_create_info->mipLevels ==
      image_data_info_.image_create_info_.miplevels_) {
    VK_CHECK(vmaCreateImage(render_engine_->GetAllocator(),
                            vk_image_create_info, vma_allocation_create_info,
                            &created_image, &created_allocation, nullptr),
             LOG_ERROR("Failed to create VkImage.");
             return MM::Utils::ExecuteResult::CREATE_OBJECT_FAILED;)
  } else {
    VkImageCreateInfo temp_image_create_info =
        image_data_info_.image_create_info_.GetVkImageCreateInfo();
    VK_CHECK(vmaCreateImage(render_engine_->GetAllocator(),
                            &temp_image_create_info, vma_allocation_create_info,
                            &created_image, &created_allocation, nullptr),
             LOG_ERROR("Failed to create VkImage.");
             return MM::Utils::ExecuteResult::CREATE_OBJECT_FAILED;)
  }

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType ::TRANSFORM,
          [render_engine = render_engine_, this_image = this, &created_image,
           &created_allocation,
           &stage_allocated_buffer](AllocatedCommandBuffer cmd) mutable {
            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     LOG_ERROR("Failed to begin VkCommandBuffer.");
                     return MM_RESULT_CODE;)

            ImageCreateInfo& image_create_info =
                this_image->image_data_info_.image_create_info_;

            VkImageAspectFlags aspect_flags =
                Utils::ChooseImageAspectFlags(image_create_info.image_layout_);
            VkImageMemoryBarrier2 image_memory_barrier2 =
                Utils::GetVkImageMemoryBarrier2(
                    VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    VK_ACCESS_2_TRANSFER_READ_BIT,
                    VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    VK_ACCESS_2_TRANSFER_READ_BIT,
                    image_create_info.initial_layout_,
                    this_image->GetImageLayout(),
                    image_create_info.queue_family_indices_[0],
                    render_engine->GetTransformQueueIndex(), created_image,
                    Utils::GetVkImageSubresourceRange(
                        aspect_flags, 0, image_create_info.miplevels_, 0,
                        image_create_info.array_levels_));
            VkDependencyInfo dependency_info = Utils::GetVkDependencyInfo(
                0, nullptr, 0, nullptr, 1, &image_memory_barrier2);
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            VkBufferImageCopy2 buffer_image_copy2 =
                Utils::GetVkBufferImageCopy2(
                    nullptr, 0, 0, 0,
                    Utils::GetVkImageSubResourceLayers(aspect_flags, 0, 0, 1),
                    VkOffset3D{0, 0, 0}, image_create_info.extent_);

            VkCopyBufferToImageInfo2 copy_buffer_to_image_info =
                Utils::GetVkCopyBufferToImageInfo2(
                    nullptr, stage_allocated_buffer, *this_image,
                    this_image->GetImageLayout(), 1, &buffer_image_copy2);

            vkCmdCopyBufferToImage2(cmd.GetCommandBuffer(),
                                    &copy_buffer_to_image_info);

            image_memory_barrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            image_memory_barrier2.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            image_memory_barrier2.oldLayout = this_image->GetImageLayout();
            image_memory_barrier2.srcQueueFamilyIndex =
                render_engine->GetTransformQueueIndex();
            image_memory_barrier2.dstQueueFamilyIndex =
                image_create_info.queue_family_indices_[0];
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     LOG_ERROR("Failed to end VkCommandBuffer.");
                     return MM_RESULT_CODE;)

            return MM::Utils::ExecuteResult ::SUCCESS;
          }),
      LOG_ERROR("Failed to copy stage buffer data to image.");
      return MM_RESULT_CODE;);

  return MM::Utils::ExecuteResult ::SUCCESS;
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
