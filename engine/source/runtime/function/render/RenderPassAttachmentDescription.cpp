//
// Created by beimingxianyu on 23-7-23.
//

#include "runtime/function/render/RenderPassAttachmentDescription.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescription(
        RenderEngine* render_engine, VkAllocationCallbacks* allocator,
        const VkRenderPassCreateInfo& vk_render_pass_create_info)
    : wrapper_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(render_engine, vk_render_pass_create_info),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  MM_CHECK(
      InitRenderPass(render_engine, allocator, vk_render_pass_create_info),
      MM_LOG_ERROR("Failed to initialization MM::RenderSystem::RenderPass.");)
}

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescription(
        RenderEngine* render_engine, VkAllocationCallbacks* allocator,
        const RenderPassCreateInfo& render_pass_create_info)
    : wrapper_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(render_engine, render_pass_create_info),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  VkRenderPassCreateInfo vk_render_pass_create_info =
      render_pass_create_info.GetVkRenderPassCreateInfo();

  MM_CHECK(
      InitRenderPass(render_engine, allocator, vk_render_pass_create_info),
      MM_LOG_ERROR("Failed to initialization MM::RenderSystem::RenderPass.");)
}

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescription(
        MM::RenderSystem::RenderEngine* render_engine,
        VkAllocationCallbacks* allocator,
        MM::RenderSystem::RenderPassCreateInfo&& render_pass_create_info) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(render_engine, render_pass_create_info),
           MM_LOG_ERROR("Input parameters is error.");
           return;)
#endif

  VkRenderPassCreateInfo vk_render_pass_create_info =
      render_pass_create_info.GetVkRenderPassCreateInfo();

  MM_CHECK(
      InitRenderPass(render_engine, allocator, vk_render_pass_create_info),
      MM_LOG_ERROR("Failed to initialization MM::RenderSystem::RenderPass.");)
}

MM::ExecuteResult
MM::RenderSystem::RenderPassAttachmentDescription::CheckInitParameters(
    RenderEngine* render_engine,
    const RenderPassCreateInfo& render_pass_create_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    MM_LOG_ERROR("The input parameter render engine must invalid.");
    return MM::ExecuteResult ::INITIALIZATION_FAILED;
  }

  for (const auto& attachment_description :
       render_pass_create_info.attachments_) {
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

MM::ExecuteResult
MM::RenderSystem::RenderPassAttachmentDescription::InitRenderPass(
    RenderEngine* render_engine, VkAllocationCallbacks* allocator,
    const VkRenderPassCreateInfo& vk_render_pass_create_info) {
  RenderPassID render_pass_ID;
  MM_CHECK(GetRenderPassID(render_pass_ID),
           MM_LOG_ERROR("MM::RenderSystem::RenderPassCreateInfo is invalid.");
           return ExecuteResult::INITIALIZATION_FAILED;)

  RenderPassContainerType ::FindResultType find_result =
      render_pass_container_.Find(render_pass_ID);

  if (find_result) {
    wrapper_ = &(find_result->second);
  } else {
    VkRenderPass created_render_pass;
    MM_VK_CHECK(vkCreateRenderPass(render_engine->GetDevice(),
                                   &vk_render_pass_create_info, allocator,
                                   &created_render_pass),
                MM_LOG_ERROR("Failed to create VkRenderPass.");)

    RenderPassAttachmentDescriptionWrapper wrapper{
        render_engine, allocator, created_render_pass,
        RenderPassCreateInfo(vk_render_pass_create_info)};

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

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescription(
        MM::RenderSystem::RenderPassAttachmentDescription&& other) noexcept
    : wrapper_(other.wrapper_) {
  other.wrapper_ = nullptr;
}

MM::RenderSystem::RenderPassAttachmentDescription&
MM::RenderSystem::RenderPassAttachmentDescription::operator=(
    MM::RenderSystem::RenderPassAttachmentDescription&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  wrapper_ = other.wrapper_;

  other.wrapper_ = nullptr;

  return *this;
}

const MM::RenderSystem::RenderPassCreateInfo&
MM::RenderSystem::RenderPassAttachmentDescription::GetRenderPassCreateInfo()
    const {
  assert(IsValid());
  return wrapper_->GetRenderPassAttachmentDescriptorCreateInfo();
}

const std::vector<VkAttachmentDescription>&
MM::RenderSystem::RenderPassAttachmentDescription::GetVkAttachmentDescriptions()
    const {
  assert(IsValid());
  return wrapper_->GetVkAttachmentDescriptions();
}

const std::vector<VkSubpassDescription>&
MM::RenderSystem::RenderPassAttachmentDescription::GetSubpassDescriptions()
    const {
  assert(IsValid());
  return wrapper_->GetSubpassDescriptions();
}

const std::vector<VkSubpassDependency>&
MM::RenderSystem::RenderPassAttachmentDescription::GetVkDependencyInfos()
    const {
  assert(IsValid());
  return wrapper_->GetVkDependencyInfos();
}

VkDevice MM::RenderSystem::RenderPassAttachmentDescription::GetDevice() const {
  assert(IsValid());
  return wrapper_->GetDevice();
}

VkAllocationCallbacks*
MM::RenderSystem::RenderPassAttachmentDescription::GetAllocator() const {
  assert(IsValid());
  return wrapper_->GetAllocator();
}

VkRenderPass
MM::RenderSystem::RenderPassAttachmentDescription::GetRenderPass() {
  assert(IsValid());
  return wrapper_->GetRenderPass();
}

const VkRenderPass_T*
MM::RenderSystem::RenderPassAttachmentDescription::GetRenderPass() const {
  assert(IsValid());
  return wrapper_->GetRenderPass();
}

bool MM::RenderSystem::RenderPassAttachmentDescription::IsValid() const {
  return wrapper_ != nullptr;
}
void MM::RenderSystem::RenderPassAttachmentDescription::Reset() {
  wrapper_ = nullptr;
}

MM::ExecuteResult
MM::RenderSystem::RenderPassAttachmentDescription::GetRenderPassID(
    MM::RenderSystem::RenderPassID& render_pass_ID) const {
  assert(IsValid());
  return wrapper_->GetRenderPassAttachmentDescriptorCreateInfo()
      .GetRenderPassID(render_pass_ID);
}

MM::ExecuteResult
MM::RenderSystem::RenderPassAttachmentDescription::CheckInitParameters(
    MM::RenderSystem::RenderEngine* render_engine,
    const VkRenderPassCreateInfo& render_pass_create_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine must invalid.");
    return MM::ExecuteResult ::INITIALIZATION_FAILED;
  }

  for (std::uint32_t i = 0; i != render_pass_create_info.attachmentCount; ++i) {
    if (render_pass_create_info.pAttachments->flags == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->format == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->samples == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->loadOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->storeOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->stencilLoadOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->stencilStoreOp == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->initialLayout == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
    if (render_pass_create_info.pAttachments->finalLayout == 0x7FFFFFFF) {
      MM_LOG_ERROR("AttachmentDecriptions is error.");
      return MM::ExecuteResult ::INITIALIZATION_FAILED;
    }
  }
}

MM::ExecuteResult
MM::RenderSystem::RenderPassAttachmentDescription::InitRenderPass(
    RenderEngine* render_engine, VkAllocationCallbacks* allocator,
    const RenderPassCreateInfo& render_pass_create_info) {
  RenderPassID render_pass_ID;
  MM_CHECK(GetRenderPassID(render_pass_ID),
           MM_LOG_ERROR("MM::RenderSystem::RenderPassCreateInfo is invalid.");
           return ExecuteResult::INITIALIZATION_FAILED;)

  RenderPassContainerType ::FindResultType find_result =
      render_pass_container_.Find(render_pass_ID);

  if (find_result) {
    wrapper_ = &(find_result->second);
  } else {
    VkRenderPass created_render_pass;
    VkRenderPassCreateInfo vk_render_pass_create_info =
        render_pass_create_info.GetVkRenderPassCreateInfo();
    MM_VK_CHECK(vkCreateRenderPass(render_engine->GetDevice(),
                                   &vk_render_pass_create_info, allocator,
                                   &created_render_pass),
                MM_LOG_ERROR("Failed to create VkRenderPass.");)

    RenderPassAttachmentDescriptionWrapper wrapper{
        render_engine, allocator, created_render_pass,
        RenderPassCreateInfo(render_pass_create_info)};

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

MM::ExecuteResult
MM::RenderSystem::RenderPassAttachmentDescription::InitRenderPass(
    MM::RenderSystem::RenderEngine* render_engine,
    VkAllocationCallbacks* allocator,
    MM::RenderSystem::RenderPassCreateInfo&& render_pass_create_info) {
  RenderPassID render_pass_ID;
  MM_CHECK(GetRenderPassID(render_pass_ID),
           MM_LOG_ERROR("MM::RenderSystem::RenderPassCreateInfo is invalid.");
           return ExecuteResult::INITIALIZATION_FAILED;)

  RenderPassContainerType ::FindResultType find_result =
      render_pass_container_.Find(render_pass_ID);

  if (find_result) {
    wrapper_ = &(find_result->second);
  } else {
    VkRenderPass created_render_pass;
    VkRenderPassCreateInfo vk_render_pass_create_info =
        render_pass_create_info.GetVkRenderPassCreateInfo();
    MM_VK_CHECK(vkCreateRenderPass(render_engine->GetDevice(),
                                   &vk_render_pass_create_info, allocator,
                                   &created_render_pass),
                MM_LOG_ERROR("Failed to create VkRenderPass.");)

    RenderPassAttachmentDescriptionWrapper wrapper{
        render_engine, allocator, created_render_pass,
        RenderPassCreateInfo(std::move(render_pass_create_info))};

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

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::
        ~RenderPassAttachmentDescriptionWrapper() {
  if (IsValid()) {
    vkDestroyRenderPass(render_engine_->GetDevice(), render_pass_, allocator_);
  }
}

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::
        RenderPassAttachmentDescriptionWrapper(
            RenderEngine* render_engine, VkAllocationCallbacks* allocator,
            VkRenderPass render_pass,
            RenderPassCreateInfo&&
                render_pass_attachment_description_create_info)
    : render_engine_(render_engine),
      allocator_(allocator),
      render_pass_(render_pass),
      render_pass_attachment_description_create_info_(
          std::move(render_pass_attachment_description_create_info)) {}

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::
        RenderPassAttachmentDescriptionWrapper(
            MM::RenderSystem::RenderPassAttachmentDescription::
                RenderPassAttachmentDescriptionWrapper&& other) noexcept
    : render_engine_(other.render_engine_),
      allocator_(other.allocator_),
      render_pass_(other.render_pass_),
      render_pass_attachment_description_create_info_(
          std::move(other.render_pass_attachment_description_create_info_)) {
  other.render_pass_ = nullptr;
  other.allocator_ = nullptr;
  other.render_pass_ = nullptr;
}

MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper&
    MM::RenderSystem::RenderPassAttachmentDescription::
        RenderPassAttachmentDescriptionWrapper::operator=(
            MM::RenderSystem::RenderPassAttachmentDescription::
                RenderPassAttachmentDescriptionWrapper&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  render_engine_ = other.render_engine_;
  allocator_ = other.allocator_;
  render_pass_ = other.render_pass_;
  render_pass_attachment_description_create_info_ =
      std::move(other.render_pass_attachment_description_create_info_);

  other.render_engine_ = nullptr;
  other.allocator_ = nullptr;
  other.render_pass_ = nullptr;

  return *this;
}

VkDevice MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::GetDevice() const {
  return render_engine_->GetDevice();
}

VkAllocationCallbacks* MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::GetAllocator() const {
  return allocator_;
}

VkRenderPass MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::GetRenderPass() {
  return render_pass_;
}

const VkRenderPass_T* MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::GetRenderPass() const {
  return render_pass_;
}

bool MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::IsValid() const {
  return render_engine_ != nullptr && allocator_ != nullptr &&
         render_pass_ != nullptr;
}

void MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::Release() {
  if (IsValid()) {
    vkDestroyRenderPass(render_engine_->GetDevice(), render_pass_, allocator_);
  }

  render_engine_ = nullptr;
  allocator_ = nullptr;
  render_pass_ = nullptr;
}

const MM::RenderSystem::RenderPassCreateInfo& MM::RenderSystem::
    RenderPassAttachmentDescription::RenderPassAttachmentDescriptionWrapper::
        GetRenderPassAttachmentDescriptorCreateInfo() const {
  return render_pass_attachment_description_create_info_;
}

const std::vector<VkAttachmentDescription>&
MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::GetVkAttachmentDescriptions()
        const {
  return render_pass_attachment_description_create_info_.attachments_;
}

const std::vector<VkSubpassDescription>&
MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::GetSubpassDescriptions() const {
  return render_pass_attachment_description_create_info_.subpasses_;
}

const std::vector<VkSubpassDependency>&
MM::RenderSystem::RenderPassAttachmentDescription::
    RenderPassAttachmentDescriptionWrapper::GetVkDependencyInfos() const {
  return render_pass_attachment_description_create_info_.dependencies_;
}
