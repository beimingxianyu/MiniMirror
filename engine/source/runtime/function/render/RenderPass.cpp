//
// Created by beimingxianyu on 23-7-23.
//

#include "runtime/function/render/RenderPass.h"

MM::RenderSystem::RenderPass::RenderPass(
    VkDevice device, VkAllocationCallbacks* allocator,
    const VkRenderPassCreateInfo& vk_render_pass_create_info)
    : render_pass_create_info_(vk_render_pass_create_info), wrapper_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, render_pass_create_info_),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  MM_CHECK(
      InitRenderPass(device, allocator, vk_render_pass_create_info),
      MM_LOG_ERROR("Failed to initialization MM::RenderSystem::RenderPass.");)
}

MM::RenderSystem::RenderPass::RenderPass(
    VkDevice device, VkAllocationCallbacks* allocator,
    const MM::RenderSystem::RenderPassCreateInfo& render_pass_create_info)
    : render_pass_create_info_(render_pass_create_info), wrapper_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(device, render_pass_create_info_),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  VkRenderPassCreateInfo vk_render_pass_create_info =
      render_pass_create_info.GetVkRenderPassCreateInfo();

  MM_CHECK(
      InitRenderPass(device, allocator, vk_render_pass_create_info),
      MM_LOG_ERROR("Failed to initialization MM::RenderSystem::RenderPass.");)
}

MM::ExecuteResult MM::RenderSystem::RenderPass::CheckInitParameters(
    VkDevice device,
    const MM::RenderSystem::RenderPassCreateInfo& render_pass_create_info)
    const {
  if (device == nullptr) {
    MM_LOG_ERROR("The input parameter device must invalid.");
    return MM::ExecuteResult ::INITIALIZATION_FAILED;
  }

  for (const auto& attachment_description : GetVkAttachmentDescriptions()) {
    if (attachment_description.flags == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.format == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.samples == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.loadOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.storeOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.stencilLoadOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.stencilStoreOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.initialLayout == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (attachment_description.finalLayout == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
  }
}

MM::ExecuteResult MM::RenderSystem::RenderPass::InitRenderPass(
    VkDevice device, VkAllocationCallbacks* allocator,
    const VkRenderPassCreateInfo& vk_render_pass_create_info) {
  RenderPassID render_pass_ID = GetRenderPassID(render_pass_create_info_);

  RenderPassContainerType ::FindResultType find_result =
      render_pass_container_.Find(render_pass_ID);

  if (find_result) {
    wrapper_ = &(find_result->second);
  } else {
    VkRenderPass created_render_pass;
    MM_VK_CHECK(vkCreateRenderPass(device, &vk_render_pass_create_info,
                                   allocator, &created_render_pass),
                MM_LOG_ERROR("Failed to create VkRenderPass.");)

    RenderPassWrapper wrapper{device, allocator, created_render_pass};

    auto emplace_result = render_pass_container_.Emplace(
        std::make_pair(render_pass_ID, std::move(wrapper)));
    if (emplace_result.second) {
      wrapper_ = &(emplace_result.first.second);
    } else {
      std::uint32_t failed_count = 0;
      while (!(find_result = render_pass_container_.Find(render_pass_ID))) {
        auto new_emplace_result = render_pass_container_.Emplace(
            std::make_pair(render_pass_ID, std::move(wrapper)));
        if (new_emplace_result.second) {
          wrapper_ = &(new_emplace_result.first.second);
          return ExecuteResult ::SUCCESS;
        }

        ++failed_count;

        if (failed_count == 3) {
          MM_LOG_ERROR(
              "Failed to get or insert RenderPassWrapper "
              "multiple times.");
          return ExecuteResult ::INITIALIZATION_FAILED;
        }
      }

      wrapper_ = &(find_result->second);
    }
  }
}

MM::RenderSystem::RenderPass::RenderPass(
    MM::RenderSystem::RenderPass&& other) noexcept
    : render_pass_create_info_(std::move(other.render_pass_create_info_)),
      wrapper_(other.wrapper_) {
  other.wrapper_ = nullptr;
}

MM::RenderSystem::RenderPass& MM::RenderSystem::RenderPass::operator=(
    MM::RenderSystem::RenderPass&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  render_pass_create_info_ = std::move(other.render_pass_create_info_);
  wrapper_ = other.wrapper_;

  other.wrapper_ = nullptr;

  return *this;
}

const MM::RenderSystem::RenderPassCreateInfo&
MM::RenderSystem::RenderPass::GetRenderPassCreateInfo() const {
  return render_pass_create_info_;
}

const std::vector<VkAttachmentDescription>&
MM::RenderSystem::RenderPass::GetVkAttachmentDescriptions() const {
  return render_pass_create_info_.attachments_;
}

const std::vector<VkSubpassDescription>&
MM::RenderSystem::RenderPass::GetSubpassDescriptions() const {
  return render_pass_create_info_.subpasses_;
}

const std::vector<VkSubpassDependency>&
MM::RenderSystem::RenderPass::GetVkDependencyInfos() const {
  return render_pass_create_info_.dependencies_;
}

VkDevice MM::RenderSystem::RenderPass::GetDevice() const {
  return wrapper_->GetDevice();
}

VkAllocationCallbacks* MM::RenderSystem::RenderPass::GetAllocator() const {
  return wrapper_->GetAllocator();
}

VkRenderPass MM::RenderSystem::RenderPass::GetRenderPass() {
  wrapper_->GetRenderPass();
}

const VkRenderPass_T* MM::RenderSystem::RenderPass::GetRenderPass() const {
  return wrapper_->GetRenderPass();
}

bool MM::RenderSystem::RenderPass::IsValid() const {
  return wrapper_ != nullptr;
}
void MM::RenderSystem::RenderPass::Reset() {
  MMObject::Reset();
  render_pass_create_info_.Reset();
  wrapper_ = nullptr;
}

MM::RenderSystem::RenderPassID MM::RenderSystem::RenderPass::GetRenderPassID(
    const MM::RenderSystem::RenderPassCreateInfo& render_pass_create_info) {
  RenderPassID render_pass_ID{0, 0, 0, 0, 0};

  std::uint64_t sub_ID1 = 0;
  std::uint32_t cycle_offset1 = 0;

  // sub_ID1
  sub_ID1 |= render_pass_create_info.dependencies_.size();
  sub_ID1 |= render_pass_create_info.subpasses_.size() << 5;
  sub_ID1 |= render_pass_create_info.attachments_.size() << 10;
  sub_ID1 |= render_pass_create_info.flags_ << 15;
  sub_ID1 |= reinterpret_cast<std::uint64_t>(render_pass_create_info.next_)
             << 17;
  render_pass_ID.SetSubID1(sub_ID1);
  sub_ID1 = 0;

  // sub_ID2
  cycle_offset1 = 13;
  for (const auto& attachment : render_pass_create_info.attachments_) {
    sub_ID1 |= attachment.flags << cycle_offset1;
    cycle_offset1 += 1;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= attachment.format << cycle_offset1;
    cycle_offset1 += 8;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= attachment.loadOp << cycle_offset1;
    cycle_offset1 += 4;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= attachment.stencilLoadOp << cycle_offset1;
    cycle_offset1 += 4;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= attachment.storeOp << cycle_offset1;
    cycle_offset1 += 3;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= attachment.stencilStoreOp << cycle_offset1;
    cycle_offset1 += 3;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= attachment.initialLayout << cycle_offset1;
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= attachment.finalLayout << cycle_offset1;
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
  }
  render_pass_ID.SetSubID2(sub_ID1);
  sub_ID1 = 0;
  cycle_offset1 = 0;

  for (const auto& subpass : render_pass_create_info.subpasses_) {
    sub_ID1 |= subpass.flags << cycle_offset1;
    cycle_offset1 += 7;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= subpass.pipelineBindPoint << cycle_offset1;
    cycle_offset1 += 4;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= subpass.inputAttachmentCount << cycle_offset1;
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= subpass.colorAttachmentCount << cycle_offset1;
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= subpass.preserveAttachmentCount << cycle_offset1;
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    for (std::uint64_t i = 0; i != subpass.inputAttachmentCount; ++i) {
      sub_ID1 |= subpass.pInputAttachments[i].attachment << cycle_offset1;
      cycle_offset1 += 5;
      if (cycle_offset1 > 63) cycle_offset1 -= 64;
      sub_ID1 |= subpass.pInputAttachments[i].layout << cycle_offset1;
      cycle_offset1 += 5;
      if (cycle_offset1 > 63) cycle_offset1 -= 64;
    }
    if (subpass.pColorAttachments) {
      for (std::uint64_t i = 0; i != subpass.colorAttachmentCount; ++i) {
        sub_ID1 |= subpass.pColorAttachments[i].attachment << cycle_offset1;
        cycle_offset1 += 5;
        if (cycle_offset1 > 63) cycle_offset1 -= 64;
        sub_ID1 |= subpass.pColorAttachments[i].layout << cycle_offset1;
        cycle_offset1 += 5;
        if (cycle_offset1 > 63) cycle_offset1 -= 64;
      }
    } else if (subpass.pResolveAttachments) {
      for (std::uint64_t i = 0; i != subpass.colorAttachmentCount; ++i) {
        sub_ID1 |= subpass.pResolveAttachments[i].attachment << cycle_offset1;
        cycle_offset1 += 5;
        if (cycle_offset1 > 63) cycle_offset1 -= 64;
        sub_ID1 |= subpass.pResolveAttachments[i].layout << cycle_offset1;
        cycle_offset1 += 5;
        if (cycle_offset1 > 63) cycle_offset1 -= 64;
      }
    } else {
      sub_ID1 |= static_cast<std::uint64_t>(0) << cycle_offset1;
      cycle_offset1 += 10;
      if (cycle_offset1 > 63) cycle_offset1 -= 64;
    }
    for (std::uint64_t i = 0; i != subpass.colorAttachmentCount; ++i) {
    }
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    if (subpass.pDepthStencilAttachment) {
      sub_ID1 |= subpass.pDepthStencilAttachment->attachment << cycle_offset1;
      cycle_offset1 += 5;
      if (cycle_offset1 > 63) cycle_offset1 -= 64;
      sub_ID1 |= subpass.pDepthStencilAttachment->layout << cycle_offset1;
      cycle_offset1 += 5;
      if (cycle_offset1 > 63) cycle_offset1 -= 64;
    } else {
      sub_ID1 |= static_cast<std::uint64_t>(0) << cycle_offset1;
      cycle_offset1 += 10;
      if (cycle_offset1 > 63) cycle_offset1 -= 64;
    }
    for (std::uint64_t i = 0; i != subpass.preserveAttachmentCount; ++i) {
      sub_ID1 |= static_cast<std::uint64_t>(subpass.pPreserveAttachments[i])
                 << cycle_offset1;
      cycle_offset1 += 5;
      if (cycle_offset1 > 63) cycle_offset1 -= 64;
    }
  }
  render_pass_ID.SetSubID3(sub_ID1);
  sub_ID1 = 0;
  cycle_offset1 = 0;

  std::uint64_t sub_ID2 = 0;
  std::uint64_t cycle_offset2 = 0;
  for (const auto& dependency : render_pass_create_info.dependencies_) {
    sub_ID1 |= dependency.srcSubpass << cycle_offset1;
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= dependency.dstSubpass << cycle_offset1;
    cycle_offset1 += 5;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= dependency.srcStageMask << cycle_offset1;
    cycle_offset1 += 26;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID1 |= dependency.dstStageMask << cycle_offset1;
    cycle_offset1 += 26;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;

    sub_ID2 |= dependency.dependencyFlags << cycle_offset2;
    cycle_offset2 += 3;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID2 |= dependency.srcAccessMask << cycle_offset2;
    cycle_offset2 += 28;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
    sub_ID2 |= dependency.dstAccessMask << cycle_offset2;
    cycle_offset2 += 28;
    if (cycle_offset1 > 63) cycle_offset1 -= 64;
  }
  render_pass_ID.SetSubID4(sub_ID1);
  render_pass_ID.SetSubID5(sub_ID2);

  return render_pass_ID;
}

MM::RenderSystem::RenderPass::RenderPassWrapper::~RenderPassWrapper() {
  if (IsValid()) {
    vkDestroyRenderPass(device_, render_pass_, allocator_);
  }
}

MM::RenderSystem::RenderPass::RenderPassWrapper::RenderPassWrapper(
    VkDevice device, VkAllocationCallbacks* allocator, VkRenderPass render_pass)
    : device_(device), allocator_(allocator), render_pass_(render_pass) {}

MM::RenderSystem::RenderPass::RenderPassWrapper::RenderPassWrapper(
    MM::RenderSystem::RenderPass::RenderPassWrapper&& other) noexcept
    : device_(other.device_),
      allocator_(other.allocator_),
      render_pass_(other.render_pass_) {}

MM::RenderSystem::RenderPass::RenderPassWrapper&
MM::RenderSystem::RenderPass::RenderPassWrapper::operator=(
    MM::RenderSystem::RenderPass::RenderPassWrapper&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  device_ = other.device_;
  allocator_ = other.allocator_;
  render_pass_ = other.render_pass_;

  other.device_ = nullptr;
  other.allocator_ = nullptr;
  other.render_pass_ = nullptr;

  return *this;
}

VkDevice MM::RenderSystem::RenderPass::RenderPassWrapper::GetDevice() const {
  return device_;
}

VkAllocationCallbacks*
MM::RenderSystem::RenderPass::RenderPassWrapper::GetAllocator() const {
  return allocator_;
}

VkRenderPass MM::RenderSystem::RenderPass::RenderPassWrapper::GetRenderPass() {
  return render_pass_;
}

const VkRenderPass_T*
MM::RenderSystem::RenderPass::RenderPassWrapper::GetRenderPass() const {
  return render_pass_;
}

bool MM::RenderSystem::RenderPass::RenderPassWrapper::IsValid() const {
  return device_ != nullptr && allocator_ != nullptr && render_pass_ != nullptr;
}

void MM::RenderSystem::RenderPass::RenderPassWrapper::Release() {
  if (IsValid()) {
    vkDestroyRenderPass(device_, render_pass_, allocator_);
  }

  device_ = nullptr;
  allocator_ = nullptr;
  render_pass_ = nullptr;
}
