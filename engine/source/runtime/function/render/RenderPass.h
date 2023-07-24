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

using RenderPassID = MM::Utils::ID5;

class RenderPass : public MMObject {
  friend class RenderEngine;

  class RenderPassWrapper;

 public:
  using RenderPassContainerType =
      MM::Utils::ConcurrentMap<RenderPassID, RenderPassWrapper>;

 public:
  RenderPass() = default;
  ~RenderPass() = default;
  explicit RenderPass(VkDevice device, VkAllocationCallbacks* allocator,
                      const VkRenderPassCreateInfo& vk_render_pass_create_info);
  explicit RenderPass(VkDevice device, VkAllocationCallbacks* allocator,
                      const RenderPassCreateInfo& render_pass_create_info);
  RenderPass(const RenderPass& other) = delete;
  RenderPass(RenderPass&& other) noexcept;
  RenderPass& operator=(const RenderPass& other) = default;
  RenderPass& operator=(RenderPass&& other) noexcept;

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

  bool IsValid() const override;

  void Reset() override;

  static RenderPassID GetRenderPassID(
      const RenderPassCreateInfo& render_pass_create_info);

 private:
  ExecuteResult CheckInitParameters(
      VkDevice device,
      const RenderPassCreateInfo& render_pass_create_info) const;

  ExecuteResult InitRenderPass(
      VkDevice device, VkAllocationCallbacks* allocator,
      const VkRenderPassCreateInfo& vk_render_pass_create_info);

 private:
  class RenderPassWrapper {
   public:
    RenderPassWrapper() = default;
    ~RenderPassWrapper();
    RenderPassWrapper(VkDevice device, VkAllocationCallbacks* allocator,
                      VkRenderPass render_pass);
    RenderPassWrapper(const RenderPassWrapper& other) = delete;
    RenderPassWrapper(RenderPassWrapper&& other) noexcept;
    RenderPassWrapper& operator=(const RenderPassWrapper& other) = delete;
    RenderPassWrapper& operator=(RenderPassWrapper&& other) noexcept;

   public:
    VkDevice GetDevice() const;

    VkAllocationCallbacks* GetAllocator() const;

    VkRenderPass GetRenderPass();

    const VkRenderPass_T* GetRenderPass() const;

    bool IsValid() const;

    void Release();

   private:
    VkDevice device_{nullptr};
    VkAllocationCallbacks* allocator_{nullptr};
    VkRenderPass render_pass_{nullptr};
  };

 private:
  // TODO render_engine recovery
  static RenderPassContainerType render_pass_container_;

 private:
  RenderPassCreateInfo render_pass_create_info_{};

  RenderPassWrapper* wrapper_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM
