#include "runtime/function/render/vk_command.h"

#include "vk_engine.h"

const MM::RenderSystem::CommandBufferType& MM::RenderSystem::CommandTask::
GetCommandBufferType() const {
  return command_type_;
}

const std::uint32_t& MM::RenderSystem::CommandTask::
GetCommandBufferIndex() const {
  return command_buffer_index_;
}

bool MM::RenderSystem::CommandTask::IsSubmitted() const { return submitted_; }

bool MM::RenderSystem::CommandTask::Wait(const std::uint64_t& timeout) const {
  if (!IsValid()) {
    LOG_ERROR(
        "Use MM::RenderSystem::CommandTask::Wait(), but CommandTask is "
        "invalid.")
    return false;
  }

  return vkWaitForFences(render_engine_->GetDevice(), 1, &task_fence_, VK_TRUE,
                  timeout);
}

void MM::RenderSystem::CommandTask::Reset() {
  command_type_ = CommandBufferType::UNDEFINED;
  command_buffer_index_ = UINT32_MAX;
  task_fence_ = nullptr;
}

MM::RenderSystem::CommandTask::CommandTask(
    RenderEngine* engine, const CommandBufferType& command_type,
    const std::uint32_t& command_buffer_index, const VkFence& task_fence)
    : render_engine_(engine),
      command_type_(command_type),
      command_buffer_index_(command_buffer_index),
      task_fence_(task_fence) {
#ifdef CHECK_PARAMETERS
  if (render_engine_ == nullptr || command_type_ == CommandBufferType::UNDEFINED
      || command_buffer_index_ == UINT32_MAX || task_fence_ == nullptr) {
    LOG_ERROR("CommandTask input parameters are error.")
    Reset();
    return;
  }
#endif
}

bool MM::RenderSystem::CommandTask::IsValid() const {
  if (render_engine_ == nullptr || command_type_ == CommandBufferType::UNDEFINED || command_buffer_index_ ==
          UINT32_MAX || task_fence_ == nullptr) {
    return false;
  }
  return true;
}

bool MM::RenderSystem::SubmitWaitSemaphore::IsValid() const {
  return wait_semaphore_ != nullptr;
}

void MM::RenderSystem::CommandBufferInfo::Reset() { is_recorded_ = false; }

bool MM::RenderSystem::CommandBufferInfo::IsValid() const { return true; }

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    RenderEngine* engine, const std::uint32_t& queue_index,
    const std::shared_ptr<VkQueue>& queue, const VkCommandPool& command_pool,
    const VkCommandBuffer& command_buffer)
    : command_buffer_info_{queue_index, false},
      wrapper_(std::make_shared<AllocatedCommandBufferWrapper>(
          engine, queue, command_pool, command_buffer)) {}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    AllocatedCommandBuffer&& other) noexcept
    : wrapper_(std::move(other.wrapper_)) {}

MM::RenderSystem::AllocatedCommandBuffer&
MM::RenderSystem::AllocatedCommandBuffer::operator=(
    const AllocatedCommandBuffer& other) {
  if (&other == this) {
    return *this;
  }
  command_buffer_info_.queue_index_ = other.command_buffer_info_.queue_index_;
  const bool is_record = other.command_buffer_info_.is_recorded_;
  command_buffer_info_.is_recorded_ = is_record;
  wrapper_ = other.wrapper_;

  return *this;
}

MM::RenderSystem::AllocatedCommandBuffer&
MM::RenderSystem::AllocatedCommandBuffer::operator=(
    AllocatedCommandBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  command_buffer_info_.queue_index_ = other.command_buffer_info_.queue_index_;
  const bool is_record = other.command_buffer_info_.is_recorded_;
  command_buffer_info_.is_recorded_ = is_record;
  wrapper_ = std::move(other.wrapper_);

  other.command_buffer_info_.Reset();

  return *this;
}

const MM::RenderSystem::RenderEngine&
MM::RenderSystem::AllocatedCommandBuffer::GetRenderEngine() const {
  return wrapper_->GetRenderEngine();
}

const VkQueue& MM::RenderSystem::AllocatedCommandBuffer::GetQueue() const {
  return wrapper_->GetQueue();
}

const VkCommandPool& MM::RenderSystem::AllocatedCommandBuffer::GetCommandPool()
    const {
  return wrapper_->GetCommandPool();
}

const VkCommandBuffer&
MM::RenderSystem::AllocatedCommandBuffer::GetCommandBuffer() const {
  return wrapper_->GetCommandBuffer();
}

const VkFence& MM::RenderSystem::AllocatedCommandBuffer::GetFence() const {
  return wrapper_->GetFence();
}

bool MM::RenderSystem::AllocatedCommandBuffer::IsRecorded() const {
  return command_buffer_info_.is_recorded_;
}

bool MM::RenderSystem::AllocatedCommandBuffer::ResetCommandBuffer() {
  return wrapper_->ResetCommandBuffer();
}

bool MM::RenderSystem::AllocatedCommandBuffer::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid();
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    ~AllocatedCommandBufferWrapper() {
  if (!IsValid()) {
    return;
  }
  vkWaitForFences(engine_->GetDevice(), 1, &command_fence_, VK_TRUE, 10000);

  vkFreeCommandBuffers(engine_->device_, command_pool_, 1, &command_buffer_);
  vkDestroyFence(engine_->device_, command_fence_, nullptr);
  command_buffer_ = nullptr;
  command_fence_ = nullptr;
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    AllocatedCommandBufferWrapper(MM::RenderSystem::RenderEngine* engine,
                                  const std::shared_ptr<VkQueue>& queue,
                                  const VkCommandPool& command_pool,
                                  const VkCommandBuffer& command_buffer)
    : engine_(engine),
      queue_(queue),
      command_pool_(command_pool),
      command_buffer_(command_buffer),
      command_fence_(nullptr) {
  if (engine_ == nullptr || queue_ == nullptr || command_pool_ == nullptr ||
      command_buffer_ == nullptr) {
    engine_ = nullptr;
    queue_ = nullptr;
    command_pool_ = nullptr;
    command_buffer_ = nullptr;
    LOG_ERROR("Failed to create AllocatedCommandBuffer!")
    return;
  }

  command_fence_ = MM::RenderSystem::Utils::GetVkFence(engine_->device_);
  if (!command_fence_) {
    engine_ = nullptr;
    queue_ = nullptr;
    command_pool_ = nullptr;
    command_buffer_ = nullptr;
    LOG_ERROR("Failed to create AllocatedCommandBuffer!")
  }
}

const MM::RenderSystem::RenderEngine& MM::RenderSystem::
    AllocatedCommandBuffer ::AllocatedCommandBufferWrapper::GetRenderEngine()
        const {
  return *engine_;
}

const VkQueue& MM::RenderSystem::AllocatedCommandBuffer::
    AllocatedCommandBufferWrapper::GetQueue() const {
  return *queue_;
}

const VkCommandPool& MM::RenderSystem::AllocatedCommandBuffer::
    AllocatedCommandBufferWrapper::GetCommandPool() const {
  return command_pool_;
}

const VkCommandBuffer& MM::RenderSystem::AllocatedCommandBuffer::
    AllocatedCommandBufferWrapper::GetCommandBuffer() const {
  return command_buffer_;
}

const VkFence& MM::RenderSystem::AllocatedCommandBuffer::
    AllocatedCommandBufferWrapper::GetFence() const {
  return command_fence_;
}

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    IsRecorded() const {
  return command_buffer_info_.is_recorded_;
}

// bool
// MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
//     RecordAndSubmitCommand(
//     AllocatedCommandBuffer& allocate_buffer,
//     const std::function<void(AllocatedCommandBuffer& cmd)>&
//     function,
//     const bool& auto_start_end_submit,
//     const bool& record_new_command, std::shared_ptr<VkSubmitInfo>
//     submit_info_ptr) {
//   if (submit_info_ptr == nullptr) {
//     submit_info_ptr = std::make_shared<VkSubmitInfo>(
//         VkSubmitInfo{Utils::GetCommandSubmitInfo(command_buffer_)});
//   }
//   VK_CHECK(
//       vkWaitForFences(engine_->device_, 1, &command_fence_, true,
//       99999999999), LOG_FATAL(
//         "The wait time for VkFence timed out.An error is expected in the
//         program, and the render system will be restarted."
//       )
//       )
//
//   if (!record_new_command) {
//     VK_CHECK(
//         vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
//         LOG_ERROR("Faild to record and submit command buffer!(can't submit "
//           "command buffer)");
//         return false;)
//     return true;
//   }
//
//   vkResetFences(engine_->device_, 1, &command_fence_);
//   std::lock_guard<std::mutex> guard{record_mutex_};
//   if (!ResetCommandBuffer()) {
//     LOG_ERROR("Faild to record and submit command buffer!");
//     return false;
//   }
//
//   if (auto_start_end_submit) {
//     const VkCommandBufferBeginInfo command_buffer_begin_info =
//         MM::RenderSystem::Utils::GetCommandBufferBeginInfo();
//     VK_CHECK(vkBeginCommandBuffer(command_buffer_,
//     &command_buffer_begin_info),
//              LOG_ERROR(
//                "Faild to record and submit command buffer!(can't begin
//                command buffer)"
//              );
//              return false;)
//   }
//
//   function(allocate_buffer);
//
//   if (auto_start_end_submit) {
//     VK_CHECK(vkEndCommandBuffer(command_buffer_),
//              LOG_ERROR("Faild to record and submit command buffer!(can't end
//              "
//                "command buffer)");
//              return false;)
//
//     VK_CHECK(vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
//              LOG_ERROR(
//                "Faild to record and submit command buffer!(can't submit "
//                "command buffer)");
//              return false;)
//   }
//   return true;
// }

// bool
// MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
// RecordAndSubmitCommand(
//     AllocatedCommandBuffer& allocated_buffer,
//     const std::function<bool(AllocatedCommandBuffer& cmd)>& function, const
//     bool& auto_start_end_submit, const bool& record_new_command,
//     std::shared_ptr<VkSubmitInfo> submit_info_ptr) {
//   if (submit_info_ptr == nullptr) {
//     submit_info_ptr = std::make_shared<VkSubmitInfo>(
//         VkSubmitInfo{Utils::GetCommandSubmitInfo(command_buffer_)});
//   }
//   VK_CHECK(
//       vkWaitForFences(engine_->device_, 1, &command_fence_, true,
//       99999999999), LOG_FATAL("The wait time for VkFence timed out.An error
//       is expected in "
//         "the program, and the render system will be restarted."))
//
//   if (!record_new_command) {
//     VK_CHECK(
//         vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
//         LOG_ERROR("Faild to record and submit command buffer!(can't submit "
//           "command buffer)");
//         return false;)
//     return true;
//   }
//
//   vkResetFences(engine_->device_, 1, &command_fence_);
//   std::lock_guard<std::mutex> guard{record_mutex_};
//   if (!ResetCommandBuffer()) {
//     LOG_ERROR("Faild to record and submit command buffer!");
//     return false;
//   }
//
//   if (auto_start_end_submit) {
//     const VkCommandBufferBeginInfo command_buffer_begin_info =
//         MM::RenderSystem::Utils::GetCommandBufferBeginInfo();
//     VK_CHECK(vkBeginCommandBuffer(command_buffer_,
//     &command_buffer_begin_info),
//              LOG_ERROR("Faild to record and submit command buffer!(can't
//              begin "
//                "command buffer)");
//              return false;)
//   }
//
//   if (!function(allocated_buffer)) {
//     LOG_ERROR("Failed to excute function that pass to
//     RecordAndSubmitCommand") return false;
//   }
//
//   if (auto_start_end_submit) {
//     VK_CHECK(vkEndCommandBuffer(command_buffer_),
//              LOG_ERROR("Faild to record and submit command buffer!(can't end
//              "
//                "command buffer)");
//              return false;)
//
//     VK_CHECK(
//         vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
//         LOG_ERROR("Faild to record and submit command buffer!(can't submit "
//           "command buffer)");
//         return false;)
//   }
//   return true;
// }

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    IsValid() const {
  return engine_ != nullptr && queue_ != nullptr && command_pool_ != nullptr &&
         command_buffer_ != nullptr && command_fence_ != nullptr;
}

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    ResetCommandBuffer() {
  VK_CHECK(vkResetCommandPool(engine_->device_, command_pool_, 0),
           LOG_ERROR("Faild to reset command buffer!");
           return false;)

  return true;
}

MM::RenderSystem::CommandExecutor::CommandExecutor(RenderEngine* engine)
    : render_engine(engine) {
  if (engine == nullptr || !engine->IsValid()) {
    LOG_ERROR("Engine is invalid.")
    render_engine = nullptr;
    return;
  }

  const std::uint32_t graph_command_number = 15;
  const std::uint32_t compute_command_number = 5;
  const std::uint32_t transform_command_number = 5;

  VkCommandPoolCreateInfo command_buffer_create_info =
      Utils::GetCommandPoolCreateInfo(
          engine->GetGraphQueueIndex(),
          VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  VkCommandPool graph_command_pool{nullptr};
  VkCommandPool compute_command_pool{nullptr};
  VkCommandPool transform_command_pool{nullptr};

  VK_CHECK(vkCreateCommandPool(engine->GetDevice(), &command_buffer_create_info,
                               nullptr, &graph_command_pool),
           LOG_ERROR("Failed to create graph command pool.");
           return;)

  command_buffer_create_info.queueFamilyIndex = engine->GetComputeQueueIndex();
  VK_CHECK(
      vkCreateCommandPool(engine->GetDevice(), &command_buffer_create_info,
                          nullptr, &compute_command_pool),
      LOG_ERROR("Failed to create compute command pool.");
      vkDestroyCommandPool(engine->GetDevice(), graph_command_pool, nullptr);
      return;)

  command_buffer_create_info.queueFamilyIndex =
      engine->GetTransformQueueIndex();
  VK_CHECK(
      vkCreateCommandPool(engine->GetDevice(), &command_buffer_create_info,
                          nullptr, &transform_command_pool),
      LOG_ERROR("Failed to create transform command pool.");
      vkDestroyCommandPool(engine->GetDevice(), graph_command_pool, nullptr);
      vkDestroyCommandPool(engine->GetDevice(), compute_command_pool, nullptr);
      return;)

  VkCommandBufferAllocateInfo command_buffer_allocate_info =
      Utils::GetCommandBufferAllocateInfo(graph_command_pool,
                                          graph_command_number);

  graph_command_buffers_.reserve(graph_command_number);

  VK_CHECK(vkAllocateCommandBuffers(render_engine->GetDevice(),
                                    &command_buffer_allocate_info,
                                    graph_command_buffers_.data()));
}
