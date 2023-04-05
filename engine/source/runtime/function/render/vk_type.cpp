#include "runtime/function/render/vk_type.h"

#include "runtime/function/render/vk_engine.h"

bool MM::RenderSystem::QueueFamilyIndices::isComplete() const {
  return graphics_family_.has_value() && present_family_.has_value() &&
         compute_family_.has_value();
}

MM::RenderSystem::ManagedObjectBase::ManagedObjectBase(
    const std::string& object_name, const std::uint32_t& object_ID) :
object_name_(object_name), object_ID_(object_ID){}

MM::RenderSystem::ManagedObjectBase::ManagedObjectBase(
    ManagedObjectBase&& other) noexcept : object_name_(std::move(other.object_name_)), object_ID_(other.object_ID_){
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
    const ImageBindInfo& other)
{
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
  mipmap_levels = 1;
  can_mapped_ = false;
}

bool MM::RenderSystem::ImageInfo::IsValid() const {
  if (image_size_ == 0 || image_format_ == VK_FORMAT_UNDEFINED ||
      image_layout_ == VK_IMAGE_LAYOUT_UNDEFINED || mipmap_levels == 0 ||
      image_extent_.width == 0 || image_extent_.height == 0 ||
      image_extent_.depth == 0) {
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
  is_transform_src = false;
  is_transform_dest = false;
}

bool MM::RenderSystem::BufferInfo::IsValid() const { return buffer_size_ != 0; }

MM::RenderSystem::VertexInputState::VertexInputState()
    : vertex_bind_(),
      vertex_buffer_offset_(0),
      vertex_attributes_(),
      instance_binds_(),
      instance_buffer_offset_(),
      instance_attributes_() {}

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

MM::RenderSystem::VertexInputState& MM::RenderSystem::VertexInputState::operator=(
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

MM::RenderSystem::VertexInputState& MM::RenderSystem::VertexInputState::operator=(
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
  for (const auto& bind: instance_binds_) {
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

  for (const auto& attribute: instance_attributes_) {
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
GetVertexBind() const { return vertex_bind_;
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
    AllocatedCommandBuffer&& other) noexcept : wrapper_(std::move(other.wrapper_)){}

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
  return wrapper_->RecordAndSubmitCommand(function, auto_start_end_submit, record_new_commands, submit_info_ptr);
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
  if (!ResetCommandBuffer()) {
    LOG_ERROR("Faild to record and submit command buffer!");
    return false;
  }

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
    AllocatedBuffer&& other) noexcept : buffer_info_(other.buffer_info_), wrapper_(std::move(other.wrapper_)){}

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
  return buffer_info_.is_transform_src;
}

bool MM::RenderSystem::AllocatedBuffer::IsTransformDest() const {
  return buffer_info_.is_transform_dest;
}

const MM::RenderSystem::BufferInfo& MM::RenderSystem::AllocatedBuffer::
GetBufferInfo() const {
  return buffer_info_;
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

void MM::RenderSystem::AllocatedBuffer::Release() {
  buffer_info_.Reset();
  wrapper_.reset();
}

uint32_t MM::RenderSystem::AllocatedBuffer::UseCount() const {
  return wrapper_.use_count();
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
    const VmaAllocation& allocation, const ImageInfo& image_info)
    : image_info_(image_info), wrapper_(std::make_shared<AllocatedImageWrapper>(allocator, image, allocation)) {
  if (!image_info_.IsValid()) {
    image_info_.Reset();
    wrapper_.reset();
  }
}


MM::RenderSystem::AllocatedImage::AllocatedImage(
    AllocatedImage&& other) noexcept : wrapper_(std::move(other.wrapper_)) {}

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
  return image_info_.mipmap_levels;
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

const VmaAllocator& MM::RenderSystem::AllocatedImage::GetAllocator() const {
  return wrapper_->GetAllocator();
}

const VkImage& MM::RenderSystem::AllocatedImage::GetImage() const {
  return wrapper_->GetImage();
}

const VmaAllocation& MM::RenderSystem::AllocatedImage::GetAllocation() const {
  return wrapper_->GetAllocation();
}

void MM::RenderSystem::AllocatedImage::Release() { wrapper_.reset(); }

uint32_t MM::RenderSystem::AllocatedImage::UseCount() const {
  return wrapper_.use_count();
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
    allocation_(allocation){
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

MM::RenderSystem::BufferChunkInfo::BufferChunkInfo(
    const VkDeviceSize& start_offset, const VkDeviceSize& end_offset)
      : start_offset_(start_offset), end_offset_(end_offset) {}

MM::RenderSystem::BufferChunkInfo& MM::RenderSystem::BufferChunkInfo::operator=(
    const BufferChunkInfo& other) noexcept {
  if (&other == this) {
    return *this;
  }
  start_offset_ = other.start_offset_;
  end_offset_ = other.end_offset_;

  return *this;
}

MM::RenderSystem::BufferChunkInfo& MM::RenderSystem::BufferChunkInfo::operator=(
    BufferChunkInfo&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  start_offset_ = other.start_offset_;
  end_offset_ = other.end_offset_;

  other.start_offset_ = 0;
  other.end_offset_ = 0;

  return *this;
}

const VkDeviceSize& MM::RenderSystem::BufferChunkInfo::GetStartOffset() const {
  return start_offset_;
}

const VkDeviceSize& MM::RenderSystem::BufferChunkInfo::GetEndOffset() const {
  return end_offset_;
}

MM::RenderSystem::VertexAndIndexBuffer::VertexAndIndexBuffer(
    RenderEngine* engine) {
  if (engine == nullptr) {
    return;
  }
  vertex_buffer_info_.offset_ = 0;
  vertex_buffer_info_.dynamic_offset_ = 0;
  vertex_buffer_info_.buffer_size_ =
      std::stoull(CONFIG_SYSTEM->GetConfig("init_vertex_buffer_size"));
  vertex_buffer_info_.can_mapped_ = false;

  index_buffer_info_.offset_ = 0;
  index_buffer_info_.dynamic_offset_ = 0;
  index_buffer_info_.buffer_size_ =
      std::stoull(CONFIG_SYSTEM->GetConfig("init_index_buffer_size"));
  index_buffer_info_.can_mapped_ = false;

  vertex_buffer_ = engine->CreateBuffer(
      vertex_buffer_info_.buffer_size_,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);
  if (!vertex_buffer_.IsValid()) {
    Release();
    LOG_ERROR("Failed to create vertex total buffer.")
  }

  index_buffer_ = engine->CreateBuffer(
      index_buffer_info_.buffer_size_,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);
  if (!index_buffer_.IsValid()) {
    Release();
    LOG_ERROR("Failed to create index total buffer.")
  }

  accessible_ = true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::IsValid() const {
  return vertex_buffer_info_.buffer_size_ != 0 &&
         index_buffer_info_.buffer_size_ != 0;
}

const MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::VertexAndIndexBuffer
::GetVertexBuffer() const {
  return vertex_buffer_;
}

const MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::VertexAndIndexBuffer
::GetIndexBuffer() const {
  return index_buffer_;
}

const MM::RenderSystem::BufferBindInfo& MM::RenderSystem::VertexAndIndexBuffer::
GetVertexBufferInfo() const {
  return vertex_buffer_info_;
}

const MM::RenderSystem::BufferBindInfo& MM::RenderSystem::VertexAndIndexBuffer::
GetIndexBufferInfo() const {
  return index_buffer_info_;
}

bool MM::RenderSystem::VertexAndIndexBuffer::Accessible() const {
  return accessible_;
}

bool MM::RenderSystem::VertexAndIndexBuffer::AllocateBuffer(
    const std::vector<AssetType::Vertex>& vertices,
    const std::vector<uint32_t>& indexes,
    const std::shared_ptr<BufferChunkInfo>& output_buffer_chunk_info) {
  VkDeviceSize vertices_size = sizeof(AssetType::Vertex) * vertices.size();
  VkDeviceSize indexes_size = sizeof(uint32_t) * indexes.size();


}

void MM::RenderSystem::VertexAndIndexBuffer::Release() {
  vertex_buffer_.Release();
  index_buffer_.Release();
  vertex_buffer_info_.Reset();
  index_buffer_info_.Reset();
  vertex_buffer_chunks_info.clear();
  index_buffer_chunks_info.clear();
}

bool MM::RenderSystem::VertexAndIndexBuffer::ReserveVertexBuffer() {

}

bool MM::RenderSystem::VertexAndIndexBuffer::Reserve() {
  TaskSystem::Taskflow task_flow;
  bool buffer_result = false, index_result = false;
  task_flow.emplace(
      [object = this, &buffer_result]() {
        buffer_result = (*object).ReserveVertexBuffer();
      },
      [object = this, &index_result]() {
        index_result = (*object).ReserveIndexBuffer();
      });

  TASK_SYSTEM->RunAndWait(TaskSystem::TaskType::Render, task_flow);

  if (buffer_result & index_result) {
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
