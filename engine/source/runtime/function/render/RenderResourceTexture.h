#pragma once

#include "runtime/function/render/AllocatedImage.h"
#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

class RenderResourceTexture {
 public:
  RenderResourceTexture() = default;
  ~RenderResourceTexture() = default;
  RenderResourceTexture(AllocatedImage* allocated_image, Sampler&& sampler,
                        ImageView&& image_view);
  RenderResourceTexture(AllocatedImage* allocated_image,
                        const SamplerCreateInfo& sampler_create_info,
                        const ImageViewCreateInfo& image_view_create_info,
                        VkAllocationCallbacks* sampler_allocator,
                        VkAllocationCallbacks* image_view_allocator);
  RenderResourceTexture(AllocatedImage* allocated_image,
                        const VkSamplerCreateInfo& vk_sampler_create_info,
                        const VkImageViewCreateInfo& vk_image_view_create_info,
                        VkAllocationCallbacks* sampler_allocator,
                        VkAllocationCallbacks* image_view_allocator);
  RenderResourceTexture(const RenderResourceTexture& other) = delete;
  RenderResourceTexture(RenderResourceTexture&& other) noexcept;
  RenderResourceTexture& operator=(const RenderResourceTexture& other) = delete;
  RenderResourceTexture& operator=(RenderResourceTexture&& other) noexcept;

 public:
  const ImageDataInfo& GetImageDataInfo() const;

  const SamplerCreateInfo& GetSamplerCreateInfo() const;

  const ImageViewCreateInfo& GetImageViewCreateInfo() const;

  AllocatedImage& GetAllocatedImage();

  const AllocatedImage& GetAllocatedImage() const;

  Sampler& GetSampler() const;

  ImageView& GetImageView();

  const ImageView& GetImageView() const;

  VkImage GetImage();

  const VkImage_T* GetImage() const;

  VkSampler GetVkSampler() const;

  VkImageView GetVkImageView();

  const VkImageView_T* GetVkImageView() const;

  bool IsValid() const;

  void Reset();

 private:
  AllocatedImage* allocated_image_{nullptr};
  mutable Sampler sampler_{};
  ImageView image_view_{};
};
}  // namespace RenderSystem
}  // namespace MM
