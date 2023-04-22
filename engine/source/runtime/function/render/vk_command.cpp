#include "runtime/function/render/vk_command.h"

#include "vk_engine.h"

const MM::RenderSystem::CommandBufferType& MM::RenderSystem::CommandTask::
GetCommandType() const {
  return command_type_;
}

const MM::RenderSystem::CommandTaskFlow& MM::RenderSystem::CommandTask::
GetCommandTaskFlow() const {
  return *task_flow_;
}

const std::vector<std::function<MM::ExecuteResult(MM::RenderSystem::
  AllocatedCommandBuffer& cmd)>>& MM::RenderSystem::CommandTask::
GetCommands() const {
  return commands_;
}

std::uint32_t MM::RenderSystem::CommandTask::
GetRequireCommandBufferNumber() const {
  return commands_.size();
}

const std::vector<MM::RenderSystem::WaitSemaphore>& MM::RenderSystem::
CommandTask::GetWaitSemaphore() const {
  return wait_semaphore_;
}

const std::vector<MM::RenderSystem::AllocateSemaphore>&
MM::RenderSystem::CommandTask::
GetSignalSemaphore() const {
  return signal_semaphore_;
}

bool MM::RenderSystem::CommandTask::IsValid() const {
  return task_flow_ != nullptr && commands_.size() != 0;
}

MM::RenderSystem::CommandTask::CommandTask(CommandTaskFlow* task_flow,
    const CommandType& command_type,
    const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>& commands, 
    const std::vector<WaitSemaphore>& wait_semaphore,
    const std::vector<AllocateSemaphore>& signal_semaphore)
  : task_flow_(task_flow),
    command_type_(command_type), commands_(commands),
    wait_semaphore_(wait_semaphore), signal_semaphore_(signal_semaphore) {
}

bool MM::RenderSystem::WaitSemaphore::WaitSemaphoreLessWithoutWaitStage::
operator()(const WaitSemaphore& l_value, const WaitSemaphore& r_value) const {
  return l_value.wait_semaphore_.GetSemaphore() < r_value.wait_semaphore_.GetSemaphore();
}

bool MM::RenderSystem::WaitSemaphore::IsValid() const {
  return wait_semaphore_.GetSemaphore() != nullptr;
}

void MM::RenderSystem::CommandBufferInfo::Reset() { is_recorded_ = false; }

bool MM::RenderSystem::CommandBufferInfo::IsValid() const { return true; }

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBuffer(
    RenderEngine* engine, const std::uint32_t& queue_index,
    const VkQueue& queue, const VkCommandPool& command_pool,
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
  vkWaitForFences(engine_->GetDevice(), 1, &command_fence_, VK_TRUE, 100000000);

  vkDestroyCommandPool(engine_->GetDevice(), command_pool_, nullptr);
  vkDestroyFence(engine_->device_, command_fence_, nullptr);
  command_pool_ = nullptr;
  command_buffer_ = nullptr;
  command_fence_ = nullptr;
}

MM::RenderSystem::AllocatedCommandBuffer::AllocatedCommandBufferWrapper::
    AllocatedCommandBufferWrapper(MM::RenderSystem::RenderEngine* engine,
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
  VK_CHECK(vkResetCommandPool(engine_->device_, command_pool_, 0),
           LOG_ERROR("Faild to reset command buffer!");
           return false;)

  return true;
}

MM::RenderSystem::CommandTaskFlow::~CommandTaskFlow() {
  for (const auto& task : tasks_) {
    delete task;
  }
}

MM::RenderSystem::CommandTaskFlow::CommandTaskFlow(
    CommandTaskFlow&& other) noexcept
{
  std::lock(task_sync_, other.task_sync_);
  root_tasks_ = std::move(other.root_tasks_);
  tasks_ = std::move(other.tasks_);

  task_sync_.unlock();
  other.task_sync_.unlock();
}

MM::RenderSystem::CommandTaskFlow& MM::RenderSystem::CommandTaskFlow::operator=(
    CommandTaskFlow&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  std::lock(task_sync_, other.task_sync_);

  root_tasks_ = std::move(other.root_tasks_);
  tasks_ = std::move(other.tasks_);

  task_sync_.unlock();
  other.task_sync_.unlock();
  return *this;
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    const CommandType& command_type,
    const std::function<MM::ExecuteResult(MM::RenderSystem::
        AllocatedCommandBuffer& cmd)>& commands,
    const std::vector<MM::RenderSystem::WaitSemaphore>& wait_semaphores,
    const std::vector<MM::RenderSystem::AllocateSemaphore>& signal_semaphores) {
  assert(command_type != CommandBufferType::UNDEFINED);
 const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer & cmd)>> temp{
      commands};
  std::unique_lock<std::shared_mutex> guard(task_sync_);

  tasks_.emplace_back(std::make_unique<CommandTask>(
      this, command_type, temp, wait_semaphores, signal_semaphores));

  root_tasks_.emplace_back(&(tasks_.back()));
  tasks_.back()->this_unique_ptr_ = &(tasks_.back());
  

  return *tasks_.back();
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    const CommandType& command_type,
    const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>
    & commands,
    const std::vector<MM::RenderSystem::WaitSemaphore>& wait_semaphores,
    const std::vector<MM::RenderSystem::AllocateSemaphore>& signal_semaphores) {
  std::unique_lock<std::shared_mutex> guard(task_sync_);

  tasks_.emplace_back(std::make_unique<CommandTask>(
      this, command_type, commands, wait_semaphores, signal_semaphores));

  root_tasks_.emplace_back(&(tasks_.back()));
  tasks_.back()->this_unique_ptr_ = &(tasks_.back());

  return *tasks_.back();
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTaskNumber() const {
  return tasks_.size();
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetGraphNumber() const {
  std::uint32_t result = 0;
  for (const auto& task: tasks_) {
    if (task->GetCommandType() == CommandType::GRAPH) {
      ++result;
    }    
  }

  return result;
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetComputeNumber() const {
  std::uint32_t result = 0;
  for (const auto& task : tasks_) {
    if (task->GetCommandType() == CommandType::COMPUTE) {
      ++result;
    }
  }

  return result;
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTransformNumber() const {
  std::uint32_t result = 0;
  for (const auto& task : tasks_) {
    if (task->GetCommandType() == CommandType::TRANSFORM) {
      ++result;
    }
  }

  return result;
}

bool MM::RenderSystem::CommandTaskFlow::IsRootTask(
    const CommandTask& command_task) const {
  return std::find(root_tasks_.begin(), root_tasks_.end(), command_task.this_unique_ptr_) != root_tasks_.end();
}

void MM::RenderSystem::CommandTaskFlow::Clear() {
  root_tasks_.clear();
  tasks_.clear();
}

bool MM::RenderSystem::CommandTaskFlow::HaveRing() const {
  std::uint32_t task_count = GetTaskNumber();
  std::vector<CommandTaskEdge> command_task_edges = GetCommandTaskEdges();

  std::unordered_map<std::unique_ptr<CommandTask>*, std::uint32_t> penetrations;
  for (const auto& task_edge : command_task_edges) {
    ++penetrations[task_edge.end_command_task_];
  }

  std::stack<std::unique_ptr<CommandTask>*> penetration_zero;

  for (const auto& count : penetrations) {
    if (count.second == 0) {
      penetration_zero.push(count.first);
    }
  }

  task_count -= task_count - penetrations.size();
  task_count -= penetration_zero.size();
  if (task_count == 0) {
    return true;
  }

  while (!penetration_zero.empty()) {
    const std::unique_ptr<CommandTask>* no_penetration_task = penetration_zero.top();
    penetration_zero.pop();
    for (const auto& post_task: (*no_penetration_task)->post_tasks_) {
      if (--penetrations[post_task] == 0) {
        penetration_zero.push(post_task);
        --task_count;
      }
    }
  }

  if (task_count == 0) {
    return true;
  }

  return false;
}

bool MM::RenderSystem::CommandTaskFlow::IsValid() const { return HaveRing(); }

MM::RenderSystem::CommandTaskFlow::CommandTaskEdge::CommandTaskEdge(
    std::unique_ptr<CommandTask>* start, std::unique_ptr<CommandTask>* end)
    : start_command_task_(start), end_command_task_(end) {}

bool MM::RenderSystem::CommandTaskFlow::CommandTaskEdge::operator<(
    const CommandTaskEdge& other) const {
  return start_command_task_ < other.start_command_task_ &&
         end_command_task_ < other.end_command_task_;
}

void MM::RenderSystem::CommandTaskFlow::RemoveRootTask(
    const CommandTask& command_task) {
  const auto target_task = std::find(root_tasks_.begin(), root_tasks_.end(),
                                    command_task.this_unique_ptr_);
  if (target_task == root_tasks_.end()) {
    return;
  }
  root_tasks_.erase(target_task);
}

void MM::RenderSystem::CommandTaskFlow::GetCommandTaskEdges(
    std::vector<CommandTaskEdge>& command_task_edges) const {
  for (auto task: tasks_) {
    for (auto last_task: tasks_) {
      command_task_edges.emplace_back(&task, &last_task); 
    }
  }
}

std::vector<MM::RenderSystem::CommandTaskFlow::CommandTaskEdge>
MM::RenderSystem ::CommandTaskFlow::GetCommandTaskEdges() const {
  std::vector<CommandTaskEdge> command_task_edges;
  for (auto task : tasks_) {
    for (auto last_task : tasks_) {
      command_task_edges.emplace_back(&task, &last_task);
    }
  }

  return command_task_edges;
}

MM::RenderSystem::CommandTask::~CommandTask() = default;


MM::RenderSystem::RenderFuture::RenderFuture(const std::uint32_t& task_flow_ID,
    const std::shared_ptr<ExecuteResult>& future_execute_result,
    const std::vector<std::shared_ptr<bool>>& command_complete_states)
  : task_flow_ID_(task_flow_ID), execute_result_(future_execute_result),
    command_complete_states_(command_complete_states) {
}

MM::RenderSystem::CommandExecutor::CommandExecutor(RenderEngine* engine)
    : render_engine_(engine), free_graph_command_buffers_(),
      free_compute_command_buffers_(), free_transform_command_buffers_(),
      recoding_graph_command_buffer_number_(),
      recording_compute_command_buffer_number_(),
      recording_transform_command_buffer_number_(),
      executing_graph_command_buffers_(), executing_compute_command_buffers_(),
      executing_transform_command_buffers_(),
      executing_graph_command_buffers_mutex_(),
      executing_compute_command_buffers_mutex_(),
      executing_transform_command_buffers_mutex_(), task_flow_queue_(),
      task_flow_queue_mutex_(), last_run_is_run_one_frame_call_(),
      processing_task_flow_queue_(),
      wait_tasks_(), wait_tasks_mutex_(),
      task_that_have_already_been_accessed_(),
      submitted_task_(),
      can_be_submitted_tasks_(),
      pre_task_not_submit_task_() {
  if (engine == nullptr || !engine->IsValid()) {
    LOG_ERROR("Engine is invalid.")
    render_engine_ = nullptr;
    return;
  }

  constexpr std::uint32_t graph_command_number = 15 * render_engine_->GetFlightFrameNumber();
  constexpr std::uint32_t compute_command_number = 5 * render_engine_->GetFlightFrameNumber();
  constexpr std::uint32_t transform_command_number = 5 * render_engine_->GetFlightFrameNumber();

  std::vector<VkCommandPool> graph_command_pools(graph_command_number, nullptr);
  std::vector<VkCommandPool> compute_command_pools{compute_command_number,
                                                   nullptr};
  std::vector<VkCommandPool> transform_command_pools{transform_command_number,
                                                     nullptr};
  MM_CHECK(InitCommandPolls(graph_command_pools, compute_command_pools,
                            transform_command_pools),
           return;)

  std::vector<VkCommandBuffer> graph_command_buffers(graph_command_number);
  std::vector<VkCommandBuffer> compute_command_buffers(compute_command_number);
  std::vector<VkCommandBuffer> transform_command_buffers(
      transform_command_number);

  MM_CHECK(
      InitCommandBuffers(graph_command_pools, compute_command_pools,
                         transform_command_pools, graph_command_buffers,
                         compute_command_buffers, transform_command_buffers),
      return;)

  MM_CHECK(InitAllocateCommandBuffers(
               graph_command_pools, compute_command_pools,
               transform_command_pools, graph_command_buffers,
               compute_command_buffers, transform_command_buffers),
           return;)


  MM_CHECK(InitSemaphores(graph_command_number + compute_command_number +
                              transform_command_number,
                          graph_command_pools, compute_command_pools,
                          transform_command_pools),
           return;)
    

}

MM::RenderSystem::CommandExecutor::CommandExecutor(RenderEngine* engine,
    const uint32_t& graph_command_number,
    const uint32_t& compute_command_number,
    const uint32_t& transform_command_number)
    : render_engine_(engine),
      free_graph_command_buffers_(),
      free_compute_command_buffers_(),
      free_transform_command_buffers_(),
      recoding_graph_command_buffer_number_(),
      recording_compute_command_buffer_number_(),
      recording_transform_command_buffer_number_(),
      executing_graph_command_buffers_(),
      executing_compute_command_buffers_(),
      executing_transform_command_buffers_(),
      executing_graph_command_buffers_mutex_(),
      executing_compute_command_buffers_mutex_(),
      executing_transform_command_buffers_mutex_(),
      task_flow_queue_(),
      task_flow_queue_mutex_(),
      last_run_is_run_one_frame_call_(),
      processing_task_flow_queue_(),
      wait_tasks_(),
      wait_tasks_mutex_(),
      semaphores_() {
  if (engine == nullptr || !engine->IsValid()) {
    LOG_ERROR("Engine is invalid.")
    render_engine_ = nullptr;
    return;
  }

  if (graph_command_number < 1 || compute_command_number < 1 || transform_command_number < 1) {
    LOG_ERROR("graph_command_number,compute_command_number and transform_command_number must greater than 1.")
    render_engine_ = nullptr;
    return;
  }

  std::vector<VkCommandPool> graph_command_pools(graph_command_number, nullptr);
  std::vector<VkCommandPool> compute_command_pools{compute_command_number,
                                                   nullptr};
  std::vector<VkCommandPool> transform_command_pools{transform_command_number,
                                                     nullptr};

  MM_CHECK(InitCommandPolls(graph_command_pools, compute_command_pools,
                            transform_command_pools),
           return;)

  std::vector<VkCommandBuffer> graph_command_buffers(graph_command_number);
  std::vector<VkCommandBuffer> compute_command_buffers(compute_command_number);
  std::vector<VkCommandBuffer> transform_command_buffers(
      transform_command_number);

  MM_CHECK(
      InitCommandBuffers(graph_command_pools, compute_command_pools,
                         transform_command_pools, graph_command_buffers,
                         compute_command_buffers, transform_command_buffers),
      return;)

  MM_CHECK(InitAllocateCommandBuffers(
               graph_command_pools, compute_command_pools,
               transform_command_pools, graph_command_buffers,
               compute_command_buffers, transform_command_buffers),
           return;)

  MM_CHECK(
      InitSemaphores(graph_command_number + compute_command_number +
        transform_command_number, graph_command_pools, compute_command_pools,
        transform_command_pools),
        return;)
  }

MM::RenderSystem::RenderFuture MM::RenderSystem::CommandExecutor::Run(
    CommandTaskFlow& command_task_flow) {
  std::uint32_t task_id = Math::Random::GetRandomUint32();
  std::shared_ptr<ExecuteResult> execute_result{
      std::make_shared<ExecuteResult>(ExecuteResult::SUCCESS)};
  std::vector<std::shared_ptr<bool>> complete_states;
  for (const auto& command_task: command_task_flow.tasks_) {
    if (command_task->post_tasks_.empty()) {
      complete_states.emplace_back(std::make_shared<bool>(false));
    }
  }
  {
    std::lock_guard<std::mutex> guard{task_flow_queue_mutex_};
    task_flow_queue_.emplace_front(std::move(command_task_flow), task_id,
                                   execute_result, complete_states);
  }

  if (!processing_task_flow_queue_) {
    TaskSystem::Taskflow task_flow;
    task_flow.emplace([this_object = this]() { this_object->ProcessTask(); });
    TASK_SYSTEM->Run(TaskSystem::TaskType::Render, task_flow);
  }

  return RenderFuture{task_id, execute_result, complete_states};
}

bool MM::RenderSystem::CommandExecutor::IsValid() const {
  return render_engine_ != nullptr && GetGraphCommandNumber() != 0 &&
         GetComputeCommandNumber() != 0 && GetTransformCommandNumber() != 0;
}

void MM::RenderSystem::CommandExecutor::ClearWhenConstructFailed(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  for (auto& command_pool: graph_command_pools) {
    if (command_pool == nullptr) {
      continue;
    }
    vkDestroyCommandPool(render_engine_->GetDevice(), command_pool, nullptr);
  }
  for (auto& command_pool : compute_command_pools) {
    if (command_pool == nullptr) {
      continue;
    }
    vkDestroyCommandPool(render_engine_->GetDevice(), command_pool, nullptr);
  }
  for (auto& command_pool : transform_command_pools) {
    if (command_pool == nullptr) {
      continue;
    }
    vkDestroyCommandPool(render_engine_->GetDevice(), command_pool, nullptr);
  }

  while (!free_graph_command_buffers_.empty()) {
    free_graph_command_buffers_.pop();
  }
  while (!free_compute_command_buffers_.empty()) {
    free_compute_command_buffers_.pop();
  }
  while (!free_transform_command_buffers_.empty()) {
    free_transform_command_buffers_.pop();
  }

  while (!semaphores_.empty()) {
    const VkSemaphore pop_object = semaphores_.top();
    vkDestroySemaphore(render_engine_->GetDevice(), pop_object, nullptr);
    semaphores_.pop();
  }
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::InitCommandPolls(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  VkCommandPoolCreateInfo command_buffer_create_info =
      Utils::GetCommandPoolCreateInfo(render_engine_->GetGraphQueueIndex());

  for (std::uint32_t i = 0; i < graph_command_pools.size(); ++i) {
    VK_CHECK(
        vkCreateCommandPool(render_engine_->GetDevice(), &command_buffer_create_info,
                            nullptr, &graph_command_pools[i]),
        LOG_ERROR("Failed to create graph command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  command_buffer_create_info.queueFamilyIndex =
      render_engine_->GetComputeQueueIndex();
  for (std::uint32_t i = 0; i < compute_command_pools.size(); ++i) {
    VK_CHECK(
        vkCreateCommandPool(render_engine_->GetDevice(),
                            &command_buffer_create_info,
                            nullptr, &compute_command_pools[i]),
        LOG_ERROR("Failed to create compute command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  command_buffer_create_info.queueFamilyIndex =
      render_engine_->GetTransformQueueIndex();
  for (std::uint32_t i = 0; i < transform_command_pools.size(); ++i) {
    VK_CHECK(
        vkCreateCommandPool(render_engine_->GetDevice(),
                            &command_buffer_create_info,
                            nullptr, &transform_command_pools[i]),
        LOG_ERROR("Failed to create transform command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::InitCommandBuffers(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools,
    std::vector<VkCommandBuffer>& graph_command_buffers,
    std::vector<VkCommandBuffer>& compute_command_buffers,
    std::vector<VkCommandBuffer>& transform_command_buffers) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info =
      Utils::GetCommandBufferAllocateInfo(graph_command_pools[0], 1);

  for (std::uint32_t i = 0; i < graph_command_buffers.size(); ++i) {
    command_buffer_allocate_info.commandPool = graph_command_pools[i];
    VK_CHECK(
        vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                 &command_buffer_allocate_info,
                                 graph_command_buffers.data()),
        LOG_ERROR("Failed to allocate graph command buffers.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  for (std::uint32_t i = 0; i < compute_command_buffers.size(); ++i) {
    command_buffer_allocate_info.commandPool = compute_command_pools[i];
    VK_CHECK(
        vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                 &command_buffer_allocate_info,
                                 compute_command_buffers.data()),
        LOG_ERROR("Failed to allocate compute command buffers.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  for (std::uint32_t i = 0; i < transform_command_buffers.size(); ++i) {
    command_buffer_allocate_info.commandPool = transform_command_pools[i];
    VK_CHECK(
        vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                 &command_buffer_allocate_info,
                                 transform_command_buffers.data()),
        LOG_ERROR("Failed to allocate transform command buffers.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::InitAllocateCommandBuffers(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools,
    std::vector<VkCommandBuffer>& graph_command_buffers,
    std::vector<VkCommandBuffer>& compute_command_buffers,
    std::vector<VkCommandBuffer>& transform_command_buffers) {
  for (std::uint32_t i = 0; i < graph_command_buffers.size(); ++i) {
    free_graph_command_buffers_.push(
        std::make_unique<AllocatedCommandBuffer>(
            render_engine_, render_engine_->GetGraphQueueIndex(),
            render_engine_->GetGraphQueue(), graph_command_pools[i],
            graph_command_buffers[i]));

    if (!free_graph_command_buffers_.top()->IsValid()) {
      LOG_ERROR("Failed to create graph allocate command buffer.")
      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                               transform_command_pools);
      return ExecuteResult::INITIALIZATION_FAILED;
    }

    graph_command_pools[i] = nullptr;
  }

  for (std::uint32_t i = 0; i < compute_command_buffers.size(); ++i) {
    free_compute_command_buffers_.push(
        std::make_unique<AllocatedCommandBuffer>(
            render_engine_, render_engine_->GetComputeQueueIndex(),
            render_engine_->GetComputeQueue(), compute_command_pools[i],
            compute_command_buffers[i]));

    if (!free_compute_command_buffers_.top()->IsValid()) {
      LOG_ERROR("Failed to create compute allocate command buffer.")
      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                               transform_command_pools);
      return ExecuteResult::INITIALIZATION_FAILED;
    }

    compute_command_pools[i] = nullptr;
  }

  for (std::uint32_t i = 0; i < transform_command_buffers.size(); ++i) {
    free_transform_command_buffers_.push(
        std::make_unique<AllocatedCommandBuffer>(
            render_engine_, render_engine_->GetTransformQueueIndex(),
            render_engine_->GetTransformQueue(), transform_command_pools[i],
            transform_command_buffers[i]));

    if (!free_transform_command_buffers_.top()->IsValid()) {
      LOG_ERROR("Failed to create transform allocate command buffer.")
      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                               transform_command_pools);
      return ExecuteResult::INITIALIZATION_FAILED;
    }

    transform_command_pools[i] = nullptr;
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::InitSemaphores(
    const std::uint32_t& need_semaphore_number,
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  const VkSemaphoreCreateInfo semaphore_create_info =
      Utils::GetSemaphoreCreateInfo();
  VkSemaphore new_semaphore{nullptr};
  for (std::uint32_t i = 0; i < need_semaphore_number; ++i) {
    VK_CHECK(
        vkCreateSemaphore(render_engine_->GetDevice(), &semaphore_create_info,
                          nullptr, &new_semaphore),
        LOG_ERROR("Failed to create VkSemaphore.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)

    semaphores_.push(new_semaphore);
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::AddCommandBuffer(
    const CommandType& command_type,
    const std::uint32_t& new_command_buffer_num) {
  std::uint32_t command_family_index;
  VkQueue command_queue;
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>* free_allocate_command_buffer;

  switch (command_type) {
    case CommandBufferType::GRAPH:
      command_family_index = render_engine_->GetGraphQueueIndex();
      command_queue = render_engine_->GetGraphQueue();
      free_allocate_command_buffer = &free_graph_command_buffers_;
      break;
    case CommandBufferType::COMPUTE:
      command_family_index = render_engine_->GetComputeQueueIndex();
      command_queue = render_engine_->GetComputeQueue();
      free_allocate_command_buffer = &free_compute_command_buffers_;
      break;
    case CommandBufferType::TRANSFORM:
      command_family_index = render_engine_->GetTransformQueueIndex();
      command_queue = render_engine_->GetTransformQueue();
      free_allocate_command_buffer = &free_transform_command_buffers_;
      break;
    default:
      return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }

  VkCommandPoolCreateInfo command_pool_create_info =
      Utils::GetCommandPoolCreateInfo(command_family_index);
  VkCommandBufferAllocateInfo command_buffer_allocate_info =
      Utils::GetCommandBufferAllocateInfo(nullptr);
  std::vector<VkCommandPool> command_pools;
  std::vector<VkCommandBuffer> command_buffers;
  command_pools.reserve(new_command_buffer_num);
  command_buffers.reserve(new_command_buffer_num);

  bool execute_state = true;
  ExecuteResult result = ExecuteResult::SUCCESS;

  for (std::uint32_t i = 0; i < new_command_buffer_num; ++i) {
    VkCommandPool new_command_pool{nullptr};
    VK_CHECK(vkCreateCommandPool(render_engine_->GetDevice(),
                                 &command_pool_create_info, nullptr,
                                 &new_command_pool),
             result = Utils::VkResultToMMResult(VK_RESULT_CODE);
             execute_state = false;)
    if (execute_state == false) {
      for (auto& command_pool : command_pools) {
        vkDestroyCommandPool(render_engine_->GetDevice(), command_pool,
                             nullptr);
      }

      return result;
    }
  }

  for (std::uint32_t i = 0; i < new_command_buffer_num; ++i) {
    VkCommandBuffer new_command_buffer{nullptr};
    command_buffer_allocate_info.commandPool = command_pools[i];
    VK_CHECK(vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                      &command_buffer_allocate_info,
                                      &new_command_buffer),
             result = Utils::VkResultToMMResult(VK_RESULT_CODE);
             execute_state = false;
      )
    if (execute_state == false) {
      for (auto& command_pool: command_pools) {
        vkDestroyCommandPool(render_engine_->GetDevice(), command_pool,
                             nullptr);
      }

      return result;
    }
  }

  std::vector<AllocatedCommandBuffer> new_allocated_command_buffers;
  new_allocated_command_buffers.reserve(new_command_buffer_num);
  for (std::uint32_t i = 0; i < new_command_buffer_num; ++i) {
    new_allocated_command_buffers.emplace_back(
        render_engine_, command_family_index, command_queue, command_pools[i],
        command_buffers[i]);

    if (!new_allocated_command_buffers.back().IsValid()) {
      LOG_ERROR("The construction of AllocateCommandBuffer failed.")
      for (auto& command_pool : command_pools) {
        if (command_pool == nullptr) {
          continue;
        }

        vkDestroyCommandPool(render_engine_->GetDevice(), command_pool,
                             nullptr);
      }
      return ExecuteResult::INITIALIZATION_FAILED;
    }

    command_pools[i] = nullptr;
  }

  for (auto& allocated_command_buffer: new_allocated_command_buffers) {
    free_allocate_command_buffer->push(std::make_unique<AllocatedCommandBuffer>(std::move(allocated_command_buffer)));
  }

  return result;
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun::
CommandTaskFlowToBeRun(CommandTaskFlow&& command_task_flow,
    const std::uint32_t& task_flow_ID,
    const std::shared_ptr<ExecuteResult>& execute_result,
    const std::vector<std::shared_ptr<bool>>& is_completes)
  : command_task_flow_(std::move(command_task_flow)),
    task_flow_ID_(task_flow_ID), execute_result_(execute_result) {
  for (const auto& is_complete: is_completes) {
    is_completes_.push(is_complete);
  }
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun::
CommandTaskFlowToBeRun(CommandTaskFlowToBeRun&& other) noexcept
  : command_task_flow_(std::move(other.command_task_flow_)),
    task_flow_ID_(other.task_flow_ID_),
    execute_result_(std::move(other.execute_result_)),
    is_completes_(std::move(other.is_completes_)) {
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun& MM::RenderSystem::
CommandExecutor::CommandTaskFlowToBeRun::operator=(
    CommandTaskFlowToBeRun&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  command_task_flow_ = std::move(other.command_task_flow_);
  task_flow_ID_ = other.task_flow_ID_;
  execute_result_ = std::move(other.execute_result_);
  is_completes_ = std::move(other.is_completes_);

  other.task_flow_ID_ = 0;

  return *this;
}

void MM::RenderSystem::CommandExecutor::UpdateCommandTaskLine(
    std::unique_ptr<CommandTask>& new_command_task,
    const std::unique_ptr<CommandTask>* old_command_task_ptr) {
  if (&new_command_task == old_command_task_ptr) {
    return;
  }

  new_command_task->this_unique_ptr_ = &new_command_task;
  for (auto& pre_task : new_command_task->pre_tasks_) {
    for (auto& pre_post_task : (*(*pre_task)).post_tasks_) {
      if (pre_post_task == old_command_task_ptr) {
        pre_post_task = &new_command_task;
        break;
      }
    }
  }

  for (auto& post_task : new_command_task->post_tasks_) {
    for (auto& post_pre_task : (*(*post_task)).pre_tasks_) {
      if (post_pre_task == old_command_task_ptr) {
        post_pre_task = &new_command_task;
        break;
      }
    }
  }
}

MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::CommandTaskToBeSubmit(
    std::unique_ptr<CommandTask>&& command_task,
    std::vector<VkSemaphore>&& default_wait_semaphore,
    std::vector<VkSemaphore>&& default_signal_semaphore)
  : command_task_(std::move(command_task)),
    default_wait_semaphore_(std::move(default_wait_semaphore)),
    default_signal_semaphore_(std::move(default_signal_semaphore)) {
  UpdateCommandTaskLine(command_task_, &command_task);
}

MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::CommandTaskToBeSubmit(
    CommandTaskToBeSubmit&& other) noexcept
: command_task_(std::move(other.command_task_)),
  default_wait_semaphore_(std::move(other.default_wait_semaphore_)),
  default_signal_semaphore_(std::move(other.default_signal_semaphore_)) {
  UpdateCommandTaskLine(command_task_, &(other.command_task_));
}

MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit& MM::RenderSystem::
CommandExecutor::CommandTaskToBeSubmit::operator=(
    CommandTaskToBeSubmit&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  command_task_ = std::move(other.command_task_);
  default_wait_semaphore_ = std::move(other.default_wait_semaphore_);
  default_signal_semaphore_ = std::move(other.default_signal_semaphore_);
  UpdateCommandTaskLine(command_task_, &(other.command_task_));

  return *this;
}

bool MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::operator<(
    const CommandTaskToBeSubmit& other) const {
  return command_task_ < other.command_task_;
}

MM::RenderSystem::CommandExecutor::ExecutingTask::ExecutingTask(
    RenderEngine* engine,
    std::vector<std::unique_ptr<AllocatedCommandBuffer>>&& command_buffer,
    std::unique_ptr<CommandTask>&& command_task,
    const std::weak_ptr<ExecuteResult>& execute_result,
    const std::optional<std::weak_ptr<bool>>& is_complete,
    std::vector<VkSemaphore>&& default_wait_semaphore,
    std::vector<VkSemaphore>&& default_signal_semaphore,
    const std::vector<WaitSemaphore>& external_wait_semaphores,
    const std::vector<AllocateSemaphore>& external_signal_semaphores)
    : render_engine_(engine),
      command_buffers_(std::move(command_buffer)),
      command_task_(std::move(std::move(command_task))),
      execute_result_(execute_result),
      is_complete_(is_complete),
      wait_semaphore_(std::move(default_wait_semaphore)),
      signal_semaphore_(std::move(default_signal_semaphore)),
      default_wait_semaphore_number_(default_wait_semaphore.size() +
                                     external_wait_semaphores.size()),
      default_signal_semaphore_number_(default_signal_semaphore.size() +
                                       external_signal_semaphores.size()),
      external_wait_semaphores_(external_wait_semaphores),
      external_signal_semaphores_(external_signal_semaphores) {
  for (const auto& external_wait_semaphore: external_signal_semaphores_) {
    wait_semaphore_.push_back(external_wait_semaphore.GetSemaphore());
  }

  for (const auto& external_signal_semaphore: external_signal_semaphores_) {
    signal_semaphore_.push_back(external_signal_semaphore.GetSemaphore());
  }

  UpdateCommandTaskLine(command_task_, &command_task);
}

MM::RenderSystem::CommandExecutor::ExecutingTask::ExecutingTask(
    ExecutingTask&& other) noexcept: render_engine_(other.render_engine_),
                                     command_buffers_(std::move(other.command_buffers_)),
                                     command_task_(std::move(other.command_task_)),
                                     execute_result_(std::move(other.execute_result_)),
                                     is_complete_(std::move(other.is_complete_)),
                                     wait_semaphore_(std::move(other.wait_semaphore_)),
                                     signal_semaphore_(std::move(other.signal_semaphore_)),
                                     default_wait_semaphore_number_(other.default_wait_semaphore_number_),
                                     default_signal_semaphore_number_(
                                         other.default_signal_semaphore_number_),
                                     external_wait_semaphores_(std::move(other.external_wait_semaphores_)),
                                     external_signal_semaphores_(
                                         std::move(other.external_signal_semaphores_)) {
  UpdateCommandTaskLine(command_task_, &(other.command_task_));
}

MM::RenderSystem::CommandExecutor::ExecutingTask& MM::RenderSystem::
CommandExecutor::ExecutingTask::operator=(ExecutingTask&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  render_engine_ = other.render_engine_;
  command_buffers_ = std::move(other.command_buffers_);
  command_task_ = std::move(other.command_task_);
  execute_result_ = std::move(other.execute_result_);
  is_complete_ = std::move(other.is_complete_);
  wait_semaphore_ = std::move(other.wait_semaphore_);
  signal_semaphore_ = std::move(other.signal_semaphore_);
  default_wait_semaphore_number_ = other.default_wait_semaphore_number_;
  default_signal_semaphore_number_ = other.default_signal_semaphore_number_;
  external_wait_semaphores_ = std::move(other.external_wait_semaphores_);
  external_signal_semaphores_ =
      std::move(other.external_signal_semaphores_);
  UpdateCommandTaskLine(command_task_, &(other.command_task_));

  return *this;
}

bool MM::RenderSystem::CommandExecutor::ExecutingTask::
IsComplete() const {
  VK_CHECK(vkGetFenceStatus(render_engine_->GetDevice(),
                            command_buffers_[0]->GetFence()),
           return false)
  return true;
}

std::vector<VkSemaphore> MM::RenderSystem::CommandExecutor::GetSemaphore(
    std::uint32_t require_number) {
  if (semaphores_.size() < require_number) {
    const VkSemaphoreCreateInfo semaphore_create_info =
        Utils::GetSemaphoreCreateInfo();
    VkSemaphore new_semaphore;
    while (semaphores_.size() < require_number) {
      vkCreateSemaphore(render_engine_->GetDevice(), &semaphore_create_info,
                        nullptr, &new_semaphore);
      semaphores_.push(new_semaphore);
    }
  }

  std::vector<VkSemaphore> result;
  result.reserve(require_number);

  for (std::uint32_t i = 0; i < require_number; ++i) {
    result.push_back(semaphores_.top());
    semaphores_.pop();
  }

  return result;
}

void MM::RenderSystem::CommandExecutor::ProcessCompleteTask() {
  {
    std::lock_guard<std::mutex> guard{executing_graph_command_buffers_mutex_};
    for (auto executing_command_buffer =
             executing_graph_command_buffers_.begin();
         executing_command_buffer != executing_graph_command_buffers_.end();
         ++executing_command_buffer) {
      if (executing_command_buffer->IsComplete()) {
        if (executing_command_buffer->is_complete_.has_value()) {
          if (!executing_command_buffer->is_complete_.value().expired()) {
            *(executing_command_buffer->is_complete_.value().lock()) = true;
          }
        }
        for (auto& complete_buffer :
             executing_command_buffer->command_buffers_) {
          free_graph_command_buffers_.push(std::move(complete_buffer));
        }
        for (std::uint32_t i = 0; i < executing_command_buffer->default_wait_semaphore_number_; ++i) {
          semaphores_.push(executing_command_buffer->wait_semaphore_[0]);
        }
        executing_graph_command_buffers_.erase(executing_command_buffer);
      }
    }
  }

  {
    std::lock_guard<std::mutex> guard{executing_compute_command_buffers_mutex_};
    for (auto executing_command_buffer =
             executing_compute_command_buffers_.begin();
         executing_command_buffer != executing_compute_command_buffers_.end();
         ++executing_command_buffer) {
      if (executing_command_buffer->IsComplete()) {
        if (executing_command_buffer->is_complete_.has_value()) {
          if (!executing_command_buffer->is_complete_.value().expired()) {
            *(executing_command_buffer->is_complete_.value().lock()) = true;
          }
        }
        for (auto& complete_buffer :
             executing_command_buffer->command_buffers_) {
          free_compute_command_buffers_.push(std::move(complete_buffer));
        }
        for (std::uint32_t i = 0;
             i < executing_command_buffer->default_wait_semaphore_number_;
             ++i) {
          semaphores_.push(executing_command_buffer->wait_semaphore_[0]);
        }
        executing_compute_command_buffers_.erase(executing_command_buffer);
      }
    }
  }

  {
    std::lock_guard<std::mutex> guard{
        executing_transform_command_buffers_mutex_};
    for (auto executing_command_buffer =
             executing_transform_command_buffers_.begin();
         executing_command_buffer != executing_transform_command_buffers_.end();
         ++executing_command_buffer) {
      if (executing_command_buffer->IsComplete()) {
        if (executing_command_buffer->is_complete_.has_value()) {
          if (!executing_command_buffer->is_complete_.value().expired()) {
            *(executing_command_buffer->is_complete_.value().lock()) = true;
          }
        }
        for (auto& complete_buffer :
             executing_command_buffer->command_buffers_) {
          free_transform_command_buffers_.push(std::move(complete_buffer));
        }
        for (std::uint32_t i = 0;
             i < executing_command_buffer->default_wait_semaphore_number_;
             ++i) {
          semaphores_.push(executing_command_buffer->wait_semaphore_[0]);
        }
        executing_transform_command_buffers_.erase(executing_command_buffer);
      }
    }
  }
}

void MM::RenderSystem::CommandExecutor::ProcessWaitTask() {
  // This function cannot obtain the \ref task_flow_queue_mutex_ lock
  // as it will cause a deadlock.
  std::unique_lock<std::mutex> wait_guard{wait_tasks_mutex_};
  if (!wait_tasks_.empty()) {
    std::list<CommandTaskFlowToBeRun> wait_task_list;
    for (const auto& wait_task : wait_tasks_) {
      for (auto iter = task_flow_queue_.begin();
           iter != task_flow_queue_.end();) {
        if (wait_task == iter->task_flow_ID_) {
          wait_task_list.emplace_back(std::move(*iter));
          task_flow_queue_.erase(iter);
          continue;
        }
        ++iter;
      }
    }

    task_flow_queue_.merge(wait_task_list);
  }
  wait_guard.unlock();
}

void MM::RenderSystem::CommandExecutor::
ProcessRequireCommandBufferNumberLagerThanExecutorHaveCommandBufferNumber(
    const CommandTaskFlowToBeRun& command_task_flow, bool& skip_task_flow) {
  for (const auto& task : command_task_flow.command_task_flow_.tasks_) {
    if (task->command_type_ == CommandType::GRAPH &&
        task->commands_.size() > GetGraphCommandNumber()) {
      std::uint32_t new_count =
          static_cast<std::uint32_t>(task->commands_.size()) -
          GetGraphCommandNumber();
      MM_CHECK(AddCommandBuffer(CommandType::GRAPH, new_count),
               skip_task_flow = true;
               LOG_ERROR("Failed to create new command buffer, skip this "
                         "command_task_flow."))
    }

    if (task->command_type_ == CommandType::COMPUTE &&
        task->commands_.size() > GetComputeCommandNumber()) {
      std::uint32_t new_count =
          static_cast<std::uint32_t>(task->commands_.size()) -
          GetComputeCommandNumber();
      MM_CHECK(AddCommandBuffer(CommandType::COMPUTE, new_count),
               skip_task_flow = true;
               LOG_ERROR("Failed to create new command buffer, skip this "
                         "command_task_flow."));
    }

    if (task->command_type_ == CommandBufferType::TRANSFORM &&
        task->commands_.size() > GetTransformCommandNumber()) {
      std::uint32_t new_count =
          static_cast<std::uint32_t>(task->commands_.size()) -
          GetTransformCommandNumber();
      MM_CHECK(AddCommandBuffer(CommandType::TRANSFORM, new_count),
               skip_task_flow = true;
               LOG_ERROR("Failed to create new command buffer, skip this "
                         "command_task_flow."));
    }
  }
}

void MM::RenderSystem::CommandExecutor::ProcessRootTaskAndSubTask(MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun&
    command_task_flow) {
  for (auto& root_task : command_task_flow.command_task_flow_.root_tasks_) {
    for (auto iter = command_task_flow.command_task_flow_.tasks_.begin();
         iter != command_task_flow.command_task_flow_.tasks_.end(); ++iter) {
      if (&(*iter) == root_task) {
        can_be_submitted_tasks_.emplace_back(
            std::move(*iter), std::vector<VkSemaphore>{},
            GetSemaphore((*iter)->post_tasks_.size()));
        task_that_have_already_been_accessed_.emplace(root_task);  // deletable
        command_task_flow.command_task_flow_.tasks_.erase(iter);
      }
    }
    std::uint32_t sub_index = 0;
    for (const auto& sub_task :
         can_be_submitted_tasks_.back().command_task_->post_tasks_) {
      if (task_that_have_already_been_accessed_.find(sub_task) !=
          task_that_have_already_been_accessed_.end()) {
        for (auto iter = command_task_flow.command_task_flow_.tasks_.begin();
             iter != command_task_flow.command_task_flow_.tasks_.end();
             ++iter) {
          if (&(*iter) == sub_task) {
            pre_task_not_submit_task_.emplace_back(
                std::move(*iter),
                std::vector<VkSemaphore>{
                    can_be_submitted_tasks_.back().default_signal_semaphore_.at(
                        sub_index)},
                GetSemaphore((*iter)->post_tasks_.size()));
            task_that_have_already_been_accessed_.emplace(sub_task);
            command_task_flow.command_task_flow_.tasks_.erase(iter);
          }
        }
      } else {
        for (auto& task : pre_task_not_submit_task_) {
          if (task.command_task_->this_unique_ptr_ == sub_task) {
            task.default_wait_semaphore_.emplace_back(
                can_be_submitted_tasks_.back().default_signal_semaphore_.at(
                    sub_index));
          }
        }
      }
      ++sub_index;
    }
  }
}

void MM::RenderSystem::CommandExecutor::ChooseVariableByCommandType(
    CommandType command_type, std::uint32_t*& free_command_buffer_number,
    std::stack<std::unique_ptr<AllocatedCommandBuffer>>*& free_command_buffers,
    std::list<ExecutingTask>*& submit_group,
    std::atomic_uint32_t*& recording_command_buffer_number,
    std::uint32_t free_graph_command_buffer_number,
    std::uint32_t free_compute_command_buffer_number,
    std::uint32_t free_transform_command_buffer_number,
    std::list<ExecutingTask>& graph_submit_group,
    std::list<ExecutingTask>& compute_submit_group,
    std::list<ExecutingTask>& transform_submit_group) {
  switch (command_type) {
    case CommandBufferType::GRAPH:
      free_command_buffer_number = &free_graph_command_buffer_number;
      free_command_buffers = &free_graph_command_buffers_;
      // executing_task_list = &executing_graph_command_buffers_;
      submit_group = &graph_submit_group;
      recording_command_buffer_number = &recoding_graph_command_buffer_number_;
      break;
    case CommandBufferType::COMPUTE:
      free_command_buffer_number = &free_compute_command_buffer_number;
      free_command_buffers = &free_compute_command_buffers_;
      // executing_task_list = &executing_compute_command_buffers_;
      submit_group = &compute_submit_group;
      recording_command_buffer_number =
          &recording_compute_command_buffer_number_;
      break;
    case CommandBufferType::TRANSFORM:
      free_command_buffer_number = &free_transform_command_buffer_number;
      free_command_buffers = &free_transform_command_buffers_;
      // executing_task_list = &executing_transform_command_buffers_;
      submit_group = &transform_submit_group;
      recording_command_buffer_number =
          &recording_transform_command_buffer_number_;
      break;
  }
}

void MM::RenderSystem::CommandExecutor::ProcessOneCanSubmitTask(
    std::list<CommandTaskToBeSubmit>::iterator& command_task_to_be_submit,
    CommandTaskFlowToBeRun& command_task_flow,
    std::uint32_t free_graph_command_buffer_number,
    std::uint32_t free_compute_command_buffer_number,
    std::uint32_t free_transform_command_buffer_number,
    std::list<ExecutingTask>& graph_submit_group,
    std::list<ExecutingTask>& compute_submit_group,
    std::list<ExecutingTask>& transform_submit_group) {
  std::uint32_t* free_command_buffer_number{nullptr};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>* free_command_buffers;
  // std::list<ExecutingTask>* executing_task_list;
  std::list<ExecutingTask>* submit_group{nullptr};
  std::atomic_uint32_t* recording_command_buffer_number{nullptr};

  ChooseVariableByCommandType(
      command_task_to_be_submit->command_task_->GetCommandType(),
      free_command_buffer_number, free_command_buffers, submit_group,
      recording_command_buffer_number, free_graph_command_buffer_number,
      free_compute_command_buffer_number, free_transform_command_buffer_number,
      graph_submit_group, compute_submit_group, transform_submit_group);

  if (command_task_to_be_submit->command_task_
          ->GetRequireCommandBufferNumber() < *free_command_buffer_number) {
    std::vector<std::unique_ptr<AllocatedCommandBuffer>>
        acquired_command_buffer;
    acquired_command_buffer.reserve(command_task_to_be_submit->command_task_
                                        ->GetRequireCommandBufferNumber());
    for (std::uint32_t i = 0; i < command_task_to_be_submit->command_task_
                                      ->GetRequireCommandBufferNumber();
         ++i) {
      acquired_command_buffer.emplace_back(
          free_command_buffers->top().release());
      free_command_buffers->pop();
      ++(*recording_command_buffer_number);
    }
    std::optional<std::weak_ptr<bool>> is_complete;
    if (command_task_to_be_submit->command_task_->post_tasks_.empty()) {
      is_complete = command_task_flow.is_completes_.top();
      command_task_flow.is_completes_.pop();
    }

    submit_group->emplace_back(
        render_engine_, std::move(acquired_command_buffer),
        std::move(command_task_to_be_submit->command_task_),
        command_task_flow.execute_result_, is_complete,
        std::move(command_task_to_be_submit->default_wait_semaphore_),
        std::move(command_task_to_be_submit->default_signal_semaphore_),
        command_task_to_be_submit->command_task_->GetWaitSemaphore(),
        command_task_to_be_submit->command_task_->GetSignalSemaphore());

    command_task_to_be_submit =
        can_be_submitted_tasks_.erase(command_task_to_be_submit);

    /*TaskSystem::Taskflow task_flow;
    task_flow.emplace(
        [&executing_task_list_in = executing_task_list]() {
          executing_task_list_in->back().Run();
        });
    TASK_SYSTEM->Run(TaskSystem::TaskType::Render, task_flow);*/

    *free_command_buffer_number -=
        submit_group->back().command_task_->GetRequireCommandBufferNumber();
  } else {
    TaskSystem::Taskflow taskflow;
    std::list<ExecutingTask>* input_submit_group{
        new std::list<ExecutingTask>{std::move(*submit_group)}};
    taskflow.emplace(
        [&submit_group = input_submit_group, this_object = this]() {
      if (!submit_group->empty()) {
        this_object->SubmitTasks(submit_group);
      }
    });
    TASK_SYSTEM->Run(TaskSystem::TaskType::Render, taskflow);
    ProcessCompleteTask();
    free_graph_command_buffer_number = GetFreeGraphCommandNumber();
    free_compute_command_buffer_number = GetFreeComputeCommandNumber();
    free_transform_command_buffer_number = GetFreeTransformCommandNumber();
    ++command_task_to_be_submit;
  }
}

void MM::RenderSystem::CommandExecutor::ProcessNextStepCanSubmitTask(
    std::list<CommandTaskToBeSubmit>& next_can_be_submitted_tasks,
    CommandTaskFlowToBeRun& command_task_flow) {
  for (auto iter = pre_task_not_submit_task_.begin();
       iter != pre_task_not_submit_task_.end(); ++iter) {
    bool is_all_submit = true;
    for (const auto& pre_task : (*iter).command_task_->pre_tasks_) {
      if (submitted_task_.count(pre_task) == 0) {
        is_all_submit = false;
      }
    }

    if (is_all_submit) {
      next_can_be_submitted_tasks.emplace_back(std::move(*iter));
      pre_task_not_submit_task_.erase(iter);
    }
  }
  for (const auto& next_can_be_submitted_task : next_can_be_submitted_tasks) {
    std::uint32_t sub_index = 0;
    for (auto& sub_task :
         next_can_be_submitted_task.command_task_->post_tasks_) {
      if (task_that_have_already_been_accessed_.find(sub_task) !=
          task_that_have_already_been_accessed_.end()) {
        for (auto iter = command_task_flow.command_task_flow_.tasks_.begin();
             iter != command_task_flow.command_task_flow_.tasks_.end();
             ++iter) {
          if (&(*iter) == sub_task) {
            pre_task_not_submit_task_.emplace_back(
                std::move(*iter),
                std::vector<VkSemaphore>{
                    next_can_be_submitted_task.default_signal_semaphore_.at(
                        sub_index)},
                GetSemaphore((*iter)->post_tasks_.size()));
            task_that_have_already_been_accessed_.emplace(sub_task);
            command_task_flow.command_task_flow_.tasks_.erase(iter);
          }
        }
      } else {
        for (auto& task : pre_task_not_submit_task_) {
          if (task.command_task_->this_unique_ptr_ == sub_task) {
            task.default_wait_semaphore_.emplace_back(
                next_can_be_submitted_task.default_signal_semaphore_.at(
                    sub_index));
          }
        }
      }
      ++sub_index;
    }
  }
  can_be_submitted_tasks_.merge(next_can_be_submitted_tasks);
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::SubmitTasks(
    std::list<ExecutingTask>* input_tasks) {
  if (input_tasks->empty()) {
    return ExecuteResult::SUCCESS;
  }
  assert(input_tasks->front().command_task_->GetCommandType() !=
         CommandBufferType::UNDEFINED);

  std::list<ExecutingTask> task{std::move(*input_tasks)};

}

void MM::RenderSystem::CommandExecutor::ProcessTask() {
  do {
    ProcessCompleteTask();

    std::unique_lock<std::mutex> task_guard{task_flow_queue_mutex_};
    if (!task_flow_queue_.empty()) {
      ProcessWaitTask();

      CommandTaskFlowToBeRun command_task_flow = std::move(task_flow_queue_.back());
      task_flow_queue_.erase(--task_flow_queue_.end());
      task_guard.unlock();

      bool skip_task_flow = false;
      ProcessRequireCommandBufferNumberLagerThanExecutorHaveCommandBufferNumber(
          command_task_flow, skip_task_flow);
      if (skip_task_flow) {
        continue;
      }

      ProcessRootTaskAndSubTask(command_task_flow);

      while (!can_be_submitted_tasks_.empty()) {
        std::uint32_t free_graph_command_buffer_number = GetFreeGraphCommandNumber();
        std::uint32_t free_compute_command_buffer_number =
            GetFreeComputeCommandNumber();
        std::uint32_t free_transform_command_buffer_number =
            GetFreeTransformCommandNumber();

        std::list<ExecutingTask> graph_submit_group;
        std::list<ExecutingTask> compute_submit_group;
        std::list<ExecutingTask> transform_submit_group;

        std::list<CommandTaskToBeSubmit> next_can_be_submitted_tasks;
        for (auto command_task_to_be_submit = can_be_submitted_tasks_.begin();
             command_task_to_be_submit != can_be_submitted_tasks_.end();) {
          assert(command_task_to_be_submit->command_task_->GetCommandType() !=
                 CommandBufferType::UNDEFINED);
          ProcessOneCanSubmitTask(command_task_to_be_submit, command_task_flow,
                                  free_graph_command_buffer_number,
                                  free_compute_command_buffer_number,
                                  free_transform_command_buffer_number,
                                  graph_submit_group, compute_submit_group,
                                  transform_submit_group);
        }

        ProcessNextStepCanSubmitTask(next_can_be_submitted_tasks,
                                     command_task_flow);

        TaskSystem::Taskflow taskflow;
        std::list<ExecutingTask>* input_graph_submit_group{
            new std::list<ExecutingTask>{std::move(graph_submit_group)}};
        std::list<ExecutingTask>* input_compute_submit_group{
            new std::list<ExecutingTask>{std::move(compute_submit_group)}};
        std::list<ExecutingTask>* input_transform_submit_group{
            new std::list<ExecutingTask>{std::move(transform_submit_group)}};
        taskflow.emplace([&graph_submit_group = input_graph_submit_group,
                          &compute_submit_group = input_compute_submit_group,
                          &transform_submit_group = input_transform_submit_group,
                          this_object = this]() {
          if (!graph_submit_group->empty()) {
            this_object->SubmitTasks(graph_submit_group);
          }
          if (!compute_submit_group->empty()) {
            this_object->SubmitTasks(compute_submit_group);
          }
          if (!transform_submit_group->empty()) {
            this_object->SubmitTasks(transform_submit_group);
          }
        });
        TASK_SYSTEM->Run(TaskSystem::TaskType::Render, taskflow);
      }

      task_that_have_already_been_accessed_.clear();
      submitted_task_.clear();
      can_be_submitted_tasks_.clear();
      pre_task_not_submit_task_.clear();
    }

    std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
  } while (executing_graph_command_buffers_.empty() &&
           executing_compute_command_buffers_.empty() &&
           executing_transform_command_buffers_.empty() && task_flow_queue_.
           empty());

  processing_task_flow_queue_ = false;
}

