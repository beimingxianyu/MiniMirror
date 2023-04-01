#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "runtime/core/log/log_system.h"
#include "runtime/function/render/import_other_system.h"
#include "runtime/function/render/vk_type.h"
#include "runtime/function/render/vk_utils.h"
#include "utils/utils.h"

extern std::shared_ptr<MM::LogSystem::LogSystem> log_system;

namespace MM {
namespace RenderSystem {
class RenderEngine;
class RenderResourceBase;

class RenderResourceManager {
  friend class RenderResourceBase;

 public:
  static std::shared_ptr<RenderResourceManager> GetInstance();

 protected:
  RenderResourceManager() = default;
  static std::shared_ptr<RenderResourceManager> render_resource_manager_;

 private:
  static std::mutex sync_flag;
  static std::atomic_uint32_t increase_index;

  std::unordered_map<uint32_t, RenderResourceBase> resources_;
};

class RenderResourceBase {
 public:
  RenderResourceBase();
  virtual ~RenderResourceBase() = default;
  RenderResourceBase(const std::string& resource_name);
  RenderResourceBase(const RenderResourceBase& other) = default;
  RenderResourceBase(RenderResourceBase&& other) noexcept;
  RenderResourceBase& operator=(const RenderResourceBase& other);
  RenderResourceBase& operator=(RenderResourceBase&& other) noexcept;

  friend bool operator==(const RenderResourceBase& lhs,
                         const RenderResourceBase& rhs) {
    return lhs.resource_ID_ == rhs.resource_ID_;
  }

  friend bool operator!=(const RenderResourceBase& lhs,
                         const RenderResourceBase& rhs) {
    return !(lhs == rhs);
  }

 public:
  const std::string& GetResourceName() const;
  RenderResourceBase& SetResourceName(const std::string& new_resource_name);

  const uint32_t& GetResourceID() const;

  std::shared_ptr<std::unordered_set<uint32_t>> GetReadPassSet();
  std::shared_ptr<const std::unordered_set<uint32_t>> GetReadPassSet() const;
  RenderResourceBase& AddReadPass(const uint32_t& read_pass_index);

  std::shared_ptr<std::unordered_set<uint32_t>> GetWritePassSet();
  std::shared_ptr<const std::unordered_set<uint32_t>> GetWritePassSet() const;
  RenderResourceBase& AddWritePass(const uint32_t& write_pass_index);

  virtual bool IsValid() const = 0;

  /**
   * \brief Release ownership of the resources held.
   */
  virtual void Release();

  /**
   * \brief replaces the managed object
   * \param other The resource to be taken ownership.
   */
  virtual void Reset(MM::RenderSystem::RenderResourceBase* other);

  /**
   * \brief Returns the number of  \ref RenderResourceBase objects referring to
   * the same managed resource. \return The number of  \ref RenderResourceBase
   * objects referring to the same managed resource.
   */
  virtual uint32_t UseCount() const = 0;

  virtual ResourceType GetResourceType() const;

  /**
   * \brief Gets the memory size occupied by the resource.
   * \return The size of the resource.
   * \remark If this object held is a resource array, the sum of the memory
   * occupied by all resources in the array is returned.
   */
  virtual const VkDeviceSize& GetSize() const = 0;

  /**
   * \brief Determine whether the resource is a array.
   * \return Returns true if resource is a array, otherwise returns false.
   */
  virtual bool IsArray() const = 0;

 private:
  std::string resource_name_{};
  uint32_t resource_ID_{0};
  std::shared_ptr<std::unordered_set<uint32_t>> read_pass_set_{};
  std::shared_ptr<std::unordered_set<uint32_t>> write_pass_set{};
};

class RenderResourceTexture final : public RenderResourceBase {
 public:
  RenderResourceTexture() = default;
  ~RenderResourceTexture() override = default;
  RenderResourceTexture(
      const std::string& resource_name, RenderEngine* engine,
      const VkDescriptorType& descriptor_type,
      const std::shared_ptr<AssetType::Image>& image, VkImageUsageFlags usages,
      const VkImageLayout& image_layout, const uint32_t mipmap_levels& = 1,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0);
  /**
   * \remark This constructor does not check the correctness of parameters, so
   * exceptions may be thrown. Therefore, please use this function with caution.
   */
  RenderResourceTexture(const std::string& resource_name, RenderEngine* engine,
                        const VkDescriptorSetLayoutBinding& bind,
                        const AllocatedImage& image,
                        const ImageInfo& image_info,
                        const std::shared_ptr<VkImageView>& image_view,
                        const std::shared_ptr<VkSampler>& sampler,
                        const std::shared_ptr<VkSemaphore>& semaphore);
  RenderResourceTexture(const RenderResourceTexture& other) = default;
  RenderResourceTexture(RenderResourceTexture&& other) noexcept;
  RenderResourceTexture& operator=(const RenderResourceTexture& other);
  RenderResourceTexture& operator=(RenderResourceTexture&& other) noexcept;

 public:
  const VkExtent3D& GetExtent() const;

  const VkFormat& GetFormat() const;

  const VkImageLayout& GetImageLayout() const;

  const ImageInfo& GetImageInfo() const;

  RenderResourceTexture GetCopy() const;

  RenderResourceTexture GetCopyWithNewImageView(
      const VkImageViewCreateInfo& image_view_create_info);

  RenderResourceTexture GetCopyWithNewSampler(
      const VkSamplerCreateInfo& sampler_create_info);

  RenderResourceTexture GetCopyWithNewImageView(
      const VkFormat& image_format, const VkImageLayout& image_layout,
      const std::shared_ptr<VkImageView>& image_view);

  RenderResourceTexture GetCopyWithNewSampler(
      const std::shared_ptr<VkSampler>& sampler);

  VkDescriptorType GetDescriptorType() const;

  std::shared_ptr<VkSemaphore> GetSemaphore() const;

  bool CanMapped() const;

  bool IsStorage() const;

  bool HaveSample() const;

  bool IsValid() const override;

  /**
   * \brief Release ownership of the resources held.
   */
  void Release() override;

  /**
   * \brief replaces the managed object
   * \param other The resource to be taken ownership.
   */
  void Reset(MM::RenderSystem::RenderResourceBase* other) override;

  /**
   * \brief Returns the number of  \ref RenderResourceBase objects referring to
   * the same managed resource. \return The number of  \ref RenderResourceBase
   * objects referring to the same managed resource.
   */
  uint32_t UseCount() const override;

  ResourceType GetResourceType() const override;

  const VkDeviceSize& GetSize() const override;

  bool IsArray() const override;

 private:
  bool CheckInitParameter(const RenderEngine* engine,
                          const VkDescriptorType& descriptor_type,
                          const std::shared_ptr<AssetType::Image>& image,
                          const uint32_t& mipmap_levels,
                          VkImageUsageFlags usages) const;

  bool LoadImageToStageBuffer(const std::shared_ptr<AssetType::Image>& image,
                              AllocatedBuffer& stage_buffer);

  bool InitImage(
      const AllocatedBuffer& stage_buffer, VkImageUsageFlags usages,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0);

  bool GenerateMipmap();

  bool InitImageView();

  bool InitSampler();

  bool InitSemaphore();

 private:
  RenderEngine* render_engine_{nullptr};
  VkDescriptorSetLayoutBinding bind_{};
  AllocatedImage image_{};
  ImageInfo image_info_;
  std::shared_ptr<VkImageView> image_view_{nullptr};
  std::shared_ptr<VkSampler> sampler_{nullptr};
  std::shared_ptr<VkSemaphore> semaphore_{nullptr};
};

class RenderResourceBuffer final : public RenderResourceBase {
 public:
  RenderResourceBuffer() = default;
  ~RenderResourceBuffer() override = default;
  /**
   * \remrak If the type of \ref descriptor_type is not one of dynamic buffer
   * type, the \ref dynamic_offset parameter is invalid. \remark If the \ref
   * data is nullptr, the \ref copy_size and \ref dynamic_offset parameters is
   * invalid. \remark The \ref copy_offset is the offset of the buffer that this
   * object hold will be
   */
  RenderResourceBuffer(
      const std::string& resource_name, RenderEngine* engine,
      const VkDescriptorType& descriptor_type, VkBufferUsageFlags buffer_usage,
      const VkDeviceSize& size, const VkDeviceSize& offset,
      const VkDeviceSize& dynamic_offset = 0, void* data = nullptr,
      const VkDeviceSize& copy_offset = 0, const VkDeviceSize& copy_size = size,
      const VkBufferCreateFlags& buffer_flags = 0,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0);
  /**
   * \remark This constructor does not check the correctness of parameters, so
   * exceptions may be thrown. Therefore, please use this function with caution.
   */
  RenderResourceBuffer(const std::string& resource_name, RenderEngine* engine,
                       const VkDescriptorSetLayoutBinding& bind,
                       const BufferInfo& buffer_info,
                       const AllocatedBuffer& buffer,
                       const std::shared_ptr<VkSemaphore>& semaphore);
  RenderResourceBuffer(const RenderResourceBuffer& other) = default;
  RenderResourceBuffer(RenderResourceBuffer&& other) noexcept;
  RenderResourceBuffer& operator=(const RenderResourceBuffer& other);
  RenderResourceBuffer& operator=(RenderResourceBuffer&& other) noexcept;

 public:
  const VkDescriptorType& GetDescriptorType() const;

  RenderResourceBuffer GetCopy() const;

  /**
   * \remark Returns a RenderResourceBuffer with an offset value of \ref
   * new_offset. When the size of the buffer held by the resource is smaller
   * than \ref new_offset and the buffer is not a dynamic buffer, the new offset
   * is \ref buffer_size; When the buffer is a dynamic buffer and \ref
   * new_offset+dynamic_offset>buffer_size, the \ref new_offset is \ref
   * buffer_size - dynamic_offset.
   */
  RenderResourceBuffer GetCopyWithNewOffset(
      const VkDeviceSize& new_offset) const;

  /**
   * \remark Returns a RenderResourceBuffer with an offset value of \ref
   * new_dynamic_offset. When the buffer_size of the buffer held by the resource
   * is smaller than \ref new_dynamic_offset, the \ref new_dynamic_size is \ref
   * buffer_size - offset.If the buffer held by this resource is not a dynamic
   * buffer, the value of its \ref dynamic_buffer will not be changed.
   */
  RenderResourceBuffer GetCopyWithNewDynamicOffset(
      const VkDeviceSize& new_dynamic_offset) const;

  /**
   * \remark It is equivalent to executing \ref GetBackupWithNewOffset first and
   * then GetBackupWithNewDynamicOffset. \remark \ref new_offset and \ref
   * new_dynamic_offset will only be set when \ref new_offset +
   * new_dynamic_offset < buffer_size is true.
   */
  RenderResourceBuffer GetCopyWithNewOffsetAndDynamicOffset(
      const VkDeviceSize& new_offset,
      const VkDeviceSize& new_dynamic_offset) const;

  /**
   * \remark If the buffer held by the resource is already a dynamic buffer, a
   * copy of the resource is returned (shallow copy, instead of copying the
   * buffer held by the resource, only the shared_ptr pointing to the buffer).
   * If the buffer held by this resource is not a dynamic buffer, a new resource
   * that holds this resource buffer but supports dynamic offset is returned. If
   * the buffer held by this resource does not support dynamic offset, this
   * resource will be returned.
   */
  RenderResourceBuffer GetCopyWithDynamicBuffer(
      const VkDeviceSize& new_offset = VK_WHOLE_SIZE,
      const VkDeviceSize& new_dynamic_offset = VK_WHOLE_SIZE) const;

  bool IsDynamic() const;

  bool IsStorage() const;

  bool IsUniform() const;

  bool IsTexel() const;

  const VkDeviceSize& GetOffset() const;

  const VkDeviceSize& GetDynamicOffset() const;

  const bool& CanMapped() const;

  const BufferInfo& GetBufferInfo() const;

  bool IsValid() const override;

  void Release() override;

  void Reset(MM::RenderSystem::RenderResourceBase* other) override;

  uint32_t UseCount() const override;

  ResourceType GetResourceType() const override;

  const VkDeviceSize& GetSize() const override;

  bool IsArray() const override;

 private:
  bool CheckInitParameter(const RenderEngine* engine,
                          const VkDescriptorType& descriptor_type,
                          const VkBufferUsageFlags& buffer_usage,
                          const VkDeviceSize& size, const VkDeviceSize& offset,
                          const VkDeviceSize& dynamic_offset, const void* data,
                          const VkDeviceSize& copy_offset,
                          const VkDeviceSize& copy_size) const;

  bool InitBuffer(
      const VkBufferUsageFlags& buffer_usage,
      const VkBufferCreateFlags& buffer_flags,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0);

  /**
   * \remark Returns true if the \ref data is nullptr or the data is
   * successfully copied, otherwise returns false.
   */
  bool CopyDataToBuffer(void* data, const VkDeviceSize& offset,
                        const VkDeviceSize& size);

  bool InitSemaphore();

  bool OffsetIsAlignment(const RenderEngine* engine,
                         const VkDescriptorType& descriptor_type,
                         const VkDeviceSize& offset,
                         const VkDeviceSize& dynamic_offset) const;

 private:
  RenderEngine* render_engine_{nullptr};
  VkDescriptorSetLayoutBinding bind_{};
  BufferInfo buffer_info_{};
  AllocatedBuffer buffer_{};
  std::shared_ptr<VkSemaphore> semaphore_{nullptr};
};

class RenderResourceMesh final : public RenderResourceBase {
public:
  RenderResourceMesh() = default;
 RenderResourceMesh(const RenderResourceMesh& other) = default;
 

private:
  RenderEngine* render_engine_{nullptr};
 VertexInputInfo vertex_input_info_{};
  std::vector<AllocatedBuffer> buffer_{};
};

class RenderResourceFrameBuffer final : public RenderResourceBase {
 public:
 private:
};

template <typename ConstantType>
class RenderResourceConstants final : public RenderResourceBase {
 public:
  RenderResourceConstants() = default;
  ~RenderResourceConstants() override = default;
  /**
   * \remark If the ConstantType cannot be directly destructed using "delete",
   * the incoming std:: shared_ptr should come with a customized destructor.
   */
  RenderResourceConstants(const std::string& resource_name,
                          const uint32_t& offset, const uint32_t& size,
                          const std::shared_ptr<ConstantType>& value);
  RenderResourceConstants(const RenderResourceConstants& other) = default;
  RenderResourceConstants(RenderResourceConstants&& other) noexcept;
  RenderResourceConstants& operator=(const RenderResourceConstants& other);
  RenderResourceConstants& operator=(RenderResourceConstants&& other) noexcept;

 public:
  const uint32_t& GetOffset() const;

  std::shared_ptr<const ConstantType> GetValue() const;
  RenderResourceConstants GetCopyWithNewValue(
      const uint32_t& new_offset, const uint32_t& new_size,
      const std::shared_ptr<ConstantType>& new_value);

  const VkDeviceSize& GetSize() const override;

  RenderResourceConstants GetCopyWithNewOffsetAndNewSize(
      const uint32_t& new_offset, const uint32_t& new_size);

  ResourceType GetResourceType() const override;

  bool IsValid() const override;

  /**
   * \brief Release ownership of the resources held.
   */
  void Release() override;

  /**
   * \brief replaces the managed object
   * \param other The resource to be taken ownership.
   * \remark If the resource type of \ref other is not
   * equal to the resource type of this object, this function
   * will only release the ownership of the resource and will
   * not obtain the ownership of the \ref other resource.
   */
  void Reset(MM::RenderSystem::RenderResourceBase* other) override;

  /**
   * \brief Returns the number of  \ref RenderResourceBase objects referring to
   * the same managed resource.
   * \return The number of  \ref RenderResourceBase
   * objects referring to the same managed resource.
   */
  uint32_t UseCount() const override;

  bool IsArray() const override;

 private:
  uint32_t offset_{0};
  uint32_t size_{0};
  std::shared_ptr<ConstantType> value_{nullptr};
};

template <typename ConstantType>
RenderResourceConstants<ConstantType>::RenderResourceConstants(
    const std::string& resource_name, const uint32_t& offset,
    const uint32_t& size, const std::shared_ptr<ConstantType>& value)
    : RenderResourceBase(resource_name),
      offset_(offset),
      size_(size),
      value_(value) {
  // Both offset and size are in units of bytes and must be a multiple of 4.
  if (offset_ % 4 != 0 || size_ % 4 != 0) {
    offset_ = 0;
    size_ = 0;
    value_.reset();
  }
}

template <typename ConstantType>
RenderResourceConstants<ConstantType>::RenderResourceConstants(
    RenderResourceConstants&& other) noexcept
    : offset_(other.offset_),
      size_(other.size_),
      value_(std::move(other.value_)) {
  other.offset_ = 0;
  other.size_ = 0;
}

template <typename ConstantType>
RenderResourceConstants<ConstantType>&
RenderResourceConstants<ConstantType>::operator=(
    const RenderResourceConstants& other) {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(other);

  offset_ = other.offset_;
  size_ = other.size_;
  value_ = other.value_;

  return *this;
}

template <typename ConstantType>
RenderResourceConstants<ConstantType>&
RenderResourceConstants<ConstantType>::operator=(
    RenderResourceConstants&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(std::move(value_));

  offset_ = other.offset_;
  size_ = other.size_;
  value_ = std::move(other.value_);

  offset_ = 0;
  size_ = 0;

  return *this;
}

template <typename ConstantType>
const uint32_t& RenderResourceConstants<ConstantType>::GetOffset() const {
  return offset_;
}

template <typename ConstantType>
std::shared_ptr<const ConstantType>
RenderResourceConstants<ConstantType>::GetValue() const {
  return value_;
}

template <typename ConstantType>
RenderResourceConstants<ConstantType>
RenderResourceConstants<ConstantType>::GetCopyWithNewValue(
    const uint32_t& new_offset, const uint32_t& new_size,
    const std::shared_ptr<ConstantType>& new_value) {
  return RenderResourceConstants<ConstantType>(new_offset, new_size, new_value);
}

template <typename ConstantType>
const VkDeviceSize& RenderResourceConstants<ConstantType>::GetSize() const {
  return size_;
}

template <typename ConstantType>
RenderResourceConstants<ConstantType>
RenderResourceConstants<ConstantType>::GetCopyWithNewOffsetAndNewSize(
    const uint32_t& new_offset, const uint32_t& new_size) {
  return RenderResourceConstants<ConstantType>(new_offset, new_size, value_);
}

template <typename ConstantType>
ResourceType RenderResourceConstants<ConstantType>::GetResourceType() const {
  return ResourceType::CONSTANTS;
}

template <typename ConstantType>
bool RenderResourceConstants<ConstantType>::IsValid() const {
  return size_ != 0 && value_ != nullptr;
}

template <typename ConstantType>
void RenderResourceConstants<ConstantType>::Release() {
  if (!IsValid()) {
    return;
  }

  RenderResourceBase::Release();

  offset_ = 0;
  size_ = 0;
  value_.reset();
}

template <typename ConstantType>
void RenderResourceConstants<ConstantType>::Reset(
    MM::RenderSystem::RenderResourceBase* other) {
  if (other == nullptr) {
    Release();
    return;
  }

  RenderResourceBase::Reset(other);

  if (other->GetResourceType() != ResourceType::CONSTANTS) {
    LOG_WARN(
        "The resource type of the reset resource is different from the "
        "original resource type. Only the resources held by the object will be "
        "released, and resources will not be reset.")
    Release();
    return;
  }

  operator=(*dynamic_cast<RenderResourceConstants*>(other));
}

template <typename ConstantType>
uint32_t RenderResourceConstants<ConstantType>::UseCount() const {
  return value_.use_count();
}

template <typename ConstantType>
bool RenderResourceConstants<ConstantType>::IsArray() const {
  return false;
}
}  // namespace RenderSystem
}  // namespace MM
