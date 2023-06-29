#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <mutex>
#include <unordered_map>

#include "runtime/function/render/import_other_system.h"
#include "runtime/function/render/vk_render_resource.h"
#include "runtime/function/render/vk_type.h"
#include "runtime/function/render/vk_utils.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;
class RenderResourceTexture final : public RenderResourceDataBase {
 public:
  RenderResourceTexture() = default;
  ~RenderResourceTexture() override = default;
  RenderResourceTexture(
      const std::string& resource_name, RenderEngine* engine,
      const VkDescriptorType& descriptor_type,
      const std::shared_ptr<AssetType::Image>& image, VkImageUsageFlags usages,
      const std::vector<std::uint32_t>& queue_index =
          std::vector<std::uint32_t>{},
      const VkSharingMode& sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
      const VkImageLayout& image_layout = VK_IMAGE_LAYOUT_UNDEFINED,
      const uint32_t& mipmap_levels = 1,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0);

  /**
   * \remark This constructor does not check the correctness of parameters, so
   * exceptions may be thrown. Therefore, please use this function with caution.
   */
  RenderResourceTexture(const std::string& resource_name, RenderEngine* engine,
                        const AllocatedImage& image,
                        const ImageBindInfo& image_bind_info);
  RenderResourceTexture(const RenderResourceTexture& other);
  RenderResourceTexture(RenderResourceTexture&& other) noexcept;
  RenderResourceTexture& operator=(const RenderResourceTexture& other);
  RenderResourceTexture& operator=(RenderResourceTexture&& other) noexcept;

 public:
  const VkExtent3D& GetExtent() const;

  const VkFormat& GetFormat() const;

  const VkImageLayout& GetImageLayout() const;

  const std::uint32_t& GetMipmapLevels() const;

  const std::uint32_t& GetArrayLayers() const;

  const ImageInfo& GetImageInfo() const;

  const ImageBindInfo& GetImageBindInfo() const;

  RenderResourceTexture GetCopy() const;

  RenderResourceTexture GetCopyWithNewImageView(
      const VkImageViewCreateInfo& image_view_create_info);

  RenderResourceTexture GetCopyWithNewSampler(
      const VkSamplerCreateInfo& sampler_create_info);

  RenderResourceTexture GetCopyWithNewImageView(
      const std::shared_ptr<VkImageView>& image_view);

  RenderResourceTexture GetCopyWithNewSampler(
      const std::shared_ptr<VkSampler>& sampler);

  VkDescriptorType GetDescriptorType() const;

  bool CanMapped() const;

  bool IsStorage() const;

  bool HaveSample() const;

  bool IsValid() const override;

  std::unique_ptr<RenderResourceDataBase> GetLightCopy(
      const std::string& new_name_of_copy_resource) const override;

  std::unique_ptr<RenderResourceDataBase> GetDeepCopy(
      const std::string& new_name_of_copy_resource) const override;

  /**
   * \brief Release ownership of the resources held.
   */
  void Release() override;

  /**
   * \brief replaces the managed object
   * \param other The resource to be taken ownership.
   */
  void Reset(MM::RenderSystem::RenderResourceDataBase* other) override;

  /**
   * \brief Returns the number of  \ref RenderResourceBase objects referring to
   * the same managed resource. \return The number of  \ref RenderResourceBase
   * objects referring to the same managed resource.
   */
  uint32_t UseCount() const override;

  ResourceType GetResourceType() const override;

  VkDeviceSize GetSize() const override;

  bool IsArray() const override;

  bool CanWrite() const override;

 private:
  bool CheckInitParameter(const RenderEngine* engine,
                          const VkDescriptorType& descriptor_type,
                          const std::shared_ptr<AssetType::Image>& image,
                          const uint32_t& mipmap_levels,
                          VkImageUsageFlags usages) const;

  bool LoadImageToStageBuffer(const std::shared_ptr<AssetType::Image>& image,
                              AllocatedBuffer& stage_buffer,
                              MM::RenderSystem::ImageInfo& image_info);

  ExecuteResult InitImage(
      const AllocatedBuffer& stage_buffer, VkImageUsageFlags usages,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0,
      const ImageInfo& image_info = ImageInfo{});

  ExecuteResult GenerateMipmap();

  bool InitImageView();

  bool InitSampler();

 private:
  RenderEngine* render_engine_{nullptr};
  AllocatedImage image_{};
  ImageBindInfo image_bind_info_{};
};

class RenderResourceBuffer final : public RenderResourceDataBase {
  // TODO add friend class RenderResourceManage
 public:
  RenderResourceBuffer() = delete;
  ~RenderResourceBuffer() override = default;
  /**
   * \remrak If the type of \ref descriptor_type is not one of dynamic buffer
   * type, the \ref dynamic_offset parameter is invalid.
   */
  RenderResourceBuffer(
      const std::string& resource_name, RenderEngine* engine,
      const VkDescriptorType& descriptor_type, VkBufferUsageFlags buffer_usage,
      const VkDeviceSize& size, const VkDeviceSize& offset,
      const std::vector<std::uint32_t>& queue_index =
          std::vector<std::uint32_t>{},
      const VkSharingMode& sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
      const VkDeviceSize& size_range = VK_WHOLE_SIZE,
      const VkDeviceSize& dynamic_offset = 0,
      const DataToBufferInfo& data_info = DataToBufferInfo{},
      const VkBufferCreateFlags& buffer_flags = 0,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0);
  /**
   * \remark This constructor does not check the correctness of parameters, so
   * exceptions may be thrown. Therefore, please use this function with caution.
   */
  RenderResourceBuffer(const std::string& resource_name, RenderEngine* engine,
                       const BufferBindInfo& buffer_info,
                       const AllocatedBuffer& buffer);
  RenderResourceBuffer(const RenderResourceBuffer& other) = default;
  RenderResourceBuffer(RenderResourceBuffer&& other) noexcept;
  RenderResourceBuffer& operator=(const RenderResourceBuffer& other);
  RenderResourceBuffer& operator=(RenderResourceBuffer&& other) noexcept;

 public:
  const VkDescriptorType& GetDescriptorType() const;

  //  /**
  //   * \remark Returns a RenderResourceBuffer with an offset value of \ref
  //   * new_offset. When the size of the buffer held by the resource is smaller
  //   * than \ref new_offset and the buffer is not a dynamic buffer, the new
  //   offset
  //   * is \ref buffer_size; When the buffer is a dynamic buffer and \ref
  //   * new_offset+dynamic_offset>buffer_size, the \ref new_offset is \ref
  //   * buffer_size - dynamic_offset.
  //   */
  //  MM::RenderSystem::RenderResourceBuffer GetCopyWithNewOffset(
  //      const std::string& new_render_resource_name,
  //      const VkDeviceSize& new_offset) const;
  //
  //  /**
  //   * \remark Returns a RenderResourceBuffer with an offset value of \ref
  //   * new_dynamic_offset. When the buffer_size of the buffer held by the
  //   resource
  //   * is smaller than \ref new_dynamic_offset, the \ref new_dynamic_size is
  //   \ref
  //   * buffer_size - offset.If the buffer held by this resource is not a
  //   dynamic
  //   * buffer, the value of its \ref dynamic_buffer will not be changed.
  //   */
  //  RenderResourceBuffer GetCopyWithNewDynamicOffset(
  //      const VkDeviceSize& new_dynamic_offset) const;
  //
  //  /**
  //   * \remark It is equivalent to executing \ref GetBackupWithNewOffset first
  //   and
  //   * then GetBackupWithNewDynamicOffset. \remark \ref new_offset and \ref
  //   * new_dynamic_offset will only be set when \ref new_offset +
  //   * new_dynamic_offset < buffer_size is true.
  //   */
  //  RenderResourceBuffer GetCopyWithNewOffsetAndDynamicOffset(
  //      const VkDeviceSize& new_offset,
  //      const VkDeviceSize& new_dynamic_offset) const;
  //
  //  /**
  //   * \remark If the buffer held by the resource is already a dynamic buffer,
  //   a
  //   * copy of the resource is returned (shallow copy, instead of copying the
  //   * buffer held by the resource, only the shared_ptr pointing to the
  //   buffer).
  //   * If the buffer held by this resource is not a dynamic buffer, a new
  //   resource
  //   * that holds this resource buffer but supports dynamic offset is
  //   returned. If
  //   * the buffer held by this resource does not support dynamic offset, this
  //   * resource will be returned.
  //   */
  //  RenderResourceBuffer GetCopyWithDynamicBuffer(
  //      const VkDeviceSize& new_offset = VK_WHOLE_SIZE,
  //      const VkDeviceSize& new_dynamic_offset = VK_WHOLE_SIZE) const;

  bool IsDynamic() const;

  bool IsStorage() const;

  bool IsUniform() const;

  bool IsTexel() const;

  bool IsTransformSrc() const;

  bool IsTransformDest() const;

  BufferInfo GetBufferInfo() const;

  const VkDeviceSize& GetBufferSize() const;

  const VkDeviceSize& GetOffset() const;

  const VkDeviceSize& GetDynamicOffset() const;

  const VkDeviceSize& GetRangeSize() const;

  bool CanMapped() const;

  const BufferBindInfo& GetBufferBindInfo() const;

  bool IsValid() const override;

  void Release() override;

  void Reset(MM::RenderSystem::RenderResourceDataBase* other) override;

  uint32_t UseCount() const override;

  ResourceType GetResourceType() const override;

  VkDeviceSize GetSize() const override;

  bool IsArray() const override;

  bool CanWrite() const override;

  std::unique_ptr<RenderResourceDataBase> GetLightCopy(
      const std::string& new_name_of_copy_resource) const override;

  std::unique_ptr<RenderResourceDataBase> GetDeepCopy(
      const std::string& new_name_of_copy_resource) const override;

 private:
  bool CheckInitParameter(
      const RenderEngine* engine, const VkDescriptorType& descriptor_type,
      const VkBufferUsageFlags& buffer_usage, const VkDeviceSize& size,
      const VkDeviceSize& range_size, const VkDeviceSize& offset,
      const VkDeviceSize& dynamic_offset, const void* data,
      const VkDeviceSize& copy_offset, const VkDeviceSize& copy_size) const;

  bool InitBuffer(
      const VkDeviceSize& size, const VkBufferUsageFlags& buffer_usage,
      const VkBufferCreateFlags& buffer_flags,
      const VmaMemoryUsage& memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      const VmaAllocationCreateFlags& allocation_flags = 0,
      const std::vector<std ::uint32_t>& queue_index =
          std::vector<std::uint32_t>{},
      const VkSharingMode& sharing_mode = VK_SHARING_MODE_EXCLUSIVE);

  /**
   * \remark Returns true if the \ref data is nullptr or the data is
   * successfully copied, otherwise returns false.
   */
  ExecuteResult CopyDataToBuffer(void* data, const VkDeviceSize& offset,
                                 const VkDeviceSize& size);

  bool OffsetIsAlignment(const RenderEngine* engine,
                         const VkDescriptorType& descriptor_type,
                         const VkDeviceSize& offset,
                         const VkDeviceSize& dynamic_offset) const;

 private:
  RenderEngine* render_engine_{nullptr};
  BufferBindInfo buffer_bind_info_{};
  AllocatedBuffer buffer_{};
  // std::shared_ptr<VkSemaphore> semaphore_{nullptr};
};

class RenderResourceMesh final : public RenderResourceDataBase {
 public:
  RenderResourceMesh() = default;
  ~RenderResourceMesh() override = default;
  RenderResourceMesh(const RenderResourceMesh& other) = default;
  RenderResourceMesh(RenderResourceMesh&& other) noexcept;
  RenderResourceMesh& operator=(const RenderResourceMesh& other);
  RenderResourceMesh& operator=(RenderResourceMesh&& other) noexcept;

 public:
  bool IsValid() const override;
  void Release() override;
  void Reset(MM::RenderSystem::RenderResourceDataBase* other) override;
  uint32_t UseCount() const override;
  ResourceType GetResourceType() const override;
  VkDeviceSize GetSize() const override;
  bool IsArray() const override;
  bool CanWrite() const override;

 protected:
  RenderResourceMesh(const std::string& resource_name, RenderEngine* engine,
                     const std::vector<AssetType::Mesh>& meshes);

 private:
  RenderEngine* render_engine_{nullptr};
  VertexInputState vertex_input_state_{};
  AllocatedBuffer index_buffer_{};
  BufferBindInfo index_buffer_info_{};
  AllocatedBuffer vertex_buffer_{};
  BufferBindInfo vertex_buffer_info{};
  std::vector<AllocatedBuffer> instance_buffers_{};
  std::vector<BufferBindInfo> instance_buffers_info{};
};

class RenderResourceStageBuffer final : public RenderResourceDataBase {};

class RenderResourceFrameBuffer final : public RenderResourceDataBase {
 public:
 private:
};

template <typename ConstantType>
class RenderResourceConstants final : public RenderResourceDataBase {
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

  VkDeviceSize GetSize() const override;

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
  void Reset(MM::RenderSystem::RenderResourceDataBase* other) override;

  /**
   * \brief Returns the number of  \ref RenderResourceBase objects referring to
   * the same managed resource.
   * \return The number of  \ref RenderResourceBase
   * objects referring to the same managed resource.
   */
  uint32_t UseCount() const override;

  bool IsArray() const override;

  bool CanWrite() const override;

 private:
  uint32_t offset_{0};
  uint32_t size_{0};
  std::shared_ptr<ConstantType> value_{nullptr};
};

template <typename ConstantType>
RenderResourceConstants<ConstantType>::RenderResourceConstants(
    const std::string& resource_name, const uint32_t& offset,
    const uint32_t& size, const std::shared_ptr<ConstantType>& value)
    : RenderResourceDataBase(resource_name),
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
  RenderResourceDataBase::operator=(other);

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
  RenderResourceDataBase::operator=(std::move(value_));

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
VkDeviceSize RenderResourceConstants<ConstantType>::GetSize() const {
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

  RenderResourceDataBase::Release();

  offset_ = 0;
  size_ = 0;
  value_.reset();
}

template <typename ConstantType>
void RenderResourceConstants<ConstantType>::Reset(
    MM::RenderSystem::RenderResourceDataBase* other) {
  if (other == nullptr) {
    Release();
    return;
  }

  RenderResourceDataBase::Reset(other);

  if (other->GetResourceType() != ResourceType::CONSTANTS) {
    LOG_WARN(
        "The resource type of the reset resource is different from the "
        "original resource type. Only the resources held by the object will be "
        "released, and resources will not be reset.");
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

template <typename ConstantType>
auto RenderResourceConstants<ConstantType>::CanWrite() const -> bool {
  return false;
}
}  // namespace RenderSystem
}  // namespace MM
