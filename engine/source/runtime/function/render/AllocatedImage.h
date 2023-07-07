#include <vulkan/vulkan_core.h>

#include <vector>

#include "RenderResourceDataID.h"
#include "runtime/function/render/RenderResourceDataBase.h"
#include "runtime/function/render/vk_type_define.h"
#include "runtime/platform/base/error.h"
#include "runtime/resource/asset_system/AssetManager.h"
#include "runtime/resource/asset_system/AssetSystem.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "utils/marco.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

class AllocatedImage final : public RenderResourceDataBase {
 public:
  AllocatedImage() = default;
  ~AllocatedImage() = default;
  AllocatedImage(const std::string& name,
                 const RenderResourceDataID& render_resource_data_ID,
                 RenderEngine* render_engine,
                 const ImageDataInfo& image_data_info,
                 const VmaAllocator& allocator, const VkImage& image,
                 const VmaAllocation& allocation, bool have_data = false);
  AllocatedImage(const std::string& name,
                 MM::RenderSystem::RenderEngine* render_engine,
                 MM::AssetSystem::AssetManager::HandlerType image_handler,
                 VkImageLayout image_layout,
                 const VkImageCreateInfo* vk_image_create_info,
                 const VmaAllocationCreateInfo* vma_allocation_create_info);
  AllocatedImage(const AllocatedImage& other) = delete;
  AllocatedImage(AllocatedImage&& other) noexcept;
  AllocatedImage& operator=(const AllocatedImage& other) = delete;
  AllocatedImage& operator=(AllocatedImage&& other) noexcept;

 public:
  const std::vector<ImageSubResourceAttribute>& GetImageSubResourceAttributes()
      const;

  // TODO complete this
  // The ownership of resources within the scope of a
  // ownership conversion operation must be the same.
  //  ExecuteResult TransformSubResourceAttribute(
  //      const std::vector<ImageSubResourceAttribute>&
  //          new_image_sub_resource_attribute);

  const VkExtent3D& GetImageExtent() const;

  VkDeviceSize GetImageSize() const;

  VkFormat GetImageFormat() const;

  VkImageLayout GetImageLayout() const;

  std::uint32_t GetMipmapLevels() const;

  std::uint32_t GetArrayLayers() const;

  bool CanMapped() const;

  bool IsTransformSrc() const;

  bool IsTransformDest() const;

  const ImageCreateInfo& GetImageCreateInfo() const;

  const AllocationCreateInfo& GetAllocationCreateInfo() const;

  const ImageDataInfo& GetImageInfo() const;

  VmaAllocator GetAllocator() const;

  VkImage GetImage() const;

  VmaAllocation GetAllocation() const;

  const std::vector<ImageSubResourceAttribute>& GetQueueIndexes() const;

  void Release() override;

  bool IsValid() const override;

  ResourceType GetResourceType() const override;

  VkDeviceSize GetSize() const override;

  ExecuteResult TransformQueueFamily(std::uint32_t new_queue_family_index);

  bool IsArray() const override;

  bool CanWrite() const override;

 private:
  static ExecuteResult CheckImageHandler(
      const AssetSystem::AssetManager::HandlerType& image_handler);

  static ExecuteResult CheckInitParameters(
      RenderEngine* render_engine,
      const AssetSystem::AssetManager::HandlerType& image_handler,
      VkImageLayout image_layout, const VkImageCreateInfo* vk_image_create_info,
      const VmaAllocationCreateInfo* vma_allocation_create_info);

  static ExecuteResult CheckInitParametersWhenInitFromAnAsset(
      RenderEngine* render_engine,
      const AssetSystem::AssetManager::HandlerType& image_handler,
      VkImageLayout image_layout, const VkImageCreateInfo* vk_image_create_info,
      const VmaAllocationCreateInfo* vma_allocation_create_info);

  ExecuteResult LoadImageDataToStageBuffer(
      AssetSystem::AssetType::Image* image_data,
      AllocatedBuffer& stage_allocated_buffer);

  void AddInitLayoutAndQueueIndexTransformCommands(
      MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage created_image);

  void AddCopyStageBufferDataToImageCommands(
      MM::RenderSystem::AllocatedCommandBuffer& cmd,
      AllocatedBuffer& stage_allocated_buffer, VkImage created_image);

  void AddGenerateMipmapsCommandsAndAddQueueIndexAndLayoutTransformCommands(
      MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage created_image);

  void AddQueueIndexAndLayoutTransformCommands(
      MM::RenderSystem::AllocatedCommandBuffer& cmd, VkImage created_image);

  MM::ExecuteResult InitImage(
      MM::RenderSystem::AllocatedBuffer& stage_allocated_buffer,
      const VkImageCreateInfo* vk_image_create_info,
      const VmaAllocationCreateInfo* vma_allocation_create_info);

 private:
  class AllocatedImageWrapper {
   public:
    AllocatedImageWrapper() = default;
    ~AllocatedImageWrapper();
    AllocatedImageWrapper(const VmaAllocator& allocator, const VkImage& image,
                          const VmaAllocation& allocation);
    AllocatedImageWrapper(const AllocatedImageWrapper& other) = delete;
    AllocatedImageWrapper(AllocatedImageWrapper&& other) noexcept;
    AllocatedImageWrapper& operator=(const AllocatedImageWrapper& other) =
        delete;
    AllocatedImageWrapper& operator=(AllocatedImageWrapper&& other) noexcept;

   public:
    VmaAllocator GetAllocator() const;

    VkImage GetImage() const;

    VmaAllocation GetAllocation() const;

    bool IsValid() const;

    void Release();

   private:
    VmaAllocator allocator_{nullptr};
    VkImage image_{nullptr};
    VmaAllocation allocation_{nullptr};
  };

 private:
  RenderEngine* render_engine_{nullptr};
  ImageDataInfo image_data_info_{};
  AllocatedImageWrapper wrapper_{};
};
}  // namespace RenderSystem
}  // namespace MM
