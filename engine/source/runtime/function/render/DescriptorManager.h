//
// Created by beimingxianyu on 23-7-27.
//
#include <vulkan/vulkan.h>

#include <atomic>
#include <cstdint>
#include <vector>

#include "runtime/function/render/RenderResourceTexture.h"
#include "runtime/function/render/import_other_system.h"
#include "runtime/function/render/vk_enum.h"

namespace MM {
namespace RenderSystem {
class DescriptorManager {
 public:
  DescriptorManager() = default;
  ~DescriptorManager();
  DescriptorManager(RenderEngine* render_engine,
                    VkAllocationCallbacks* allocator,
                    std::uint32_t one_descriptor_element_count);
  DescriptorManager(
      RenderEngine* render_engine, VkAllocationCallbacks* allocator,
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
      std::uint32_t storage_textureCUBE_descriptor_element_count);
  DescriptorManager(const DescriptorManager& other) = delete;
  DescriptorManager(DescriptorManager&& other) noexcept;
  DescriptorManager& operator=(const DescriptorManager& other) = delete;
  DescriptorManager& operator=(DescriptorManager&& other) noexcept;

 public:
  ExecuteResult GetGlobalSamplerTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetGlobalSamplerTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetGlobalSamplerTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetGlobalStorageTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetGlobalStorageTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetGlobalStorageTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetSamplerTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetSamplerTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetSamplerTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetStorageTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetStorageTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  ExecuteResult GetStorageTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info,
      VkWriteDescriptorSet& output) const;

  void UpdateDescriptorSet(std::uint32_t descriptor_write_count,
                           const VkWriteDescriptorSet* write_descriptor_sets);

  ExecuteResult AllocateSlot(std::uint32_t& free_slot,
                             DescriptorType descriptor_type, bool is_global);

  ExecuteResult AllocateSlot(std::uint32_t& need_free_count,
                             std::vector<std::uint32_t>& free_slots,
                             DescriptorType descriptor_type, bool is_global);
  void FreeSlot(std::uint32_t free_slot, DescriptorType descriptor_type,
                bool is_global);

  void FreeSlot(std::uint32_t need_free_slot, std::uint32_t* free_slots,
                DescriptorType descriptor_type, bool is_global);

  void FreeSlot(const std::vector<std::uint32_t>& free_slots,
                DescriptorType descriptor_type, bool is_global);

  VkDescriptorSetLayout GetGlobalDescriptorSetLayout() const;

  VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const;

  bool IsValid() const;

  void Release();

 private:
  ExecuteResult InitDescriptorManager(
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
      std::uint32_t storage_textureCUBE_descriptor_element_count);

  ExecuteResult AllocateSlot(std::uint32_t& free_slot,
                             std::atomic_uint32_t& last_use_index,
                             std::vector<std::atomic_flag>& array_use_info);

  ExecuteResult AllocateSlot(std::uint32_t need_free_count,
                             std::vector<std::uint32_t>& free_slot,
                             std::atomic_uint32_t& last_use_index,
                             std::vector<std::atomic_flag>& array_use_info);

  void FreeSlot(std::uint32_t free_slot,
                std::vector<std::atomic_flag>& array_use_info);

  void FreeSlot(std::uint32_t need_free_slot, std::uint32_t* free_slots,
                std::vector<std::atomic_flag>& array_use_info);

  void FreeSlot(const std::vector<std::uint32_t>& free_slots,
                std::vector<std::atomic_flag>& array_use_info);

 private:
  RenderEngine* render_engine_{nullptr};
  VkAllocationCallbacks* allocator_{nullptr};

  VkDescriptorPool descriptor_pool_{nullptr};
  VkDescriptorSetLayout global_set_layout_{nullptr};
  VkDescriptorSetLayout material_set_layout_{nullptr};
  VkDescriptorSet global_set_{nullptr};
  VkDescriptorSet material_set_{nullptr};

  std::vector<std::atomic_flag> global_sampler_texture2D_array_use_info_{};
  std::vector<std::atomic_flag> global_sampler_texture3D_array_use_info_{};
  std::vector<std::atomic_flag> global_sampler_textureCUBE_array_use_info_{};
  std::vector<std::atomic_flag> global_storage_texture2D_array_use_info_{};
  std::vector<std::atomic_flag> global_storage_texture3D_array_use_info_{};
  std::vector<std::atomic_flag> global_storage_textureCUBE_array_use_info_{};
  std::vector<std::atomic_flag> sampler_texture2D_array_use_info_{};
  std::vector<std::atomic_flag> sampler_texture3D_array_use_info_{};
  std::vector<std::atomic_flag> sampler_textureCUBE_array_use_info_{};
  std::vector<std::atomic_flag> storage_texture2D_array_use_info_{};
  std::vector<std::atomic_flag> storage_texture3D_array_use_info_{};
  std::vector<std::atomic_flag> storage_textureCUBE_array_use_info_{};

  std::atomic_uint32_t global_sampler_texture2D_last_use_index_{};
  std::atomic_uint32_t global_sampler_texture3D_last_use_index_{};
  std::atomic_uint32_t global_sampler_textureCUBE_last_use_index_{};
  std::atomic_uint32_t global_storage_texture2D_last_use_index_{};
  std::atomic_uint32_t global_storage_texture3D_last_use_index_{};
  std::atomic_uint32_t global_storage_textureCUBE_last_use_index_{};
  std::atomic_uint32_t sampler_texture2D_last_use_index_{};
  std::atomic_uint32_t sampler_texture3D_last_use_index_{};
  std::atomic_uint32_t sampler_textureCUBE_last_use_index_{};
  std::atomic_uint32_t storage_texture2D_last_use_index_{};
  std::atomic_uint32_t storage_texture3D_last_use_index_{};
  std::atomic_uint32_t storage_textureCUBE_last_use_index_{};
};

}  // namespace RenderSystem
}  // namespace MM
