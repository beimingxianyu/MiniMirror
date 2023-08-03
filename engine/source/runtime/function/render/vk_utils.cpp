#include "runtime/function/render/vk_utils.h"

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>

#include "runtime/function/render/AllocatedImage.h"
#include "runtime/function/render/vk_engine.h"

MM::ExecuteResult MM::RenderSystem::Utils::VkResultToMMResult(
    VkResult vk_result) {
  ExecuteResult MM_result;
  switch (vk_result) {
    case VK_SUCCESS:
      MM_result = ExecuteResult::SUCCESS;
      break;
    case VK_TIMEOUT:
      MM_result = ExecuteResult::TIMEOUT;
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      MM_result = ExecuteResult::OUT_OF_HOST_MEMORY;
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      MM_result = ExecuteResult::OUT_OF_DEVICE_MEMORY;
      break;
    case VK_ERROR_INITIALIZATION_FAILED:
      MM_result = ExecuteResult::INITIALIZATION_FAILED;
      break;
    default:
      MM_result = ExecuteResult::UNDEFINED_ERROR;
  }

  return MM_result;
}

VkCommandPoolCreateInfo MM::RenderSystem::Utils::GetCommandPoolCreateInfo(
    const uint32_t& queue_family_index, const VkCommandPoolCreateFlags& flags) {
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;

  info.queueFamilyIndex = queue_family_index;
  info.flags = flags;
  return info;
}

VkCommandBufferAllocateInfo
MM::RenderSystem::Utils::GetCommandBufferAllocateInfo(
    const VkCommandPool& command_pool, const uint32_t& count,
    const VkCommandBufferLevel& level) {
  VkCommandBufferAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;

  info.commandPool = command_pool;
  info.commandBufferCount = count;
  info.level = level;
  return info;
}

VmaAllocationCreateInfo MM::RenderSystem::Utils::GetVmaAllocationCreateInfo(
    const VmaMemoryUsage& usage, const VmaAllocationCreateFlags& flags) {
  VmaAllocationCreateInfo allocation_create_info{};

  allocation_create_info.flags = flags;
  allocation_create_info.usage = usage;

  // Use default values for other values.

  return allocation_create_info;
}

VkCommandBufferBeginInfo MM::RenderSystem::Utils::GetCommandBufferBeginInfo(
    const VkCommandBufferUsageFlags& flags) {
  VkCommandBufferBeginInfo command_buffer_begin_info{};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.pNext = nullptr;
  command_buffer_begin_info.flags = flags;
  command_buffer_begin_info.pInheritanceInfo = nullptr;

  return command_buffer_begin_info;
}

VkFenceCreateInfo MM::RenderSystem::Utils::GetFenceCreateInfo(
    const VkFenceCreateFlags& flags) {
  VkFenceCreateInfo fence_create_info{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.pNext = nullptr;
  fence_create_info.flags = flags;

  return fence_create_info;
}

VkFence MM::RenderSystem::Utils::GetVkFence(const VkDevice& device,
                                            const bool& is_signaled) {
  VkFenceCreateInfo fence_create_info{};
  if (is_signaled) {
    fence_create_info = GetFenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
  } else {
    fence_create_info = GetFenceCreateInfo();
  }
  VkFence fence{nullptr};
  MM_VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &fence),
              fence = nullptr;
              MM_LOG_ERROR("Failed to create VkFence!"))
  return fence;
}

VkSubmitInfo MM::RenderSystem::Utils::GetCommandSubmitInfo(
    const VkCommandBuffer& command_buffer, const uint32_t& command_count) {
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pNext = nullptr;

  submit_info.waitSemaphoreCount = 0;
  submit_info.pWaitSemaphores = nullptr;
  submit_info.pWaitDstStageMask = nullptr;
  submit_info.commandBufferCount = command_count;
  submit_info.pCommandBuffers = &command_buffer;
  submit_info.signalSemaphoreCount = 0;
  submit_info.pSignalSemaphores = nullptr;

  return submit_info;
}

VkSubmitInfo MM::RenderSystem::Utils::GetCommandSubmitInfo(
    const std::vector<VkCommandBuffer>& command_buffers) {
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pNext = nullptr;

  submit_info.waitSemaphoreCount = 0;
  submit_info.pWaitSemaphores = nullptr;
  submit_info.pWaitDstStageMask = nullptr;
  submit_info.commandBufferCount = command_buffers.size();
  submit_info.pCommandBuffers = command_buffers.data();
  submit_info.signalSemaphoreCount = 0;
  submit_info.pSignalSemaphores = nullptr;

  return submit_info;
}

VkSubmitInfo MM::RenderSystem::Utils::GetCommandSubmitInfo(
    const std::vector<AllocatedCommandBuffer>& command_buffers) {
  std::vector<VkCommandBuffer> vk_command_buffers;
  vk_command_buffers.reserve(command_buffers.size());
  for (const auto& command_buffer : command_buffers) {
    vk_command_buffers.emplace_back(
        const_cast<VkCommandBuffer>(command_buffer.GetCommandBuffer()));
  }

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pNext = nullptr;

  submit_info.waitSemaphoreCount = 0;
  submit_info.pWaitSemaphores = nullptr;
  submit_info.pWaitDstStageMask = nullptr;
  submit_info.commandBufferCount = command_buffers.size();
  submit_info.pCommandBuffers = vk_command_buffers.data();
  submit_info.signalSemaphoreCount = 0;
  submit_info.pSignalSemaphores = nullptr;

  return submit_info;
}

MM::ExecuteResult MM::RenderSystem::Utils::SubmitCommandBuffers(
    VkQueue queue, const std::vector<VkSubmitInfo>& submit_infos,
    VkFence fence) {
  return VkResultToMMResult(
      vkQueueSubmit(queue, submit_infos.size(), submit_infos.data(), fence));
}

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
    MM::RenderSystem::AllocatedImage& image,
    const ImageTransferMode& transfer_mode) {
  VkImageMemoryBarrier2 image_barrier{};
  image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  image_barrier.pNext = nullptr;

  switch (transfer_mode) {
    case ImageTransferMode::INIT_TO_ATTACHMENT:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_NONE;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;

      break;
    case ImageTransferMode::ATTACHMENT_TO_TRANSFER_SOURCE:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR;

      break;
    case ImageTransferMode::INIT_TO_TRANSFER_DESTINATION:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_NONE;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;

      break;
    case ImageTransferMode::TRANSFER_DESTINATION_TO_SHARED_READABLE:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_NONE;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;

      break;
    case ImageTransferMode::ATTACHMENT_TO_PRESENT:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR;

      break;
    case ImageTransferMode::TRANSFER_DESTINATION_TO_SHARED_PRESENT:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR;

      break;

    case ImageTransferMode::INIT_TO_DEPTH_TEST:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR;

      break;

    case ImageTransferMode::TRANSFER_DESTINATION_TO_TRANSFER_SOURCE:
      image_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      image_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

      image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
      image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR;
  }

  VkImageSubresourceRange sub_image{};
  sub_image.aspectMask =
      (transfer_mode == ImageTransferMode::INIT_TO_DEPTH_TEST)
          ? VK_IMAGE_ASPECT_DEPTH_BIT
          : VK_IMAGE_ASPECT_COLOR_BIT;
  sub_image.baseMipLevel = 0;
  sub_image.levelCount = 1;
  sub_image.baseArrayLayer = 0;
  sub_image.layerCount = 1;

  image_barrier.subresourceRange = sub_image;
  image_barrier.image = image.GetImage();

  return image_barrier;
}

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
    AllocatedImage& image, const VkImageLayout& old_layout,
    const VkImageLayout& new_layout) {
  VkImageMemoryBarrier2 image_barrier{};
  image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  image_barrier.pNext = nullptr;

  image_barrier.oldLayout = old_layout;
  image_barrier.newLayout = new_layout;

  image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
  image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR;

  VkImageSubresourceRange sub_image{};
  sub_image.aspectMask =
      (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
          ? VK_IMAGE_ASPECT_DEPTH_BIT
          : VK_IMAGE_ASPECT_COLOR_BIT;
  sub_image.baseMipLevel = 0;
  sub_image.levelCount = 1;
  sub_image.baseArrayLayer = 0;
  sub_image.layerCount = 1;

  image_barrier.subresourceRange = sub_image;
  image_barrier.image = image.GetImage();

  return image_barrier;
}

VkDependencyInfo MM::RenderSystem::Utils::GetVkImageDependencyInfo(
    VkImageMemoryBarrier2& image_barrier, const VkDependencyFlags& flags) {
  VkDependencyInfo dep_info{};
  dep_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dep_info.pNext = nullptr;

  dep_info.imageMemoryBarrierCount = 1;
  dep_info.pImageMemoryBarriers = &image_barrier;

  dep_info.dependencyFlags = flags;

  return dep_info;
}

VkMemoryBarrier2 MM::RenderSystem::Utils::GetVkMemoryBarrier2(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access) {
  return VkMemoryBarrier2{VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                          nullptr,
                          src_stage,
                          src_access,
                          dest_stage,
                          dest_access};
}

VkBufferMemoryBarrier2 MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    const AllocatedBuffer& buffer, VkDeviceSize offset, VkDeviceSize size) {
  return VkBufferMemoryBarrier2{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                nullptr,
                                src_stage,
                                src_access,
                                dest_stage,
                                dest_access,
                                src_queue_family_index,
                                dest_queue_family_index,
                                const_cast<VkBuffer>(buffer.GetBuffer()),
                                offset,
                                size};
}

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    VkImageLayout old_layout, VkImageLayout new_layout,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    const AllocatedImage& image,
    const VkImageSubresourceRange& sub_resource_range) {
  return VkImageMemoryBarrier2{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                               nullptr,
                               src_stage,
                               src_access,
                               dest_stage,
                               dest_access,
                               old_layout,
                               new_layout,
                               src_queue_family_index,
                               dest_queue_family_index,
                               const_cast<VkImage>(image.GetImage()),
                               sub_resource_range};
}

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetImageMemoryBarrier(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    VkImageLayout old_layout, VkImageLayout new_layout,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    const AllocatedImage& image, VkImageAspectFlags aspect_mask,
    std::uint32_t base_mipmap_level, std::uint32_t level_count,
    std::uint32_t base_array_layer, std::uint32_t layer_count) {
  return VkImageMemoryBarrier2{
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      nullptr,
      src_stage,
      src_access,
      dest_stage,
      dest_access,
      old_layout,
      new_layout,
      src_queue_family_index,
      dest_queue_family_index,
      const_cast<VkImage>(image.GetImage()),
      VkImageSubresourceRange{aspect_mask, base_mipmap_level, level_count,
                              base_array_layer, layer_count}};
}

VkDependencyInfo MM::RenderSystem::Utils::GetMemoryDependencyInfo(
    const std::vector<VkMemoryBarrier2>& memory_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          static_cast<uint32_t>(memory_barriers.size()),
                          memory_barriers.data(),
                          0,
                          nullptr,
                          0,
                          nullptr};
}

VkDependencyInfo MM::RenderSystem::Utils::GetMemoryDependencyInfo(
    const VkMemoryBarrier2& memory_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          1,
                          &memory_barriers,
                          0,
                          nullptr,
                          0,
                          nullptr};
}

VkDependencyInfo MM::RenderSystem::Utils::GetBufferMemoryDependencyInfo(
    const std::vector<VkBufferMemoryBarrier2>& buffer_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          0,
                          nullptr,
                          static_cast<uint32_t>(buffer_barriers.size()),
                          buffer_barriers.data(),
                          0,
                          nullptr};
}

VkDependencyInfo MM::RenderSystem::Utils::GetBufferMemoryDependencyInfo(
    const VkBufferMemoryBarrier2& buffer_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          0,
                          nullptr,
                          1,
                          &buffer_barriers,
                          0,
                          nullptr};
}

VkDependencyInfo MM::RenderSystem::Utils::GetImageMemoryDependencyInfo(
    const std::vector<VkImageMemoryBarrier2>& image_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          0,
                          nullptr,
                          0,
                          nullptr,
                          static_cast<uint32_t>(image_barriers.size()),
                          image_barriers.data()};
}

VkDependencyInfo MM::RenderSystem::Utils::GetImageMemoryDependencyInfo(
    const VkImageMemoryBarrier2& image_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          0,
                          nullptr,
                          0,
                          nullptr,
                          1,
                          &image_barriers};
}

VkDependencyInfo MM::RenderSystem::Utils::GetVkDependencyInfo(
    const std::vector<VkMemoryBarrier2>* memory_barriers,
    const std::vector<VkBufferMemoryBarrier2>* buffer_barriers,
    const std::vector<VkImageMemoryBarrier2>* image_barriers,
    VkDependencyFlags dependency_flags) {
  VkDependencyInfo dependency_info{VK_STRUCTURE_TYPE_DEPENDENCY_INFO, nullptr};
  if (memory_barriers && !memory_barriers->empty()) {
    dependency_info.memoryBarrierCount = memory_barriers->size();
    dependency_info.pMemoryBarriers = memory_barriers->data();
  }
  if (buffer_barriers && !memory_barriers->empty()) {
    dependency_info.bufferMemoryBarrierCount = buffer_barriers->size();
    dependency_info.pBufferMemoryBarriers = buffer_barriers->data();
  }
  if (image_barriers && !image_barriers->empty()) {
    dependency_info.imageMemoryBarrierCount = image_barriers->size();
    dependency_info.pImageMemoryBarriers = image_barriers->data();
  }
  dependency_info.dependencyFlags = dependency_flags;
  return dependency_info;
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    VkCommandBuffer& command_buffer, AllocatedImage& image,
    const ImageTransferMode& transfer_mode, const VkDependencyFlags& flags) {
  auto image_barrier = Utils::GetVkImageMemoryBarrier2(image, transfer_mode);
  const auto image_transfer_dependency =
      Utils::GetVkImageDependencyInfo(image_barrier, flags);

  vkCmdPipelineBarrier2(command_buffer, &image_transfer_dependency);
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    VkCommandBuffer& command_buffer, AllocatedImage& image,
    const VkImageLayout& old_layout, const VkImageLayout& new_layout,
    const VkDependencyFlags& flags) {
  auto image_barrier =
      Utils::GetVkImageMemoryBarrier2(image, old_layout, new_layout);
  auto image_transfer_dependency =
      Utils::GetVkImageDependencyInfo(image_barrier, flags);

  vkCmdPipelineBarrier2(command_buffer, &image_transfer_dependency);
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    AllocatedCommandBuffer& command_buffer, AllocatedImage& image,
    const ImageTransferMode& transfer_mode, const VkDependencyFlags& flags) {
  auto image_barrier = Utils::GetVkImageMemoryBarrier2(image, transfer_mode);
  const auto image_transfer_dependency =
      Utils::GetVkImageDependencyInfo(image_barrier, flags);

  vkCmdPipelineBarrier2(command_buffer.GetCommandBuffer(),
                        &image_transfer_dependency);
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    AllocatedCommandBuffer& command_buffer, AllocatedImage& image,
    const VkImageLayout& old_layout, const VkImageLayout& new_layout,
    const VkDependencyFlags& flags) {
  auto image_barrier =
      Utils::GetVkImageMemoryBarrier2(image, old_layout, new_layout);
  auto image_transfer_dependency =
      Utils::GetVkImageDependencyInfo(image_barrier, flags);

  vkCmdPipelineBarrier2(command_buffer.GetCommandBuffer(),
                        &image_transfer_dependency);
}

VkBufferImageCopy MM::RenderSystem::Utils::GetBufferToImageCopyRegion(
    const VkImageAspectFlagBits& aspect, const VkExtent3D& image_extent,
    const VkDeviceSize& buffer_offset, const VkOffset3D& image_offset) {
  VkBufferImageCopy copy_region = {};
  copy_region.bufferOffset = buffer_offset;
  copy_region.bufferRowLength = 0;
  copy_region.bufferImageHeight = 0;

  copy_region.imageSubresource.aspectMask = aspect;
  copy_region.imageSubresource.mipLevel = 0;
  copy_region.imageSubresource.baseArrayLayer = 0;
  copy_region.imageSubresource.layerCount = 1;
  copy_region.imageExtent = image_extent;
  copy_region.imageOffset = image_offset;

  return copy_region;
}

bool MM::RenderSystem::Utils::DescriptorTypeIsImage(
    const VkDescriptorType& descriptor_type) {
  return descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLER ||
         descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
         descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
         descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
         descriptor_type == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
}

bool MM::RenderSystem::Utils::DescriptorTypeIsImageSampler(
    const VkDescriptorType& descriptor_type) {
  return descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLER ||
         descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
         descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
}

VkImageViewCreateInfo MM::RenderSystem::Utils::GetImageViewCreateInfo(
    const AllocatedImage& image, const VkFormat& format,
    const VkImageViewType& view_type, const VkImageAspectFlags& aspect_flags,
    const VkImageViewCreateFlags& flags) {
  VkImageViewCreateInfo image_view_create_info{};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.pNext = nullptr;

  image_view_create_info.image = const_cast<VkImage>(image.GetImage());
  image_view_create_info.format = format;
  image_view_create_info.viewType = view_type;
  image_view_create_info.flags = flags;
  // image_view_create_info.components
  image_view_create_info.subresourceRange.aspectMask = aspect_flags;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;

  return image_view_create_info;
}

VkSamplerCreateInfo MM::RenderSystem::Utils::GetSamplerCreateInfo(
    const VkPhysicalDevice& physical_device) {
  VkSamplerCreateInfo sampler_create_info{};
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.pNext = nullptr;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(physical_device, &properties);

  sampler_create_info.magFilter = VK_FILTER_LINEAR;
  sampler_create_info.minFilter = VK_FILTER_LINEAR;
  sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
  sampler_create_info.unnormalizedCoordinates = VK_FALSE;
  sampler_create_info.compareEnable = VK_FALSE;
  sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_create_info.mipLodBias = 0.0f;
  sampler_create_info.minLod = 0.0f;
  sampler_create_info.maxLod = 0.0f;

  return sampler_create_info;
}

VkSemaphoreCreateInfo MM::RenderSystem::Utils::GetSemaphoreCreateInfo(
    const VkSemaphoreCreateFlags& flags) {
  VkSemaphoreCreateInfo semaphore_create_info{};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.pNext = nullptr;
  semaphore_create_info.flags = flags;

  return semaphore_create_info;
}

bool MM::RenderSystem::Utils::DescriptorTypeIsDynamicBuffer(
    const VkDescriptorType& descriptor_type) {
  return descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
         descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
}

bool MM::RenderSystem::Utils::DescriptorTypeIsBuffer(
    const VkDescriptorType& descriptor_type) {
  switch (descriptor_type) {
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      return true;
    default:
      return false;
  }
}

bool MM::RenderSystem::Utils::CanBeMapped(
    const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags) {
  switch (memory_usage) {
    case VMA_MEMORY_USAGE_CPU_ONLY:
    case VMA_MEMORY_USAGE_CPU_TO_GPU:
    case VMA_MEMORY_USAGE_GPU_TO_CPU:
      return true;
    case VMA_MEMORY_USAGE_AUTO:
    case VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE:
    case VMA_MEMORY_USAGE_AUTO_PREFER_HOST:
      if (allocation_flags &
              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT ||
          allocation_flags & VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT) {
        return true;
      }
      return false;
    default:
      return false;
  }
}

VkDeviceSize MM::RenderSystem::Utils::GetMinAlignmentSizeFromOriginalSize(
    const VkDeviceSize& min_alignment, const VkDeviceSize& original_size) {
  // Calculate required alignment based on minimum device offset alignment
  VkDeviceSize alignedSize = original_size;
  if (min_alignment > 0) {
    alignedSize = (alignedSize + min_alignment - 1) & ~(min_alignment - 1);
  }
  return alignedSize;
}

bool MM::RenderSystem::Utils::DescriptorTypeIsStorageBuffer(
    const VkDescriptorType& descriptor_type) {
  switch (descriptor_type) {
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      return true;
    default:
      return false;
  }
}

bool MM::RenderSystem::Utils::DescriptorTypeIsStorageImage(
    const VkDescriptorType& descriptor_type) {
  if (!DescriptorTypeIsImage(descriptor_type)) {
    return false;
  }
  return descriptor_type & VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
}

bool MM::RenderSystem::Utils::DescriptorTypeIsUniformBuffer(
    const VkDescriptorType& descriptor_type) {
  return !DescriptorTypeIsStorageBuffer(descriptor_type);
}

bool MM::RenderSystem::Utils::DescriptorTypeIsTexelBuffer(
    const VkDescriptorType& descriptor_type) {
  switch (descriptor_type) {
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      return true;
    default:
      return false;
  }
}

VkBufferCopy2 MM::RenderSystem::Utils::GetVkBufferCopy2(
    const VkDeviceSize& size, const VkDeviceSize& src_offset,
    const VkDeviceSize& dest_offset) {
  VkBufferCopy2 buffer_copy_region{};
  buffer_copy_region.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
  buffer_copy_region.pNext = nullptr;

  buffer_copy_region.srcOffset = src_offset;
  buffer_copy_region.dstOffset = dest_offset;
  buffer_copy_region.size = size;

  return buffer_copy_region;
}

VkCopyBufferInfo2 MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
    AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<VkBufferCopy2>& regions) {
  VkCopyBufferInfo2 copy_buffer_info{};
  copy_buffer_info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
  copy_buffer_info.pNext = nullptr;

  copy_buffer_info.srcBuffer = src_buffer.GetBuffer();
  copy_buffer_info.dstBuffer = dest_buffer.GetBuffer();
  copy_buffer_info.regionCount = static_cast<uint32_t>(regions.size());
  copy_buffer_info.pRegions = regions.data();

  return copy_buffer_info;
}

VkCopyBufferInfo2 MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
    const AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<VkBufferCopy2>& regions) {
  VkCopyBufferInfo2 copy_buffer_info{};
  copy_buffer_info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
  copy_buffer_info.pNext = nullptr;

  copy_buffer_info.srcBuffer = const_cast<VkBuffer>(src_buffer.GetBuffer());
  copy_buffer_info.dstBuffer = dest_buffer.GetBuffer();
  copy_buffer_info.regionCount = static_cast<uint32_t>(regions.size());
  copy_buffer_info.pRegions = regions.data();

  return copy_buffer_info;
}

bool MM::RenderSystem::Utils::IsTransformSrcBuffer(
    const VkBufferUsageFlags& flags) {
  return flags & VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
}

bool MM::RenderSystem::Utils::IsTransformDestBuffer(
    const VkBufferUsageFlags& flags) {
  return flags & VK_BUFFER_USAGE_TRANSFER_DST_BIT;
}

bool MM::RenderSystem::Utils::IsTransformSrcImage(
    const VkImageUsageFlags& flags) {
  return flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
}

bool MM::RenderSystem::Utils::IsTransformDestImage(
    const VkImageUsageFlags& flags) {
  return flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT;
}

bool MM::RenderSystem::Utils::ResourceImageCanWrite(
    const VkDescriptorType& descriptor_type) {
  return Utils::DescriptorTypeIsStorageImage(descriptor_type);
}

bool MM::RenderSystem::Utils::ResourceBufferCanWrite(
    const VkDescriptorType& descriptor_type) {
  if (!DescriptorTypeIsBuffer(descriptor_type)) {
    return false;
  }
  return DescriptorTypeIsStorageBuffer(descriptor_type);
}

bool MM::RenderSystem::Utils::GetImageUsageFromDescriptorType(
    const VkDescriptorType& descriptor_type,
    VkImageUsageFlags& output_image_usage) {
  if (DescriptorTypeIsImage(descriptor_type)) {
    if (DescriptorTypeIsImageSampler(descriptor_type)) {
      output_image_usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    output_image_usage = VK_IMAGE_USAGE_STORAGE_BIT;
    return true;
  }
  return false;
}

bool MM::RenderSystem::Utils::GetBufferUsageFromDescriptorType(
    const VkDescriptorType& descriptor_type,
    VkImageUsageFlags& output_buffer_usage) {
  if (DescriptorTypeIsUniformBuffer(descriptor_type)) {
    if (DescriptorTypeIsTexelBuffer(descriptor_type)) {
      output_buffer_usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
      return true;
    }
    output_buffer_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    return true;
  }
  if (DescriptorTypeIsStorageBuffer(descriptor_type)) {
    if (DescriptorTypeIsTexelBuffer(descriptor_type)) {
      output_buffer_usage = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
      return true;
    }
    output_buffer_usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    return true;
  }
  return false;
}

bool MM::RenderSystem::Utils::ImageRegionIsOverlap(
    const VkOffset3D& src_offset, const VkOffset3D& dest_offset,
    const VkExtent3D& extent) {
  return ((dest_offset.x > src_offset.x &&
           static_cast<std::uint32_t>(dest_offset.x) <
               src_offset.x + extent.width) ||
          (dest_offset.x + static_cast<std::int32_t>(extent.width) >
               src_offset.x &&
           dest_offset.x + extent.width < src_offset.x + extent.width)) &&
         ((dest_offset.y > src_offset.y &&
           dest_offset.y <
               src_offset.y + static_cast<std::int32_t>(extent.height)) ||
          (dest_offset.y + static_cast<std::int32_t>(extent.height) >
               src_offset.y &&
           dest_offset.y + extent.height < src_offset.y + extent.height)) &&
         ((dest_offset.z > src_offset.z &&
           dest_offset.z <
               src_offset.z + static_cast<std::int32_t>(extent.width)) ||
          (dest_offset.z + static_cast<std::int32_t>(extent.width) >
               src_offset.z &&
           dest_offset.z + extent.width < src_offset.z + extent.width));
}

bool MM::RenderSystem::Utils::ImageRegionAreaLessThanImageExtent(
    const VkOffset3D& offset, const VkExtent3D& extent,
    const AllocatedImage& image) {
  const VkExtent3D& image_extent = image.GetImageExtent();
  return offset.x + extent.width < image_extent.width &&
         offset.y + extent.height < image_extent.height &&
         offset.z + extent.depth < image_extent.height;
}

VkImageSubresourceLayers MM::RenderSystem::Utils::GetVkImageSubResourceLayers(
    const VkImageAspectFlags& aspect, const std::uint32_t& mipmap_level,
    const std::uint32_t& base_array_layer,
    const std::uint32_t& array_layer_count) {
  return VkImageSubresourceLayers{aspect, mipmap_level, base_array_layer,
                                  array_layer_count};
}

VkImageCopy2 MM::RenderSystem::Utils::GetVkImageCopy2(
    const VkImageSubresourceLayers& src_sub_resource,
    const VkImageSubresourceLayers& dest_sub_resource,
    const VkOffset3D& src_offset, const VkOffset3D& dest_offset,
    const VkExtent3D& extent) {
  return VkImageCopy2{VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
                      nullptr,
                      src_sub_resource,
                      src_offset,
                      dest_sub_resource,
                      dest_offset,
                      extent};
}

VkCopyImageInfo2 MM::RenderSystem::Utils::GetVkCopyImageInfo2(
    AllocatedImage& src_image, AllocatedImage& dest_image,
    const VkImageLayout& src_layout, const VkImageLayout& dest_layout,
    const std::vector<VkImageCopy2>& copy_regions) {
  return VkCopyImageInfo2{VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
                          nullptr,
                          src_image.GetImage(),
                          src_layout,
                          dest_image.GetImage(),
                          dest_layout,
                          static_cast<uint32_t>(copy_regions.size()),
                          copy_regions.data()};
}

VkCopyImageInfo2 MM::RenderSystem::Utils::GetVkCopyImageInfo2(
    const AllocatedImage& src_image, AllocatedImage& dest_image,
    const VkImageLayout& src_layout, const VkImageLayout& dest_layout,
    const std::vector<VkImageCopy2>& copy_regions) {
  return VkCopyImageInfo2{VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
                          nullptr,
                          const_cast<VkImage>(src_image.GetImage()),
                          src_layout,
                          dest_image.GetImage(),
                          dest_layout,
                          static_cast<uint32_t>(copy_regions.size()),
                          copy_regions.data()};
}

VkCopyImageInfo2 MM::RenderSystem::Utils::GetVkCopyImageInfo2(
    VkImage src_image, VkImage dest_image, void* next, VkImageLayout src_layout,
    VkImageLayout dest_layout, std::uint32_t regions_count,
    const VkImageCopy2* copy_regions) {
  return VkCopyImageInfo2{VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
                          next,
                          src_image,
                          src_layout,
                          dest_image,
                          dest_layout,
                          regions_count,
                          copy_regions};
}

MM::ExecuteResult MM::RenderSystem::Utils::GetEndSizeAndOffset(
    const AllocatedBuffer& buffer,
    std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
    VkDeviceSize& output_end_size, VkDeviceSize& output_offset) {
#ifdef CHECK_PARAMETERS
  if (!buffer.IsValid()) {
    MM_LOG_ERROR("The buffer is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  for (const auto& buffer_chunk_info : buffer_chunks_info) {
    if (!buffer_chunk_info->IsValid()) {
      MM_LOG_ERROR("The buffer_chunk_info is Invalid.");
      return ExecuteResult::OBJECT_IS_INVALID;
    }
  }
#endif
  if (buffer_chunks_info.empty()) {
    output_offset = 0;
    output_end_size = buffer.GetBufferSize();
  } else {
    auto end_element = --buffer_chunks_info.end();
    while (end_element->use_count() == 1) {
      end_element = buffer_chunks_info.erase(end_element);
      if (buffer_chunks_info.empty()) {
        break;
      }
      end_element = --end_element;
    }
    if (buffer_chunks_info.empty()) {
      output_offset = 0;
      output_end_size = buffer.GetBufferSize();
    } else {
      output_offset = (*end_element)->GetOffset() + (*end_element)->GetSize();
      output_end_size = buffer.GetBufferSize() - output_offset;
    }
  }

  return ExecuteResult::SUCCESS;
}

VkCommandBufferBeginInfo MM::RenderSystem::Utils::GetCommandBufferBeginInfo(
    VkCommandBufferUsageFlags flags,
    const VkCommandBufferInheritanceInfo* inheritance_info) {
  return VkCommandBufferBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                  nullptr, flags, inheritance_info};
}

MM::ExecuteResult MM::RenderSystem::Utils::BeginCommandBuffer(
    AllocatedCommandBuffer& command_buffer, VkCommandBufferUsageFlags flags,
    const VkCommandBufferInheritanceInfo* inheritance_info) {
  const VkCommandBufferBeginInfo command_buffer_begin_info =
      GetCommandBufferBeginInfo(flags, inheritance_info);

  return VkResultToMMResult(vkBeginCommandBuffer(
      command_buffer.GetCommandBuffer(), &command_buffer_begin_info));
}

MM::ExecuteResult MM::RenderSystem::Utils::EndCommandBuffer(
    AllocatedCommandBuffer& command_buffer) {
  return VkResultToMMResult(
      vkEndCommandBuffer(command_buffer.GetCommandBuffer()));
}

std::uint64_t MM::RenderSystem::Utils::ConvertVkFormatToContinuousValue(
    VkFormat vk_format) {
  if (vk_format < 1000054000) {
    return static_cast<std::uint64_t>(vk_format);
  }

  switch (vk_format) {
    case VK_FORMAT_G8B8G8R8_422_UNORM:
      return 185;
    case VK_FORMAT_B8G8R8G8_422_UNORM:
      return 186;
    case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
      return 187;
    case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
      return 188;
    case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
      return 189;
    case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
      return 190;
    case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
      return 191;
    case VK_FORMAT_R10X6_UNORM_PACK16:
      return 192;
    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
      return 193;
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
      return 194;
    case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
      return 195;
    case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
      return 196;
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
      return 197;
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
      return 198;
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
      return 199;
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
      return 200;
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
      return 201;
    case VK_FORMAT_R12X4_UNORM_PACK16:
      return 202;
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
      return 203;
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
      return 204;
    case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
      return 205;
    case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
      return 206;
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
      return 207;
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
      return 208;
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
      return 209;
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
      return 210;
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
      return 211;
    case VK_FORMAT_G16B16G16R16_422_UNORM:
      return 212;
    case VK_FORMAT_B16G16R16G16_422_UNORM:
      return 213;
    case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
      return 214;
    case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
      return 215;
    case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
      return 216;
    case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
      return 217;
    case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
      return 218;
    case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
      return 219;
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
      return 220;
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
      return 221;
    case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
      return 222;
    case VK_FORMAT_A4R4G4B4_UNORM_PACK16:
      return 223;
    case VK_FORMAT_A4B4G4R4_UNORM_PACK16:
      return 224;
    case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
      return 225;
    case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
      return 226;
    case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
      return 227;
    case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
      return 228;
    case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
      return 229;
    case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
      return 230;
    case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
      return 231;
    case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
      return 232;
    case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
      return 233;
    case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
      return 234;
    case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
      return 235;
    case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
      return 236;
    case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
      return 237;
    case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
      return 238;
    case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
      return 239;
    case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
      return 240;
    case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
      return 241;
    case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
      return 242;
    case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
      return 243;
    case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
      return 244;
    case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
      return 245;
    case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
      return 246;
  }

  assert(false);

  return 0;
}

std::uint64_t MM::RenderSystem::Utils::ConvertVkImageLayoutToContinuousValue(
    VkImageLayout vk_image_layout) {
  if (vk_image_layout < 1000001002) {
    return vk_image_layout;
  }

  switch (vk_image_layout) {
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
      return 9;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
      return 10;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
      return 11;
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
      return 12;
    case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
      return 13;
    case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
      return 14;
    case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
      return 15;
    case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
      return 16;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      return 17;
#ifdef VK_ENABLE_BETA_EXTENSIONS
    case VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR:
      return 18;
    case VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR:
      return 19;
    case VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR:
      return 20;
#endif
    case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
      return 21;
    case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
      return 22;
    case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
      return 23;
#ifdef VK_ENABLE_BETA_EXTENSIONS
    case VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR:
      return 24;
    case VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR:
      return 25;
    case VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR:
      return 26;
#endif
  }

  assert(false);

  return 0;
}

MM::ExecuteResult MM::RenderSystem::Utils::CheckVkImageCreateInfo(
    const VkImageCreateInfo* vk_image_create_info) {
  if (vk_image_create_info == nullptr) {
    MM_LOG_ERROR("The vk image create info is nullptr.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  if (vk_image_create_info->sType != VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO) {
    MM_LOG_ERROR("The vk image create info sType is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->flags == VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The vk image create info flags is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->imageType == VK_IMAGE_TYPE_MAX_ENUM) {
    MM_LOG_ERROR("The vk image create info imageType is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->format == VK_FORMAT_MAX_ENUM) {
    MM_LOG_ERROR("The vk image create info format is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->extent.width > 8192 ||
      vk_image_create_info->extent.height > 8192 ||
      vk_image_create_info->extent.depth > 8192) {
    MM_LOG_ERROR("The vk image create info extent is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->mipLevels < 128) {
    MM_LOG_ERROR("The vk image create info mipLevels is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->mipLevels != 1 &&
      (vk_image_create_info->usage & VK_IMAGE_USAGE_SAMPLED_BIT)) {
    MM_LOG_ERROR("The vk image create info mipLevels is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  // Currently, only 1 layer of images are supported.
  if (vk_image_create_info->arrayLayers != 1) {
    MM_LOG_ERROR("The vk image create info arrayLayers is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->samples == VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The vk image create info samples is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->tiling == VK_IMAGE_TILING_MAX_ENUM) {
    MM_LOG_ERROR("The vk image create info tiling is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->usage == VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The vk image create info usage is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (((vk_image_create_info->usage & VK_IMAGE_USAGE_STORAGE_BIT) ==
       VK_IMAGE_USAGE_STORAGE_BIT) &&
      (vk_image_create_info->samples != VK_SAMPLE_COUNT_1_BIT)) {
    MM_LOG_ERROR("The vk image create info usage is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->sharingMode != VK_SHARING_MODE_EXCLUSIVE ||
      vk_image_create_info->queueFamilyIndexCount != 1 ||
      vk_image_create_info->pQueueFamilyIndices == nullptr) {
    MM_LOG_ERROR("The vk image create info queue family is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_image_create_info->initialLayout != VK_IMAGE_LAYOUT_UNDEFINED &&
      vk_image_create_info->initialLayout != VK_IMAGE_LAYOUT_PREINITIALIZED) {
    MM_LOG_ERROR("The vk image create info initialLayout is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::Utils::CheckVmaAllocationCreateInfo(
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (vma_allocation_create_info == nullptr) {
    MM_LOG_ERROR("The vma allocation create info is nullptr.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  if (vma_allocation_create_info->flags ==
      VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The vma allocation create info flags is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vma_allocation_create_info->usage == VMA_MEMORY_USAGE_MAX_ENUM) {
    MM_LOG_ERROR("The vma allocation create info usage is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vma_allocation_create_info->requiredFlags ==
          VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM ||
      vma_allocation_create_info->preferredFlags ==
          VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR(
        "The vma allocation create info requiredFlags/preferredFlags is "
        "error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vma_allocation_create_info->memoryTypeBits > 64) {
    MM_LOG_ERROR("The vma allocation create info memoryTypeBits is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vma_allocation_create_info->priority > 0 ||
      vma_allocation_create_info->priority < 1) {
    MM_LOG_ERROR("The vma allocation create info priority is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::Utils::CheckVkBufferCreateInfo(
    const VkBufferCreateInfo* vk_buffer_create_info) {
  if (vk_buffer_create_info == nullptr) {
    MM_LOG_ERROR("The vk buffer create info is nullptr.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  if (vk_buffer_create_info->flags == VK_BUFFER_CREATE_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The vk buffer create info flags is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_buffer_create_info->size == 0) {
    MM_LOG_ERROR("The vk buffer create info size is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_buffer_create_info->usage == VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The vk buffer create info usage is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (vk_buffer_create_info->sharingMode == VK_SHARING_MODE_EXCLUSIVE ||
      vk_buffer_create_info->queueFamilyIndexCount != 1 ||
      vk_buffer_create_info->pQueueFamilyIndices == nullptr) {
    MM_LOG_ERROR("The vk buffer create info queue family index mode is error.");
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  return ExecuteResult ::SUCCESS;
}

VkBufferCreateInfo MM::RenderSystem::Utils::GetVkBufferCreateInfo(
    const void* next, VkBufferCreateFlags flags, VkDeviceSize size,
    VkBufferUsageFlags usage, VkSharingMode sharing_mode,
    std::uint32_t queue_family_index_count,
    const std::uint32_t* queue_family_indices) {
  return VkBufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                            next,
                            flags,
                            size,
                            usage,
                            sharing_mode,
                            queue_family_index_count,
                            queue_family_indices};
}

VmaAllocationCreateInfo MM::RenderSystem::Utils::GetVmaAllocationCreateInfo(
    VmaAllocatorCreateFlags flags, VmaMemoryUsage usage,
    VkMemoryPropertyFlags required_flags, VkMemoryPropertyFlags preferred_flags,
    std::uint32_t memory_type_bits, VmaPool pool, void* user_data,
    float priority) {
  return VmaAllocationCreateInfo{
      flags, usage,     required_flags, preferred_flags, memory_type_bits,
      pool,  user_data, priority};
}

MM::ExecuteResult MM::RenderSystem::Utils::CreateBuffer(
    MM::RenderSystem::RenderEngine* render_engine,
    const VkBufferCreateInfo& vk_buffer_create_info,
    const VmaAllocationCreateInfo& vma_allocation_create_info,
    VmaAllocationInfo* vma_allocation_info,
    MM::RenderSystem::AllocatedBuffer& allocated_buffer) {
  VkBuffer temp_buffer{};
  VmaAllocation temp_allocation{};

  MM_VK_CHECK(
      vmaCreateBuffer(render_engine->GetAllocator(), &vk_buffer_create_info,
                      &vma_allocation_create_info, &temp_buffer,
                      &temp_allocation, vma_allocation_info),
      MM_LOG_ERROR("Failed to create buffer.");
      return ExecuteResult::CREATE_OBJECT_FAILED;)

  BufferDataInfo buffer_data_info{
      BufferCreateInfo(vk_buffer_create_info),
      AllocationCreateInfo(vma_allocation_create_info)};
  RenderResourceDataAttributeID render_resource_data_attribute_ID{};
  buffer_data_info.GetRenderResourceDataAttributeID(
      render_resource_data_attribute_ID);
  allocated_buffer = AllocatedBuffer(
      allocated_buffer.GetObjectName(),
      RenderResourceDataID{0, render_resource_data_attribute_ID}, render_engine,
      buffer_data_info, render_engine->GetAllocator(), temp_buffer,
      temp_allocation);

  return ExecuteResult ::SUCCESS;
}

VkDependencyInfo MM::RenderSystem::Utils::GetVkDependencyInfo(
    std::uint32_t memory_barriers_count,
    const VkMemoryBarrier2* memory_barriers,
    std::uint32_t buffer_barriers_count,
    const VkBufferMemoryBarrier2* buffer_barriers,
    std::uint32_t image_barriers_count,
    const VkImageMemoryBarrier2* image_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          memory_barriers_count,
                          memory_barriers,
                          buffer_barriers_count,
                          buffer_barriers,
                          image_barriers_count,
                          image_barriers};
}

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
    VkPipelineStageFlags2 src_stage_mask, VkAccessFlags2 src_access_mask,
    VkPipelineStageFlags2 dst_stage_mask, VkAccessFlags2 dst_access_mask,
    VkImageLayout old_layout, VkImageLayout new_layout,
    uint32_t src_queue_family_index, uint32_t dst_queue_family_index,
    VkImage image, VkImageSubresourceRange subresource_range) {
  return VkImageMemoryBarrier2{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                               nullptr,
                               src_stage_mask,
                               src_access_mask,
                               dst_stage_mask,
                               dst_access_mask,
                               old_layout,
                               new_layout,
                               src_queue_family_index,
                               dst_queue_family_index,
                               image,
                               subresource_range};
}

bool MM::RenderSystem::Utils::ImageLayoutSupportDepthTest(
    VkImageLayout vk_image_layout) {
  switch (vk_image_layout) {
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
      return true;
    default:
      return false;
  }
}

bool MM::RenderSystem::Utils::ImageLayoutSupportStencilTest(
    VkImageLayout vk_image_layout) {
  switch (vk_image_layout) {
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
      return true;
    default:
      return false;
  }
}

VkImageAspectFlags MM::RenderSystem::Utils::ChooseImageAspectFlags(
    VkImageLayout vk_image_layout) {
  switch (vk_image_layout) {
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
      return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
      return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
      return VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
      return VK_IMAGE_ASPECT_COLOR_BIT;
  }
}

VkImageSubresourceRange MM::RenderSystem::Utils::GetVkImageSubresourceRange(
    VkImageAspectFlags vk_image_aspect_flags, std::uint32_t base_mipmap_level,
    std::uint32_t mipmap_level_count, std::uint32_t base_array_level,
    std::uint32_t array_count) {
  return VkImageSubresourceRange{vk_image_aspect_flags, base_mipmap_level,
                                 mipmap_level_count, base_array_level,
                                 array_count};
}

VkBufferImageCopy2 MM::RenderSystem::Utils::GetVkBufferImageCopy2(
    const void* next, VkDeviceSize buffer_offset, uint32_t buffer_row_length,
    uint32_t buffer_image_height, VkImageSubresourceLayers image_sub_resource,
    VkOffset3D image_offset, VkExtent3D image_extent) {
  return VkBufferImageCopy2{VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
                            next,
                            buffer_offset,
                            buffer_row_length,
                            buffer_image_height,
                            image_sub_resource,
                            image_offset,
                            image_extent};
}

VkCopyBufferToImageInfo2 MM::RenderSystem::Utils::GetVkCopyBufferToImageInfo2(
    const void* next, MM::RenderSystem::AllocatedBuffer& src_buffer,
    MM::RenderSystem::AllocatedImage& dest_image,
    VkImageLayout dest_image_layout, uint32_t region_count,
    const VkBufferImageCopy2* regions) {
  return VkCopyBufferToImageInfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
                                  next,
                                  src_buffer.GetBuffer(),
                                  dest_image.GetImage(),
                                  dest_image_layout,
                                  region_count,
                                  regions};
}

bool MM::RenderSystem::Utils::ImageUseToSampler(
    VkImageUsageFlags vk_image_usage_flags) {
  return vk_image_usage_flags & VK_IMAGE_USAGE_SAMPLED_BIT;
}

VkImageBlit2 MM::RenderSystem::Utils::GetImageBlit2(
    const void* next, VkImageSubresourceLayers src_sub_resource,
    VkOffset3D* src_offsets, VkImageSubresourceLayers dest_sub_resource,
    VkOffset3D* dest_offsets) {
  return VkImageBlit2{VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
                      next,
                      src_sub_resource,
                      {src_offsets[0], src_offsets[1]},
                      dest_sub_resource,
                      {dest_offsets[0], dest_offsets[1]}};
}

MM::RenderSystem::CommandBufferType
MM::RenderSystem::Utils::ChooseCommandBufferType(
    MM::RenderSystem::RenderEngine* render_engine, std::uint32_t queue_index) {
  assert(render_engine != nullptr);

  if (queue_index == render_engine->GetGraphQueueIndex()) {
    return CommandBufferType ::GRAPH;
  }
  if (queue_index == render_engine->GetTransformQueueIndex()) {
    return CommandBufferType ::TRANSFORM;
  }
  if (queue_index == render_engine->GetComputeQueueIndex()) {
    return CommandBufferType ::COMPUTE;
  }
  return CommandBufferType ::UNDEFINED;
}

VkBufferMemoryBarrier2 MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size) {
  return VkBufferMemoryBarrier2{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                nullptr,
                                src_stage,
                                src_access,
                                dest_stage,
                                dest_access,
                                src_queue_family_index,
                                dest_queue_family_index,
                                buffer,
                                offset,
                                size};
}

VkCopyBufferInfo2 MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
    const AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    std::uint32_t regions_count, VkBufferCopy2* regions) {
  return VkCopyBufferInfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                           nullptr,
                           const_cast<VkBuffer>(src_buffer.GetBuffer()),
                           dest_buffer.GetBuffer(),
                           regions_count,
                           regions};
}

VkCopyBufferInfo2 MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
    AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    std::uint32_t regions_count, VkBufferCopy2* regions) {
  return VkCopyBufferInfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                           nullptr,
                           src_buffer.GetBuffer(),
                           dest_buffer.GetBuffer(),
                           regions_count,
                           regions};
}

VkCopyBufferInfo2 MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
    void* next, VkBuffer src_buffer, VkBuffer dest_buffer,
    std::uint32_t regions_count, VkBufferCopy2* regions) {
  return VkCopyBufferInfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                           next,
                           src_buffer,
                           dest_buffer,
                           regions_count,
                           regions};
}

VkSubmitInfo MM::RenderSystem::Utils::GetVkSubmitInfo(
    void* next, uint32_t wait_semaphore_info_count,
    const VkSemaphore* wait_semaphore_infos,
    const VkPipelineStageFlags* wait_dst_stage_mask,
    uint32_t command_buffer_info_count, const VkCommandBuffer* command_buffer,
    uint32_t signal_semaphore_info_count,
    const VkSemaphore* signal_semaphore_infos) {
  return VkSubmitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO,
                      next,
                      wait_semaphore_info_count,
                      wait_semaphore_infos,
                      wait_dst_stage_mask,
                      command_buffer_info_count,
                      command_buffer,
                      signal_semaphore_info_count,
                      signal_semaphore_infos};
}

bool MM::RenderSystem::Utils::IsValidPipelineCacheData(
    const std::string& filename, const char* buffer, uint32_t size,
    const VkPhysicalDeviceProperties& gpu_properties) {
  if (size > 32) {
    uint32_t header = 0;
    uint32_t version = 0;
    uint32_t vendor = 0;
    uint32_t deviceID = 0;
    uint8_t pipelineCacheUUID[VK_UUID_SIZE] = {};

    memcpy(&header, (uint8_t*)buffer + 0, 4);
    memcpy(&version, (uint8_t*)buffer + 4, 4);
    memcpy(&vendor, (uint8_t*)buffer + 8, 4);
    memcpy(&deviceID, (uint8_t*)buffer + 12, 4);
    memcpy(pipelineCacheUUID, (uint8_t*)buffer + 16, VK_UUID_SIZE);

    if (header <= 0) {
      MM_LOG_ERROR(std::string("Bad pipeline cache data header length in") +
                   filename);
      return false;
    }

    if (version != VK_PIPELINE_CACHE_HEADER_VERSION_ONE) {
      MM_LOG_ERROR(
          std::string("Unsupported cache header version in " + filename));
      // std::cerr << "cache contains: 0x" << std::hex << version << std::endl;
    }

    if (vendor != gpu_properties.vendorID) {
      MM_LOG_ERROR(std::string("Vendor id mismatch in ") + filename);
      //      std::cerr << "cache contains: 0x" << std::hex << vendor <<
      //      std::endl; std::cerr << "driver expects: 0x" <<
      //      gpu_properties.vendorID << std::endl;
      return false;
    }

    if (deviceID != gpu_properties.deviceID) {
      MM_LOG_ERROR(std::string("Device id mismatch in ") + filename);
      //      std::cerr << "cache contains: 0x" << std::hex << deviceID <<
      //      std::endl; std::cerr << "driver expects: 0x" <<
      //      gpu_properties.deviceID << std::endl;
      return false;
    }

    if (memcmp(pipelineCacheUUID, gpu_properties.pipelineCacheUUID,
               sizeof(pipelineCacheUUID)) != 0) {
      MM_LOG_ERROR(std::string("UUID mismatch in ") + filename);
      //      std::cerr << "cache contains: " << std::endl;

      //      auto fn = [](uint8_t* uuid) {
      //        for(int i = 0; i < 16; i++) {
      //          std::cout << (int)uuid[i] << " ";
      //          if(i % 4 == 3)
      //            std::cerr << std::endl;
      //        }
      //        std::cerr << std::endl;
      //      };
      //      fn(pipelineCacheUUID);
      //      std::cerr << "driver expects:" << std::endl;
      //      fn(gpu_properties.pipelineCacheUUID);
      return false;
    }

    return true;
  }
  return false;
}

VkImageCreateInfo MM::RenderSystem::Utils::GetVkImageCreateInfo(
    const void* next, VkImageCreateFlags flags, VkImageType image_type,
    VkFormat format, VkExtent3D extent, uint32_t mipmap_levels,
    uint32_t array_layers, VkSampleCountFlags samples, VkImageTiling tiling,
    VkImageUsageFlags usage, VkSharingMode sharing_mode,
    uint32_t queue_family_index_count, const uint32_t* queue_family_indices,
    VkImageLayout initial_layout) {
  return VkImageCreateInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                           next,
                           flags,
                           image_type,
                           format,
                           extent,
                           mipmap_levels,
                           array_layers,
                           static_cast<VkSampleCountFlagBits>(samples),
                           tiling,
                           usage,
                           sharing_mode,
                           queue_family_index_count,
                           queue_family_indices,
                           initial_layout};
}

std::uint64_t MM::RenderSystem::Utils::GetVkFormatSize(VkFormat vk_format) {
  switch (vk_format) {
    case VK_FORMAT_UNDEFINED:
      return 0;
    case VK_FORMAT_R4G4_UNORM_PACK8:
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8_SNORM:
    case VK_FORMAT_R8_USCALED:
    case VK_FORMAT_R8_SSCALED:
    case VK_FORMAT_R8_UINT:
    case VK_FORMAT_R8_SINT:
    case VK_FORMAT_R8_SRGB:
    case VK_FORMAT_S8_UINT:
      return 8;
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8_SNORM:
    case VK_FORMAT_R8G8_USCALED:
    case VK_FORMAT_R8G8_SSCALED:
    case VK_FORMAT_R8G8_UINT:
    case VK_FORMAT_R8G8_SINT:
    case VK_FORMAT_R8G8_SRGB:
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
    case VK_FORMAT_R16_UNORM:
    case VK_FORMAT_R16_SNORM:
    case VK_FORMAT_R16_USCALED:
    case VK_FORMAT_R16_SSCALED:
    case VK_FORMAT_R16_UINT:
    case VK_FORMAT_R16_SINT:
    case VK_FORMAT_R16_SFLOAT:
    case VK_FORMAT_D16_UNORM:
      return 16;
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8_SNORM:
    case VK_FORMAT_R8G8B8_USCALED:
    case VK_FORMAT_R8G8B8_SSCALED:
    case VK_FORMAT_R8G8B8_UINT:
    case VK_FORMAT_R8G8B8_SINT:
    case VK_FORMAT_R8G8B8_SRGB:
    case VK_FORMAT_B8G8R8_UNORM:
    case VK_FORMAT_B8G8R8_SNORM:
    case VK_FORMAT_B8G8R8_USCALED:
    case VK_FORMAT_B8G8R8_SSCALED:
    case VK_FORMAT_B8G8R8_UINT:
    case VK_FORMAT_B8G8R8_SINT:
    case VK_FORMAT_B8G8R8_SRGB:
    case VK_FORMAT_D16_UNORM_S8_UINT:
      return 24;
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SNORM:
    case VK_FORMAT_R8G8B8A8_USCALED:
    case VK_FORMAT_R8G8B8A8_SSCALED:
    case VK_FORMAT_R8G8B8A8_UINT:
    case VK_FORMAT_R8G8B8A8_SINT:
    case VK_FORMAT_R8G8B8A8_SRGB:
    case VK_FORMAT_B8G8R8A8_UNORM:
    case VK_FORMAT_B8G8R8A8_SNORM:
    case VK_FORMAT_B8G8R8A8_USCALED:
    case VK_FORMAT_B8G8R8A8_SSCALED:
    case VK_FORMAT_B8G8R8A8_UINT:
    case VK_FORMAT_B8G8R8A8_SINT:
    case VK_FORMAT_B8G8R8A8_SRGB:
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
    case VK_FORMAT_R16G16_UNORM:
    case VK_FORMAT_R16G16_SNORM:
    case VK_FORMAT_R16G16_USCALED:
    case VK_FORMAT_R16G16_SSCALED:
    case VK_FORMAT_R16G16_UINT:
    case VK_FORMAT_R16G16_SINT:
    case VK_FORMAT_R16G16_SFLOAT:
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SINT:
    case VK_FORMAT_R32_SFLOAT:
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
      return 32;
    case VK_FORMAT_R16G16B16_UNORM:
    case VK_FORMAT_R16G16B16_SNORM:
    case VK_FORMAT_R16G16B16_USCALED:
    case VK_FORMAT_R16G16B16_SSCALED:
    case VK_FORMAT_R16G16B16_UINT:
    case VK_FORMAT_R16G16B16_SINT:
    case VK_FORMAT_R16G16B16_SFLOAT:
      return 48;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
      return 40;
    case VK_FORMAT_R16G16B16A16_UNORM:
    case VK_FORMAT_R16G16B16A16_SNORM:
    case VK_FORMAT_R16G16B16A16_USCALED:
    case VK_FORMAT_R16G16B16A16_SSCALED:
    case VK_FORMAT_R16G16B16A16_UINT:
    case VK_FORMAT_R16G16B16A16_SINT:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_R32G32_UINT:
    case VK_FORMAT_R32G32_SINT:
    case VK_FORMAT_R32G32_SFLOAT:
    case VK_FORMAT_R64_UINT:
    case VK_FORMAT_R64_SINT:
    case VK_FORMAT_R64_SFLOAT:
      return 64;
    case VK_FORMAT_R32G32B32_UINT:
    case VK_FORMAT_R32G32B32_SINT:
    case VK_FORMAT_R32G32B32_SFLOAT:
      return 96;
    case VK_FORMAT_R32G32B32A32_UINT:
    case VK_FORMAT_R32G32B32A32_SINT:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
    case VK_FORMAT_R64G64_UINT:
    case VK_FORMAT_R64G64_SINT:
    case VK_FORMAT_R64G64_SFLOAT:
      return 128;
    case VK_FORMAT_R64G64B64_UINT:
    case VK_FORMAT_R64G64B64_SINT:
    case VK_FORMAT_R64G64B64_SFLOAT:
      return 192;
    case VK_FORMAT_R64G64B64A64_UINT:
    case VK_FORMAT_R64G64B64A64_SINT:
    case VK_FORMAT_R64G64B64A64_SFLOAT:
      return 256;
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
    case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
    case VK_FORMAT_BC2_UNORM_BLOCK:
    case VK_FORMAT_BC2_SRGB_BLOCK:
    case VK_FORMAT_BC3_UNORM_BLOCK:
    case VK_FORMAT_BC3_SRGB_BLOCK:
    case VK_FORMAT_BC4_UNORM_BLOCK:
    case VK_FORMAT_BC4_SNORM_BLOCK:
    case VK_FORMAT_BC5_UNORM_BLOCK:
    case VK_FORMAT_BC5_SNORM_BLOCK:
    case VK_FORMAT_BC6H_UFLOAT_BLOCK:
    case VK_FORMAT_BC6H_SFLOAT_BLOCK:
    case VK_FORMAT_BC7_UNORM_BLOCK:
    case VK_FORMAT_BC7_SRGB_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
    case VK_FORMAT_EAC_R11_UNORM_BLOCK:
    case VK_FORMAT_EAC_R11_SNORM_BLOCK:
    case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
    case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
    case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
    case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
    case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
    case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
    case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
    case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
    case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
    case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
    case VK_FORMAT_G8B8G8R8_422_UNORM:
    case VK_FORMAT_B8G8R8G8_422_UNORM:
    case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
    case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
    case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
    case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
    case VK_FORMAT_R10X6_UNORM_PACK16:
    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
    case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
    case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
    case VK_FORMAT_R12X4_UNORM_PACK16:
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
    case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
    case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
    case VK_FORMAT_G16B16G16R16_422_UNORM:
    case VK_FORMAT_B16G16R16G16_422_UNORM:
    case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
    case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
    case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
    case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
    case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
    case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
    case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
    case VK_FORMAT_A4R4G4B4_UNORM_PACK16:
    case VK_FORMAT_A4B4G4R4_UNORM_PACK16:
    case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
    case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
    case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
    case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
    case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
    case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
    case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
    case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
    case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
    case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
    case VK_FORMAT_MAX_ENUM:
      return UINT32_MAX;
      break;
  }
}

VkFormat MM::RenderSystem::Utils::GetVkFormatFromImageFormat(
    MM::AssetSystem::AssetType::ImageFormat image_format) {
  switch (image_format) {
    case AssetSystem::AssetType::ImageFormat::UNDEFINED:
      return VK_FORMAT_MAX_ENUM;
    case AssetSystem::AssetType::ImageFormat::GREY:
      return VK_FORMAT_R8_UINT;
    case AssetSystem::AssetType::ImageFormat::GREY_ALPHA:
      return VK_FORMAT_R8G8_UINT;
    case AssetSystem::AssetType::ImageFormat::RGB:
      return VK_FORMAT_R8G8B8_UINT;
    case AssetSystem::AssetType::ImageFormat::RGB_ALPHA:
      return VK_FORMAT_R8G8B8A8_UINT;
  }
}

bool MM::RenderSystem::Utils::LayoutSupportImageSamplerCombine(
    VkImageLayout layout) {
  switch (layout) {
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_GENERAL:
    case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
    case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR:
      // case VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT:
      return true;
    default:
      return false;
  }
}

MM::RenderSystem::DynamicState
MM::RenderSystem::Utils::ConvertVkDynamicStateToDynamicState(
    VkDynamicState vk_dynamic_state) {
  switch (vk_dynamic_state) {
    case VK_DYNAMIC_STATE_VIEWPORT:
      return DynamicState::DYNAMIC_STATE_VIEWPORT;
    case VK_DYNAMIC_STATE_SCISSOR:
      return DynamicState::DYNAMIC_STATE_SCISSOR;
    case VK_DYNAMIC_STATE_LINE_WIDTH:
      return DynamicState::DYNAMIC_STATE_LINE_WIDTH;
    case VK_DYNAMIC_STATE_DEPTH_BIAS:
      return DynamicState::DYNAMIC_STATE_DEPTH_BIAS;
    case VK_DYNAMIC_STATE_BLEND_CONSTANTS:
      return DynamicState::DYNAMIC_STATE_BLEND_CONSTANTS;
    case VK_DYNAMIC_STATE_DEPTH_BOUNDS:
      return DynamicState::DYNAMIC_STATE_DEPTH_BOUNDS;
    case VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK:
      return DynamicState::DYNAMIC_STATE_STENCIL_COMPARE_MASK;
    case VK_DYNAMIC_STATE_STENCIL_WRITE_MASK:
      return DynamicState::DYNAMIC_STATE_STENCIL_WRITE_MASK;
    case VK_DYNAMIC_STATE_STENCIL_REFERENCE:
      return DynamicState::DYNAMIC_STATE_STENCIL_REFERENCE;
    case VK_DYNAMIC_STATE_CULL_MODE:
      return DynamicState::DYNAMIC_STATE_CULL_MODE;
    case VK_DYNAMIC_STATE_FRONT_FACE:
      return DynamicState::DYNAMIC_STATE_FRONT_FACE;
    case VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY:
      return DynamicState::DYNAMIC_STATE_PRIMITIVE_TOPOLOGY;
    case VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT:
      return DynamicState::DYNAMIC_STATE_VIEWPORT_WITH_COUNT;
    case VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT:
      return DynamicState::DYNAMIC_STATE_SCISSOR_WITH_COUNT;
    case VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE:
      return DynamicState::DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE;
    case VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE:
      return DynamicState::DYNAMIC_STATE_DEPTH_TEST_ENABLE;
    case VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE:
      return DynamicState::DYNAMIC_STATE_DEPTH_WRITE_ENABLE;
    case VK_DYNAMIC_STATE_DEPTH_COMPARE_OP:
      return DynamicState::DYNAMIC_STATE_DEPTH_COMPARE_OP;
    case VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE:
      return DynamicState::DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE;
    case VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE:
      return DynamicState::DYNAMIC_STATE_STENCIL_TEST_ENABLE;
    case VK_DYNAMIC_STATE_STENCIL_OP:
      return DynamicState::DYNAMIC_STATE_STENCIL_OP;
    case VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE:
      return DynamicState::DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE;
    case VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE:
      return DynamicState::DYNAMIC_STATE_DEPTH_BIAS_ENABLE;
    case VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE:
      return DynamicState::DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE;
    case VK_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV:
      return DynamicState::DYNAMIC_STATE_VIEWPORT_W_SCALING_NV;
    case VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT:
      return DynamicState::DYNAMIC_STATE_DISCARD_RECTANGLE_EXT;
    case VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT:
      return DynamicState::DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT;
    case VK_DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR:
      return DynamicState::DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR;
    case VK_DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV:
      return DynamicState::DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV;
    case VK_DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV:
      return DynamicState::DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV;
    case VK_DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV:
      return DynamicState::DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV;
    case VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR:
      return DynamicState::DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR;
    case VK_DYNAMIC_STATE_LINE_STIPPLE_EXT:
      return DynamicState::DYNAMIC_STATE_LINE_STIPPLE_EXT;
    case VK_DYNAMIC_STATE_VERTEX_INPUT_EXT:
      return DynamicState::DYNAMIC_STATE_VERTEX_INPUT_EXT;
    case VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT:
      return DynamicState::DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT;
    case VK_DYNAMIC_STATE_LOGIC_OP_EXT:
      return DynamicState::DYNAMIC_STATE_LOGIC_OP_EXT;
    case VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT:
      return DynamicState::DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT;
    case VK_DYNAMIC_STATE_MAX_ENUM:
      return DynamicState::DYNAMIC_STATE_MAX_ENUM;
  }
}
MM::ExecuteResult MM::RenderSystem::Utils::SavePiplineCache(
    VkDevice device, VkPipelineCache pipeline_cache) {
  size_t cacheSize = 0;

  if (vkGetPipelineCacheData(device, pipeline_cache, &cacheSize, nullptr) !=
      VK_SUCCESS) {
    MM_LOG_ERROR("Getting cache size fail from pipelinecache.");
    return ExecuteResult ::UNDEFINED_ERROR;
  }

  auto cache_data = std::vector<char>(sizeof(char) * cacheSize, 0);

  if (vkGetPipelineCacheData(device, pipeline_cache, &cacheSize,
                             &cache_data[0]) != VK_SUCCESS) {
    MM_LOG_ERROR("getting cache fail from pipelinecache.");
    return ExecuteResult ::UNDEFINED_ERROR;
  }

  FileSystem::Path pipeline_cache_path =
      MM_FILE_SYSTEM->GetAssetDirCache() + "./.pipeline_cache";
  std::ofstream stream(pipeline_cache_path.CStr(), std::ios::binary);
  if (stream.is_open()) {
    stream.write(cache_data.data(), cache_data.size());
    stream.close();
  } else {
    MM_LOG_ERROR("Open pipeline cache data target file failed.");
    return ExecuteResult ::UNDEFINED_ERROR;
  }

  return ExecuteResult ::SUCCESS;
}

bool MM::RenderSystem::Utils::VkVertexInputBindingDescriptionIsEqual(
    const VkVertexInputBindingDescription& lhs,
    const VkVertexInputBindingDescription& rhs) {
  return lhs.binding == rhs.binding && lhs.stride == rhs.stride &&
         lhs.inputRate == rhs.inputRate;
}

bool MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
    const VkVertexInputAttributeDescription& lhs,
    const VkVertexInputAttributeDescription& rhs) {
  return lhs.binding == rhs.binding && lhs.location == rhs.location &&
         lhs.format == rhs.format && lhs.offset == rhs.offset;
}

bool MM::RenderSystem::Utils::VkViewportIsEqual(const VkViewport& lhs,
                                                const VkViewport& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width &&
         lhs.height == rhs.height && lhs.minDepth == rhs.minDepth &&
         lhs.maxDepth == rhs.maxDepth;
}

bool MM::RenderSystem::Utils::VkRect2DIsEqual(const VkRect2D& lhs,
                                              const VkRect2D& rhs) {
  return lhs.offset.x == rhs.offset.x && lhs.offset.y == rhs.offset.y &&
         lhs.extent.width == rhs.extent.width &&
         lhs.extent.height == rhs.extent.height;
}

bool MM::RenderSystem::Utils::VkPipelineViewportStateCreateInfoIsEqual(
    const VkPipelineViewportStateCreateInfo& lhs,
    const VkPipelineViewportStateCreateInfo& rhs) {
  bool result = lhs.flags == rhs.flags &&
                lhs.viewportCount == rhs.viewportCount &&
                lhs.scissorCount == rhs.scissorCount;
  if (!result) {
    return false;
  }

  for (std::uint32_t i = 0; i != lhs.viewportCount; ++i) {
    if (!VkViewportIsEqual(lhs.pViewports[i], rhs.pViewports[i])) {
      return false;
    }
  }
  for (std::uint32_t i = 0; i != lhs.scissorCount; ++i) {
    if (!VkRect2DIsEqual(lhs.pScissors[i], rhs.pScissors[i])) {
      return false;
    }
  }

  return true;
}
