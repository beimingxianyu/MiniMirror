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

  VkPipelineLayout GetDefaultPipelineLayout0() const;

  VkPipelineLayout GetDefaultPipelineLayout1() const;

  VkPipelineLayout GetDefaultPipelineLayout2() const;

  VkPipelineLayout GetDefaultPipelineLayout3() const;

  VkPipelineLayout GetDefaultPipelineLayout4() const;

  VkPipelineLayout GetDefaultPipelineLayout5() const;

  VkPipelineLayout GetDefaultPipelineLayout6() const;

  VkPipelineLayout GetDefaultPipelineLayout7() const;

  VkPipelineLayout GetDefaultPipelineLayout8() const;

  VkPipelineLayout GetDefaultPipelineLayout9() const;

  VkPipelineLayout GetDefaultPipelineLayout10() const;

  VkPipelineLayout GetDefaultPipelineLayout11() const;

  VkPipelineLayout GetDefaultPipelineLayout12() const;

  VkPipelineLayout GetDefaultPipelineLayout13() const;

  VkPipelineLayout GetDefaultPipelineLayout14() const;

  VkPipelineLayout GetDefaultPipelineLayout15() const;

  VkPipelineLayout GetDefaultPipelineLayout16() const;

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

  ExecuteResult CreatePipelineLayout();

  void CleanPipelineLayout();

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

  VkPipelineLayout pipeline_layout0_{nullptr};
  VkPipelineLayout pipeline_layout1_{nullptr};
  VkPipelineLayout pipeline_layout2_{nullptr};
  VkPipelineLayout pipeline_layout3_{nullptr};
  VkPipelineLayout pipeline_layout4_{nullptr};
  VkPipelineLayout pipeline_layout5_{nullptr};
  VkPipelineLayout pipeline_layout6_{nullptr};
  VkPipelineLayout pipeline_layout7_{nullptr};
  VkPipelineLayout pipeline_layout8_{nullptr};
  VkPipelineLayout pipeline_layout9_{nullptr};
  VkPipelineLayout pipeline_layout10_{nullptr};
  VkPipelineLayout pipeline_layout11_{nullptr};
  VkPipelineLayout pipeline_layout12_{nullptr};
  VkPipelineLayout pipeline_layout13_{nullptr};
  VkPipelineLayout pipeline_layout14_{nullptr};
  VkPipelineLayout pipeline_layout15_{nullptr};
  VkPipelineLayout pipeline_layout16_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM
