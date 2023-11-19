#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

#include "runtime/function/render/RenderResourceDataBase.h"
#include "runtime/function/render/vk_type_define.h"
#include "runtime/function/render/vk_utils.h"
#include "runtime/resource/asset_system/AssetManager.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

class AllocatedBufferWrapper {
 public:
  AllocatedBufferWrapper() = default;
  ~AllocatedBufferWrapper();
  AllocatedBufferWrapper(const VmaAllocator& allocator, const VkBuffer& buffer,
                         const VmaAllocation& allocation);
  AllocatedBufferWrapper(const AllocatedBufferWrapper& other) = delete;
  AllocatedBufferWrapper(AllocatedBufferWrapper&& other) noexcept;
  AllocatedBufferWrapper& operator=(const AllocatedBufferWrapper& other) =
      delete;
  AllocatedBufferWrapper& operator=(AllocatedBufferWrapper&& other) noexcept;

 public:
  VmaAllocator GetAllocator() const;

  VkBuffer GetBuffer();

  VmaAllocation GetAllocation();

  const VkBuffer_T* GetBuffer() const;

  const VmaAllocation_T* GetAllocation() const;

  void SetAllocator(VmaAllocator allocator);

  void SetBuffer(VkBuffer buffer);

  void SetAllocation(VmaAllocation allocation);

  void Release();

  bool IsValid() const;

 private:
  VmaAllocator allocator_{nullptr};
  VkBuffer buffer_{nullptr};
  VmaAllocation allocation_{nullptr};
};

class AllocatedBuffer final : public RenderResourceDataBase {
 public:
  AllocatedBuffer() = default;
  virtual ~AllocatedBuffer() override = default;
  AllocatedBuffer(
      const std::string& name,
      const RenderResourceDataID& render_resource_data_ID,
      RenderEngine* render_engine,
      const BufferDataInfo& buffer_data_info,
      VmaAllocator vma_allocator, VkBuffer vk_buffer,
      VmaAllocation vk_allocation);
  AllocatedBuffer(const std::string& name, RenderEngine* render_engine,
                  const VkBufferCreateInfo* vk_buffer_create_info,
                  const VmaAllocationCreateInfo* vma_allocation_create_info);
  AllocatedBuffer(const std::string& name, RenderEngine* render_engine,
                  AssetSystem::AssetManager::HandlerType asset_handler,
                  const VkBufferCreateInfo* vk_buffer_create_info,
                  const VmaAllocationCreateInfo* vma_allocation_create_info);
  AllocatedBuffer(const AllocatedBuffer& other) = delete;
  AllocatedBuffer(AllocatedBuffer&& other) noexcept;
  AllocatedBuffer& operator=(const AllocatedBuffer& other) = delete;
  AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept;

 public:
  const VkDeviceSize& GetBufferSize() const;

  RenderEngine* GetRenderEnginePtr();

  const RenderEngine* GetRenderEnginePtr() const;

  bool CanMapped() const;

  bool IsTransformSrc() const;

  bool IsTransformDest() const;

  const BufferDataInfo& GetBufferInfo() const;

  VmaAllocator GetAllocator() const;

  VkBuffer GetBuffer();

  VmaAllocation GetAllocation();

  const VkBuffer_T* GetBuffer() const;

  const VmaAllocation_T* GetAllocation() const;

  const std::vector<BufferSubResourceAttribute>& GetSubResourceAttributes()
      const;

  Result<Nil> TransformSubResourceAttribute(
      const std::vector<BufferSubResourceAttribute>&
          new_sub_resource_attribute);

  const BufferDataInfo& GetBufferDataInfo() const;

  const BufferCreateInfo& GetBufferCreateInfo() const;

  const AllocationCreateInfo& GetAllocationCreateInfo() const;

  ResourceType GetResourceType() const override;

  VkDeviceSize GetSize() const override;

  bool IsArray() const override;

  bool CanWrite() const override;

  //  // The ownership of resources within the scope of a ownership conversion
  //  // operation must be the same.
  //  ExecuteResult TransformQueueFamily(const BufferChunkInfo&
  //  buffer_chunk_info,
  //                                     std::uint32_t new_queue_family_index);

  Result<Nil> CopyDataToBuffer(std::uint64_t dest_offset, void* data,
                                 std::uint64_t src_offset, std::uint64_t size);

  Result<Nil> CopyDataToBuffer(
      const std::tuple<std::uint64_t, void*, std::uint64_t, std::uint64_t>*
          copy_info,
      std::uint32_t count);

  Result<Nil> CopyDataToBuffer(
      const std::vector<std::tuple<std::uint64_t, void*, std::uint64_t,
                                   std::uint64_t>>& copy_info);

  Result<Nil> CopyAssetDataToBuffer(
      AssetSystem::AssetManager::HandlerType asset_handler,
      std::uint32_t queue_index);

  Result<std::vector<VkBufferMemoryBarrier2>> GetVkBufferMemoryBarriber2(
      VkDeviceSize offset, VkDeviceSize size,
      VkPipelineStageFlags2 src_stage_mask, VkAccessFlags2 src_access_mask,
      VkPipelineStageFlags2 dst_stage_mask, VkAccessFlags2 dst_access_mask,
      QueueIndex new_index) const;

  Result<AllocatedBuffer> GetCopy(const std::string& new_name) const;

  Result<std::vector<AllocatedBuffer>> GetCopy(
      std::vector<std::string>& new_names) const;

  void Release() override;

  bool IsValid() const override;

 private:
  static Result<Nil> CheckInitParameters(
      RenderEngine* render_engine,
      const VkBufferCreateInfo* vk_buffer_create_info,
      const VmaAllocationCreateInfo* vma_allocation_create_info);

  static Result<Nil> CheckInitParametersWhenInitFromAnAsset(
      RenderEngine* render_engine,
      AssetSystem::AssetManager::HandlerType asset_handler,
      const VkBufferCreateInfo* vk_buffer_create_info,
      const VmaAllocationCreateInfo* vma_allocation_create_info);

  Result<Nil> InitBuffer(
      RenderEngine* render_engine,
      const VkBufferCreateInfo& vk_buffer_create_info,
      const VmaAllocationCreateInfo& vma_allocation_create_info);

  Result<Nil> AddCopyBufferCommandsWhenOneSubResource(
      AllocatedCommandBuffer& cmd, VkBuffer new_buffer) const;

  Result<Nil> AddCopyBufferCommandseWhenMultSubResource(
      AllocatedCommandBuffer& cmd, VkBuffer new_buffer) const;

  Result<Nil> AddCopyBufferCommandsWhenOneSubResource(
      AllocatedCommandBuffer& cmd, std::vector<VkBuffer>& new_buffers) const;

  Result<Nil> AddCopyBufferCommandseWhenMultSubResource(
      AllocatedCommandBuffer& cmd, std::vector<VkBuffer>& new_buffers) const;

  Result<Nil> CheckTransformInputParameter(
      const std::vector<BufferSubResourceAttribute>& new_sub_resource_attribute)
      const;

 private:
  RenderEngine* render_engine_{nullptr};
  BufferDataInfo buffer_data_info_{};
  AllocatedBufferWrapper wrapper_{};
};
}  // namespace RenderSystem
}  // namespace MM
