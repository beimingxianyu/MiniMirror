#include <vector>

#include "runtime/function/render/RenderResourceDataBase.h"
#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {
class AllocatedImage final : public RenderResourceDataBase {
  friend class RenderResourceTexture;

 public:
  AllocatedImage() = default;
  ~AllocatedImage() = default;
  AllocatedImage(const std::string& name,
                 const RenderResourceDataID& render_resource_data_ID,
                 const VmaAllocator& allocator, const VkImage& image,
                 const VmaAllocation& allocation,
                 const ImageDataInfo& image_data_info);
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

  void Release();

  bool IsValid() const;

 private:
  class AllocatedImageWrapper {
    friend class RenderEngine;
    friend class RenderResourceTexture;

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
