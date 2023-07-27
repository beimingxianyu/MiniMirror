//
// Created by beimingxianyu on 23-7-27.
//

#include "runtime/function/render/DescriptorManager.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::DescriptorManager::~DescriptorManager() { Release(); }

MM::RenderSystem::DescriptorManager::DescriptorManager(
    MM::RenderSystem::RenderEngine* render_engine,
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
      storage_textureCUBE_last_use_index_() {
  if (render_engine_ == nullptr || !render_engine_->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is invalid.");
    return;
  }

  MM_CHECK(InitDescriptorManager(
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count,
               one_descriptor_element_count, one_descriptor_element_count),
           MM_LOG_ERROR("Failed initialization descriptor manager.");)
}

MM::RenderSystem::DescriptorManager::DescriptorManager(
    MM::RenderSystem::RenderEngine* render_engine,
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
      storage_textureCUBE_last_use_index_() {
  if (render_engine_ == nullptr || !render_engine_->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is invalid.");
    return;
  }

  MM_CHECK(
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
                            storage_textureCUBE_descriptor_element_count),
      MM_LOG_ERROR("Failed initialization descriptor manager.");)
}

MM::RenderSystem::DescriptorManager::DescriptorManager(
    MM::RenderSystem::DescriptorManager&& other) noexcept
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
    MM::RenderSystem::DescriptorManager&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

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

MM::Utils::ExecuteResult MM::RenderSystem::DescriptorManager::
    GetGlobalSamplerTexture2DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info,
        VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_SAMPLER_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::RenderSystem::DescriptorManager::
    GetGlobalSamplerTexture3DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info,
        VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_SAMPLER_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::RenderSystem::DescriptorManager::
    GetGlobalSamplerTextureCUBEDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info,
        VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_SAMPLER_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::RenderSystem::DescriptorManager::
    GetGlobalStorageTexture2DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info,
        VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_STORAGE_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::RenderSystem::DescriptorManager::
    GetGlobalStorageTexture3DDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info,
        VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_STORAGE_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::RenderSystem::DescriptorManager::
    GetGlobalStorageTextureCUBEDescriptorWriteInfo(
        std::uint32_t dest_array_element, std::uint32_t descriptor_count,
        const VkDescriptorImageInfo* image_info,
        VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                global_set_,
                                MM_SHADER_STORAGE_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult
MM::RenderSystem::DescriptorManager::GetSamplerTexture2DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info,
    VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_SAMPLER_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult
MM::RenderSystem::DescriptorManager::GetSamplerTexture3DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info,
    VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_SAMPLER_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult
MM::RenderSystem::DescriptorManager::GetSamplerTextureCUBEDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info,
    VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_SAMPLER_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult
MM::RenderSystem::DescriptorManager::GetStorageTexture2DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info,
    VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_STORAGE_TEXTURE2D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult
MM::RenderSystem::DescriptorManager::GetStorageTexture3DDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info,
    VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_STORAGE_TEXTURE3D_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult
MM::RenderSystem::DescriptorManager::GetStorageTextureCUBEDescriptorWriteInfo(
    std::uint32_t dest_array_element, std::uint32_t descriptor_count,
    const VkDescriptorImageInfo* image_info,
    VkWriteDescriptorSet& output) const {
  output = VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                nullptr,
                                material_set_,
                                MM_SHADER_STORAGE_TEXTURECUBE_BINDING,
                                dest_array_element,
                                descriptor_count,
                                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                image_info,
                                nullptr,
                                nullptr};

  return ExecuteResult::SUCCESS;
}

void MM::RenderSystem::DescriptorManager::UpdateDescriptorSet(
    std::uint32_t descriptor_write_count,
    const VkWriteDescriptorSet* write_descriptor_sets) {
  vkUpdateDescriptorSets(render_engine_->GetDevice(), descriptor_write_count,
                         write_descriptor_sets, 0, nullptr);
}

MM::ExecuteResult MM::RenderSystem::DescriptorManager::AllocateSlot(
    std::uint32_t& free_slot, DescriptorType descriptor_type, bool is_global) {
  switch (descriptor_type) {
    case DescriptorType::SAMPLER_TEXTURE2D:
      if (is_global) {
        return AllocateSlot(free_slot, global_sampler_texture2D_last_use_index_,
                            global_sampler_texture2D_array_use_info_);
      } else {
        return AllocateSlot(free_slot, sampler_texture2D_last_use_index_,
                            sampler_texture2D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURE3D:
      if (is_global) {
        return AllocateSlot(free_slot, global_sampler_texture3D_last_use_index_,
                            global_sampler_texture3D_array_use_info_);
      } else {
        return AllocateSlot(free_slot, sampler_texture3D_last_use_index_,
                            sampler_texture3D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURECUBE:
      if (is_global) {
        return AllocateSlot(free_slot,
                            global_sampler_textureCUBE_last_use_index_,
                            global_sampler_textureCUBE_array_use_info_);
      } else {
        return AllocateSlot(free_slot, sampler_textureCUBE_last_use_index_,
                            sampler_textureCUBE_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE2D:
      if (is_global) {
        return AllocateSlot(free_slot, global_storage_texture2D_last_use_index_,
                            global_storage_texture2D_array_use_info_);
      } else {
        return AllocateSlot(free_slot, storage_texture2D_last_use_index_,
                            storage_texture2D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE3D:
      if (is_global) {
        return AllocateSlot(free_slot, global_storage_texture3D_last_use_index_,
                            global_storage_texture3D_array_use_info_);
      } else {
        return AllocateSlot(free_slot, storage_texture3D_last_use_index_,
                            storage_texture3D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURECUBE:
      if (is_global) {
        return AllocateSlot(free_slot,
                            global_storage_textureCUBE_last_use_index_,
                            global_storage_textureCUBE_array_use_info_);
      } else {
        return AllocateSlot(free_slot, storage_textureCUBE_last_use_index_,
                            storage_textureCUBE_array_use_info_);
      }
  }
}

MM::ExecuteResult MM::RenderSystem::DescriptorManager::AllocateSlot(
    std::uint32_t& need_free_count, std::vector<std::uint32_t>& free_slots,
    DescriptorType descriptor_type, bool is_global) {
  switch (descriptor_type) {
    case DescriptorType::SAMPLER_TEXTURE2D:
      if (is_global) {
        return AllocateSlot(need_free_count, free_slots,
                            global_sampler_texture2D_last_use_index_,
                            global_sampler_texture2D_array_use_info_);
      } else {
        return AllocateSlot(need_free_count, free_slots,
                            sampler_texture2D_last_use_index_,
                            sampler_texture2D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURE3D:
      if (is_global) {
        return AllocateSlot(need_free_count, free_slots,
                            global_sampler_texture3D_last_use_index_,
                            global_sampler_texture3D_array_use_info_);
      } else {
        return AllocateSlot(need_free_count, free_slots,
                            sampler_texture3D_last_use_index_,
                            sampler_texture3D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURECUBE:
      if (is_global) {
        return AllocateSlot(need_free_count, free_slots,
                            global_sampler_textureCUBE_last_use_index_,
                            global_sampler_textureCUBE_array_use_info_);
      } else {
        return AllocateSlot(need_free_count, free_slots,
                            sampler_textureCUBE_last_use_index_,
                            sampler_textureCUBE_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE2D:
      if (is_global) {
        return AllocateSlot(need_free_count, free_slots,
                            global_storage_texture2D_last_use_index_,
                            global_storage_texture2D_array_use_info_);
      } else {
        return AllocateSlot(need_free_count, free_slots,
                            storage_texture2D_last_use_index_,
                            storage_texture2D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE3D:
      if (is_global) {
        return AllocateSlot(need_free_count, free_slots,
                            global_storage_texture3D_last_use_index_,
                            global_storage_texture3D_array_use_info_);
      } else {
        return AllocateSlot(need_free_count, free_slots,
                            storage_texture3D_last_use_index_,
                            storage_texture3D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURECUBE:
      if (is_global) {
        return AllocateSlot(need_free_count, free_slots,
                            global_storage_textureCUBE_last_use_index_,
                            global_storage_textureCUBE_array_use_info_);
      } else {
        return AllocateSlot(need_free_count, free_slots,
                            storage_textureCUBE_last_use_index_,
                            storage_textureCUBE_array_use_info_);
      }
  }
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    std::uint32_t free_slot, MM::RenderSystem::DescriptorType descriptor_type,
    bool is_global) {
  switch (descriptor_type) {
    case DescriptorType::SAMPLER_TEXTURE2D:
      if (is_global) {
        FreeSlot(free_slot, global_sampler_texture2D_array_use_info_);
      } else {
        FreeSlot(free_slot, sampler_texture2D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURE3D:
      if (is_global) {
        FreeSlot(free_slot, global_sampler_texture3D_array_use_info_);
      } else {
        FreeSlot(free_slot, sampler_texture3D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURECUBE:
      if (is_global) {
        FreeSlot(free_slot, global_sampler_textureCUBE_array_use_info_);
      } else {
        FreeSlot(free_slot, sampler_textureCUBE_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE2D:
      if (is_global) {
        FreeSlot(free_slot, global_storage_texture2D_array_use_info_);
      } else {
        FreeSlot(free_slot, storage_texture2D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE3D:
      if (is_global) {
        FreeSlot(free_slot, global_storage_texture3D_array_use_info_);
      } else {
        FreeSlot(free_slot, storage_texture3D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURECUBE:
      if (is_global) {
        FreeSlot(free_slot, global_storage_textureCUBE_array_use_info_);
      } else {
        FreeSlot(free_slot, storage_textureCUBE_array_use_info_);
      }
  }
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    std::uint32_t need_free_slot, std::uint32_t* free_slots,
    MM::RenderSystem::DescriptorType descriptor_type, bool is_global) {
  switch (descriptor_type) {
    case DescriptorType::SAMPLER_TEXTURE2D:
      if (is_global) {
        FreeSlot(need_free_slot, free_slots,
                 global_sampler_texture2D_array_use_info_);
      } else {
        FreeSlot(need_free_slot, free_slots, sampler_texture2D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURE3D:
      if (is_global) {
        FreeSlot(need_free_slot, free_slots,
                 global_sampler_texture3D_array_use_info_);
      } else {
        FreeSlot(need_free_slot, free_slots, sampler_texture3D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURECUBE:
      if (is_global) {
        FreeSlot(need_free_slot, free_slots,
                 global_sampler_textureCUBE_array_use_info_);
      } else {
        FreeSlot(need_free_slot, free_slots,
                 sampler_textureCUBE_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE2D:
      if (is_global) {
        FreeSlot(need_free_slot, free_slots,
                 global_storage_texture2D_array_use_info_);
      } else {
        FreeSlot(need_free_slot, free_slots, storage_texture2D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE3D:
      if (is_global) {
        FreeSlot(need_free_slot, free_slots,
                 global_storage_texture3D_array_use_info_);
      } else {
        FreeSlot(need_free_slot, free_slots, storage_texture3D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURECUBE:
      if (is_global) {
        FreeSlot(need_free_slot, free_slots,
                 global_storage_textureCUBE_array_use_info_);
      } else {
        FreeSlot(need_free_slot, free_slots,
                 storage_textureCUBE_array_use_info_);
      }
  }
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    const std::vector<std::uint32_t>& free_slots,
    MM::RenderSystem::DescriptorType descriptor_type, bool is_global) {
  switch (descriptor_type) {
    case DescriptorType::SAMPLER_TEXTURE2D:
      if (is_global) {
        FreeSlot(free_slots, global_sampler_texture2D_array_use_info_);
      } else {
        FreeSlot(free_slots, sampler_texture2D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURE3D:
      if (is_global) {
        FreeSlot(free_slots, global_sampler_texture3D_array_use_info_);
      } else {
        FreeSlot(free_slots, sampler_texture3D_array_use_info_);
      }
    case DescriptorType::SAMPLER_TEXTURECUBE:
      if (is_global) {
        FreeSlot(free_slots, global_sampler_textureCUBE_array_use_info_);
      } else {
        FreeSlot(free_slots, sampler_textureCUBE_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE2D:
      if (is_global) {
        FreeSlot(free_slots, global_storage_texture2D_array_use_info_);
      } else {
        FreeSlot(free_slots, storage_texture2D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURE3D:
      if (is_global) {
        FreeSlot(free_slots, global_storage_texture3D_array_use_info_);
      } else {
        FreeSlot(free_slots, storage_texture3D_array_use_info_);
      }
    case DescriptorType::STORAGE_TEXTURECUBE:
      if (is_global) {
        FreeSlot(free_slots, global_storage_textureCUBE_array_use_info_);
      } else {
        FreeSlot(free_slots, storage_textureCUBE_array_use_info_);
      }
  }
}

MM::ExecuteResult MM::RenderSystem::DescriptorManager::InitDescriptorManager(
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
  std::array<VkDescriptorType, 2> types{
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

  MM_VK_CHECK(
      vkCreateDescriptorSetLayout(render_engine_->GetDevice(),
                                  &global_layout_info, allocator_,
                                  &global_set_layout_),
      MM_LOG_ERROR("Failed to create VkDescriptorSetLayout.");
      render_engine_ = nullptr;
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)
  MM_VK_CHECK(
      vkCreateDescriptorSetLayout(render_engine_->GetDevice(),
                                  &material_layout_info, allocator_,
                                  &global_set_layout_),
      MM_LOG_ERROR("Failed to create VkDescriptorSetLayout.");
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   global_set_layout_, allocator_);
      render_engine_ = nullptr;
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)

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

  MM_VK_CHECK(
      vkCreateDescriptorPool(render_engine_->GetDevice(), &pool_info, nullptr,
                             &descriptor_pool_),
      MM_LOG_ERROR("Failed to create descriptor pool.");
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   global_set_layout_, allocator_);
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   material_set_layout_, allocator_);
      render_engine_ = nullptr;
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)

  std::array<VkDescriptorSetLayout, 2> layouts{global_set_layout_,
                                               material_set_layout_};

  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor_pool_;
  alloc_info.descriptorSetCount = layouts.size();
  alloc_info.pSetLayouts = layouts.data();

  std::array<VkDescriptorSet, 2> descriptor_sets_{};

  MM_VK_CHECK(
      vkAllocateDescriptorSets(render_engine_->GetDevice(), &alloc_info,
                               descriptor_sets_.data()),
      MM_LOG_ERROR("Failed to allocate descriptor sets.");
      vkDestroyDescriptorPool(render_engine_->GetDevice(), descriptor_pool_,
                              allocator_);
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   global_set_layout_, allocator_);
      vkDestroyDescriptorSetLayout(render_engine_->GetDevice(),
                                   material_set_layout_, allocator_);
      render_engine_ = nullptr;
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)

  global_set_ = descriptor_sets_[0];
  material_set_ = descriptor_sets_[1];

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
}

MM::ExecuteResult MM::RenderSystem::DescriptorManager::AllocateSlot(
    std::uint32_t& free_slot, std::atomic_uint32_t& last_use_index,
    std::vector<std::atomic_flag>& array_use_info) {
  std::uint32_t start_index = last_use_index.load(std::memory_order_acquire);

  std::uint32_t array_size = array_use_info.size();
  for (std::uint32_t index = start_index; index != array_size; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      last_use_index.store(index, std::memory_order_release);
      free_slot = index;
      return ExecuteResult ::SUCCESS;
    }
  }

  for (std::uint32_t index = 0; index != start_index; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      last_use_index.store(index, std::memory_order_release);
      free_slot = index;
      return ExecuteResult ::SUCCESS;
    }
  }

  return ExecuteResult::NO_AVAILABLE_ELEMENT;
}

MM::ExecuteResult MM::RenderSystem::DescriptorManager::AllocateSlot(
    std::uint32_t need_free_count, std::vector<std::uint32_t>& free_slot,
    std::atomic_uint32_t& last_use_index,
    std::vector<std::atomic_flag>& array_use_info) {
  std::uint32_t start_index = last_use_index.load(std::memory_order_acquire);
  std::uint32_t add_count = 0;

  std::uint32_t array_size = array_use_info.size();
  for (std::uint32_t index = start_index; index != array_size; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      free_slot.emplace_back(index);
      ++add_count;
      if (--need_free_count == 0) {
        last_use_index.store(index, std::memory_order_release);
        return ExecuteResult ::SUCCESS;
      }
    }
  }

  for (std::uint32_t index = 0; index != start_index; ++index) {
    if (array_use_info[index].test_and_set() == 0) {
      free_slot.emplace_back(index);
      ++add_count;
      if (--need_free_count == 0) {
        last_use_index.store(index, std::memory_order_release);
        return ExecuteResult ::SUCCESS;
      }
    }
  }

  free_slot.resize(free_slot.size() - add_count);

  return ExecuteResult::NO_AVAILABLE_ELEMENT;
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    std::uint32_t free_slot, std::vector<std::atomic_flag>& array_use_info) {
  array_use_info[free_slot].clear();
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    std::uint32_t need_free_slot, std::uint32_t* free_slots,
    std::vector<std::atomic_flag>& array_use_info) {
  for (std::uint32_t i = 0; i != need_free_slot; ++i) {
    array_use_info[free_slots[i]].clear();
  }
}

void MM::RenderSystem::DescriptorManager::FreeSlot(
    const std::vector<std::uint32_t>& free_slots,
    std::vector<std::atomic_flag>& array_use_info) {
  for (const auto& index : free_slots) {
    array_use_info[index].clear();
  }
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
