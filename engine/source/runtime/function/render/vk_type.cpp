#include <utility>

#include "runtime/function/render/vk_type.h"

#include "runtime/function/render/vk_engine.h"

bool MM::RenderSystem::QueueFamilyIndices::isComplete() const {
  return graphics_family_.has_value() && present_family_.has_value() &&
         compute_family_.has_value() && transform_family_.has_value();
}

MM::RenderSystem::ManagedObjectBase::ManagedObjectBase(
    const std::string& object_name, const std::uint32_t& object_ID)
  : object_name_(object_name),
    object_ID_(object_ID) {
}

MM::RenderSystem::ManagedObjectBase::ManagedObjectBase(
    ManagedObjectBase&& other) noexcept
  : object_name_(std::move(other.object_name_)),
    object_ID_(other.object_ID_) {
  other.object_ID_ = 0;
}

MM::RenderSystem::ManagedObjectBase& MM::RenderSystem::ManagedObjectBase::
operator=(const ManagedObjectBase& other) {
  if (&other == this) {
    return *this;
  }
  object_name_ = other.object_name_;
  object_ID_ = other.object_ID_;

  return *this;
}

MM::RenderSystem::ManagedObjectBase& MM::RenderSystem::ManagedObjectBase::
operator=(ManagedObjectBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  object_name_ = std::move(other.object_name_);
  object_ID_ = other.object_ID_;

  other.object_ID_ = 0;

  return *this;
}

const std::string& MM::RenderSystem::ManagedObjectBase::GetObjectName() const {
  return object_name_;
}

const std::uint32_t& MM::RenderSystem::ManagedObjectBase::GetObjectID() const {
  return object_ID_;
}

void MM::RenderSystem::ManagedObjectBase::Release() {
  object_name_ = std::string{};
  object_ID_ = 0;
}

void MM::RenderSystem::ManagedObjectBase::SetObjectName(
    const std::string& new_object_name) {
  object_name_ = new_object_name;
}

void MM::RenderSystem::ManagedObjectBase::SetObjectID(
    const std::uint32_t& new_object_ID) {
  object_ID_ = new_object_ID;
}


MM::RenderSystem::ImageBindInfo::ImageBindInfo(ImageBindInfo&& other) noexcept
  : bind_(other.bind_),
    image_view_(std::move(other.image_view_)),
    sampler_(std::move(other.sampler_)),
    semaphore_(std::move(other.semaphore_)) {
  other.Reset();
}

MM::RenderSystem::ImageBindInfo& MM::RenderSystem::ImageBindInfo::operator=(
    const ImageBindInfo& other) {
  if (&other == this) {
    return *this;
  }
  bind_ = other.bind_;
  image_view_ = other.image_view_;
  sampler_ = other.sampler_;
  semaphore_ = other.semaphore_;

  return *this;
}

MM::RenderSystem::ImageBindInfo& MM::RenderSystem::ImageBindInfo::operator=(
    ImageBindInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  bind_ = other.bind_;
  image_view_ = std::move(other.image_view_);
  sampler_ = std::move(other.sampler_);
  semaphore_ = std::move(other.semaphore_);

  other.Reset();

  return *this;
}

bool MM::RenderSystem::ImageBindInfo::IsValid() const {
  if (bind_.descriptorCount == 0 || image_view_ == nullptr ||
      sampler_ == nullptr || semaphore_ == nullptr) {
    return false;
  }
  return true;
}

void MM::RenderSystem::ImageBindInfo::Reset() {
  bind_.descriptorCount = 0;
  sampler_.reset();
  image_view_.reset();
  semaphore_.reset();
}

void MM::RenderSystem::ImageInfo::Reset() {
  image_extent_ = {0, 0, 0};
  image_size_ = 0;
  image_format_ = VK_FORMAT_UNDEFINED;
  image_layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  mipmap_levels_ = 1;
  array_layers_ = 1;
  can_mapped_ = false;
  is_exclusive_ = false;
  queue_index_ = 0;
}

bool MM::RenderSystem::ImageInfo::IsValid() const {
  if (image_size_ == 0 || image_format_ == VK_FORMAT_UNDEFINED ||
      image_layout_ == VK_IMAGE_LAYOUT_UNDEFINED || mipmap_levels_ == 0 ||
      image_extent_.width == 0 || image_extent_.height == 0 ||
      image_extent_.depth == 0 || array_layers_ == 0) {
    return false;
  }
  return true;
}

MM::RenderSystem::BufferBindInfo::BufferBindInfo(
    BufferBindInfo&& other) noexcept
  : bind_(other.bind_),
    range_size_(other.range_size_),
    offset_(other.offset_),
    dynamic_offset_(other.dynamic_offset_),
    semaphore_(std::move(other.semaphore_)) {
  other.Reset();
}

MM::RenderSystem::BufferBindInfo& MM::RenderSystem::BufferBindInfo::operator=(
    const BufferBindInfo& other) {
  if (&other == this) {
    return *this;
  }
  bind_ = other.bind_;
  range_size_ = other.range_size_;
  offset_ = other.offset_;
  dynamic_offset_ = other.dynamic_offset_;
  semaphore_ = other.semaphore_;

  return *this;
}

MM::RenderSystem::BufferBindInfo& MM::RenderSystem::BufferBindInfo::operator=(
    BufferBindInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  bind_ = other.bind_;
  range_size_ = other.range_size_;
  offset_ = other.offset_;
  dynamic_offset_ = other.dynamic_offset_;
  semaphore_ = std::move(other.semaphore_);

  other.Reset();

  return *this;
}

void MM::RenderSystem::BufferBindInfo::Reset() {
  range_size_ = 0;
  offset_ = 0;
  dynamic_offset_ = 0;
  semaphore_.reset();
}

bool MM::RenderSystem::BufferBindInfo::IsValid() const {
  return range_size_ != 0 && dynamic_offset_ < range_size_ &&
         bind_.descriptorCount != 0 && semaphore_ != nullptr;
}

void MM::RenderSystem::BufferInfo::Reset() {
  buffer_size_ = 0;
  can_mapped_ = false;
  is_transform_src_ = false;
  is_transform_dest_ = false;
  is_exclusive_ = false;
  queue_index_ = 0;
}

bool MM::RenderSystem::BufferInfo::IsValid() const { return buffer_size_ != 0; }

MM::RenderSystem::VertexInputState::VertexInputState()
  : vertex_bind_(),
    vertex_buffer_offset_(0),
    vertex_attributes_(),
    instance_binds_(),
    instance_buffer_offset_(),
    instance_attributes_() {
}

MM::RenderSystem::VertexInputState::VertexInputState(
    const VkDeviceSize& vertex_buffer_offset)
  : vertex_bind_{0, sizeof(AssetType::Vertex), VK_VERTEX_INPUT_RATE_VERTEX},
    vertex_buffer_offset_(vertex_buffer_offset),
    vertex_attributes_(),
    instance_binds_(),
    instance_buffer_offset_(),
    instance_attributes_() {
  InitDefaultVertexInput();
}

MM::RenderSystem::VertexInputState::VertexInputState(
    const VkDeviceSize& vertex_buffer_offset,
    const std::vector<VkVertexInputBindingDescription>& instance_binds,
    const std::vector<VkDeviceSize>& instance_buffer_offset,
    const std::vector<VkVertexInputAttributeDescription>& instance_attributes)
  : vertex_bind_{0, sizeof(MM::AssetType::Vertex),
                 VK_VERTEX_INPUT_RATE_VERTEX},
    vertex_buffer_offset_(vertex_buffer_offset),
    vertex_attributes_{5},
    instance_binds_(instance_binds),
    instance_buffer_offset_(instance_buffer_offset),
    instance_attributes_(instance_attributes) {
  std::string error_message;
  if (!CheckLayoutIsCorrect(error_message)) {
    LOG_ERROR(error_message)
    Reset();
    return;
  }
  InitDefaultVertexInput();
}

MM::RenderSystem::VertexInputState::VertexInputState(
    VertexInputState&& other) noexcept
  : vertex_bind_(other.vertex_bind_),
    vertex_buffer_offset_(other.vertex_buffer_offset_),
    vertex_attributes_(std::move(other.vertex_attributes_)),
    instance_binds_(std::move(other.instance_binds_)),
    instance_buffer_offset_(std::move(other.instance_buffer_offset_)),
    instance_attributes_(std::move(other.instance_attributes_)) {
  other.vertex_bind_.stride = 0;
  vertex_buffer_offset_ = 0;
}

MM::RenderSystem::VertexInputState& MM::RenderSystem::VertexInputState::operator
=(
    const VertexInputState& other) {
  if (&other == this) {
    return *this;
  }
  vertex_bind_ = other.vertex_bind_;
  vertex_buffer_offset_ = other.vertex_buffer_offset_;
  vertex_attributes_ = other.vertex_attributes_;
  instance_binds_ = other.instance_binds_;
  instance_buffer_offset_ = other.instance_buffer_offset_;
  instance_attributes_ = other.instance_attributes_;

  return *this;
}

MM::RenderSystem::VertexInputState& MM::RenderSystem::VertexInputState::operator
=(
    VertexInputState&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  vertex_bind_ = other.vertex_bind_;
  vertex_buffer_offset_ = other.vertex_buffer_offset_;
  vertex_attributes_ = std::move(other.vertex_attributes_);
  instance_binds_ = std::move(other.instance_binds_);
  instance_buffer_offset_ = std::move(other.instance_buffer_offset_);
  instance_attributes_ = std::move(other.instance_attributes_);

  vertex_bind_.stride = 0;
  vertex_buffer_offset_ = 0;

  return *this;
}

void MM::RenderSystem::VertexInputState::InitDefaultVertexInput() {
  vertex_attributes_[0].binding = 0;
  vertex_attributes_[0].location = 0;
  vertex_attributes_[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[0].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfPosition());

  vertex_attributes_[1].binding = 0;
  vertex_attributes_[1].location = 1;
  vertex_attributes_[1].format = VK_FORMAT_R32G32_SFLOAT;
  vertex_attributes_[1].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfTextureCoord());

  vertex_attributes_[2].binding = 0;
  vertex_attributes_[2].location = 2;
  vertex_attributes_[2].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[2].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfNormal());

  vertex_attributes_[3].binding = 0;
  vertex_attributes_[3].location = 3;
  vertex_attributes_[3].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[3].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfTangent());

  vertex_attributes_[4].binding = 0;
  vertex_attributes_[4].location = 4;
  vertex_attributes_[4].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[4].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfBiTangent());
}

MM::RenderSystem::CommandBufferInfo::CommandBufferInfo(
    RenderEngine* engine, const std::uint32_t& queue_index_) : render_engine_(engine), queue_index_(queue_index_) {
#ifdef CHECK_PARAMETERS
  if (render_engine_ == nullptr) {
    return;
  }
#endif
  VkFence temp_fence{nullptr};
  constexpr VkFenceCreateInfo fence_create_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                            nullptr, 0};

  VK_CHECK(vkCreateFence(render_engine_->GetDevice(), &fence_create_info,
                         nullptr, &temp_fence),
           LOG_ERROR("Failed to create fence.") return;)

  command_fence_.reset(&temp_fence);
}

MM::RenderSystem::CommandBufferInfo::CommandBufferInfo(
    CommandBufferInfo&& other) noexcept
  : render_engine_(other.render_engine_),
    queue_index_(other.queue_index_),
    command_fence_(std::move(other.command_fence_)) {
  other.Reset();
}

MM::RenderSystem::CommandBufferInfo& MM::RenderSystem::CommandBufferInfo::
operator=(CommandBufferInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  render_engine_ = other.render_engine_;
  queue_index_ = other.queue_index_;
  command_fence_ = std::move(other.command_fence_);

  other.Reset();

  return *this;
}

void MM::RenderSystem::CommandBufferInfo::Reset() {
  render_engine_ = nullptr;
  queue_index_ = 0;
  command_fence_.reset();
}

bool MM::RenderSystem::CommandBufferInfo::IsValid() const {
  if (render_engine_ == nullptr || command_fence_ == nullptr) {
    return false;
  }
  return true;
}

bool MM::RenderSystem::VertexInputState::IsValid() const {
  return vertex_bind_.stride != 0;
}

void MM::RenderSystem::VertexInputState::Reset() {
  vertex_bind_.stride = 0;
  vertex_buffer_offset_ = 0;
  vertex_attributes_.clear();
  instance_binds_.clear();
  instance_buffer_offset_.clear();
  instance_attributes_.clear();
}

bool MM::RenderSystem::VertexInputState::CheckLayoutIsCorrect(
    std::string& error_message) const {
  if (instance_buffer_offset_.size() != instance_binds_.size()) {
    error_message =
        "The number of offset values is not equal to the number of instance "
        "bind.";
    return false;
  }
  std::map<uint32_t, VkDeviceSize> binds_info;
  for (const auto& bind : instance_binds_) {
    if (bind.binding == 0) {
      error_message =
          "The binding of instance input description is 0.0 is the exclusive binding slot of Vertex input description.";
      return false;
    }
    if (bind.stride == 0) {
      error_message = "The size of one instance data cannot be 0.";
      return false;
    }
    if (bind.inputRate != VK_VERTEX_INPUT_RATE_INSTANCE) {
      error_message =
          "The instance input description input rate is not "
          "VK_VERTEX_INPUT_RATE_INSTANCE";
      return false;
    }
    if (binds_info.find(bind.binding) != binds_info.end()) {
      error_message =
          "Multiple instance input description are repeatedly bound to a same "
          "binding.";
      return false;
    }
    binds_info.emplace(bind.binding, bind.stride);
  }

  for (const auto& attribute : instance_attributes_) {
    if (attribute.location < 5) {
      error_message =
          "The instance attribute location less than 5.0 to 4 is the exclusive "
          "location slot of Vertex input description.";
      return false;
    }
    const auto bind_itr = binds_info.find(attribute.binding);
    if (bind_itr == binds_info.end()) {
      error_message =
          "The instance attribute description is bound to a nonexistent "
          "binding.";
      return false;
    }
    if (attribute.offset > bind_itr->second - 4) {
      error_message = "The instance attribute description offset too larger.";
      return false;
    }
  }

  return true;
}

bool MM::RenderSystem::VertexInputState::CheckLayoutIsCorrect() const {
  std::string temp;
  return CheckLayoutIsCorrect(temp);
}

const VkVertexInputBindingDescription& MM::RenderSystem::VertexInputState::
GetVertexBind() const {
  return vertex_bind_;
}

const VkDeviceSize& MM::RenderSystem::VertexInputState::
GetVertexBufferOffset() const {
  return vertex_buffer_offset_;
}

const std::vector<VkVertexInputAttributeDescription>& MM::RenderSystem::
VertexInputState::GetVertexAttributes() const {
  return vertex_attributes_;
}

const std::vector<VkVertexInputBindingDescription>& MM::RenderSystem::
VertexInputState::GetInstanceBinds() const {
  return instance_binds_;
}

const std::vector<VkDeviceSize>& MM::RenderSystem::VertexInputState::
GetInstanceBufferOffset() const {
  return instance_buffer_offset_;
}

const std::vector<VkVertexInputAttributeDescription>& MM::RenderSystem::
VertexInputState::GetInstanceAttributes() const {
  return instance_attributes_;
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    RenderEngine* engine, VkQueue queue, const VkCommandPool& command_pool,
    const VkCommandBuffer& command_buffer)
  : wrapper_(
      std::make_shared<AllocatedCommandBufferWrapper>(
          engine, queue, command_pool, command_buffer)) {
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    AllocatedCommandBuffer&& other) noexcept
  : wrapper_(std::move(other.wrapper_)) {
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
  wrapper_ = std::move(other.wrapper_);

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
    const bool& auto_start_end_submit, const bool& record_new_commands,
    const std::shared_ptr<VkSubmitInfo>& submit_info_ptr) {
  return wrapper_->RecordAndSubmitCommand(function, auto_start_end_submit,
                                          record_new_commands, submit_info_ptr);
}

bool MM::RenderSystem::AllocatedCommandBuffer::RecordAndSubmitCommand(
    const std::function<bool(VkCommandBuffer& cmd)>& function,
    const bool& auto_start_end_submit, const bool& record_new_commands,
    const std::shared_ptr<VkSubmitInfo>& submit_info_ptr) {
  return wrapper_->RecordAndSubmitCommand(function, auto_start_end_submit,
                                          record_new_commands, submit_info_ptr);
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
        MM::Utils::MakeSharedWithDestructor<VkCommandPool>(
            [device = engine_->device_](
            VkCommandPool*
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
  if (engine_ == nullptr || queue_ == nullptr || command_pool_ == nullptr ||
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
      LOG_FATAL(
        "The wait time for VkFence timed out.An error is expected in the program, and the render system will be restarted."
      )
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
  if (!ResetCommandBuffer()) {
    LOG_ERROR("Faild to record and submit command buffer!");
    return false;
  }

  if (auto_start_end_submit) {
    const VkCommandBufferBeginInfo command_buffer_begin_info =
        MM::RenderSystem::Utils::GetCommandBufferBeginInfo();
    VK_CHECK(vkBeginCommandBuffer(command_buffer_, &command_buffer_begin_info),
             LOG_ERROR(
               "Faild to record and submit command buffer!(can't begin command buffer)"
             );
             return false;)
  }

  function(command_buffer_);

  if (auto_start_end_submit) {
    VK_CHECK(vkEndCommandBuffer(command_buffer_),
             LOG_ERROR("Faild to record and submit command buffer!(can't end "
               "command buffer)");
             return false;)

    VK_CHECK(vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
             LOG_ERROR(
               "Faild to record and submit command buffer!(can't submit "
               "command buffer)");
             return false;)
  }
  return true;
}

bool MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
RecordAndSubmitCommand(
    const std::function<bool(VkCommandBuffer& cmd)>& function,
    const bool& auto_start_end_submit, const bool& record_new_command,
    std::shared_ptr<VkSubmitInfo> submit_info_ptr) {
  if (submit_info_ptr == nullptr) {
    submit_info_ptr = std::make_shared<VkSubmitInfo>(
        VkSubmitInfo{Utils::GetCommandSubmitInfo(command_buffer_)});
  }
  VK_CHECK(
      vkWaitForFences(engine_->device_, 1, &command_fence_, true, 99999999999),
      LOG_FATAL("The wait time for VkFence timed out.An error is expected in "
        "the program, and the render system will be restarted."))

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
  if (!ResetCommandBuffer()) {
    LOG_ERROR("Faild to record and submit command buffer!");
    return false;
  }

  if (auto_start_end_submit) {
    const VkCommandBufferBeginInfo command_buffer_begin_info =
        MM::RenderSystem::Utils::GetCommandBufferBeginInfo();
    VK_CHECK(vkBeginCommandBuffer(command_buffer_, &command_buffer_begin_info),
             LOG_ERROR("Faild to record and submit command buffer!(can't begin "
               "command buffer)");
             return false;)
  }

  if (!function(command_buffer_)) {
    LOG_ERROR("Failed to excute function that pass to RecordAndSubmitCommand")
    return false;
  }

  if (auto_start_end_submit) {
    VK_CHECK(vkEndCommandBuffer(command_buffer_),
             LOG_ERROR("Faild to record and submit command buffer!(can't end "
               "command buffer)");
             return false;)

    VK_CHECK(
        vkQueueSubmit(queue_, 1, submit_info_ptr.get(), command_fence_),
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
    const VmaAllocation& allocation, const BufferInfo& buffer_info)
  : buffer_info_(buffer_info),
    wrapper_(
        std::make_shared<AllocatedBufferWrapper>(
            allocator, buffer, allocation)) {
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    AllocatedBuffer&& other) noexcept
  : buffer_info_(other.buffer_info_),
    wrapper_(std::move(other.wrapper_)) {
}

MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::AllocatedBuffer::operator=(
    const AllocatedBuffer& other) {
  if (&other == this) {
    return *this;
  }

  buffer_info_ = other.buffer_info_;
  wrapper_ = other.wrapper_;

  return *this;
}

MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::AllocatedBuffer::operator=(
    AllocatedBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  buffer_info_ = other.buffer_info_;
  wrapper_ = std::move(other.wrapper_);

  return *this;
}

const VkDeviceSize& MM::RenderSystem::AllocatedBuffer::GetBufferSize() const {
  return buffer_info_.buffer_size_;
}

bool MM::RenderSystem::AllocatedBuffer::CanMapped() const {
  return buffer_info_.can_mapped_;
}

bool MM::RenderSystem::AllocatedBuffer::IsTransformSrc() const {
  return buffer_info_.is_transform_src_;
}

bool MM::RenderSystem::AllocatedBuffer::IsTransformDest() const {
  return buffer_info_.is_transform_dest_;
}

const MM::RenderSystem::BufferInfo& MM::RenderSystem::AllocatedBuffer::
GetBufferInfo() const {
  return buffer_info_;
}

VmaAllocator MM::RenderSystem::AllocatedBuffer::GetAllocator() const {
  if (wrapper_) {
    return wrapper_->GetAllocator();
  }

  return nullptr;
}

VkBuffer MM::RenderSystem::AllocatedBuffer::GetBuffer() const {
  if (wrapper_) {
    return wrapper_->GetBuffer();
  }

  return nullptr;
}

VmaAllocation MM::RenderSystem::AllocatedBuffer::GetAllocation() const {
  if (wrapper_) {
    return wrapper_->GetAllocation();
  }

  return nullptr;
}

void MM::RenderSystem::AllocatedBuffer::Release() {
  buffer_info_.Reset();
  wrapper_.reset();
}

uint32_t MM::RenderSystem::AllocatedBuffer::UseCount() const {
  if (wrapper_) {
    return wrapper_.use_count();
  }
  return 0;
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
                       const VmaAllocation& allocation)
  : allocator_(allocator),
    buffer_(buffer),
    allocation_(allocation) {
  if (allocator_ == nullptr || buffer_ == nullptr || allocation_ == nullptr) {
    allocator_ = nullptr;
    buffer_ = nullptr;
    allocation_ = nullptr;
  }
}

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

MM::RenderSystem::ImageChunkInfo::ImageChunkInfo(const VkOffset3D& offset,
                                                 const VkExtent3D& extent)
  : offset_(offset),
    extent_(extent) {
}

MM::RenderSystem::ImageChunkInfo& MM::RenderSystem::ImageChunkInfo::operator=(
    const ImageChunkInfo& other) {
  if (&other == this) {
    return *this;
  }

  offset_ = other.offset_;
  extent_ = other.extent_;

  return *this;
}

MM::RenderSystem::ImageChunkInfo& MM::RenderSystem::ImageChunkInfo::operator=(
    ImageChunkInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  offset_ = other.offset_;
  extent_ = other.extent_;

  other.Reset();

  return *this;
}

VkOffset3D& MM::RenderSystem::ImageChunkInfo::GetOffset() { return offset_; }

const VkOffset3D& MM::RenderSystem::ImageChunkInfo::GetOffset() const {
  return offset_;
}

VkExtent3D& MM::RenderSystem::ImageChunkInfo::GetExtent() { return extent_; }

const VkExtent3D& MM::RenderSystem::ImageChunkInfo::GetExtent() const {
  return extent_;
}

void MM::RenderSystem::ImageChunkInfo::SetOffset(const VkOffset3D& new_offset) {
  offset_ = new_offset;
}

void MM::RenderSystem::ImageChunkInfo::SetExtent(const VkExtent3D& new_extent) {
  extent_ = new_extent;
}

void MM::RenderSystem::ImageChunkInfo::Reset() {
  offset_ = VkOffset3D{0, 0, 0};
  extent_ = VkExtent3D{0, 0, 0};
}

bool MM::RenderSystem::ImageChunkInfo::IsValid() const {
  if (extent_.width == 0 || extent_.depth == 0 || extent_.height == 0) {
    return false;
  }
  return true;
}

MM::RenderSystem::AllocatedImage::AllocatedImage(
    const VmaAllocator& allocator, const VkImage& image,
    const VmaAllocation& allocation, const ImageInfo& image_info)
  : image_info_(image_info),
    wrapper_(
        std::make_shared<AllocatedImageWrapper>(allocator, image, allocation)) {
  if (!image_info_.IsValid()) {
    image_info_.Reset();
    wrapper_.reset();
  }
}


MM::RenderSystem::AllocatedImage::AllocatedImage(
    AllocatedImage&& other) noexcept
  : wrapper_(std::move(other.wrapper_)) {
}

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
  wrapper_ = std::move(other.wrapper_);

  return *this;
}

const VkExtent3D& MM::RenderSystem::AllocatedImage::GetImageExtent() const {
  return image_info_.image_extent_;
}

const VkDeviceSize& MM::RenderSystem::AllocatedImage::GetImageSize() const {
  return image_info_.image_size_;
}

const VkFormat& MM::RenderSystem::AllocatedImage::GetImageFormat() const {
  return image_info_.image_format_;
}

const VkImageLayout& MM::RenderSystem::AllocatedImage::GetImageLayout() const {
  return image_info_.image_layout_;
}

const uint32_t& MM::RenderSystem::AllocatedImage::GetMipmapLevels() const {
  return image_info_.mipmap_levels_;
}

const uint32_t& MM::RenderSystem::AllocatedImage::GetArrayLayers() const {
  return image_info_.array_layers_;
}

const bool& MM::RenderSystem::AllocatedImage::CanMapped() const {
  return image_info_.can_mapped_;
}

const bool& MM::RenderSystem::AllocatedImage::IsTransformSrc() const {
  return image_info_.is_transform_src_;
}

const bool& MM::RenderSystem::AllocatedImage::IsTransformDest() const {
  return image_info_.is_transform_dest_;
}

const MM::RenderSystem::ImageInfo& MM::RenderSystem::AllocatedImage::
GetImageInfo() const {
  return image_info_;
}

VmaAllocator MM::RenderSystem::AllocatedImage::GetAllocator() const {
  if (wrapper_) {
    return wrapper_->GetAllocator();
  }
  return nullptr;
}

VkImage MM::RenderSystem::AllocatedImage::GetImage() const {
  if (wrapper_) {
    return wrapper_->GetImage();
  }
  return nullptr;
}

VmaAllocation MM::RenderSystem::AllocatedImage::GetAllocation() const {
  if (wrapper_) {
    return wrapper_->GetAllocation();
  }
  return nullptr;
}

void MM::RenderSystem::AllocatedImage::Release() { wrapper_.reset(); }

uint32_t MM::RenderSystem::AllocatedImage::UseCount() const {
  if (wrapper_) {
    return wrapper_.use_count();
  }
  return 0;
}

bool MM::RenderSystem::AllocatedImage::IsValid() const {
  return wrapper_ != nullptr && wrapper_->IsValid() && image_info_.IsValid();
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
  }
}

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

MM::RenderSystem::BufferChunkInfo::BufferChunkInfo(
    const VkDeviceSize& start_offset, const VkDeviceSize& end_offset)
  : offset_(start_offset),
    size_(end_offset) {
}

MM::RenderSystem::BufferChunkInfo& MM::RenderSystem::BufferChunkInfo::operator=(
    const BufferChunkInfo& other) noexcept {
  if (&other == this) {
    return *this;
  }
  offset_ = other.offset_;
  size_ = other.size_;

  return *this;
}

MM::RenderSystem::BufferChunkInfo& MM::RenderSystem::BufferChunkInfo::operator=(
    BufferChunkInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  offset_ = other.offset_;
  size_ = other.size_;

  other.offset_ = 0;
  other.size_ = 0;

  return *this;
}

VkDeviceSize& MM::RenderSystem::BufferChunkInfo::GetOffset() { return offset_; }

const VkDeviceSize& MM::RenderSystem::BufferChunkInfo::GetOffset() const {
  return offset_;
}

VkDeviceSize& MM::RenderSystem::BufferChunkInfo::GetSize() { return size_; }

const VkDeviceSize& MM::RenderSystem::BufferChunkInfo::GetSize() const {
  return size_;
}

void MM::RenderSystem::BufferChunkInfo::SetOffset(
    const VkDeviceSize& new_offset) {
  offset_ = new_offset;
}

void MM::RenderSystem::BufferChunkInfo::SetSize(const VkDeviceSize& new_size) {
  size_ = new_size;
}

void MM::RenderSystem::BufferChunkInfo::Reset() {
  offset_ = 0;
  size_ = 0;
}

bool MM::RenderSystem::BufferChunkInfo::IsValid() const { return size_ != 0; }

MM::RenderSystem::VertexAndIndexBuffer::VertexAndIndexBuffer(
    RenderEngine* engine) : render_engine_(engine) {
  if (engine == nullptr) {
    return;
  }

  const auto vertex_buffer_size =
      std::stoull(CONFIG_SYSTEM->GetConfig("init_vertex_buffer_size"));

  const auto index_buffer_size =
      std::stoull(CONFIG_SYSTEM->GetConfig("init_index_buffer_size"));

  vertex_buffer_ = engine->CreateBuffer(vertex_buffer_size,
                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);
  if (!vertex_buffer_.IsValid()) {
    Release();
    LOG_ERROR("Failed to create vertex total buffer.")
  }

  index_buffer_ = engine->CreateBuffer(index_buffer_size,
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);
  if (!index_buffer_.IsValid()) {
    Release();
    LOG_ERROR("Failed to create index total buffer.")
  }
}

bool MM::RenderSystem::VertexAndIndexBuffer::IsValid() const {
  return vertex_buffer_.GetBufferSize() != 0 &&
         index_buffer_.GetBufferSize() != 0 && render_engine_ != nullptr;
}

const MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::VertexAndIndexBuffer
::GetVertexBuffer() const {
  return vertex_buffer_;
}

const MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::VertexAndIndexBuffer
::GetIndexBuffer() const {
  return index_buffer_;
}

const MM::RenderSystem::BufferInfo& MM::RenderSystem::VertexAndIndexBuffer::
GetVertexBufferInfo() const {
  return vertex_buffer_.GetBufferInfo();
}

const MM::RenderSystem::BufferInfo& MM::RenderSystem::VertexAndIndexBuffer::
GetIndexBufferInfo() const {
  return index_buffer_.GetBufferInfo();
}

bool MM::RenderSystem::VertexAndIndexBuffer::AllocateBuffer(
    const std::vector<AssetType::Vertex>& vertices,
    const std::vector<uint32_t>& indexes,
    const std::shared_ptr<BufferChunkInfo>& output_vertex_buffer_chunk_info,
    const std::shared_ptr<MM::RenderSystem::BufferChunkInfo>&
    output_index_buffer_chunk_info) {
  if (vertices.empty() || indexes.empty()) {
    LOG_ERROR("Input Vertices and indexes must not be empty.")
    return false;
  }

  const VkDeviceSize vertices_size =
      sizeof(AssetType::Vertex) * vertices.size();
  VkDeviceSize vertex_offset = 0;
  const VkDeviceSize indexes_size = sizeof(uint32_t) * indexes.size();
  VkDeviceSize index_offset = 0;
  TaskSystem::Taskflow taskflow;

  VkDeviceSize vertex_end_size = 0;
  GetEndSizeAndOffset(vertex_buffer_, vertex_buffer_chunks_info_,
                      vertex_end_size, vertex_offset);

  if (vertices_size < vertex_end_size) {
    *output_vertex_buffer_chunk_info =
        BufferChunkInfo{vertex_offset, vertices_size};
  } else {
    // If the remaining space behind cannot meet the requirements,
    // scan the entire buffer area to find suitable unused space.
    if (!ScanBufferToFindSuitableArea(vertex_buffer_,
                                      vertex_buffer_chunks_info_, vertices_size,
                                      vertex_offset)) {
      return false;
    }
    *output_vertex_buffer_chunk_info =
        BufferChunkInfo{vertex_offset, vertices_size};
  }

  AllocatedBuffer vertex_stage_buffer = render_engine_->CreateBuffer(
      vertices_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

  render_engine_->CopyDataToBuffer(vertex_stage_buffer, vertices.data(), 0,
                                   vertices_size);

  const VkBufferCopy2 vertex_region =
      Utils::GetBufferCopy(vertices_size, 0, vertex_offset);
  if (!render_engine_->CopyBuffer(vertex_stage_buffer, vertex_buffer_,
                                  std::vector<VkBufferCopy2>{vertex_region})) {
    LOG_ERROR("Failed to copy new vertex data to vertex buffer.")
    return false;
  }

  vertex_stage_buffer.Release();

  VkDeviceSize index_end_size = 0;
  GetEndSizeAndOffset(index_buffer_, index_buffer_chunks_info_, index_end_size,
                      index_offset);

  if (indexes_size < index_end_size) {
    *output_index_buffer_chunk_info =
        BufferChunkInfo{index_offset, indexes_size};
  } else {
    // If the remaining space behind cannot meet the requirements,
    // scan the entire buffer area to find suitable unused space.
    if (!ScanBufferToFindSuitableArea(index_buffer_, index_buffer_chunks_info_,
                                      indexes_size, index_offset)) {
      return false;
    }
    *output_index_buffer_chunk_info =
        BufferChunkInfo{index_offset, indexes_size};
  }

  AllocatedBuffer index_stage_buffer = render_engine_->CreateBuffer(
      indexes_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

  std::vector<VkDeviceSize> new_indexes(indexes.size());
  for (std::size_t i = 0; i < new_indexes.size(); ++i) {
    new_indexes[i] = indexes[i] + vertex_offset;
  }

  if (!render_engine_->CopyDataToBuffer(index_stage_buffer, new_indexes.data(), 0,
                                   indexes_size)) {
    LOG_ERROR("Failed to copy new index data to stage buffer")
    return false;
  }

  const VkBufferCopy2 index_region =
      Utils::GetBufferCopy(indexes_size, 0, index_offset);
  if (!render_engine_->CopyBuffer(index_stage_buffer, index_buffer_,
                                  std::vector<VkBufferCopy2>{index_region})) {
    LOG_ERROR("Failed to copy new index data to index buffer.")
    return false;
  }

  index_stage_buffer.Release();

  return true;
}

void MM::RenderSystem::VertexAndIndexBuffer::Release() {
  render_engine_ = nullptr;
  vertex_buffer_.Release();
  index_buffer_.Release();
  vertex_buffer_chunks_info_.clear();
  index_buffer_chunks_info_.clear();
}

bool MM::RenderSystem::VertexAndIndexBuffer::ChooseVertexBufferReserveSize(
    const VkDeviceSize& require_size, VkDeviceSize& output_reserve_size) {
  VkDeviceSize max_buffer_size = 0;
  if (!CONFIG_SYSTEM->GetConfig("max_vertex_buffer_size", max_buffer_size)) {
    LOG_WARN(
        "The max_vertex_buffer_size"
        " is not set,vertexbuffer_size will become"
        "infinitely larger.")
    max_buffer_size = VK_WHOLE_SIZE;
  }

  VkDeviceSize total_used_size = 0;
  for (auto buffer_chunk_info = vertex_buffer_chunks_info_.begin();
       buffer_chunk_info != vertex_buffer_chunks_info_.end();
       ++buffer_chunk_info) {
    while ((*buffer_chunk_info).use_count() == 1) {
      buffer_chunk_info = vertex_buffer_chunks_info_.erase(buffer_chunk_info);
      if (buffer_chunk_info == vertex_buffer_chunks_info_.end()) {
        break;
      }
    }
    if (buffer_chunk_info == vertex_buffer_chunks_info_.end()) {
      break;
    }
    total_used_size += (*buffer_chunk_info)->GetSize();
  }
  if (require_size + total_used_size > max_buffer_size) {
    LOG_ERROR("Insufficient buffer space to load data.")
    return false;
  }

  VkDeviceSize reserve_size = vertex_buffer_.GetBufferSize();
  while (reserve_size *= 2 < require_size + total_used_size) {}
  if (reserve_size > max_buffer_size) {
    reserve_size = max_buffer_size;
  }

  output_reserve_size = reserve_size;

  return true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::ChooseIndexBufferReserveSize(
    const VkDeviceSize& require_size, VkDeviceSize& output_reserve_size) {
  VkDeviceSize max_buffer_size = 0;
  if (!CONFIG_SYSTEM->GetConfig("max_index_buffer_size", max_buffer_size)) {
    LOG_WARN(
        "The max_index_buffer_size"
        " is not set,vertexbuffer_size will become"
        "infinitely larger.")
    max_buffer_size = VK_WHOLE_SIZE;
  }

  VkDeviceSize total_used_size = 0;
  for (auto buffer_chunk_info = index_buffer_chunks_info_.begin();
       buffer_chunk_info != index_buffer_chunks_info_.end(); ++buffer_chunk_info) {
    while ((*buffer_chunk_info).use_count() == 1) {
      buffer_chunk_info = index_buffer_chunks_info_.erase(buffer_chunk_info);
      if (buffer_chunk_info == index_buffer_chunks_info_.end()) {
        break;
      }
    }
    if (buffer_chunk_info == index_buffer_chunks_info_.end()) {
      break;
    }
    total_used_size += (*buffer_chunk_info)->GetSize();
  }
  if (require_size + total_used_size > max_buffer_size) {
    LOG_ERROR("Insufficient buffer space to load data.")
    return false;
  }

  VkDeviceSize reserve_size = index_buffer_.GetBufferSize();
  while (reserve_size *= 2 < require_size + total_used_size) {
  }
  if (reserve_size > max_buffer_size) {
    reserve_size = max_buffer_size;
  }

  output_reserve_size = reserve_size;

  return true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::ChooseReserveSize(
    const BufferType& buffer_type, const VkDeviceSize& require_size, 
    VkDeviceSize& output_reserve_size) {
  if (buffer_type == BufferType::VERTEX ) {
    return ChooseVertexBufferReserveSize(require_size, output_reserve_size);
  }

  return ChooseIndexBufferReserveSize(require_size, output_reserve_size);
}

void MM::RenderSystem::VertexAndIndexBuffer::GetEndSizeAndOffset(
    const AllocatedBuffer& buffer,
    std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
    VkDeviceSize& output_end_size, VkDeviceSize& output_offset
    ) {
  // The reason why MM::RenderSystem::Utils::GetEndSizeAndOffset is not used is
  // because MM::RenderSystem::Utils::GetEndSizeAndOffset checks whether the
  // input parameters are valid, which can cause performance loss, and the place
  // where this function is called can wrap the input parameters to be valid.
  if (buffer_chunks_info.empty()) {
    output_offset = 0;
    output_end_size = buffer.GetBufferSize();
  } else {
    auto end_element = --buffer_chunks_info.end();
    while (end_element->use_count() == 1) {
      end_element = buffer_chunks_info.erase(end_element);
      if (buffer_chunks_info.empty()) {
        break;
      }
      end_element = --end_element;
    }
    if (buffer_chunks_info.empty()) {
      output_offset = 0;
      output_end_size = buffer.GetBufferSize();
    } else {
      output_offset = (*end_element)->GetOffset() + (*end_element)->GetSize();
      output_end_size = buffer.GetBufferSize() - output_offset;
    }
  }
}

bool MM::RenderSystem::VertexAndIndexBuffer::ScanBufferToFindSuitableArea(
    MM::RenderSystem::AllocatedBuffer& buffer,
    std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
    const VkDeviceSize& require_size, VkDeviceSize& output_offset) {
  if (buffer_chunks_info.empty()) {
    if (require_size > buffer.GetBufferSize()) {
      VkDeviceSize reserve_size = 0;
      if (&buffer == &vertex_buffer_) {
        if (!ChooseVertexBufferReserveSize(require_size, reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
        if (!ReserveVertexBuffer(reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
      } else {
        if (!ChooseIndexBufferReserveSize(require_size, reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
        if (!ReserveIndexBuffer(reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
      }
    }
    output_offset = 0;
    return true;
  }
  auto first_element = buffer_chunks_info.begin();
  while (first_element->use_count() == 1) {
    first_element = buffer_chunks_info.erase(first_element);
    if (first_element == buffer_chunks_info.end()) {
      break;
    }
  }
  if (first_element == buffer_chunks_info.end()) {
    if (require_size > buffer.GetBufferSize()) {
      VkDeviceSize reserve_size = 0;
      if (&buffer == &vertex_buffer_) {
        if (!ChooseVertexBufferReserveSize(require_size, reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
        if (!ReserveVertexBuffer(reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
      } else {
        if (!ChooseIndexBufferReserveSize(require_size, reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
        if (!ReserveIndexBuffer(reserve_size)) {
          LOG_ERROR("Unable to find a suitable buffer.")
          return false;
        }
      }
      output_offset = 0;
      return true;
    }
  }

  if ((*first_element)->GetOffset() > require_size) {
    output_offset = 0;
    return true;
  }

  auto next_element = ++first_element;
  for (; next_element != buffer_chunks_info.end();
         ++first_element, ++next_element) {
    const VkDeviceSize first_end =
        (*first_element)->GetOffset() + (*first_element)->GetSize();
    if (require_size < (*next_element)->GetOffset - first_end) {
      output_offset = first_end;
      return true;
    }
  }

  VkDeviceSize reserve_size = 0;
  if (&buffer == &vertex_buffer_) {
    if (!ChooseVertexBufferReserveSize(require_size, reserve_size)) {
      LOG_ERROR("Unable to find a suitable buffer.")
      return false;
    }
    if (!ReserveVertexBuffer(reserve_size)) {
      LOG_ERROR("Unable to find a suitable buffer.")
      return false;
    }
    const auto the_last_element = (--vertex_buffer_chunks_info_.end());
    output_offset =
        (*the_last_element)->GetOffset() + (*the_last_element)->GetSize();
    return true;
  }
  if (!ChooseIndexBufferReserveSize(require_size, reserve_size)) {
    LOG_ERROR("Unable to find a suitable buffer.")
    return false;
  }
  if (!ReserveIndexBuffer(reserve_size)) {
    LOG_ERROR("Unable to find a suitable buffer.")
    return false;
  }
  const auto the_last_element = (--index_buffer_chunks_info_.end());
  output_offset =
      (*the_last_element)->GetOffset() + (*the_last_element)->GetSize();
  return true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::ReserveVertexBuffer(
    const VkDeviceSize& new_buffer_size) {
  bool result_step1 = false;
  bool result_step3 = false;


  TaskSystem::Taskflow taskflow;

  AllocatedBuffer new_buffer;

  auto step1 = taskflow.emplace([&render_engine = render_engine_, &new_buffer,
                                 &result_step1, &new_buffer_size, &
                                 index_buffer_chunks_info =
                                 index_buffer_chunks_info_]() {
    new_buffer = render_engine->CreateBuffer(
        new_buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);
    if (!new_buffer.IsValid()) {
      LOG_ERROR("Failed to create new vertex buffer.")
      return;
    }
    result_step1 = true;
  });

  auto step2 = taskflow.emplace([&render_engine = render_engine_]() {
    // TODO 去除碎片化的空间

  });

  std::vector<VkBufferCopy2> regions;
  
  std::vector<VkDeviceSize> index_offsets;
  index_offsets.reserve(vertex_buffer_chunks_info_.size());
  std::vector<VkDeviceSize> new_buffer_offsets;
  new_buffer_offsets.reserve(vertex_buffer_chunks_info_.size());

  auto step3 = taskflow.emplace(
      [&regions, &vertex_buffer_chunks_info = vertex_buffer_chunks_info_,
         &index_buffer_chunks_info = index_buffer_chunks_info_, &index_offsets, &new_buffer_offsets]() {
        VkDeviceSize new_buffer_offset = 0;
        regions.reserve(vertex_buffer_chunks_info.size());
        auto index_buffer_chunk_info = index_buffer_chunks_info.begin();
        for (const auto& buffer_chunk_info : vertex_buffer_chunks_info) {
          regions.emplace_back(Utils::GetBufferCopy(
              (*buffer_chunk_info).GetSize(), (*buffer_chunk_info).GetOffset(),
              new_buffer_offset));

          // save offset data and use them after copy succeed
          index_offsets.emplace_back(buffer_chunk_info->GetOffset() -
                                     new_buffer_offset);
          new_buffer_offsets.emplace_back(new_buffer_offset);

          new_buffer_offset += (*buffer_chunk_info).GetSize();
          ++index_buffer_chunk_info;
        }
      });

  auto step4 = taskflow.emplace([&render_engine = render_engine_,
                                 &vertex_buffer = vertex_buffer_, &regions,
                                 &new_buffer, &result_step1, &result_step3]() {
    if (!result_step1) {
      return;
    }
    result_step3 = render_engine->CopyBuffer(vertex_buffer, new_buffer, regions);
    if (!result_step3) {
      LOG_ERROR("Failed to copy vertiex data to new vertex buffer.")
    }
  });

  step3.succeed(step1, step2);

  TASK_SYSTEM->RunAndWait(TaskSystem::TaskType::Render, taskflow);

  if (!result_step3) {
    LOG_ERROR("Failed to reserve vertex buffer")
    return false;
  }

  // The next process is linear and cannot be parallel.
  if (!stage_index_buffer.IsValid()) {
    LOG_ERROR("Failed to create stage index buffer")
    return false;
  }

  // TODO 直接ReserveIndexBuffer


  vertex_buffer_ = new_buffer;

  return true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::ReserveIndexBuffer(
    const VkDeviceSize& new_buffer_size) {
  bool result_step1 = false;
  bool result_step3 = false;

  TaskSystem::Taskflow taskflow;

  AllocatedBuffer new_buffer;

  auto step1 = taskflow.emplace([&render_engine = render_engine_, &new_buffer,
                                 &result_step1, &new_buffer_size]() {
    new_buffer = render_engine->CreateBuffer(
        new_buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);
    if (!new_buffer.IsValid()) {
      LOG_ERROR("Failed to create new index buffer.")
      return;
    }
    result_step1 = true;
  });

  std::vector<VkBufferCopy2> regions;

  auto step2 = taskflow.emplace(
      [&regions, &index_buffer_chunks_info = index_buffer_chunks_info_]() {
        VkDeviceSize new_buffer_offset = 0;
        regions.reserve(index_buffer_chunks_info.size());
        for (const auto& buffer_chunk_info : index_buffer_chunks_info) {
          regions.emplace_back(Utils::GetBufferCopy(
              (*buffer_chunk_info).GetSize(), (*buffer_chunk_info).GetOffset(),
              new_buffer_offset));
          buffer_chunk_info->SetOffset(new_buffer_offset);

          new_buffer_offset += (*buffer_chunk_info).GetSize();
        }
      });

  auto step3 = taskflow.emplace([&render_engine = render_engine_,
                                 &index_buffer = index_buffer_, &regions,
                                 &new_buffer, &result_step1, &result_step3]() {
    if (!result_step1) {
      return;
    }
    result_step3 =
        render_engine->CopyBuffer(index_buffer, new_buffer, regions);
    if (!result_step3) {
      LOG_ERROR("Failed to copy vertiex data to new index buffer.")
    }
  });

  step3.succeed(step1, step2);

  TASK_SYSTEM->RunAndWait(TaskSystem::TaskType::Render, taskflow);

  if (!result_step3) {
    LOG_ERROR("Failed to reserve vertex buffer")
    return false;
  }

  index_buffer_ = new_buffer;

  return true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::Reserve(const VkDeviceSize& new_vertex_buffer_size, const VkDeviceSize&
                                                     new_index_buffer_size) {
  TaskSystem::Taskflow task_flow;
  bool buffer_result = false, index_result = false;
  task_flow.emplace(
      [object = this, &buffer_result, new_vertex_buffer_size]() {
        buffer_result = (*object).ReserveVertexBuffer(new_vertex_buffer_size);
      },
      [object = this, &index_result, new_index_buffer_size]() {
        index_result = (*object).ReserveIndexBuffer(new_index_buffer_size);
      });

  TASK_SYSTEM->RunAndWait(TaskSystem::TaskType::Render, task_flow);

  if (buffer_result && index_result) {
    return true;
  }
  return false;
}

bool MM::RenderSystem::operator==(const ManagedObjectBase& lhs,
                                  const ManagedObjectBase& rhs) {
  return lhs.object_ID_ == rhs.object_ID_;
}

bool MM::RenderSystem::operator!=(const ManagedObjectBase& lhs,
                                  const ManagedObjectBase& rhs) {
  return !(lhs == rhs);
}
