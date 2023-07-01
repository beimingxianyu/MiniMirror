#include "runtime/function/render/vk_utils.h"

#include <cstdint>
#include <string>
#include <vector>

#include "runtime/function/render/AllocatedImage.h"
#include "runtime/function/render/vk_engine.h"
#include "utils/marco.h"

MM::ExecuteResult MM::RenderSystem::Utils::VkResultToMMResult(
    VkResult vk_result) {
  ExecuteResult MM_result = ExecuteResult::SUCCESS;
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

VkBufferCreateInfo MM::RenderSystem::Utils::GetBufferCreateInfo(
    const VkDeviceSize& size, const VkBufferUsageFlags& usage,
    const VkBufferCreateFlags& flags) {
  VkBufferCreateInfo buffer_create_info{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.pNext = nullptr;

  buffer_create_info.flags = flags;
  buffer_create_info.size = size;
  buffer_create_info.usage = usage;

  buffer_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
  // By default, this buffer can be accessed by multiple queue families,
  // so the following two items can be ignored.
  // buffer_create_info.queueFamilyIndexCount = 0;
  // buffer_create_info.pQueueFamilyIndices = nullptr;

  return buffer_create_info;
}

VkBufferCreateInfo MM::RenderSystem::Utils::GetBufferCreateInfo(
    const VkDeviceSize& size, const VkBufferUsageFlags& usage,
    std::vector<std::uint32_t>& queue_index, const VkBufferCreateFlags& flags) {
  VkBufferCreateInfo buffer_create_info{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.pNext = nullptr;

  buffer_create_info.flags = flags;
  buffer_create_info.size = size;
  buffer_create_info.usage = usage;

  if (queue_index.empty()) {
    buffer_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
  } else {
    buffer_create_info.queueFamilyIndexCount = queue_index.size();
    buffer_create_info.pQueueFamilyIndices = queue_index.data();
  }

  return buffer_create_info;
}

VkImageCreateInfo MM::RenderSystem::Utils::GetImageCreateInfo(
    const VkFormat& image_format, const VkImageUsageFlags& usage,
    const VkExtent3D& extent, const VkImageCreateFlags& flags) {
  VkImageCreateInfo image_create_info{};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.pNext = nullptr;

  image_create_info.imageType = VK_IMAGE_TYPE_2D;

  image_create_info.flags = flags;
  image_create_info.format = image_format;
  image_create_info.usage = usage;
  image_create_info.extent = extent;

  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

  image_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;

  return image_create_info;
}

VkImageCreateInfo MM::RenderSystem::Utils::GetImageCreateInfo(
    const VkFormat& image_format, const VkImageUsageFlags& usage,
    const VkExtent3D& extent, std::vector<std::uint32_t>& queue_index,
    const VkImageCreateFlags& flags) {
  VkImageCreateInfo image_create_info{};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.pNext = nullptr;

  image_create_info.imageType = VK_IMAGE_TYPE_2D;

  image_create_info.flags = flags;
  image_create_info.format = image_format;
  image_create_info.usage = usage;
  image_create_info.extent = extent;

  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

  if (queue_index.empty()) {
    image_create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
  } else {
    image_create_info.queueFamilyIndexCount = queue_index.size();
    image_create_info.pQueueFamilyIndices = queue_index.data();
  }

  return image_create_info;
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
  VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &fence),
           fence = nullptr;
           LOG_ERROR("Failed to create VkFence!"))
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
    vk_command_buffers.emplace_back(command_buffer.GetCommandBuffer());
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

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetImageMemoryBarrier(
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

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetImageMemoryBarrier(
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

VkDependencyInfo MM::RenderSystem::Utils::GetImageDependencyInfo(
    VkImageMemoryBarrier2& image_barrier, const VkDependencyFlags& flags) {
  VkDependencyInfo dep_info{};
  dep_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dep_info.pNext = nullptr;

  dep_info.imageMemoryBarrierCount = 1;
  dep_info.pImageMemoryBarriers = &image_barrier;

  dep_info.dependencyFlags = flags;

  return dep_info;
}

VkMemoryBarrier2 MM::RenderSystem::Utils::GetMemoryBarrier(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access) {
  return VkMemoryBarrier2{VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                          nullptr,
                          src_stage,
                          src_access,
                          dest_stage,
                          dest_access};
}

VkBufferMemoryBarrier2 MM::RenderSystem::Utils::GetBufferMemoryBarrier(
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
                                buffer.GetBuffer(),
                                offset,
                                size};
}

VkImageMemoryBarrier2 MM::RenderSystem::Utils::GetImageMemoryBarrier(
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
                               image.GetImage(),
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
      image.GetImage(),
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

VkDependencyInfo MM::RenderSystem::Utils::GetDependencyInfo(
    const std::vector<VkMemoryBarrier2>& memory_barriers,
    const std::vector<VkBufferMemoryBarrier2>& buffer_barriers,
    const std::vector<VkImageMemoryBarrier2>& image_barriers,
    VkDependencyFlags dependency_flags) {
  return VkDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                          nullptr,
                          dependency_flags,
                          static_cast<uint32_t>(memory_barriers.size()),
                          memory_barriers.data(),
                          static_cast<uint32_t>(buffer_barriers.size()),
                          buffer_barriers.data(),
                          static_cast<uint32_t>(image_barriers.size()),
                          image_barriers.data()};
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    VkCommandBuffer& command_buffer, AllocatedImage& image,
    const ImageTransferMode& transfer_mode, const VkDependencyFlags& flags) {
  auto image_barrier = Utils::GetImageMemoryBarrier(image, transfer_mode);
  const auto image_transfer_dependency =
      Utils::GetImageDependencyInfo(image_barrier, flags);

  vkCmdPipelineBarrier2(command_buffer, &image_transfer_dependency);
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    VkCommandBuffer& command_buffer, AllocatedImage& image,
    const VkImageLayout& old_layout, const VkImageLayout& new_layout,
    const VkDependencyFlags& flags) {
  auto image_barrier =
      Utils::GetImageMemoryBarrier(image, old_layout, new_layout);
  auto image_transfer_dependency =
      Utils::GetImageDependencyInfo(image_barrier, flags);

  vkCmdPipelineBarrier2(command_buffer, &image_transfer_dependency);
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    AllocatedCommandBuffer& command_buffer, AllocatedImage& image,
    const ImageTransferMode& transfer_mode, const VkDependencyFlags& flags) {
  auto image_barrier = Utils::GetImageMemoryBarrier(image, transfer_mode);
  const auto image_transfer_dependency =
      Utils::GetImageDependencyInfo(image_barrier, flags);

  vkCmdPipelineBarrier2(command_buffer.GetCommandBuffer(),
                        &image_transfer_dependency);
}

void MM::RenderSystem::Utils::AddTransferImageCommands(
    AllocatedCommandBuffer& command_buffer, AllocatedImage& image,
    const VkImageLayout& old_layout, const VkImageLayout& new_layout,
    const VkDependencyFlags& flags) {
  auto image_barrier =
      Utils::GetImageMemoryBarrier(image, old_layout, new_layout);
  auto image_transfer_dependency =
      Utils::GetImageDependencyInfo(image_barrier, flags);

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
  if (descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLER ||
      descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
      descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
      descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
      descriptor_type == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT) {
    return true;
  }
  return false;
}

bool MM::RenderSystem::Utils::DescriptorTypeIsImageSampler(
    const VkDescriptorType& descriptor_type) {
  if (descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLER ||
      descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
      descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
    return true;
  }
  return false;
}

VkImageViewCreateInfo MM::RenderSystem::Utils::GetImageViewCreateInfo(
    const AllocatedImage& image, const VkFormat& format,
    const VkImageViewType& view_type, const VkImageAspectFlags& aspect_flags,
    const VkImageViewCreateFlags& flags) {
  VkImageViewCreateInfo image_view_create_info{};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.pNext = nullptr;

  image_view_create_info.image = image.GetImage();
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
  if (descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
      descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
    return true;
  }
  return false;
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
      break;
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

MM::RenderSystem::AllocatedBuffer MM::RenderSystem::Utils::CreateBuffer(
    const RenderEngine* engine, const size_t& alloc_size,
    const VkBufferUsageFlags& usage, const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags,
    const bool& is_BDA_buffer) {
  VkBufferUsageFlags temp_usage = usage;
  VmaAllocationCreateFlags temp_allocation_flags = allocation_flags;
  if (is_BDA_buffer) {
    temp_usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    temp_allocation_flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  }

  const auto buffer_Info =
      MM::RenderSystem::Utils::GetBufferCreateInfo(alloc_size, temp_usage);

  VmaAllocationCreateInfo vma_alloc_info = {};
  vma_alloc_info.usage = memory_usage;
  vma_alloc_info.flags = temp_allocation_flags;

  VkBuffer temp_buffer{};
  VmaAllocation temp_allocation{};

  VK_CHECK(
      vmaCreateBuffer(engine->GetAllocator(), &buffer_Info, &vma_alloc_info,
                      &temp_buffer, &temp_allocation, nullptr),
      LOG_ERROR("Failed to create buffer."))

  const BufferInfo temp_buffer_info{
      alloc_size, Utils::CanBeMapped(memory_usage, allocation_flags),
      static_cast<bool>(usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
      static_cast<bool>(usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)};

  return AllocatedBuffer(engine->GetAllocator(), temp_buffer, temp_allocation,
                         temp_buffer_info);
}

VkBufferCopy2 MM::RenderSystem::Utils::GetBufferCopy(
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

VkCopyBufferInfo2 MM::RenderSystem::Utils::GetCopyBufferInfo(
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

VkCopyBufferInfo2 MM::RenderSystem::Utils::GetCopyBufferInfo(
    const AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
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
  if (((dest_offset.x > src_offset.x &&
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
        dest_offset.z + extent.width < src_offset.z + extent.width))) {
    return true;
  }

  return false;
}

bool MM::RenderSystem::Utils::ImageRegionAreaLessThanImageExtent(
    const VkOffset3D& offset, const VkExtent3D& extent,
    const AllocatedImage& image) {
  const VkExtent3D& image_extent = image.GetImageExtent();
  if (offset.x + extent.width < image_extent.width &&
      offset.y + extent.height < image_extent.height &&
      offset.z + extent.depth < image_extent.height) {
    return true;
  }

  return false;
}

VkImageSubresourceLayers MM::RenderSystem::Utils::GetImageSubResourceLayers(
    const VkImageAspectFlags& aspect, const std::uint32_t& mipmap_level,
    const std::uint32_t& base_array_layer,
    const std::uint32_t& array_layer_count) {
  return VkImageSubresourceLayers{aspect, mipmap_level, base_array_layer,
                                  array_layer_count};
}

VkImageCopy2 MM::RenderSystem::Utils::GetImageCopy(
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

VkCopyImageInfo2 MM::RenderSystem::Utils::GetCopyImageInfo(
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

VkCopyImageInfo2 MM::RenderSystem::Utils::GetCopyImageInfo(
    const AllocatedImage& src_image, AllocatedImage& dest_image,
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

MM::ExecuteResult MM::RenderSystem::Utils::GetEndSizeAndOffset(
    const AllocatedBuffer& buffer,
    std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
    VkDeviceSize& output_end_size, VkDeviceSize& output_offset) {
#ifdef CHECK_PARAMETERS
  if (!buffer.IsValid()) {
    LOG_ERROR("The buffer is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  for (const auto& buffer_chunk_info : buffer_chunks_info) {
    if (!buffer_chunk_info->IsValid()) {
      LOG_ERROR("The buffer_chunk_info is Invalid.");
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
  if (vk_format < 1000156000) {
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
