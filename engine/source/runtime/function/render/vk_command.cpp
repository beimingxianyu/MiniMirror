#include "runtime/function/render/vk_command.h"

#include "vk_engine.h"

const MM::RenderSystem::CommandBufferType& MM::RenderSystem::CommandTask::
GetCommandBufferType() const {
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
GetUsedCommandBufferNumber() const {
  return commands_.size();
}

const std::vector<MM::RenderSystem::WaitSemaphore>& MM::RenderSystem::
CommandTask::GetWaitSemaphore() const {
  return wait_semaphore_;
}

const std::vector<VkSemaphore>& MM::RenderSystem::CommandTask::
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
    const std::vector<VkSemaphore>& signal_semaphore)
  : task_flow_(task_flow), command_type_(command_type), commands_(commands),
    wait_semaphore_(wait_semaphore), signal_semaphore_(signal_semaphore) {
}

bool MM::RenderSystem::WaitSemaphore::WaitSemaphoreLessWithoutWaitStage::
operator()(const WaitSemaphore& l_value, const WaitSemaphore& r_value) const {
  return l_value.wait_semaphore_ < r_value.wait_semaphore_;
}

bool MM::RenderSystem::WaitSemaphore::IsValid() const {
  return wait_semaphore_ != nullptr;
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

MM::RenderSystem::CommandTaskFlow::~CommandTaskFlow() {
  for (const auto& task : tasks_) {
    delete task;
  }
}

MM::RenderSystem::CommandTaskFlow::CommandTaskFlow(
    CommandTaskFlow&& other) noexcept
  : root_tasks_(std::move(other.root_tasks_)),
    tasks_(std::move(other.tasks_)),
    task_count_(std::move(other.task_count_)) {
}

MM::RenderSystem::CommandTaskFlow& MM::RenderSystem::CommandTaskFlow::operator=(
    CommandTaskFlow&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  root_tasks_ = std::move(other.root_tasks_);
  tasks_ = std::move(other.tasks_);
  task_count_ = std::move(other.task_count_);

  return *this;
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    const CommandType& command_type,
    const std::function<MM::ExecuteResult(MM::RenderSystem::
        AllocatedCommandBuffer& cmd)>& commands,
    const std::vector<MM::RenderSystem::WaitSemaphore>& wait_semaphores,
    const std::vector<VkSemaphore>& signal_semaphores) {
  assert(command_type != CommandBufferType::UNDEFINED);
 const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer & cmd)>> temp{
      commands};
  CommandTask* new_command_task =
      new CommandTask(this, command_type, temp, wait_semaphores, signal_semaphores);

  root_tasks_.push_back(new_command_task);
  tasks_.push_back(new_command_task);

 switch (command_type) {
   case CommandBufferType::GRAPH:
      ++task_count_[0];
     break;
   case CommandBufferType::COMPUTE:
     ++task_count_[1];
     break;
   case CommandBufferType::TRANSFORM:
     ++task_count_[2];
     break;
    case CommandBufferType::UNDEFINED:
      LOG_ERROR("Input parameter command_type is error.")
      break;
 }

  return *new_command_task;
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    const CommandType& command_type,
    const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>
    & commands,
    const std::vector<MM::RenderSystem::WaitSemaphore>& wait_semaphores,
    const std::vector<VkSemaphore>& signal_semaphores) {
  CommandTask* new_command_task = new CommandTask(
      this, command_type, commands, wait_semaphores, signal_semaphores);

  root_tasks_.push_back(new_command_task);
  tasks_.push_back(new_command_task);

  switch (command_type) {
    case CommandBufferType::GRAPH:
      ++task_count_[0];
      break;
    case CommandBufferType::COMPUTE:
      ++task_count_[1];
      break;
    case CommandBufferType::TRANSFORM:
      ++task_count_[2];
      break;
    case CommandBufferType::UNDEFINED:
      LOG_ERROR("Input parameter command_type is error.")
      break;
  }

  return *new_command_task;
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTaskNumber() const {
  return task_count_[0] + task_count_[1] + task_count_[2];
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetGraphNumber() const {
  return task_count_[0];
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetComputeNumber() const {
  return task_count_[1];
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTransformNumber() const {
  return task_count_[2];
}

bool MM::RenderSystem::CommandTaskFlow::IsRootTask(
    const CommandTask& command_task) const {
  return std::find(root_tasks_.begin(), root_tasks_.end(), const_cast<CommandTask*>(&command_task)) != root_tasks_.end();
}

void MM::RenderSystem::CommandTaskFlow::Clear() {
  for (const auto& task: tasks_) {
    delete task;
  }

  root_tasks_.clear();
  tasks_.clear();
  task_count_.fill(0);
}

bool MM::RenderSystem::CommandTaskFlow::HaveRing() const {
  std::uint32_t task_count = GetTaskNumber();
  std::vector<CommandTaskEdge> command_task_edges;
  GetCommandTaskEdges(command_task_edges);

  std::unordered_map<CommandTask*, std::uint32_t> penetrations;
  for (const auto& task_edge : command_task_edges) {
    ++penetrations[task_edge.end_command_task_];
  }

  std::stack<CommandTask*> penetration_zero;

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
    CommandTask* no_penetration_task = penetration_zero.top();
    penetration_zero.pop();
    for (const auto& post_task: no_penetration_task->post_tasks_) {
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
    CommandTask* start, CommandTask* end):start_command_task_(start), end_command_task_(end) {}

bool MM::RenderSystem::CommandTaskFlow::CommandTaskEdge::operator<(
    const CommandTaskEdge& other) const {
  return start_command_task_ < other.start_command_task_ &&
         end_command_task_ < other.end_command_task_;
}

void MM::RenderSystem::CommandTaskFlow::RemoveRootTask(
    const CommandTask& command_task) {
  const auto target_task = std::find(root_tasks_.begin(), root_tasks_.end(),
                                     const_cast<CommandTask*>(&command_task));
  if (target_task == root_tasks_.end()) {
    return;
  }
  root_tasks_.erase(target_task);
}

void MM::RenderSystem::CommandTaskFlow::GetCommandTaskEdges(
    std::vector<CommandTaskEdge>& command_task_edges) const {
  for (const auto task: tasks_) {
    for (const auto last_task: tasks_) {
      command_task_edges.emplace_back(task, last_task); 
    }
  }
}

MM::RenderSystem::CommandTask::~CommandTask() = default;


MM::RenderSystem::CommandExecutor::CommandExecutor(RenderEngine* engine)
    : render_engine(engine), free_graph_command_buffers_(),
      free_compute_command_buffers_(), free_transform_command_buffers_(),
      executing_graph_command_buffers_(), executing_compute_command_buffers_(),
      executing_transform_command_buffers_(), task_flow_queue_(),
      task_flow_queue_mutex_(), last_run_is_run_one_frame_call_(),
      processing_task_flow_queue_(),
      wait_tasks_(), wait_tasks_mutex_(){
  if (engine == nullptr || !engine->IsValid()) {
    LOG_ERROR("Engine is invalid.")
    render_engine = nullptr;
    return;
  }

  constexpr std::uint32_t graph_command_number = 15;
  constexpr std::uint32_t compute_command_number = 5;
  constexpr std::uint32_t transform_command_number = 5;

  std::vector<VkCommandPool> graph_command_pools(graph_command_number, nullptr);
  std::vector<VkCommandPool> compute_command_pools{compute_command_number,
                                                   nullptr};
  std::vector<VkCommandPool> transform_command_pools{transform_command_number,
                                                     nullptr};

  MM_CHECK(InitCommandPolls(graph_command_pools, compute_command_pools,
                            transform_command_pools), return;)

  std::vector<VkCommandBuffer> graph_command_buffers(graph_command_number);
  std::vector<VkCommandBuffer> compute_command_buffers(compute_command_number);
  std::vector<VkCommandBuffer> transform_command_buffers(
      transform_command_number);

  MM_CHECK(
      InitCommandBuffers(graph_command_pools, compute_command_pools,
                         transform_command_pools, graph_command_buffers,
                         compute_command_buffers, transform_command_buffers),
      return;)

  MM_CHECK(
      InitAllocateCommandBuffers(graph_command_pools, compute_command_pools,
        transform_command_pools, graph_command_buffers, compute_command_buffers,
        transform_command_buffers), return;)
}

MM::RenderSystem::CommandExecutor::CommandExecutor(RenderEngine* engine,
    const uint32_t& graph_command_number,
    const uint32_t& compute_command_number,
    const uint32_t& transform_command_number)
    : render_engine(engine),
      free_graph_command_buffers_(),
      free_compute_command_buffers_(),
      free_transform_command_buffers_(),
      executing_graph_command_buffers_(),
      executing_compute_command_buffers_(),
      executing_transform_command_buffers_(),
      task_flow_queue_(),
      task_flow_queue_mutex_(),
      last_run_is_run_one_frame_call_(),
      processing_task_flow_queue_(),
      wait_tasks_(),
      wait_tasks_mutex_(){
  if (engine == nullptr || !engine->IsValid()) {
    LOG_ERROR("Engine is invalid.")
    render_engine = nullptr;
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
}

MM::RenderSystem::RenderFuture MM::RenderSystem::CommandExecutor::Run(
    CommandTaskFlow& command_task_flow) {
  command_task_flow.GetTaskNumber()
}

bool MM::RenderSystem::CommandExecutor::IsValid() const {
  return render_engine != nullptr && GetGraphCommandNumber() != 0 &&
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
    vkDestroyCommandPool(render_engine->GetDevice(), command_pool, nullptr);
  }
  for (auto& command_pool : compute_command_pools) {
    if (command_pool == nullptr) {
      continue;
    }
    vkDestroyCommandPool(render_engine->GetDevice(), command_pool, nullptr);
  }
  for (auto& command_pool : transform_command_pools) {
    if (command_pool == nullptr) {
      continue;
    }
    vkDestroyCommandPool(render_engine->GetDevice(), command_pool, nullptr);
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
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::InitCommandPolls(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  VkCommandPoolCreateInfo command_buffer_create_info =
      Utils::GetCommandPoolCreateInfo(render_engine->GetGraphQueueIndex());

  for (std::uint32_t i = 0; i < graph_command_pools.size(); ++i) {
    VK_CHECK(
        vkCreateCommandPool(render_engine->GetDevice(), &command_buffer_create_info,
                            nullptr, &graph_command_pools[i]),
        LOG_ERROR("Failed to create graph command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  command_buffer_create_info.queueFamilyIndex =
      render_engine->GetComputeQueueIndex();
  for (std::uint32_t i = 0; i < compute_command_pools.size(); ++i) {
    VK_CHECK(
        vkCreateCommandPool(render_engine->GetDevice(),
                            &command_buffer_create_info,
                            nullptr, &compute_command_pools[i]),
        LOG_ERROR("Failed to create compute command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ExecuteResult::INITIALIZATION_FAILED;)
  }

  command_buffer_create_info.queueFamilyIndex =
      render_engine->GetTransformQueueIndex();
  for (std::uint32_t i = 0; i < transform_command_pools.size(); ++i) {
    VK_CHECK(
        vkCreateCommandPool(render_engine->GetDevice(),
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
        vkAllocateCommandBuffers(render_engine->GetDevice(),
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
        vkAllocateCommandBuffers(render_engine->GetDevice(),
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
        vkAllocateCommandBuffers(render_engine->GetDevice(),
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
            render_engine, render_engine->GetGraphQueueIndex(),
            render_engine->GetGraphQueue(), graph_command_pools[i],
            graph_command_buffers[i]));
    graph_command_pools[i] = nullptr;

    if (!free_graph_command_buffers_.top()->IsValid()) {
      LOG_ERROR("Failed to create graph allocate command buffer.")
      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                               transform_command_pools);
      return ExecuteResult::INITIALIZATION_FAILED;
    }
  }

  for (std::uint32_t i = 0; i < compute_command_buffers.size(); ++i) {
    free_compute_command_buffers_.push(
        std::make_unique<AllocatedCommandBuffer>(
            render_engine, render_engine->GetComputeQueueIndex(),
            render_engine->GetComputeQueue(), compute_command_pools[i],
            compute_command_buffers[i]));
    compute_command_pools[i] = nullptr;

    if (!free_compute_command_buffers_.top()->IsValid()) {
      LOG_ERROR("Failed to create compute allocate command buffer.")
      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                               transform_command_pools);
      return ExecuteResult::INITIALIZATION_FAILED;
    }
  }

  for (std::uint32_t i = 0; i < transform_command_buffers.size(); ++i) {
    free_transform_command_buffers_.push(
        std::make_unique<AllocatedCommandBuffer>(
            render_engine, render_engine->GetTransformQueueIndex(),
            render_engine->GetTransformQueue(), transform_command_pools[i],
            transform_command_buffers[i]));
    transform_command_pools[i] = nullptr;

    if (!free_transform_command_buffers_.top()->IsValid()) {
      LOG_ERROR("Failed to create transform allocate command buffer.")
      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                               transform_command_pools);
      return ExecuteResult::INITIALIZATION_FAILED;
    }
  }

  return ExecuteResult::SUCCESS;
}

bool MM::RenderSystem::CommandExecutor::ExecutingCommandBuffer::
IsComplete() const {
  VK_CHECK(vkGetFenceStatus(render_engine_->GetDevice(),
                            command_buffers_[0]->GetFence()),
           return false);
  return true;
}

