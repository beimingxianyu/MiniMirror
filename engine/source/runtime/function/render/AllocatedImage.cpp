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
  return render_engine_ != nullptr && wrapper_.IsValid() &&
         image_data_info_.IsValid() && RenderResourceDataBase::IsValid();
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
           LOG_ERROR("Failed to get RenderResourceDataAttributeID.");
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
    LOG_ERROR(
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
                       LOG_ERROR("Image asset handler is invalid.");
                       return MM_RESULT_CODE;)

  if (!(vk_image_create_info->usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
    LOG_ERROR("This image can not initialization from a image asset.");
    return ExecuteResult::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

const std::vector<MM::RenderSystem::ImageSubResourceAttribute>&
MM::RenderSystem::AllocatedImage::GetImageSubResourceAttributes() const {
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
    MM::RenderSystem::AllocatedBuffer& stage_allocated_buffer,
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
          CommandBufferType::GRAPH, 1,
          [this_image = this, &created_image,
           &stage_allocated_buffer](AllocatedCommandBuffer& cmd) mutable {
            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     LOG_ERROR("Failed to begin VkCommandBuffer.");
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
                     LOG_ERROR("Failed to end VkCommandBuffer.");
                     return MM_RESULT_CODE;)

            return MM::Utils::ExecuteResult ::SUCCESS;
          }),
      LOG_ERROR("Failed to copy stage buffer data to image.");
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
    LOG_ERROR("The queue family index is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (image_layout == VK_IMAGE_LAYOUT_MAX_ENUM) {
    LOG_ERROR("The image layout is error.");
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