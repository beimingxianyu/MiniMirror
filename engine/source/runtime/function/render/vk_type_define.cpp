//
// Created by beimingxianyu on 23-6-29.
//
#include "runtime/function/render/vk_type_define.h"

#include <vulkan/vulkan_core.h>

#include "runtime/function/render/vk_engine.h"

bool MM::RenderSystem::QueueFamilyIndices::isComplete() const {
  return graphics_family_.has_value() && present_family_.has_value() &&
         compute_family_.has_value() && transform_family_.has_value();
}

void MM::RenderSystem::SemaphoreDestructor::operator()(VkSemaphore* value) {
  if (value == nullptr) {
    return;
  }

  vkDestroySemaphore(render_engine_->GetDevice(), *value, nullptr);

  value = nullptr;
}

MM::RenderSystem::AllocateSemaphore::AllocateSemaphore(
    RenderEngine* engine, VkSemaphoreCreateFlags flags) {
  if (engine == nullptr) {
    return;
  }

  const VkSemaphoreCreateInfo semaphore_create_info =
      Utils::GetSemaphoreCreateInfo(flags);
  VkSemaphore new_semaphore{nullptr};
  MM_VK_CHECK(vkCreateSemaphore(engine->GetDevice(), &semaphore_create_info,
                                nullptr, &new_semaphore),
              MM_LOG_ERROR("Failed to create Semaphore.");
              return;)
  wrapper_ = std::make_shared<AllocateSemaphoreWrapper>(engine, new_semaphore);
}

MM::RenderSystem::AllocateSemaphore&
MM::RenderSystem::AllocateSemaphore::operator=(const AllocateSemaphore& other) {
  if (&other == this) {
    return *this;
  }

  wrapper_ = std::move(other.wrapper_);

  return *this;
}

MM::RenderSystem::AllocateSemaphore&
MM::RenderSystem::AllocateSemaphore::operator=(
    AllocateSemaphore&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  wrapper_ = std::move(other.wrapper_);

  return *this;
}

VkSemaphore& MM::RenderSystem::AllocateSemaphore::GetSemaphore() {
  return wrapper_->GetSemaphore();
}

const VkSemaphore& MM::RenderSystem::AllocateSemaphore::GetSemaphore() const {
  return wrapper_->GetSemaphore();
}

bool MM::RenderSystem::AllocateSemaphore::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid();
}

MM::RenderSystem::AllocateSemaphore::AllocateSemaphoreWrapper::
    AllocateSemaphoreWrapper(RenderEngine* engine, VkSemaphore semaphore)
    : render_engine_(engine), semaphore_(semaphore) {}

MM::RenderSystem::AllocateSemaphore::AllocateSemaphoreWrapper::
    ~AllocateSemaphoreWrapper() {
  vkDestroySemaphore(render_engine_->GetDevice(), semaphore_, nullptr);
}

MM::RenderSystem::AllocateSemaphore::AllocateSemaphoreWrapper::
    AllocateSemaphoreWrapper(AllocateSemaphoreWrapper&& other) noexcept
    : render_engine_(other.render_engine_), semaphore_(other.semaphore_) {
  other.render_engine_ = nullptr;
  other.semaphore_ = nullptr;
}

MM::RenderSystem::AllocateSemaphore::AllocateSemaphoreWrapper&
MM::RenderSystem ::AllocateSemaphore::AllocateSemaphoreWrapper::operator=(
    AllocateSemaphoreWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  render_engine_ = other.render_engine_;
  semaphore_ = other.semaphore_;

  other.render_engine_ = nullptr;
  other.semaphore_ = nullptr;

  return *this;
}

VkSemaphore&
MM::RenderSystem::AllocateSemaphore::AllocateSemaphoreWrapper::GetSemaphore() {
  return semaphore_;
}

const VkSemaphore&
MM::RenderSystem::AllocateSemaphore::AllocateSemaphoreWrapper ::GetSemaphore()
    const {
  return semaphore_;
}

bool MM::RenderSystem::AllocateSemaphore::AllocateSemaphoreWrapper::IsValid()
    const {
  return render_engine_ != nullptr && semaphore_ != nullptr;
}

MM::RenderSystem::AllocateFence::AllocateFence(RenderEngine* engine,
                                               VkFenceCreateFlags flags) {
  if (engine == nullptr) {
    return;
  }

  const VkFenceCreateInfo fence_create_info = Utils::GetFenceCreateInfo(flags);
  VkFence new_fence{nullptr};
  MM_VK_CHECK(vkCreateFence(engine->GetDevice(), &fence_create_info, nullptr,
                            &new_fence),
              MM_LOG_ERROR("Failed to create Fence.");
              return;)
  wrapper_ = std::make_shared<AllocateFenceWrapper>(engine, new_fence);
}

MM::RenderSystem::AllocateFence& MM::RenderSystem::AllocateFence::operator=(
    const AllocateFence& other) {
  if (&other == this) {
    return *this;
  }

  wrapper_ = std::move(other.wrapper_);

  return *this;
}

MM::RenderSystem::AllocateFence& MM::RenderSystem::AllocateFence::operator=(
    AllocateFence&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  wrapper_ = std::move(other.wrapper_);

  return *this;
}

VkFence& MM::RenderSystem::AllocateFence::GetFence() {
  return wrapper_->GetFence();
}

const VkFence& MM::RenderSystem::AllocateFence::GetFence() const {
  return wrapper_->GetFence();
}

bool MM::RenderSystem::AllocateFence::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid();
}

MM::RenderSystem::AllocateFence::AllocateFenceWrapper::AllocateFenceWrapper(
    RenderEngine* engine, VkFence fence)
    : render_engine_(engine), fence_(fence) {}

MM::RenderSystem::AllocateFence::AllocateFenceWrapper::~AllocateFenceWrapper() {
  vkDestroyFence(render_engine_->GetDevice(), fence_, nullptr);
}

MM::RenderSystem::AllocateFence::AllocateFenceWrapper::AllocateFenceWrapper(
    AllocateFenceWrapper&& other) noexcept
    : render_engine_(other.render_engine_), fence_(other.fence_) {
  other.render_engine_ = nullptr;
  other.fence_ = nullptr;
}

MM::RenderSystem::AllocateFence::AllocateFenceWrapper&
MM::RenderSystem ::AllocateFence::AllocateFenceWrapper::operator=(
    AllocateFenceWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  render_engine_ = other.render_engine_;
  fence_ = other.fence_;

  other.render_engine_ = nullptr;
  other.fence_ = nullptr;

  return *this;
}

VkFence& MM::RenderSystem::AllocateFence::AllocateFenceWrapper::GetFence() {
  return fence_;
}

const VkFence&
MM::RenderSystem::AllocateFence::AllocateFenceWrapper ::GetFence() const {
  return fence_;
}

bool MM::RenderSystem::AllocateFence::AllocateFenceWrapper::IsValid() const {
  return render_engine_ != nullptr && fence_ != nullptr;
}

MM::RenderSystem::BufferChunkInfo::BufferChunkInfo(
    const VkDeviceSize& start_offset, const VkDeviceSize& size)
    : offset_(start_offset), size_(size) {}

MM::RenderSystem::BufferChunkInfo& MM::RenderSystem::BufferChunkInfo::operator=(
    const BufferChunkInfo& other) noexcept {
  if (&other == this) {
    return *this;
  }
  offset_ = other.offset_;
  size_ = other.size_;

  return *this;
}

MM::RenderSystem::BufferChunkInfo& MM::RenderSystem::BufferChunkInfo::operator=(
    BufferChunkInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  offset_ = other.offset_;
  size_ = other.size_;

  other.offset_ = 0;
  other.size_ = 0;

  return *this;
}

VkDeviceSize MM::RenderSystem::BufferChunkInfo::GetOffset() const {
  return offset_;
}

VkDeviceSize MM::RenderSystem::BufferChunkInfo::GetSize() const {
  return size_;
}

void MM::RenderSystem::BufferChunkInfo::SetOffset(
    const VkDeviceSize& new_offset) {
  offset_ = new_offset;
}

void MM::RenderSystem::BufferChunkInfo::SetSize(const VkDeviceSize& new_size) {
  size_ = new_size;
}

void MM::RenderSystem::BufferChunkInfo::Reset() {
  offset_ = 0;
  size_ = 0;
}

bool MM::RenderSystem::BufferChunkInfo::IsValid() const { return size_ != 0; }

MM::RenderSystem::BufferChunkInfo::BufferChunkInfo(
    MM::RenderSystem::BufferChunkInfo&& other) noexcept
    : offset_(other.offset_), size_(other.size_) {
  Reset();
}

void MM::RenderSystem::Swap(MM::RenderSystem::BufferChunkInfo& lhs,
                            MM::RenderSystem::BufferChunkInfo& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.offset_, rhs.offset_);
  swap(lhs.size_, rhs.size_);
}

void MM::RenderSystem::swap(MM::RenderSystem::BufferChunkInfo& lhs,
                            MM::RenderSystem::BufferChunkInfo& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.offset_, rhs.offset_);
  swap(lhs.size_, rhs.size_);
}

bool MM::RenderSystem::BufferChunkInfo::operator==(
    const MM::RenderSystem::BufferChunkInfo& rhs) const {
  return offset_ == rhs.offset_ && size_ == rhs.size_;
}

bool MM::RenderSystem::BufferChunkInfo::operator!=(
    const MM::RenderSystem::BufferChunkInfo& rhs) const {
  return !(rhs == *this);
}

MM::RenderSystem::ImageChunkInfo::ImageChunkInfo(const VkOffset3D& offset,
                                                 const VkExtent3D& extent)
    : offset_(offset), extent_(extent) {}

MM::RenderSystem::ImageChunkInfo& MM::RenderSystem::ImageChunkInfo::operator=(
    const ImageChunkInfo& other) {
  if (&other == this) {
    return *this;
  }

  offset_ = other.offset_;
  extent_ = other.extent_;

  return *this;
}

MM::RenderSystem::ImageChunkInfo& MM::RenderSystem::ImageChunkInfo::operator=(
    ImageChunkInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  offset_ = other.offset_;
  extent_ = other.extent_;

  other.Reset();

  return *this;
}

const VkOffset3D& MM::RenderSystem::ImageChunkInfo::GetOffset() const {
  return offset_;
}

const VkExtent3D& MM::RenderSystem::ImageChunkInfo::GetExtent() const {
  return extent_;
}

void MM::RenderSystem::ImageChunkInfo::SetOffset(const VkOffset3D& new_offset) {
  offset_ = new_offset;
}

void MM::RenderSystem::ImageChunkInfo::SetExtent(const VkExtent3D& new_extent) {
  extent_ = new_extent;
}

void MM::RenderSystem::ImageChunkInfo::Reset() {
  offset_ = VkOffset3D{0, 0, 0};
  extent_ = VkExtent3D{0, 0, 0};
}

bool MM::RenderSystem::ImageChunkInfo::IsValid() const {
  if (extent_.width == 0 || extent_.depth == 0 || extent_.height == 0) {
    return false;
  }
  return true;
}

MM::RenderSystem::ImageChunkInfo::ImageChunkInfo(
    MM::RenderSystem::ImageChunkInfo&& other) noexcept
    : offset_(other.offset_), extent_(other.extent_) {
  Reset();
}

MM::RenderSystem::AllocationCreateInfo::AllocationCreateInfo(
    MM::RenderSystem::AllocationCreateInfo&& other) noexcept
    : flags_(other.flags_),
      usage_(other.usage_),
      required_flags_(other.required_flags_),
      preferred_flags_(other.preferred_flags_),
      memory_type_bits_(other.memory_type_bits_),
      priority_(other.priority_) {
  other.Reset();
}

MM::RenderSystem::AllocationCreateInfo&
MM::RenderSystem::AllocationCreateInfo::operator=(
    MM::RenderSystem::AllocationCreateInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  flags_ = other.flags_;
  usage_ = other.usage_;
  required_flags_ = other.required_flags_;
  preferred_flags_ = other.preferred_flags_;
  memory_type_bits_ = other.memory_type_bits_;
  priority_ = other.priority_;

  other.Reset();

  return *this;
}

MM::RenderSystem::AllocationCreateInfo::AllocationCreateInfo(
    VmaAllocationCreateFlags flags, VmaMemoryUsage usage,
    VkMemoryPropertyFlags required_flags, VkMemoryPropertyFlags preferred_flags,
    uint32_t memory_type_bits, float priority)
    : flags_(flags),
      usage_(usage),
      required_flags_(required_flags),
      preferred_flags_(preferred_flags),
      memory_type_bits_(memory_type_bits),
      priority_(priority) {}

MM::RenderSystem::AllocationCreateInfo::AllocationCreateInfo(
    const VmaAllocationCreateInfo& vma_allocation_create_info)
    : flags_(vma_allocation_create_info.flags),
      usage_(vma_allocation_create_info.usage),
      required_flags_(vma_allocation_create_info.requiredFlags),
      preferred_flags_(vma_allocation_create_info.preferredFlags),
      memory_type_bits_(vma_allocation_create_info.memoryTypeBits),
      priority_(vma_allocation_create_info.priority) {}

bool MM::RenderSystem::AllocationCreateInfo::IsValid() const {
  return flags_ != VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM &&
         usage_ != VMA_MEMORY_USAGE_MAX_ENUM;
}

void MM::RenderSystem::AllocationCreateInfo::Reset() {
  flags_ = VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM;
  usage_ = VMA_MEMORY_USAGE_MAX_ENUM;
  required_flags_ = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
  preferred_flags_ = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
  priority_ = 0;
}

VmaAllocationCreateInfo
MM::RenderSystem::AllocationCreateInfo::GetVmaAllocationCreateInfo() const {
  return VmaAllocationCreateInfo{
      flags_,  usage_,  required_flags_, preferred_flags_, memory_type_bits_,
      nullptr, nullptr, priority_};
}

MM::RenderSystem::AllocationCreateInfo&
MM::RenderSystem::AllocationCreateInfo::operator=(
    const MM::RenderSystem::AllocationCreateInfo& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  flags_ = other.flags_;
  usage_ = other.usage_;
  required_flags_ = other.required_flags_;
  preferred_flags_ = other.preferred_flags_;
  memory_type_bits_ = other.memory_type_bits_;
  priority_ = other.priority_;

  return *this;
}

MM::RenderSystem::ImageCreateInfo::ImageCreateInfo(
    uint64_t image_size, VkImageLayout image_layout, const void* next,
    VkImageCreateFlags flags, VkImageType image_type, VkFormat format,
    const VkExtent3D& extent, uint32_t miplevels, uint32_t array_levels,
    VkSampleCountFlags samples, VkImageTiling tiling, VkImageUsageFlags usage,
    VkSharingMode sharing_mode,
    const std::vector<std::uint32_t>& queue_family_indices,
    VkImageLayout initial_layout)
    : image_size_(image_size),
      image_layout_(image_layout),
      next_(next),
      flags_(flags),
      image_type_(image_type),
      format_(format),
      extent_(extent),
      miplevels_(miplevels),
      array_levels_(array_levels),
      samples_(samples),
      tiling_(tiling),
      usage_(usage),
      sharing_mode_(sharing_mode),
      queue_family_indices_(queue_family_indices),
      initial_layout_(initial_layout) {}

MM::RenderSystem::ImageCreateInfo::ImageCreateInfo(
    std::uint64_t image_size, VkImageLayout image_layout,
    const VkImageCreateInfo& vk_image_create_info)
    : image_size_(image_size),
      image_layout_(image_layout),
      next_(vk_image_create_info.pNext),
      flags_(vk_image_create_info.flags),
      image_type_(vk_image_create_info.imageType),
      format_(vk_image_create_info.format),
      extent_(vk_image_create_info.extent),
      miplevels_(vk_image_create_info.mipLevels),
      array_levels_(vk_image_create_info.arrayLayers),
      samples_(vk_image_create_info.samples),
      tiling_(vk_image_create_info.tiling),
      usage_(vk_image_create_info.usage),
      sharing_mode_(vk_image_create_info.sharingMode),
      initial_layout_(vk_image_create_info.initialLayout) {
  queue_family_indices_.reserve(vk_image_create_info.queueFamilyIndexCount);
  for (std::uint64_t i = 0; i != vk_image_create_info.queueFamilyIndexCount;
       ++i) {
    queue_family_indices_.emplace_back(
        vk_image_create_info.pQueueFamilyIndices[i]);
  }
}

bool MM::RenderSystem::ImageCreateInfo::IsValid() const {
  return image_size_ != 0;
}

void MM::RenderSystem::ImageCreateInfo::Reset() {
  image_size_ = 0;
  image_layout_ = VK_IMAGE_LAYOUT_MAX_ENUM;
  next_ = nullptr;
  flags_ = VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM;
  image_type_ = VK_IMAGE_TYPE_MAX_ENUM;
  format_ = VK_FORMAT_MAX_ENUM;
  extent_.width = 0;
  extent_.height = 0;
  extent_.depth = 0;
  miplevels_ = 0;
  array_levels_ = 0;
  samples_ = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
  tiling_ = VK_IMAGE_TILING_MAX_ENUM;
  usage_ = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
  sharing_mode_ = VK_SHARING_MODE_EXCLUSIVE;
  queue_family_indices_.clear();
  initial_layout_ = VK_IMAGE_LAYOUT_MAX_ENUM;
}

MM::RenderSystem::ImageCreateInfo::ImageCreateInfo(
    ImageCreateInfo&& other) noexcept
    : image_size_(other.image_size_),
      image_layout_(other.image_layout_),
      next_(other.next_),
      flags_(other.flags_),
      image_type_(other.image_type_),
      format_(other.format_),
      extent_(other.extent_),
      miplevels_(other.miplevels_),
      array_levels_(other.array_levels_),
      samples_(other.samples_),
      tiling_(other.tiling_),
      usage_(other.usage_),
      sharing_mode_(other.sharing_mode_),
      queue_family_indices_(std::move(other.queue_family_indices_)),
      initial_layout_(other.initial_layout_) {
  other.Reset();
}

MM::RenderSystem::ImageCreateInfo& MM::RenderSystem::ImageCreateInfo::operator=(
    const MM::RenderSystem::ImageCreateInfo& other) {
  if (&other == this) {
    return *this;
  }

  image_size_ = other.image_size_;
  image_layout_ = other.image_layout_;
  next_ = other.next_;
  flags_ = other.flags_;
  image_type_ = other.image_type_;
  format_ = other.format_;
  extent_ = other.extent_;
  miplevels_ = other.miplevels_;
  array_levels_ = other.array_levels_;
  samples_ = other.samples_;
  tiling_ = other.tiling_;
  usage_ = other.usage_;
  sharing_mode_ = other.sharing_mode_;
  queue_family_indices_ = other.queue_family_indices_;
  initial_layout_ = other.initial_layout_;

  return *this;
}

MM::RenderSystem::ImageCreateInfo& MM::RenderSystem::ImageCreateInfo::operator=(
    MM::RenderSystem::ImageCreateInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  image_size_ = other.image_size_;
  image_layout_ = other.image_layout_;
  next_ = other.next_;
  flags_ = other.flags_;
  image_type_ = other.image_type_;
  format_ = other.format_;
  extent_ = other.extent_;
  miplevels_ = other.miplevels_;
  array_levels_ = other.array_levels_;
  samples_ = other.samples_;
  tiling_ = other.tiling_;
  usage_ = other.usage_;
  sharing_mode_ = other.sharing_mode_;
  queue_family_indices_ = std::move(other.queue_family_indices_);
  initial_layout_ = other.initial_layout_;

  other.Reset();

  return *this;
}

VkImageCreateInfo MM::RenderSystem::ImageCreateInfo::GetVkImageCreateInfo()
    const {
  return VkImageCreateInfo{
      VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      next_,
      flags_,
      image_type_,
      format_,
      extent_,
      miplevels_,
      array_levels_,
      static_cast<VkSampleCountFlagBits>(samples_),
      tiling_,
      usage_,
      sharing_mode_,
      static_cast<std::uint32_t>(queue_family_indices_.size()),
      queue_family_indices_.data(),
      initial_layout_};
}

bool MM::RenderSystem::ImageDataInfo::IsValid() const {
  return !image_sub_resource_attributes_.empty() &&
         image_create_info_.IsValid() && allocation_create_info_.IsValid();
}

void MM::RenderSystem::ImageDataInfo::Reset() {
  image_create_info_.Reset();
  allocation_create_info_.Reset();
  image_sub_resource_attributes_.clear();
}

MM::ExecuteResult
MM::RenderSystem::ImageDataInfo::GetRenderResourceDataAttributeID(
    RenderImageDataAttributeID render_image_data_attribute_ID) const {
  if (!IsValid()) {
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  std::uint64_t attribute1 = 0, attribute2 = 0, attribute3 = 0;

  // attribute1
  attribute1 |= image_create_info_.sharing_mode_;
  if (image_create_info_.tiling_ < 1000158000) {
    attribute1 |= image_create_info_.tiling_ << 2;
  } else {
    attribute1 |= 2;
  }
  attribute1 |= image_create_info_.samples_ << 4;
  attribute1 |= image_create_info_.array_levels_ << 7;
  attribute1 |= image_create_info_.miplevels_ << 13;
  attribute1 |=
      Utils::ConvertVkFormatToContinuousValue(image_create_info_.format_) << 19;
  attribute1 |= image_create_info_.image_type_ << 27;
  if (image_create_info_.flags_ == 0x00020000) {
    attribute1 |= static_cast<std::uint64_t>(0x00010000) << 29;
  } else {
    attribute1 |= image_create_info_.flags_ << 29;
  }
  attribute1 |= reinterpret_cast<std::uint64_t>(image_create_info_.next_) << 48;

  // attribute2
  attribute2 |= Utils::ConvertVkImageLayoutToContinuousValue(
      image_create_info_.image_layout_);
  if (image_create_info_.usage_ == 0x00040000) {
    attribute2 |= static_cast<std::uint64_t>(0x00010000) << 5;
  } else {
    attribute2 |= image_create_info_.usage_ << 5;
  }
  attribute2 |= static_cast<std::uint64_t>(image_create_info_.extent_.depth)
                << 25;
  attribute2 |= static_cast<std::uint64_t>(image_create_info_.extent_.height)
                << 38;
  attribute2 |= static_cast<std::uint64_t>(image_create_info_.extent_.width)
                << 51;

  // attribute3
  attribute3 |= static_cast<std::uint64_t>(
      std::floor(allocation_create_info_.priority_ * 100));
  attribute3 |= allocation_create_info_.memory_type_bits_ << 14;
  attribute3 |= allocation_create_info_.preferred_flags_ << 18;
  attribute3 |= allocation_create_info_.required_flags_ << 28;
  attribute3 |= static_cast<std::uint64_t>(allocation_create_info_.usage_)
                << 38;
  attribute3 |= static_cast<std::uint64_t>(allocation_create_info_.flags_)
                << 42;

  render_image_data_attribute_ID.SetSubID1(attribute1);
  render_image_data_attribute_ID.SetSubID2(attribute2);
  render_image_data_attribute_ID.SetSubID3(attribute3);

  return ExecuteResult ::SUCCESS;
}

void MM::RenderSystem::ImageDataInfo::SetAllocationCreateInfo(
    const VmaAllocationCreateInfo& vma_allocation_create_info) {
  allocation_create_info_.flags_ = vma_allocation_create_info.flags;
  allocation_create_info_.usage_ = vma_allocation_create_info.usage;
  allocation_create_info_.required_flags_ =
      vma_allocation_create_info.requiredFlags;
  allocation_create_info_.preferred_flags_ =
      vma_allocation_create_info.preferredFlags;
  allocation_create_info_.memory_type_bits_ =
      vma_allocation_create_info.memoryTypeBits;
  allocation_create_info_.priority_ = vma_allocation_create_info.priority;
}

void MM::RenderSystem::ImageDataInfo::SetImageCreateInfo(
    std::uint64_t image_size, VkImageLayout image_layout,
    const VkImageCreateInfo& vk_image_create_info) {
  image_create_info_.image_size_ = image_size;
  image_create_info_.image_layout_ = image_layout;
  image_create_info_.next_ = vk_image_create_info.pNext;
  image_create_info_.flags_ = vk_image_create_info.flags;
  image_create_info_.image_type_ = vk_image_create_info.imageType;
  image_create_info_.format_ = vk_image_create_info.format;
  image_create_info_.extent_ = vk_image_create_info.extent;
  image_create_info_.miplevels_ = vk_image_create_info.mipLevels;
  image_create_info_.array_levels_ = vk_image_create_info.arrayLayers;
  image_create_info_.samples_ = vk_image_create_info.samples;
  image_create_info_.tiling_ = vk_image_create_info.tiling;
  image_create_info_.usage_ = vk_image_create_info.usage;
  image_create_info_.sharing_mode_ = vk_image_create_info.sharingMode;
  image_create_info_.queue_family_indices_.clear();
  image_create_info_.queue_family_indices_.reserve(
      vk_image_create_info.queueFamilyIndexCount);
  for (std::uint64_t i = 0; i != vk_image_create_info.queueFamilyIndexCount;
       ++i) {
    image_create_info_.queue_family_indices_.emplace_back(
        vk_image_create_info.pQueueFamilyIndices[i]);
  }
  image_create_info_.initial_layout_ = vk_image_create_info.initialLayout;
}

MM::RenderSystem::ImageDataInfo& MM::RenderSystem::ImageDataInfo::operator=(
    const MM::RenderSystem::ImageDataInfo& other) {
  if (&other == this) {
    return *this;
  }

  image_create_info_ = other.image_create_info_;
  allocation_create_info_ = other.allocation_create_info_;
  image_sub_resource_attributes_ = other.image_sub_resource_attributes_;

  return *this;
}

MM::RenderSystem::ImageDataInfo& MM::RenderSystem::ImageDataInfo::operator=(
    MM::RenderSystem::ImageDataInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  image_create_info_ = std::move(other.image_create_info_);
  allocation_create_info_ = std::move(other.allocation_create_info_);
  image_sub_resource_attributes_ =
      std::move(other.image_sub_resource_attributes_);

  return *this;
}

MM::RenderSystem::ImageDataInfo::ImageDataInfo(
    const MM::RenderSystem::ImageCreateInfo& image_create_info,
    const MM::RenderSystem::AllocationCreateInfo& allocation_create_info)
    : image_create_info_(image_create_info),
      allocation_create_info_(allocation_create_info),
      image_sub_resource_attributes_{ImageSubResourceAttribute{
          ImageSubresourceRangeInfo{0, image_create_info.miplevels_, 0,
                                    image_create_info.array_levels_},
          image_create_info.queue_family_indices_[0],
          image_create_info.initial_layout_}} {}

MM::RenderSystem::ImageDataInfo::ImageDataInfo(
    VkDeviceSize size, VkImageLayout image_layout,
    const VkImageCreateInfo& vk_image_create_info,
    const VmaAllocationCreateInfo& vma_allocation_create_info)
    : image_create_info_(size, image_layout, vk_image_create_info),
      allocation_create_info_(vma_allocation_create_info),
      image_sub_resource_attributes_{ImageSubResourceAttribute{
          ImageSubresourceRangeInfo{0, vk_image_create_info.mipLevels, 0,
                                    vk_image_create_info.arrayLayers},
          *vk_image_create_info.pQueueFamilyIndices,
          vk_image_create_info.initialLayout}} {}

MM::RenderSystem::BufferBindInfo::BufferBindInfo(
    const VkDescriptorSetLayoutBinding& bind, VkDeviceSize range_size,
    VkDeviceSize offset, VkDeviceSize dynamic_offset)
    : bind_(bind),
      range_size_(range_size),
      offset_(offset),
      dynamic_offset_(dynamic_offset) {}

MM::RenderSystem::BufferBindInfo::BufferBindInfo(
    BufferBindInfo&& other) noexcept
    : bind_(other.bind_),
      range_size_(other.range_size_),
      offset_(other.offset_),
      dynamic_offset_(other.dynamic_offset_) {
  other.Reset();
}

MM::RenderSystem::BufferBindInfo& MM::RenderSystem::BufferBindInfo::operator=(
    const BufferBindInfo& other) {
  if (&other == this) {
    return *this;
  }
  bind_ = other.bind_;
  range_size_ = other.range_size_;
  offset_ = other.offset_;
  dynamic_offset_ = other.dynamic_offset_;

  return *this;
}

MM::RenderSystem::BufferBindInfo& MM::RenderSystem::BufferBindInfo::operator=(
    BufferBindInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  bind_ = other.bind_;
  range_size_ = other.range_size_;
  offset_ = other.offset_;
  dynamic_offset_ = other.dynamic_offset_;

  other.Reset();

  return *this;
}

void MM::RenderSystem::BufferBindInfo::Reset() {
  range_size_ = 0;
  offset_ = 0;
  dynamic_offset_ = 0;
}

bool MM::RenderSystem::BufferBindInfo::IsValid() const {
  return range_size_ != 0 && dynamic_offset_ < range_size_ &&
         bind_.descriptorCount != 0;
}

MM::RenderSystem::DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(
    uint32_t binding, VkDescriptorType descriptor_type,
    uint32_t descriptor_count, VkShaderStageFlags stage_flags,
    VkSampler const* immutable_samplers)
    : binding_(binding),
      descriptor_type_(descriptor_type),
      descriptor_count_(descriptor_count),
      stage_flags_(stage_flags),
      Immutable_samplers_(immutable_samplers) {}

MM::RenderSystem::DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(
    const VkDescriptorSetLayoutBinding& vk_descriptor_set_layout_binding)
    : binding_(vk_descriptor_set_layout_binding.binding),
      descriptor_type_(vk_descriptor_set_layout_binding.descriptorType),
      descriptor_count_(vk_descriptor_set_layout_binding.descriptorCount),
      stage_flags_(vk_descriptor_set_layout_binding.stageFlags),
      Immutable_samplers_(vk_descriptor_set_layout_binding.pImmutableSamplers) {
}

MM::RenderSystem::DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(
    MM::RenderSystem::DescriptorSetLayoutBinding&& other) noexcept
    : binding_(other.binding_),
      descriptor_type_(other.descriptor_type_),
      descriptor_count_(other.descriptor_count_),
      stage_flags_(other.stage_flags_),
      Immutable_samplers_(other.Immutable_samplers_) {
  other.Reset();
}

MM::RenderSystem::DescriptorSetLayoutBinding&
MM::RenderSystem::DescriptorSetLayoutBinding::operator=(
    MM::RenderSystem::DescriptorSetLayoutBinding&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  binding_ = other.binding_;
  descriptor_type_ = other.descriptor_type_;
  descriptor_count_ = other.descriptor_count_;
  stage_flags_ = other.stage_flags_;
  Immutable_samplers_ = other.Immutable_samplers_;

  other.Reset();

  return *this;
}

bool MM::RenderSystem::DescriptorSetLayoutBinding::IsValid() const {
  return descriptor_count_ != 0;
}

void MM::RenderSystem::DescriptorSetLayoutBinding::Reset() {
  binding_ = UINT32_MAX;
  descriptor_type_ = VK_DESCRIPTOR_TYPE_MAX_ENUM;
  descriptor_count_ = 0;
  stage_flags_ = VK_SHADER_STAGE_ALL;
  Immutable_samplers_ = nullptr;
}

VkDescriptorSetLayoutBinding
MM::RenderSystem::DescriptorSetLayoutBinding::GetVkDescriptorSetLayoutBinding()
    const {
  return VkDescriptorSetLayoutBinding{binding_, descriptor_type_,
                                      descriptor_count_, stage_flags_,
                                      Immutable_samplers_};
}

MM::RenderSystem::ImageViewCreateInfo::ImageViewCreateInfo(
    const void* next, VkImageCreateFlags flags, VkImage image,
    VkImageViewType view_type, VkFormat format,
    const VkComponentMapping& components,
    const VkImageSubresourceRange& subresource_range)
    : next_(next),
      flags_(flags),
      image_(image),
      view_type_(view_type),
      format_(format),
      components_(components),
      subresource_range_(subresource_range) {}

MM::RenderSystem::ImageViewCreateInfo::ImageViewCreateInfo(
    const VkImageViewCreateInfo& vk_image_create_info)
    : next_(vk_image_create_info.pNext),
      flags_(vk_image_create_info.flags),
      image_(vk_image_create_info.image),
      view_type_(vk_image_create_info.viewType),
      format_(vk_image_create_info.format),
      components_(vk_image_create_info.components),
      subresource_range_(vk_image_create_info.subresourceRange) {}

MM::RenderSystem::ImageViewCreateInfo::ImageViewCreateInfo(
    MM::RenderSystem::ImageViewCreateInfo&& other) noexcept
    : next_(other.next_),
      flags_(other.flags_),
      image_(other.image_),
      view_type_(other.view_type_),
      format_(other.format_),
      components_(other.components_),
      subresource_range_(other.subresource_range_) {
  other.Reset();
}

MM::RenderSystem::ImageViewCreateInfo&
MM::RenderSystem::ImageViewCreateInfo::operator=(
    MM::RenderSystem::ImageViewCreateInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  next_ = other.next_;
  flags_ = other.flags_;
  image_ = other.image_;
  view_type_ = other.view_type_;
  format_ = other.format_;
  components_ = other.components_;
  subresource_range_ = other.subresource_range_;

  other.Reset();

  return *this;
}

bool MM::RenderSystem::ImageViewCreateInfo::IsValid() const {
  return image_ != nullptr;
}

MM::ExecuteResult
MM::RenderSystem::ImageViewCreateInfo::GetRenderImageViewAttributeID(
    MM::RenderSystem::RenderImageViewAttributeID&
        render_image_view_attribute_ID) const {
  if (!IsValid()) {
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  std::uint64_t attribute1 = 0;
  std::uint64_t attribute2 = 0;

  // attribute1
  attribute1 |= components_.a;
  attribute1 |= components_.b << 3;
  attribute1 |= components_.g << 6;
  attribute1 |= components_.r << 9;
  attribute1 |= Utils::ConvertVkFormatToContinuousValue(format_) << 12;
  attribute1 |= view_type_ << 20;
  // empty 7bit
  attribute1 |= flags_ << 30;
  attribute1 |= reinterpret_cast<std::uint64_t>(next_) << 48;

  // attribute2
  attribute2 |= subresource_range_.layerCount;
  attribute2 |= subresource_range_.baseArrayLayer << 7;
  attribute2 |= subresource_range_.levelCount << 14;
  attribute2 |= subresource_range_.baseMipLevel << 21;
  attribute2 |= subresource_range_.aspectMask << 28;
  attribute2 |= reinterpret_cast<std::uint64_t>(image_) << 41;

  render_image_view_attribute_ID.SetSubID1(attribute1);
  render_image_view_attribute_ID.SetSubID2(attribute2);

  return ExecuteResult ::SUCCESS;
}

VkImageViewCreateInfo
MM::RenderSystem::ImageViewCreateInfo::GetVkImageViewCreateInfo() const {
  return VkImageViewCreateInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                               next_,
                               flags_,
                               image_,
                               view_type_,
                               format_,
                               components_,
                               subresource_range_};
}

void MM::RenderSystem::ImageViewCreateInfo::Reset() {
  next_ = nullptr;
  flags_ = VK_IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM;
  image_ = nullptr;
  view_type_ = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
  format_ = VK_FORMAT_MAX_ENUM;
  components_.r = VK_COMPONENT_SWIZZLE_MAX_ENUM;
  components_.b = VK_COMPONENT_SWIZZLE_MAX_ENUM;
  components_.g = VK_COMPONENT_SWIZZLE_MAX_ENUM;
  components_.a = VK_COMPONENT_SWIZZLE_MAX_ENUM;
  subresource_range_.aspectMask = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
  subresource_range_.layerCount = 0;
  subresource_range_.baseMipLevel = 0;
  subresource_range_.levelCount = 0;
  subresource_range_.baseArrayLayer = 0;
}

MM::RenderSystem::SamplerCreateInfo::SamplerCreateInfo(
    const void* next, VkSamplerCreateFlags flags, VkFilter mag_filter,
    VkFilter min_filter, VkSamplerMipmapMode mipmap_mode,
    VkSamplerAddressMode address_mode_u, VkSamplerAddressMode address_mode_v,
    VkSamplerAddressMode address_mode_w, float mip_lod_bias,
    VkBool32 anisotropy_enable, float max_anisotropy, VkBool32 compare_enable,
    VkCompareOp compare_op, float min_lod, float max_lod,
    VkBorderColor border_color, VkBool32 unnormalized_coordinates)
    : next_(next),
      flags_(flags),
      mag_filter_(mag_filter),
      min_filter_(min_filter),
      mipmap_mode_(mipmap_mode),
      address_mode_u_(address_mode_u),
      address_mode_v_(address_mode_v),
      address_mode_w_(address_mode_w),
      mip_lod_bias_(mip_lod_bias),
      anisotropy_enable_(anisotropy_enable),
      max_anisotropy_(max_anisotropy),
      compare_enable_(compare_enable),
      compare_op_(compare_op),
      min_lod_(min_lod),
      max_lod_(max_lod),
      border_color_(border_color),
      unnormalized_coordinates_(unnormalized_coordinates) {}

bool MM::RenderSystem::SamplerCreateInfo::IsValid() const {
  return flags_ != VK_SAMPLER_CREATE_FLAG_BITS_MAX_ENUM;
}

VkSamplerCreateInfo
MM::RenderSystem::SamplerCreateInfo::GetVkSamplerCreateInfo() const {
  return VkSamplerCreateInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                             next_,
                             flags_,
                             mag_filter_,
                             min_filter_,
                             mipmap_mode_,
                             address_mode_u_,
                             address_mode_v_,
                             address_mode_w_,
                             mip_lod_bias_,
                             anisotropy_enable_,
                             max_anisotropy_,
                             compare_enable_,
                             compare_op_,
                             min_lod_,
                             max_lod_,
                             border_color_,
                             unnormalized_coordinates_};
}

void MM::RenderSystem::SamplerCreateInfo::Reset() {
  next_ = nullptr;
  flags_ = VK_SAMPLER_CREATE_FLAG_BITS_MAX_ENUM;
  mag_filter_ = VK_FILTER_MAX_ENUM;
  min_filter_ = VK_FILTER_MAX_ENUM;
  mipmap_mode_ = VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
  address_mode_u_ = VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
  address_mode_v_ = VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
  address_mode_w_ = VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
  mip_lod_bias_ = 0;
  anisotropy_enable_ = false;
  max_anisotropy_ = 0;
  compare_enable_ = false;
  compare_op_ = VK_COMPARE_OP_MAX_ENUM;
  min_lod_ = 0;
  max_lod_ = 0;
  border_color_ = VK_BORDER_COLOR_MAX_ENUM;
  unnormalized_coordinates_ = false;
}

MM::RenderSystem::SamplerCreateInfo::SamplerCreateInfo(
    const VkSamplerCreateInfo& vk_sampler_create_info)
    : next_(vk_sampler_create_info.pNext),
      flags_(vk_sampler_create_info.flags),
      mag_filter_(vk_sampler_create_info.magFilter),
      min_filter_(vk_sampler_create_info.minFilter),
      mipmap_mode_(vk_sampler_create_info.mipmapMode),
      address_mode_u_(vk_sampler_create_info.addressModeU),
      address_mode_v_(vk_sampler_create_info.addressModeV),
      address_mode_w_(vk_sampler_create_info.addressModeW),
      mip_lod_bias_(vk_sampler_create_info.mipLodBias),
      anisotropy_enable_(vk_sampler_create_info.anisotropyEnable),
      max_anisotropy_(vk_sampler_create_info.maxAnisotropy),
      compare_enable_(vk_sampler_create_info.compareEnable),
      compare_op_(vk_sampler_create_info.compareOp),
      min_lod_(vk_sampler_create_info.minLod),
      max_lod_(vk_sampler_create_info.maxLod),
      border_color_(vk_sampler_create_info.borderColor),
      unnormalized_coordinates_(
          vk_sampler_create_info.unnormalizedCoordinates) {}

MM::RenderSystem::SamplerCreateInfo::SamplerCreateInfo(
    MM::RenderSystem::SamplerCreateInfo&& other) noexcept
    : next_(other.next_),
      flags_(other.flags_),
      mag_filter_(other.mag_filter_),
      min_filter_(other.min_filter_),
      mipmap_mode_(other.mipmap_mode_),
      address_mode_u_(other.address_mode_u_),
      address_mode_v_(other.address_mode_v_),
      address_mode_w_(other.address_mode_w_),
      mip_lod_bias_(other.mip_lod_bias_),
      anisotropy_enable_(other.anisotropy_enable_),
      max_anisotropy_(other.max_anisotropy_),
      compare_enable_(other.compare_enable_),
      compare_op_(other.compare_op_),
      min_lod_(other.min_lod_),
      max_lod_(other.max_lod_),
      border_color_(other.border_color_),
      unnormalized_coordinates_(other.unnormalized_coordinates_) {
  other.Reset();
}

MM::RenderSystem::SamplerCreateInfo&
MM::RenderSystem::SamplerCreateInfo::operator=(
    MM::RenderSystem::SamplerCreateInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  next_ = other.next_;
  flags_ = other.flags_;
  mag_filter_ = other.mag_filter_;
  min_filter_ = other.min_filter_;
  mipmap_mode_ = other.mipmap_mode_;
  address_mode_u_ = other.address_mode_u_;
  address_mode_v_ = other.address_mode_v_;
  address_mode_w_ = other.address_mode_w_;
  mip_lod_bias_ = other.mip_lod_bias_;
  anisotropy_enable_ = other.anisotropy_enable_;
  max_anisotropy_ = other.max_anisotropy_;
  compare_enable_ = other.compare_enable_;
  compare_op_ = other.compare_op_;
  min_lod_ = other.min_lod_;
  max_lod_ = other.max_lod_;
  border_color_ = other.border_color_;
  unnormalized_coordinates_ = other.unnormalized_coordinates_;

  other.Reset();

  return *this;
}

MM::ExecuteResult
MM::RenderSystem::SamplerCreateInfo::GetRenderSamplerAttributeID(
    MM::RenderSystem::RenderSamplerAttributeID& render_sampler_attribute_ID)
    const {
  if (!IsValid()) {
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  std::uint64_t attribute1 = 0;
  std::uint64_t attribute2 = 0;

  // attribute1
  // empty 37bits
  attribute1 |= static_cast<std::uint64_t>(std::floor(mip_lod_bias_ * 100))
                << 34;
  attribute1 |= static_cast<std::uint64_t>(address_mode_w_) << 40;
  attribute1 |= static_cast<std::uint64_t>(address_mode_v_) << 41;
  attribute1 |= static_cast<std::uint64_t>(address_mode_u_) << 42;
  attribute1 |= static_cast<std::uint64_t>(mipmap_mode_) << 43;
  if (min_filter_ < 1000015000) {
    attribute1 |= static_cast<std::uint64_t>(min_filter_) << 44;
  } else {
    attribute1 |= static_cast<std::uint64_t>(2) << 44;
  }
  if (mag_filter_ < 1000015000) {
    attribute1 |= static_cast<std::uint64_t>(mag_filter_) << 45;
  } else {
    attribute1 |= static_cast<std::uint64_t>(2) << 45;
  }
  attribute1 |= static_cast<std::uint64_t>(flags_) << 46;
  attribute1 |= reinterpret_cast<std::uint64_t>(next_) << 48;

  // attribute2
  attribute2 |= unnormalized_coordinates_;
  if (border_color_ < 1000287003) {
    attribute2 |= border_color_ << 1;
  } else {
    attribute2 |= border_color_ - 1000286997;
  }
  attribute2 |= static_cast<std::uint64_t>(std::floor(min_lod_ * 100)) << 4;
  attribute2 |= static_cast<std::uint64_t>(std::floor(max_lod_ * 100)) << 10;
  attribute2 |= compare_op_ << 17;
  attribute2 |= compare_enable_ << 20;
  attribute2 |= static_cast<std::uint64_t>(std::floor(max_anisotropy_)) << 21;
  attribute2 |= anisotropy_enable_ << 28;

  render_sampler_attribute_ID.SetSubID1(attribute1);
  render_sampler_attribute_ID.SetSubID2(attribute2);

  return ExecuteResult ::SUCCESS;
}

MM::RenderSystem::ImageView::ImageView(
    VkDevice device, VkAllocationCallbacks* allocator,
    const VkImageViewCreateInfo& vk_image_view_create_info)
    : image_view_wrapper_(),
      image_view_create_info_(vk_image_view_create_info) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, vk_image_view_create_info),
           image_view_create_info_.Reset();
           return;)
#endif
  VkImageView image_view;
  MM_VK_CHECK(vkCreateImageView(device, &vk_image_view_create_info, allocator,
                                &image_view),
              MM_LOG_ERROR("Failed to create MM::Render::ImageView.");
              image_view_create_info_.Reset(); return;)

  image_view_wrapper_ = ImageViewWrapper(device, allocator, image_view);
}

MM::RenderSystem::ImageView::ImageView(
    MM::RenderSystem::ImageView&& other) noexcept
    : image_view_wrapper_(std::move(other.image_view_wrapper_)),
      image_view_create_info_(std::move(other.image_view_create_info_)) {}

MM::RenderSystem::ImageView& MM::RenderSystem::ImageView::operator=(
    MM::RenderSystem::ImageView&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  image_view_wrapper_ = std::move(other.image_view_wrapper_);
  image_view_create_info_ = std::move(other.image_view_create_info_);

  return *this;
}

const VkDevice_T* MM::RenderSystem::ImageView::GetDevice() const {
  return image_view_wrapper_.device_;
}

const VkAllocationCallbacks* MM::RenderSystem::ImageView::GetAllocator() const {
  return image_view_wrapper_.allocator_;
}

const VkImageView_T* MM::RenderSystem::ImageView::GetVkImageView() const {
  return image_view_wrapper_.image_view_;
}

const MM::RenderSystem::ImageViewCreateInfo&
MM::RenderSystem::ImageView::GetImageViewCreateInfo() const {
  return image_view_create_info_;
}

bool MM::RenderSystem::ImageView::IsValid() const {
  return image_view_wrapper_.IsValid() && image_view_create_info_.IsValid();
}

void MM::RenderSystem::ImageView::Release() {
  image_view_create_info_.Reset();
  image_view_wrapper_.Release();
}

MM::ExecuteResult MM::RenderSystem::ImageView::CheckInitParameters(
    VkDevice device, const VkImageViewCreateInfo& vk_image_view_create_info) {
  if (device == nullptr) {
    MM_LOG_ERROR("The incoming engine parameter pointer is null.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (vk_image_view_create_info.image == nullptr) {
    MM_LOG_ERROR("The incoming engine parameter pointer is null.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM) {
    MM_LOG_ERROR("The image view type is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.flags ==
      VK_IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The image view create flags is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.format == VK_FORMAT_MAX_ENUM) {
    MM_LOG_ERROR("The image view format is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.components.r == VK_COMPONENT_SWIZZLE_MAX_ENUM ||
      vk_image_view_create_info.components.g == VK_COMPONENT_SWIZZLE_MAX_ENUM ||
      vk_image_view_create_info.components.b == VK_COMPONENT_SWIZZLE_MAX_ENUM ||
      vk_image_view_create_info.components.a == VK_COMPONENT_SWIZZLE_MAX_ENUM) {
    MM_LOG_ERROR("The image view components is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.subresourceRange.aspectMask ==
          VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM ||
      vk_image_view_create_info.subresourceRange.baseMipLevel +
              vk_image_view_create_info.subresourceRange.levelCount >
          128 ||
      vk_image_view_create_info.subresourceRange.baseArrayLayer +
              vk_image_view_create_info.subresourceRange.layerCount >
          128) {
    MM_LOG_ERROR("The image view subresource range is error");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

VkDevice MM::RenderSystem::ImageView::GetDevice() {
  return image_view_wrapper_.device_;
}

VkAllocationCallbacks* MM::RenderSystem::ImageView::GetAllocator() {
  return image_view_wrapper_.allocator_;
}

VkImageView MM::RenderSystem::ImageView::GetVkImageView() {
  return image_view_wrapper_.image_view_;
}

MM::Utils::ConcurrentMap<MM::RenderSystem::RenderSamplerAttributeID,
                         MM::RenderSystem::Sampler::SamplerWrapper>
    MM::RenderSystem::Sampler::sampler_container_(512);

MM::RenderSystem::Sampler::Sampler(
    VkDevice device, VkAllocationCallbacks* allocator,
    const VkSamplerCreateInfo& vk_sampler_create_info)
    : sampler_wrapper_(nullptr), sampler_create_info_(vk_sampler_create_info) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, vk_sampler_create_info),
           device = nullptr;
           allocator = nullptr; sampler_create_info_.Reset(); return;)
#endif
  RenderSamplerAttributeID render_sampler_attribute_ID{};
  MM_CHECK(sampler_create_info_.GetRenderSamplerAttributeID(
               render_sampler_attribute_ID),
           sampler_create_info_.Reset();
           return;)

  std::pair<const RenderSamplerAttributeID, SamplerWrapper>* find_resource =
      sampler_container_.Find(render_sampler_attribute_ID);
  if (find_resource) {
    sampler_wrapper_ = &(find_resource->second);
  }

  VkSampler sampler;
  MM_VK_CHECK(
      vkCreateSampler(device, &vk_sampler_create_info, allocator, &sampler),
      MM_LOG_ERROR("Failed to create MM::Render::Sampler.");
      sampler_create_info_.Reset(); return;)

  auto insert_result = sampler_container_.Emplace(std::make_pair(
      render_sampler_attribute_ID, SamplerWrapper{device, allocator, sampler}));

  if (!insert_result.second) {
    sampler_create_info_.Reset();
    vkDestroySampler(device, sampler, allocator);
    MM_LOG_ERROR("Sampler insert error.");
    return;
  }

  sampler_wrapper_ = &insert_result.first.second;
}

MM::RenderSystem::Sampler::Sampler(MM::RenderSystem::Sampler&& other) noexcept
    : sampler_wrapper_(other.sampler_wrapper_),
      sampler_create_info_(std::move(other.sampler_create_info_)) {
  other.sampler_wrapper_ = nullptr;
}

MM::RenderSystem::Sampler& MM::RenderSystem::Sampler::operator=(
    MM::RenderSystem::Sampler&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  sampler_wrapper_ = other.sampler_wrapper_;
  sampler_create_info_ = std::move(other.sampler_create_info_);

  other.sampler_wrapper_ = nullptr;

  return *this;
}

const VkDevice_T* MM::RenderSystem::Sampler::GetDevice() const {
  return sampler_wrapper_->device_;
}

const VkAllocationCallbacks* MM::RenderSystem::Sampler::GetAllocator() const {
  return sampler_wrapper_->allocator_;
}

const VkSampler_T* MM::RenderSystem::Sampler::GetVkSampler() const {
  return sampler_wrapper_->sampler_;
}

const MM::RenderSystem::SamplerCreateInfo&
MM::RenderSystem::Sampler::GetSamplerCreateInfo() const {
  return sampler_create_info_;
}

bool MM::RenderSystem::Sampler::IsValid() const {
  return sampler_wrapper_->IsValid() && sampler_create_info_.IsValid();
}

void MM::RenderSystem::Sampler::Reset() {
  if (!IsValid()) {
    return;
  }

  sampler_wrapper_ = nullptr;
  sampler_create_info_.Reset();
}

MM::ExecuteResult MM::RenderSystem::Sampler::CheckInitParameters(
    VkDevice device, const VkSamplerCreateInfo& vk_sampler_create_info) {
  if (device == nullptr) {
    MM_LOG_ERROR("The incoming engine parameter pointer is null.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (vk_sampler_create_info.flags == VK_SAMPLER_CREATE_FLAG_BITS_MAX_ENUM) {
    MM_LOG_ERROR("The sampler create flags is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.magFilter == VK_FILTER_MAX_ENUM ||
      vk_sampler_create_info.minFilter == VK_FILTER_MAX_ENUM) {
    MM_LOG_ERROR("The sampler filter is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.mipmapMode == VK_SAMPLER_MIPMAP_MODE_MAX_ENUM) {
    MM_LOG_ERROR("The sampler mipmap mode is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.addressModeU == VK_SAMPLER_ADDRESS_MODE_MAX_ENUM ||
      vk_sampler_create_info.addressModeV == VK_SAMPLER_ADDRESS_MODE_MAX_ENUM ||
      vk_sampler_create_info.addressModeW == VK_SAMPLER_ADDRESS_MODE_MAX_ENUM) {
    MM_LOG_ERROR("The sampler address mode is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.mipLodBias < 0 ||
      vk_sampler_create_info.mipLodBias > 1) {
    MM_LOG_ERROR("The sampler mip lod bias is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.anisotropyEnable == VK_TRUE &&
      vk_sampler_create_info.maxAnisotropy < 1) {
    MM_LOG_ERROR("The sampler max anisotropy is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.compareOp == VK_COMPARE_OP_MAX_ENUM) {
    MM_LOG_ERROR("The sampler compare operator is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.minLod < 0 || vk_sampler_create_info.minLod > 1 ||
      vk_sampler_create_info.maxLod < 0 || vk_sampler_create_info.maxLod > 1 ||
      vk_sampler_create_info.maxLod < vk_sampler_create_info.minLod) {
    MM_LOG_ERROR("The sampler min/max lod is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.borderColor == VK_BORDER_COLOR_MAX_ENUM) {
    MM_LOG_ERROR("The sampler border color is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

VkDevice MM::RenderSystem::Sampler::GetDevice() {
  return sampler_wrapper_->device_;
}

VkAllocationCallbacks* MM::RenderSystem::Sampler::GetAllocator() {
  return sampler_wrapper_->allocator_;
}

VkSampler MM::RenderSystem::Sampler::GetVkSampler() {
  return sampler_wrapper_->sampler_;
}

bool MM::RenderSystem::Sampler::SamplerWrapper::IsValid() const {
  return device_ != nullptr && sampler_ != nullptr;
}

void MM::RenderSystem::Sampler::SamplerWrapper::Release() {
  if (!IsValid()) {
    return;
  }

  vkDestroySampler(device_, sampler_, allocator_);

  device_ = nullptr;
  sampler_ = nullptr;
  allocator_ = nullptr;
}

MM::RenderSystem::Sampler::SamplerWrapper::SamplerWrapper(
    VkDevice device, VkAllocationCallbacks* allocator, VkSampler sampler)
    : device_(device), allocator_(allocator), sampler_(sampler) {}

bool MM::RenderSystem::ImageView::ImageViewWrapper::IsValid() const {
  return device_ != nullptr && image_view_ != nullptr;
}

void MM::RenderSystem::ImageView::ImageViewWrapper::Release() {
  if (!IsValid()) {
    return;
  }

  vkDestroyImageView(device_, image_view_, allocator_);

  device_ = nullptr;
  image_view_ = nullptr;
  allocator_ = nullptr;
}

MM::RenderSystem::ImageView::ImageViewWrapper::ImageViewWrapper(
    VkDevice device, VkAllocationCallbacks* allocator, VkImageView image_view)
    : device_(device), allocator_(allocator), image_view_(image_view) {}

MM::RenderSystem::ImageBindData::ImageBindData(
    const MM::RenderSystem::DescriptorSetLayoutBinding& bind,
    MM::RenderSystem::ImageView&& image_view,
    MM::RenderSystem::Sampler&& sampler)
    : bind_(bind),
      image_view_(std::move(image_view)),
      sampler_(std::move(sampler)) {}

MM::RenderSystem::ImageBindData::ImageBindData(
    MM::RenderSystem::ImageBindData&& other) noexcept
    : bind_(std::move(other.bind_)),
      image_view_(std::move(other.image_view_)),
      sampler_(std::move(other.sampler_)) {}

MM::RenderSystem::ImageBindData& MM::RenderSystem::ImageBindData::operator=(
    MM::RenderSystem::ImageBindData&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  bind_ = std::move(other.bind_);
  image_view_ = std::move(other.image_view_);
  sampler_ = std::move(other.sampler_);
}

const MM::RenderSystem::DescriptorSetLayoutBinding&
MM::RenderSystem::ImageBindData::GetDescriptorSetLayoutBinding() const {
  return bind_;
}

MM::RenderSystem::ImageView& MM::RenderSystem::ImageBindData::GetImageView() {
  return image_view_;
}

const MM::RenderSystem::ImageView&
MM::RenderSystem::ImageBindData::GetImageView() const {
  return image_view_;
}

MM::RenderSystem::Sampler& MM::RenderSystem::ImageBindData::GetSampler() {
  return sampler_;
}

const MM::RenderSystem::Sampler& MM::RenderSystem::ImageBindData::GetSampler()
    const {
  return sampler_;
}

bool MM::RenderSystem::ImageBindData::IsValid() const {
  return bind_.IsValid() && image_view_.IsValid() && sampler_.IsValid();
}

void MM::RenderSystem::ImageBindData::Release() {
  bind_.Reset();
  image_view_.Release();
  sampler_.Reset();
}

MM::RenderSystem::BufferCreateInfo::BufferCreateInfo(
    MM::RenderSystem::BufferCreateInfo&& other) noexcept
    : next_(other.next_),
      flags_(other.flags_),
      size_(other.size_),
      usage_(other.usage_),
      sharing_mode_(other.sharing_mode_),
      queue_family_indices_(std::move(other.queue_family_indices_)) {
  Reset();
}

MM::RenderSystem::BufferCreateInfo&
MM::RenderSystem::BufferCreateInfo::operator=(
    const MM::RenderSystem::BufferCreateInfo& other) {
  if (&other == this) {
    return *this;
  }

  next_ = other.next_;
  flags_ = other.flags_;
  size_ = other.size_;
  usage_ = other.usage_;
  sharing_mode_ = other.sharing_mode_;
  queue_family_indices_ = other.queue_family_indices_;

  return *this;
}

MM::RenderSystem::BufferCreateInfo&
MM::RenderSystem::BufferCreateInfo::operator=(
    MM::RenderSystem::BufferCreateInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  next_ = other.next_;
  flags_ = other.flags_;
  size_ = other.size_;
  usage_ = other.usage_;
  sharing_mode_ = other.sharing_mode_;
  queue_family_indices_ = std::move(queue_family_indices_);

  Reset();

  return *this;
}

bool MM::RenderSystem::BufferCreateInfo::IsValid() const { return size_ != 0; }

VkBufferCreateInfo MM::RenderSystem::BufferCreateInfo::GetVkBufferCreateInfo()
    const {
  return VkBufferCreateInfo{
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      next_,
      flags_,
      size_,
      usage_,
      sharing_mode_,
      static_cast<std::uint32_t>(queue_family_indices_.size()),
      queue_family_indices_.data()};
}

void MM::RenderSystem::BufferCreateInfo::Reset() {
  next_ = nullptr;
  flags_ = VK_BUFFER_CREATE_FLAG_BITS_MAX_ENUM;
  size_ = 0;
  usage_ = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
  sharing_mode_ = VK_SHARING_MODE_MAX_ENUM;
  queue_family_indices_.clear();
}

MM::RenderSystem::BufferCreateInfo::BufferCreateInfo(
    const void* next, VkBufferCreateFlags flags, VkDeviceSize size,
    VkBufferUsageFlags usage, VkSharingMode sharing_mode,
    const std::vector<std::uint32_t>& queue_family_indices)
    : next_(next),
      flags_(flags),
      size_(size),
      usage_(usage),
      sharing_mode_(sharing_mode),
      queue_family_indices_(queue_family_indices) {}

MM::RenderSystem::BufferCreateInfo::BufferCreateInfo(
    const VkBufferCreateInfo& vk_buffer_create_info)
    : next_(vk_buffer_create_info.pNext),
      flags_(vk_buffer_create_info.flags),
      size_(vk_buffer_create_info.size),
      usage_(vk_buffer_create_info.usage),
      sharing_mode_(vk_buffer_create_info.sharingMode),
      queue_family_indices_(vk_buffer_create_info.queueFamilyIndexCount) {
  for (std::uint64_t i = 0; i != vk_buffer_create_info.queueFamilyIndexCount;
       ++i) {
    queue_family_indices_.emplace_back(
        vk_buffer_create_info.pQueueFamilyIndices[i]);
  }
}

MM::ExecuteResult
MM::RenderSystem::BufferDataInfo::GetRenderResourceDataAttributeID(
    RenderImageDataAttributeID& render_image_data_attribute_ID) const {
  if (!IsValid()) {
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  std::uint64_t attribute1 = 0, attribute2 = 0, attribute3 = 0;

  // attribute1
  attribute1 |= buffer_create_info_.usage_;
  attribute1 |= buffer_create_info_.flags_ << 25;
  attribute1 |= static_cast<std::uint64_t>(buffer_create_info_.sharing_mode_)
                << 33;
  attribute1 |= reinterpret_cast<std::uint64_t>(buffer_create_info_.next_)
                << 35;

  // attribute2
  attribute2 = buffer_create_info_.size_;

  // attribute3
  attribute3 |= static_cast<std::uint64_t>(
      std::floor(allocation_create_info_.priority_ * 100));
  attribute3 |= allocation_create_info_.memory_type_bits_ << 14;
  attribute3 |= allocation_create_info_.preferred_flags_ << 18;
  attribute3 |= allocation_create_info_.required_flags_ << 28;
  attribute3 |= static_cast<std::uint64_t>(allocation_create_info_.usage_)
                << 38;
  attribute3 |= static_cast<std::uint64_t>(allocation_create_info_.flags_)
                << 42;

  render_image_data_attribute_ID.SetSubID1(attribute1);
  render_image_data_attribute_ID.SetSubID2(attribute2);
  render_image_data_attribute_ID.SetSubID3(attribute3);

  return ExecuteResult ::SUCCESS;
}

void MM::RenderSystem::BufferDataInfo::SetBufferCreateInfo(
    const VkBufferCreateInfo& vk_buffer_create_info) {
  buffer_create_info_.next_ = vk_buffer_create_info.pNext;
  buffer_create_info_.flags_ = vk_buffer_create_info.flags;
  buffer_create_info_.size_ = vk_buffer_create_info.size;
  buffer_create_info_.usage_ = vk_buffer_create_info.usage;
  buffer_create_info_.sharing_mode_ = vk_buffer_create_info.sharingMode;
  buffer_create_info_.queue_family_indices_.clear();
  buffer_create_info_.queue_family_indices_.reserve(
      vk_buffer_create_info.queueFamilyIndexCount);
  for (std::uint64_t i = 0; i != vk_buffer_create_info.queueFamilyIndexCount;
       ++i) {
    buffer_create_info_.queue_family_indices_.emplace_back(
        vk_buffer_create_info.pQueueFamilyIndices[i]);
  }
}

void MM::RenderSystem::BufferDataInfo::SetAllocationCreateInfo(
    const VmaAllocationCreateInfo& vma_allocation_create_info) {
  allocation_create_info_.flags_ = vma_allocation_create_info.flags;
  allocation_create_info_.usage_ = vma_allocation_create_info.usage;
  allocation_create_info_.required_flags_ =
      vma_allocation_create_info.requiredFlags;
  allocation_create_info_.preferred_flags_ =
      vma_allocation_create_info.preferredFlags;
  allocation_create_info_.memory_type_bits_ =
      vma_allocation_create_info.memoryTypeBits;
  allocation_create_info_.priority_ = vma_allocation_create_info.priority;
}

bool MM::RenderSystem::BufferDataInfo::IsValid() const {
  return buffer_create_info_.IsValid() && allocation_create_info_.IsValid() &&
         !buffer_sub_resource_attributes_.empty();
}

void MM::RenderSystem::BufferDataInfo::Reset() {
  buffer_create_info_.Reset();
  allocation_create_info_.Reset();
  buffer_sub_resource_attributes_.clear();
}

MM::RenderSystem::BufferDataInfo::BufferDataInfo(
    MM::RenderSystem::BufferDataInfo&& other) noexcept
    : buffer_create_info_(std::move(other.buffer_create_info_)),
      allocation_create_info_(std::move(other.allocation_create_info_)),
      buffer_sub_resource_attributes_(
          std::move(other.buffer_sub_resource_attributes_)) {}

MM::RenderSystem::BufferDataInfo& MM::RenderSystem::BufferDataInfo::operator=(
    const MM::RenderSystem::BufferDataInfo& other) {
  if (&other == this) {
    return *this;
  }

  buffer_create_info_ = other.buffer_create_info_;
  allocation_create_info_ = other.allocation_create_info_;
  buffer_sub_resource_attributes_ = other.buffer_sub_resource_attributes_;

  return *this;
}

MM::RenderSystem::BufferDataInfo& MM::RenderSystem::BufferDataInfo::operator=(
    MM::RenderSystem::BufferDataInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  buffer_create_info_ = std::move(other.buffer_create_info_);
  allocation_create_info_ = std::move(other.allocation_create_info_);
  buffer_sub_resource_attributes_ =
      std::move(other.buffer_sub_resource_attributes_);

  return *this;
}

MM::RenderSystem::BufferDataInfo::BufferDataInfo(
    const MM::RenderSystem::BufferCreateInfo& buffer_create_info,
    const MM::RenderSystem::AllocationCreateInfo& allocation_create_info)
    : buffer_create_info_(buffer_create_info),
      allocation_create_info_(allocation_create_info),
      buffer_sub_resource_attributes_{BufferSubResourceAttribute{
          0, buffer_create_info.size_,
          buffer_create_info.queue_family_indices_[0]}} {}

std::uint32_t MM::RenderSystem::ImageSubresourceRangeInfo::GetBaseMipmapsLevel()
    const {
  return base_mipmaps_level_;
}

void MM::RenderSystem::ImageSubresourceRangeInfo::SetBaseMipmapsLevel(
    std::uint32_t dest_mipmaps_level) {
  base_mipmaps_level_ = dest_mipmaps_level;
}

std::uint32_t MM::RenderSystem::ImageSubresourceRangeInfo::GetMipmapsCount()
    const {
  return mipmaps_count_;
}

void MM::RenderSystem::ImageSubresourceRangeInfo::SetMipmapsCount(
    std::uint32_t mipmaps_count) {
  mipmaps_count_ = mipmaps_count;
}

std::uint32_t MM::RenderSystem::ImageSubresourceRangeInfo::GetBaseArrayLevel()
    const {
  return base_array_level_;
}

void MM::RenderSystem::ImageSubresourceRangeInfo::SetBaseArrayLevel(
    std::uint32_t dest_array_level) {
  base_array_level_ = dest_array_level;
}

std::uint32_t MM::RenderSystem::ImageSubresourceRangeInfo::GetArrayCount()
    const {
  return array_count_;
}

void MM::RenderSystem::ImageSubresourceRangeInfo::SetArrayCount(
    std::uint32_t array_count) {
  array_count_ = array_count;
}

void MM::RenderSystem::ImageSubresourceRangeInfo::Reset() {
  base_mipmaps_level_ = 0;
  mipmaps_count_ = 0;
  base_array_level_ = 0;
  array_count_ = 0;
}

bool MM::RenderSystem::ImageSubresourceRangeInfo::IsValid() const {
  return mipmaps_count_ != 0 && array_count_ != 0;
}

MM::RenderSystem::ImageSubresourceRangeInfo::ImageSubresourceRangeInfo(
    MM::RenderSystem::ImageSubresourceRangeInfo&& other) noexcept
    : base_mipmaps_level_(other.base_mipmaps_level_),
      mipmaps_count_(other.mipmaps_count_),
      base_array_level_(other.array_count_),
      array_count_(other.array_count_) {
  Reset();
}

MM::RenderSystem::ImageSubresourceRangeInfo&
MM::RenderSystem::ImageSubresourceRangeInfo::operator=(
    const MM::RenderSystem::ImageSubresourceRangeInfo& other) {
  if (&other == this) {
    return *this;
  }

  base_mipmaps_level_ = other.base_mipmaps_level_;
  mipmaps_count_ = other.mipmaps_count_;
  base_array_level_ = other.base_array_level_;
  array_count_ = other.array_count_;

  return *this;
}

MM::RenderSystem::ImageSubresourceRangeInfo&
MM::RenderSystem::ImageSubresourceRangeInfo::operator=(
    MM::RenderSystem::ImageSubresourceRangeInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  base_mipmaps_level_ = other.base_mipmaps_level_;
  mipmaps_count_ = other.mipmaps_count_;
  base_array_level_ = other.base_array_level_;
  array_count_ = other.array_count_;

  Reset();

  return *this;
}

MM::RenderSystem::ImageSubresourceRangeInfo::ImageSubresourceRangeInfo(
    uint32_t dest_mipmaps_level, uint32_t mipmaps_count,
    uint32_t dest_array_level, uint32_t array_count)
    : base_mipmaps_level_(dest_mipmaps_level),
      mipmaps_count_(mipmaps_count),
      base_array_level_(dest_array_level),
      array_count_(array_count) {}

MM::RenderSystem::ImageSubresourceRangeInfo::ImageSubresourceRangeInfo(
    const VkImageSubresourceRange& vk_image_subresource_range)
    : base_mipmaps_level_(vk_image_subresource_range.baseMipLevel),
      mipmaps_count_(vk_image_subresource_range.levelCount),
      base_array_level_(vk_image_subresource_range.baseArrayLayer),
      array_count_(vk_image_subresource_range.layerCount) {}

bool MM::RenderSystem::ImageSubresourceRangeInfo::operator==(
    const MM::RenderSystem::ImageSubresourceRangeInfo& rhs) const {
  return base_mipmaps_level_ == rhs.base_mipmaps_level_ &&
         mipmaps_count_ == rhs.mipmaps_count_ &&
         base_array_level_ == rhs.base_array_level_ &&
         array_count_ == rhs.array_count_;
}

bool MM::RenderSystem::ImageSubresourceRangeInfo::operator!=(
    const MM::RenderSystem::ImageSubresourceRangeInfo& rhs) const {
  return !(rhs == *this);
}

const MM::RenderSystem::ImageSubresourceRangeInfo&
MM::RenderSystem::ImageSubResourceAttribute::GetImageSubresourceRangeInfo()
    const {
  return image_subresource_range_info_;
}

void MM::RenderSystem::ImageSubResourceAttribute::SetImageSubresourceRangeInfo(
    const MM::RenderSystem::ImageSubresourceRangeInfo&
        image_subresource_range_info) {
  image_subresource_range_info_ = image_subresource_range_info;
}

uint32_t MM::RenderSystem::ImageSubResourceAttribute::GetQueueIndex() const {
  return queue_index_;
}

void MM::RenderSystem::ImageSubResourceAttribute::SetQueueIndex(
    uint32_t queue_index) {
  queue_index_ = queue_index;
}

VkImageLayout MM::RenderSystem::ImageSubResourceAttribute::GetImageLayout()
    const {
  return image_layout_;
}

void MM::RenderSystem::ImageSubResourceAttribute::SetImageLayout(
    VkImageLayout image_layout) {
  image_layout_ = image_layout;
}

bool MM::RenderSystem::ImageSubResourceAttribute::IsValid() const {
  return queue_index_ != UINT32_MAX &&
         image_layout_ != VK_IMAGE_LAYOUT_MAX_ENUM &&
         image_subresource_range_info_.IsValid();
}

void MM::RenderSystem::ImageSubResourceAttribute::Reset() {
  queue_index_ = UINT32_MAX;
  image_layout_ = VK_IMAGE_LAYOUT_MAX_ENUM;
  image_subresource_range_info_.Reset();
}

MM::RenderSystem::ImageSubResourceAttribute::ImageSubResourceAttribute(
    const MM::RenderSystem::ImageSubresourceRangeInfo&
        image_subresource_range_info,
    uint32_t queue_index, VkImageLayout image_layout)
    : image_subresource_range_info_(image_subresource_range_info),
      queue_index_(queue_index),
      image_layout_(image_layout) {}

MM::RenderSystem::ImageSubResourceAttribute::ImageSubResourceAttribute(
    const VkImageSubresourceRange& vk_image_subresource_range,
    uint32_t queue_index, VkImageLayout image_layout)
    : image_subresource_range_info_(vk_image_subresource_range),
      queue_index_(queue_index),
      image_layout_(image_layout) {}

MM::RenderSystem::ImageSubResourceAttribute::ImageSubResourceAttribute(
    MM::RenderSystem::ImageSubResourceAttribute&& other) noexcept

    : image_subresource_range_info_(
          std::move(other.image_subresource_range_info_)),
      queue_index_(other.queue_index_),
      image_layout_(other.image_layout_) {
  other.Reset();
}

MM::RenderSystem::ImageSubResourceAttribute&
MM::RenderSystem::ImageSubResourceAttribute::operator=(
    const MM::RenderSystem::ImageSubResourceAttribute& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  image_subresource_range_info_ = other.image_subresource_range_info_;
  queue_index_ = other.queue_index_;
  image_layout_ = other.image_layout_;

  return *this;
}

MM::RenderSystem::ImageSubResourceAttribute&
MM::RenderSystem::ImageSubResourceAttribute::operator=(
    MM::RenderSystem::ImageSubResourceAttribute&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  image_subresource_range_info_ =
      std::move(other.image_subresource_range_info_);
  queue_index_ = other.queue_index_;
  image_layout_ = other.image_layout_;

  queue_index_ = UINT32_MAX;
  image_layout_ = VK_IMAGE_LAYOUT_MAX_ENUM;

  return *this;
}

std::uint32_t MM::RenderSystem::ImageSubResourceAttribute::GetBaseMipmapLevel()
    const {
  return image_subresource_range_info_.GetBaseMipmapsLevel();
}

std::uint32_t MM::RenderSystem::ImageSubResourceAttribute::GetBaseArrayLevel()
    const {
  return image_subresource_range_info_.GetBaseArrayLevel();
}

std::uint32_t MM::RenderSystem::ImageSubResourceAttribute::GetMipmapCount()
    const {
  return image_subresource_range_info_.GetMipmapsCount();
}

std::uint32_t MM::RenderSystem::ImageSubResourceAttribute::GetArrayCount()
    const {
  return image_subresource_range_info_.GetArrayCount();
}

bool MM::RenderSystem::ImageSubResourceAttribute::operator==(
    const MM::RenderSystem::ImageSubResourceAttribute& rhs) const {
  return image_subresource_range_info_ == rhs.image_subresource_range_info_ &&
         queue_index_ == rhs.queue_index_ && image_layout_ == rhs.image_layout_;
}

bool MM::RenderSystem::ImageSubResourceAttribute::operator!=(
    const MM::RenderSystem::ImageSubResourceAttribute& rhs) const {
  return !(rhs == *this);
}

MM::RenderSystem::BufferSubResourceAttribute::BufferSubResourceAttribute(
    MM::RenderSystem::BufferSubResourceAttribute&& other) noexcept
    : chunk_info_(std::move(other.chunk_info_)),
      queue_index_(other.queue_index_) {
  other.queue_index_ = UINT32_MAX;
}

MM::RenderSystem::BufferSubResourceAttribute&
MM::RenderSystem::BufferSubResourceAttribute::operator=(
    const MM::RenderSystem::BufferSubResourceAttribute& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  chunk_info_ = other.chunk_info_;
  queue_index_ = other.queue_index_;

  return *this;
}

MM::RenderSystem::BufferSubResourceAttribute&
MM::RenderSystem::BufferSubResourceAttribute::operator=(
    MM::RenderSystem::BufferSubResourceAttribute&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  chunk_info_ = std::move(other.chunk_info_);
  queue_index_ = other.queue_index_;

  queue_index_ = UINT32_MAX;

  return *this;
}

const MM::RenderSystem::BufferChunkInfo&
MM::RenderSystem::BufferSubResourceAttribute::GetChunkInfo() const {
  return chunk_info_;
}

void MM::RenderSystem::BufferSubResourceAttribute::SetChunkInfo(
    const MM::RenderSystem::BufferChunkInfo& chunk_info) {
  chunk_info_ = chunk_info;
}

std::uint32_t MM::RenderSystem::BufferSubResourceAttribute::GetQueueIndex()
    const {
  return queue_index_;
}

void MM::RenderSystem::BufferSubResourceAttribute::SetQueueIndex(
    uint32_t queue_index) {
  queue_index_ = queue_index;
}

bool MM::RenderSystem::BufferSubResourceAttribute::operator==(
    const MM::RenderSystem::BufferSubResourceAttribute& rhs) const {
  return chunk_info_ == rhs.chunk_info_ && queue_index_ == rhs.queue_index_;
}

bool MM::RenderSystem::BufferSubResourceAttribute::operator!=(
    const MM::RenderSystem::BufferSubResourceAttribute& rhs) const {
  return !(rhs == *this);
}

bool MM::RenderSystem::BufferSubResourceAttribute::IsValid() const {
  return chunk_info_.IsValid() && queue_index_ != UINT32_MAX;
}

void MM::RenderSystem::BufferSubResourceAttribute::Reset() {
  chunk_info_.Reset();
  queue_index_ = UINT32_MAX;
}

VkDeviceSize MM::RenderSystem::BufferSubResourceAttribute::GetOffset() const {
  return GetChunkInfo().GetOffset();
}

VkDeviceSize MM::RenderSystem::BufferSubResourceAttribute::GetSize() const {
  return GetChunkInfo().GetSize();
}

VkDeviceSize MM::RenderSystem::BufferSubResourceAttribute::SetOffset(
    VkDeviceSize new_offset) {
  chunk_info_.SetOffset(new_offset);
}

VkDeviceSize MM::RenderSystem::BufferSubResourceAttribute::SetSize(
    VkDeviceSize new_size) {
  chunk_info_.SetSize(new_size);
}

void MM::RenderSystem::MeshBufferCapacityData::Reset() {
  capacity_coefficient_ = 0;
  expansion_coefficient_ = 0;
  index_buffer_remaining_capacity_ = 0;
  vertex_buffer_remaining_capacity_ = 0;
}

bool MM::RenderSystem::MeshBufferCapacityData::IsValid() const {
  return capacity_coefficient_ > 0.0f && expansion_coefficient_ > 1.0f;
}

MM::RenderSystem::MeshBufferCapacityData::MeshBufferCapacityData(
    float capacity_coefficient, float expansion_coefficient,
    VkDeviceSize index_buffer_remaining_capacity,
    VkDeviceSize vertex_buffer_remaining_capacity)
    : capacity_coefficient_(capacity_coefficient),
      expansion_coefficient_(expansion_coefficient),
      index_buffer_remaining_capacity_(index_buffer_remaining_capacity),
      vertex_buffer_remaining_capacity_(vertex_buffer_remaining_capacity) {}

MM::RenderSystem::MeshBufferCapacityData::MeshBufferCapacityData(
    MM::RenderSystem::MeshBufferCapacityData&& other) noexcept
    : capacity_coefficient_(other.capacity_coefficient_),
      expansion_coefficient_(other.expansion_coefficient_),
      index_buffer_remaining_capacity_(other.index_buffer_remaining_capacity_),
      vertex_buffer_remaining_capacity_(
          other.vertex_buffer_remaining_capacity_) {
  other.Reset();
}

MM::RenderSystem::MeshBufferCapacityData&
MM::RenderSystem::MeshBufferCapacityData::operator=(
    const MM::RenderSystem::MeshBufferCapacityData& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  capacity_coefficient_ = other.capacity_coefficient_;
  expansion_coefficient_ = other.expansion_coefficient_;
  index_buffer_remaining_capacity_ = other.index_buffer_remaining_capacity_;
  vertex_buffer_remaining_capacity_ = other.vertex_buffer_remaining_capacity_;

  return *this;
}

MM::RenderSystem::MeshBufferCapacityData&
MM::RenderSystem::MeshBufferCapacityData::operator=(
    MM::RenderSystem::MeshBufferCapacityData&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  capacity_coefficient_ = other.capacity_coefficient_;
  expansion_coefficient_ = other.expansion_coefficient_;
  index_buffer_remaining_capacity_ = other.index_buffer_remaining_capacity_;
  vertex_buffer_remaining_capacity_ = other.vertex_buffer_remaining_capacity_;

  other.Reset();

  return *this;
}

MM::RenderSystem::MeshBufferInfoBase::MeshBufferInfoBase(
    const MM::RenderSystem::BufferCreateInfo& buffer_create_info,
    const MM::RenderSystem::AllocationCreateInfo& allocation_create_info)
    : buffer_create_info_(buffer_create_info),
      allocation_create_info_(allocation_create_info) {}

MM::RenderSystem::MeshBufferInfoBase::MeshBufferInfoBase(
    MM::RenderSystem::MeshBufferInfoBase&& other) noexcept
    : buffer_create_info_(std::move(other.buffer_create_info_)),
      allocation_create_info_(std::move(other.allocation_create_info_)) {}

MM::RenderSystem::MeshBufferInfoBase&
MM::RenderSystem::MeshBufferInfoBase::operator=(
    const MM::RenderSystem::MeshBufferInfoBase& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  buffer_create_info_ = other.buffer_create_info_;
  allocation_create_info_ = other.allocation_create_info_;

  return *this;
}

MM::RenderSystem::MeshBufferInfoBase&
MM::RenderSystem::MeshBufferInfoBase::operator=(
    MM::RenderSystem::MeshBufferInfoBase&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  buffer_create_info_ = std::move(other.buffer_create_info_);
  allocation_create_info_ = std::move(other.allocation_create_info_);

  return *this;
}

void MM::RenderSystem::MeshBufferInfoBase::SetBufferCreateInfo(
    const VkBufferCreateInfo& vk_buffer_create_info) {
  buffer_create_info_.next_ = vk_buffer_create_info.pNext;
  buffer_create_info_.flags_ = vk_buffer_create_info.flags;
  buffer_create_info_.size_ = vk_buffer_create_info.size;
  buffer_create_info_.usage_ = vk_buffer_create_info.usage;
  buffer_create_info_.sharing_mode_ = vk_buffer_create_info.sharingMode;
  buffer_create_info_.queue_family_indices_.clear();
  buffer_create_info_.queue_family_indices_.reserve(
      vk_buffer_create_info.queueFamilyIndexCount);
  for (std::uint64_t i = 0; i != vk_buffer_create_info.queueFamilyIndexCount;
       ++i) {
    buffer_create_info_.queue_family_indices_.emplace_back(
        vk_buffer_create_info.pQueueFamilyIndices[i]);
  }
}

void MM::RenderSystem::MeshBufferInfoBase::SetAllocationCreateInfo(
    const VmaAllocationCreateInfo& vma_allocation_create_info) {
  allocation_create_info_.flags_ = vma_allocation_create_info.flags;
  allocation_create_info_.usage_ = vma_allocation_create_info.usage;
  allocation_create_info_.required_flags_ =
      vma_allocation_create_info.requiredFlags;
  allocation_create_info_.preferred_flags_ =
      vma_allocation_create_info.preferredFlags;
  allocation_create_info_.memory_type_bits_ =
      vma_allocation_create_info.memoryTypeBits;
  allocation_create_info_.priority_ = vma_allocation_create_info.priority;
}

bool MM::RenderSystem::MeshBufferInfoBase::IsValid() const {
  return buffer_create_info_.IsValid() && allocation_create_info_.IsValid();
}

void MM::RenderSystem::MeshBufferInfoBase::Reset() {
  buffer_create_info_.Reset();
  allocation_create_info_.Reset();
}

MM::RenderSystem::MeshBufferSubResourceAttribute::
    MeshBufferSubResourceAttribute(
        const MM::RenderSystem::BufferSubResourceAttribute&
            vertex_buffer_sub_resource_attribute,
        const MM::RenderSystem::BufferSubResourceAttribute&
            index_buffer_sub_resource_attribute)
    : vertex_buffer_sub_resource_attribute_(
          vertex_buffer_sub_resource_attribute),
      index_buffer_sub_resource_attribute_(
          index_buffer_sub_resource_attribute) {}

MM::RenderSystem::MeshBufferSubResourceAttribute&
MM::RenderSystem::MeshBufferSubResourceAttribute::operator=(
    const MM::RenderSystem::MeshBufferSubResourceAttribute& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  vertex_buffer_sub_resource_attribute_ =
      other.vertex_buffer_sub_resource_attribute_;
  index_buffer_sub_resource_attribute_ =
      other.index_buffer_sub_resource_attribute_;

  return *this;
}

MM::RenderSystem::MeshBufferSubResourceAttribute&
MM::RenderSystem::MeshBufferSubResourceAttribute::operator=(
    MM::RenderSystem::MeshBufferSubResourceAttribute&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  vertex_buffer_sub_resource_attribute_ =
      std::move(other.vertex_buffer_sub_resource_attribute_);
  index_buffer_sub_resource_attribute_ =
      std::move(other.index_buffer_sub_resource_attribute_);

  return *this;
}

bool MM::RenderSystem::MeshBufferSubResourceAttribute::operator==(
    const MM::RenderSystem::MeshBufferSubResourceAttribute& rhs) const {
  return (vertex_buffer_sub_resource_attribute_ ==
          rhs.vertex_buffer_sub_resource_attribute_) &&
         (index_buffer_sub_resource_attribute_ ==
          rhs.index_buffer_sub_resource_attribute_);
}
bool MM::RenderSystem::MeshBufferSubResourceAttribute::operator!=(
    const MM::RenderSystem::MeshBufferSubResourceAttribute& rhs) const {
  return !(*this == rhs);
}

const MM::RenderSystem::BufferChunkInfo&
MM::RenderSystem::MeshBufferSubResourceAttribute::VertexGetChunkInfo() const {
  return vertex_buffer_sub_resource_attribute_.GetChunkInfo();
}

void MM::RenderSystem::MeshBufferSubResourceAttribute::VertexSetChunkInfo(
    const MM::RenderSystem::BufferChunkInfo& chunk_info) {
  vertex_buffer_sub_resource_attribute_.SetChunkInfo(chunk_info);
}

std::uint32_t
MM::RenderSystem::MeshBufferSubResourceAttribute::VertexGetQueueIndex() const {
  return vertex_buffer_sub_resource_attribute_.GetQueueIndex();
}

void MM::RenderSystem::MeshBufferSubResourceAttribute::VertexSetQueueIndex(
    uint32_t queue_index) {
  vertex_buffer_sub_resource_attribute_.SetQueueIndex(queue_index);
}

const MM::RenderSystem::BufferChunkInfo&
MM::RenderSystem::MeshBufferSubResourceAttribute::IndexGetChunkInfo() const {
  return index_buffer_sub_resource_attribute_.GetChunkInfo();
}

void MM::RenderSystem::MeshBufferSubResourceAttribute::IndexSetChunkInfo(
    const MM::RenderSystem::BufferChunkInfo& chunk_info) {
  index_buffer_sub_resource_attribute_.SetChunkInfo(chunk_info);
}

std::uint32_t
MM::RenderSystem::MeshBufferSubResourceAttribute::IndexGetQueueIndex() const {
  return index_buffer_sub_resource_attribute_.GetQueueIndex();
}

void MM::RenderSystem::MeshBufferSubResourceAttribute::IndexSetQueueIndex(
    uint32_t queue_index) {
  index_buffer_sub_resource_attribute_.SetQueueIndex(queue_index);
}

bool MM::RenderSystem::MeshBufferSubResourceAttribute::IsValid() const {
  return vertex_buffer_sub_resource_attribute_.IsValid() &&
         index_buffer_sub_resource_attribute_.IsValid();
}

void MM::RenderSystem::MeshBufferSubResourceAttribute::Reset() {
  vertex_buffer_sub_resource_attribute_.Reset();
  index_buffer_sub_resource_attribute_.Reset();
}

MM::RenderSystem::RenderPassCreateInfo::~RenderPassCreateInfo() {
  for (auto& subpasse : subpasses_) {
    delete subpasse.pInputAttachments;
    delete subpasse.pResolveAttachments;
    delete subpasse.pColorAttachments;
    delete subpasse.pDepthStencilAttachment;
    delete subpasse.pPreserveAttachments;
  }
}

MM::RenderSystem::RenderPassCreateInfo::RenderPassCreateInfo(
    const void* next, VkRenderPassCreateFlags flags,
    std::uint32_t attachments_count, const VkAttachmentDescription* attachments,
    std::uint32_t subpasses_count, const VkSubpassDescription* subpasses,
    std::uint32_t dependency_count, const VkSubpassDependency* dependencies)
    : next_(next),
      flags_(flags),
      attachments_(attachments_count),
      subpasses_(subpasses_count),
      dependencies_(dependency_count) {
  assert(attachments_count != 0 && attachments == nullptr);
  assert(subpasses_count != 0 && subpasses == nullptr);
  assert(dependency_count != 0 && dependencies == nullptr);
  for (std::uint32_t i = 0; i != attachments_.size(); ++i) {
    attachments_[i] = attachments[i];
  }

  for (std::uint32_t i = 0; i != subpasses_.size(); ++i) {
    subpasses_[i] = subpasses[i];

    VkAttachmentReference* input_attachment{nullptr};
    if (subpasses[i].inputAttachmentCount != 0) {
      assert(subpasses[i].pInputAttachments != nullptr);
      input_attachment =
          new VkAttachmentReference[subpasses[i].inputAttachmentCount]{};
      for (std::uint32_t j = 0; j != subpasses[i].inputAttachmentCount; ++j) {
        input_attachment[j] = subpasses[i].pInputAttachments[j];
      }
    }
    VkAttachmentReference* color_or_resolve_attachment{nullptr};
    if (subpasses[i].colorAttachmentCount != 0) {
      color_or_resolve_attachment =
          new VkAttachmentReference[subpasses[i].colorAttachmentCount]{};
      if (subpasses[i].pResolveAttachments) {
        for (std::uint32_t j = 0; j != subpasses[i].colorAttachmentCount; ++j) {
          color_or_resolve_attachment[j] = subpasses[i].pResolveAttachments[j];
        }
      } else if (subpasses[i].pColorAttachments) {
        for (std::uint32_t j = 0; j != subpasses[i].colorAttachmentCount; ++j) {
          color_or_resolve_attachment[j] = subpasses[i].pColorAttachments[j];
        }
      } else {
        assert(false);
      }
    }
    const VkAttachmentReference* depth_attachment{nullptr};
    if (subpasses[i].pDepthStencilAttachment) {
      depth_attachment =
          new VkAttachmentReference{*subpasses[i].pDepthStencilAttachment};
    }
    std::uint32_t* preserve_attachment{nullptr};
    if (subpasses[i].preserveAttachmentCount != 0) {
      assert(subpasses[i].pPreserveAttachments != nullptr);

      preserve_attachment =
          new std::uint32_t[subpasses[i].preserveAttachmentCount]{};

      for (std::uint32_t j = 0; j != subpasses[i].preserveAttachmentCount;
           ++j) {
        preserve_attachment[j] = subpasses[i].pPreserveAttachments[j];
      }
    }
  }

  for (std::uint32_t i = 0; i != dependencies_.size(); ++i) {
    dependencies_[i] = dependencies[i];
  }
}

MM::RenderSystem::RenderPassCreateInfo::RenderPassCreateInfo(
    const VkRenderPassCreateInfo& vk_render_pass_create_info)
    : next_(vk_render_pass_create_info.pNext),
      flags_(vk_render_pass_create_info.flags),
      attachments_(vk_render_pass_create_info.attachmentCount),
      subpasses_(vk_render_pass_create_info.subpassCount),
      dependencies_(vk_render_pass_create_info.dependencyCount) {
  assert(vk_render_pass_create_info.attachmentCount != 0 &&
         vk_render_pass_create_info.pAttachments == nullptr);
  assert(vk_render_pass_create_info.subpassCount != 0 &&
         vk_render_pass_create_info.pSubpasses == nullptr);
  assert(vk_render_pass_create_info.dependencyCount != 0 &&
         vk_render_pass_create_info.pDependencies == nullptr);
  for (std::uint32_t i = 0; i != attachments_.size(); ++i) {
    attachments_[i] = vk_render_pass_create_info.pAttachments[i];
  }

  for (std::uint32_t i = 0; i != subpasses_.size(); ++i) {
    subpasses_[i] = vk_render_pass_create_info.pSubpasses[i];

    VkAttachmentReference* input_attachment{nullptr};
    if (vk_render_pass_create_info.pSubpasses[i].inputAttachmentCount != 0) {
      assert(vk_render_pass_create_info.pSubpasses[i].pInputAttachments !=
             nullptr);
      input_attachment =
          new VkAttachmentReference[vk_render_pass_create_info.pSubpasses[i]
                                        .inputAttachmentCount]{};
      for (std::uint32_t j = 0;
           j != vk_render_pass_create_info.pSubpasses[i].inputAttachmentCount;
           ++j) {
        input_attachment[j] =
            vk_render_pass_create_info.pSubpasses[i].pInputAttachments[j];
      }
    }
    VkAttachmentReference* color_or_resolve_attachment{nullptr};
    if (vk_render_pass_create_info.pSubpasses[i].colorAttachmentCount != 0) {
      color_or_resolve_attachment =
          new VkAttachmentReference[vk_render_pass_create_info.pSubpasses[i]
                                        .colorAttachmentCount]{};
      if (vk_render_pass_create_info.pSubpasses[i].pResolveAttachments !=
          nullptr) {
        for (std::uint32_t j = 0;
             j != vk_render_pass_create_info.pSubpasses[i].colorAttachmentCount;
             ++j) {
          color_or_resolve_attachment[j] =
              vk_render_pass_create_info.pSubpasses[i].pResolveAttachments[j];
        }
      } else if (vk_render_pass_create_info.pSubpasses[i].pColorAttachments !=
                 nullptr) {
        for (std::uint32_t j = 0;
             j != vk_render_pass_create_info.pSubpasses[i].colorAttachmentCount;
             ++j) {
          color_or_resolve_attachment[j] =
              vk_render_pass_create_info.pSubpasses[i].pColorAttachments[j];
        }
      } else {
        assert(false);
      }
    }
    const VkAttachmentReference* depth_attachment{nullptr};
    if (vk_render_pass_create_info.pSubpasses[i].pDepthStencilAttachment) {
      depth_attachment = new VkAttachmentReference{
          *vk_render_pass_create_info.pSubpasses[i].pDepthStencilAttachment};
    }
    std::uint32_t* preserve_attachment{nullptr};
    if (vk_render_pass_create_info.pSubpasses[i].preserveAttachmentCount != 0) {
      assert(vk_render_pass_create_info.pSubpasses[i].pPreserveAttachments !=
             nullptr);

      preserve_attachment =
          new std::uint32_t[vk_render_pass_create_info.pSubpasses[i]
                                .preserveAttachmentCount]{};

      for (std::uint32_t j = 0;
           j !=
           vk_render_pass_create_info.pSubpasses[i].preserveAttachmentCount;
           ++j) {
        preserve_attachment[j] =
            vk_render_pass_create_info.pSubpasses[i].pPreserveAttachments[j];
      }
    }
  }

  for (std::uint32_t i = 0; i != dependencies_.size(); ++i) {
    dependencies_[i] = vk_render_pass_create_info.pDependencies[i];
  }
}

MM::RenderSystem::RenderPassCreateInfo::RenderPassCreateInfo(
    MM::RenderSystem::RenderPassCreateInfo&& other) noexcept
    : next_(other.next_),
      flags_(other.flags_),
      attachments_(std::move(other.attachments_)),
      subpasses_(),
      dependencies_(std::move(other.dependencies_)) {
  subpasses_ = std::move(other.subpasses_);

  other.next_ = nullptr;
  other.flags_ = VK_RENDER_PASS_CREATE_FLAG_BITS_MAX_ENUM;
}

MM::RenderSystem::RenderPassCreateInfo&
MM::RenderSystem::RenderPassCreateInfo::operator=(
    const MM::RenderSystem::RenderPassCreateInfo& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  next_ = other.next_;
  flags_ = other.flags_;
  attachments_ = other.attachments_;
  subpasses_ = other.subpasses_;
  dependencies_ = other.dependencies_;

  return *this;
}

MM::RenderSystem::RenderPassCreateInfo&
MM::RenderSystem::RenderPassCreateInfo::operator=(
    MM::RenderSystem::RenderPassCreateInfo&& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  Reset();

  next_ = other.next_;
  flags_ = other.flags_;
  attachments_ = std::move(other.attachments_);
  subpasses_ = std::move(other.subpasses_);
  dependencies_ = std::move(other.dependencies_);

  other.next_ = nullptr;
  other.flags_ = VK_RENDER_PASS_CREATE_FLAG_BITS_MAX_ENUM;

  for (std::uint64_t i = 0; i != other.subpasses_.size(); ++i) {
    if (other.subpasses_[i].pInputAttachments) {
      VkAttachmentReference* input_attachments =
          new VkAttachmentReference[other.subpasses_[i].inputAttachmentCount]{};
      for (std::uint32_t j = 0; j != other.subpasses_[i].inputAttachmentCount;
           ++j) {
        input_attachments[j] = other.subpasses_[i].pInputAttachments[j];
      }

      subpasses_[i].pInputAttachments = input_attachments;
    }

    if (other.subpasses_[i].pColorAttachments) {
      VkAttachmentReference* color_attachments =
          new VkAttachmentReference[other.subpasses_[i].colorAttachmentCount]{};
      if (other.subpasses_[i].pResolveAttachments) {
        for (std::uint32_t j = 0; j != other.subpasses_[i].colorAttachmentCount;
             ++j) {
          color_attachments[j] = other.subpasses_[i].pResolveAttachments[j];
        }

        subpasses_[i].pResolveAttachments = color_attachments;
      } else if (other.subpasses_[i].pColorAttachments) {
        for (std::uint32_t j = 0; j != other.subpasses_[i].colorAttachmentCount;
             ++j) {
          color_attachments[j] = other.subpasses_[i].pColorAttachments[j];
        }

        subpasses_[i].pColorAttachments = color_attachments;
      } else {
        assert(false);
      }
    }

    if (other.subpasses_[i].pDepthStencilAttachment) {
      VkAttachmentReference* depth_attachment = new VkAttachmentReference{
          *other.subpasses_[i].pDepthStencilAttachment};
    }

    if (other.subpasses_[i].pPreserveAttachments) {
      std::uint32_t* preserve_attachments =
          new std::uint32_t[other.subpasses_[i].preserveAttachmentCount]{};

      for (std::uint32_t j = 0;
           j != other.subpasses_[i].preserveAttachmentCount; ++j) {
        preserve_attachments[j] = other.subpasses_[i].pPreserveAttachments[j];
      }

      subpasses_[i].pPreserveAttachments = preserve_attachments;
    }
  }

  return *this;
}

bool MM::RenderSystem::RenderPassCreateInfo::IsValid() const {
  return !subpasses_.empty();
}

void MM::RenderSystem::RenderPassCreateInfo::Reset() {
  next_ = nullptr;
  flags_ = VK_RENDER_PASS_CREATE_FLAG_BITS_MAX_ENUM;
  attachments_.clear();
  for (auto& subpasse : subpasses_) {
    delete subpasse.pInputAttachments;
    delete subpasse.pResolveAttachments;
    delete subpasse.pColorAttachments;
    delete subpasse.pDepthStencilAttachment;
    delete subpasse.pPreserveAttachments;
  }
  subpasses_.clear();
  dependencies_.clear();
}

VkRenderPassCreateInfo
MM::RenderSystem::RenderPassCreateInfo::GetVkRenderPassCreateInfo() const {
  return VkRenderPassCreateInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                                next_,
                                flags_,
                                static_cast<uint32_t>(attachments_.size()),
                                attachments_.data(),
                                static_cast<uint32_t>(subpasses_.size()),
                                subpasses_.data(),
                                static_cast<uint32_t>(dependencies_.size()),
                                dependencies_.data()};
}

MM::RenderSystem::RenderPassCreateInfo::RenderPassCreateInfo(
    const MM::RenderSystem::RenderPassCreateInfo& other)
    : next_(other.next_),
      flags_(other.flags_),
      attachments_(other.attachments_),
      subpasses_(other.subpasses_),
      dependencies_(other.dependencies_) {
  for (std::uint64_t i = 0; i != other.subpasses_.size(); ++i) {
    if (other.subpasses_[i].pInputAttachments) {
      VkAttachmentReference* input_attachments =
          new VkAttachmentReference[other.subpasses_[i].inputAttachmentCount]{};
      for (std::uint32_t j = 0; j != other.subpasses_[i].inputAttachmentCount;
           ++j) {
        input_attachments[j] = other.subpasses_[i].pInputAttachments[j];
      }

      subpasses_[i].pInputAttachments = input_attachments;
    }

    if (other.subpasses_[i].pColorAttachments) {
      VkAttachmentReference* color_attachments =
          new VkAttachmentReference[other.subpasses_[i].colorAttachmentCount]{};
      if (other.subpasses_[i].pResolveAttachments) {
        for (std::uint32_t j = 0; j != other.subpasses_[i].colorAttachmentCount;
             ++j) {
          color_attachments[j] = other.subpasses_[i].pResolveAttachments[j];
        }

        subpasses_[i].pResolveAttachments = color_attachments;
      } else if (other.subpasses_[i].pColorAttachments) {
        for (std::uint32_t j = 0; j != other.subpasses_[i].colorAttachmentCount;
             ++j) {
          color_attachments[j] = other.subpasses_[i].pColorAttachments[j];
        }

        subpasses_[i].pColorAttachments = color_attachments;
      } else {
        assert(false);
      }
    }

    if (other.subpasses_[i].pDepthStencilAttachment) {
      VkAttachmentReference* depth_attachment = new VkAttachmentReference{
          *other.subpasses_[i].pDepthStencilAttachment};
    }

    if (other.subpasses_[i].pPreserveAttachments) {
      std::uint32_t* preserve_attachments =
          new std::uint32_t[other.subpasses_[i].preserveAttachmentCount]{};

      for (std::uint32_t j = 0;
           j != other.subpasses_[i].preserveAttachmentCount; ++j) {
        preserve_attachments[j] = other.subpasses_[i].pPreserveAttachments[j];
      }

      subpasses_[i].pPreserveAttachments = preserve_attachments;
    }
  }
}

MM::RenderSystem::FrameBufferCreateInfo::FrameBufferCreateInfo(
    const void* next, VkFramebufferCreateFlags flags, VkRenderPass render_pass,
    std::uint32_t attachments_count, const VkImageView* attachments,
    uint32_t width, uint32_t height, uint32_t layers)
    : next_(next),
      flags_(flags),
      render_pass_(render_pass),
      attachments_(attachments_count),
      width_(width),
      height_(height),
      layers_(layers) {
  if (!attachments_.empty()) {
    assert(attachments != nullptr);
    for (std::uint32_t i = 0; i != attachments_count; ++i) {
      attachments_[i] = attachments[i];
    }
  }
}

MM::RenderSystem::FrameBufferCreateInfo::FrameBufferCreateInfo(
    const VkFramebufferCreateInfo& frame_buffer_create_info)

    : next_(frame_buffer_create_info.pNext),
      flags_(frame_buffer_create_info.flags),
      render_pass_(frame_buffer_create_info.renderPass),
      attachments_(frame_buffer_create_info.attachmentCount),
      width_(frame_buffer_create_info.width),
      height_(frame_buffer_create_info.height),
      layers_(frame_buffer_create_info.layers) {
  if (!attachments_.empty()) {
    assert(frame_buffer_create_info.pAttachments != nullptr);
    for (std::uint32_t i = 0; i != frame_buffer_create_info.attachmentCount;
         ++i) {
      attachments_[i] = frame_buffer_create_info.pAttachments[i];
    }
  }
}

MM::RenderSystem::FrameBufferCreateInfo::FrameBufferCreateInfo(
    MM::RenderSystem::FrameBufferCreateInfo&& other) noexcept
    : next_(other.next_),
      flags_(other.flags_),
      render_pass_(other.render_pass_),
      attachments_(std::move(other.attachments_)),
      width_(other.width_),
      height_(other.height_),
      layers_(other.layers_) {
  other.Reset();
}

MM::RenderSystem::FrameBufferCreateInfo&
MM::RenderSystem::FrameBufferCreateInfo::operator=(
    const MM::RenderSystem::FrameBufferCreateInfo& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  next_ = other.next_;
  flags_ = other.flags_;
  render_pass_ = other.render_pass_;
  attachments_ = other.attachments_;
  width_ = other.width_;
  height_ = other.height_;
  layers_ = other.layers_;

  return *this;
}

MM::RenderSystem::FrameBufferCreateInfo&
MM::RenderSystem::FrameBufferCreateInfo::operator=(
    MM::RenderSystem::FrameBufferCreateInfo&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  next_ = other.next_;
  flags_ = other.flags_;
  render_pass_ = other.render_pass_;
  attachments_ = std::move(other.attachments_);
  width_ = other.width_;
  height_ = other.height_;
  layers_ = other.layers_;

  other.Reset();

  return *this;
}

bool MM::RenderSystem::FrameBufferCreateInfo::IsValid() const {
  return render_pass_ != nullptr;
}

void MM::RenderSystem::FrameBufferCreateInfo::Reset() {
  next_ = nullptr;
  flags_ = 0x7FFFFFFF;
  render_pass_ = nullptr;
  attachments_.clear();
  width_ = 0;
  height_ = 0;
  layers_ = 0;
}

VkFramebufferCreateInfo
MM::RenderSystem::FrameBufferCreateInfo::GetVkFrameBufferCreateInfo() const {
  return VkFramebufferCreateInfo{
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      next_,
      flags_,
      render_pass_,
      static_cast<std::uint32_t>(attachments_.size()),
      attachments_.data(),
      width_,
      height_,
      layers_};
}
