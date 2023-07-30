//
// Created by beimingxianyu on 23-7-23.
//
#pragma once

#include <vulkan/vulkan.h>

#include "runtime/function/render/vk_type_define.h"
#include "runtime/platform/base/MMObject.h"
#include "utils/hash_table.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

class RenderPassAttachmentDescription {
  friend class RenderEngine;

  class RenderPassAttachmentDescriptionWrapper;

 public:
  using RenderPassContainerType =
      MM::Utils::ConcurrentMap<RenderPassID,
                               RenderPassAttachmentDescriptionWrapper>;

 public:
  RenderPassAttachmentDescription() = default;
  ~RenderPassAttachmentDescription() = default;
  RenderPassAttachmentDescription(
      RenderEngine* render_engine, VkAllocationCallbacks* allocator,
      const VkRenderPassCreateInfo& vk_render_pass_create_info);
  RenderPassAttachmentDescription(
      RenderEngine* render_engine, VkAllocationCallbacks* allocator,
      const RenderPassCreateInfo& render_pass_create_info);
  RenderPassAttachmentDescription(
      RenderEngine* render_engine, VkAllocationCallbacks* allocator,
      RenderPassCreateInfo&& render_pass_create_info);
  RenderPassAttachmentDescription(
      const RenderPassAttachmentDescription& other) = delete;
  RenderPassAttachmentDescription(
      RenderPassAttachmentDescription&& other) noexcept;
  RenderPassAttachmentDescription& operator=(
      const RenderPassAttachmentDescription& other) = default;
  RenderPassAttachmentDescription& operator=(
      RenderPassAttachmentDescription&& other) noexcept;

 public:
  const RenderPassCreateInfo& GetRenderPassCreateInfo() const;

  const std::vector<VkAttachmentDescription>& GetVkAttachmentDescriptions()
      const;

  const std::vector<VkSubpassDescription>& GetSubpassDescriptions() const;

  const std::vector<VkSubpassDependency>& GetVkDependencyInfos() const;

  VkDevice GetDevice() const;

  VkAllocationCallbacks* GetAllocator() const;

  VkRenderPass GetRenderPass();

  const VkRenderPass_T* GetRenderPass() const;

  ExecuteResult GetRenderPassID(RenderPassID& render_pass_ID) const;

  bool IsValid() const;

  void Reset();

 private:
  static ExecuteResult CheckInitParameters(
      RenderEngine* render_engine,
      const RenderPassCreateInfo& render_pass_create_info);

  static ExecuteResult CheckInitParameters(
      RenderEngine* render_engine,
      const VkRenderPassCreateInfo& render_pass_create_info);

  ExecuteResult InitRenderPass(
      RenderEngine* render_engine, VkAllocationCallbacks* allocator,
      const VkRenderPassCreateInfo& vk_render_pass_create_info);

  ExecuteResult InitRenderPass(
      RenderEngine* render_engine, VkAllocationCallbacks* allocator,
      const RenderPassCreateInfo& render_pass_create_info);

  ExecuteResult InitRenderPass(RenderEngine* render_engine,
                               VkAllocationCallbacks* allocator,
                               RenderPassCreateInfo&& render_pass_create_info);

 private:
  class RenderPassAttachmentDescriptionWrapper {
   public:
    RenderPassAttachmentDescriptionWrapper() = default;
    ~RenderPassAttachmentDescriptionWrapper();
    RenderPassAttachmentDescriptionWrapper(
        RenderEngine* render_engine, VkAllocationCallbacks* allocator,
        VkRenderPass render_pass,
        RenderPassCreateInfo&& render_pass_attachment_description_create_info);
    RenderPassAttachmentDescriptionWrapper(
        const RenderPassAttachmentDescriptionWrapper& other) = delete;
    RenderPassAttachmentDescriptionWrapper(
        RenderPassAttachmentDescriptionWrapper&& other) noexcept;
    RenderPassAttachmentDescriptionWrapper& operator=(
        const RenderPassAttachmentDescriptionWrapper& other) = delete;
    RenderPassAttachmentDescriptionWrapper& operator=(
        RenderPassAttachmentDescriptionWrapper&& other) noexcept;

   public:
    VkDevice GetDevice() const;

    VkAllocationCallbacks* GetAllocator() const;

    VkRenderPass GetRenderPass();

    const VkRenderPass_T* GetRenderPass() const;

    const RenderPassCreateInfo& GetRenderPassAttachmentDescriptorCreateInfo()
        const;

    const std::vector<VkAttachmentDescription>& GetVkAttachmentDescriptions()
        const;

    const std::vector<VkSubpassDescription>& GetSubpassDescriptions() const;

    const std::vector<VkSubpassDependency>& GetVkDependencyInfos() const;

    bool IsValid() const;

    void Release();

   private:
    RenderEngine* render_engine_{nullptr};
    VkAllocationCallbacks* allocator_{nullptr};
    VkRenderPass render_pass_{nullptr};

    RenderPassCreateInfo render_pass_attachment_description_create_info_{};
  };

 private:
  // TODO render_engine recovery
  static RenderPassContainerType render_pass_container_;

 private:
  RenderPassAttachmentDescriptionWrapper* wrapper_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM
