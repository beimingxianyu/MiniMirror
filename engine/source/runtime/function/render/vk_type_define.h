//
// Created by beimingxianyu on 23-6-28.
//

#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "runtime/function/render/RenderDataAttributeID.h"
#include "runtime/function/render/pre_header.h"
#include "runtime/function/render/vk_enum.h"
#include "runtime/function/render/vk_utils.h"
#include "utils/error.h"
#include "utils/hash_table.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

using QueueIndex = std::uint32_t;
using VertexIndex = std::uint32_t;
using AtomicRenderFutureState = std::atomic<RenderFutureState>;
using CommandTaskFlowExecutingState = RenderFutureState;
using AtomicCommandTaskExecutingState = std::atomic<CommandTaskExecutingState>;

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
  bool operator==(const ImageSubresourceRangeInfo& rhs) const;

  bool operator!=(const ImageSubresourceRangeInfo& rhs) const;

 public:
  std::uint32_t GetBaseMipmapsLevel() const;

  void SetBaseMipmapsLevel(std::uint32_t dest_mipmaps_level);

  std::uint32_t GetMipmapsCount() const;

  void SetMipmapsCount(std::uint32_t mipmaps_count);

  std::uint32_t GetBaseArrayLevel() const;

  void SetBaseArrayLevel(std::uint32_t dest_array_level);

  std::uint32_t GetArrayCount() const;

  void SetArrayCount(std::uint32_t array_count);

  void Reset();

  bool IsValid() const;

 private:
  std::uint32_t base_mipmaps_level_{0};
  std::uint32_t mipmaps_count_{0};
  std::uint32_t base_array_level_{0};
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
  friend struct ImageDataInfo;
  friend class AllocatedImage;

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

 private:
  const void* next_{nullptr};

 public:
  std::uint64_t image_size_{0};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_MAX_ENUM};
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

 public:
  bool operator==(const ImageSubResourceAttribute& rhs) const;

  bool operator!=(const ImageSubResourceAttribute& rhs) const;

 public:
  const ImageSubresourceRangeInfo& GetImageSubresourceRangeInfo() const;

  void SetImageSubresourceRangeInfo(
      const ImageSubresourceRangeInfo& image_subresource_range_info);

  std::uint32_t GetQueueIndex() const;

  void SetQueueIndex(std::uint32_t queue_index);

  VkImageLayout GetImageLayout() const;

  void SetImageLayout(VkImageLayout image_layout);

  std::uint32_t GetBaseMipmapLevel() const;

  std::uint32_t GetBaseArrayLevel() const;

  std::uint32_t GetMipmapCount() const;

  std::uint32_t GetArrayCount() const;

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

  Result<RenderImageDataAttributeID> GetRenderResourceDataAttributeID() const;

  void SetImageCreateInfo(std::uint64_t image_size, VkImageLayout image_layout,
                          const VkImageCreateInfo& vk_image_create_info);

  void SetAllocationCreateInfo(
      const VmaAllocationCreateInfo& vma_allocation_create_info);

  bool IsValid() const;

  void Reset();
};

struct BufferCreateInfo {
  friend struct BufferDataInfo;
  friend struct MeshBufferInfoBase;
  friend class AllocatedBuffer;

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

 private:
  const void* next_;

 public:
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

  VkDeviceSize GetOffset() const;

  VkDeviceSize GetSize() const;

  void SetOffset(VkDeviceSize new_offset);

  void SetSize(VkDeviceSize new_size);

  bool IsValid() const;

  void Reset();

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

  Result<RenderBufferDataAttributeID>
  GetRenderResourceDataAttributeID() const;

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

 private:
  const void* next_{nullptr};

 public:
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

  Result<RenderImageViewAttributeID>
  GetRenderImageViewAttributeID() const;

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

 private:
  const void* next_{nullptr};

 public:
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

  Result<RenderSamplerAttributeID> GetRenderSamplerAttributeID()
      const;

  VkSamplerCreateInfo GetVkSamplerCreateInfo() const;

  void Reset();
};

class ImageView {
  friend class RenderEngine;
  friend class AllocatedImage;

 public:
  ImageView() = default;
  ~ImageView() = default;
  ImageView(RenderEngine* render_engine, VkAllocationCallbacks* allocator,
            const VkImageViewCreateInfo& vk_image_view_create_info);
  ImageView(RenderEngine* render_engine, VkAllocationCallbacks* allocator,
            const ImageViewCreateInfo& image_view_create_info);
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

  void Release();

 private:
  static Result<Nil> CheckInitParameters(
      RenderEngine* render_engine,
      const VkImageViewCreateInfo& vk_image_view_create_info);

 private:
  struct ImageViewWrapper {
    ImageViewWrapper() = default;
    ~ImageViewWrapper();
    ImageViewWrapper(RenderEngine* render_engine,
                     VkAllocationCallbacks* allocator, VkImageView image_view);
    ImageViewWrapper(const ImageViewWrapper& other) = default;
    ImageViewWrapper(ImageViewWrapper&& other) noexcept = default;
    ImageViewWrapper& operator=(const ImageViewWrapper& other) = default;
    ImageViewWrapper& operator=(ImageViewWrapper&& other) noexcept = default;

    RenderEngine* render_engine_{nullptr};
    VkAllocationCallbacks* allocator_{nullptr};
    VkImageView image_view_{nullptr};

    bool IsValid() const;

    void Release();
  };

 private:
  ImageViewCreateInfo image_view_create_info_{};
  ImageViewWrapper image_view_wrapper_{};
};

class Sampler {
  friend class RenderEngine;

 public:
  Sampler() = default;
  ~Sampler() = default;
  Sampler(RenderEngine* render_engine, VkAllocationCallbacks* allocator,
          const VkSamplerCreateInfo& vk_sampler_create_info);
  Sampler(RenderEngine* render_engine, VkAllocationCallbacks* allocator,
          const SamplerCreateInfo& sampler_create_info);
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
  static Result<Nil> CheckInitParameters(
      RenderEngine* render_engine,
      const VkSamplerCreateInfo& vk_sampler_create_info);

 private:
  struct SamplerWrapper {
    SamplerWrapper() = default;
    ~SamplerWrapper() { Release(); }
    SamplerWrapper(RenderEngine* render_engine,
                   VkAllocationCallbacks* allocator, VkSampler sampler);
    SamplerWrapper(const SamplerWrapper& other) = default;
    SamplerWrapper(SamplerWrapper&& other) noexcept = default;
    SamplerWrapper& operator=(const SamplerWrapper& other) = default;
    SamplerWrapper& operator=(SamplerWrapper&& other) noexcept = default;

    bool IsValid() const;

    void Release();

    RenderEngine* render_engine_{nullptr};
    VkAllocationCallbacks* allocator_{nullptr};
    VkSampler sampler_{nullptr};
  };

 private:
  SamplerCreateInfo sampler_create_info_{};
  SamplerWrapper* sampler_wrapper_{nullptr};

  // TODO render_engine recovery
  static Utils::ConcurrentMap<RenderSamplerAttributeID, SamplerWrapper>
      sampler_container_;
};

class ImageBindData {
 public:
  ImageBindData() = default;
  ~ImageBindData() = default;
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

  void Release();

 private:
  DescriptorSetLayoutBinding bind_{};
  ImageView image_view_{};
  Sampler sampler_{};
};

struct MeshBufferCapacityData {
  MeshBufferCapacityData() = default;
  ~MeshBufferCapacityData() = default;
  MeshBufferCapacityData(float capacity_coefficient,
                         float expansion_coefficient,
                         VkDeviceSize index_buffer_remaining_capacity,
                         VkDeviceSize vertex_buffer_remaining_capacity);
  MeshBufferCapacityData(const MeshBufferCapacityData& other) = default;
  MeshBufferCapacityData(MeshBufferCapacityData&& other) noexcept;
  MeshBufferCapacityData& operator=(const MeshBufferCapacityData& other);
  MeshBufferCapacityData& operator=(MeshBufferCapacityData&& other) noexcept;

  float capacity_coefficient_{0.9f};
  float expansion_coefficient_{2.0f};
  VkDeviceSize index_buffer_remaining_capacity_{0};
  VkDeviceSize vertex_buffer_remaining_capacity_{0};

  void Reset();

  bool IsValid() const;
};

struct MeshBufferInfoBase {
  MeshBufferInfoBase() = default;
  ~MeshBufferInfoBase() = default;
  MeshBufferInfoBase(const BufferCreateInfo& buffer_create_info,
                     const AllocationCreateInfo& allocation_create_info);
  MeshBufferInfoBase(const MeshBufferInfoBase& other) = default;
  MeshBufferInfoBase(MeshBufferInfoBase&& other) noexcept;
  MeshBufferInfoBase& operator=(const MeshBufferInfoBase& other);
  MeshBufferInfoBase& operator=(MeshBufferInfoBase&& other) noexcept;

  BufferCreateInfo buffer_create_info_{};
  AllocationCreateInfo allocation_create_info_{};

  void SetBufferCreateInfo(const VkBufferCreateInfo& vk_buffer_create_info);

  void SetAllocationCreateInfo(
      const VmaAllocationCreateInfo& vma_allocation_create_info);

  bool IsValid() const;

  void Reset();
};

using MeshVertexInfo = MeshBufferInfoBase;
using MeshIndexInfo = MeshBufferInfoBase;

class MeshBufferSubResourceAttribute {
 public:
  MeshBufferSubResourceAttribute() = default;
  ~MeshBufferSubResourceAttribute() = default;
  MeshBufferSubResourceAttribute(
      const BufferSubResourceAttribute& vertex_buffer_sub_resource_attribute,
      const BufferSubResourceAttribute& index_buffer_sub_resource_attribute);
  MeshBufferSubResourceAttribute(const MeshBufferSubResourceAttribute& other) =
      default;
  MeshBufferSubResourceAttribute(
      MeshBufferSubResourceAttribute&& other) noexcept = default;
  MeshBufferSubResourceAttribute& operator=(
      const MeshBufferSubResourceAttribute& other);
  MeshBufferSubResourceAttribute& operator=(
      MeshBufferSubResourceAttribute&& other) noexcept;

 public:
  bool operator==(const MeshBufferSubResourceAttribute& rhs) const;

  bool operator!=(const MeshBufferSubResourceAttribute& rhs) const;

 public:
  const BufferChunkInfo& VertexGetChunkInfo() const;

  void VertexSetChunkInfo(const BufferChunkInfo& chunk_info);

  std::uint32_t VertexGetQueueIndex() const;

  void VertexSetQueueIndex(uint32_t queue_index);

  const BufferChunkInfo& IndexGetChunkInfo() const;

  void IndexSetChunkInfo(const BufferChunkInfo& chunk_info);

  std::uint32_t IndexGetQueueIndex() const;

  void IndexSetQueueIndex(uint32_t queue_index);

  bool IsValid() const;

  void Reset();

 private:
  BufferSubResourceAttribute vertex_buffer_sub_resource_attribute_{};
  BufferSubResourceAttribute index_buffer_sub_resource_attribute_{};
};

using RenderPassID = Utils::ID5;

struct RenderPassCreateInfo {
  RenderPassCreateInfo() = default;
  ~RenderPassCreateInfo();
  RenderPassCreateInfo(const void* next, VkRenderPassCreateFlags flags,
                       std::uint32_t attachments_count,
                       const VkAttachmentDescription* attachments,
                       std::uint32_t subpasses_count,
                       const VkSubpassDescription* subpasses,
                       std::uint32_t dependency_count,
                       const VkSubpassDependency* dependencies);
  explicit RenderPassCreateInfo(
      const VkRenderPassCreateInfo& vk_render_pass_create_info);
  RenderPassCreateInfo(const RenderPassCreateInfo& other);
  RenderPassCreateInfo(RenderPassCreateInfo&& other) noexcept;
  RenderPassCreateInfo& operator=(const RenderPassCreateInfo& other);
  RenderPassCreateInfo& operator=(RenderPassCreateInfo&& other) noexcept;

  bool IsValid() const;

  void Reset();

  Result<RenderPassID> GetRenderPassID() const;

  VkRenderPassCreateInfo GetVkRenderPassCreateInfo() const;

  static Result<RenderPassID> GetRenderPassID(
      const RenderPassCreateInfo& render_pass_create_info);

  static Result<RenderPassID> GetRenderPassID(
      const VkRenderPassCreateInfo& vk_render_pass_create_info);

 private:
  const void* next_{nullptr};

 public:
  VkRenderPassCreateFlags flags_;
  std::vector<VkAttachmentDescription> attachments_;
  std::vector<VkSubpassDescription> subpasses_;
  std::vector<VkSubpassDependency> dependencies_;
};

using FrameBufferID = Utils::ID2;

struct FrameBufferCreateInfo {
  FrameBufferCreateInfo() = default;
  ~FrameBufferCreateInfo() = default;
  FrameBufferCreateInfo(const void* next, VkFramebufferCreateFlags flags,
                        VkRenderPass render_pass,
                        std::uint32_t attachment_count,
                        const VkImageView* attachments, uint32_t width,
                        uint32_t height, uint32_t layers);
  explicit FrameBufferCreateInfo(
      const VkFramebufferCreateInfo& frame_buffer_create_info);
  FrameBufferCreateInfo(const FrameBufferCreateInfo& other) = default;
  FrameBufferCreateInfo(FrameBufferCreateInfo&& other) noexcept;
  FrameBufferCreateInfo& operator=(const FrameBufferCreateInfo& other);
  FrameBufferCreateInfo& operator=(FrameBufferCreateInfo&& other) noexcept;

  bool IsValid() const;

  void Reset();

  Result<FrameBufferID> GetRenderFrameID() const;

  VkFramebufferCreateInfo GetVkFrameBufferCreateInfo() const;

  static Result<FrameBufferID> GetRenderFrameID(
      const FrameBufferCreateInfo& frame_buffer_create_info);

 private:
  const void* next_;

 public:
  VkFramebufferCreateFlags flags_;
  VkRenderPass render_pass_;
  std::vector<VkImageView> attachments_;
  uint32_t width_;
  uint32_t height_;
  uint32_t layers_;
};

struct PipelineShaderStageCreateInfo {
  VkPipelineShaderStageCreateFlags flags_{};
  VkShaderStageFlagBits stage_{};
  VkShaderModule module_{nullptr};
  std::string name_{};

  VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo() const;
};

struct PipelineShaderStageCreateInfoes {
  std::vector<VkPipelineShaderStageCreateInfo> stages_{};
};

struct PipelineVertexInputStateCreateInfo {
  VkPipelineVertexInputStateCreateFlags flags_{};
  std::vector<VkVertexInputBindingDescription> vertex_binding_description_;
  std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions_;

  VkPipelineVertexInputStateCreateInfo GetVkPipelineVertexInputStateCreateInfo()
      const {
    return VkPipelineVertexInputStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        nullptr,
        flags_,
        static_cast<uint32_t>(vertex_binding_description_.size()),
        vertex_binding_description_.data(),
        static_cast<uint32_t>(vertex_attribute_descriptions_.size()),
        vertex_attribute_descriptions_.data()};
  }
};

struct PipelineTessellationStateCreateInfo {
  VkPipelineTessellationStateCreateFlags flags{};
  uint32_t patchControlPoints{};

  VkPipelineTessellationStateCreateInfo
  GetVkPipelineTessellationStateCreateInfo() const;
};

struct PipelineRasterizationStateCreateInfo {
  VkPipelineRasterizationStateCreateFlags flags{};
  bool depth_clamp_enable{};
  bool rasterizer_discard_enable{};
  VkPolygonMode polygon_mode{};
  VkCullModeFlags cull_mode{};
  VkFrontFace front_face{};
  bool depth_bias_enable{};
  float depth_bias_constant_factor{};
  float depth_bias_clamp{};
  float depth_bias_slope_factor{};
  float line_width{};

  VkPipelineRasterizationStateCreateInfo
  GetVkPipelineRasterizationStateCreateInfo() const;
};

struct PipelineMultisampleStateCreateInfo {
  VkPipelineMultisampleStateCreateFlags flags_{};
  VkSampleCountFlagBits rasterization_samples_{};
  bool sample_shading_enable_{};
  float min_sample_shading_{};
  VkSampleMask sample_mask_{};
  bool alpha_to_coverage_enable_{};
  bool alpha_to_one_enable_{};

  VkPipelineMultisampleStateCreateInfo GetVkPipelineMultisampleStateCreateInfo()
      const;
};

struct PipelineDepthStencilStateCreateInfo {
  VkPipelineDepthStencilStateCreateFlags flags_{};
  bool depth_test_enable_{};
  bool depth_write_enable_{};
  VkCompareOp depth_compare_op_{};
  bool depth_bounds_test_enable_{};
  bool stencil_test_enable_{};
  VkStencilOpState front_{};
  VkStencilOpState back_{};
  float min_depth_bounds_{};
  float max_depth_bounds_{};

  VkPipelineDepthStencilStateCreateInfo
  GetVkPipelineDepthStencilStateCreateInfo() const;
};

struct PipelineColorBlendStateCreateInfo {
  VkPipelineColorBlendStateCreateFlags flags_{};
  VkBool32 logic_op_enable_{};
  VkLogicOp logic_op_{};
  std::vector<VkPipelineColorBlendAttachmentState> attachments_{};
  std::array<float, 4> blend_constants_{};

  VkPipelineColorBlendStateCreateInfo GetVkPipelineColorBlendStateCreateInfo()
      const;
};

struct PipelineDynamicStateCreateInfo {
  VkPipelineDynamicStateCreateFlags flags_{};
  std::vector<VkDynamicState> dynamic_state_{};

  VkPipelineDynamicStateCreateInfo GetVkPipelineDynamicStateCreateInfo() const;
};

struct PipelineViewportStateCreateInfo {
  VkPipelineViewportStateCreateFlags flags_;
  std::vector<VkViewport> viewports_;
  std::vector<VkRect2D> scissors_;

  VkPipelineViewportStateCreateInfo GetVkPipelineViewportStateCreateInfo()
      const;
};

struct GraphicsPipelineDataInfo {
  VkPipelineCreateFlags flags_;
  PipelineShaderStageCreateInfoes stage_{};
  PipelineVertexInputStateCreateInfo vertex_input_state_;
  PipelineTessellationStateCreateInfo tessellation_state_{};
  PipelineViewportStateCreateInfo viewport_state_{};
  PipelineRasterizationStateCreateInfo rasterization_state_{};
  PipelineMultisampleStateCreateInfo multisample_state_{};
  PipelineDepthStencilStateCreateInfo depth_stencil_state_{};
  PipelineColorBlendStateCreateInfo color_blend_state_{};
  PipelineDynamicStateCreateInfo dynamic_state_{};
  VkPipelineLayout layout_{nullptr};
  VkRenderPass render_pass_{nullptr};
  uint32_t subpass_{0};
};

struct ComputePipelineDataInfo {
  VkPipelineCreateFlags flags_{};
  VkPipelineShaderStageCreateInfo stage_{};
  VkPipelineLayout layout_{nullptr};

  VkComputePipelineCreateInfo GetVkComputePipelineCreateInfo() const;
};

struct PushData1 {
  std::uint64_t slot1_;
};

struct PushData2 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
};

struct PushData3 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
};

struct PushData4 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
};

struct PushData5 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
};

struct PushData6 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
};

struct PushData7 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
};

struct PushData8 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
};

struct PushData9 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
};

struct PushData10 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
  std::uint64_t slot10_;
};

struct PushData11 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
  std::uint64_t slot10_;
  std::uint64_t slot11_;
};

struct PushData12 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
  std::uint64_t slot10_;
  std::uint64_t slot11_;
  std::uint64_t slot12_;
};

struct PushData13 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
  std::uint64_t slot10_;
  std::uint64_t slot11_;
  std::uint64_t slot12_;
  std::uint64_t slot13_;
};

struct PushData14 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
  std::uint64_t slot10_;
  std::uint64_t slot11_;
  std::uint64_t slot12_;
  std::uint64_t slot13_;
  std::uint64_t slot14_;
};

struct PushData15 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
  std::uint64_t slot10_;
  std::uint64_t slot11_;
  std::uint64_t slot12_;
  std::uint64_t slot13_;
  std::uint64_t slot14_;
  std::uint64_t slot15_;
};

struct PushData16 {
  std::uint64_t slot1_;
  std::uint64_t slot2_;
  std::uint64_t slot3_;
  std::uint64_t slot4_;
  std::uint64_t slot5_;
  std::uint64_t slot6_;
  std::uint64_t slot7_;
  std::uint64_t slot8_;
  std::uint64_t slot9_;
  std::uint64_t slot10_;
  std::uint64_t slot11_;
  std::uint64_t slot12_;
  std::uint64_t slot13_;
  std::uint64_t slot14_;
  std::uint64_t slot15_;
  std::uint64_t slot16_;
};

struct DefaultVertexInputStateDescription {
  constexpr static VkVertexInputBindingDescription
      vertex_input_state_bind_description_{
          0, sizeof(AssetSystem::AssetType::Vertex),
          VK_VERTEX_INPUT_RATE_VERTEX};
  static const std::array<VkVertexInputAttributeDescription, 5>
      vertex_input_state_attribute_descriptions_;
};

struct DefaultViewportState {
  static VkViewport default_viewport_;
  static VkRect2D default_scissors_;
  constexpr static VkPipelineViewportStateCreateInfo viewpore_state_{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      nullptr,
      0,
      1,
      &default_viewport_,
      1,
      &default_scissors_};
};

}  // namespace RenderSystem
}  // namespace MM
