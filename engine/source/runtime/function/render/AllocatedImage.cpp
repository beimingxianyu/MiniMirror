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
    VmaAllocator allocator, VkImage image, VmaAllocation allocation)
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

VkImageLayout MM::RenderSystem::AllocatedImage::GetImageInitLayout() const {
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
  return GetImageDataInfo().image_create_info_;
}

const MM::RenderSystem::AllocationCreateInfo&
MM::RenderSystem::AllocatedImage::GetAllocationCreateInfo() const {
  return GetImageDataInfo().allocation_create_info_;
}

const MM::RenderSystem::ImageDataInfo&
MM::RenderSystem::AllocatedImage::GetImageDataInfo() const {
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
  RenderResourceDataBase::Release();
  render_engine_ = nullptr;
  image_data_info_.Reset();
  wrapper_.Release();
}

bool MM::RenderSystem::AllocatedImage::IsValid() const {
  return render_engine_ != nullptr && wrapper_.IsValid();
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
    : RenderResourceDataBase(name, RenderResourceDataID()),
      render_engine_(render_engine),
      image_data_info_(),
      wrapper_() {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParametersWhenInitFromAnAsset(
               render_engine, image_handler, image_layout, vk_image_create_info,
               vma_allocation_create_info),
           render_engine_ = nullptr;
           return;)
#endif

  AssetSystem::AssetType::Image* image =
      static_cast<AssetSystem::AssetType::Image*>(&image_handler.GetAsset());

  const std::uint32_t recommend_mipmap_level =
      static_cast<uint32_t>(std::floor(
          std::log2(MM::Utils::Max(vk_image_create_info->extent.width,
                                   vk_image_create_info->extent.height,
                                   vk_image_create_info->extent.depth)))) +
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
  MM_CHECK(LoadImageDataToStageBuffer(image, stage_allocated_buffer),
           RenderResourceDataBase::Release();
           render_engine_ = nullptr; image_data_info_.Reset(); return;)

  RenderResourceDataAttributeID render_resource_data_attribute_ID;
  MM_CHECK(image_data_info_.GetRenderResourceDataAttributeID(
               render_resource_data_attribute_ID),
           MM_LOG_ERROR("Failed to get RenderResourceDataAttributeID.");
           RenderResourceDataBase::Release(); render_engine_ = nullptr;
           image_data_info_.Reset(); return;)
  SetRenderResourceDataID(RenderResourceDataID{
      image->GetAssetID(), render_resource_data_attribute_ID});

  MM_CHECK(InitImage(stage_allocated_buffer, vk_image_create_info,
                     vma_allocation_create_info),
           RenderResourceDataBase::Release();
           render_engine_ = nullptr; image_data_info_.Reset(); return;)

  MarkThisIsManaged();
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::CheckImageHandler(
    const MM::AssetSystem::AssetManager::HandlerType& image_handler) {
  if (!image_handler.IsValid() ||
      image_handler.GetObject()->GetAssetType() !=
          AssetSystem::AssetType::AssetType::IMAGE) {
    MM_LOG_ERROR(
        "The object pointed to by the image handle is not an image asset.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::CheckInitParametersWhenInitFromAnAsset(
    RenderEngine* render_engine,
    const AssetSystem::AssetManager::HandlerType& image_handler,
    VkImageLayout image_layout, const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  MM_CHECK_WITHOUT_LOG(
      CheckInitParameters(render_engine, image_layout, vk_image_create_info,
                          vma_allocation_create_info),
      return MM_RESULT_CODE;)

  MM_CHECK_WITHOUT_LOG(CheckImageHandler(image_handler),
                       MM_LOG_ERROR("Image asset handler is invalid.");
                       return MM_RESULT_CODE;)

  if (!(vk_image_create_info->usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
    MM_LOG_ERROR("This image can not initialization from a image asset.");
    return ExecuteResult::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

const std::vector<MM::RenderSystem::ImageSubResourceAttribute>&
MM::RenderSystem::AllocatedImage::GetSubResourceAttributes() const {
  return image_data_info_.image_sub_resource_attributes_;
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::LoadImageDataToStageBuffer(
    MM::AssetSystem::AssetType::Image* image_data,
    MM::RenderSystem::AllocatedBuffer& stage_allocated_buffer) {
  std::uint32_t graph_queue_index = render_engine_->GetGraphQueueIndex();
  MM_CHECK(
      render_engine_->CreateBuffer(
          Utils::GetVkBufferCreateInfo(
              nullptr, 0, image_data_info_.image_create_info_.image_size_,
              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, 1,
              &graph_queue_index),
          VmaAllocationCreateInfo{
              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
              VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0, nullptr, nullptr, 0},
          nullptr, stage_allocated_buffer),
      MM_LOG_ERROR("Failed to create stage VkBuffer.");
      return MM_RESULT_CODE;)

  void* stage_data;
  MM_VK_CHECK(
      vmaMapMemory(render_engine_->GetAllocator(),
                   stage_allocated_buffer.GetAllocation(), &stage_data),
      MM_LOG_ERROR("The vmaMapMemory operation failed, unable to complete "
                   "the pointer mapping operation.");
      return ExecuteResult::UNDEFINED_ERROR;)
  memcpy(stage_data, image_data->GetPixelsData(),
         image_data_info_.image_create_info_.image_size_);
  vmaUnmapMemory(render_engine_->GetAllocator(),
                 stage_allocated_buffer.GetAllocation());

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::InitImage(
    MM::RenderSystem::AllocatedBuffer& stage_allocated_buffer,
    const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  VkImage created_image;
  VmaAllocation created_allocation;
  if (vk_image_create_info->mipLevels ==
      image_data_info_.image_create_info_.miplevels_) {
    MM_VK_CHECK(vmaCreateImage(render_engine_->GetAllocator(),
                               vk_image_create_info, vma_allocation_create_info,
                               &created_image, &created_allocation, nullptr),
                MM_LOG_ERROR("Failed to create VkImage.");
                return MM::Utils::ExecuteResult::CREATE_OBJECT_FAILED;)
  } else {
    VkImageCreateInfo temp_image_create_info =
        image_data_info_.image_create_info_.GetVkImageCreateInfo();
    MM_VK_CHECK(
        vmaCreateImage(render_engine_->GetAllocator(), &temp_image_create_info,
                       vma_allocation_create_info, &created_image,
                       &created_allocation, nullptr),
        MM_LOG_ERROR("Failed to create VkImage.");
        return MM::Utils::ExecuteResult::CREATE_OBJECT_FAILED;)
  }

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::GRAPH, 1,
          [this_image = this, &created_image,
           &stage_allocated_buffer](AllocatedCommandBuffer& cmd) mutable {
            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to begin VkCommandBuffer.");
                     return MM_RESULT_CODE;)

            ImageCreateInfo& image_create_info =
                this_image->image_data_info_.image_create_info_;

            this_image->AddInitLayoutAndQueueIndexTransformCommands(
                cmd, created_image);

            this_image->AddCopyStageBufferDataToImageCommands(
                cmd, stage_allocated_buffer, created_image);

            if (Utils::ImageUseToSampler(
                    this_image->image_data_info_.image_create_info_.usage_)) {
              this_image
                  ->AddGenerateMipmapsCommandsAndAddQueueIndexAndLayoutTransformCommands(
                      cmd, created_image);
            } else {
              this_image->AddQueueIndexAndLayoutTransformCommands(
                  cmd, created_image);
            }

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to end VkCommandBuffer.");
                     return MM_RESULT_CODE;)

            return MM::Utils::ExecuteResult ::SUCCESS;
          }),
      MM_LOG_ERROR("Failed to copy stage buffer data to image.");
      return MM_RESULT_CODE;)

  wrapper_ = AllocatedImageWrapper{render_engine_->GetAllocator(),
                                   created_image, created_allocation};

  return MM::Utils::ExecuteResult ::SUCCESS;
}

void MM::RenderSystem::AllocatedImage::
    AddInitLayoutAndQueueIndexTransformCommands(
        MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage created_image) {
  VkImageAspectFlags aspect_flags = Utils::ChooseImageAspectFlags(
      image_data_info_.image_create_info_.image_layout_);
  VkImageMemoryBarrier2 image_memory_barrier2 = Utils::GetVkImageMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      VK_ACCESS_2_TRANSFER_WRITE_BIT,
      image_data_info_.image_create_info_.initial_layout_,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      image_data_info_.image_create_info_.queue_family_indices_[0],
      render_engine_->GetGraphQueueIndex(), created_image,
      Utils::GetVkImageSubresourceRange(
          aspect_flags, 0, image_data_info_.image_create_info_.miplevels_, 0,
          image_data_info_.image_create_info_.array_levels_));
  VkDependencyInfo dependency_info = Utils::GetVkDependencyInfo(
      0, nullptr, 0, nullptr, 1, &image_memory_barrier2);
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
}

void MM::RenderSystem::AllocatedImage::AddCopyStageBufferDataToImageCommands(
    MM::RenderSystem::AllocatedCommandBuffer& cmd,
    AllocatedBuffer& stage_allocated_buffer, VkImage created_image) {
  VkImageAspectFlags aspect_flags = Utils::ChooseImageAspectFlags(
      image_data_info_.image_create_info_.image_layout_);

  VkBufferImageCopy2 buffer_image_copy2 = Utils::GetVkBufferImageCopy2(
      nullptr, 0, 0, 0,
      Utils::GetVkImageSubResourceLayers(aspect_flags, 0, 0, 1),
      VkOffset3D{0, 0, 0}, image_data_info_.image_create_info_.extent_);

  VkCopyBufferToImageInfo2 copy_buffer_to_image_info{
      VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
      nullptr,
      stage_allocated_buffer.GetBuffer(),
      created_image,
      GetImageInitLayout(),
      1,
      &buffer_image_copy2};

  vkCmdCopyBufferToImage2(cmd.GetCommandBuffer(), &copy_buffer_to_image_info);
}

MM::ExecuteResult MM::RenderSystem::AllocatedImage::CheckInitParameters(
    MM::RenderSystem::RenderEngine* render_engine, VkImageLayout image_layout,
    const VkImageCreateInfo* vk_image_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

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
    MM_LOG_ERROR("The queue family index is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (image_layout == VK_IMAGE_LAYOUT_MAX_ENUM) {
    MM_LOG_ERROR("The image layout is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

void MM::RenderSystem::AllocatedImage::
    AddGenerateMipmapsCommandsAndAddQueueIndexAndLayoutTransformCommands(
        MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage created_image) {
  VkImageAspectFlags aspect_flags = Utils::ChooseImageAspectFlags(
      image_data_info_.image_create_info_.image_layout_);

  VkImageMemoryBarrier2 barrier = Utils::GetVkImageMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_READ_BIT,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      render_engine_->GetGraphQueueIndex(),
      render_engine_->GetGraphQueueIndex(), created_image,
      Utils::GetVkImageSubresourceRange(
          aspect_flags, 0, 1, 0,
          image_data_info_.image_create_info_.array_levels_));

  VkDependencyInfo dependency_info =
      Utils::GetVkDependencyInfo(0, nullptr, 0, nullptr, 1, &barrier);
  std::int32_t mip_width =
      static_cast<int32_t>(image_data_info_.image_create_info_.extent_.width);
  std::int32_t mip_height =
      static_cast<int32_t>(image_data_info_.image_create_info_.extent_.height);
  std::int32_t mip_depth =
      static_cast<int32_t>(image_data_info_.image_create_info_.extent_.depth);

  for (uint32_t i = 1; i < image_data_info_.image_create_info_.miplevels_;
       ++i) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstStageMask = VK_ACCESS_2_TRANSFER_READ_BIT;
    // barrier.srcQueueFamilyIndex = render_engine_->GetGraphQueueIndex();
    barrier.dstQueueFamilyIndex = render_engine_->GetGraphQueueIndex();

    vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

    VkImageBlit blit;
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mip_width, mip_height, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount =
        image_data_info_.image_create_info_.array_levels_;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1,
                          mip_height > 1 ? mip_height / 2 : 1,
                          mip_depth > 1 ? mip_depth / 2 : 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(cmd.GetCommandBuffer(), created_image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, created_image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                   VK_FILTER_LINEAR);

    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = GetImageInitLayout();
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    // barrier.srcQueueFamilyIndex = render_engine_->GetGraphQueueIndex();
    barrier.dstQueueFamilyIndex =
        image_data_info_.image_create_info_.queue_family_indices_[0];

    vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

    if (mip_width > 1) {
      mip_width >>= 1;
    }
    if (mip_height > 1) {
      mip_height >>= 1;
    }
    if (mip_depth > 1) {
      mip_depth >>= 1;
    }
  }

  barrier.subresourceRange.baseMipLevel =
      image_data_info_.image_create_info_.miplevels_ - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = GetImageInitLayout();
  barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
  barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
  barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
  // barrier.srcQueueFamilyIndex = render_engine_->GetGraphQueueIndex();
  barrier.dstQueueFamilyIndex =
      image_data_info_.image_create_info_.queue_family_indices_[0];

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
}

void MM::RenderSystem::AllocatedImage::AddQueueIndexAndLayoutTransformCommands(
    MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage created_image) {
  VkImageAspectFlags aspect_flags = Utils::ChooseImageAspectFlags(
      image_data_info_.image_create_info_.image_layout_);
  VkImageMemoryBarrier2 image_memory_barrier2 = Utils::GetVkImageMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, GetImageInitLayout(),
      render_engine_->GetGraphQueueIndex(),
      image_data_info_.image_create_info_.queue_family_indices_[0],
      created_image,
      Utils::GetVkImageSubresourceRange(
          aspect_flags, 0, image_data_info_.image_create_info_.miplevels_, 0,
          image_data_info_.image_create_info_.array_levels_));
  VkDependencyInfo dependency_info = Utils::GetVkDependencyInfo(
      0, nullptr, 0, nullptr, 1, &image_memory_barrier2);

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
}

MM::RenderSystem::RenderEngine*
MM::RenderSystem::AllocatedImage::GetRenderEnginePtr() {
  return render_engine_;
}

const MM::RenderSystem::RenderEngine*
MM::RenderSystem::AllocatedImage::GetRenderEnginePtr() const {
  return render_engine_;
}

MM::Utils::ExecuteResult MM::RenderSystem::AllocatedImage::GetImageLayout(
    std::uint32_t mipmap_level, VkImageLayout& output_image_layout) const {
  if (mipmap_level > GetMipmapLevels()) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }
  for (const auto& sub_resource :
       image_data_info_.image_sub_resource_attributes_) {
    if (mipmap_level >=
            sub_resource.GetImageSubresourceRangeInfo().GetBaseMipmapsLevel() &&
        mipmap_level <
            sub_resource.GetImageSubresourceRangeInfo().GetBaseMipmapsLevel() +
                sub_resource.GetImageSubresourceRangeInfo().GetMipmapsCount()) {
      output_image_layout = sub_resource.GetImageLayout();
      return ExecuteResult ::SUCCESS;
    }
  }

  return ExecuteResult ::UNDEFINED_ERROR;
}

MM::Utils::ExecuteResult MM::RenderSystem::AllocatedImage::GetCopy(
    const std::string& new_name, AllocatedImage& new_allocated_image) const {
  if (IsValid()) {
    return MM::Utils::ExecuteResult::OBJECT_IS_INVALID;
  }
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  VkImage new_image{nullptr};
  VmaAllocation new_allocation{nullptr};

  VkImageCreateInfo image_create_info{
      MM::RenderSystem::Utils::GetVkImageCreateInfo(
          GetImageCreateInfo().next_, GetImageCreateInfo().flags_,
          GetImageCreateInfo().image_type_, GetImageCreateInfo().format_,
          GetImageCreateInfo().extent_, GetImageCreateInfo().miplevels_,
          GetImageCreateInfo().array_levels_, GetImageCreateInfo().samples_,
          GetImageCreateInfo().tiling_, GetImageCreateInfo().usage_,
          GetImageCreateInfo().sharing_mode_, 1, &transform_queue_index,
          GetImageCreateInfo().initial_layout_)};
  VmaAllocationCreateInfo allocation_create_info =
      image_data_info_.allocation_create_info_.GetVmaAllocationCreateInfo();
  MM_VK_CHECK(
      vmaCreateImage(wrapper_.GetAllocator(), &image_create_info,
                     &allocation_create_info, &new_image, &new_allocation,
                     nullptr),
      MM_LOG_ERROR("Failed to create VkImage.");
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)

  MM_CHECK(render_engine_->RunSingleCommandAndWait(
               CommandBufferType::TRANSFORM, 1,
               [this_image = this, new_image](AllocatedCommandBuffer& cmd) {
                 if (this_image->GetSubResourceAttributes().size() == 1) {
                   return this_image->AddCopyImageCommandsWhenOneSubResource(
                       cmd, new_image);
                 } else {
                   return this_image->AddCopyImagCommandseWhenMultSubResource(
                       cmd, new_image);
                 }
               },
               std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
           MM_LOG_ERROR("Failed to copy data to new image.");
           return MM_RESULT_CODE;)

  new_allocated_image = AllocatedImage{
      new_name,         GetRenderResourceDataID(), render_engine_,
      image_data_info_, wrapper_.GetAllocator(),   new_image,
      new_allocation};

  return ExecuteResult ::SUCCESS;
}

MM::Utils::ExecuteResult MM::RenderSystem::AllocatedImage::GetCopy(
    const std::vector<std::string>& new_names,
    std::vector<AllocatedImage>& new_allocated_images) const {
  if ((new_names.empty()) ||
      (new_names.size() != new_allocated_images.size())) {
    MM_LOG_ERROR("The size of input parameters are error.");
    return MM::Utils::ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  if (IsValid()) {
    return MM::Utils::ExecuteResult::OBJECT_IS_INVALID;
  }
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();

  std::vector<VkImage> new_images(new_allocated_images.size());
  std::vector<VmaAllocation> new_allocations(new_allocated_images.size());

  VkImageCreateInfo image_create_info{
      MM::RenderSystem::Utils::GetVkImageCreateInfo(
          GetImageCreateInfo().next_, GetImageCreateInfo().flags_,
          GetImageCreateInfo().image_type_, GetImageCreateInfo().format_,
          GetImageCreateInfo().extent_, GetImageCreateInfo().miplevels_,
          GetImageCreateInfo().array_levels_, GetImageCreateInfo().samples_,
          GetImageCreateInfo().tiling_, GetImageCreateInfo().usage_,
          GetImageCreateInfo().sharing_mode_, 1, &transform_queue_index,
          GetImageCreateInfo().initial_layout_)};
  image_data_info_.image_create_info_.GetVkImageCreateInfo();
  VmaAllocationCreateInfo allocation_create_info =
      image_data_info_.allocation_create_info_.GetVmaAllocationCreateInfo();
  for (std::uint64_t i = 0; i != new_allocated_images.size(); ++i) {
    MM_VK_CHECK(
        vmaCreateImage(const_cast<VmaAllocator>(wrapper_.GetAllocator()),
                       &image_create_info, &allocation_create_info,
                       &new_images[i], &new_allocations[i], nullptr),
        for (std::uint64_t j = 0; j != i; ++j) {
          vmaDestroyImage(const_cast<VmaAllocator>(wrapper_.GetAllocator()),
                          new_images[j], new_allocations[j]);
        } MM_LOG_ERROR("Failed to create VkBuffer.");
        return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)
  }

  MM_CHECK(render_engine_->RunSingleCommandAndWait(
               CommandBufferType::TRANSFORM, 1,
               [this_image = this, new_images](AllocatedCommandBuffer& cmd) {
                 if (this_image->GetSubResourceAttributes().size() == 1) {
                   return this_image->AddCopyImageCommandsWhenOneSubResource(
                       cmd, new_images);
                 } else {
                   return this_image->AddCopyImagCommandseWhenMultSubResource(
                       cmd, new_images);
                 }
               },
               std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
           MM_LOG_ERROR("Failed to copy data to new image.");
           return MM_RESULT_CODE;)

  for (std::uint64_t i = 0; i != new_allocated_images.size(); ++i) {
    new_allocated_images[i] = AllocatedImage{
        new_names[i],      GetRenderResourceDataID(), render_engine_,
        image_data_info_,  wrapper_.GetAllocator(),   new_images[i],
        new_allocations[i]};
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::AddCopyImageCommandsWhenOneSubResource(
    MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage new_image) const {
  std::array<VkImageMemoryBarrier2, 2> barriers{};
  const ImageSubResourceAttribute& sub_resource = GetSubResourceAttributes()[0];
  VkImageAspectFlags aspect_flags{
      MM::RenderSystem::Utils::ChooseImageAspectFlags(
          sub_resource.GetImageLayout())};
  VkImageSubresourceRange sub_resource_range{
      MM::RenderSystem::Utils::GetVkImageSubresourceRange(
          aspect_flags, sub_resource.GetBaseMipmapLevel(),
          sub_resource.GetMipmapCount(), sub_resource.GetBaseArrayLevel(),
          sub_resource.GetArrayCount())};

  // old image barrier
  barriers[0] = MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetImageLayout(),
      sub_resource.GetImageLayout(), sub_resource.GetQueueIndex(),
      render_engine_->GetTransformQueueIndex(), GetImage(), sub_resource_range);

  // new image barrier
  barriers[1] = MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      VK_ACCESS_2_TRANSFER_WRITE_BIT,
      image_data_info_.image_create_info_.initial_layout_,
      sub_resource.GetImageLayout(), sub_resource.GetQueueIndex(),
      render_engine_->GetTransformQueueIndex(), new_image, sub_resource_range);

  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, 0, nullptr, barriers.size(), barriers.data(), 0)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begine command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkImageCopy2> image_copy_infos{GetMipmapLevels()};
  image_copy_infos.reserve(GetMipmapLevels());
  for (std::uint32_t i = 0; i != sub_resource.GetMipmapCount(); ++i) {
    VkImageSubresourceLayers image_subresource_layers{
        MM::RenderSystem::Utils::GetVkImageSubResourceLayers(
            aspect_flags, sub_resource.GetBaseMipmapLevel() + i, 0, 1)};
    image_copy_infos.emplace_back(MM::RenderSystem::Utils::GetVkImageCopy2(
        image_subresource_layers, image_subresource_layers, VkOffset3D{0, 0, 0},
        VkOffset3D{0, 0, 0}, GetImageExtent()));
  }

  VkCopyImageInfo2 copy_image_info{MM::RenderSystem::Utils::GetVkCopyImageInfo2(
      GetImage(), new_image, nullptr, sub_resource.GetImageLayout(),
      sub_resource.GetImageLayout(), image_copy_infos.size(),
      image_copy_infos.data())};
  vkCmdCopyImage2(cmd.GetCommandBuffer(), &copy_image_info);

  barriers[0].dstQueueFamilyIndex = barriers[0].srcQueueFamilyIndex;
  barriers[0].srcQueueFamilyIndex = render_engine_->GetTransformQueueIndex();
  barriers[0].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

  barriers[1].dstQueueFamilyIndex = barriers[1].srcQueueFamilyIndex;
  barriers[1].srcQueueFamilyIndex = render_engine_->GetTransformQueueIndex();
  barriers[1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[1].oldLayout = barriers[1].newLayout;

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::AddCopyImagCommandseWhenMultSubResource(
    MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage new_image) const {
  std::vector<VkImageMemoryBarrier2> barriers{};
  barriers.reserve(GetSubResourceAttributes().size() * 2);
  // old image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    VkImageAspectFlags aspect_flags{
        MM::RenderSystem::Utils::ChooseImageAspectFlags(
            sub_resource.GetImageLayout())};
    VkImageSubresourceRange sub_resource_range{
        MM::RenderSystem::Utils::GetVkImageSubresourceRange(
            aspect_flags, sub_resource.GetBaseMipmapLevel(),
            sub_resource.GetMipmapCount(), sub_resource.GetBaseArrayLevel(),
            sub_resource.GetArrayCount())};
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetImageLayout(),
        sub_resource.GetImageLayout(), sub_resource.GetQueueIndex(),
        render_engine_->GetTransformQueueIndex(), GetImage(),
        sub_resource_range));
  }
  // new image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    VkImageAspectFlags aspect_flags{
        MM::RenderSystem::Utils::ChooseImageAspectFlags(
            sub_resource.GetImageLayout())};
    VkImageSubresourceRange sub_resource_range{
        MM::RenderSystem::Utils::GetVkImageSubresourceRange(
            aspect_flags, sub_resource.GetBaseMipmapLevel(),
            sub_resource.GetMipmapCount(), sub_resource.GetBaseArrayLevel(),
            sub_resource.GetArrayCount())};
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, GetImageCreateInfo().initial_layout_,
        sub_resource.GetImageLayout(),
        GetImageCreateInfo().queue_family_indices_[0],
        render_engine_->GetTransformQueueIndex(), new_image,
        sub_resource_range));
  }
  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, 0, nullptr, barriers.size(), barriers.data(), 0)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begine command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkImageCopy2> image_copy_infos{};
  image_copy_infos.reserve(GetMipmapLevels());
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    VkImageAspectFlags aspect_flags =
        MM::RenderSystem::Utils::ChooseImageAspectFlags(
            sub_resource.GetImageLayout());
    for (std::uint32_t i = 0; i != sub_resource.GetMipmapCount(); ++i) {
      VkImageSubresourceLayers image_subresource_layers{
          MM::RenderSystem::Utils::GetVkImageSubResourceLayers(
              aspect_flags, sub_resource.GetBaseMipmapLevel() + i, 0, 1)};
      image_copy_infos.emplace_back(MM::RenderSystem::Utils::GetVkImageCopy2(
          image_subresource_layers, image_subresource_layers,
          VkOffset3D{0, 0, 0}, VkOffset3D{0, 0, 0}, GetImageExtent()));
    }
  }

  auto sub_resource_all = GetSubResourceAttributes();
  std::uint32_t same_layout_count = 0;
  std::uint64_t same_layout_offset = 0;
  VkImageLayout current_layout = sub_resource_all[0].GetImageLayout();
  for (std::uint64_t i = 0; i != sub_resource_all.size(); ++i) {
    same_layout_count += sub_resource_all[i].GetMipmapCount();
    if (i == sub_resource_all.size() - 1 ||
        current_layout != sub_resource_all[i].GetImageLayout()) {
      VkCopyImageInfo2 copy_image_info{
          MM::RenderSystem::Utils::GetVkCopyImageInfo2(
              GetImage(), new_image, nullptr, current_layout, current_layout,
              same_layout_count, image_copy_infos.data() + same_layout_offset)};
      vkCmdCopyImage2(cmd.GetCommandBuffer(), &copy_image_info);

      same_layout_count = 0;
      same_layout_offset = i + 1;
      if (i != sub_resource_all.size() - 1) {
        current_layout = sub_resource_all[i + 1].GetImageLayout();
      }
    }
  }

  std::uint64_t sub_resource_count = GetSubResourceAttributes().size();
  for (std::uint64_t i = 0; i != sub_resource_count; ++i) {
    std::swap(barriers[i].srcQueueFamilyIndex, barriers[i].dstQueueFamilyIndex);
    barriers[i].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[i].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    barriers[i + sub_resource_count].srcQueueFamilyIndex =
        barriers[i].srcQueueFamilyIndex;
    barriers[i + sub_resource_count].dstQueueFamilyIndex =
        barriers[i].dstQueueFamilyIndex;
    barriers[i + sub_resource_count].srcAccessMask =
        VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[i + sub_resource_count].oldLayout =
        barriers[i + sub_resource_count].newLayout;
  }
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::AddCopyImageCommandsWhenOneSubResource(
    MM::RenderSystem::AllocatedCommandBuffer& cmd,
    std::vector<VkImage> new_images) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();

  std::vector<VkImageMemoryBarrier2> barriers{};
  barriers.reserve(1 + new_images.size());
  const ImageSubResourceAttribute& sub_resource = GetSubResourceAttributes()[0];
  VkImageAspectFlags aspect_flags{
      MM::RenderSystem::Utils::ChooseImageAspectFlags(
          sub_resource.GetImageLayout())};
  VkImageSubresourceRange sub_resource_range{
      MM::RenderSystem::Utils::GetVkImageSubresourceRange(
          aspect_flags, sub_resource.GetBaseMipmapLevel(),
          sub_resource.GetMipmapCount(), sub_resource.GetBaseArrayLevel(),
          sub_resource.GetArrayCount())};

  // old image barrier
  barriers.emplace_back(MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetImageLayout(),
      sub_resource.GetImageLayout(), sub_resource.GetQueueIndex(),
      transform_queue_index, GetImage(), sub_resource_range));

  // new image barrier
  for (const auto& new_image : new_images) {
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT,
        image_data_info_.image_create_info_.initial_layout_,
        sub_resource.GetImageLayout(), transform_queue_index,
        transform_queue_index, new_image, sub_resource_range));
  }

  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, 0, nullptr, barriers.size(), barriers.data(), 0)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begine command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkImageCopy2> image_copy_infos{GetMipmapLevels()};
  image_copy_infos.reserve(GetMipmapLevels());
  for (std::uint32_t i = 0; i != sub_resource.GetMipmapCount(); ++i) {
    VkImageSubresourceLayers image_subresource_layers{
        MM::RenderSystem::Utils::GetVkImageSubResourceLayers(
            aspect_flags, sub_resource.GetBaseMipmapLevel() + i, 0, 1)};
    image_copy_infos.emplace_back(MM::RenderSystem::Utils::GetVkImageCopy2(
        image_subresource_layers, image_subresource_layers, VkOffset3D{0, 0, 0},
        VkOffset3D{0, 0, 0}, GetImageExtent()));
  }

  for (const auto& new_image : new_images) {
    VkCopyImageInfo2 copy_image_info{
        MM::RenderSystem::Utils::GetVkCopyImageInfo2(
            GetImage(), new_image, nullptr, sub_resource.GetImageLayout(),
            sub_resource.GetImageLayout(), image_copy_infos.size(),
            image_copy_infos.data())};
    vkCmdCopyImage2(cmd.GetCommandBuffer(), &copy_image_info);
  }

  barriers[0].dstQueueFamilyIndex = barriers[0].srcQueueFamilyIndex;
  barriers[0].srcQueueFamilyIndex = transform_queue_index;
  barriers[0].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  for (std::uint64_t i = 0; i != new_images.size(); ++i) {
    barriers[i + 1].dstQueueFamilyIndex = barriers[0].dstQueueFamilyIndex;
    barriers[i + 1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[i + 1].oldLayout = barriers[i + 1].newLayout;
  }

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::AddCopyImagCommandseWhenMultSubResource(
    MM::RenderSystem::AllocatedCommandBuffer& cmd,
    std::vector<VkImage> new_images) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();

  std::vector<VkImageMemoryBarrier2> barriers{};
  barriers.reserve(GetSubResourceAttributes().size() * (1 + new_images.size()));
  // old image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    VkImageAspectFlags aspect_flags{
        MM::RenderSystem::Utils::ChooseImageAspectFlags(
            sub_resource.GetImageLayout())};
    VkImageSubresourceRange sub_resource_range{
        MM::RenderSystem::Utils::GetVkImageSubresourceRange(
            aspect_flags, sub_resource.GetBaseMipmapLevel(),
            sub_resource.GetMipmapCount(), sub_resource.GetBaseArrayLevel(),
            sub_resource.GetArrayCount())};
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetImageLayout(),
        sub_resource.GetImageLayout(), sub_resource.GetQueueIndex(),
        transform_queue_index, GetImage(), sub_resource_range));
  }
  // new image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    VkImageAspectFlags aspect_flags{
        MM::RenderSystem::Utils::ChooseImageAspectFlags(
            sub_resource.GetImageLayout())};
    VkImageSubresourceRange sub_resource_range{
        MM::RenderSystem::Utils::GetVkImageSubresourceRange(
            aspect_flags, sub_resource.GetBaseMipmapLevel(),
            sub_resource.GetMipmapCount(), sub_resource.GetBaseArrayLevel(),
            sub_resource.GetArrayCount())};
    for (auto& new_image : new_images) {
      barriers.emplace_back(MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, GetImageCreateInfo().initial_layout_,
          sub_resource.GetImageLayout(),
          GetImageCreateInfo().queue_family_indices_[0],
          render_engine_->GetTransformQueueIndex(), new_image,
          sub_resource_range));
    }
  }
  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, 0, nullptr, barriers.size(), barriers.data(), 0)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begine command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkImageCopy2> image_copy_infos{};
  image_copy_infos.reserve(GetMipmapLevels());
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    VkImageAspectFlags aspect_flags =
        MM::RenderSystem::Utils::ChooseImageAspectFlags(
            sub_resource.GetImageLayout());
    for (std::uint32_t i = 0; i != sub_resource.GetMipmapCount(); ++i) {
      VkImageSubresourceLayers image_subresource_layers{
          MM::RenderSystem::Utils::GetVkImageSubResourceLayers(
              aspect_flags, sub_resource.GetBaseMipmapLevel() + i, 0, 1)};
      image_copy_infos.emplace_back(MM::RenderSystem::Utils::GetVkImageCopy2(
          image_subresource_layers, image_subresource_layers,
          VkOffset3D{0, 0, 0}, VkOffset3D{0, 0, 0}, GetImageExtent()));
    }
  }

  auto sub_resource_all = GetSubResourceAttributes();
  std::uint32_t same_layout_count = 0;
  std::uint64_t same_layout_offset = 0;
  VkImageLayout current_layout = sub_resource_all[0].GetImageLayout();
  for (std::uint64_t i = 0; i != sub_resource_all.size(); ++i) {
    same_layout_count += sub_resource_all[i].GetMipmapCount();
    if (i == sub_resource_all.size() - 1 ||
        current_layout != sub_resource_all[i].GetImageLayout()) {
      for (auto& new_image : new_images) {
        VkCopyImageInfo2 copy_image_info{
            MM::RenderSystem::Utils::GetVkCopyImageInfo2(
                GetImage(), new_image, nullptr, current_layout, current_layout,
                same_layout_count,
                image_copy_infos.data() + same_layout_offset)};
        vkCmdCopyImage2(cmd.GetCommandBuffer(), &copy_image_info);
      }

      same_layout_count = 0;
      same_layout_offset = i + 1;
      if (i != sub_resource_all.size() - 1) {
        current_layout = sub_resource_all[i + 1].GetImageLayout();
      }
    }
  }

  std::uint64_t sub_resource_count = GetSubResourceAttributes().size();
  std::uint64_t new_images_count = new_images.size();
  for (std::uint64_t i = 0; i != sub_resource_count; ++i) {
    std::swap(barriers[i].srcQueueFamilyIndex, barriers[i].dstQueueFamilyIndex);
    barriers[i].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[i].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    for (std::uint64_t j = 0; j != new_images_count; ++j) {
      barriers[i + sub_resource_count + j + i * new_images_count]
          .dstQueueFamilyIndex = barriers[i].dstQueueFamilyIndex;
      barriers[i + sub_resource_count + j + i * new_images_count]
          .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
      barriers[i + sub_resource_count + j + i * new_images_count].oldLayout =
          barriers[i + sub_resource_count + j + i * new_images_count].newLayout;
    }
  }
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::TransformSubResourceAttribute(
    const std::vector<ImageSubResourceAttribute>& new_sub_resource_attribute) {
  if (!IsValid()) {
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (new_sub_resource_attribute ==
      image_data_info_.image_sub_resource_attributes_) {
    return ExecuteResult ::SUCCESS;
  }

  MM_CHECK(CheckTransformInputParameter(new_sub_resource_attribute),
           MM_LOG_ERROR("New sub resource attribute is error.");
           return MM_RESULT_CODE;)

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, 1,
          [this_image = this,
           &new_sub_resource_attribute](AllocatedCommandBuffer& cmd) {
            std::vector<VkImageMemoryBarrier2> current_to_transform_barriers{},
                transform_to_new_barriers{};

            std::uint64_t new_sub_resource_index = 0;
            for (const auto& old_sub_resource :
                 this_image->GetSubResourceAttributes()) {
              std::uint32_t
                  old_sub_resource_mipmap_level_end =
                      old_sub_resource.GetBaseMipmapLevel() +
                      old_sub_resource.GetMipmapCount(),
                  new_sub_resource_mipmap_level_end =
                      new_sub_resource_attribute[new_sub_resource_index]
                          .GetBaseMipmapLevel() +
                      new_sub_resource_attribute[new_sub_resource_index]
                          .GetMipmapCount();
              if (new_sub_resource_attribute[new_sub_resource_index]
                          .GetQueueIndex() !=
                      old_sub_resource.GetQueueIndex() ||
                  new_sub_resource_attribute[new_sub_resource_index]
                          .GetImageLayout() !=
                      old_sub_resource.GetImageLayout()) {
                std::uint32_t
                    transform_mipmap_level_offset =
                        old_sub_resource.GetBaseMipmapLevel() >
                                new_sub_resource_attribute
                                    [new_sub_resource_index]
                                        .GetBaseMipmapLevel()
                            ? old_sub_resource.GetBaseMipmapLevel()
                            : new_sub_resource_attribute[new_sub_resource_index]
                                  .GetBaseMipmapLevel(),
                    transform_mipmap_level_count =
                        old_sub_resource_mipmap_level_end <
                                new_sub_resource_mipmap_level_end
                            ? old_sub_resource_mipmap_level_end -
                                  transform_mipmap_level_offset
                            : new_sub_resource_mipmap_level_end -
                                  transform_mipmap_level_offset;
                VkImageAspectFlags aspect_flag =
                    MM::RenderSystem::Utils::ChooseImageAspectFlags(
                        new_sub_resource_attribute[new_sub_resource_index]
                            .GetImageLayout());
                current_to_transform_barriers.emplace_back(
                    MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_READ_BIT,
                        old_sub_resource.GetImageLayout(),
                        new_sub_resource_attribute[new_sub_resource_index]
                            .GetImageLayout(),
                        old_sub_resource.GetQueueIndex(),
                        this_image->render_engine_->GetTransformQueueIndex(),
                        this_image->GetImage(),
                        MM::RenderSystem::Utils::GetVkImageSubresourceRange(
                            aspect_flag, transform_mipmap_level_offset,
                            transform_mipmap_level_count, 0, 1)));
                transform_to_new_barriers.emplace_back(
                    MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_READ_BIT,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_WRITE_BIT,
                        old_sub_resource.GetImageLayout(),
                        new_sub_resource_attribute[new_sub_resource_index]
                            .GetImageLayout(),
                        this_image->render_engine_->GetTransformQueueIndex(),
                        new_sub_resource_attribute[new_sub_resource_index]
                            .GetQueueIndex(),
                        this_image->GetImage(),
                        MM::RenderSystem::Utils::GetVkImageSubresourceRange(
                            aspect_flag, transform_mipmap_level_offset,
                            transform_mipmap_level_count, 0, 1)));
              }

              if (new_sub_resource_mipmap_level_end ==
                  old_sub_resource_mipmap_level_end) {
                ++new_sub_resource_index;
                continue;
              }

              while (new_sub_resource_mipmap_level_end <
                     old_sub_resource_mipmap_level_end) {
                ++new_sub_resource_index;
                new_sub_resource_mipmap_level_end =
                    new_sub_resource_attribute[new_sub_resource_index]
                        .GetBaseMipmapLevel() +
                    new_sub_resource_attribute[new_sub_resource_index]
                        .GetMipmapCount();
                if (new_sub_resource_attribute[new_sub_resource_index]
                            .GetQueueIndex() !=
                        old_sub_resource.GetQueueIndex() ||
                    new_sub_resource_attribute[new_sub_resource_index]
                            .GetImageLayout() !=
                        old_sub_resource.GetImageLayout()) {
                  std::uint32_t transform_offset =
                                    old_sub_resource.GetBaseMipmapLevel() >
                                            new_sub_resource_attribute
                                                [new_sub_resource_index]
                                                    .GetBaseMipmapLevel()
                                        ? old_sub_resource.GetBaseMipmapLevel()
                                        : new_sub_resource_attribute
                                              [new_sub_resource_index]
                                                  .GetBaseMipmapLevel(),
                                transform_size =
                                    old_sub_resource_mipmap_level_end <
                                            new_sub_resource_mipmap_level_end
                                        ? old_sub_resource_mipmap_level_end -
                                              transform_offset
                                        : new_sub_resource_mipmap_level_end -
                                              transform_offset;
                  VkImageAspectFlags aspect_flag =
                      MM::RenderSystem::Utils::ChooseImageAspectFlags(
                          new_sub_resource_attribute[new_sub_resource_index]
                              .GetImageLayout());
                  current_to_transform_barriers.emplace_back(
                      MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_READ_BIT,
                          old_sub_resource.GetImageLayout(),
                          new_sub_resource_attribute[new_sub_resource_index]
                              .GetImageLayout(),
                          old_sub_resource.GetQueueIndex(),
                          this_image->render_engine_->GetTransformQueueIndex(),
                          this_image->GetImage(),
                          MM::RenderSystem::Utils::GetVkImageSubresourceRange(
                              aspect_flag, transform_offset, transform_size, 0,
                              1)));
                  transform_to_new_barriers.emplace_back(
                      MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_READ_BIT,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_WRITE_BIT,
                          old_sub_resource.GetImageLayout(),
                          new_sub_resource_attribute[new_sub_resource_index]
                              .GetImageLayout(),
                          this_image->render_engine_->GetTransformQueueIndex(),
                          new_sub_resource_attribute[new_sub_resource_index]
                              .GetQueueIndex(),
                          this_image->GetImage(),
                          MM::RenderSystem::Utils::GetVkImageSubresourceRange(
                              aspect_flag, transform_offset, transform_size, 0,
                              1)));
                }

                if (new_sub_resource_mipmap_level_end ==
                    old_sub_resource_mipmap_level_end) {
                  ++new_sub_resource_index;
                  break;
                }
              }
            }

            VkDependencyInfo dependency_info1{
                MM::RenderSystem::Utils::GetVkDependencyInfo(
                    0, nullptr, 0, nullptr,
                    current_to_transform_barriers.size(),
                    current_to_transform_barriers.data(), 0)},
                dependency_info2{MM::RenderSystem::Utils::GetVkDependencyInfo(
                    0, nullptr, 0, nullptr, transform_to_new_barriers.size(),
                    transform_to_new_barriers.data(), 0)};

            MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to begin command buffer.");
                     return MM_RESULT_CODE;)

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info1);
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info2);

            MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to end command buffer.");
                     return MM_RESULT_CODE;)

            this_image->MarkThisUseForWrite();
            this_image->image_data_info_.image_sub_resource_attributes_ =
                std::move(new_sub_resource_attribute);

            return ExecuteResult ::SUCCESS;
          },
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
      MM_LOG_ERROR("Failed to transform sub resource attribute.");
      return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedImage::CheckTransformInputParameter(
    const std::vector<ImageSubResourceAttribute>& new_sub_resource_attribute)
    const {
  QueueIndex graph_queue_index = render_engine_->GetGraphQueueIndex(),
             compute_queue_index = render_engine_->GetComputeQueueIndex(),
             transform_queue_index = render_engine_->GetTransformQueueIndex(),
             present_queue_index = render_engine_->GetPresentQueueIndex();

  // [start, end)
  std::uint32_t start = UINT32_MAX, count = 0;
  for (const auto& sub_resource : new_sub_resource_attribute) {
    // Check queue index is valid
    if (sub_resource.GetQueueIndex() != graph_queue_index &&
        sub_resource.GetQueueIndex() != compute_queue_index &&
        sub_resource.GetQueueIndex() != transform_queue_index &&
        sub_resource.GetQueueIndex() != present_queue_index) {
      return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    // Check image layout
    if (sub_resource.GetImageLayout() == VK_IMAGE_LAYOUT_UNDEFINED ||
        sub_resource.GetImageLayout() == VK_IMAGE_LAYOUT_PREINITIALIZED) {
      return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    // Check array layer
    if (sub_resource.GetBaseArrayLevel() != 0 ||
        sub_resource.GetArrayCount() != 1) {
      return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    // Check overlap
    if ((sub_resource.GetBaseMipmapLevel() < start &&
         sub_resource.GetBaseMipmapLevel() + sub_resource.GetMipmapCount() >
             start) ||
        (sub_resource.GetBaseMipmapLevel() >= start &&
         sub_resource.GetBaseMipmapLevel() < start + count)) {
      return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }
    if (sub_resource.GetBaseMipmapLevel() < start) {
      start = sub_resource.GetBaseMipmapLevel();
    }
    if (sub_resource.GetBaseMipmapLevel() + sub_resource.GetMipmapCount() >
        start + count) {
      count = sub_resource.GetBaseMipmapLevel() +
              sub_resource.GetMipmapCount() - start;
    }
  }

  // Check that all ranges are covered.
  if (start != 0 || count != GetMipmapLevels()) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  return ExecuteResult ::SUCCESS;
}

// MM::ExecuteResult
// MM::RenderSystem::AllocatedImage::TransformSubResourceAttribute(
//     const std::vector<ImageSubResourceAttribute>&
//         new_image_sub_resource_attribute) {
//   if (new_image_sub_resource_attribute.empty()) {
//     LOG_WARN(
//         "The new_image_sub_resource_attribute is empty, invoke this function
//         " "is meaningless.");
//     return ExecuteResult::SUCCESS;
//   }
//
// #ifdef CHECK_PARAMETERS
//   std::uint32_t check_queue_index =
//       new_image_sub_resource_attribute[0].GetQueueIndex();
//   for (std::uint64_t i = 1; i != new_image_sub_resource_attribute.size();
//   ++i) {
//     if (new_image_sub_resource_attribute[i].GetQueueIndex() !=
//         check_queue_index) {
//       return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
//     }
//   }
//
//   // TODO check layer count is 1 and is there any overlap
//
// #endif
//
//   CommandBufferType command_buffer_type;
//   if (new_image_sub_resource_attribute[0].GetQueueIndex() ==
//       render_engine_->GetGraphQueueIndex()) {
//     command_buffer_type = CommandBufferType::GRAPH;
//   } else if (new_image_sub_resource_attribute[0].GetQueueIndex() ==
//              render_engine_->GetTransformQueueIndex()) {
//     command_buffer_type = CommandBufferType::TRANSFORM;
//   } else if (new_image_sub_resource_attribute[0].GetQueueIndex() ==
//              render_engine_->GetComputeQueueIndex()) {
//     command_buffer_type = CommandBufferType::COMPUTE;
//   } else {
//     return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
//   }
//
//   std::vector<ImageSubResourceAttribute> new_image_sub_resource_Attribute;
//   MM_CHECK(
//       render_engine_->RunSingleCommandAndWait(
//           command_buffer_type,
//           [](AllocatedCommandBuffer& cmd) { return ExecuteResult::SUCCESS; },
//           std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
//       return MM_RESULT_CODE;)
//
//   image_data_info_.image_sub_resource_attributes_ =
//       std::move(new_image_sub_resource_attribute);
//
//   return MM::ExecuteResult::SUCCESS;
// }

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