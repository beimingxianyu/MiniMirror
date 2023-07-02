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
                 const VmaAllocator& allocator, const VkImage& image,
                 const VmaAllocation& allocation,
                 const ImageDataInfo& image_data_info);
  AllocatedImage(const std::string& name, RenderEngine* render_engine,
                 AssetSystem::AssetManager::HandlerType image_handler,
                 const VkImageCreateInfo* vk_image_create_info,
                 const VmaAllocationCreateInfo* vma_allocation_create_info);
  AllocatedImage(const AllocatedImage& other) = delete;
  AllocatedImage(AllocatedImage&& other) noexcept;
  AllocatedImage& operator=(const AllocatedImage& other) = delete;
  AllocatedImage& operator=(AllocatedImage&& other) noexcept;

 public:
  const VkExtent3D& GetImageExtent() const;

  const VkDeviceSize& GetImageSize() const;

  const VkFormat& GetImageFormat() const;

  const VkImageLayout& GetImageLayout() const;

  const uint32_t& GetMipmapLevels() const;

  const uint32_t& GetArrayLayers() const;

  bool CanMapped() const;

  bool IsTransformSrc() const;

  bool IsTransformDest() const;

  const ImageCreateInfo& GetImageCreateInfo() const;

  const AllocationCreateInfo& GetAllocationCreateInfo() const;

  const ImageDataInfo& GetImageInfo() const;

  VmaAllocator GetAllocator() const;

  VkImage GetImage() const;

  VmaAllocation GetAllocation() const;

  const std::vector<std::uint32_t>& GetQueueIndexes() const;

  void Release() override;

  bool IsValid() const override;

  ResourceType GetResourceType() const override;
  VkDeviceSize GetSize() const override;
  bool IsArray() const override;
  bool CanWrite() const override;
  std::unique_ptr<RenderResourceDataBase> GetCopy(
      const std::string& new_name_of_copy_resource) const override;

 private:
  static ExecuteResult CheckImageHandler(
      const AssetSystem::AssetManager::HandlerType& image_handler);

  static ExecuteResult CheckVkImageCreateInfo(
      const VkImageCreateInfo* vk_image_create_info);

  static ExecuteResult CheckVmaAllocationCreateInfo(
      const VmaAllocationCreateInfo* vma_allocation_create_info);

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
  ImageDataInfo image_data_info_{};
  AllocatedImageWrapper wrapper_{};
};
}  // namespace RenderSystem
}  // namespace MM
