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
  AllocatedImage(const std::string& name, RenderEngine* render_engine,
                 AssetSystem::AssetManager::HandlerType image_handler,
                 const VkImageCreateInfo* vk_image_create_info,
                 const VmaAllocationCreateInfo* vma_allocation_create_info);
  AllocatedImage(const AllocatedImage& other) = delete;
  AllocatedImage(AllocatedImage&& other) noexcept;
  AllocatedImage& operator=(const AllocatedImage& other) = delete;
  AllocatedImage& operator=(AllocatedImage&& other) noexcept;

 public:
  std::uint32_t GetQueueIndex() const;

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
