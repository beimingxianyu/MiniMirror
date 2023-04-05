#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>

#include "vk_type.h"
#include "runtime/function/render/pre_header.h"

namespace MM {
namespace RenderSystem {
class AllocatedImage;
class AllocatedBuffer;
class RenderEngine;

enum class ImageTransferMode {
  INIT_TO_ATTACHMENT,
  INIT_TO_TRANSFER_DESTINATION,
  TRANSFER_DESTINATION_TO_SHARED_READABLE,
  TRANSFER_DESTINATION_TO_SHARED_PRESENT,
  ATTACHMENT_TO_PRESENT,
  INIT_TO_DEPTH_TEST,
  ATTACHMENT_TO_TRANSFER_SOURCE,
  TRANSFER_DESTINATION_TO_TRANSFER_SOURCE
};

enum class ResourceType {
  Texture,
  BUFFER,
  VERTEX_BUFFER,
  FRAME_BUFFER,
  CONSTANTS,
  UNDEFINED
};

/**
 * \brief Memory operations allowed for rendering resources.
 */
enum class MemoryOperate { READ, WRITE, READ_AND_WRITE, UNDEFINED };

enum class CommandBufferType { GRAPH, COMPUTE };

namespace Utils {
// TODO Add default value annotations for various functions that obtain creation information, such as the \ref GetSamplerCreateInfo function.
VkCommandPoolCreateInfo GetCommandPoolCreateInfo(
    const uint32_t& queue_family_index, const VkCommandPoolCreateFlags& flags = 0);

VkCommandBufferAllocateInfo GetCommandBufferAllocateInfo(
    const VkCommandPool& command_pool, const uint32_t& count = 1,
    const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkBufferCreateInfo GetBufferCreateInfo(const VkDeviceSize& size,
                                       const VkBufferUsageFlags& usage,
                                       const VkBufferCreateFlags& flags = 0);

VkImageCreateInfo GetImageCreateInfo(const VkFormat& image_format,
                                     const VkImageUsageFlags& usage,
                                     const VkExtent3D& extent,
                                     const VkImageCreateFlags& flags = 0);

VmaAllocationCreateInfo GetVmaAllocationCreateInfo(
    const VmaMemoryUsage& usage = VMA_MEMORY_USAGE_AUTO,
    const VmaAllocationCreateFlags& flags = 0);

VkCommandBufferBeginInfo GetCommandBufferBeginInfo(
    const VkCommandBufferUsageFlags& flags = 0);

VkFenceCreateInfo GetFenceCreateInfo(const VkFenceCreateFlags& flags = 0);

VkFence GetVkFence(const VkDevice& device, const bool& is_signaled = false);

VkSubmitInfo GetCommandSubmitInfo(const VkCommandBuffer& command_buffer, const uint32_t& command_count = 1);

/**
 * \brief Get \ref VkImageMemoryBarrier2, but performance is poor.
 * \param image The image that this barrier will affect.
 * \param transfer_mode The image transfer operation mode to be executed.
 * \return The \ref VkImageMemoryBarrier2
 * \remark Blockage at all stages, poor performance. \n
 * \remark sub_image.aspectMask = (transfer_mode == ImageTransferMode::INIT_TO_DEPTH_TEST) \n
 *                          ? VK_IMAGE_ASPECT_DEPTH_BIT \n
 *                          : VK_IMAGE_ASPECT_COLOR_BIT; \n
 * sub_image.baseMipLevel = 0; \n
 * sub_image.levelCount = 1; \n
 * sub_image.baseArrayLayer = 0; \n
 * sub_image.layerCount = 1;
 */
VkImageMemoryBarrier2 GetImageTransferBarrier(AllocatedImage& image,
                                              const ImageTransferMode&
                                              transfer_mode);

/**
 * \brief Get \ref VkImageMemoryBarrier2, but performance is poor.
 * \param image image The image that this barrier will affect.
 * \param old_layout Old layout.
 * \param new_layout New layout.
 * \return The \ref VkImageMemoryBarrier2
 * \remark Blockage at all stages, poor performance.
 */
VkImageMemoryBarrier2 GetImageTransferBarrier(AllocatedImage& image,
                                              const VkImageLayout& old_layout,
                                              const VkImageLayout& new_layout);

VkDependencyInfo GetImageDependencyInfo(VkImageMemoryBarrier2& image_barrier,
                                        const VkDependencyFlags& flags = 0);
/**
 * \remark Blockage at all stages, poor performance.
*/
void AddTransferImageCommands(VkCommandBuffer& command_buffer, AllocatedImage& image,
                              const ImageTransferMode& transfer_mode,
                              const VkDependencyFlags& flags = 0);

void AddTransferImageCommands(VkCommandBuffer& command_buffer,
                              AllocatedImage& image,
                              const VkImageLayout& old_layout,
                              const VkImageLayout& new_layout,
                              const VkDependencyFlags& flags = 0);

VkBufferImageCopy GetBufferToImageCopyRegion(
    const VkImageAspectFlagBits& aspect, const VkExtent3D& image_extent,
    const VkDeviceSize& buffer_offset = 0,
    const VkOffset3D& image_offset = VkOffset3D{0, 0, 0});

bool DescriptorTypeIsImage(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsImageSampler(const VkDescriptorType& descriptor_type);

VkImageViewCreateInfo GetImageViewCreateInfo(const AllocatedImage& image,
                                             const VkFormat& format,
                                             const VkImageViewType& view_type,
                                             const VkImageAspectFlags&
                                             aspect_flags,
                                             const VkImageViewCreateFlags& flags
                                                 = 0);
/**
 *\remark The default setting is: \n
 *sampler_create_info.magFilter = VK_FILTER_LINEAR; \n
 *sampler_create_info.minFilter = VK_FILTER_LINEAR; \n
 *sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; \n
 *sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; \n
 *sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; \n
 *sampler_create_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy; \n
 *sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK; \n
 *sampler_create_info.unnormalizedCoordinates = VK_FALSE; \n
 *sampler_create_info.compareEnable = VK_FALSE; \n
 *sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS; \n
 *sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; \n
 *sampler_create_info.mipLodBias = 0.0f; \n
 *sampler_create_info.minLod = 0.0f; \n
 *sampler_create_info.maxLod = 0.0f; 
**/
VkSamplerCreateInfo GetSamplerCreateInfo(const VkPhysicalDevice& physical_device);

VkSemaphoreCreateInfo GetSemaphoreCreateInfo();

bool DescriptorTypeIsDynamicBuffer(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsBuffer(const VkDescriptorType& descriptor_type);

/**
 * \brief Determine whether the buffer/image can be mapped (accessible to the CPU)
 * based on \ref memory_usage and \ref allocation_flags.
 */
bool CanBeMapped(const VmaMemoryUsage& memory_usage,
                 const VmaAllocationCreateFlags& allocation_flags);

VkDeviceSize GetMinAlignmentSizeFromOriginalSize(const VkDeviceSize& min_alignment,
                              const VkDeviceSize& original_size);

bool DescriptorTypeIsStorageBuffer(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsStorageImage(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsUniformBuffer(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsTexel(const VkDescriptorType& descriptor_type);

AllocatedBuffer CreateBuffer(
    const RenderEngine* engine,
    const size_t& alloc_size, const VkBufferUsageFlags& usage,
    const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags = 0);

VkBufferCopy2 GetCopyBufferRegion(const VkDeviceSize& size,
                                  const VkDeviceSize& src_offset = 0,
                                  const VkDeviceSize& dest_offset = 0);

VkCopyBufferInfo2 GetCopyBufferInfo(const AllocatedBuffer& src_buffer,
                                    const AllocatedBuffer& dest_buffer,
                                    const std::vector<VkBufferCopy2>& regions);

bool IsTransformSrcBuffer(const VkBufferUsageFlags& flags);

bool IsTransformDestBuffer(const VkBufferUsageFlags& flags);

bool IsTransformSrcImage(const VkImageUsageFlags& flags);

bool IsTransformDestImage(const VkImageUsageFlags& flags);

bool ResourceImageCanWrite(const VkDescriptorType& descriptor_type);

bool ResourceBufferCanWrite(const VkDescriptorType& descriptor_type);
}
}
}
