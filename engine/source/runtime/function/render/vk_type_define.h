//
// Created by beimingxianyu on 23-6-28.
//

#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace MM {
namespace RenderSystem {
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
  STAGE_BUFFER,
  UNDEFINED
};

/**
 * \brief Memory operations allowed for rendering resources.
 */
enum class MemoryOperate { READ, WRITE, READ_AND_WRITE, UNDEFINED };

enum class CommandBufferType { GRAPH, COMPUTE, TRANSFORM, UNDEFINED };

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family_;
  std::optional<uint32_t> transform_family_;
  std::optional<uint32_t> present_family_;
  std::optional<uint32_t> compute_family_;

  bool isComplete() const;
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities_{};
  std::vector<VkSurfaceFormatKHR> formats_{};
  std::vector<VkPresentModeKHR> presentModes_{};
};

struct SemaphoreDestructor {
  void operator()(VkSemaphore* value);

  RenderEngine* render_engine_;
};

struct RenderResourceManageInfo {
  bool use_to_write{false};
  bool is_shared_{true};
};

struct DataToBufferInfo {
  void* data_ = nullptr;
  VkDeviceSize copy_offset_ = 0;
  VkDeviceSize copy_size_ = 0;
  bool used_{false};
};

struct RenderEngineInfo {
  VkSampleCountFlagBits multi_sample_count_{VK_SAMPLE_COUNT_1_BIT};
};

struct ImageRange {
  VkImageSubresource range_;
};

struct ImageBindInfo {
  ImageBindInfo() = default;
  ~ImageBindInfo() = default;
  ImageBindInfo(const VkDescriptorSetLayoutBinding& bind,
                std::unique_ptr<VkImageView>&& image_view,
                std::unique_ptr<VkSampler>&& sampler);
  ImageBindInfo(const ImageBindInfo& other) = delete;
  ImageBindInfo(ImageBindInfo&& other) noexcept;
  ImageBindInfo& operator=(const ImageBindInfo& other) = delete;
  ImageBindInfo& operator=(ImageBindInfo&& other) noexcept;

  VkDescriptorSetLayoutBinding bind_{};
  std::unique_ptr<VkImageView> image_view_{nullptr};
  std::unique_ptr<VkSampler> sampler_{nullptr};

  bool IsValid() const;

  void Reset();
};

struct AllocationCreateInfo {
  VmaAllocationCreateFlags flags_;
  VmaMemoryUsage usage_;
  VkMemoryPropertyFlags required_flags_;
  VkMemoryPropertyFlags preferred_flags_;
  std::uint32_t memory_type_bits_;
  float priority_;

  bool IsValid() const;

  void Reset();
};

struct ImageCreateInfo {
  ImageCreateInfo() = default;
  ~ImageCreateInfo() = default;
  ImageCreateInfo(std::uint64_t image_size_,
                  const VkImageCreateInfo& vk_image_create_info);
  ImageCreateInfo(const ImageCreateInfo& other) = default;
  ImageCreateInfo(ImageCreateInfo&& other) noexcept;
  ImageCreateInfo& operator=(const ImageCreateInfo& other);
  ImageCreateInfo& operator=(ImageCreateInfo&& other) noexcept;

  std::uint64_t image_size_{0};
  const void* next_{nullptr};
  VkImageCreateFlags flags_{VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM};
  VkImageType image_type_{VK_IMAGE_TYPE_MAX_ENUM};
  VkFormat format_{VK_FORMAT_MAX_ENUM};
  VkExtent3D extent_{0, 0, 0};
  std::uint32_t miplevels_{0};
  std::uint32_t array_levels_{0};
  VkSampleCountFlagBits samples_{VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM};
  VkImageTiling tiling_{VK_IMAGE_TILING_MAX_ENUM};
  VkImageUsageFlags usage_{VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM};
  VkSharingMode sharing_mode_{VK_SHARING_MODE_EXCLUSIVE};
  std::vector<std::uint32_t> queue_family_indices_{};
  VkImageLayout initial_layout_{VK_IMAGE_LAYOUT_MAX_ENUM};

  bool IsValid() const;

  void Reset();
};

struct ImageDataInfo {
  ImageCreateInfo image_create_info_;
  AllocationCreateInfo allocation_create_info_;

  bool IsValid() const;

  void Reset();
};

}  // namespace RenderSystem
}  // namespace MM
