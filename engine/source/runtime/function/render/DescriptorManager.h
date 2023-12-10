//
// Created by beimingxianyu on 23-7-27.
//
#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

#include "runtime/function/render/RenderResourceTexture.h"
#include "runtime/function/render/pre_header.h"
#include "runtime/function/render/vk_enum.h"

namespace MM {
namespace RenderSystem {
class DescriptorManager {
 class DescriptorSlot {
friend class DescriptorManager;

 public:
   DescriptorSlot() = default;
   ~DescriptorSlot() = default;
   DescriptorSlot(const DescriptorSlot& other) = delete;
   DescriptorSlot(DescriptorSlot&& other) noexcept;
   DescriptorSlot& operator=(const DescriptorSlot& other) = delete;
   DescriptorSlot& operator=(DescriptorSlot&& other) noexcept;

  public:
   std::uint32_t GetSlotIndex() const;

   DescriptorType GetDescriptorType() const;

   bool GetIsGlobal() const;

   bool IsValid() const;

  private:
  DescriptorSlot(std::uint32_t slot_index, DescriptorType descriptor_type,
                  bool is_global);

   void Reset() {
     slot_index_ = UINT32_MAX;
     descriptor_type_ = DescriptorType::UNDEFINED;
     is_global_ = false;
   }

  private:
   std::uint32_t slot_index_{UINT32_MAX};
   DescriptorType descriptor_type_{DescriptorType::UNDEFINED};
   bool is_global_{false};
 };

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
  VkWriteDescriptorSet GetGlobalSamplerTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetGlobalSamplerTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetGlobalSamplerTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetGlobalStorageTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetGlobalStorageTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetGlobalStorageTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetSamplerTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetSamplerTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetSamplerTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetStorageTexture2DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetStorageTexture3DDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  VkWriteDescriptorSet GetStorageTextureCUBEDescriptorWriteInfo(
      std::uint32_t dest_array_element, std::uint32_t descriptor_count,
      const VkDescriptorImageInfo* image_info) const;

  void UpdateDescriptorSet(std::uint32_t descriptor_write_count,
                           const VkWriteDescriptorSet* write_descriptor_sets);

  Result<DescriptorSlot> AllocateSlot(
                             DescriptorType descriptor_type, bool is_global);

  Result<std::vector<DescriptorSlot>> AllocateSlot(
      const std::uint32_t& need_free_count,
                             DescriptorType descriptor_type, bool is_global);

  void FreeSlot(DescriptorSlot&& free_slot);

  void FreeSlot(std::uint32_t need_free_slot, DescriptorSlot* free_slots);

  void FreeSlot(std::vector<DescriptorSlot>&& free_slots);

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
  Result<Nil> InitDescriptorManager(
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

  Result<Nil> CreatePipelineLayout();

  void CleanPipelineLayout();

  static Result<DescriptorSlot> AllocateSlot(
      DescriptorType descriptor_type, bool is_global,
      std::atomic_uint32_t& last_use_index,
      std::vector<std::atomic_flag>& array_use_info);

  static Result<std::vector<DescriptorSlot>> AllocateSlot(
                             DescriptorType descriptor_type, bool is_global,
                             std::uint32_t need_free_count,
                             std::atomic_uint32_t& last_use_index,
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
