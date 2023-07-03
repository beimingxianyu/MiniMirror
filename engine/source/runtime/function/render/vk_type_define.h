//
// Created by beimingxianyu on 23-6-28.
//

#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "runtime/function/render/RenderDataAttributeID.h"
#include "runtime/function/render/import_other_system.h"
#include "runtime/platform/base/error.h"
#include "utils/error.h"
#include "utils/hash_table.h"
#include "utils/marco.h"
#include "vk_utils.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

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

struct AllocationCreateInfo {
  AllocationCreateInfo() = default;
  ~AllocationCreateInfo() = default;
  AllocationCreateInfo(VmaAllocationCreateFlags flags, VmaMemoryUsage usage,
                       VkMemoryPropertyFlags required_flags,
                       VkMemoryPropertyFlags preferred_flags,
                       uint32_t memory_type_bits, float priority);
  explicit AllocationCreateInfo(
      const VmaAllocationCreateInfo& vma_allocation_create_info);
  AllocationCreateInfo(const AllocationCreateInfo& other) = default;
  AllocationCreateInfo(AllocationCreateInfo&& other) noexcept;
  AllocationCreateInfo& operator=(const AllocationCreateInfo& other) = default;
  AllocationCreateInfo& operator=(AllocationCreateInfo&& other) noexcept;

  VmaAllocationCreateFlags flags_{VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM};
  VmaMemoryUsage usage_{VMA_MEMORY_USAGE_MAX_ENUM};
  VkMemoryPropertyFlags required_flags_{VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM};
  VkMemoryPropertyFlags preferred_flags_{VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM};
  std::uint32_t memory_type_bits_{UINT32_MAX};
  float priority_{0};

  bool IsValid() const;

  VmaAllocationCreateInfo GetVmaAllocationCreateInfo() const;

  void Reset();
};

struct ImageCreateInfo {
  ImageCreateInfo() = default;
  ~ImageCreateInfo() = default;
  ImageCreateInfo(uint64_t image_size, const void* next,
                  VkImageCreateFlags flags, VkImageType image_type,
                  VkFormat format, const VkExtent3D& extent, uint32_t miplevels,
                  uint32_t array_levels, VkSampleCountFlags samples,
                  VkImageTiling tiling, VkImageUsageFlags usage,
                  VkSharingMode sharing_mode,
                  const std::vector<std::uint32_t>& queue_family_indices,
                  VkImageLayout initial_layout);
  ImageCreateInfo(std::uint64_t image_size,
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
  VkSampleCountFlags samples_{VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM};
  VkImageTiling tiling_{VK_IMAGE_TILING_MAX_ENUM};
  VkImageUsageFlags usage_{VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM};
  VkSharingMode sharing_mode_{VK_SHARING_MODE_EXCLUSIVE};
  std::vector<std::uint32_t> queue_family_indices_{};
  VkImageLayout initial_layout_{VK_IMAGE_LAYOUT_MAX_ENUM};

  bool IsValid() const;

  VkImageCreateInfo GetVkImageCreateInfo() const;

  void Reset();
};

struct ImageDataInfo {
  ImageDataInfo() = default;
  ~ImageDataInfo() = default;
  ImageDataInfo(const ImageDataInfo& other) = default;
  ImageDataInfo(ImageDataInfo&& other) noexcept;
  ImageDataInfo& operator=(const ImageDataInfo& other);
  ImageDataInfo& operator=(ImageDataInfo&& other) noexcept;

  ImageCreateInfo image_create_info_{};
  AllocationCreateInfo allocation_create_info_{};

  std::uint32_t queue_index_{UINT32_MAX};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_MAX_ENUM};

  ExecuteResult GetRenderDataAttributeID(
      RenderImageDataAttributeID render_image_data_attribute_ID) const;

  void SetImageCreateInfo(std::uint64_t image_size,
                          const VkImageCreateInfo& vk_image_create_info);

  void SetAllocationCreateInfo(
      const VmaAllocationCreateInfo& vma_allocation_create_info);

  bool IsValid() const;

  void Reset();
};

struct BufferCreateInfo {
  BufferCreateInfo() = default;
  ~BufferCreateInfo() = default;
  BufferCreateInfo(const void* next, VkBufferCreateFlags flags,
                   VkDeviceSize size, VkBufferUsageFlags usage,
                   VkSharingMode sharing_mode,
                   const std::vector<std::uint32_t>& queue_family_indices);
  explicit BufferCreateInfo(const VkBufferCreateInfo& vk_buffer_create_info);
  BufferCreateInfo(const BufferCreateInfo& other) = default;
  BufferCreateInfo(BufferCreateInfo&& other) noexcept;
  BufferCreateInfo& operator=(const BufferCreateInfo& other);
  BufferCreateInfo& operator=(BufferCreateInfo&& other) noexcept;

  const void* next_;
  VkBufferCreateFlags flags_;
  VkDeviceSize size_;
  VkBufferUsageFlags usage_;
  VkSharingMode sharing_mode_;
  std::vector<std::uint32_t> queue_family_indices_;

  bool IsValid() const;

  VkBufferCreateInfo GetVkBufferCreateInfo() const;

  void Reset();
};

struct BufferDataInfo {
  BufferDataInfo() = default;
  ~BufferDataInfo() = default;
  BufferDataInfo(const BufferCreateInfo& buffer_create_info,
                 const AllocationCreateInfo& allocation_create_info);
  BufferDataInfo(const BufferDataInfo& other) = default;
  BufferDataInfo(BufferDataInfo&& other) noexcept;
  BufferDataInfo& operator=(const BufferDataInfo& other);
  BufferDataInfo& operator=(BufferDataInfo&& other) noexcept;

  BufferCreateInfo buffer_create_info_{};
  AllocationCreateInfo allocation_create_info_{};

  std::uint32_t queue_index_{UINT32_MAX};

  ExecuteResult GetRenderDataAttributeID(
      RenderImageDataAttributeID& render_image_data_attribute_ID) const;

  void SetBufferCreateInfo(const VkBufferCreateInfo& vk_buffer_create_info);

  void SetAllocationCreateInfo(
      const VmaAllocationCreateInfo& vma_allocation_create_info);

  bool IsValid() const;

  void Reset();
};

struct BufferBindInfo {
  BufferBindInfo() = default;
  ~BufferBindInfo() = default;
  BufferBindInfo(const VkDescriptorSetLayoutBinding& bind,
                 VkDeviceSize range_size, VkDeviceSize offset,
                 VkDeviceSize dynamic_offset);
  BufferBindInfo(const BufferBindInfo& other) = default;
  BufferBindInfo(BufferBindInfo&& other) noexcept;
  BufferBindInfo& operator=(const BufferBindInfo& other);
  BufferBindInfo& operator=(BufferBindInfo&& other) noexcept;

  // TODO replace to DescriptorSetLayoutBinding
  VkDescriptorSetLayoutBinding bind_{};
  VkDeviceSize range_size_{0};
  VkDeviceSize offset_{0};
  VkDeviceSize dynamic_offset_{0};

  void Reset();

  bool IsValid() const;
};

struct DescriptorSetLayoutBinding {
  DescriptorSetLayoutBinding() = default;
  ~DescriptorSetLayoutBinding() = default;
  DescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptor_type,
                             uint32_t descriptor_count,
                             VkShaderStageFlags stage_flags,
                             VkSampler const* immutable_samplers);
  explicit DescriptorSetLayoutBinding(
      const VkDescriptorSetLayoutBinding& vk_descriptor_set_layout_binding);
  DescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& other) = default;
  DescriptorSetLayoutBinding(DescriptorSetLayoutBinding&& other) noexcept;
  DescriptorSetLayoutBinding& operator=(
      const DescriptorSetLayoutBinding& other) = default;
  DescriptorSetLayoutBinding& operator=(
      DescriptorSetLayoutBinding&& other) noexcept;

  uint32_t binding_{0};
  VkDescriptorType descriptor_type_{VK_DESCRIPTOR_TYPE_MAX_ENUM};
  uint32_t descriptor_count_{0};
  VkShaderStageFlags stage_flags_{VK_SHADER_STAGE_ALL};
  const VkSampler* Immutable_samplers_{nullptr};

  bool IsValid() const;

  VkDescriptorSetLayoutBinding GetVkDescriptorSetLayoutBinding() const;

  void Reset();
};

struct ImageViewCreateInfo {
  ImageViewCreateInfo() = default;
  ~ImageViewCreateInfo() = default;
  ImageViewCreateInfo(const void* next, VkImageCreateFlags flags, VkImage image,
                      VkImageViewType view_type, VkFormat format,
                      const VkComponentMapping& components,
                      const VkImageSubresourceRange& subresource_range);
  explicit ImageViewCreateInfo(
      const VkImageViewCreateInfo& vk_image_create_info);
  ImageViewCreateInfo(const ImageViewCreateInfo& other) = default;
  ImageViewCreateInfo(ImageViewCreateInfo&& other) noexcept;
  ImageViewCreateInfo& operator=(const ImageViewCreateInfo& other) = default;
  ImageViewCreateInfo& operator=(ImageViewCreateInfo&& other) noexcept;

  const void* next_{nullptr};
  VkImageCreateFlags flags_{VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM};
  VkImage image_{nullptr};
  VkImageViewType view_type_{VK_IMAGE_VIEW_TYPE_MAX_ENUM};
  VkFormat format_{VK_FORMAT_MAX_ENUM};
  VkComponentMapping components_{
      VK_COMPONENT_SWIZZLE_MAX_ENUM,
      VK_COMPONENT_SWIZZLE_MAX_ENUM,
      VK_COMPONENT_SWIZZLE_MAX_ENUM,
      VK_COMPONENT_SWIZZLE_MAX_ENUM,
  };
  VkImageSubresourceRange subresource_range_{VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM,
                                             0, 0, 0, 0};

  bool IsValid() const;

  ExecuteResult GetRenderImageViewAttributeID(
      RenderImageViewAttributeID& render_image_view_attribute_ID) const;

  VkImageViewCreateInfo GetVkImageViewCreateInfo() const;

  void Reset();
};

struct SamplerCreateInfo {
  SamplerCreateInfo() = default;
  ~SamplerCreateInfo() = default;
  SamplerCreateInfo(const void* next, VkSamplerCreateFlags flags,
                    VkFilter mag_filter, VkFilter min_filter,
                    VkSamplerMipmapMode mipmap_mode,
                    VkSamplerAddressMode address_mode_u,
                    VkSamplerAddressMode address_mode_v,
                    VkSamplerAddressMode address_mode_w, float mip_lod_bias,
                    VkBool32 anisotropy_enable, float max_anisotropy,
                    VkBool32 compare_enable, VkCompareOp compare_op,
                    float min_lod, float max_lod, VkBorderColor border_color,
                    VkBool32 unnormalized_coordinates);
  explicit SamplerCreateInfo(const VkSamplerCreateInfo& vk_sampler_create_info);
  SamplerCreateInfo(const SamplerCreateInfo& other) = default;
  SamplerCreateInfo(SamplerCreateInfo&& other) noexcept;
  SamplerCreateInfo& operator=(const SamplerCreateInfo& other) = default;
  SamplerCreateInfo& operator=(SamplerCreateInfo&& other) noexcept;

  const void* next_{nullptr};
  VkSamplerCreateFlags flags_{VK_SAMPLER_CREATE_FLAG_BITS_MAX_ENUM};
  VkFilter mag_filter_{VK_FILTER_MAX_ENUM};
  VkFilter min_filter_{VK_FILTER_MAX_ENUM};
  VkSamplerMipmapMode mipmap_mode_{VK_SAMPLER_MIPMAP_MODE_MAX_ENUM};
  VkSamplerAddressMode address_mode_u_{VK_SAMPLER_ADDRESS_MODE_MAX_ENUM};
  VkSamplerAddressMode address_mode_v_{VK_SAMPLER_ADDRESS_MODE_MAX_ENUM};
  VkSamplerAddressMode address_mode_w_{VK_SAMPLER_ADDRESS_MODE_MAX_ENUM};
  float mip_lod_bias_{0};
  VkBool32 anisotropy_enable_{false};
  float max_anisotropy_{0};
  VkBool32 compare_enable_{false};
  VkCompareOp compare_op_{VK_COMPARE_OP_MAX_ENUM};
  float min_lod_{0};
  float max_lod_{0};
  VkBorderColor border_color_{VK_BORDER_COLOR_MAX_ENUM};
  VkBool32 unnormalized_coordinates_{false};

  bool IsValid() const;

  ExecuteResult GetRenderSamplerAttributeID(
      RenderSamplerAttributeID& render_sampler_attribute_ID) const;

  VkSamplerCreateInfo GetVkSamplerCreateInfo() const;

  void Reset();
};

class ImageView {
  friend class RenderEngine;

 public:
  ImageView() = default;
  ~ImageView() = default;
  ImageView(VkDevice device, VkAllocationCallbacks* allocator,
            const VkImageViewCreateInfo& vk_image_view_create_info);
  ImageView(const ImageView& other) = delete;
  ImageView(ImageView&& other) noexcept;
  ImageView& operator=(const ImageView& other) = delete;
  ImageView& operator=(ImageView&& other) noexcept;

 public:
  VkDevice GetDevice();

  const VkDevice_T* GetDevice() const;

  VkAllocationCallbacks* GetAllocator();

  const VkAllocationCallbacks* GetAllocator() const;

  VkImageView GetVkImageView();

  const VkImageView_T* GetVkImageView() const;

  const ImageViewCreateInfo& GetImageViewCreateInfo() const;

  bool IsValid() const;

  void Reset();

 private:
  static ExecuteResult CheckInitParameters(
      VkDevice device, const VkImageViewCreateInfo& vk_image_view_create_info);

 private:
  struct ImageViewWrapper {
    ImageViewWrapper() = default;
    ~ImageViewWrapper() { Release(); }
    ImageViewWrapper(VkDevice device, VkAllocationCallbacks* allocator,
                     VkImageView image_view);
    ImageViewWrapper(const ImageViewWrapper& other) = default;
    ImageViewWrapper(ImageViewWrapper&& other) noexcept = default;
    ImageViewWrapper& operator=(const ImageViewWrapper& other) = default;
    ImageViewWrapper& operator=(ImageViewWrapper&& other) noexcept = default;

    VkDevice device_{nullptr};
    VkAllocationCallbacks* allocator_{nullptr};
    VkImageView image_view_{nullptr};

    bool IsValid() const;

    void Release();
  };

 private:
  ImageViewCreateInfo image_view_create_info_{};
  ImageViewWrapper* image_view_wrapper_{nullptr};

  // TODO render_engine recovery
  static MM::Utils::ConcurrentMap<RenderImageViewAttributeID, ImageViewWrapper>
      image_view_container_;
};

class Sampler {
  friend class RenderEngine;

 public:
  Sampler() = default;
  ~Sampler() = default;
  Sampler(VkDevice device, VkAllocationCallbacks* allocator,
          const VkSamplerCreateInfo& vk_sampler_create_info);
  Sampler(const Sampler& other) = delete;
  Sampler(Sampler&& other) noexcept;
  Sampler& operator=(const Sampler& other) = delete;
  Sampler& operator=(Sampler&& other) noexcept;

 public:
  VkDevice GetDevice();

  const VkDevice_T* GetDevice() const;

  VkAllocationCallbacks* GetAllocator();

  const VkAllocationCallbacks* GetAllocator() const;

  VkSampler GetVkSampler();

  const VkSampler_T* GetVkSampler() const;

  const SamplerCreateInfo& GetSamplerCreateInfo() const;

  bool IsValid() const;

  void Reset();

 private:
  static ExecuteResult CheckInitParameters(
      VkDevice device, const VkSamplerCreateInfo& vk_sampler_create_info);

 private:
  struct SamplerWrapper {
    SamplerWrapper() = default;
    ~SamplerWrapper() { Release(); }
    SamplerWrapper(VkDevice device, VkAllocationCallbacks* allocator,
                   VkSampler sampler);
    SamplerWrapper(const SamplerWrapper& other) = default;
    SamplerWrapper(SamplerWrapper&& other) noexcept = default;
    SamplerWrapper& operator=(const SamplerWrapper& other) = default;
    SamplerWrapper& operator=(SamplerWrapper&& other) noexcept = default;

    bool IsValid() const;

    void Release();

    VkDevice device_{nullptr};
    VkAllocationCallbacks* allocator_{nullptr};
    VkSampler sampler_{nullptr};
  };

 private:
  SamplerCreateInfo sampler_create_info_{};
  SamplerWrapper* sampler_wrapper_{nullptr};

  // TODO render_engine recovery
  static MM::Utils::ConcurrentMap<RenderSamplerAttributeID, SamplerWrapper>
      sampler_container_;
};

class ImageBindData {
 public:
  ImageBindData() = default;
  ~ImageBindData() = default;
  ImageBindData(const VkDescriptorSetLayoutBinding& bind, VkDevice device,
                VkAllocationCallbacks* image_view_allocator,
                const VkImageViewCreateInfo& vk_image_view_create_info,
                VkAllocationCallbacks* sampler_allocator,
                const VkSamplerCreateInfo& vk_sampler_create_info);
  ImageBindData(const DescriptorSetLayoutBinding& bind, ImageView&& image_view,
                Sampler&& sampler);
  ImageBindData(const ImageBindData& other) = delete;
  ImageBindData(ImageBindData&& other) noexcept;
  ImageBindData& operator=(const ImageBindData& other) = delete;
  ImageBindData& operator=(ImageBindData&& other) noexcept;

 public:
  const DescriptorSetLayoutBinding& GetDescriptorSetLayoutBinding() const;

  ImageView& GetImageView();

  const ImageView& GetImageView() const;

  Sampler& GetSampler();

  const Sampler& GetSampler() const;

  bool IsValid() const;

  void Reset();

 private:
  DescriptorSetLayoutBinding bind_{};
  ImageView image_view_{};
  Sampler sampler_{};
};
}  // namespace RenderSystem
}  // namespace MM
