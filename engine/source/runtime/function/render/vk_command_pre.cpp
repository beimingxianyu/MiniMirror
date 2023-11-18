//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/vk_command_pre.h"

#include "runtime/function/render/vk_engine.h"

bool MM::RenderSystem::WaitSemaphore::IsValid() const {
  return wait_semaphore_ != nullptr;
}

bool MM::RenderSystem::WaitAllocatedSemaphore::
    WaitSemaphoreLessWithoutWaitStage::operator()(
        const WaitAllocatedSemaphore& l_value,
        const WaitAllocatedSemaphore& r_value) const {
  return l_value.wait_semaphore_.GetSemaphore() <
         r_value.wait_semaphore_.GetSemaphore();
}

bool MM::RenderSystem::WaitAllocatedSemaphore::IsValid() const {
  return wait_semaphore_.GetSemaphore() != nullptr;
}

MM::RenderSystem::CommandBufferInfo::CommandBufferInfo(
    std::uint32_t queue_index, CommandBufferType command_buffer_type)
    : queue_index_(queue_index), command_buffer_type(command_buffer_type) {}

MM::RenderSystem::CommandBufferInfo::CommandBufferInfo(
    CommandBufferInfo&& other) noexcept
    : queue_index_(other.queue_index_),
      command_buffer_type(other.command_buffer_type) {
  command_buffer_type = CommandBufferType::UNDEFINED;
}

MM::RenderSystem::CommandBufferInfo&
MM::RenderSystem::CommandBufferInfo::operator=(const CommandBufferInfo& other) {
  if (&other == this) {
    return *this;
  }

  queue_index_ = other.queue_index_;
  command_buffer_type = other.command_buffer_type;

  return *this;
}

MM::RenderSystem::CommandBufferInfo&
MM::RenderSystem::CommandBufferInfo::operator=(
    CommandBufferInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  queue_index_ = other.queue_index_;
  command_buffer_type = other.command_buffer_type;

  command_buffer_type = CommandBufferType::UNDEFINED;

  return *this;
}

void MM::RenderSystem::CommandBufferInfo::Reset() {
  command_buffer_type = CommandBufferType::UNDEFINED;
}

bool MM::RenderSystem::CommandBufferInfo::IsValid() const {
  return command_buffer_type != CommandBufferType::UNDEFINED;
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    RenderEngine* engine, const std::uint32_t& queue_index,
    const VkQueue& queue, const VkCommandPool& command_pool,
    const VkCommandBuffer& command_buffer)
    : command_buffer_info_{queue_index, CommandBufferType::UNDEFINED},
      wrapper_(std::make_unique<AllocatedCommandBufferWrapper>(
          engine, queue, command_pool, command_buffer)) {
  if (engine == nullptr) {
    return;
  }
  if (queue_index == engine->GetGraphQueueIndex()) {
    command_buffer_info_.command_buffer_type = CommandBufferType::GRAPH;
  } else if (queue_index == engine->GetComputeQueueIndex()) {
    command_buffer_info_.command_buffer_type = CommandBufferType::COMPUTE;
  } else if (queue_index == engine->GetTransformQueueIndex()) {
    command_buffer_info_.command_buffer_type = CommandBufferType::TRANSFORM;
  }

  assert(command_buffer_info_.command_buffer_type !=
         CommandBufferType::UNDEFINED);
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    AllocatedCommandBuffer&& other) noexcept
    : wrapper_(std::move(other.wrapper_)) {}

MM::RenderSystem::AllocatedCommandBuffer&
MM::RenderSystem::AllocatedCommandBuffer::operator=(
    AllocatedCommandBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  command_buffer_info_ = other.command_buffer_info_;
  wrapper_ = std::move(other.wrapper_);

  other.command_buffer_info_.Reset();

  other.command_buffer_info_.Reset();

  return *this;
}

const MM::RenderSystem::RenderEngine&
MM::RenderSystem::AllocatedCommandBuffer::GetRenderEngine() const {
  return wrapper_->GetRenderEngine();
}

const VkQueue_T* MM::RenderSystem::AllocatedCommandBuffer::GetQueue() const {
  return wrapper_->GetQueue();
}

std::uint32_t MM::RenderSystem::AllocatedCommandBuffer::GetQueueIndex() const {
  return command_buffer_info_.queue_index_;
}

MM::RenderSystem::CommandBufferType
MM::RenderSystem::AllocatedCommandBuffer::GetCommandBufferType() const {
  return command_buffer_info_.command_buffer_type;
}

const VkCommandPool_T*
MM::RenderSystem::AllocatedCommandBuffer::GetCommandPool() const {
  return wrapper_->GetCommandPool();
}

const VkCommandBuffer_T*
MM::RenderSystem::AllocatedCommandBuffer::GetCommandBuffer() const {
  return wrapper_->GetCommandBuffer();
}

const VkFence_T* MM::RenderSystem::AllocatedCommandBuffer::GetFence() const {
  return wrapper_->GetFence();
}

bool MM::RenderSystem::AllocatedCommandBuffer::ResetCommandBuffer() {
  return wrapper_->ResetCommandBuffer();
}

bool MM::RenderSystem::AllocatedCommandBuffer::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid();
}

VkQueue MM::RenderSystem::AllocatedCommandBuffer::GetQueue() {
  return wrapper_->GetQueue();
}

VkCommandPool MM::RenderSystem::AllocatedCommandBuffer::GetCommandPool() {
  return wrapper_->GetCommandPool();
}

VkCommandBuffer MM::RenderSystem::AllocatedCommandBuffer::GetCommandBuffer() {
  return wrapper_->GetCommandBuffer();
}

VkFence MM::RenderSystem::AllocatedCommandBuffer::GetFence() {
  return wrapper_->GetFence();
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    ~AllocatedCommandBufferWrapper() {
  if (!IsValid()) {
    return;
  }
  vkWaitForFences(engine_->GetDevice(), 1, &command_fence_, VK_TRUE, 100000000);

  vkDestroyCommandPool(engine_->GetDevice(), command_pool_, nullptr);
  vkDestroyFence(engine_->GetDevice(), command_fence_, nullptr);
  command_pool_ = nullptr;
  command_buffer_ = nullptr;
  command_fence_ = nullptr;
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    AllocatedCommandBufferWrapper(RenderEngine* engine,
                                  const VkQueue& queue,
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
    MM_LOG_ERROR("Failed to create AllocatedCommandBuffer!");
    return;
  }

  command_fence_ = GetVkFence(engine_->GetDevice());
  if (!command_fence_) {
    engine_ = nullptr;
    queue_ = nullptr;
    command_pool_ = nullptr;
    command_buffer_ = nullptr;
    MM_LOG_ERROR("Failed to create AllocatedCommandBuffer!");
  }
}

const MM::RenderSystem::RenderEngine& MM::RenderSystem::
    AllocatedCommandBuffer ::AllocatedCommandBufferWrapper::GetRenderEngine()
        const {
  return *engine_;
}

const VkQueue& MM::RenderSystem::AllocatedCommandBuffer::
    AllocatedCommandBufferWrapper::GetQueue() const {
  return queue_;
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
    IsValid() const {
  return engine_ != nullptr && queue_ != nullptr && command_pool_ != nullptr &&
         command_buffer_ != nullptr && command_fence_ != nullptr;
}

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    ResetCommandBuffer() {
  MM_VK_CHECK(vkResetCommandPool(engine_->GetDevice(), command_pool_, 0),
              MM_LOG_ERROR("Failed to reset command buffer!");
              return false;)

  return true;
}

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    ResetFence() {
  MM_VK_CHECK(vkResetFences(engine_->GetDevice(), 1, &command_fence_),
              MM_LOG_ERROR("Failed to reset fence.");
              return false;)

  return true;
}
