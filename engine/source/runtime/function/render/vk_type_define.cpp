//
// Created by beimingxianyu on 23-6-29.
//
#include "runtime/function/render/vk_type_define.h"

#include "runtime/function/render/vk_engine.h"
#include "vk_utils.h"

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

MM::RenderSystem::ImageCreateInfo::ImageCreateInfo(
    uint64_t image_size, const void* next, VkImageCreateFlags flags,
    VkImageType image_type, VkFormat format, const VkExtent3D& extent,
    uint32_t miplevels, uint32_t array_levels, VkSampleCountFlags samples,
    VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharing_mode,
    const std::vector<std::uint32_t>& queue_family_indices,
    VkImageLayout initial_layout)
    : image_size_(image_size),
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
    std::uint64_t image_size, const VkImageCreateInfo& vk_image_create_info)
    : image_size_(image_size),
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
  return image_create_info_.IsValid() && allocation_create_info_.IsValid();
}

void MM::RenderSystem::ImageDataInfo::Reset() {
  image_create_info_.Reset();
  allocation_create_info_.Reset();
}

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

  render_image_view_attribute_ID.SetAttribute1(attribute1);
  render_image_view_attribute_ID.SetAttribute2(attribute2);

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

  render_sampler_attribute_ID.SetAttribute1(attribute1);
  render_sampler_attribute_ID.SetAttribute2(attribute2);

  return ExecuteResult ::SUCCESS;
}

MM::Utils::ConcurrentMap<MM::RenderSystem::RenderImageViewAttributeID,
                         MM::RenderSystem::ImageView::ImageViewWrapper>
    MM::RenderSystem::ImageView::image_view_container_{512};

MM::RenderSystem::ImageView::ImageView(
    VkDevice device, VkAllocationCallbacks* allocator,
    const VkImageViewCreateInfo& vk_image_view_create_info)
    : image_view_wrapper_(nullptr),
      image_view_create_info_(vk_image_view_create_info) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, vk_image_view_create_info),
           image_view_create_info_.Reset();
           return;)
#endif
  RenderImageViewAttributeID render_image_view_attribute_ID{};
  MM_CHECK(image_view_create_info_.GetRenderImageViewAttributeID(
               render_image_view_attribute_ID),
           image_view_create_info_.Reset();
           return;)

  auto* find_resource =
      image_view_container_.Find(render_image_view_attribute_ID);
  if (find_resource) {
    image_view_wrapper_ = &find_resource->second;
  }

  VkImageView image_view;
  VK_CHECK(vkCreateImageView(device, &vk_image_view_create_info, allocator,
                             &image_view),
           LOG_ERROR("Failed to create MM::Render::ImageView.");
           image_view_create_info_.Reset(); return;)

  auto insert_result = image_view_container_.Emplace(
      std::make_pair(render_image_view_attribute_ID,
                     ImageViewWrapper{device, allocator, image_view}));

  if (!insert_result.second) {
    image_view_create_info_.Reset();
    vkDestroyImageView(device, image_view, allocator);
    LOG_ERROR("Image view insert error.");
    return;
  }

  image_view_wrapper_ = &insert_result.first.second;
}

MM::RenderSystem::ImageView::ImageView(
    MM::RenderSystem::ImageView&& other) noexcept
    : image_view_wrapper_(other.image_view_wrapper_),
      image_view_create_info_(std::move(other.image_view_create_info_)) {
  other.image_view_wrapper_ = nullptr;
}

MM::RenderSystem::ImageView& MM::RenderSystem::ImageView::operator=(
    MM::RenderSystem::ImageView&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  image_view_wrapper_ = other.image_view_wrapper_;
  image_view_create_info_ = std::move(other.image_view_create_info_);

  other.image_view_wrapper_ = nullptr;

  return *this;
}

const VkDevice_T* MM::RenderSystem::ImageView::GetDevice() const {
  return image_view_wrapper_->device_;
}

const VkAllocationCallbacks* MM::RenderSystem::ImageView::GetAllocator() const {
  return image_view_wrapper_->allocator_;
}

const VkImageView_T* MM::RenderSystem::ImageView::GetVkImageView() const {
  return image_view_wrapper_->image_view_;
}

const MM::RenderSystem::ImageViewCreateInfo&
MM::RenderSystem::ImageView::GetImageViewCreateInfo() const {
  return image_view_create_info_;
}

bool MM::RenderSystem::ImageView::IsValid() const {
  return image_view_wrapper_->IsValid() && image_view_create_info_.IsValid();
}

void MM::RenderSystem::ImageView::Reset() {
  image_view_create_info_.Reset();
  image_view_wrapper_ = nullptr;
}

MM::ExecuteResult MM::RenderSystem::ImageView::CheckInitParameters(
    VkDevice device, const VkImageViewCreateInfo& vk_image_view_create_info) {
  if (device == nullptr) {
    LOG_ERROR("The incoming engine parameter pointer is null.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (vk_image_view_create_info.image == nullptr) {
    LOG_ERROR("The incoming engine parameter pointer is null.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM) {
    LOG_ERROR("The image view type is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.flags ==
      VK_IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM) {
    LOG_ERROR("The image view create flags is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.format == VK_FORMAT_MAX_ENUM) {
    LOG_ERROR("The image view format is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_image_view_create_info.components.r == VK_COMPONENT_SWIZZLE_MAX_ENUM ||
      vk_image_view_create_info.components.g == VK_COMPONENT_SWIZZLE_MAX_ENUM ||
      vk_image_view_create_info.components.b == VK_COMPONENT_SWIZZLE_MAX_ENUM ||
      vk_image_view_create_info.components.a == VK_COMPONENT_SWIZZLE_MAX_ENUM) {
    LOG_ERROR("The image view components is error.");
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
    LOG_ERROR("The image view subresource range is error");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

VkDevice MM::RenderSystem::ImageView::GetDevice() {
  return image_view_wrapper_->device_;
}

VkAllocationCallbacks* MM::RenderSystem::ImageView::GetAllocator() {
  return image_view_wrapper_->allocator_;
}

VkImageView MM::RenderSystem::ImageView::GetVkImageView() {
  return image_view_wrapper_->image_view_;
}

MM::Utils::ConcurrentMap<MM::RenderSystem::RenderSamplerAttributeID,
                         MM::RenderSystem::Sampler::SamplerWrapper>
    MM::RenderSystem::Sampler::sampler_container_{512};

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

  auto* find_resource = sampler_container_.Find(render_sampler_attribute_ID);
  if (find_resource) {
    sampler_wrapper_ = &find_resource->second;
  }

  VkSampler sampler;
  VK_CHECK(
      vkCreateSampler(device, &vk_sampler_create_info, allocator, &sampler),
      LOG_ERROR("Failed to create MM::Render::Sampler.");
      sampler_create_info_.Reset(); return;)

  auto insert_result = sampler_container_.Emplace(std::make_pair(
      render_sampler_attribute_ID, SamplerWrapper{device, allocator, sampler}));

  if (!insert_result.second) {
    sampler_create_info_.Reset();
    vkDestroySampler(device, sampler, allocator);
    LOG_ERROR("Sampler insert error.");
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
    LOG_ERROR("The incoming engine parameter pointer is null.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (vk_sampler_create_info.flags == VK_SAMPLER_CREATE_FLAG_BITS_MAX_ENUM) {
    LOG_ERROR("The sampler create flags is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.magFilter == VK_FILTER_MAX_ENUM ||
      vk_sampler_create_info.minFilter == VK_FILTER_MAX_ENUM) {
    LOG_ERROR("The sampler filter is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.mipmapMode == VK_SAMPLER_MIPMAP_MODE_MAX_ENUM) {
    LOG_ERROR("The sampler mipmap mode is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.addressModeU == VK_SAMPLER_ADDRESS_MODE_MAX_ENUM ||
      vk_sampler_create_info.addressModeV == VK_SAMPLER_ADDRESS_MODE_MAX_ENUM ||
      vk_sampler_create_info.addressModeW == VK_SAMPLER_ADDRESS_MODE_MAX_ENUM) {
    LOG_ERROR("The sampler address mode is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.mipLodBias < 0 ||
      vk_sampler_create_info.mipLodBias > 1) {
    LOG_ERROR("The sampler mip lod bias is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.anisotropyEnable == VK_TRUE &&
      vk_sampler_create_info.maxAnisotropy < 1) {
    LOG_ERROR("The sampler max anisotropy is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.compareOp == VK_COMPARE_OP_MAX_ENUM) {
    LOG_ERROR("The sampler compare operator is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.minLod < 0 || vk_sampler_create_info.minLod > 1 ||
      vk_sampler_create_info.maxLod < 0 || vk_sampler_create_info.maxLod > 1 ||
      vk_sampler_create_info.maxLod < vk_sampler_create_info.minLod) {
    LOG_ERROR("The sampler min/max lod is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (vk_sampler_create_info.borderColor == VK_BORDER_COLOR_MAX_ENUM) {
    LOG_ERROR("The sampler border color is error.");
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
    const VkDescriptorSetLayoutBinding& bind, VkDevice device,
    VkAllocationCallbacks* image_view_allocator,
    const VkImageViewCreateInfo& vk_image_view_create_info,
    VkAllocationCallbacks* sampler_allocator,
    const VkSamplerCreateInfo& vk_sampler_create_info)
    : bind_(bind),
      image_view_(device, image_view_allocator, vk_image_view_create_info),
      sampler_(device, sampler_allocator, vk_sampler_create_info) {
  if (!bind_.IsValid() && !image_view_.IsValid() && !sampler_.IsValid()) {
    bind_.Reset();
    image_view_.Reset();
    sampler_.Reset();
  }
}

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

void MM::RenderSystem::ImageBindData::Reset() {
  bind_.Reset();
  image_view_.Reset();
  sampler_.Reset();
}
