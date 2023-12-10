//
// Created by beimingxianyu on 23-7-27.
//

#include "runtime/function/render/DescriptorManager.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::DescriptorManager::DescriptorSlot::DescriptorSlot(
    DescriptorSlot&& other) noexcept
    : slot_index_(other.slot_index_),
      descriptor_type_(other.descriptor_type_),
      is_global_(other.is_global_) {
  other.slot_index_ = UINT32_MAX;
  other.descriptor_type_ = DescriptorType::UNDEFINED;
  other.is_global_ = false;
}

MM::RenderSystem::DescriptorManager::DescriptorSlot&
MM::RenderSystem::DescriptorManager::DescriptorSlot::operator=(
    DescriptorSlot&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  assert(!IsValid());

  slot_index_ = other.slot_index_;
  descriptor_type_ = other.descriptor_type_;
  is_global_ = other.is_global_;

  other.slot_index_ = UINT32_MAX;
  other.descriptor_type_ = DescriptorType::UNDEFINED;
  other.is_global_ = false;

  return *this;
}

std::uint32_t
MM::RenderSystem::DescriptorManager::DescriptorSlot::GetSlotIndex() const {
  return slot_index_;
}

MM::RenderSystem::DescriptorType
MM::RenderSystem::DescriptorManager::DescriptorSlot::GetDescriptorType() const {
  return descriptor_type_;
}

bool MM::RenderSystem::DescriptorManager::DescriptorSlot::GetIsGlobal() const {
  return is_global_;
}

bool MM::RenderSystem::DescriptorManager::DescriptorSlot::IsValid() const {
  return descriptor_type_ != DescriptorType::UNDEFINED;
}

MM::RenderSystem::DescriptorManager::DescriptorSlot::DescriptorSlot(
    std::uint32_t slot_index, DescriptorType descriptor_type, bool is_global)
    : slot_index_(slot_index),
      descriptor_type_(descriptor_type),
      is_global_(is_global) {}

MM::RenderSystem::DescriptorManager::~DescriptorManager() { Release(); }

MM::RenderSystem::DescriptorManager::DescriptorManager(
    RenderEngine* render_engine,
    VkAllocationCallbacks* allocator,
    std::uint32_t one_descriptor_element_count)
    : render_engine_(render_engine),
      allocator_(allocator),
      descriptor_pool_(),
      global_set_layout_(),
      material_set_layout_(),
      global_set_(),
      material_set_(),
      global_sampler_texture2D_array_use_info_(),
      global_sampler_texture3D_array_use_info_(),
      global_sampler_textureCUBE_array_use_info_(),
      global_storage_texture2D_array_use_info_(),
      global_storage_texture3D_array_use_info_(),
      global_storage_textureCUBE_array_use_info_(),
      sampler_texture2D_array_use_info_(),
      sampler_texture3D_array_use_info_(),
      sampler_textureCUBE_array_use_info_(),
      storage_texture2D_array_use_info_(),
      storage_texture3D_array_use_info_(),
      storage_textureCUBE_array_use_info_(),
      global_sampler_texture2D_last_use_index_(),
      global_sampler_texture3D_last_use_index_(),
      global_sampler_textureCUBE_last_use_index_(),
      global_storage_texture2D_last_use_index_(),
      global_storage_texture3D_last_use_index_(),
      global_storage_textureCUBE_last_use_index_(),
      sampler_texture2D_last_use_index_(),
      sampler_texture3D_last_use_index_(),
      sampler_textureCUBE_last_use_index_(),
      storage_texture2D_last_use_index_(),
      storage_texture3D_last_use_index_(),
      storage_textureCUBE_last_use_index_(),
      pipeline_layout0_(nullptr),
      pipeline_layout1_(nullptr),
      pipeline_layout2_(nullptr),
      pipeline_layout3_(nullptr),
      pipeline_layout4_(nullptr),
      pipeline_layout5_(nullptr),
      pipeline_layout6_(nullptr),
      pipeline_layout7_(nullptr),
      pipeline_layout8_(nullptr),
      pipeline_layout9_(nullptr),
      pipeline_layout10_(nullptr),
      pipeline_layout11_(nullptr),
      pipeline_layout12_(nullptr),
      pipeline_layout13_(nullptr),
      pipeline_layout14_(nullptr),
      pipeline_layout15_(nullptr),
      pipeline_layout16_(nullptr) {
  if (render_engine_ == nullptr || !render_engine_->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is invalid.");
    return;
  }

  if (auto if_result = InitDescriptorManager(
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count);
               if_result.Exception(MM_ERROR_DESCRIPTION2("Failed initialization descriptor manager.")).IsError()) {
    return;
  }
}

MM::RenderSystem::DescriptorManager::DescriptorManager(
    RenderEngine* render_engine,
    VkAllocationCallbacks* allocator,
    std::uint32_t global_sampler_texture2D_descriptor_element_count,
    std::uint32_t global_sampler_texture3D_descriptor_element_count,
    std::uint32_t global_sampler_textureCUBE_descriptor_element_count,
    std::uint32_t global_storage_texture2D_descriptor_element_count,
    std::uint32_t global_storage_texture3D_descriptor_element_count,
    std::uint32_t global_storage_textureCUBE_descriptor_element_count,
    std::uint32_t sampler_texture2D_descriptor_element_count,
    std::uint32_t sampler_texture3D_descriptor_element_count,
    std::uint32_t sampler_textureCUBE_descriptor_element_count,
    std::uint32_t storage_texture2D_descriptor_element_count,
    std::uint32_t storage_texture3D_descriptor_element_count,
    std::uint32_t storage_textureCUBE_descriptor_element_count)
    : render_engine_(render_engine),
      allocator_(allocator),
      descriptor_pool_(),
      global_set_layout_(),
      material_set_layout_(),
      global_set_(),
      material_set_(),
      global_sampler_texture2D_array_use_info_(),
      global_sampler_texture3D_array_use_info_(),
      global_sampler_textureCUBE_array_use_info_(),
      global_storage_texture2D_array_use_info_(),
      global_storage_texture3D_array_use_info_(),
      global_storage_textureCUBE_array_use_info_(),
      sampler_texture2D_array_use_info_(),
      sampler_texture3D_array_use_info_(),
      sampler_textureCUBE_array_use_info_(),
      storage_texture2D_array_use_info_(),
      storage_texture3D_array_use_info_(),
      storage_textureCUBE_array_use_info_(),
      global_sampler_texture2D_last_use_index_(),
      global_sampler_texture3D_last_use_index_(),
      global_sampler_textureCUBE_last_use_index_(),
      global_storage_texture2D_last_use_index_(),
      global_storage_texture3D_last_use_index_(),
      global_storage_textureCUBE_last_use_index_(),
      sampler_texture2D_last_use_index_(),
      sampler_texture3D_last_use_index_(),
      sampler_textureCUBE_last_use_index_(),
      storage_texture2D_last_use_index_(),
      storage_texture3D_last_use_index_(),
      storage_textureCUBE_last_use_index_(),
      pipeline_layout0_(nullptr),
      pipeline_layout1_(nullptr),
      pipeline_layout2_(nullptr),
      pipeline_layout3_(nullptr),
      pipeline_layout4_(nullptr),
      pipeline_layout5_(nullptr),
      pipeline_layout6_(nullptr),
      pipeline_layout7_(nullptr),
      pipeline_layout8_(nullptr),
      pipeline_layout9_(nullptr),
      pipeline_layout10_(nullptr),
      pipeline_layout11_(nullptr),
      pipeline_layout12_(nullptr),
      pipeline_layout13_(nullptr),
      pipeline_layout14_(nullptr),
      pipeline_layout15_(nullptr),
      pipeline_layout16_(nullptr) {
  if (render_engine_ == nullptr || !render_engine_->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is invalid.");
    return;
  }

  if (auto if_result =
      InitDescriptorManager(global_sampler_texture2D_descriptor_element_count,
                            global_sampler_texture3D_descriptor_element_count,
                            global_sampler_textureCUBE_descriptor_element_count,
                            global_storage_texture2D_descriptor_element_count,
                            global_storage_texture3D_descriptor_element_count,
                            global_storage_textureCUBE_descriptor_element_count,
                            sampler_texture2D_descriptor_element_count,
                            sampler_texture3D_descriptor_element_count,
                            sampler_textureCUBE_descriptor_element_count,
                            storage_texture2D_descriptor_element_count,
                            storage_texture3D_descriptor_element_count,
                            storage_textureCUBE_descriptor_element_count);
                            if_result.Exception(MM_ERROR_DESCRIPTION2("Failed initialization descriptor manager.")).IsError()) {
    return;
  }
}

MM::RenderSystem::DescriptorManager::DescriptorManager(
    DescriptorManager&& other) noexcept
    : render_engine_(other.render_engine_),
      allocator_(other.allocator_),
      descriptor_pool_(other.descriptor_pool_),
      global_set_layout_(other.global_set_layout_),
      material_set_layout_(other.material_set_layout_),
      global_set_(other.global_set_),
      material_set_(other.material_set_),
      global_sampler_texture2D_array_use_info_(
          std::move(other.global_sampler_texture2D_array_use_info_)),
      global_sampler_texture3D_array_use_info_(
          std::move(other.global_sampler_texture3D_array_use_info_)),
      global_sampler_textureCUBE_array_use_info_(
          std::move(other.global_sampler_textureCUBE_array_use_info_)),
      global_storage_texture2D_array_use_info_(
          std::move(other.global_storage_texture2D_array_use_info_)),
      global_storage_texture3D_array_use_info_(
          std::move(other.global_storage_texture3D_array_use_info_)),
      global_storage_textureCUBE_array_use_info_(
          std::move(other.global_storage_textureCUBE_array_use_info_)),
      sampler_texture2D_array_use_info_(
          std::move(other.sampler_texture2D_array_use_info_)),
      sampler_texture3D_array_use_info_(
          std::move(other.sampler_texture3D_array_use_info_)),
      sampler_textureCUBE_array_use_info_(
          std::move(other.sampler_textureCUBE_array_use_info_)),
      storage_texture2D_array_use_info_(
          std::move(other.storage_texture2D_array_use_info_)),
      storage_texture3D_array_use_info_(
          std::move(other.storage_texture3D_array_use_info_)),
      storage_textureCUBE_array_use_info_(
          std::move(other.storage_textureCUBE_array_use_info_)),
      global_sampler_texture2D_last_use_index_(
          other.global_sampler_texture2D_last_use_index_.load(
              std::memory_order_acquire)),
      global_sampler_texture3D_last_use_index_(
          other.global_sampler_texture3D_last_use_index_.load(
              std::memory_order_acquire)),
      global_sampler_textureCUBE_last_use_index_(
          other.global_sampler_textureCUBE_last_use_index_.load(
              std::memory_order_acquire)),
      global_storage_texture2D_last_use_index_(
          other.global_storage_texture2D_last_use_index_.load(
              std::memory_order_acquire)),
      global_storage_texture3D_last_use_index_(
          other.global_storage_texture3D_last_use_index_.load(
              std::memory_order_acquire)),
      global_storage_textureCUBE_last_use_index_(
          other.global_storage_textureCUBE_last_use_index_.load(
              std::memory_order_acquire)),
      sampler_texture2D_last_use_index_(
          other.sampler_texture2D_last_use_index_.load(
              std::memory_order_acquire)),
      sampler_texture3D_last_use_index_(
          other.sampler_texture3D_last_use_index_.load(
              std::memory_order_acquire)),
      sampler_textureCUBE_last_use_index_(
          other.sampler_textureCUBE_last_use_index_.load(
              std::memory_order_acquire)),
      storage_texture2D_last_use_index_(
          other.storage_texture2D_last_use_index_.load(
              std::memory_order_acquire)),
      storage_texture3D_last_use_index_(
          other.storage_texture3D_last_use_index_.load(
              std::memory_order_acquire)),
      storage_textureCUBE_last_use_index_(
          other.storage_textureCUBE_last_use_index_.load(
              std::memory_order_acquire)) {
  other.render_engine_ = nullptr;
  other.allocator_ = nullptr;
  other.descriptor_pool_ = nullptr;
  other.global_set_layout_ = nullptr;
  other.material_set_layout_ = nullptr;
  other.global_set_ = nullptr;
  other.material_set_ = nullptr;
}

MM::RenderSystem::DescriptorManager&
MM::RenderSystem::DescriptorManager::operator=(
    DescriptorManager&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  Release();

  render_engine_ = other.render_engine_;
  allocator_ = other.allocator_;
  descriptor_pool_ = other.descriptor_pool_;
  global_set_layout_ = other.global_set_layout_;
  material_set_layout_ = other.material_set_layout_;
  global_set_ = other.global_set_;
  material_set_ = other.material_set_;
  global_sampler_texture2D_array_use_info_ =
      std::move(other.global_sampler_texture2D_array_use_info_);
  global_sampler_texture3D_array_use_info_ =
      std::move(other.global_sampler_texture3D_array_use_info_);
  global_sampler_textureCUBE_array_use_info_ =
      std::move(other.global_sampler_textureCUBE_array_use_info_);
  global_storage_texture2D_array_use_info_ =
      std::move(other.global_storage_texture2D_array_use_info_);
  global_storage_texture3D_array_use_info_ =
      std::move(other.global_storage_texture3D_array_use_info_);
  global_storage_textureCUBE_array_use_info_ =
      std::move(other.global_storage_textureCUBE_array_use_info_);
  sampler_texture2D_array_use_info_ =
      std::move(other.sampler_texture2D_array_use_info_);
  sampler_texture3D_array_use_info_ =
      std::move(other.sampler_texture3D_array_use_info_);
  sampler_textureCUBE_array_use_info_ =
      std::move(other.sampler_textureCUBE_array_use_info_);
  storage_texture2D_array_use_info_ =
      std::move(other.storage_texture2D_array_use_info_);
  storage_texture3D_array_use_info_ =
      std::move(other.storage_texture3D_array_use_info_);
  storage_textureCUBE_array_use_info_ =
      std::move(other.storage_textureCUBE_array_use_info_);
  global_sampler_texture2D_last_use_index_ =
      other.global_sampler_texture2D_last_use_index_.load(
          std::memory_order_acquire);
  global_sampler_texture3D_last_use_index_ =
      other.global_sampler_texture3D_last_use_index_.load(
          std::memory_order_acquire);
  global_sampler_textureCUBE_last_use_index_ =
      other.global_sampler_textureCUBE_last_use_index_.load(
          std::memory_order_acquire);
  global_storage_texture2D_last_use_index_ =
      other.global_storage_texture2D_last_use_index_.load(
          std::memory_order_acquire);
  global_storage_texture3D_last_use_index_ =
      other.global_storage_texture3D_last_use_index_.load(
          std::memory_order_acquire);
  global_storage_textureCUBE_last_use_index_ =
      other.global_storage_textureCUBE_last_use_index_.load(
          std::memory_order_acquire);
  sampler_texture2D_last_use_index_ =
      other.sampler_texture2D_last_use_index_.load(std::memory_order_acquire);
  sampler_texture3D_last_use_index_ =
      other.sampler_texture3D_last_use_index_.load(std::memory_order_acquire);
  sampler_textureCUBE_last_use_index_ =
      other.sampler_textureCUBE_last_use_index_.load(std::memory_order_acquire);
  storage_texture2D_last_use_index_ =
      other.storage_texture2D_last_use_index_.load(std::memory_order_acquire);
  storage_texture3D_last_use_index_ =
      other.storage_texture3D_last_use_index_.load(std::memory_order_acquire);
  storage_textureCUBE_last_use_index_ =
      other.storage_textureCUBE_last_use_index_.load(std::memory_order_acquire);

  other.render_engine_ = nullptr;
  other.allocator_ = nullptr;
  other.descriptor_pool_ = nullptr;
  other.global_set_layout_ = nullptr;
  other.material_set_layout_ = nullptr;
  other.global_set_ = nullptr;
  other.material_set_ = nullptr;

  return *this;
}

VkWriteDescriptorSet MM::RenderSystem::DescriptorManager::
    GetGlobalSamplerTexture2DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_SAMPLER_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet  MM::RenderSystem::DescriptorManager::
    GetGlobalSamplerTexture3DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info
        ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_SAMPLER_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet  MM::RenderSystem::DescriptorManager::
    GetGlobalSamplerTextureCUBEDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info
        ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_SAMPLER_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet  MM::RenderSystem::DescriptorManager::
    GetGlobalStorageTexture2DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info
        ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_STORAGE_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet  MM::RenderSystem::DescriptorManager::
    GetGlobalStorageTexture3DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info
        ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_STORAGE_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet  MM::RenderSystem::DescriptorManager::
    GetGlobalStorageTextureCUBEDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info
        ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_STORAGE_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet
MM::RenderSystem::DescriptorManager::GetSamplerTexture2DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info
    ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_SAMPLER_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet
MM::RenderSystem::DescriptorManager::GetSamplerTexture3DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info
    ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_SAMPLER_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet
MM::RenderSystem::DescriptorManager::GetSamplerTextureCUBEDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info
    ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_SAMPLER_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet
MM::RenderSystem::DescriptorManager::GetStorageTexture2DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info
    ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_STORAGE_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet
MM::RenderSystem::DescriptorManager::GetStorageTexture3DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info
    ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_STORAGE_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

VkWriteDescriptorSet
MM::RenderSystem::DescriptorManager::GetStorageTextureCUBEDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info
    ) const {
  assert(IsValid());
  const VkWriteDescriptorSet output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_STORAGE_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return output;
}

void MM::RenderSystem::DescriptorManager::UpdateDescriptorSet(
    std::uint32_t descriptor_write_count,
    const VkWriteDescriptorSet* write_descriptor_sets) {
  assert(IsValid());
  vkUpdateDescriptorSets(render_engine_->GetDevice(), descriptor_write_count,
                         write_descriptor_sets, 0, nullptr);
}

MM::Result<MM::RenderSystem::DescriptorManager::DescriptorSlot> MM::RenderSystem::DescriptorManager::AllocateSlot(
    DescriptorType descriptor_type, bool is_global) {
  assert(IsValid());
  switch (descriptor_type) {
    case DescriptorType::SAMPLER_TEXTURE2D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global,
                            global_sampler_texture2D_last_use_index_,
                            global_sampler_texture2D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global,
                          sampler_texture2D_last_use_index_,
                          sampler_texture2D_array_use_info_);
    }
    case DescriptorType::SAMPLER_TEXTURE3D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global,
                            global_sampler_texture3D_last_use_index_,
                            global_sampler_texture3D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global,
                          sampler_texture3D_last_use_index_,
                          sampler_texture3D_array_use_info_);
    }
    case DescriptorType::SAMPLER_TEXTURECUBE: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global,
                            global_sampler_textureCUBE_last_use_index_,
                            global_sampler_textureCUBE_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global,
                          sampler_textureCUBE_last_use_index_,
                          sampler_textureCUBE_array_use_info_);
    }
    case DescriptorType::STORAGE_TEXTURE2D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global,
                            global_storage_texture2D_last_use_index_,
                            global_storage_texture2D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global,
                          storage_texture2D_last_use_index_,
                          storage_texture2D_array_use_info_);
    }
    case DescriptorType::STORAGE_TEXTURE3D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global,
                            global_storage_texture3D_last_use_index_,
                            global_storage_texture3D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global,
                          storage_texture3D_last_use_index_,
                          storage_texture3D_array_use_info_);
    }
    case DescriptorType::STORAGE_TEXTURECUBE: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global,
                            global_storage_textureCUBE_last_use_index_,
                            global_storage_textureCUBE_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global,
                          storage_textureCUBE_last_use_index_,
                          storage_textureCUBE_array_use_info_);
    }
    default:
      assert(false);
  }
}

MM::Result<std::vector<MM::RenderSystem::DescriptorManager::DescriptorSlot>> MM::RenderSystem::DescriptorManager::AllocateSlot(
    const std::uint32_t& need_free_count,
    DescriptorType descriptor_type, bool is_global) {
  assert(IsValid());
  switch (descriptor_type) {
    case DescriptorType::SAMPLER_TEXTURE2D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global, need_free_count,
                            global_sampler_texture2D_last_use_index_,
                            global_sampler_texture2D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global, need_free_count,
                          sampler_texture2D_last_use_index_,
                          sampler_texture2D_array_use_info_);
    }
    case DescriptorType::SAMPLER_TEXTURE3D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global, need_free_count,
                            global_sampler_texture3D_last_use_index_,
                            global_sampler_texture3D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global, need_free_count,
                          sampler_texture3D_last_use_index_,
                          sampler_texture3D_array_use_info_);
    }
    case DescriptorType::SAMPLER_TEXTURECUBE: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global, need_free_count,
                            global_sampler_textureCUBE_last_use_index_,
                            global_sampler_textureCUBE_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global, need_free_count,
                          sampler_textureCUBE_last_use_index_,
                          sampler_textureCUBE_array_use_info_);
    }
    case DescriptorType::STORAGE_TEXTURE2D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global, need_free_count,
                            global_storage_texture2D_last_use_index_,
                            global_storage_texture2D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global, need_free_count,
                          storage_texture2D_last_use_index_,
                          storage_texture2D_array_use_info_);
    }
    case DescriptorType::STORAGE_TEXTURE3D: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global, need_free_count,
                            global_storage_texture3D_last_use_index_,
                            global_storage_texture3D_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global, need_free_count,
                          storage_texture3D_last_use_index_,
                          storage_texture3D_array_use_info_);
    }
    case DescriptorType::STORAGE_TEXTURECUBE: {
      if (is_global) {
        return AllocateSlot(descriptor_type, is_global, need_free_count,
                            global_storage_textureCUBE_last_use_index_,
                            global_storage_textureCUBE_array_use_info_);
      }
      return AllocateSlot(descriptor_type, is_global, need_free_count,
                          storage_textureCUBE_last_use_index_,
                          storage_textureCUBE_array_use_info_);
    }
    default:
      assert(false);
  }
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    DescriptorSlot&& free_slot) {
  assert(IsValid() && free_slot.IsValid());
  switch (free_slot.GetDescriptorType()) {
    case DescriptorType::SAMPLER_TEXTURE2D:
      if (free_slot.GetIsGlobal()) {
        global_sampler_texture2D_array_use_info_[free_slot.GetSlotIndex()].clear();
      } else {
        sampler_texture2D_array_use_info_[free_slot.GetSlotIndex()].clear();
      }
    case DescriptorType::SAMPLER_TEXTURE3D:
      if (free_slot.GetIsGlobal()) {
        global_sampler_texture3D_array_use_info_[free_slot.GetSlotIndex()].clear();
      } else {
        sampler_texture3D_array_use_info_[free_slot.GetSlotIndex()].clear();
      }
    case DescriptorType::SAMPLER_TEXTURECUBE:
      if (free_slot.GetIsGlobal()) {
        global_sampler_textureCUBE_array_use_info_[free_slot.GetSlotIndex()].clear();
      } else {
        sampler_textureCUBE_array_use_info_[free_slot.GetSlotIndex()].clear();
      }
    case DescriptorType::STORAGE_TEXTURE2D:
      if (free_slot.GetIsGlobal()) {
        global_storage_texture2D_array_use_info_[free_slot.GetSlotIndex()].clear();
      } else {
        storage_texture2D_array_use_info_[free_slot.GetSlotIndex()].clear();
      }
    case DescriptorType::STORAGE_TEXTURE3D:
      if (free_slot.GetIsGlobal()) {
        global_storage_texture3D_array_use_info_[free_slot.GetSlotIndex()].clear();
      } else {
        storage_texture3D_array_use_info_[free_slot.GetSlotIndex()].clear();
      }
    case DescriptorType::STORAGE_TEXTURECUBE:
      if (free_slot.GetSlotIndex()) {
        global_storage_textureCUBE_array_use_info_[free_slot.GetSlotIndex()].clear();
      } else {
        storage_textureCUBE_array_use_info_[free_slot.GetSlotIndex()].clear();
      }
    default:
      assert(false);
  }

  free_slot.Reset();
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    std::uint32_t need_free_slot, DescriptorSlot* free_slots) {
  assert(IsValid());

  for (std::uint32_t i = 0; i != need_free_slot; ++i) {
    FreeSlot(std::move(free_slots[i]));
  }
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    std::vector<DescriptorSlot>&& free_slots) {
  assert(IsValid());

  for (DescriptorSlot& free_slot: free_slots) {
    FreeSlot(std::move(free_slot));
  }

  free_slots.clear();
}

MM::Result<MM::Nil> MM::RenderSystem::DescriptorManager::InitDescriptorManager(
    std::uint32_t global_sampler_texture2D_descriptor_element_count,
    std::uint32_t global_sampler_texture3D_descriptor_element_count,
    std::uint32_t global_sampler_textureCUBE_descriptor_element_count,
    std::uint32_t global_storage_texture2D_descriptor_element_count,
    std::uint32_t global_storage_texture3D_descriptor_element_count,
    std::uint32_t global_storage_textureCUBE_descriptor_element_count,
    std::uint32_t sampler_texture2D_descriptor_element_count,
    std::uint32_t sampler_texture3D_descriptor_element_count,
    std::uint32_t sampler_textureCUBE_descriptor_element_count,
    std::uint32_t storage_texture2D_descriptor_element_count,
    std::uint32_t storage_texture3D_descriptor_element_count,
    std::uint32_t storage_textureCUBE_descriptor_element_count) {
  std::array<VkDescriptorSetLayoutBinding, 12> bindings{};
  std::array<VkDescriptorBindingFlags, 6> flags{};
  constexpr std::array<VkDescriptorType, 2> types{
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER};

  for (std::uint32_t i = 0; i < 6; ++i) {
    flags.at(i) = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                  VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
  }

  for (std::uint32_t i = 0; i < 6; ++i) {
    bindings.at(i).binding = i;
    bindings.at(i + 6).binding = i;
    if (i < 4) {
      bindings.at(i).descriptorType = types.at(0);
      bindings.at(i + 6).descriptorType = types.at(0);
    } else {
      bindings.at(i).descriptorType = types.at(1);
      bindings.at(i + 6).descriptorType = types.at(1);
    }
    bindings.at(i).stageFlags = VK_SHADER_STAGE_ALL;
    bindings.at(i + 6).stageFlags = VK_SHADER_STAGE_ALL;
    bindings.at(i).pImmutableSamplers = nullptr;
    bindings.at(i + 6).pImmutableSamplers = nullptr;
  }

  bindings.at(0).descriptorCount =
      global_sampler_texture2D_descriptor_element_count;
  bindings.at(1).descriptorCount =
      global_sampler_texture3D_descriptor_element_count;
  bindings.at(2).descriptorCount =
      global_sampler_textureCUBE_descriptor_element_count;
  bindings.at(3).descriptorCount =
      global_storage_texture2D_descriptor_element_count;
  bindings.at(4).descriptorCount =
      global_storage_texture3D_descriptor_element_count;
  bindings.at(5).descriptorCount =
      global_storage_textureCUBE_descriptor_element_count;
  bindings.at(6).descriptorCount = sampler_texture2D_descriptor_element_count;
  bindings.at(7).descriptorCount = sampler_texture3D_descriptor_element_count;
  bindings.at(8).descriptorCount = sampler_textureCUBE_descriptor_element_count;
  bindings.at(9).descriptorCount = storage_texture2D_descriptor_element_count;
  bindings.at(10).descriptorCount = storage_texture3D_descriptor_element_count;
  bindings.at(11).descriptorCount =
      storage_textureCUBE_descriptor_element_count;

  VkDescriptorSetLayoutBindingFlagsCreateInfo flags_create_info;
  flags_create_info.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
  flags_create_info.pNext = nullptr;
  flags_create_info.bindingCount = flags.size();
  flags_create_info.pBindingFlags = flags.data();

  VkDescriptorSetLayoutCreateInfo global_layout_info{};
  global_layout_info.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  global_layout_info.pNext = &flags_create_info;
  global_layout_info.flags =
      VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
  global_layout_info.bindingCount = static_cast<uint32_t>(bindings.size()) / 2;
  global_layout_info.pBindings = bindings.data();

  VkDescriptorSetLayoutCreateInfo material_layout_info{};
  material_layout_info.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  material_layout_info.pNext = &flags_create_info;
  material_layout_info.flags =
      VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
  material_layout_info.bindingCount =
      static_cast<uint32_t>(bindings.size()) / 2;
  material_layout_info.pBindings = bindings.data() + 6;

  if (auto if_result = ConvertVkResultToMMResult(
      vkCreateDescriptorSetLayout(render_engine_->GetDevice(),
                                  &global_layout_info, allocator_,
                                  &global_set_layout_));
                                  if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkDescriptorSetLayout.")).IsError()) {
    render_engine_ = nullptr;
    return ResultE{if_result.GetError()};
  }

  if (auto if_result = ConvertVkResultToMMResult(
      vkCreateDescriptorSetLayout(render_engine_->GetDevice(),
                                  &material_layout_info, allocator_,
                                  &global_set_layout_));
                                  if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkDescriptorSetLayout.")).IsError()) {
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   global_set_layout_, allocator_);
      render_engine_ = nullptr;
    return ResultE{if_result.GetError()};
  }

  std::array<VkDescriptorPoolSize, 2> pool_sizes{};
  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount = 6;
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[1].descriptorCount = 6;

  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = static_cast<uint32_t>(2);

  if (auto if_result = ConvertVkResultToMMResult(vkCreateDescriptorPool(
          render_engine_->GetDevice(), &pool_info, nullptr, &descriptor_pool_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Failed to create descriptor pool."))
          .IsError()) {
    vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                 global_set_layout_, allocator_);
    vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                 material_set_layout_, allocator_);
    render_engine_ = nullptr;
    return ResultE{if_result.GetError()};
  }

  const std::array<VkDescriptorSetLayout, 2> layouts{global_set_layout_,
                                               material_set_layout_};

  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor_pool_;
  alloc_info.descriptorSetCount = layouts.size();
  alloc_info.pSetLayouts = layouts.data();

  std::array<VkDescriptorSet, 2> descriptor_sets_{};

  if (auto if_result = ConvertVkResultToMMResult(
      vkAllocateDescriptorSets(render_engine_->GetDevice(), &alloc_info,
                               descriptor_sets_.data()));
                               if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to allocate descriptor sets.")).IsError()) {
      vkDestroyDescriptorPool(render_engine_->GetDevice(), descriptor_pool_,
                              allocator_);
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   global_set_layout_, allocator_);
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   material_set_layout_, allocator_);
      render_engine_ = nullptr;
    return ResultE{if_result.GetError()};
  }

  global_set_ = descriptor_sets_[0];
  material_set_ = descriptor_sets_[1];

  if (auto if_result = CreatePipelineLayout();
    if_result.Exception(MM_ERROR_DESCRIPTION2("Failed create default pipeline layouts.")).IsError()) {
           vkDestroyDescriptorPool(render_engine_->GetDevice(),
                                   descriptor_pool_, allocator_);
           vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                        global_set_layout_, allocator_);
           vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                        material_set_layout_, allocator_);
           render_engine_ = nullptr;
    return ResultE{if_result.GetError()};
  }

  global_sampler_texture2D_array_use_info_ = std::vector<std::atomic_flag>(
      global_sampler_texture2D_descriptor_element_count);
  global_sampler_texture3D_array_use_info_ = std::vector<std::atomic_flag>(
      global_sampler_texture3D_descriptor_element_count);
  global_sampler_textureCUBE_array_use_info_ = std::vector<std::atomic_flag>(
      global_sampler_textureCUBE_descriptor_element_count);
  global_storage_texture2D_array_use_info_ = std::vector<std::atomic_flag>(
      global_storage_texture2D_descriptor_element_count);
  global_storage_texture3D_array_use_info_ = std::vector<std::atomic_flag>(
      global_storage_texture3D_descriptor_element_count);
  global_storage_textureCUBE_array_use_info_ = std::vector<std::atomic_flag>(
      global_storage_textureCUBE_descriptor_element_count);
  sampler_texture2D_array_use_info_ =
      std::vector<std::atomic_flag>(sampler_texture2D_descriptor_element_count);
  sampler_texture3D_array_use_info_ =
      std::vector<std::atomic_flag>(sampler_texture3D_descriptor_element_count);
  sampler_textureCUBE_array_use_info_ = std::vector<std::atomic_flag>(
      sampler_textureCUBE_descriptor_element_count);
  storage_texture2D_array_use_info_ =
      std::vector<std::atomic_flag>(storage_texture2D_descriptor_element_count);
  storage_texture3D_array_use_info_ =
      std::vector<std::atomic_flag>(storage_texture3D_descriptor_element_count);
  storage_textureCUBE_array_use_info_ = std::vector<std::atomic_flag>(
      storage_textureCUBE_descriptor_element_count);

  return ResultS<Nil>{};
}

MM::Result<MM::RenderSystem::DescriptorManager::DescriptorSlot> MM::RenderSystem::DescriptorManager::AllocateSlot(
    DescriptorType descriptor_type, bool is_global,
    std::atomic_uint32_t& last_use_index,
    std::vector<std::atomic_flag>& array_use_info) {
  const std::uint32_t start_index =
      last_use_index.load(std::memory_order_acquire);

  const std::uint32_t array_size = array_use_info.size();
  for (std::uint32_t index = start_index; index != array_size; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      last_use_index.store(index, std::memory_order_release);
      return ResultS<DescriptorSlot>{index, descriptor_type, is_global};
    }
  }

  for (std::uint32_t index = 0; index != start_index; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      last_use_index.store(index, std::memory_order_release);
      return ResultS<DescriptorSlot>{index, descriptor_type, is_global};
    }
  }

  return ResultE<>{ErrorCode::NO_AVAILABLE_ELEMENT};
}

MM::Result<std::vector<MM::RenderSystem::DescriptorManager::DescriptorSlot>> MM::RenderSystem::DescriptorManager::AllocateSlot(
    DescriptorType descriptor_type, bool is_global,
    std::uint32_t need_free_count,
    std::atomic_uint32_t& last_use_index,
    std::vector<std::atomic_flag>& array_use_info) {
  const std::uint32_t start_index = last_use_index.load(std::memory_order_acquire);
  std::uint32_t add_count = 0;

  const std::uint32_t array_size = array_use_info.size();
  std::vector<DescriptorSlot> free_slot{};
  for (std::uint32_t index = start_index; index != array_size; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      free_slot.emplace_back(index, descriptor_type, is_global);
      ++add_count;
      if (--need_free_count == 0) {
        last_use_index.store(index, std::memory_order_release);
        return ResultS{std::move(free_slot)};
      }
    }
  }

  for (std::uint32_t index = 0; index != start_index; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      free_slot.emplace_back(index, descriptor_type, is_global);
      ++add_count;
      if (--need_free_count == 0) {
        last_use_index.store(index, std::memory_order_release);
        return ResultS{std::move(free_slot)};
      }
    }
  }

  free_slot.resize(free_slot.size() - add_count);

  return ResultE<>{ErrorCode::NO_AVAILABLE_ELEMENT};
}

bool MM::RenderSystem::DescriptorManager::IsValid() const {
#ifdef NDEBUG
  return render_engine_ != nullptr;
#else
  return render_engine_ != nullptr && render_engine_->IsValid() &&
         descriptor_pool_ != nullptr && global_set_layout_ != nullptr &&
         material_set_layout_ != nullptr && global_set_ != nullptr &&
         material_set_ != nullptr;
#endif
}

void MM::RenderSystem::DescriptorManager::Release() {
  if (IsValid()) {
    global_sampler_texture2D_last_use_index_.store(0,
                                                   std::memory_order_release);
    global_sampler_texture3D_last_use_index_.store(0,
                                                   std::memory_order_release);
    global_sampler_textureCUBE_last_use_index_.store(0,
                                                     std::memory_order_release);
    global_storage_texture2D_last_use_index_.store(0,
                                                   std::memory_order_release);
    global_storage_texture3D_last_use_index_.store(0,
                                                   std::memory_order_release);
    global_storage_textureCUBE_last_use_index_.store(0,
                                                     std::memory_order_release);
    sampler_texture2D_last_use_index_.store(0, std::memory_order_release);
    sampler_texture3D_last_use_index_.store(0, std::memory_order_release);
    sampler_textureCUBE_last_use_index_.store(0, std::memory_order_release);
    storage_texture2D_last_use_index_.store(0, std::memory_order_release);
    storage_texture3D_last_use_index_.store(0, std::memory_order_release);
    storage_textureCUBE_last_use_index_.store(0, std::memory_order_release);

    global_sampler_texture2D_array_use_info_.clear();
    global_sampler_texture3D_array_use_info_.clear();
    global_sampler_textureCUBE_array_use_info_.clear();
    global_storage_texture2D_array_use_info_.clear();
    global_storage_texture3D_array_use_info_.clear();
    global_storage_textureCUBE_array_use_info_.clear();
    sampler_texture2D_array_use_info_.clear();
    sampler_texture3D_array_use_info_.clear();
    sampler_textureCUBE_array_use_info_.clear();
    storage_texture2D_array_use_info_.clear();
    storage_texture3D_array_use_info_.clear();
    storage_textureCUBE_array_use_info_.clear();

    CleanPipelineLayout();

    vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                 global_set_layout_, allocator_);
    vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                 material_set_layout_, allocator_);
    vkDestroyDescriptorPool(render_engine_->GetDevice(), descriptor_pool_,
                            allocator_);

    allocator_ = nullptr;
    render_engine_ = nullptr;
  }
}

VkDescriptorSetLayout
MM::RenderSystem::DescriptorManager::GetGlobalDescriptorSetLayout() const {
  assert(IsValid());
  return global_set_layout_;
}

VkDescriptorSetLayout
MM::RenderSystem::DescriptorManager::GetMaterialDescriptorSetLayout() const {
  assert(IsValid());
  return material_set_layout_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout0() const {
  assert(IsValid());
  return pipeline_layout0_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout1() const {
  assert(IsValid());
  return pipeline_layout1_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout2() const {
  assert(IsValid());
  return pipeline_layout2_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout3() const {
  assert(IsValid());
  return pipeline_layout3_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout4() const {
  assert(IsValid());
  return pipeline_layout4_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout5() const {
  assert(IsValid());
  return pipeline_layout5_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout6() const {
  assert(IsValid());
  return pipeline_layout6_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout7() const {
  assert(IsValid());
  return pipeline_layout7_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout8() const {
  assert(IsValid());
  return pipeline_layout8_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout9() const {
  assert(IsValid());
  return pipeline_layout9_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout10() const {
  assert(IsValid());
  return pipeline_layout10_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout11() const {
  assert(IsValid());
  return pipeline_layout11_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout12() const {
  assert(IsValid());
  return pipeline_layout12_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout13() const {
  assert(IsValid());
  return pipeline_layout13_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout14() const {
  assert(IsValid());
  return pipeline_layout14_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout15() const {
  assert(IsValid());
  return pipeline_layout15_;
}

VkPipelineLayout
MM::RenderSystem::DescriptorManager::GetDefaultPipelineLayout16() const {
  assert(IsValid());
  return pipeline_layout16_;
}

MM::Result<MM::Nil> MM::RenderSystem::DescriptorManager::CreatePipelineLayout() {
  std::array<VkDescriptorSetLayout, 2> descriptor_set_layout{
      global_set_layout_, material_set_layout_};

  VkPushConstantRange range{VK_SHADER_STAGE_ALL, 0, 0};

  VkPipelineLayoutCreateInfo vk_pipeline_layout_create_info{};
  vk_pipeline_layout_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  vk_pipeline_layout_create_info.pNext = nullptr;
  vk_pipeline_layout_create_info.flags = 0;
  vk_pipeline_layout_create_info.setLayoutCount = descriptor_set_layout.size();
  vk_pipeline_layout_create_info.pSetLayouts = descriptor_set_layout.data();
  vk_pipeline_layout_create_info.pushConstantRangeCount = 0;
  vk_pipeline_layout_create_info.pPushConstantRanges = nullptr;
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout0_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
      CleanPipelineLayout();
      return ResultE{if_result.GetError()};
  }

  vk_pipeline_layout_create_info.pushConstantRangeCount = 1;
  vk_pipeline_layout_create_info.pPushConstantRanges = &range;

  range.size = sizeof(PushData1);
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout1_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
      CleanPipelineLayout();
      return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData2);
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout2_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
      CleanPipelineLayout();
      return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData3);
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout3_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
      CleanPipelineLayout();
      return ResultE{if_result.GetError()};
  }


  range.size = sizeof(PushData4);
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout4_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
      CleanPipelineLayout();
      return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData5);
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout5_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).GetError()) {
      CleanPipelineLayout();
      return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData6);
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout6_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
      CleanPipelineLayout();
      return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData7);
  if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout7_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData8);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout8_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData9);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout9_));
      if_result
          .Exception(
              MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout."))
          .IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData10);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout10_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData11);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout11_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData12);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout12_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData13);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout13_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData14);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout14_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData15);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout15_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData15);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout15_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  range.size = sizeof(PushData16);
    if (auto if_result = ConvertVkResultToMMResult(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &vk_pipeline_layout_create_info, allocator_,
                             &pipeline_layout16_));
                             if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkPipelineLayout.")).IsError()) {
    CleanPipelineLayout();
    return ResultE{if_result.GetError()};
  }

  return ResultS<Nil>{};
}

void MM::RenderSystem::DescriptorManager::CleanPipelineLayout() {
  if (pipeline_layout0_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout0_,
                            allocator_);
  }

  if (pipeline_layout1_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout1_,
                            allocator_);
  }

  if (pipeline_layout2_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout2_,
                            allocator_);
  }

  if (pipeline_layout3_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout3_,
                            allocator_);
  }

  if (pipeline_layout4_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout4_,
                            allocator_);
  }

  if (pipeline_layout5_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout5_,
                            allocator_);
  }

  if (pipeline_layout6_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout6_,
                            allocator_);
  }

  if (pipeline_layout7_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout7_,
                            allocator_);
  }

  if (pipeline_layout8_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout8_,
                            allocator_);
  }

  if (pipeline_layout9_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout9_,
                            allocator_);
  }

  if (pipeline_layout10_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout10_,
                            allocator_);
  }

  if (pipeline_layout11_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout11_,
                            allocator_);
  }

  if (pipeline_layout12_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout12_,
                            allocator_);
  }

  if (pipeline_layout13_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout13_,
                            allocator_);
  }

  if (pipeline_layout14_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout14_,
                            allocator_);
  }

  if (pipeline_layout15_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout15_,
                            allocator_);
  }

  if (pipeline_layout16_ != nullptr) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout16_,
                            allocator_);
  }
}
