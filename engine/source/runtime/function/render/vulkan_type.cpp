#include "runtime/function/render/vulkan_type.h"

#include "vk_engine.h"

bool MM::RenderSystem::QueueFamilyIndices::isComplete() const {
  return graphics_family_.has_value() && present_family_.has_value() &&
         compute_family_.has_value();
}


MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    RenderEngine* engine, VkQueue queue, const VkCommandPool& command_pool,
    const VkCommandBuffer& command_buffer)
  : wrapper_(
      std::make_shared<AllocatedCommandBufferWrapper>(
          engine, queue, command_pool, command_buffer)) {
}

MM::RenderSystem::AllocatedCommandBuffer& MM::RenderSystem::
AllocatedCommandBuffer::operator=(const AllocatedCommandBuffer& other) {
  if (&other == this) {
    return *this;
  }
  wrapper_ = other.wrapper_;

  return *this;
}

MM::RenderSystem::AllocatedCommandBuffer& MM::RenderSystem::
AllocatedCommandBuffer::operator=(AllocatedCommandBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  wrapper_ = other.wrapper_;

  other.wrapper_.reset();

  return *this;
}

const MM::RenderSystem::RenderEngine& MM::RenderSystem::AllocatedCommandBuffer::
GetRenderEngine() const {
  return wrapper_->GetRenderEngine();
}

const VkQueue& MM::RenderSystem::AllocatedCommandBuffer::GetQueue() const {
  return wrapper_->GetQueue();
}

const VkCommandPool& MM::RenderSystem::AllocatedCommandBuffer::
GetCommandPool() const {
  return wrapper_->GetCommandPool();
}

const VkCommandBuffer& MM::RenderSystem::AllocatedCommandBuffer::
GetCommandBuffer() const {
  return wrapper_->GetCommandBuffer();
}

const VkFence& MM::RenderSystem::AllocatedCommandBuffer::GetFence() const {
  return wrapper_->GetFence();
}

bool MM::RenderSystem::AllocatedCommandBuffer::RecordAndSubmitCommand(
    const std::function<void(VkCommandBuffer& cmd)>& function,
    const bool& auto_start_end_submit, const bool& record_new_command,
    const std::shared_ptr<VkSubmitInfo>& submit_info_ptr) {
  return wrapper_->RecordAndSubmitCommand(function, auto_start_end_submit, record_new_command, submit_info_ptr);
}

bool MM::RenderSystem::AllocatedCommandBuffer::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid();
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::~
AllocatedCommandBufferWrapper() {
  if (!IsValid()) {
    return;
  }
  std::lock_guard<std::mutex> guard(record_mutex_);
  vkFreeCommandBuffers(engine_->device_, *command_pool_, 1, &command_buffer_);
  vkDestroyFence(engine_->device_, command_fence_, nullptr);
  command_buffer_ = nullptr;
  command_fence_ = nullptr;
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    AllocatedCommandBufferWrapper(RenderEngine* engine, const VkQueue& queue,
                                  const VkCommandPool& command_pool,
                                  const VkCommandBuffer& command_buffer)
    : engine_(engine),
      queue_(queue),
      command_pool_(
          MM::Utils::MakeSharedWithDestructor<VkCommandPool>([device = engine_->device_](
                                                                 VkCommandPool *
                                                                 value) {
            if (value == nullptr) {
              return;
            }
            vkDestroyCommandPool(device, *value, nullptr);

            delete value;
            *value = nullptr;
          }, command_pool)),
  command_buffer_(command_buffer),
  command_fence_(nullptr),
  record_mutex_() {
  if (engine_ == nullptr || queue_ == nullptr|| command_pool_ == nullptr ||
      command_buffer_ == nullptr) {
    engine_ = nullptr;
    queue_ = nullptr;
    command_pool_ = nullptr;
    command_buffer_ = nullptr;
    LOG_ERROR("Failed to create AllocatedCommandBuffer!")
    return;
  }

  command_fence_ =
      MM::RenderSystem::Utils::GetVkFence(engine_->device_);
  if (!command_fence_) {
    engine_ = nullptr;
    queue_ = nullptr;
    command_pool_ = nullptr;
    command_buffer_ = nullptr;
    LOG_ERROR("Failed to create AllocatedCommandBuffer!")
  }
}

const MM::RenderSystem::RenderEngine& MM::RenderSystem::AllocatedCommandBuffer
::AllocatedCommandBufferWrapper::GetRenderEngine() const {
  return *engine_;
}

const VkQueue& MM::RenderSystem::AllocatedCommandBuffer::
AllocatedCommandBufferWrapper::GetQueue() const {
  return queue_;
}

const VkCommandPool& MM::RenderSystem::AllocatedCommandBuffer::
AllocatedCommandBufferWrapper::GetCommandPool() const {
  return *command_pool_;
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
    RecordAndSubmitCommand(const std::function<void(VkCommandBuffer& cmd)>&
                               function,
                           const bool& auto_start_end_submit, 
                           const bool& record_new_command,
                           std::shared_ptr<VkSubmitInfo> submit_info_ptr) {
  if (submit_info_ptr == nullptr) {
    submit_info_ptr = std::make_shared<VkSubmitInfo>(
        VkSubmitInfo{Utils::GetCommandSubmitInfo(command_buffer_)});
  }
  VK_CHECK(
      vkWaitForFences(engine_->device_, 1, &command_fence_, true, 99999999999),
      LOG_FATAL("The wait time for VkFence timed out.An error is expected in the program, and the render system will be restarted.")
  )

  if (!record_new_command) {
    VK_CHECK(
        vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
        LOG_ERROR("Faild to record and submit command buffer!(can't submit "
                  "command buffer)");
        return false;)
    return true;
  }

  vkResetFences(engine_->device_, 1, &command_fence_);
  std::lock_guard<std::mutex> guard{record_mutex_};
  VK_CHECK(!ResetCommandBuffer(), 
    LOG_ERROR("Faild to record and submit command buffer!");
    return false;)
  if (auto_start_end_submit) {
    const VkCommandBufferBeginInfo command_buffer_begin_info =
        MM::RenderSystem::Utils::GetCommandBufferBeginInfo();
    VK_CHECK(vkBeginCommandBuffer(command_buffer_, &command_buffer_begin_info),
             LOG_ERROR("Faild to record and submit command buffer!(can't begin command buffer)");
             return false;)
  }
  
  function(command_buffer_);

  if (auto_start_end_submit) {
    VK_CHECK(vkEndCommandBuffer(command_buffer_),
             LOG_ERROR("Faild to record and submit command buffer!(can't end "
                       "command buffer)");
             return false;)

    VK_CHECK(vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
             LOG_ERROR("Faild to record and submit command buffer!(can't submit "
                       "command buffer)");
        return false;)
  }
  return true;
}

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
IsValid() const {
  return engine_ != nullptr && queue_ != nullptr && command_pool_ != nullptr &&
         command_buffer_ != nullptr && command_fence_ != nullptr;
}

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    ResetCommandBuffer() {
  VK_CHECK(vkResetCommandBuffer(command_buffer_, 0),
           LOG_ERROR("Faild to reset command buffer!");
           return false;
  )
  return true;
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    const VmaAllocator& allocator, const VkBuffer& buffer,
    const VmaAllocation& allocation) : wrapper_(std::make_shared<AllocatedBufferWrapper>(allocator, buffer, allocation)){}

MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::AllocatedBuffer::operator=(
    const AllocatedBuffer& other) {
  if (&other == this) {
    return *this;
  }
  wrapper_ = other.wrapper_;

  return *this;
}

MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::AllocatedBuffer::operator=(
    AllocatedBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  wrapper_ = other.wrapper_;

  other.wrapper_.reset();

  return *this;
}

const VmaAllocator& MM::RenderSystem::AllocatedBuffer::GetAllocator() const {
  return wrapper_->GetAllocator();
}

const VkBuffer& MM::RenderSystem::AllocatedBuffer::GetBuffer() const {
  return wrapper_->GetBuffer();
}

const VmaAllocation& MM::RenderSystem::AllocatedBuffer::GetAllocation() const {
  return wrapper_->GetAllocation();
}

bool MM::RenderSystem::AllocatedBuffer::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid();
}

MM::RenderSystem::AllocatedBuffer::AllocatedBufferWrapper::~
AllocatedBufferWrapper() {
  if (allocator_ == nullptr) {
    return;
  }
  vmaDestroyBuffer(allocator_, buffer_, allocation_);
}

MM::RenderSystem::AllocatedBuffer::AllocatedBufferWrapper::
AllocatedBufferWrapper(const VmaAllocator& allocator, const VkBuffer& buffer,
    const VmaAllocation& allocation) : allocator_(allocator), buffer_(buffer), allocation_(allocation) {
  if (allocator_ == nullptr || buffer_ == nullptr || allocation_ == nullptr) {
    allocator_ = nullptr;
    buffer_ = nullptr;
    allocation_ = nullptr;
  } }

const VmaAllocator& MM::RenderSystem::AllocatedBuffer::AllocatedBufferWrapper::
GetAllocator() const {
  return allocator_;
}

const VkBuffer& MM::RenderSystem::AllocatedBuffer::AllocatedBufferWrapper::
GetBuffer() const {
  return buffer_;
}

const VmaAllocation& MM::RenderSystem::AllocatedBuffer::AllocatedBufferWrapper::
GetAllocation() const {
  return allocation_;
}

bool MM::RenderSystem::AllocatedBuffer::AllocatedBufferWrapper::
IsValid() const {
  return allocator_ != nullptr && buffer_ != nullptr && allocation_ != nullptr;
}

MM::RenderSystem::AllocatedImage::AllocatedImage(
    const VmaAllocator& allocator, const VkImage& image,
    const VmaAllocation& allocation)
    : wrapper_(std::make_shared<AllocatedImageWrapper>(allocator, image, allocation)) {}

MM::RenderSystem::AllocatedImage& MM::RenderSystem::AllocatedImage::operator=(
    const AllocatedImage& other) {
  if (&other == this) {
    return *this;
  }
  wrapper_ = other.wrapper_;
  return *this;
}

MM::RenderSystem::AllocatedImage& MM::RenderSystem::AllocatedImage::operator=(
    AllocatedImage&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  wrapper_ = other.wrapper_;

  other.wrapper_.reset();

  return *this;
}

const VmaAllocator& MM::RenderSystem::AllocatedImage::GetAllocator() const {
  return wrapper_->GetAllocator();
}

const VkImage& MM::RenderSystem::AllocatedImage::GetImage() const {
  return wrapper_->GetImage();
}

const VmaAllocation& MM::RenderSystem::AllocatedImage::GetAllocation() const {
  return wrapper_->GetAllocation();
}

bool MM::RenderSystem::AllocatedImage::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid();
}

MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::~
AllocatedImageWrapper() {
  if (allocator_ == nullptr) {
    return;
  }
  vmaDestroyImage(allocator_, image_, allocation_);
}

MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::AllocatedImageWrapper(
    const VmaAllocator& allocator, const VkImage& image,
    const VmaAllocation& allocation)
  : allocator_(allocator),
    image_(image),
    allocation_(allocation) {
  if (allocator_ == nullptr || image_ == nullptr || allocation_ == nullptr) {
    allocator_ = nullptr;
    image_ = nullptr;
    allocation_ = nullptr;
  } }

const VmaAllocator& MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::
GetAllocator() const {
  return allocator_;
}

const VkImage& MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::
GetImage() const {
  return image_;
}

const VmaAllocation& MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::
GetAllocation() const {
  return allocation_;
}

bool MM::RenderSystem::AllocatedImage::AllocatedImageWrapper::IsValid() const {
  return allocator_ != nullptr && image_ != nullptr && allocation_ != nullptr;
}
