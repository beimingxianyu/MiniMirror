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
#include "runtime/function/render/vk_enum.h"
#include "runtime/function/render/vk_utils.h"
#include "runtime/platform/base/error.h"
#include "utils/error.h"
#include "utils/hash_table.h"
#include "utils/marco.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

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

class AllocateSemaphore {
 public:
  explicit AllocateSemaphore(RenderEngine* engine,
                             VkSemaphoreCreateFlags flags = 0);
  ~AllocateSemaphore() = default;
  AllocateSemaphore(const AllocateSemaphore& other) = default;
  AllocateSemaphore(AllocateSemaphore&& other) noexcept = default;
  AllocateSemaphore& operator=(const AllocateSemaphore& other);
  AllocateSemaphore& operator=(AllocateSemaphore&& other) noexcept;

 public:
  VkSemaphore& GetSemaphore();
  const VkSemaphore& GetSemaphore() const;

  bool IsValid() const;

 private:
  class AllocateSemaphoreWrapper {
   public:
    AllocateSemaphoreWrapper() = default;
    AllocateSemaphoreWrapper(RenderEngine* engine, VkSemaphore semaphore);
    ~AllocateSemaphoreWrapper();
    AllocateSemaphoreWrapper(const AllocateSemaphoreWrapper& other) = delete;
    AllocateSemaphoreWrapper(AllocateSemaphoreWrapper&& other) noexcept;
    AllocateSemaphoreWrapper& operator=(const AllocateSemaphoreWrapper& other) =
        delete;
    AllocateSemaphoreWrapper& operator=(
        AllocateSemaphoreWrapper&& other) noexcept;

   public:
    VkSemaphore& GetSemaphore();
    const VkSemaphore& GetSemaphore() const;

    bool IsValid() const;

   private:
    RenderEngine* render_engine_{nullptr};
    VkSemaphore semaphore_{nullptr};
  };

 private:
  std::shared_ptr<AllocateSemaphoreWrapper> wrapper_{nullptr};
};

class AllocateFence {
 public:
  explicit AllocateFence(RenderEngine* engine, VkFenceCreateFlags flags = 0);
  ~AllocateFence() = default;
  AllocateFence(const AllocateFence& other) = default;
  AllocateFence(AllocateFence&& other) noexcept = default;
  AllocateFence& operator=(const AllocateFence& other);
  AllocateFence& operator=(AllocateFence&& other) noexcept;

 public:
  VkFence& GetFence();
  const VkFence& GetFence() const;

  bool IsValid() const;

 private:
  class AllocateFenceWrapper {
   public:
    AllocateFenceWrapper() = default;
    AllocateFenceWrapper(RenderEngine* engine, VkFence fence);
    ~AllocateFenceWrapper();
    AllocateFenceWrapper(const AllocateFenceWrapper& other) = delete;
    AllocateFenceWrapper(AllocateFenceWrapper&& other) noexcept;
    AllocateFenceWrapper& operator=(const AllocateFenceWrapper& other) = delete;
    AllocateFenceWrapper& operator=(AllocateFenceWrapper&& other) noexcept;

   public:
    VkFence& GetFence();
    const VkFence& GetFence() const;

    bool IsValid() const;

   private:
    RenderEngine* render_engine_{nullptr};
    VkFence fence_{nullptr};
  };

 private:
  std::shared_ptr<AllocateFenceWrapper> wrapper_{nullptr};
};

class BufferChunkInfo {
 public:
  BufferChunkInfo() = default;
  ~BufferChunkInfo() = default;
  BufferChunkInfo(const VkDeviceSize& start_offset, const VkDeviceSize& size);
  BufferChunkInfo(const BufferChunkInfo& other) = default;
  BufferChunkInfo(BufferChunkInfo&& other) noexcept;
  BufferChunkInfo& operator=(const BufferChunkInfo& other) noexcept;
  BufferChunkInfo& operator=(BufferChunkInfo&& other) noexcept;

 public:
  bool operator==(const BufferChunkInfo& rhs) const;

  bool operator!=(const BufferChunkInfo& rhs) const;

 public:
  VkDeviceSize GetOffset() const;

  VkDeviceSize GetSize() const;

  void SetOffset(const VkDeviceSize& new_offset);

  void SetSize(const VkDeviceSize& new_size);

  void Reset();

  bool IsValid() const;

  friend void Swap(BufferChunkInfo& lhs, BufferChunkInfo& rhs) noexcept;

  friend void swap(BufferChunkInfo& lhs, BufferChunkInfo& rhs) noexcept;

 private:
  VkDeviceSize offset_{0};
  VkDeviceSize size_{0};
};

class ImageChunkInfo {
 public:
  ImageChunkInfo() = default;
  ~ImageChunkInfo() = default;
  ImageChunkInfo(const VkOffset3D& offset, const VkExtent3D& extent);
  ImageChunkInfo(const ImageChunkInfo& other) = default;
  ImageChunkInfo(ImageChunkInfo&& other) noexcept;
  ImageChunkInfo& operator=(const ImageChunkInfo& other);
  ImageChunkInfo& operator=(ImageChunkInfo&& other) noexcept;

 public:
  const VkOffset3D& GetOffset() const;

  const VkExtent3D& GetExtent() const;

  void SetOffset(const VkOffset3D& new_offset);

  void SetExtent(const VkExtent3D& new_extent);

  void Reset();

  bool IsValid() const;

 private:
  VkOffset3D offset_{0, 0, 0};
  VkExtent3D extent_{0, 0, 0};
};

class ImageSubresourceRangeInfo {
 public:
  ImageSubresourceRangeInfo() = default;
  ~ImageSubresourceRangeInfo() = default;
  ImageSubresourceRangeInfo(uint32_t dest_mipmaps_level, uint32_t mipmaps_count,
                            uint32_t dest_array_level, uint32_t array_count);
  explicit ImageSubresourceRangeInfo(
      const VkImageSubresourceRange& vk_image_subresource_range);
  ImageSubresourceRangeInfo(const ImageSubresourceRangeInfo& other) = default;
  ImageSubresourceRangeInfo(ImageSubresourceRangeInfo&& other) noexcept;
  ImageSubresourceRangeInfo& operator=(const ImageSubresourceRangeInfo& other);
  ImageSubresourceRangeInfo& operator=(
      ImageSubresourceRangeInfo&& other) noexcept;

 public:
  std::uint32_t GetDestMipmapsLevel() const;

  void SetDestMipmapsLevel(std::uint32_t dest_mipmaps_level);

  std::uint32_t GetMipmapsCount() const;

  void SetMipmapsCount(std::uint32_t mipmaps_count);

  std::uint32_t GetDestArrayLevel() const;

  void SetDestArrayLevel(std::uint32_t dest_array_level);

  std::uint32_t GetArrayCount() const;

  void SetArrayCount(std::uint32_t array_count);

  void Reset();

  bool IsValid() const;

 private:
  std::uint32_t dest_mipmaps_level_{0};
  std::uint32_t mipmaps_count_{0};
  std::uint32_t dest_array_level_{0};
  std::uint32_t array_count_{0};
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
  AllocationCreateInfo& operator=(const AllocationCreateInfo& other);
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
  ImageCreateInfo(uint64_t image_size, VkImageLayout image_layout,
                  const void* next, VkImageCreateFlags flags,
                  VkImageType image_type, VkFormat format,
                  const VkExtent3D& extent, uint32_t miplevels,
                  uint32_t array_levels, VkSampleCountFlags samples,
                  VkImageTiling tiling, VkImageUsageFlags usage,
                  VkSharingMode sharing_mode,
                  const std::vector<std::uint32_t>& queue_family_indices,
                  VkImageLayout initial_layout);
  ImageCreateInfo(std::uint64_t image_size, VkImageLayout image_layout,
                  const VkImageCreateInfo& vk_image_create_info);
  ImageCreateInfo(const ImageCreateInfo& other) = default;
  ImageCreateInfo(ImageCreateInfo&& other) noexcept;
  ImageCreateInfo& operator=(const ImageCreateInfo& other);
  ImageCreateInfo& operator=(ImageCreateInfo&& other) noexcept;

  std::uint64_t image_size_{0};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_MAX_ENUM};
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

class ImageSubResourceAttribute {
 public:
  ImageSubResourceAttribute() = default;
  ~ImageSubResourceAttribute() = default;
  ImageSubResourceAttribute(
      const ImageSubresourceRangeInfo& image_subresource_range_info,
      uint32_t queue_index, VkImageLayout image_layout);
  ImageSubResourceAttribute(
      const VkImageSubresourceRange& vk_image_subresource_range,
      uint32_t queue_index, VkImageLayout image_layout);
  ImageSubResourceAttribute(const ImageSubResourceAttribute& other) = default;
  ImageSubResourceAttribute(ImageSubResourceAttribute&& other) noexcept;
  ImageSubResourceAttribute& operator=(const ImageSubResourceAttribute& other);
  ImageSubResourceAttribute& operator=(
      ImageSubResourceAttribute&& other) noexcept;
  ;

 public:
  const ImageSubresourceRangeInfo& GetImageSubresourceRangeInfo() const;

  void SetImageSubresourceRangeInfo(
      const ImageSubresourceRangeInfo& image_subresource_range_info);

  std::uint32_t GetQueueIndex() const;

  void SetQueueIndex(std::uint32_t queue_index);

  VkImageLayout GetImageLayout() const;

  void SetImageLayout(VkImageLayout image_layout);

  bool IsValid() const;

  void Reset();

 private:
  ImageSubresourceRangeInfo image_subresource_range_info_;
  std::uint32_t queue_index_{UINT32_MAX};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_MAX_ENUM};
};

struct ImageDataInfo {
  ImageDataInfo() = default;
  ~ImageDataInfo() = default;
  ImageDataInfo(const ImageCreateInfo& image_create_info,
                const AllocationCreateInfo& allocation_create_info);
  ImageDataInfo(VkDeviceSize size, VkImageLayout image_layout,
                const VkImageCreateInfo& vk_image_create_info,
                const VmaAllocationCreateInfo& vma_allocation_create_info);
  ImageDataInfo(const ImageDataInfo& other) = default;
  ImageDataInfo(ImageDataInfo&& other) noexcept = default;
  ImageDataInfo& operator=(const ImageDataInfo& other);
  ImageDataInfo& operator=(ImageDataInfo&& other) noexcept;

  ImageCreateInfo image_create_info_{};
  AllocationCreateInfo allocation_create_info_{};
  std::vector<ImageSubResourceAttribute> image_sub_resource_attributes_{};

  ExecuteResult GetRenderResourceDataAttributeID(
      RenderImageDataAttributeID render_image_data_attribute_ID) const;

  void SetImageCreateInfo(std::uint64_t image_size, VkImageLayout image_layout,
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

class BufferSubResourceAttribute {
 public:
  BufferSubResourceAttribute() = default;
  ~BufferSubResourceAttribute() = default;
  BufferSubResourceAttribute(const BufferChunkInfo& buffer_chunk_info,
                             std::uint32_t queue_index)
      : chunk_info_(buffer_chunk_info), queue_index_(queue_index) {}
  BufferSubResourceAttribute(VkDeviceSize offset, VkDeviceSize size,
                             std::uint32_t queue_index)
      : chunk_info_(offset, size), queue_index_(queue_index) {}
  BufferSubResourceAttribute(const BufferSubResourceAttribute& other) = default;
  BufferSubResourceAttribute(BufferSubResourceAttribute&& other) noexcept;
  BufferSubResourceAttribute& operator=(
      const BufferSubResourceAttribute& other);
  BufferSubResourceAttribute& operator=(
      BufferSubResourceAttribute&& other) noexcept;

 public:
  bool operator==(const BufferSubResourceAttribute& rhs) const;

  bool operator!=(const BufferSubResourceAttribute& rhs) const;

 public:
  const BufferChunkInfo& GetChunkInfo() const;

  void SetChunkInfo(const BufferChunkInfo& chunk_info);

  std::uint32_t GetQueueIndex() const;

  void SetQueueIndex(uint32_t queue_index);

 private:
  BufferChunkInfo chunk_info_{};

  std::uint32_t queue_index_{UINT32_MAX};
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

  std::vector<BufferSubResourceAttribute> buffer_sub_resource_attributes_{};

  ExecuteResult GetRenderResourceDataAttributeID(
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

class MeshBufferData {
 private:
  float capacity_coefficient_{0.9};
  float expansion_coefficient_{2};
  VkDeviceSize index_buffer_remaining_capacity_{0};
  VkDeviceSize vertex_buffer_remaining_capacity_{0};
};
}  // namespace RenderSystem
}  // namespace MM
