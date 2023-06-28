#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <vector>

#include "runtime/function/render/import_other_system.h"
#include "runtime/function/render/vk_command.h"
#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {
class AllocatedImage;
class AllocatedBuffer;
class AllocatedCommandBuffer;
class RenderEngine;
class BufferChunkInfo;

namespace Utils {
ExecuteResult VkResultToMMResult(VkResult vk_result);

// TODO Add default value annotations for various functions that obtain creation
// information, such as the \ref GetSamplerCreateInfo function.
VkCommandPoolCreateInfo GetCommandPoolCreateInfo(
    const uint32_t& queue_family_index,
    const VkCommandPoolCreateFlags& flags = 0);

VkCommandBufferAllocateInfo GetCommandBufferAllocateInfo(
    const VkCommandPool& command_pool, const uint32_t& count = 1,
    const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

[[deprecated]] VkBufferCreateInfo GetBufferCreateInfo(
    const VkDeviceSize& size, const VkBufferUsageFlags& usage,
    const VkBufferCreateFlags& flags = 0);

VkBufferCreateInfo GetBufferCreateInfo(const VkDeviceSize& size,
                                       const VkBufferUsageFlags& usage,
                                       std::vector<std::uint32_t>& queue_index,
                                       const VkBufferCreateFlags& flags = 0);
[[deprecated]] VkImageCreateInfo GetImageCreateInfo(
    const VkFormat& image_format, const VkImageUsageFlags& usage,
    const VkExtent3D& extent, const VkImageCreateFlags& flags = 0);

VkImageCreateInfo GetImageCreateInfo(const VkFormat& image_format,
                                     const VkImageUsageFlags& usage,
                                     const VkExtent3D& extent,
                                     std::vector<std::uint32_t>& queue_index,
                                     const VkImageCreateFlags& flags = 0);

VmaAllocationCreateInfo GetVmaAllocationCreateInfo(
    const VmaMemoryUsage& usage = VMA_MEMORY_USAGE_AUTO,
    const VmaAllocationCreateFlags& flags = 0);

VkCommandBufferBeginInfo GetCommandBufferBeginInfo(
    const VkCommandBufferUsageFlags& flags = 0);

VkFenceCreateInfo GetFenceCreateInfo(const VkFenceCreateFlags& flags = 0);

VkFence GetVkFence(const VkDevice& device, const bool& is_signaled = false);

[[deprecated]] VkSubmitInfo GetCommandSubmitInfo(
    const VkCommandBuffer& command_buffer, const uint32_t& command_count = 1);

VkSubmitInfo GetCommandSubmitInfo(
    const std::vector<VkCommandBuffer>& command_buffers);

VkSubmitInfo GetCommandSubmitInfo(
    const std::vector<AllocatedCommandBuffer>& command_buffers);

ExecuteResult SubmitCommandBuffers(
    VkQueue queue, const std::vector<VkSubmitInfo>& submit_infos,
    VkFence fence);

/**
 * \brief Get \ref VkImageMemoryBarrier2, but performance is poor.
 * \param image The image that this barrier will affect.
 * \param transfer_mode The image transfer operation mode to be executed.
 * \return The \ref VkImageMemoryBarrier2
 * \remark Blockage at all stages, poor performance. \n
 * \remark sub_image.aspectMask = (transfer_mode ==
 * ImageTransferMode::INIT_TO_DEPTH_TEST) \n ? VK_IMAGE_ASPECT_DEPTH_BIT \n :
 * VK_IMAGE_ASPECT_COLOR_BIT; \n sub_image.baseMipLevel = 0; \n
 * sub_image.levelCount = 1; \n
 * sub_image.baseArrayLayer = 0; \n
 * sub_image.layerCount = 1;
 */
VkImageMemoryBarrier2 GetImageMemoryBarrier(
    AllocatedImage& image, const ImageTransferMode& transfer_mode);

/**
 * \brief Get \ref VkImageMemoryBarrier2, but performance is poor.
 * \param image image The image that this barrier will affect.
 * \param old_layout Old layout.
 * \param new_layout New layout.
 * \return The \ref VkImageMemoryBarrier2
 * \remark Blockage at all stages, poor performance.
 */
[[deprecated]] VkImageMemoryBarrier2 GetImageMemoryBarrier(
    AllocatedImage& image, const VkImageLayout& old_layout,
    const VkImageLayout& new_layout);

[[deprecated]] VkDependencyInfo GetImageDependencyInfo(
    VkImageMemoryBarrier2& image_barrier, const VkDependencyFlags& flags = 0);

VkMemoryBarrier2 GetMemoryBarrier(VkPipelineStageFlags2 src_stage,
                                  VkAccessFlags2 src_access,
                                  VkPipelineStageFlags2 dest_stage,
                                  VkAccessFlags2 dest_access);

VkBufferMemoryBarrier2 GetBufferMemoryBarrier(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    const AllocatedBuffer& buffer, VkDeviceSize offset, VkDeviceSize size);
// TODO stage buffer memory barrier
// VkBufferMemoryBarrier2 GetBufferMemoryBarrier(
//    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
//    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
//    std::uint32_t src_queue_family_index, std::uint32_t
//    dest_queue_family_index, const AllocatedStageBuffer& allocated_buffer,
//    VkDeviceSize offset, VkDeviceSize size);

VkImageMemoryBarrier2 GetImageMemoryBarrier(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    VkImageLayout old_layout, VkImageLayout new_layout,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    const AllocatedImage& image,
    const VkImageSubresourceRange& sub_resource_range);

VkImageMemoryBarrier2 GetImageMemoryBarrier(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    VkImageLayout old_layout, VkImageLayout new_layout,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    const AllocatedImage& image, VkImageAspectFlags aspect_mask,
    std::uint32_t base_mipmap_level, std::uint32_t level_count,
    std::uint32_t base_array_layer, std::uint32_t layer_count);

VkDependencyInfo GetMemoryDependencyInfo(
    const std::vector<VkMemoryBarrier2>& memory_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetMemoryDependencyInfo(
    const VkMemoryBarrier2& memory_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetBufferMemoryDependencyInfo(
    const std::vector<VkBufferMemoryBarrier2>& buffer_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetBufferMemoryDependencyInfo(
    const VkBufferMemoryBarrier2& buffer_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetImageMemoryDependencyInfo(
    const std::vector<VkImageMemoryBarrier2>& image_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetImageMemoryDependencyInfo(
    const VkImageMemoryBarrier2& image_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetDependencyInfo(
    const std::vector<VkMemoryBarrier2>& memory_barriers,
    const std::vector<VkBufferMemoryBarrier2>& buffer_barriers,
    const std::vector<VkImageMemoryBarrier2>& image_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetBufferQueueFamilyOwnershipTransformDependencyInfo(
    AllocatedBuffer& buffer, std::uint32_t dest_queue_family_index);

// TODO stage buffer memory ownership transform
// VkDependencyInfo GetBufferQueueFamilyOwnershipTransformDependencyInfo(
//    AllocatedStageBuffer& buffer, std::uint32_t dest_queue_family_index);

/**
 * \remark Blockage at all stages, poor performance.
 */
[[deprecated]] void AddTransferImageCommands(
    VkCommandBuffer& command_buffer, AllocatedImage& image,
    const ImageTransferMode& transfer_mode, const VkDependencyFlags& flags = 0);

[[deprecated]] void AddTransferImageCommands(
    VkCommandBuffer& command_buffer, AllocatedImage& image,
    const VkImageLayout& old_layout, const VkImageLayout& new_layout,
    const VkDependencyFlags& flags = 0);

[[deprecated]] void AddTransferImageCommands(
    AllocatedCommandBuffer& command_buffer, AllocatedImage& image,
    const ImageTransferMode& transfer_mode, const VkDependencyFlags& flags = 0);

[[deprecated]] void AddTransferImageCommands(
    AllocatedCommandBuffer& command_buffer, AllocatedImage& image,
    const VkImageLayout& old_layout, const VkImageLayout& new_layout,
    const VkDependencyFlags& flags = 0);

VkBufferImageCopy GetBufferToImageCopyRegion(
    const VkImageAspectFlagBits& aspect, const VkExtent3D& image_extent,
    const VkDeviceSize& buffer_offset = 0,
    const VkOffset3D& image_offset = VkOffset3D{0, 0, 0});

bool DescriptorTypeIsImage(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsImageSampler(const VkDescriptorType& descriptor_type);

VkImageViewCreateInfo GetImageViewCreateInfo(
    const AllocatedImage& image, const VkFormat& format,
    const VkImageViewType& view_type, const VkImageAspectFlags& aspect_flags,
    const VkImageViewCreateFlags& flags = 0);
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
VkSamplerCreateInfo GetSamplerCreateInfo(
    const VkPhysicalDevice& physical_device);

VkSemaphoreCreateInfo GetSemaphoreCreateInfo(
    const VkSemaphoreCreateFlags& flags = 0);

bool DescriptorTypeIsDynamicBuffer(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsBuffer(const VkDescriptorType& descriptor_type);

/**
 * \brief Determine whether the buffer/image can be mapped (accessible to the
 * CPU) based on \ref memory_usage and \ref allocation_flags.
 */
bool CanBeMapped(const VmaMemoryUsage& memory_usage,
                 const VmaAllocationCreateFlags& allocation_flags);

VkDeviceSize GetMinAlignmentSizeFromOriginalSize(
    const VkDeviceSize& min_alignment, const VkDeviceSize& original_size);

bool DescriptorTypeIsStorageBuffer(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsStorageImage(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsUniformBuffer(const VkDescriptorType& descriptor_type);

bool DescriptorTypeIsTexelBuffer(const VkDescriptorType& descriptor_type);

AllocatedBuffer CreateBuffer(
    const RenderEngine* engine, const size_t& alloc_size,
    const VkBufferUsageFlags& usage, const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags = 0,
    const bool& is_BDA_buffer = true);

VkBufferCopy2 GetBufferCopy(const VkDeviceSize& size,
                            const VkDeviceSize& src_offset = 0,
                            const VkDeviceSize& dest_offset = 0);

VkCopyBufferInfo2 GetCopyBufferInfo(AllocatedBuffer& src_buffer,
                                    AllocatedBuffer& dest_buffer,
                                    const std::vector<VkBufferCopy2>& regions);

VkCopyBufferInfo2 GetCopyBufferInfo(const AllocatedBuffer& src_buffer,
                                    AllocatedBuffer& dest_buffer,
                                    const std::vector<VkBufferCopy2>& regions);

bool IsTransformSrcBuffer(const VkBufferUsageFlags& flags);

bool IsTransformDestBuffer(const VkBufferUsageFlags& flags);

bool IsTransformSrcImage(const VkImageUsageFlags& flags);

bool IsTransformDestImage(const VkImageUsageFlags& flags);

bool ResourceImageCanWrite(const VkDescriptorType& descriptor_type);

bool ResourceBufferCanWrite(const VkDescriptorType& descriptor_type);

bool GetImageUsageFromDescriptorType(const VkDescriptorType& descriptor_type,
                                     VkImageUsageFlags& output_image_usage);

bool GetBufferUsageFromDescriptorType(const VkDescriptorType& descriptor_type,
                                      VkImageUsageFlags& output_buffer_usage);

bool ImageRegionIsOverlap(const VkOffset3D& src_offset,
                          const VkOffset3D& dest_offset,
                          const VkExtent3D& extent);

bool ImageRegionAreaLessThanImageExtent(const VkOffset3D& offset,
                                        const VkExtent3D& extent,
                                        const AllocatedImage& image);

VkImageSubresourceLayers GetImageSubResourceLayers(
    const VkImageAspectFlags& aspect, const std::uint32_t& mipmap_level,
    const std::uint32_t& base_array_layer,
    const std::uint32_t& array_layer_count);

VkImageCopy2 GetImageCopy(const VkImageSubresourceLayers& src_sub_resource,
                          const VkImageSubresourceLayers& dest_sub_resource,
                          const VkOffset3D& src_offset,
                          const VkOffset3D& dest_offset,
                          const VkExtent3D& extent);

VkCopyImageInfo2 GetCopyImageInfo(
    AllocatedImage& src_image, AllocatedImage& dest_image,
    const VkImageLayout& src_layout, const VkImageLayout& dest_layout,
    const std::vector<VkImageCopy2>& copy_regions);

VkCopyImageInfo2 GetCopyImageInfo(
    const AllocatedImage& src_image, AllocatedImage& dest_image,
    const VkImageLayout& src_layout, const VkImageLayout& dest_layout,
    const std::vector<VkImageCopy2>& copy_regions);

ExecuteResult GetEndSizeAndOffset(
    const AllocatedBuffer& buffer,
    std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
    VkDeviceSize& output_end_size, VkDeviceSize& output_offset);

VkCommandBufferBeginInfo GetCommandBufferBeginInfo(
    VkCommandBufferUsageFlags flags = 0,
    const VkCommandBufferInheritanceInfo* inheritance_info = nullptr);

ExecuteResult BeginCommandBuffer(
    AllocatedCommandBuffer& command_buffer, VkCommandBufferUsageFlags flags = 0,
    const VkCommandBufferInheritanceInfo* inheritance_info = nullptr);

ExecuteResult EndCommandBuffer(AllocatedCommandBuffer& command_buffer);
}  // namespace Utils
}  // namespace RenderSystem
}  // namespace MM
