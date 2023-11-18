#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

#include "runtime/function/render/pre_header.h"
#include "runtime/function/render/vk_enum.h"

namespace MM {
namespace RenderSystem {
class AllocatedImage;
class AllocatedBuffer;
class AllocatedMeshBuffer;
class AllocatedCommandBuffer;
class RenderEngine;
class BufferChunkInfo;

ErrorCode VkResultToMMErrorCode(VkResult vk_result);

// TODO Add default value annotations for various functions that obtain creation
// information, such as the \ref GetSamplerCreateInfo function.
VkCommandPoolCreateInfo GetCommandPoolCreateInfo(
    const uint32_t& queue_family_index,
    const VkCommandPoolCreateFlags& flags = 0);

VkCommandBufferAllocateInfo GetCommandBufferAllocateInfo(
    const VkCommandPool& command_pool, const uint32_t& count = 1,
    const VkCommandBufferLevel& level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkBufferCreateInfo GetVkBufferCreateInfo(
    const void* next, VkBufferCreateFlags flags, VkDeviceSize size,
    VkBufferUsageFlags usage, VkSharingMode sharing_mode,
    std::uint32_t queue_family_index_count,
    const std::uint32_t* queue_family_indices);

VmaAllocationCreateInfo GetVmaAllocationCreateInfo(
    VmaAllocatorCreateFlags flags, VmaMemoryUsage usage,
    VkMemoryPropertyFlags required_flags, VkMemoryPropertyFlags preferred_flags,
    std::uint32_t memory_type_bits, VmaPool pool, void* user_data,
    float priority);

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

Result<Nil> SubmitCommandBuffers(
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
VkImageMemoryBarrier2 GetVkImageMemoryBarrier2(
    AllocatedImage& image,
    const ImageTransferMode& transfer_mode);

VkImageMemoryBarrier2 GetVkImageMemoryBarrier2(
    VkPipelineStageFlags2 src_stage_mask, VkAccessFlags2 src_access_mask,
    VkPipelineStageFlags2 dst_stage_mask, VkAccessFlags2 dst_access_mask,
    VkImageLayout old_layout, VkImageLayout new_layout,
    uint32_t src_queue_family_index, uint32_t dst_queue_family_index,
    VkImage image, VkImageSubresourceRange subresource_range);

/**
 * \brief Get \ref VkImageMemoryBarrier2, but performance is poor.
 * \param image image The image that this barrier will affect.
 * \param old_layout Old layout.
 * \param new_layout New layout.
 * \return The \ref VkImageMemoryBarrier2
 * \remark Blockage at all stages, poor performance.
 */
[[deprecated]] VkImageMemoryBarrier2 GetVkImageMemoryBarrier2(
    AllocatedImage& image, const VkImageLayout& old_layout,
    const VkImageLayout& new_layout);

[[deprecated]] VkDependencyInfo GetVkImageDependencyInfo(
    const VkImageMemoryBarrier2& image_barrier, const VkDependencyFlags& flags = 0);

VkMemoryBarrier2 GetVkMemoryBarrier2(VkPipelineStageFlags2 src_stage,
                                     VkAccessFlags2 src_access,
                                     VkPipelineStageFlags2 dest_stage,
                                     VkAccessFlags2 dest_access);

VkBufferMemoryBarrier2 GetVkBufferMemoryBarrier2(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    const AllocatedBuffer& buffer, VkDeviceSize offset, VkDeviceSize size);

VkBufferMemoryBarrier2 GetVkBufferMemoryBarrier2(
    VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
    VkPipelineStageFlags2 dest_stage, VkAccessFlags2 dest_access,
    std::uint32_t src_queue_family_index, std::uint32_t dest_queue_family_index,
    VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size);

VkImageMemoryBarrier2 GetVkImageMemoryBarrier2(
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

VkDependencyInfo GetVkDependencyInfo(
    const std::vector<VkMemoryBarrier2>* memory_barriers,
    const std::vector<VkBufferMemoryBarrier2>* buffer_barriers,
    const std::vector<VkImageMemoryBarrier2>* image_barriers,
    VkDependencyFlags dependency_flags = 0);

VkDependencyInfo GetVkDependencyInfo(
    std::uint32_t memory_barriers_count,
    const VkMemoryBarrier2* memory_barriers,
    std::uint32_t buffer_barriers_count,
    const VkBufferMemoryBarrier2* buffer_barriers,
    std::uint32_t image_barriers_count,
    const VkImageMemoryBarrier2* image_barriers,
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

VkBufferImageCopy2 GetVkBufferImageCopy2(
    const void* next, VkDeviceSize buffer_offset, uint32_t buffer_row_length,
    uint32_t buffer_image_height, VkImageSubresourceLayers image_sub_resource,
    VkOffset3D image_offset, VkExtent3D image_extent);

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

Result<AllocatedBuffer> CreateBuffer(
    RenderEngine* render_engine, const std::string& object_name,
    const VkBufferCreateInfo& vk_buffer_create_info,
    const VmaAllocationCreateInfo& vma_allocation_create_info,
    VmaAllocationInfo* vma_allocation_info);

VkBufferCopy2 GetVkBufferCopy2(const VkDeviceSize& size,
                               const VkDeviceSize& src_offset = 0,
                               const VkDeviceSize& dest_offset = 0);

VkCopyBufferInfo2 GetVkCopyBufferInfo2(
    AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<VkBufferCopy2>& regions);

VkCopyBufferInfo2 GetVkCopyBufferInfo2(
    const AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<VkBufferCopy2>& regions);

VkCopyBufferInfo2 GetVkCopyBufferInfo2(AllocatedBuffer& src_buffer,
                                       AllocatedBuffer& dest_buffer,
                                       std::uint32_t regions_count,
                                       const VkBufferCopy2* regions);

VkCopyBufferInfo2 GetVkCopyBufferInfo2(const AllocatedBuffer& src_buffer,
                                       AllocatedBuffer& dest_buffer,
                                       std::uint32_t regions_count,
                                       const VkBufferCopy2* regions);

VkCopyBufferInfo2 GetVkCopyBufferInfo2(void* next, VkBuffer src_buffer,
                                       VkBuffer dest_buffer,
                                       std::uint32_t regions_count,
                                       const VkBufferCopy2* regions);

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

VkImageSubresourceLayers GetVkImageSubResourceLayers(
    const VkImageAspectFlags& aspect, const std::uint32_t& mipmap_level,
    const std::uint32_t& base_array_layer,
    const std::uint32_t& array_layer_count);

VkImageCopy2 GetVkImageCopy2(const VkImageSubresourceLayers& src_sub_resource,
                             const VkImageSubresourceLayers& dest_sub_resource,
                             const VkOffset3D& src_offset,
                             const VkOffset3D& dest_offset,
                             const VkExtent3D& extent);

VkCopyImageInfo2 GetVkCopyImageInfo2(
    AllocatedImage& src_image, AllocatedImage& dest_image,
    const VkImageLayout& src_layout, const VkImageLayout& dest_layout,
    const std::vector<VkImageCopy2>& copy_regions);

VkCopyImageInfo2 GetVkCopyImageInfo2(
    const AllocatedImage& src_image, AllocatedImage& dest_image,
    const VkImageLayout& src_layout, const VkImageLayout& dest_layout,
    const std::vector<VkImageCopy2>& copy_regions);

VkCopyImageInfo2 GetVkCopyImageInfo2(VkImage src_image, VkImage dest_image,
                                     void* next, VkImageLayout src_layout,
                                     VkImageLayout dest_layout,
                                     uint32_t regions_count,
                                     const VkImageCopy2* copy_regions);

Result<std::pair<VkDeviceSize, VkDeviceSize>> GetEndSizeAndOffset(
    const AllocatedBuffer& buffer,
    std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info);

VkCommandBufferBeginInfo GetCommandBufferBeginInfo(
    VkCommandBufferUsageFlags flags = 0,
    const VkCommandBufferInheritanceInfo* inheritance_info = nullptr);

Result<Nil> BeginCommandBuffer(
    AllocatedCommandBuffer& command_buffer, VkCommandBufferUsageFlags flags = 0,
    const VkCommandBufferInheritanceInfo* inheritance_info = nullptr);

Result<Nil> EndCommandBuffer(
    AllocatedCommandBuffer& command_buffer);

std::uint64_t ConvertVkFormatToContinuousValue(VkFormat vk_format);

std::uint64_t ConvertVkImageLayoutToContinuousValue(
    VkImageLayout vk_image_layout);

Result<Nil> CheckVkImageCreateInfo(
    const VkImageCreateInfo* vk_image_create_info);

Result<Nil> CheckVmaAllocationCreateInfo(
    const VmaAllocationCreateInfo* vma_allocation_create_info);

Result<Nil> CheckVkBufferCreateInfo(
    const VkBufferCreateInfo* vk_buffer_create_info);

bool ImageLayoutSupportDepthTest(VkImageLayout vk_image_layout);

bool ImageLayoutSupportStencilTest(VkImageLayout vk_image_layout);

VkImageAspectFlags ChooseImageAspectFlags(VkImageLayout vk_image_layout);

VkImageSubresourceRange GetVkImageSubresourceRange(
    VkImageAspectFlags vk_image_aspect_flags, std::uint32_t base_mipmap_level,
    std::uint32_t mipmap_level_count, std::uint32_t base_array_level,
    std::uint32_t array_count);

VkCopyBufferToImageInfo2 GetVkCopyBufferToImageInfo2(
    const void* next, AllocatedBuffer& src_buffer, AllocatedImage& dest_image,
    VkImageLayout dest_image_layout, uint32_t region_count,
    const VkBufferImageCopy2* regions);

bool ImageUseToSampler(VkImageUsageFlags vk_image_usage_flags);

VkImageBlit2 GetImageBlit2(const void* next,
                           VkImageSubresourceLayers src_sub_resource,
                           VkOffset3D src_offsets[2],
                           VkImageSubresourceLayers dest_sub_resource,
                           VkOffset3D dest_offsets[2]);

CommandBufferType ChooseCommandBufferType(RenderEngine* render_engine,
                                          std::uint32_t queue_index);

VkSubmitInfo GetVkSubmitInfo(void* next, uint32_t wait_semaphore_info_count,
                             const VkSemaphore* wait_semaphore_infos,
                             const VkPipelineStageFlags* wait_dst_stage_mask,
                             uint32_t command_buffer_info_count,
                             const VkCommandBuffer* command_buffer,
                             uint32_t signal_semaphore_info_count,
                             const VkSemaphore* signal_semaphore_infos);

bool IsValidPipelineCacheData(const std::string& filename, const char* buffer,
                              uint32_t size,
                              const VkPhysicalDeviceProperties& gpu_properties);

VkImageCreateInfo GetVkImageCreateInfo(
    const void* next, VkImageCreateFlags flags, VkImageType image_type,
    VkFormat format, VkExtent3D extent, uint32_t mipmap_levels,
    uint32_t array_layers, VkSampleCountFlags samples, VkImageTiling tiling,
    VkImageUsageFlags usage, VkSharingMode sharing_mode,
    uint32_t queue_family_index_count, const uint32_t* queue_family_indices,
    VkImageLayout initial_layout);

std::uint64_t GetVkFormatSize(VkFormat vk_format);

VkFormat GetVkFormatFromImageFormat(
    AssetSystem::AssetType::ImageFormat image_format);

bool LayoutSupportImageSamplerCombine(VkImageLayout layout);

DynamicState ConvertVkDynamicStateToDynamicState(
    VkDynamicState vk_dynamic_state);

Result<Nil> SavePiplineCache(VkDevice device,
                                          VkPipelineCache pipeline_cache);

bool VkVertexInputBindingDescriptionIsEqual(
    const VkVertexInputBindingDescription& lhs,
    const VkVertexInputBindingDescription& rhs);

bool VkVertexInputAttributeDescriptionIsEqual(
    const VkVertexInputAttributeDescription& lhs,
    const VkVertexInputAttributeDescription& rhs);

bool VkViewportIsEqual(const VkViewport& lhs, const VkViewport& rhs);

bool VkRect2DIsEqual(const VkRect2D& lhs, const VkRect2D& rhs);

bool VkPipelineViewportStateCreateInfoIsEqual(
    const VkPipelineViewportStateCreateInfo& lhs,
    const VkPipelineViewportStateCreateInfo& rhs);

Result<Nil> ConvertVkResultToMMResult(VkResult vk_result);
}  // namespace RenderSystem
}  // namespace MM
