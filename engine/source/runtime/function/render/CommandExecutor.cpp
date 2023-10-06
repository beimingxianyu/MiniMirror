//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/CommandExecutor.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::CommandExecutor::CommandExecutor(RenderEngine* engine)
    : render_engine_(engine),
      valid_(true),
      free_graph_command_buffers_(),
      free_compute_command_buffers_(),
      free_transform_command_buffers_(),
      general_command_buffers_(),
      general_command_buffers_acquire_release_mutex_(),
      general_command_buffers_acquire_release_condition_variable_(),
      general_garph_command_wait_flag_(false),
      general_compute_command_wait_flag_(false),
      general_transform_command_wait_flag_(false),
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
      // last_run_is_run_one_frame_call_(),
      processing_task_flow_queue_(),
      wait_tasks_(),
      wait_tasks_mutex_(),
      semaphores_(),
      // signal_recovery_semaphore_()
      submit_failed_to_be_recycled_semaphore_(),
      submit_failed_to_be_recycled_semaphore_current_index_(),
      submit_failed_to_be_recycled_semaphore_mutex_(),
      recycled_semaphore_(),
      recycled_semaphore_mutex_(),
      submit_failed_to_be_recycled_command_buffer_(),
      submit_failed_to_be_recycled_command_buffer_mutex_() {
  if (engine == nullptr || !engine->IsValid()) {
    MM_LOG_ERROR("Engine is invalid.");
    render_engine_ = nullptr;
    return;
  }

  graph_command_number_ = 15 * render_engine_->GetFlightFrameNumber();
  compute_command_number_ = 5 * render_engine_->GetFlightFrameNumber();
  transform_command_number_ = 5 * render_engine_->GetFlightFrameNumber();

  std::vector<VkCommandPool> graph_command_pools(graph_command_number_ + 3,
                                                 nullptr);
  std::vector<VkCommandPool> compute_command_pools{compute_command_number_ + 3,
                                                   nullptr};
  std::vector<VkCommandPool> transform_command_pools{
      transform_command_number_ + 3, nullptr};
  if (InitCommandPolls(graph_command_pools, compute_command_pools,
                       transform_command_pools)
          .Exception(
              MM_ERROR_DESCRIPTION(Failed to initialization command polls.))
          .IsError()) {
    return;
  }

  std::vector<VkCommandBuffer> graph_command_buffers(graph_command_number_ + 1);
  std::vector<VkCommandBuffer> compute_command_buffers(compute_command_number_ +
                                                       1);
  std::vector<VkCommandBuffer> transform_command_buffers(
      transform_command_number_ + 1);

  if (InitCommandBuffers(graph_command_pools, compute_command_pools,
                         transform_command_pools, graph_command_buffers,
                         compute_command_buffers, transform_command_buffers)
          .Exception(
              MM_ERROR_DESCRIPTION(Failed to initialization command buffers))
          .IsError()) {
    return;
  }

  if (InitGeneralAllocatedCommandBuffers(
          graph_command_pools, compute_command_pools, transform_command_pools,
          graph_command_buffers, compute_command_buffers,
          transform_command_buffers)
          .Exception(MM_ERROR_DESCRIPTION(
              Failed to initialization general allocated command buffers.))
          .IsError()) {
    return;
  }

  if (InitAllocatedCommandBuffers(
          graph_command_pools, compute_command_pools, transform_command_pools,
          graph_command_buffers, compute_command_buffers,
          transform_command_buffers)
          .Exception(MM_ERROR_DESCRIPTION(
              Failed to initialization allocated command buffers.))
          .IsError()) {
    return;
  }

  if (InitSemaphores(graph_command_number_ + compute_command_number_ +
                         transform_command_number_,
                     graph_command_pools, compute_command_pools,
                     transform_command_pools)
          .Exception(MM_ERROR_DESCRIPTION(Failed to initialization semaphores.))
          .IsError()) {
    return;
  }

  TaskSystem::Taskflow taskflow;
  taskflow.emplace([this_object = this]() {
    this_object->RecycledSemaphoreThatSubmittedFailed();
  });
  MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, taskflow);
}

MM::RenderSystem::CommandExecutor::~CommandExecutor() {
  std::unique_lock<std::mutex> guard{wait_tasks_mutex_};
  wait_tasks_.clear();
  guard.unlock();
  while (processing_task_flow_queue_) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
  }
}

MM::RenderSystem::CommandExecutor::CommandExecutor(
    RenderEngine* engine, const uint32_t& graph_command_number,
    const uint32_t& compute_command_number,
    const uint32_t& transform_command_number)
    : render_engine_(engine),
      graph_command_number_(graph_command_number),
      compute_command_number_(compute_command_number),
      transform_command_number_(transform_command_number),
      valid_(true),
      free_graph_command_buffers_(),
      free_compute_command_buffers_(),
      free_transform_command_buffers_(),
      general_command_buffers_(),
      general_command_buffers_acquire_release_mutex_(),
      general_command_buffers_acquire_release_condition_variable_(),
      general_garph_command_wait_flag_(false),
      general_compute_command_wait_flag_(false),
      general_transform_command_wait_flag_(false),
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
      // last_run_is_run_one_frame_call_(),
      processing_task_flow_queue_(),
      wait_tasks_(),
      wait_tasks_mutex_(),
      executing_command_task_flows_(),
      command_task_render_resource_states_(),
      command_task_render_resource_states_mutex_(),
      semaphores_(),
      submit_failed_to_be_recycled_semaphore_(),
      submit_failed_to_be_recycled_semaphore_current_index_(),
      submit_failed_to_be_recycled_semaphore_mutex_(),
      recycled_semaphore_(),
      recycled_semaphore_mutex_(),
      submit_failed_to_be_recycled_command_buffer_(),
      submit_failed_to_be_recycled_command_buffer_mutex_() {
  if (engine == nullptr || !engine->IsValid()) {
    MM_LOG_ERROR("Engine is invalid.");
    render_engine_ = nullptr;
    return;
  }

  if (graph_command_number < 1 || compute_command_number < 1 ||
      transform_command_number < 1) {
    MM_LOG_ERROR(
        "graph_command_number,compute_command_number and "
        "transform_command_number must greater than 1.");
    render_engine_ = nullptr;
    return;
  }

  std::vector<VkCommandPool> graph_command_pools(graph_command_number + 3,
                                                 nullptr);
  std::vector<VkCommandPool> compute_command_pools{compute_command_number + 3,
                                                   nullptr};
  std::vector<VkCommandPool> transform_command_pools{
      transform_command_number + 3, nullptr};

  if (InitCommandPolls(graph_command_pools, compute_command_pools,
                       transform_command_pools)
          .Exception(
              MM_ERROR_DESCRIPTION(Falied to initialization command polls.))
          .IsError()) {
    return;
  }

  std::vector<VkCommandBuffer> graph_command_buffers(graph_command_number + 3,
                                                     nullptr);
  std::vector<VkCommandBuffer> compute_command_buffers(
      compute_command_number + 3, nullptr);
  std::vector<VkCommandBuffer> transform_command_buffers(
      transform_command_number + 3, nullptr);

  if (InitCommandBuffers(graph_command_pools, compute_command_pools,
                         transform_command_pools, graph_command_buffers,
                         compute_command_buffers, transform_command_buffers)
          .Exception(
              MM_ERROR_DESCRIPTION(Falied to initialization command buffers.))
          .IsError()) {
    return;
  }

  InitGeneralAllocatedCommandBuffers(
      graph_command_pools, compute_command_pools, transform_command_pools,
      graph_command_buffers, compute_command_buffers, transform_command_buffers)
      .Exception();

  InitAllocatedCommandBuffers(
      graph_command_pools, compute_command_pools, transform_command_pools,
      graph_command_buffers, compute_command_buffers, transform_command_buffers)
      .Exception();

  if (InitSemaphores(graph_command_number + compute_command_number +
                         transform_command_number,
                     graph_command_pools, compute_command_pools,
                     transform_command_pools)
          .Exception(MM_ERROR_DESCRIPTION(Failed to initialization semaphores.))
          .IsError()) {
    return;
  }

  TaskSystem::Taskflow taskflow;
  taskflow.emplace([this_object = this]() {
    this_object->RecycledSemaphoreThatSubmittedFailed();
  });
  MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, taskflow);
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetGraphCommandNumber() const {
  return graph_command_number_;
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetComputeCommandNumber()
    const {
  return compute_command_number_;
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetTransformCommandNumber()
    const {
  return transform_command_number_;
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetFreeGraphCommandNumber()
    const {
  return free_graph_command_buffers_.size();
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetFreeComputeCommandNumber()
    const {
  return free_compute_command_buffers_.size();
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetFreeTransformCommandNumber()
    const {
  return free_transform_command_buffers_.size();
}

MM::RenderSystem::RenderFuture MM::RenderSystem::CommandExecutor::Run(
    CommandTaskFlow&& command_task_flow) {
  if (!command_task_flow.IsValid()) {
    MM_LOG_ERROR("Command task flow is valid.");
    return RenderFuture{};
  }

  std::uint32_t task_id = Math::Random::GetRandomUint32();
  std::shared_ptr<Result<Nil, ErrorResult>> execute_result{
      std::make_shared<Result<Nil, ErrorResult>>(st_execute_success)};
  std::uint32_t complete_state_count;
  for (const auto& command_task : command_task_flow.tasks_) {
    if (command_task->post_tasks_.empty() && !command_task->is_sub_task_) {
      ++complete_state_count;
    }
  }
  std::shared_ptr<std::atomic_uint32_t> complete_state{
      std::make_shared<std::atomic_uint32_t>(complete_state_count)};
  if (lock_count_.load(std::memory_order_acquire)) {
    std::lock_guard guard{task_flow_submit_during_lockdown_mutex_};
    task_flow_submit_during_lockdown_.emplace_back(
        std::move(command_task_flow), task_id, execute_result, complete_state);
  } else {
    {
      std::lock_guard<std::mutex> guard{task_flow_queue_mutex_};
      task_flow_queue_.emplace_back(std::move(command_task_flow), task_id,
                                    execute_result, complete_state);
    }

    if (!processing_task_flow_queue_) {
      TaskSystem::Taskflow task_flow;
      task_flow.emplace([this_object = this]() { this_object->ProcessTask(); });
      MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, task_flow);
    }
  }

  return RenderFuture{this, task_id, execute_result, complete_state};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::RunAndWait(
    CommandTaskFlow&& command_task_flow) {
  return Run(std::move(command_task_flow)).Get();
}

MM::RenderSystem::RenderFuture MM::RenderSystem::CommandExecutor::Run(
    CommandTaskFlow& command_task_flow) {
  return Run(std::move(command_task_flow));
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::RunAndWait(
    CommandTaskFlow& command_task_flow) {
  return RunAndWait(std::move(command_task_flow));
}

bool MM::RenderSystem::CommandExecutor::IsValid() const {
  return render_engine_ != nullptr && render_engine_->IsValid() &&
         GetGraphCommandNumber() != 0 && GetComputeCommandNumber() != 0 &&
         GetTransformCommandNumber() != 0;
}

void MM::RenderSystem::CommandExecutor::ClearWhenConstructFailed(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  for (auto& command_pool : graph_command_pools) {
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

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::InitCommandPolls(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  VkCommandPoolCreateInfo command_buffer_create_info =
      Utils::GetCommandPoolCreateInfo(render_engine_->GetGraphQueueIndex());

  for (std::uint32_t i = 0; i < graph_command_pools.size(); ++i) {
    MM_VK_CHECK(
        vkCreateCommandPool(render_engine_->GetDevice(),
                            &command_buffer_create_info, nullptr,
                            &graph_command_pools[i]),
        MM_LOG_ERROR("Failed to create graph command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};)
  }

  command_buffer_create_info.queueFamilyIndex =
      render_engine_->GetComputeQueueIndex();
  for (std::uint32_t i = 0; i < compute_command_pools.size(); ++i) {
    MM_VK_CHECK(
        vkCreateCommandPool(render_engine_->GetDevice(),
                            &command_buffer_create_info, nullptr,
                            &compute_command_pools[i]),
        MM_LOG_ERROR("Failed to create compute command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};)
  }

  command_buffer_create_info.queueFamilyIndex =
      render_engine_->GetTransformQueueIndex();
  for (std::uint32_t i = 0; i < transform_command_pools.size(); ++i) {
    MM_VK_CHECK(
        vkCreateCommandPool(render_engine_->GetDevice(),
                            &command_buffer_create_info, nullptr,
                            &transform_command_pools[i]),
        MM_LOG_ERROR("Failed to create transform command pool.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};)
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::InitCommandBuffers(
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
    MM_VK_CHECK(
        vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                 &command_buffer_allocate_info,
                                 graph_command_buffers.data()),
        MM_LOG_ERROR("Failed to allocate graph command buffers.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};)
  }

  for (std::uint32_t i = 0; i < compute_command_buffers.size(); ++i) {
    command_buffer_allocate_info.commandPool = compute_command_pools[i];
    MM_VK_CHECK(
        vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                 &command_buffer_allocate_info,
                                 compute_command_buffers.data()),
        MM_LOG_ERROR("Failed to allocate compute command buffers.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};)
  }

  for (std::uint32_t i = 0; i < transform_command_buffers.size(); ++i) {
    command_buffer_allocate_info.commandPool = transform_command_pools[i];
    MM_VK_CHECK(
        vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                 &command_buffer_allocate_info,
                                 transform_command_buffers.data()),
        MM_LOG_ERROR("Failed to allocate transform command buffers.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};)
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::InitGeneralAllocatedCommandBuffers(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools,
    std::vector<VkCommandBuffer>& graph_command_buffers,
    std::vector<VkCommandBuffer>& compute_command_buffers,
    std::vector<VkCommandBuffer>& transform_command_buffers) {
  for (std::uint64_t i = 0; i != 3; ++i) {
    general_command_buffers_[0][i] = std::make_unique<AllocatedCommandBuffer>(
        render_engine_, render_engine_->GetGraphQueueIndex(),
        render_engine_->GetGraphQueue(), graph_command_pools.back(),
        graph_command_buffers.back());
    general_command_buffers_[1][i] = std::make_unique<AllocatedCommandBuffer>(
        render_engine_, render_engine_->GetComputeQueueIndex(),
        render_engine_->GetComputeQueue(), compute_command_pools.back(),
        compute_command_buffers.back());

    general_command_buffers_[3][i] = std::make_unique<AllocatedCommandBuffer>(
        render_engine_, render_engine_->GetTransformQueueIndex(),
        render_engine_->GetTransformQueue(), transform_command_pools.back(),
        transform_command_buffers.back());

    graph_command_pools.pop_back();
    graph_command_buffers.pop_back();
    compute_command_pools.pop_back();
    compute_command_buffers.pop_back();
    transform_command_pools.pop_back();
    transform_command_buffers.pop_back();
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::InitAllocatedCommandBuffers(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools,
    std::vector<VkCommandBuffer>& graph_command_buffers,
    std::vector<VkCommandBuffer>& compute_command_buffers,
    std::vector<VkCommandBuffer>& transform_command_buffers) {
  for (std::uint32_t i = 0; i < graph_command_buffers.size(); ++i) {
    free_graph_command_buffers_.push(std::make_unique<AllocatedCommandBuffer>(
        render_engine_, render_engine_->GetGraphQueueIndex(),
        render_engine_->GetGraphQueue(), graph_command_pools[i],
        graph_command_buffers[i]));

    //    if (!free_graph_command_buffers_.top()->IsValid()) {
    //      LOG_ERROR("Failed to create graph allocate command buffer.");
    //      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
    //                               transform_command_pools);
    //      return ExecuteResult::INITIALIZATION_FAILED;
    //    }

    graph_command_pools[i] = nullptr;
  }

  for (std::uint32_t i = 0; i < compute_command_buffers.size(); ++i) {
    free_compute_command_buffers_.push(std::make_unique<AllocatedCommandBuffer>(
        render_engine_, render_engine_->GetComputeQueueIndex(),
        render_engine_->GetComputeQueue(), compute_command_pools[i],
        compute_command_buffers[i]));

    //    if (!free_compute_command_buffers_.top()->IsValid()) {
    //      LOG_ERROR("Failed to create compute allocate command buffer.");
    //      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
    //                               transform_command_pools);
    //      return ExecuteResult::INITIALIZATION_FAILED;
    //    }

    compute_command_pools[i] = nullptr;
  }

  for (std::uint32_t i = 0; i < transform_command_buffers.size(); ++i) {
    free_transform_command_buffers_.push(
        std::make_unique<AllocatedCommandBuffer>(
            render_engine_, render_engine_->GetTransformQueueIndex(),
            render_engine_->GetTransformQueue(), transform_command_pools[i],
            transform_command_buffers[i]));

    //    if (!free_transform_command_buffers_.top()->IsValid()) {
    //      LOG_ERROR("Failed to create transform allocate command buffer.");
    //      ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
    //                               transform_command_pools);
    //      return ExecuteResult::INITIALIZATION_FAILED;
    //    }

    transform_command_pools[i] = nullptr;
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::InitSemaphores(
    const std::uint32_t& need_semaphore_number,
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  const VkSemaphoreCreateInfo semaphore_create_info =
      Utils::GetSemaphoreCreateInfo();
  VkSemaphore new_semaphore{nullptr};
  for (std::uint32_t i = 0;
       i < (need_semaphore_number * need_semaphore_number) / 2; ++i) {
    MM_VK_CHECK(
        vkCreateSemaphore(render_engine_->GetDevice(), &semaphore_create_info,
                          nullptr, &new_semaphore),
        MM_LOG_ERROR("Failed to create VkSemaphore.");
        ClearWhenConstructFailed(graph_command_pools, compute_command_pools,
                                 transform_command_pools);
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};)

    semaphores_.push(new_semaphore);
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::AddCommandBuffer(
    const CommandType& command_type,
    const std::uint32_t& new_command_buffer_num) {
  std::uint32_t command_family_index;
  VkQueue command_queue;
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>*
      free_allocate_command_buffer;

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
      return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
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
  ErrorCode result = ErrorCode::SUCCESS;

  for (std::uint32_t i = 0; i < new_command_buffer_num; ++i) {
    VkCommandPool new_command_pool{nullptr};
    MM_VK_CHECK(vkCreateCommandPool(render_engine_->GetDevice(),
                                    &command_pool_create_info, nullptr,
                                    &new_command_pool),
                result = Utils::VkResultToMMErrorCode(MM_VK_RESULT_CODE);
                execute_state = false;)
    if (!execute_state) {
      for (auto& command_pool : command_pools) {
        vkDestroyCommandPool(render_engine_->GetDevice(), command_pool,
                             nullptr);
      }

      return ResultE<>{result};
    }
  }

  for (std::uint32_t i = 0; i < new_command_buffer_num; ++i) {
    VkCommandBuffer new_command_buffer{nullptr};
    command_buffer_allocate_info.commandPool = command_pools[i];
    MM_VK_CHECK(vkAllocateCommandBuffers(render_engine_->GetDevice(),
                                         &command_buffer_allocate_info,
                                         &new_command_buffer),
                result = Utils::VkResultToMMErrorCode(MM_VK_RESULT_CODE);
                execute_state = false;)
    if (!execute_state) {
      for (auto& command_pool : command_pools) {
        vkDestroyCommandPool(render_engine_->GetDevice(), command_pool,
                             nullptr);
      }

      return ResultE<>{result};
    }
  }

  std::vector<AllocatedCommandBuffer> new_allocated_command_buffers;
  new_allocated_command_buffers.reserve(new_command_buffer_num);
  for (std::uint32_t i = 0; i < new_command_buffer_num; ++i) {
    new_allocated_command_buffers.emplace_back(
        render_engine_, command_family_index, command_queue, command_pools[i],
        command_buffers[i]);

    if (!new_allocated_command_buffers.back().IsValid()) {
      MM_LOG_ERROR("The construction of AllocateCommandBuffer failed.");
      for (auto& command_pool : command_pools) {
        if (command_pool == nullptr) {
          continue;
        }

        vkDestroyCommandPool(render_engine_->GetDevice(), command_pool,
                             nullptr);
      }
      return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
    }

    command_pools[i] = nullptr;
  }

  for (auto& allocated_command_buffer : new_allocated_command_buffers) {
    free_allocate_command_buffer->push(std::make_unique<AllocatedCommandBuffer>(
        std::move(allocated_command_buffer)));
  }

  return ResultS<Nil>{};
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun::
    CommandTaskFlowToBeRun(
        CommandTaskFlow&& command_task_flow, const std::uint32_t& task_flow_ID,
        const std::shared_ptr<Result<Nil, ErrorResult>>& execute_result,
        const std::shared_ptr<CommandCompleteState>& complete_state)
    : command_task_flow_(std::move(command_task_flow)),
      task_flow_ID_(task_flow_ID),
      execute_result_(execute_result),
      complete_state_(complete_state),
      the_maximum_number_of_graph_buffers_required_for_one_task_(0),
      the_maximum_number_of_compute_buffers_required_for_one_task_(0),
      the_maximum_number_of_transform_buffers_required_for_one_task_(0) {
  for (const auto& task : command_task_flow_.tasks_) {
    std::uint32_t require_command_number =
        task->HaveSubTasks()
            ? task->GetRequireCommandBufferNumberIncludeSuBTasks()
            : task->GetRequireCommandBufferNumber();
    if (task->command_type_ == CommandType::GRAPH) {
      if (require_command_number >
          the_maximum_number_of_graph_buffers_required_for_one_task_) {
        the_maximum_number_of_graph_buffers_required_for_one_task_ =
            require_command_number;
      }
    } else if (task->command_type_ == CommandType::COMPUTE) {
      if (require_command_number >
          the_maximum_number_of_compute_buffers_required_for_one_task_) {
        the_maximum_number_of_compute_buffers_required_for_one_task_ =
            require_command_number;
      }
    } else if (task->command_type_ == CommandType::TRANSFORM) {
      if (require_command_number >
          the_maximum_number_of_transform_buffers_required_for_one_task_) {
        the_maximum_number_of_transform_buffers_required_for_one_task_ =
            require_command_number;
      }
    }
  }
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun::
    CommandTaskFlowToBeRun(CommandTaskFlowToBeRun&& other) noexcept
    : command_task_flow_(std::move(other.command_task_flow_)),
      task_flow_ID_(other.task_flow_ID_),
      execute_result_(std::move(other.execute_result_)),
      complete_state_(std::move(other.complete_state_)),
      the_maximum_number_of_graph_buffers_required_for_one_task_(
          other.the_maximum_number_of_graph_buffers_required_for_one_task_),
      the_maximum_number_of_compute_buffers_required_for_one_task_(
          other.the_maximum_number_of_compute_buffers_required_for_one_task_),
      the_maximum_number_of_transform_buffers_required_for_one_task_(
          other
              .the_maximum_number_of_transform_buffers_required_for_one_task_) {
  other.task_flow_ID_ = 0;
  other.the_maximum_number_of_graph_buffers_required_for_one_task_ = 0;
  other.the_maximum_number_of_compute_buffers_required_for_one_task_ = 0;
  other.the_maximum_number_of_transform_buffers_required_for_one_task_ = 0;
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun&
MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun::operator=(
    CommandTaskFlowToBeRun&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  command_task_flow_ = std::move(other.command_task_flow_);
  task_flow_ID_ = other.task_flow_ID_;
  execute_result_ = std::move(other.execute_result_);
  complete_state_ = std::move(other.complete_state_);
  the_maximum_number_of_graph_buffers_required_for_one_task_ =
      other.the_maximum_number_of_graph_buffers_required_for_one_task_;
  the_maximum_number_of_compute_buffers_required_for_one_task_ =
      other.the_maximum_number_of_compute_buffers_required_for_one_task_;
  the_maximum_number_of_transform_buffers_required_for_one_task_ =
      other.the_maximum_number_of_transform_buffers_required_for_one_task_;

  other.task_flow_ID_ = 0;
  other.the_maximum_number_of_graph_buffers_required_for_one_task_ = 0;
  other.the_maximum_number_of_compute_buffers_required_for_one_task_ = 0;
  other.the_maximum_number_of_transform_buffers_required_for_one_task_ = 0;

  return *this;
}

MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow::
    ExecutingCommandTaskFlow(
        CommandTaskFlowToBeRun&& command_task_flow_to_be_run)
    : command_task_flow_(
          std::move(command_task_flow_to_be_run.command_task_flow_)),
      task_flow_ID_(command_task_flow_to_be_run.task_flow_ID_),
      initialize_or_not_(false),
      have_wait_one_task_(false),
      execute_result_(std::move(command_task_flow_to_be_run.execute_result_)),
      complete_state_(std::move(command_task_flow_to_be_run.complete_state_)),
      task_that_have_already_been_accessed_(),
      submitted_task_(),
      can_be_submitted_tasks_(),
      pre_task_not_submit_task_() {}

MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow::
    ExecutingCommandTaskFlow(ExecutingCommandTaskFlow&& other) noexcept
    : command_task_flow_(std::move(other.command_task_flow_)),
      task_flow_ID_(other.task_flow_ID_),
      initialize_or_not_(other.initialize_or_not_),
      have_wait_one_task_(other.initialize_or_not_),
      execute_result_(std::move(other.execute_result_)),
      complete_state_(std::move(other.complete_state_)),
      task_that_have_already_been_accessed_(
          std::move(other.task_that_have_already_been_accessed_)),
      can_be_submitted_tasks_(std::move(other.can_be_submitted_tasks_)),
      pre_task_not_submit_task_(std::move(other.pre_task_not_submit_task_)) {
  std::unique_lock<std::mutex> guard(other.submitted_task_mutex_);
  submitted_task_ = std::move(other.submitted_task_);

  task_flow_ID_ = 0;
}

MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow&
MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow::operator=(
    ExecutingCommandTaskFlow&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  std::lock(submitted_task_mutex_, other.submitted_task_mutex_);
  std::lock_guard<std::mutex> guard1(submitted_task_mutex_, std::adopt_lock);
  std::lock_guard<std::mutex> guard2(other.submitted_task_mutex_,
                                     std::adopt_lock);

  command_task_flow_ = std::move(other.command_task_flow_);
  task_flow_ID_ = other.task_flow_ID_;
  initialize_or_not_ = other.initialize_or_not_;
  have_wait_one_task_ = other.have_wait_one_task_;
  execute_result_ = std::move(other.execute_result_);
  complete_state_ = std::move(other.complete_state_);
  task_that_have_already_been_accessed_ =
      std::move(other.task_that_have_already_been_accessed_);
  submitted_task_ = std::move(other.submitted_task_);
  can_be_submitted_tasks_ = std::move(other.can_be_submitted_tasks_);
  pre_task_not_submit_task_ = std::move(other.pre_task_not_submit_task_);

  other.task_flow_ID_ = 0;

  return *this;
}
std::uint32_t MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow::
    GetRequireGraphCommandBufferNumber() const {
  std::uint32_t result = 0;
  for (const auto& task : command_task_flow_.tasks_) {
    if (task->command_type_ == CommandType::GRAPH) {
      ++result;
    }
  }

  return result;
}
std::uint32_t MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow::
    GetRequireComputeCommandBufferNumber() const {
  std::uint32_t result = 0;
  for (const auto& task : command_task_flow_.tasks_) {
    if (task->command_type_ == CommandType::COMPUTE) {
      ++result;
    }
  }

  return result;
}
std::uint32_t MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow::
    GetRequireTransformCommandBufferNumber() const {
  std::uint32_t result = 0;
  for (const auto& task : command_task_flow_.tasks_) {
    if (task->command_type_ == CommandType::TRANSFORM) {
      ++result;
    }
  }

  return result;
}
std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> MM::RenderSystem::
    CommandExecutor::ExecutingCommandTaskFlow::GetRequireCommandBufferNumber()
        const {
  std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> result{0, 0, 0};
  for (const auto& task : command_task_flow_.tasks_) {
    if (task->command_type_ == CommandType::GRAPH) {
      ++std::get<0>(result);
    } else if (task->command_type_ == CommandType::COMPUTE) {
      ++std::get<1>(result);
    } else if (task->command_type_ == CommandType::TRANSFORM) {
      ++std::get<2>(result);
    }
  }

  return result;
}

void MM::RenderSystem::CommandExecutor::UpdateCommandTaskLine(
    ExecutingCommandTaskFlow& command_task_flow,
    std::unique_ptr<CommandTask>& new_command_task,
    std::unique_ptr<CommandTask>* old_command_task_ptr) {
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

  if (command_task_flow.task_that_have_already_been_accessed_.erase(
          old_command_task_ptr)) {
    command_task_flow.task_that_have_already_been_accessed_.insert(
        &new_command_task);
  }

  std::unique_lock<std::mutex> guard(command_task_flow.submitted_task_mutex_);
  if (command_task_flow.submitted_task_.erase(old_command_task_ptr)) {
    command_task_flow.submitted_task_.insert(&new_command_task);
  }
}

MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::CommandTaskToBeSubmit(
    CommandExecutor* command_executor,
    const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    std::unique_ptr<CommandTask>&& command_task,
    const std::vector<VkSemaphore>& default_wait_semaphore,
    const std::vector<VkSemaphore>& default_signal_semaphore)
    : command_executor_(command_executor),
      task_flow_ID_(command_task_flow->task_flow_ID_),
      command_task_flow_(command_task_flow),
      command_task_(std::move(command_task)),
      default_wait_semaphore_(),
      default_signal_semaphore_(),
      post_task_sub_task_numbers_(),
      wait_coefficient_(0) {
  assert(!command_task_flow_.expired());
  const auto vector_number =
      static_cast<std::uint32_t>(1 + command_task_->sub_tasks_.size());
  const auto one_wait_vector_semaphore_number =
      static_cast<std::uint32_t>(default_wait_semaphore.size() / vector_number);
  const auto one_signal_vector_semaphore_number = static_cast<std::uint32_t>(
      default_signal_semaphore.size() / vector_number);
  default_wait_semaphore_.reserve(vector_number);
  default_signal_semaphore_.reserve(vector_number);
  for (std::uint32_t i = 0; i < vector_number; ++i) {
    default_wait_semaphore_.emplace_back(one_wait_vector_semaphore_number);
    default_signal_semaphore_.emplace_back(one_signal_vector_semaphore_number);
    auto& wait_back = default_wait_semaphore_.back();
    auto& signal_back = default_signal_semaphore_.back();
    for (std::uint32_t j = 0; j < one_wait_vector_semaphore_number; ++j) {
      wait_back[j] =
          default_wait_semaphore[i * one_wait_vector_semaphore_number + j];
    }
    for (std::uint32_t j = 0; j < one_signal_vector_semaphore_number; ++j) {
      signal_back[j] =
          default_signal_semaphore[i * one_signal_vector_semaphore_number + j];
    }
  }

  post_task_sub_task_numbers_.reserve(command_task_->post_tasks_.size());
  for (const auto& post_task : command_task_->post_tasks_) {
    post_task_sub_task_numbers_.emplace_back((*post_task)->sub_tasks_.size());
  }

  if (!command_task_flow_.expired()) {
    UpdateCommandTaskLine(*(command_task_flow_.lock()), command_task_,
                          &command_task);
  }
}

MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::CommandTaskToBeSubmit(
    CommandTaskToBeSubmit&& other) noexcept
    : command_executor_(other.command_executor_),
      task_flow_ID_(other.task_flow_ID_),
      command_task_flow_(std::move(other.command_task_flow_)),
      command_task_(std::move(other.command_task_)),
      default_wait_semaphore_(std::move(other.default_wait_semaphore_)),
      default_signal_semaphore_(std::move(other.default_signal_semaphore_)),
      post_task_sub_task_numbers_(std::move(other.post_task_sub_task_numbers_)),
      wait_coefficient_(other.wait_coefficient_) {
  if (!command_task_flow_.expired()) {
    UpdateCommandTaskLine(*(command_task_flow_.lock()), command_task_,
                          &(other.command_task_));
  }

  other.command_executor_ = nullptr;
}

MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit&
MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::operator=(
    CommandTaskToBeSubmit&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  command_executor_ = other.command_executor_;
  task_flow_ID_ = other.task_flow_ID_;
  command_task_flow_ = std::move(other.command_task_flow_);
  command_task_ = std::move(other.command_task_);
  default_wait_semaphore_ = std::move(other.default_wait_semaphore_);
  default_signal_semaphore_ = std::move(other.default_signal_semaphore_);
  post_task_sub_task_numbers_ = std::move(other.post_task_sub_task_numbers_);
  wait_coefficient_ = other.wait_coefficient_;
  if (!command_task_flow_.expired()) {
    UpdateCommandTaskLine(*(command_task_flow_.lock()), command_task_,
                          &(other.command_task_));
  }

  other.command_executor_ = nullptr;

  return *this;
}

bool MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::operator<(
    const CommandTaskToBeSubmit& other) const {
  return command_task_ < other.command_task_;
}

MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit::CommandTaskToBeSubmit(
    CommandExecutor* command_executor,
    const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    std::unique_ptr<CommandTask>&& command_task,
    const std::vector<VkSemaphore>& default_wait_semaphore,
    const std::vector<VkSemaphore>& default_signal_semaphore,
    std::vector<std::uint32_t>&& post_task_sub_task_numbers)
    : command_executor_(command_executor),
      task_flow_ID_(command_task_flow->task_flow_ID_),
      command_task_flow_(command_task_flow),
      command_task_(std::move(command_task)),
      default_wait_semaphore_(),
      default_signal_semaphore_(),
      post_task_sub_task_numbers_(std::move(post_task_sub_task_numbers)),
      wait_coefficient_(0) {
  assert(!command_task_flow_.expired());
  const auto vector_number =
      static_cast<std::uint32_t>(1 + command_task_->sub_tasks_.size());
  const auto one_wait_vector_semaphore_number =
      static_cast<std::uint32_t>(default_wait_semaphore.size() / vector_number);
  const auto one_signal_vector_semaphore_number = static_cast<std::uint32_t>(
      default_signal_semaphore.size() / vector_number);
  default_wait_semaphore_.reserve(vector_number);
  default_signal_semaphore_.reserve(vector_number);
  for (std::uint32_t i = 0; i < vector_number; ++i) {
    default_wait_semaphore_.emplace_back(one_wait_vector_semaphore_number);
    default_signal_semaphore_.emplace_back(one_signal_vector_semaphore_number);
    auto& wait_back = default_wait_semaphore_.back();
    auto& signal_back = default_signal_semaphore_.back();
    for (std::uint32_t j = 0; j < one_wait_vector_semaphore_number; ++j) {
      wait_back[j] =
          default_wait_semaphore[i * one_wait_vector_semaphore_number + j];
    }
    for (std::uint32_t j = 0; j < one_signal_vector_semaphore_number; ++j) {
      signal_back[j] =
          default_signal_semaphore[i * one_signal_vector_semaphore_number + j];
    }
  }

  if (!command_task_flow_.expired()) {
    UpdateCommandTaskLine(*(command_task_flow_.lock()), command_task_,
                          &command_task);
  }
}

MM::RenderSystem::CommandExecutor::ExecutingTask::ExecutingTask(
    RenderEngine* engine, CommandExecutor* command_executor,
    const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    std::vector<std::unique_ptr<AllocatedCommandBuffer>>&& command_buffer,
    std::unique_ptr<CommandTask>&& command_task,
    const std::weak_ptr<Result<Nil, ErrorResult>>& execute_result,
    const std::weak_ptr<CommandCompleteState>& complete_state,
    std::vector<std::vector<VkSemaphore>>&& default_wait_semaphore,
    std::vector<std::vector<VkSemaphore>>&& default_signal_semaphore)
    : render_engine_(engine),
      command_executor_(command_executor),
      task_flow_ID_(command_task_flow->task_flow_ID_),
      command_task_flow_(command_task_flow),
      command_buffers_(std::move(command_buffer)),
      command_task_(std::move(command_task)),
      complete_state_(complete_state),
      wait_semaphore_(std::move(default_wait_semaphore)),
      wait_semaphore_stages_(wait_semaphore_.size()),
      signal_semaphore_(std::move(default_signal_semaphore)),
      default_wait_semaphore_number_(wait_semaphore_[0].size()),
      default_signal_semaphore_number_(signal_semaphore_[0].size()) {
  assert(!command_task_flow_.expired());
  for (std::uint32_t i = 0; i != wait_semaphore_.size(); ++i) {
    if (i == 0) {
      wait_semaphore_[0].reserve(wait_semaphore_[0].size() +
                                 command_task_->wait_semaphore_.size());
      wait_semaphore_stages_[0].reserve(wait_semaphore_[0].capacity());
      signal_semaphore_[0].reserve(signal_semaphore_[0].size() +
                                   command_task_->signal_semaphore_.size());

      std::fill_n(wait_semaphore_stages_[0].begin(), wait_semaphore_[0].size(),
                  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

      for (const auto& external_wait_semaphore :
           command_task_->wait_semaphore_) {
        wait_semaphore_[0].emplace_back(
            external_wait_semaphore.wait_semaphore_.GetSemaphore());
        wait_semaphore_stages_[0].emplace_back(
            external_wait_semaphore.wait_stage_);
      }

      for (const auto& external_signal_semaphore :
           command_task_->signal_semaphore_) {
        signal_semaphore_[0].emplace_back(
            external_signal_semaphore.GetSemaphore());
      }
      continue;
    }

    wait_semaphore_[i].reserve(
        wait_semaphore_[i].size() +
        command_task_->sub_tasks_[i - 1]->wait_semaphore_.size());
    wait_semaphore_stages_[i].reserve(wait_semaphore_[i].capacity());
    signal_semaphore_[i].reserve(
        signal_semaphore_[i].size() +
        command_task_->sub_tasks_[i - 1]->signal_semaphore_.size());

    std::fill_n(wait_semaphore_stages_[i].begin(), wait_semaphore_.size(),
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    for (const auto& external_wait_semaphore :
         command_task_->sub_tasks_[i - 1]->wait_semaphore_) {
      wait_semaphore_[i].emplace_back(
          external_wait_semaphore.wait_semaphore_.GetSemaphore());
      wait_semaphore_stages_[i].emplace_back(
          external_wait_semaphore.wait_stage_);
    }

    for (const auto& external_signal_semaphore :
         command_task_->sub_tasks_[i - 1]->signal_semaphore_) {
      signal_semaphore_[i].emplace_back(
          external_signal_semaphore.GetSemaphore());
    }
  }

  if (!command_task_flow_.expired()) {
    UpdateCommandTaskLine(*(command_task_flow_.lock()), command_task_,
                          &command_task);
  }
}

MM::RenderSystem::CommandExecutor::ExecutingTask::ExecutingTask(
    ExecutingTask&& other) noexcept
    : render_engine_(other.render_engine_),
      command_executor_(other.command_executor_),
      task_flow_ID_(other.task_flow_ID_),
      command_task_flow_(std::move(other.command_task_flow_)),
      command_buffers_(std::move(other.command_buffers_)),
      command_task_(std::move(other.command_task_)),
      complete_state_(std::move(other.complete_state_)),
      wait_semaphore_(std::move(other.wait_semaphore_)),
      wait_semaphore_stages_(std::move(other.wait_semaphore_stages_)),
      signal_semaphore_(std::move(other.signal_semaphore_)),
      default_wait_semaphore_number_(other.default_wait_semaphore_number_),
      default_signal_semaphore_number_(other.default_signal_semaphore_number_) {
  if (!command_task_flow_.expired()) {
    UpdateCommandTaskLine(*(command_task_flow_.lock()), command_task_,
                          &(other.command_task_));
  }

  other.render_engine_ = nullptr;
  other.command_executor_ = nullptr;
  other.default_wait_semaphore_number_ = 0;
  other.default_signal_semaphore_number_ = 0;
}

MM::RenderSystem::CommandExecutor::ExecutingTask&
MM::RenderSystem::CommandExecutor::ExecutingTask::operator=(
    ExecutingTask&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  render_engine_ = other.render_engine_;
  command_executor_ = other.command_executor_;
  task_flow_ID_ = other.task_flow_ID_;
  command_task_flow_ = other.command_task_flow_;
  command_buffers_ = std::move(other.command_buffers_);
  command_task_ = std::move(other.command_task_);
  complete_state_ = std::move(other.complete_state_);
  wait_semaphore_ = std::move(other.wait_semaphore_);
  wait_semaphore_stages_ = std::move(other.wait_semaphore_stages_);
  signal_semaphore_ = std::move(other.signal_semaphore_);
  default_wait_semaphore_number_ = other.default_wait_semaphore_number_;
  default_signal_semaphore_number_ = other.default_signal_semaphore_number_;

  if (!command_task_flow_.expired()) {
    UpdateCommandTaskLine(*(command_task_flow_.lock()), command_task_,
                          &(other.command_task_));
  }

  other.render_engine_ = nullptr;
  other.command_executor_ = nullptr;
  other.default_wait_semaphore_number_ = 0;
  other.default_signal_semaphore_number_ = 0;

  return *this;
}

bool MM::RenderSystem::CommandExecutor::ExecutingTask::IsComplete() const {
  MM_VK_CHECK(vkGetFenceStatus(render_engine_->GetDevice(),
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

void MM::RenderSystem::CommandExecutor::RecycledSemaphoreThatSubmittedFailed() {
  while (valid_) {
    std::lock(submit_failed_to_be_recycled_semaphore_mutex_,
              recycled_semaphore_mutex_);
    std::lock_guard<std::mutex> guard1(
        submit_failed_to_be_recycled_semaphore_mutex_, std::adopt_lock);
    {
      std::lock_guard<std::mutex> guard2{recycled_semaphore_mutex_,
                                         std::adopt_lock};
      for (const auto& semaphore : submit_failed_to_be_recycled_semaphore_.at(
               submit_failed_to_be_recycled_semaphore_current_index_)) {
        recycled_semaphore_.emplace_back(semaphore);
      }
    }
    submit_failed_to_be_recycled_semaphore_
        .at(submit_failed_to_be_recycled_semaphore_current_index_)
        .clear();
    submit_failed_to_be_recycled_semaphore_current_index_ =
        (submit_failed_to_be_recycled_semaphore_current_index_ + 1) % 2;

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void MM::RenderSystem::CommandExecutor::ProcessCompleteTask() {
  std::unique_lock<std::mutex> recycled_semaphore_guard{
      recycled_semaphore_mutex_};
  for (const auto& semaphore : recycled_semaphore_) {
    semaphores_.push(semaphore);
  }
  recycled_semaphore_.clear();
  recycled_semaphore_guard.unlock();

  std::unique_lock<std::mutex> recycled_command_buffer_guard{
      submit_failed_to_be_recycled_command_buffer_mutex_};
  std::uint32_t graph_buffer_number = 0;
  std::uint32_t compute_buffer_number = 0;
  std::uint32_t transform_buffer_number = 0;
  for (auto& command_buffer : submit_failed_to_be_recycled_command_buffer_) {
    command_buffer->ResetCommandBuffer();
    if (command_buffer->GetCommandBufferType() == CommandBufferType::GRAPH) {
      free_graph_command_buffers_.push(std::move(command_buffer));
      ++graph_buffer_number;
    } else if (command_buffer->GetCommandBufferType() ==
               CommandBufferType::COMPUTE) {
      free_compute_command_buffers_.push(std::move(command_buffer));
      ++compute_buffer_number;
    } else if (command_buffer->GetCommandBufferType() ==
               CommandBufferType::TRANSFORM) {
      free_transform_command_buffers_.push(std::move(command_buffer));
      ++transform_buffer_number;
    }
  }
  recoding_graph_command_buffer_number_ -= graph_buffer_number;
  recording_compute_command_buffer_number_ -= compute_buffer_number;
  recording_transform_command_buffer_number_ -= transform_buffer_number;
  submit_failed_to_be_recycled_command_buffer_.clear();
  recycled_command_buffer_guard.unlock();

  {
    std::lock_guard<std::mutex> guard{executing_graph_command_buffers_mutex_};
    for (auto executing_command_buffer =
             executing_graph_command_buffers_.begin();
         executing_command_buffer != executing_graph_command_buffers_.end();) {
      if (executing_command_buffer->IsComplete()) {
        if (auto complete_state =
                executing_command_buffer->complete_state_.lock()) {
          if (complete_state->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            RenderFuture::wait_condition_variable_.notify_all();
          }
        }

        for (const auto& wait_semaphore_vector :
             executing_command_buffer->wait_semaphore_) {
          for (std::uint32_t i = 0;
               i < executing_command_buffer->default_wait_semaphore_number_;
               ++i) {
            semaphores_.push(wait_semaphore_vector[i]);
          }
        }

        for (auto& complete_buffer :
             executing_command_buffer->command_buffers_) {
          complete_buffer->ResetCommandBuffer();
          complete_buffer->ResetFence();
          free_graph_command_buffers_.push(std::move(complete_buffer));
        }

        {
          std::lock_guard<std::mutex> command_task_render_resource_states_guard{
              command_task_render_resource_states_mutex_};
          for (const RenderResourceDataID& render_resource_ID :
               executing_command_buffer->command_task_
                   ->cross_task_flow_sync_render_resource_IDs_) {
            auto& states =
                command_task_render_resource_states_.find(render_resource_ID)
                    ->second;
            for (auto iter = states.begin(); iter != states.end();) {
              if ((*iter).task_flow_ID_ ==
                      executing_command_buffer->task_flow_ID_ &&
                  (*iter).task_ID_ == executing_command_buffer->command_task_
                                          ->command_task_ID_) {
                iter = states.erase(iter);
                continue;
              }

              ++iter;
            }

            if (states.empty()) {
              command_task_render_resource_states_.erase(
                  command_task_render_resource_states_.find(
                      render_resource_ID));
            }
          }
        }

        executing_command_buffer =
            executing_graph_command_buffers_.erase(executing_command_buffer);
        continue;
      }
      ++executing_command_buffer;
    }
  }

  {
    std::lock_guard<std::mutex> guard{executing_compute_command_buffers_mutex_};
    for (auto executing_command_buffer =
             executing_compute_command_buffers_.begin();
         executing_command_buffer !=
         executing_compute_command_buffers_.end();) {
      if (executing_command_buffer->IsComplete()) {
        if (auto complete_state =
                executing_command_buffer->complete_state_.lock()) {
          if (complete_state->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            RenderFuture::wait_condition_variable_.notify_all();
          }
        }
        for (const auto& wait_semaphore_vector :
             executing_command_buffer->wait_semaphore_) {
          for (std::uint32_t i = 0;
               i < executing_command_buffer->default_wait_semaphore_number_;
               ++i) {
            semaphores_.push(wait_semaphore_vector[i]);
          }
        }

        for (auto& complete_buffer :
             executing_command_buffer->command_buffers_) {
          complete_buffer->ResetCommandBuffer();
          complete_buffer->ResetFence();
          free_compute_command_buffers_.push(std::move(complete_buffer));
        }

        {
          std::lock_guard<std::mutex> command_task_render_resource_states_guard{
              command_task_render_resource_states_mutex_};
          for (const RenderResourceDataID& render_resource_ID :
               executing_command_buffer->command_task_
                   ->cross_task_flow_sync_render_resource_IDs_) {
            auto& states =
                command_task_render_resource_states_.find(render_resource_ID)
                    ->second;
            for (auto iter = states.begin(); iter != states.end();) {
              if ((*iter).task_flow_ID_ ==
                      executing_command_buffer->task_flow_ID_ &&
                  (*iter).task_ID_ == executing_command_buffer->command_task_
                                          ->command_task_ID_) {
                iter = states.erase(iter);
                continue;
              }

              ++iter;
            }

            if (states.empty()) {
              command_task_render_resource_states_.erase(
                  command_task_render_resource_states_.find(
                      render_resource_ID));
            }
          }
        }

        executing_command_buffer =
            executing_compute_command_buffers_.erase(executing_command_buffer);
        continue;
      }
      ++executing_command_buffer;
    }
  }

  {
    std::lock_guard<std::mutex> guard{
        executing_transform_command_buffers_mutex_};
    for (auto executing_command_buffer =
             executing_transform_command_buffers_.begin();
         executing_command_buffer !=
         executing_transform_command_buffers_.end();) {
      if (executing_command_buffer->IsComplete()) {
        if (auto complete_state =
                executing_command_buffer->complete_state_.lock()) {
          if (complete_state->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            RenderFuture::wait_condition_variable_.notify_all();
          }
        }
        for (const auto& wait_semaphore_vector :
             executing_command_buffer->wait_semaphore_) {
          for (std::uint32_t i = 0;
               i < executing_command_buffer->default_wait_semaphore_number_;
               ++i) {
            semaphores_.push(wait_semaphore_vector[i]);
          }
        }

        for (auto& complete_buffer :
             executing_command_buffer->command_buffers_) {
          complete_buffer->ResetCommandBuffer();
          complete_buffer->ResetFence();
          free_transform_command_buffers_.push(std::move(complete_buffer));
        }

        {
          std::lock_guard<std::mutex> command_task_render_resource_states_guard{
              command_task_render_resource_states_mutex_};
          for (const RenderResourceDataID& render_resource_ID :
               executing_command_buffer->command_task_
                   ->cross_task_flow_sync_render_resource_IDs_) {
            auto& states =
                command_task_render_resource_states_.find(render_resource_ID)
                    ->second;
            for (auto iter = states.begin(); iter != states.end();) {
              if ((*iter).task_flow_ID_ ==
                      executing_command_buffer->task_flow_ID_ &&
                  (*iter).task_ID_ == executing_command_buffer->command_task_
                                          ->command_task_ID_) {
                iter = states.erase(iter);
                continue;
              }

              ++iter;
            }

            if (states.empty()) {
              command_task_render_resource_states_.erase(
                  command_task_render_resource_states_.find(
                      render_resource_ID));
            }
          }
        }

        executing_command_buffer = executing_transform_command_buffers_.erase(
            executing_command_buffer);
        continue;
      }
      ++executing_command_buffer;
    }
  }
}

void MM::RenderSystem::CommandExecutor::ProcessWaitTask() {
  // This function cannot obtain the \ref task_flow_queue_mutex_ lock
  // as it will cause a deadlock.
  std::lock(wait_tasks_mutex_, task_flow_queue_mutex_);
  std::lock_guard<std::mutex> guard1(wait_tasks_mutex_, std::adopt_lock);
  std::lock_guard<std::mutex> guard2(task_flow_queue_mutex_, std::adopt_lock);
  if (!wait_tasks_.empty()) {
    for (const auto& wait_task : wait_tasks_) {
      for (auto iter = task_flow_queue_.begin();
           iter != task_flow_queue_.end();) {
        if (wait_task == iter->task_flow_ID_) {
          executing_command_task_flows_.emplace_front(
              std::make_shared<ExecutingCommandTaskFlow>(std::move(*iter)));
          iter = task_flow_queue_.erase(iter);
          break;
        }
        ++iter;
      }
    }
  }
}

void MM::RenderSystem::CommandExecutor::ProcessRootTaskAndSubTask(
    const std::shared_ptr<
        MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow>&
        command_task_flow) {
  for (auto& root_task : command_task_flow->command_task_flow_.root_tasks_) {
    for (auto iter = command_task_flow->command_task_flow_.tasks_.begin();
         iter != command_task_flow->command_task_flow_.tasks_.end();) {
      if (&(*iter) == root_task) {
        command_task_flow->task_that_have_already_been_accessed_.emplace(
            root_task);  // deletable
        std::vector<std::uint32_t> post_task_sub_task_numbers;
        post_task_sub_task_numbers.reserve((*iter)->post_tasks_.size());
        std::uint32_t all_post_task_sub_task_number = 0;
        for (const auto& post_task : (*iter)->post_tasks_) {
          post_task_sub_task_numbers.emplace_back(
              (*post_task)->sub_tasks_.size());
          all_post_task_sub_task_number += (*post_task)->sub_tasks_.size();
        }
        command_task_flow->can_be_submitted_tasks_.emplace_back(
            this, command_task_flow, std::move(*iter),
            std::vector<VkSemaphore>{},
            GetSemaphore(
                all_post_task_sub_task_number *
                static_cast<std::uint32_t>(1 + (*iter)->sub_tasks_.size())),
            std::move(post_task_sub_task_numbers));  // TODO why add 1?
        iter = command_task_flow->command_task_flow_.tasks_.erase(iter);
        continue;
      }
      ++iter;
    }
    CommandTaskToBeSubmit& to_be_submit_task =
        command_task_flow->can_be_submitted_tasks_.back();
    ProcessPreTaskNoSubmitTask(command_task_flow, to_be_submit_task);
  }
}

void MM::RenderSystem::CommandExecutor::ChooseVariableByCommandType(
    CommandType command_type, std::uint32_t*& free_command_buffer_number,
    std::stack<std::unique_ptr<AllocatedCommandBuffer>>*& free_command_buffers,
    std::list<ExecutingTask>*& executing_task_list,
    std::atomic_uint32_t*& recording_command_buffer_number,
    std::uint32_t free_graph_command_buffer_number,
    std::uint32_t free_compute_command_buffer_number,
    std::uint32_t free_transform_command_buffer_number) {
  switch (command_type) {
    case CommandBufferType::GRAPH:
      free_command_buffer_number = &free_graph_command_buffer_number;
      free_command_buffers = &free_graph_command_buffers_;
      executing_task_list = &executing_graph_command_buffers_;
      recording_command_buffer_number = &recoding_graph_command_buffer_number_;
      break;
    case CommandBufferType::COMPUTE:
      free_command_buffer_number = &free_compute_command_buffer_number;
      free_command_buffers = &free_compute_command_buffers_;
      executing_task_list = &executing_compute_command_buffers_;
      recording_command_buffer_number =
          &recording_compute_command_buffer_number_;
      break;
    case CommandBufferType::TRANSFORM:
      free_command_buffer_number = &free_transform_command_buffer_number;
      free_command_buffers = &free_transform_command_buffers_;
      executing_task_list = &executing_transform_command_buffers_;
      recording_command_buffer_number =
          &recording_transform_command_buffer_number_;
      break;
    default:
      free_command_buffer_number = &free_graph_command_buffer_number;
      free_command_buffers = &free_graph_command_buffers_;
      executing_task_list = &executing_graph_command_buffers_;
      recording_command_buffer_number = &recoding_graph_command_buffer_number_;
  }
}

void MM::RenderSystem::CommandExecutor::ProcessOneCanSubmitTask(
    std::list<CommandTaskToBeSubmit>::iterator& command_task_to_be_submit,
    std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    std::uint32_t& number_of_blocked_cross_task_flow) {
  std::uint32_t* free_command_buffer_number{nullptr};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>* free_command_buffers;
  std::list<ExecutingTask>* submit_group;
  std::atomic_uint32_t* recording_command_buffer_number{nullptr};

  std::uint32_t free_graph_command_buffer_number = GetFreeGraphCommandNumber();
  std::uint32_t free_compute_command_buffer_number =
      GetFreeComputeCommandNumber();
  std::uint32_t free_transform_command_buffer_number =
      GetFreeTransformCommandNumber();

  ChooseVariableByCommandType(
      command_task_to_be_submit->command_task_->GetCommandType(),
      free_command_buffer_number, free_command_buffers, submit_group,
      recording_command_buffer_number, free_graph_command_buffer_number,
      free_compute_command_buffer_number, free_transform_command_buffer_number);

  const std::uint32_t require_command_buffer_numbers =
      command_task_to_be_submit->command_task_->HaveSubTasks()
          ? command_task_to_be_submit->require_command_buffer_include_sub_task_
          : command_task_to_be_submit->require_command_buffer_;

  bool command_submit_condition = false;
  if (require_command_buffer_numbers <= *free_command_buffer_number) {
    ProcessCrossTaskFlowRenderResourceSync(
        command_task_to_be_submit, command_task_flow, command_submit_condition);
  } else {
    if (require_command_buffer_numbers > 3) {
      command_task_to_be_submit->wait_coefficient_ +=
          *free_command_buffer_number;
      if (command_task_to_be_submit->wait_coefficient_ >
          3 * require_command_buffer_numbers) {
        while (true) {
          ProcessCompleteTask();
          *free_command_buffer_number = GetFreeCommandNumber(
              command_task_to_be_submit->command_task_->command_type_);
          if (require_command_buffer_numbers <= *free_command_buffer_number) {
            if (!command_submit_condition) {
              ProcessCrossTaskFlowRenderResourceSync(command_task_to_be_submit,
                                                     command_task_flow,
                                                     command_submit_condition);
            }
            if (command_submit_condition) {
              break;
            }
          }

          std::this_thread::sleep_for(std::chrono::nanoseconds(5000));
        }
      }
    }
  }

  if (command_submit_condition) {
    std::vector<std::unique_ptr<AllocatedCommandBuffer>>
        acquired_command_buffer;
    acquired_command_buffer.reserve(require_command_buffer_numbers);
    for (std::uint32_t i = 0; i < require_command_buffer_numbers; ++i) {
      acquired_command_buffer.emplace_back(
          free_command_buffers->top().release());
      free_command_buffers->pop();
      ++(*recording_command_buffer_number);
    }

    std::unique_ptr<ExecutingTask> temp_execute_task =
        std::make_unique<ExecutingTask>(
            render_engine_, this, command_task_flow,
            std::move(acquired_command_buffer),
            std::move(command_task_to_be_submit->command_task_),
            command_task_flow->execute_result_,
            command_task_flow->complete_state_,
            std::move(command_task_to_be_submit->default_wait_semaphore_),
            std::move(command_task_to_be_submit->default_signal_semaphore_));

    // *free_command_buffer_number -= require_command_buffer_numbers;

    command_task_to_be_submit =
        command_task_flow->can_be_submitted_tasks_.erase(
            command_task_to_be_submit);

    // TODO  Attempt to directly call the \ref SubmitTasksSync method on tasks
    // that use synchronous submission instead of submitting to the task system.
    SubmitTasks(command_task_flow, std::move(temp_execute_task));
    //    TaskSystem::Taskflow task_flow;
    //    task_flow.emplace([command_task_flow = command_task_flow,
    //                       input_tasks = temp_execute_task.release(),
    //                       this_object = this]() mutable {
    //      this_object->SubmitTasks(command_task_flow,
    //                               std::unique_ptr<ExecutingTask>(input_tasks));
    //    });
    //    TASK_SYSTEM->Run(TaskSystem::TaskType::Render, task_flow);
  } else {
    //    ProcessCompleteTask();
    //    switch (command_task_to_be_submit->command_task_->GetCommandType())
    //    {
    //      case CommandType::GRAPH:
    //        *free_command_buffer_number = GetFreeGraphCommandNumber();
    //        break;
    //      case CommandType::COMPUTE:
    //        *free_command_buffer_number = GetFreeComputeCommandNumber();
    //        break;
    //      case CommandType::TRANSFORM:
    //        *free_command_buffer_number = GetFreeTransformCommandNumber();
    //        break;
    //      default:
    //        LOG_ERROR("Command type is error.");
    //    }
    //
    //    if (require_command_buffer_numbers > *free_command_buffer_number) {
    //      ++command_task_to_be_submit;
    //    }

    ++command_task_to_be_submit;
    ++number_of_blocked_cross_task_flow;
  }
}

void MM::RenderSystem::CommandExecutor::ProcessWhenOneFailedSubmit(
    const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow) {
  if (auto complete_state = command_task_flow->complete_state_.lock()) {
    complete_state->store(0, std::memory_order_release);
    RenderFuture::wait_condition_variable_.notify_all();
  }

  // TODO Poor performance, awaiting optimization.
  std::unique_lock<std::mutex> guard{
      submit_failed_to_be_recycled_semaphore_mutex_};
  const std::uint32_t submit_failed_to_be_recycled_semaphore_index =
      (submit_failed_to_be_recycled_semaphore_current_index_ + 1) % 2;
  for (const auto& task : command_task_flow->can_be_submitted_tasks_) {
    for (const auto& semaphore_vector : task.default_wait_semaphore_) {
      for (const auto& semaphore : semaphore_vector) {
        submit_failed_to_be_recycled_semaphore_
            .at(submit_failed_to_be_recycled_semaphore_index)
            .insert(semaphore);
      }
    }
    for (const auto& semaphore_vector : task.default_signal_semaphore_) {
      for (const auto& semaphore : semaphore_vector) {
        submit_failed_to_be_recycled_semaphore_
            .at(submit_failed_to_be_recycled_semaphore_index)
            .insert(semaphore);
      }
    }
  }

  for (const auto& task : command_task_flow->pre_task_not_submit_task_) {
    for (const auto& semaphore_vector : task.default_wait_semaphore_) {
      for (const auto& semaphore : semaphore_vector) {
        submit_failed_to_be_recycled_semaphore_
            .at(submit_failed_to_be_recycled_semaphore_index)
            .insert(semaphore);
      }
    }
    for (const auto& semaphore_vector : task.default_signal_semaphore_) {
      for (const auto& semaphore : semaphore_vector) {
        submit_failed_to_be_recycled_semaphore_
            .at(submit_failed_to_be_recycled_semaphore_index)
            .insert(semaphore);
      }
    }
  }

  //  for (const auto& task : next_can_be_submitted_tasks) {
  //    for (const auto& semaphore : task.default_signal_semaphore_) {
  //      submit_failed_to_be_recycled_semaphore_
  //          .at(submit_failed_to_be_recycled_semaphore_index)
  //          .insert(semaphore);
  //    }
  //    for (const auto& semaphore : task.default_wait_semaphore_) {
  //      submit_failed_to_be_recycled_semaphore_
  //          .at(submit_failed_to_be_recycled_semaphore_index)
  //          .insert(semaphore);
  //    }
  //  }
}

void MM::RenderSystem::CommandExecutor::ProcessNextStepCanSubmitTask(
    const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow) {
  std::list<CommandTaskToBeSubmit> next_can_be_submitted_tasks;
  for (auto iter = command_task_flow->pre_task_not_submit_task_.begin();
       iter != command_task_flow->pre_task_not_submit_task_.end();) {
    bool is_all_submit = true;
    std::unique_lock<std::mutex> guard{
        command_task_flow->submitted_task_mutex_};
    for (const auto& pre_task : (*iter).command_task_->pre_tasks_) {
      if (command_task_flow->submitted_task_.count(pre_task) == 0) {
        is_all_submit = false;
        break;
      }
    }
    guard.unlock();

    if (is_all_submit) {
      next_can_be_submitted_tasks.emplace_back(std::move(*iter));
      iter = command_task_flow->pre_task_not_submit_task_.erase(iter);
      continue;
    }
    ++iter;
  }

  for (const auto& to_be_submit_task : next_can_be_submitted_tasks) {
    ProcessPreTaskNoSubmitTask(command_task_flow, to_be_submit_task);
  }
  command_task_flow->can_be_submitted_tasks_.splice(
      command_task_flow->can_be_submitted_tasks_.end(),
      next_can_be_submitted_tasks);
}

void MM::RenderSystem::CommandExecutor::SubmitTasks(
    std::shared_ptr<
        MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow>&
        command_task_flow,
    std::unique_ptr<ExecutingTask>&& input_tasks) {
  assert(input_tasks != nullptr);
  assert(input_tasks->command_task_->GetCommandType() !=
         CommandBufferType::UNDEFINED);

  if (input_tasks->command_buffers_.size() == 1) {
    SubmitTasksSync(command_task_flow, std::move(input_tasks));
  }

  // TODO Modify the coefficients to achieve optimal performance.
  if (input_tasks->command_task_->use_render_resource_count_ / 2 > 200) {
    SubmitTaskAsync(command_task_flow, std::move(input_tasks));
  } else {
    SubmitTasksSync(command_task_flow, std::move(input_tasks));
  }
}

void MM::RenderSystem::CommandExecutor::ProcessTask() {
  do {
    ProcessCompleteTask();

    {
      std::unique_lock<std::mutex> task_guard{task_flow_queue_mutex_};
      if (!task_flow_queue_.empty()) {
        task_guard.unlock();
        ProcessWaitTask();
        ProcessCommandFlowList();

        for (auto iter = executing_command_task_flows_.begin();
             iter != executing_command_task_flows_.end();) {
          std::shared_ptr<ExecutingCommandTaskFlow> command_task_flow = *(iter);
          if (!command_task_flow->initialize_or_not_) {
            ProcessRootTaskAndSubTask(command_task_flow);
          } else {
            ProcessNextStepCanSubmitTask(command_task_flow);
          }

          std::uint32_t number_of_blocked_cross_task_flow_sync_task = 0;

          while (command_task_flow->can_be_submitted_tasks_.size() -
                     number_of_blocked_cross_task_flow_sync_task >
                 0) {
            number_of_blocked_cross_task_flow_sync_task = 0;
            for (auto command_task_to_be_submit =
                     command_task_flow->can_be_submitted_tasks_.begin();
                 command_task_to_be_submit !=
                 command_task_flow->can_be_submitted_tasks_.end();) {
              assert(
                  command_task_to_be_submit->command_task_->GetCommandType() !=
                  CommandBufferType::UNDEFINED);
              ProcessOneCanSubmitTask(
                  command_task_to_be_submit, command_task_flow,
                  number_of_blocked_cross_task_flow_sync_task);
            }

            ProcessCompleteTask();
            ProcessNextStepCanSubmitTask(command_task_flow);

            if (command_task_flow->execute_result_->IsError()) {
              ProcessWhenOneFailedSubmit(command_task_flow);
              break;
            }
          }

          if (command_task_flow->can_be_submitted_tasks_.empty() &&
              command_task_flow->pre_task_not_submit_task_.empty()) {
            iter = executing_command_task_flows_.erase(iter);
            continue;
          }

          ++iter;
          break;
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::nanoseconds(500000));
  } while (executing_graph_command_buffers_.empty() &&
           executing_compute_command_buffers_.empty() &&
           executing_transform_command_buffers_.empty() &&
           task_flow_queue_.empty() &&
           recoding_graph_command_buffer_number_ == 0 &&
           recording_compute_command_buffer_number_ == 0 &&
           recording_transform_command_buffer_number_ == 0 &&
           executing_command_task_flows_.empty());

  processing_task_flow_queue_ = false;
}

void MM::RenderSystem::CommandExecutor::ProcessPreTaskNoSubmitTask(
    const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    const MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit&
        to_be_submit_task) {
  std::uint32_t post_index = 0;
  std::uint32_t semaphore_offset = 0;
  for (auto& post_task : to_be_submit_task.command_task_->post_tasks_) {
    if (command_task_flow->task_that_have_already_been_accessed_.find(
            post_task) !=
        command_task_flow->task_that_have_already_been_accessed_.end()) {
      for (auto iter = command_task_flow->command_task_flow_.tasks_.begin();
           iter != command_task_flow->command_task_flow_.tasks_.end(); ++iter) {
        if (&(*iter) == post_task) {
          std::vector<VkSemaphore> semaphore_to_be_wait;
          semaphore_to_be_wait.reserve(
              to_be_submit_task.default_wait_semaphore_.size() *
              (*iter)->sub_tasks_.size());
          for (std::uint32_t post_task_sub_task_index = 0;
               post_task_sub_task_index !=
               to_be_submit_task.post_task_sub_task_numbers_[post_index];
               ++post_task_sub_task_index) {
            for (const auto& wait_semaphore :
                 to_be_submit_task.default_wait_semaphore_) {
              semaphore_to_be_wait.emplace_back(
                  wait_semaphore[semaphore_offset + post_task_sub_task_index]);
            }
          }
          std::vector<std::uint32_t> post_task_sub_task_numbers;
          post_task_sub_task_numbers.reserve((*iter)->post_tasks_.size());
          std::uint32_t all_post_task_sub_task_number = 0;
          for (const auto& post_post_task : (*iter)->post_tasks_) {
            post_task_sub_task_numbers.emplace_back(
                (*post_post_task)->sub_tasks_.size());
            all_post_task_sub_task_number +=
                (*post_post_task)->sub_tasks_.size();
          }
          command_task_flow->task_that_have_already_been_accessed_.emplace(
              post_task);
          command_task_flow->pre_task_not_submit_task_.emplace_back(
              this, command_task_flow, std::move(*iter), semaphore_to_be_wait,
              GetSemaphore(
                  all_post_task_sub_task_number *
                  static_cast<std::uint32_t>((1 + (*iter)->sub_tasks_.size()))),
              std::move(post_task_sub_task_numbers));
          iter = command_task_flow->command_task_flow_.tasks_.erase(iter);
          continue;
        }
        ++iter;
      }
    } else {
      for (auto& task : command_task_flow->pre_task_not_submit_task_) {
        if (task.command_task_->this_unique_ptr_ == post_task) {
          for (std::uint32_t post_task_sub_task_index = 0;
               post_task_sub_task_index !=
               to_be_submit_task.post_task_sub_task_numbers_[post_index];
               ++post_task_sub_task_index) {
            for (const auto& wait_semaphore :
                 to_be_submit_task.default_wait_semaphore_) {
              task.default_wait_semaphore_[post_task_sub_task_index]
                  .emplace_back(wait_semaphore[semaphore_offset +
                                               post_task_sub_task_index]);
            }
          }
        }
      }
    }
  }
  semaphore_offset += to_be_submit_task.post_task_sub_task_numbers_[post_index];
  ++post_index;
}

void MM::RenderSystem::CommandExecutor::ProcessCommandFlowList() {
  std::uint32_t list_graph_command_number = 0;
  std::uint32_t list_compute_command_number = 0;
  std::uint32_t list_transform_command_number = 0;

  for (const auto& task_flow : executing_command_task_flows_) {
    std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> command_numbers =
        task_flow->GetRequireCommandBufferNumber();
    list_graph_command_number += std::get<0>(command_numbers);
    list_compute_command_number += std::get<1>(command_numbers);
    list_transform_command_number += std::get<2>(command_numbers);
  }

  std::uint32_t available_graph_command_number =
      GetFreeGraphCommandNumber() - list_graph_command_number;
  std::uint32_t available_compute_command_number =
      GetFreeComputeCommandNumber() - list_compute_command_number;
  std::uint32_t available_transform_command_number =
      GetFreeTransformCommandNumber() - list_transform_command_number;

  std::lock_guard<std::mutex> guard{task_flow_queue_mutex_};
  for (auto iter = task_flow_queue_.begin(); iter != task_flow_queue_.end();) {
    CommandTaskFlowToBeRun& task_flow = *iter;
    if (task_flow.the_maximum_number_of_graph_buffers_required_for_one_task_ >
        graph_command_number_) {
      AddCommandBuffer(
          CommandType::GRAPH,
          static_cast<std::uint32_t>((
              static_cast<float>(
                  task_flow
                      .the_maximum_number_of_graph_buffers_required_for_one_task_) *
              1.5)) -
              graph_command_number_)
          .Exception(
              [function_name = MM_FUNCTION_NAME, &iter,
               &task_flow_queue = task_flow_queue_](ErrorResult error_result) {
                MM_LOG_SYSTEM->CheckResult(
                    error_result.GetErrorCode(),
                    std::string("[") + function_name + "] " +
                        "Failed to create new command buffer, skip this "
                        "command_task_flow.",
                    MM::LogSystem::LogSystem::LogLevel::ERROR);
                if (auto complete_state = iter->complete_state_.lock()) {
                  complete_state->store(0, std::memory_order_release);
                  *(iter->execute_result_->GetError().GetErrorCode()) ==
                      MM::ErrorCode::RENDER_COMMAND_RECORD_OR_SUBMIT_FAILED;
                  RenderFuture::wait_condition_variable_.notify_all();
                }
                iter = task_flow_queue.erase(iter);
              }

          );
      continue;
    }
    if (task_flow.the_maximum_number_of_compute_buffers_required_for_one_task_ >
        compute_command_number_) {
      MM_CHECK(
          AddCommandBuffer(
              CommandType::COMPUTE,
              static_cast<std::uint32_t>((
                  static_cast<float>(
                      task_flow
                          .the_maximum_number_of_compute_buffers_required_for_one_task_) *
                  1.5)) -
                  compute_command_number_),
          if (auto complete_state = iter->complete_state_.lock()) {
            complete_state->store(0, std::memory_order_release);
            *(iter->execute_result_) ==
                MM::Utils::ExecuteResult ::
                    RENDER_COMMAND_RECORD_OR_SUBMIT_FAILED;
            RenderFuture::wait_condition_variable_.notify_all();
          } iter = task_flow_queue_.erase(iter);
          MM_LOG_ERROR("Failed to create new command buffer, skip this "
                       "command_task_flow.");)
      continue;
    }
    if (task_flow
            .the_maximum_number_of_transform_buffers_required_for_one_task_ >
        transform_command_number_) {
      MM_CHECK(
          AddCommandBuffer(
              CommandType::TRANSFORM,
              static_cast<std::uint32_t>((
                  static_cast<float>(
                      task_flow
                          .the_maximum_number_of_transform_buffers_required_for_one_task_) *
                  1.5)) -
                  transform_command_number_),
          if (auto complete_state = iter->complete_state_.lock()) {
            complete_state->store(0, std::memory_order_release);
            *(iter->execute_result_) ==
                MM::Utils::ExecuteResult ::
                    RENDER_COMMAND_RECORD_OR_SUBMIT_FAILED;
            RenderFuture::wait_condition_variable_.notify_all();
          } iter = task_flow_queue_.erase(iter);
          MM_LOG_ERROR("Failed to create new command buffer, skip this "
                       "command_task_flow.");)
      continue;
    }

    // TODO Adjust the coefficient to achieve the best effect (now the
    // coefficient is 0.75).
    if ((task_flow.command_task_flow_.task_numbers_[0] != 0 &&
         MM::Utils::IntegerMult(task_flow.command_task_flow_.task_numbers_[0],
                                0.75) < available_graph_command_number) ||
        (task_flow.command_task_flow_.task_numbers_[1] != 0 &&
         MM::Utils::IntegerMult(task_flow.command_task_flow_.task_numbers_[1],
                                0.75) < available_compute_command_number) ||
        (task_flow.command_task_flow_.task_numbers_[2] != 0 &&
         MM::Utils::IntegerMult(task_flow.command_task_flow_.task_numbers_[2],
                                0.75) < available_transform_command_number)) {
      executing_command_task_flows_.emplace_back(
          std::make_shared<ExecutingCommandTaskFlow>(std::move(*iter)));
    } else {
      break;
    }
  }
}
void MM::RenderSystem::CommandExecutor::ProcessCrossTaskFlowRenderResourceSync(
    std::list<MM::RenderSystem::CommandExecutor::CommandTaskToBeSubmit>::
        iterator& command_task_to_be_submit,
    const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    bool& command_submit_condition) {
  std::lock_guard<std::mutex> guard(command_task_render_resource_states_mutex_);
  std::vector<RenderResourceDataID> same_command_type_sync_render_resource;
  std::vector<RenderResourceDataID> new_use_of_render_resource;
  bool
      have_different_command_type_sync_render_resource_or_same_command_type_but_recording =
          false;

  for (const auto& render_resource_ID :
       command_task_to_be_submit->command_task_
           ->cross_task_flow_sync_render_resource_IDs_) {
    auto render_resource_states_iter =
        command_task_render_resource_states_.find(render_resource_ID);
    if (render_resource_states_iter !=
        command_task_render_resource_states_.end()) {
      for (const auto& render_resource_states :
           render_resource_states_iter->second) {
        if (render_resource_states.task_flow_ID_ !=
            command_task_flow->task_flow_ID_) {
          if (render_resource_states.command_type_ !=
                  command_task_to_be_submit->command_task_->command_type_ ||
              render_resource_states.command_buffer_state_ ==
                  CommandBufferState::Recording) {
            have_different_command_type_sync_render_resource_or_same_command_type_but_recording =
                true;
            break;
          }
        }
      }

      if (have_different_command_type_sync_render_resource_or_same_command_type_but_recording) {
        break;
      }

      same_command_type_sync_render_resource.emplace_back(render_resource_ID);
    } else {
      new_use_of_render_resource.emplace_back(render_resource_ID);
    }
  }

  if (!have_different_command_type_sync_render_resource_or_same_command_type_but_recording) {
    for (const auto& render_resource_ID :
         same_command_type_sync_render_resource) {
      command_task_render_resource_states_.at(render_resource_ID)
          .emplace_back(CommandTaskRenderResourceState{
              command_task_flow->task_flow_ID_,
              command_task_to_be_submit->command_task_->command_task_ID_,
              command_task_to_be_submit->command_task_->command_type_,
              CommandBufferState::Recording});
    }

    for (const auto& render_resource_ID : new_use_of_render_resource) {
      if (command_task_render_resource_states_.count(render_resource_ID) == 0) {
        command_task_render_resource_states_.insert(std::make_pair(
            render_resource_ID,
            std::list<CommandTaskRenderResourceState>{
                CommandTaskRenderResourceState{
                    command_task_flow->task_flow_ID_,
                    command_task_to_be_submit->command_task_->command_task_ID_,
                    command_task_to_be_submit->command_task_->command_type_,
                    CommandBufferState::Recording}}));
      }
    }

    command_submit_condition = true;
  }
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetFreeCommandNumber(
    MM::RenderSystem::CommandType command_type) const {
  switch (command_type) {
    case CommandType::GRAPH:
      return free_graph_command_buffers_.size();
      break;
    case CommandType::COMPUTE:
      return free_compute_command_buffers_.size();
      break;
    case CommandType::TRANSFORM:
      return free_transform_command_buffers_.size();
      break;
    default:
      return free_graph_command_buffers_.size();
  }
}

void MM::RenderSystem::CommandExecutor::SubmitTasksSync(
    std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    std::unique_ptr<ExecutingTask>&& input_tasks) {
  ExecuteResult result = RecordAndSubmitCommandSync(input_tasks);
  PostProcessOfSubmitTask(command_task_flow, input_tasks, result);
}

void MM::RenderSystem::CommandExecutor::SubmitTaskAsync(
    std::shared_ptr<ExecutingCommandTaskFlow> command_task_flow,
    std::unique_ptr<ExecutingTask>&& input_tasks) {
  TaskSystem::Taskflow taskflow;
  taskflow.emplace([this_executor = this,
                    command_task_flow_in = std::move(command_task_flow),
                    input_tasks_in = input_tasks.release()]() mutable {
    std::unique_ptr<ExecutingTask> input_tasks(input_tasks_in);
    ExecuteResult result =
        this_executor->RecordAndSubmitCommandASync(input_tasks);
    this_executor->PostProcessOfSubmitTask(command_task_flow_in, input_tasks,
                                           result);
  });
  MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, std::move(taskflow));
}

MM::ExecuteResult MM::RenderSystem::CommandExecutor::RecordAndSubmitCommandSync(
    std::unique_ptr<ExecutingTask>& input_tasks) {
  ExecuteResult result = ExecuteResult::SUCCESS;

  std::uint32_t buffer_index = 0;
  for (; buffer_index <
         input_tasks->command_task_->GetRequireCommandBufferNumber();
       ++buffer_index) {
    const auto& command =
        input_tasks->command_task_->GetCommands().at(buffer_index);
    auto& buffer = input_tasks->command_buffers_.at(buffer_index);
    command(*buffer);
  }

  for (const auto& sub_task : input_tasks->command_task_->sub_tasks_) {
    for (std::uint32_t i = 0; i < sub_task->GetRequireCommandBufferNumber();
         ++i, ++buffer_index) {
      const auto& command = sub_task->GetCommands().at(i);
      auto& buffer = input_tasks->command_buffers_.at(buffer_index);
      command(*buffer);
    }
  }

  std::vector<VkSubmitInfo> submit_infos;
  submit_infos.reserve(1 + input_tasks->command_task_->GetSubTasksNumber());

  std::vector<std::vector<VkCommandBuffer>> command_buffers;
  command_buffers.reserve(submit_infos.size());

  buffer_index = 0;
  command_buffers.emplace_back();
  command_buffers.begin()->reserve(
      input_tasks->command_task_->GetRequireCommandBufferNumber());
  for (; buffer_index !=
         input_tasks->command_task_->GetRequireCommandBufferNumber();
       ++buffer_index) {
    command_buffers.begin()->emplace_back(
        input_tasks->command_buffers_.at(buffer_index)->GetCommandBuffer());
  }

  for (const auto& sub_task : input_tasks->command_task_->sub_tasks_) {
    std::vector<VkCommandBuffer> temp_buffers;
    for (std::uint32_t i = 0; i != sub_task->GetRequireCommandBufferNumber();
         ++i, ++buffer_index) {
      temp_buffers.emplace_back(
          input_tasks->command_buffers_.at(buffer_index)->GetCommandBuffer());
    }
    command_buffers.emplace_back(std::move(temp_buffers));
  }

  submit_infos.emplace_back(Utils::GetCommandSubmitInfo(command_buffers.at(0)));
  submit_infos.back().waitSemaphoreCount =
      static_cast<uint32_t>(input_tasks->wait_semaphore_.at(0).size());
  submit_infos.back().pWaitDstStageMask =
      input_tasks->wait_semaphore_stages_.at(0).data();
  submit_infos.back().pWaitSemaphores =
      input_tasks->wait_semaphore_.at(0).data();
  submit_infos.back().signalSemaphoreCount =
      static_cast<uint32_t>(input_tasks->signal_semaphore_.at(0).size());
  submit_infos.back().pSignalSemaphores =
      input_tasks->signal_semaphore_.at(0).data();

  std::uint32_t sub_task_index = 1;
  for (const auto& sub_task : input_tasks->command_task_->sub_tasks_) {
    submit_infos.emplace_back(
        Utils::GetCommandSubmitInfo(command_buffers.at(sub_task_index)));
    submit_infos.back().waitSemaphoreCount = static_cast<uint32_t>(
        input_tasks->wait_semaphore_.at(sub_task_index).size());
    submit_infos.back().pWaitDstStageMask =
        input_tasks->wait_semaphore_stages_.at(sub_task_index).data();
    submit_infos.back().pWaitSemaphores =
        input_tasks->wait_semaphore_.at(sub_task_index).data();
    submit_infos.back().signalSemaphoreCount = static_cast<uint32_t>(
        input_tasks->signal_semaphore_.at(sub_task_index).size());
    submit_infos.back().pSignalSemaphores =
        input_tasks->signal_semaphore_.at(sub_task_index).data();

    ++sub_task_index;
  }

  MM_VK_CHECK(vkQueueSubmit(input_tasks->command_buffers_.at(0)->GetQueue(),
                            static_cast<std::uint32_t>(submit_infos.size()),
                            submit_infos.data(),
                            input_tasks->command_buffers_.at(0)->GetFence()),
              result = Utils::VkResultToMMResult(MM_VK_RESULT_CODE))

  return result;
}

MM::ExecuteResult
MM::RenderSystem::CommandExecutor::RecordAndSubmitCommandASync(
    std::unique_ptr<ExecutingTask>& input_tasks) {
  TaskSystem::Taskflow taskflow;
  ExecuteResult result = ExecuteResult::SUCCESS;

  auto submit_task = taskflow.emplace([&execute_command_task = input_tasks,
                                       &result = result]() {
    std::vector<VkSubmitInfo> submit_infos;
    submit_infos.reserve(
        1 + execute_command_task->command_task_->GetSubTasksNumber());

    std::vector<std::vector<VkCommandBuffer>> command_buffers;
    command_buffers.reserve(submit_infos.size());

    std::uint32_t buffer_index = 0;
    command_buffers.emplace_back();
    command_buffers.begin()->reserve(
        execute_command_task->command_task_->GetRequireCommandBufferNumber());
    for (; buffer_index !=
           execute_command_task->command_task_->GetRequireCommandBufferNumber();
         ++buffer_index) {
      command_buffers.begin()->emplace_back(
          execute_command_task->command_buffers_.at(buffer_index)
              ->GetCommandBuffer());
    }

    for (const auto& sub_task :
         execute_command_task->command_task_->sub_tasks_) {
      std::vector<VkCommandBuffer> temp_buffers;
      for (std::uint32_t i = 0; i != sub_task->GetRequireCommandBufferNumber();
           ++i, ++buffer_index) {
        temp_buffers.emplace_back(
            execute_command_task->command_buffers_.at(buffer_index)
                ->GetCommandBuffer());
      }
      command_buffers.emplace_back(std::move(temp_buffers));
    }

    submit_infos.emplace_back(
        Utils::GetCommandSubmitInfo(command_buffers.at(0)));
    submit_infos.back().waitSemaphoreCount = static_cast<uint32_t>(
        execute_command_task->wait_semaphore_.at(0).size());
    submit_infos.back().pWaitDstStageMask =
        execute_command_task->wait_semaphore_stages_.at(0).data();
    submit_infos.back().pWaitSemaphores =
        execute_command_task->wait_semaphore_.at(0).data();
    submit_infos.back().signalSemaphoreCount = static_cast<uint32_t>(
        execute_command_task->signal_semaphore_.at(0).size());
    submit_infos.back().pSignalSemaphores =
        execute_command_task->signal_semaphore_.at(0).data();

    std::uint32_t sub_task_index = 1;
    for (const auto& sub_task :
         execute_command_task->command_task_->sub_tasks_) {
      submit_infos.emplace_back(
          Utils::GetCommandSubmitInfo(command_buffers.at(sub_task_index)));
      submit_infos.back().waitSemaphoreCount = static_cast<uint32_t>(
          execute_command_task->wait_semaphore_.at(sub_task_index).size());
      submit_infos.back().pWaitDstStageMask =
          execute_command_task->wait_semaphore_stages_.at(sub_task_index)
              .data();
      submit_infos.back().pWaitSemaphores =
          execute_command_task->wait_semaphore_.at(sub_task_index).data();
      submit_infos.back().signalSemaphoreCount = static_cast<uint32_t>(
          execute_command_task->signal_semaphore_.at(sub_task_index).size());
      submit_infos.back().pSignalSemaphores =
          execute_command_task->signal_semaphore_.at(sub_task_index).data();

      ++sub_task_index;
    }

    MM_VK_CHECK(
        vkQueueSubmit(execute_command_task->command_buffers_.at(0)->GetQueue(),
                      static_cast<std::uint32_t>(submit_infos.size()),
                      submit_infos.data(),
                      execute_command_task->command_buffers_.at(0)->GetFence()),
        result = Utils::VkResultToMMResult(MM_VK_RESULT_CODE))
  });

  std::uint32_t buffer_index = 0;
  for (; buffer_index <
         input_tasks->command_task_->GetRequireCommandBufferNumber();
       ++buffer_index) {
    auto record_command = taskflow.emplace(
        [&command = input_tasks->command_task_->GetCommands().at(buffer_index),
         &buffer = input_tasks->command_buffers_.at(buffer_index)]() {
          command(*buffer);
        });
    record_command.precede(submit_task);
  }
  for (const auto& sub_task : input_tasks->command_task_->sub_tasks_) {
    for (std::uint32_t i = 0; i < sub_task->GetRequireCommandBufferNumber();
         ++i, ++buffer_index) {
      auto record_command = taskflow.emplace(
          [&command = sub_task->GetCommands().at(i),
           &buffer = input_tasks->command_buffers_.at(buffer_index)]() {
            command(*buffer);
          });
      record_command.precede(submit_task);
    }
  }

  auto future = MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, taskflow);
  future.get();

  return result;
}
void MM::RenderSystem::CommandExecutor::PostProcessOfSubmitTask(
    std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
    std::unique_ptr<ExecutingTask>& input_tasks, ExecuteResult& result) {
  *(command_task_flow->execute_result_) |= result;

  if (result != ExecuteResult::SUCCESS) {
    if (auto complete_state = command_task_flow->complete_state_.lock()) {
      complete_state->store(0, std::memory_order_release);
      RenderFuture::wait_condition_variable_.notify_all();
    }

    {
      std::lock_guard<std::mutex> command_buffer_guard{
          submit_failed_to_be_recycled_command_buffer_mutex_};
      for (auto& command_buffer : input_tasks->command_buffers_) {
        command_buffer->ResetCommandBuffer();
        submit_failed_to_be_recycled_command_buffer_.emplace_back(
            std::move(command_buffer));
      }
    }

    {
      std::lock_guard<std::mutex> semaphore_guard{
          submit_failed_to_be_recycled_semaphore_mutex_};
      const std::uint32_t submit_failed_to_be_recycled_semaphore_index =
          (submit_failed_to_be_recycled_semaphore_current_index_ + 1) % 2;
      for (const auto& wait_semaphore_vector : input_tasks->wait_semaphore_) {
        for (std::uint32_t i = 0;
             i != input_tasks->default_wait_semaphore_number_; ++i) {
          submit_failed_to_be_recycled_semaphore_
              .at(submit_failed_to_be_recycled_semaphore_index)
              .insert(wait_semaphore_vector.at(i));
        }
      }
    }

    {
      std::lock_guard<std::mutex> guard{
          command_task_render_resource_states_mutex_};
      for (const auto& render_resource_ID :
           input_tasks->command_task_
               ->cross_task_flow_sync_render_resource_IDs_) {
        auto& states =
            command_task_render_resource_states_.find(render_resource_ID)
                ->second;
        for (auto iter = states.begin(); iter != states.end();) {
          if ((*iter).task_flow_ID_ == input_tasks->task_flow_ID_ &&
              (*iter).task_ID_ ==
                  input_tasks->command_task_->command_task_ID_) {
            iter = states.erase(iter);
            continue;
          }

          ++iter;
        }

        if (states.empty()) {
          command_task_render_resource_states_.erase(
              command_task_render_resource_states_.find(render_resource_ID));
        }
      }
    }

    MM_LOG_ERROR("Render command submit failed.");
    return;
  }

  if (command_task_flow->execute_result_->IsError()) {
    if (auto complete_state = command_task_flow->complete_state_.lock()) {
      complete_state->store(0, std::memory_order_release);
      RenderFuture::wait_condition_variable_.notify_all();
    }

    std::lock_guard<std::mutex> semaphore_guard{
        submit_failed_to_be_recycled_semaphore_mutex_};
    const std::uint32_t submit_failed_to_be_recycled_semaphore_index =
        (submit_failed_to_be_recycled_semaphore_current_index_ + 1) % 2;
    for (const auto& wait_semaphore_vector : input_tasks->wait_semaphore_) {
      for (std::uint32_t i = 0;
           i != input_tasks->default_wait_semaphore_number_; ++i) {
        submit_failed_to_be_recycled_semaphore_
            .at(submit_failed_to_be_recycled_semaphore_index)
            .insert(wait_semaphore_vector.at(i));
      }
    }

    {
      std::lock_guard<std::mutex> guard{
          command_task_render_resource_states_mutex_};
      for (const auto& render_resource_ID :
           input_tasks->command_task_
               ->cross_task_flow_sync_render_resource_IDs_) {
        auto& states =
            command_task_render_resource_states_.find(render_resource_ID)
                ->second;
        for (auto iter = states.begin(); iter != states.end();) {
          if ((*iter).task_flow_ID_ == input_tasks->task_flow_ID_ &&
              (*iter).task_ID_ ==
                  input_tasks->command_task_->command_task_ID_) {
            iter->command_buffer_state_ = CommandBufferState::Executiong;
          }

          ++iter;
        }
      }
    }
  }

  if (input_tasks->command_task_->GetCommandType() ==
      CommandBufferType::GRAPH) {
    std::unique_lock<std::mutex> executing_command_guard{
        executing_graph_command_buffers_mutex_};
    const std::uint32_t temp =
        static_cast<std::uint32_t>(input_tasks->command_buffers_.size());
    executing_graph_command_buffers_.emplace_back(std::move(*input_tasks));
    executing_command_guard.unlock();
    std::unique_lock<std::mutex> submitted_task_guard{
        command_task_flow->submitted_task_mutex_};
    command_task_flow->submitted_task_.insert(
        executing_graph_command_buffers_.back()
            .command_task_->this_unique_ptr_);
    submitted_task_guard.unlock();

    recoding_graph_command_buffer_number_ -= temp;
  } else if (input_tasks->command_task_->GetCommandType() ==
             CommandBufferType::COMPUTE) {
    std::unique_lock<std::mutex> executing_command_guard{
        executing_compute_command_buffers_mutex_};
    const std::uint32_t temp =
        static_cast<std::uint32_t>(input_tasks->command_buffers_.size());
    executing_compute_command_buffers_.emplace_back(std::move(*input_tasks));
    executing_command_guard.unlock();
    std::unique_lock<std::mutex> submitted_task_guard{
        command_task_flow->submitted_task_mutex_};
    command_task_flow->submitted_task_.insert(
        executing_compute_command_buffers_.back()
            .command_task_->this_unique_ptr_);
    submitted_task_guard.unlock();

    recording_compute_command_buffer_number_ -= temp;
  } else {
    std::unique_lock<std::mutex> executing_command_guard{
        executing_transform_command_buffers_mutex_};
    const std::uint32_t temp =
        static_cast<std::uint32_t>(input_tasks->command_buffers_.size());
    executing_transform_command_buffers_.emplace_back(std::move(*input_tasks));
    executing_command_guard.unlock();
    std::unique_lock<std::mutex> submitted_task_guard{
        command_task_flow->submitted_task_mutex_};
    command_task_flow->submitted_task_.insert(
        executing_transform_command_buffers_.back()
            .command_task_->this_unique_ptr_);
    submitted_task_guard.unlock();

    recording_transform_command_buffer_number_ -= temp;
  }
}

void MM::RenderSystem::CommandExecutor::LockExecutor() {
  lock_count_.fetch_add(1, std::memory_order_acq_rel);
}

void MM::RenderSystem::CommandExecutor::UnlockExecutor() {
  assert(lock_count_.load(std::memory_order_acquire) != 0);
  if (lock_count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    std::lock(task_flow_queue_mutex_, task_flow_submit_during_lockdown_mutex_);
    std::lock_guard guard1(task_flow_queue_mutex_, std::adopt_lock),
        guard2(task_flow_submit_during_lockdown_mutex_, std::adopt_lock);

    task_flow_queue_.splice(task_flow_queue_.end(),
                            task_flow_submit_during_lockdown_);
  }
}

bool MM::RenderSystem::CommandExecutor::IsFree() const {
  return !processing_task_flow_queue_.load(std::memory_order_acquire);
}

void MM::RenderSystem::CommandExecutor::AcquireGeneralGraphCommandBuffer(
    std::unique_ptr<AllocatedCommandBuffer>& output) {
  std::unique_lock<std::mutex> guard{
      general_command_buffers_acquire_release_mutex_};
  while (true) {
    if (general_command_buffers_[0][0] != nullptr) {
      output = std::move(general_command_buffers_[0][0]);
      general_garph_command_wait_flag_ = false;
      return;
    }
    if (general_command_buffers_[0][1] != nullptr) {
      output = std::move(general_command_buffers_[0][1]);
      general_garph_command_wait_flag_ = false;
      return;
    }
    if (general_command_buffers_[0][2] != nullptr) {
      output = std::move(general_command_buffers_[0][2]);
      general_garph_command_wait_flag_ = false;
      return;
    }

    general_garph_command_wait_flag_ = true;
    general_command_buffers_acquire_release_condition_variable_.wait(
        guard, [this_executor = this]() {
          return this_executor->general_command_buffers_[0][0] != nullptr ||
                 this_executor->general_command_buffers_[0][1] != nullptr ||
                 this_executor->general_command_buffers_[0][2] != nullptr;
        });
  }
}

MM::ExecuteResult
MM::RenderSystem::CommandExecutor::AcquireGeneralComputeCommandBuffer(
    std::unique_ptr<AllocatedCommandBuffer>& output) {
  std::unique_lock<std::mutex> guard{
      general_command_buffers_acquire_release_mutex_};
  if (general_command_buffers_[1][0] != nullptr) {
    output = std::move(general_command_buffers_[1][0]);
    general_compute_command_wait_flag_ = false;
    return ExecuteResult ::SUCCESS;
  }
  if (general_command_buffers_[1][1] != nullptr) {
    output = std::move(general_command_buffers_[1][1]);
    general_compute_command_wait_flag_ = false;
    return ExecuteResult ::SUCCESS;
  }
  if (general_command_buffers_[1][2] != nullptr) {
    output = std::move(general_command_buffers_[1][2]);
    general_compute_command_wait_flag_ = false;
    return ExecuteResult ::SUCCESS;
  }

  general_compute_command_wait_flag_ = true;
  general_command_buffers_acquire_release_condition_variable_.wait(
      guard, [this_executor = this]() {
        return this_executor->general_command_buffers_[1][0] != nullptr ||
               this_executor->general_command_buffers_[1][1] != nullptr ||
               this_executor->general_command_buffers_[1][2] != nullptr;
      });
  return ExecuteResult ::SYNCHRONIZE_FAILED;
}

MM::ExecuteResult
MM::RenderSystem::CommandExecutor::AcquireGeneralTransformCommandBuffer(
    std::unique_ptr<AllocatedCommandBuffer>& output) {
  std::unique_lock<std::mutex> guard{
      general_command_buffers_acquire_release_mutex_};
  if (general_command_buffers_[2][0] != nullptr) {
    output = std::move(general_command_buffers_[2][0]);
    general_transform_command_wait_flag_ = false;
    return ExecuteResult ::SUCCESS;
  }
  if (general_command_buffers_[2][1] != nullptr) {
    output = std::move(general_command_buffers_[2][1]);
    general_transform_command_wait_flag_ = false;
    return ExecuteResult ::SUCCESS;
  }
  if (general_command_buffers_[2][2] != nullptr) {
    output = std::move(general_command_buffers_[2][2]);
    general_transform_command_wait_flag_ = false;
    return ExecuteResult ::SUCCESS;
  }

  general_transform_command_wait_flag_ = true;
  general_command_buffers_acquire_release_condition_variable_.wait(
      guard, [this_executor = this]() {
        return this_executor->general_command_buffers_[2][0] != nullptr ||
               this_executor->general_command_buffers_[2][1] != nullptr ||
               this_executor->general_command_buffers_[2][2] != nullptr;
      });
  return ExecuteResult ::SYNCHRONIZE_FAILED;
}

void MM::RenderSystem::CommandExecutor::ReleaseGeneralCommandBuffer(
    std::unique_ptr<AllocatedCommandBuffer>&& output) {
  if (output != nullptr && output->IsValid()) {
    return;
  }
  std::uint64_t general_command_type_index = UINT32_MAX;
  switch (output->GetCommandBufferType()) {
    case CommandBufferType::GRAPH:
      general_command_type_index = 0;
      break;
    case CommandBufferType::COMPUTE:
      general_command_type_index = 1;
      break;
    case CommandBufferType::TRANSFORM:
      general_command_type_index = 2;
      break;
    case CommandBufferType::UNDEFINED:
      return;
  }

  std::lock_guard guard{general_command_buffers_acquire_release_mutex_};
  if (general_command_buffers_[general_command_type_index][0] == nullptr) {
    output->ResetCommandBuffer();
    output->ResetFence();
    general_command_buffers_[general_command_type_index][0] = std::move(output);
    if (general_garph_command_wait_flag_) {
      general_command_buffers_acquire_release_condition_variable_.notify_all();
    }
    return;
  }
  if (general_command_buffers_[general_command_type_index][1] == nullptr) {
    output->ResetCommandBuffer();
    output->ResetFence();
    general_command_buffers_[general_command_type_index][1] = std::move(output);
    if (general_transform_command_wait_flag_) {
      general_command_buffers_acquire_release_condition_variable_.notify_all();
    }
    return;
  }
  if (general_command_buffers_[general_command_type_index][2] != nullptr) {
    output->ResetCommandBuffer();
    output->ResetFence();
    general_command_buffers_[general_command_type_index][2] = std::move(output);
    if (general_transform_command_wait_flag_) {
      general_command_buffers_acquire_release_condition_variable_.notify_all();
    }
    return;
  }
}

MM::RenderSystem::CommandExecutorLockGuard::~CommandExecutorLockGuard() {
  Unlock();
}

MM::RenderSystem::CommandExecutorLockGuard::CommandExecutorLockGuard(
    MM::RenderSystem::CommandExecutor& command_executor)
    : command_executor_(&command_executor), lock_flag_(false) {
  Lock();
}

MM::RenderSystem::CommandExecutorLockGuard::CommandExecutorLockGuard(
    MM::RenderSystem::CommandExecutorLockGuard&& other) noexcept
    : command_executor_(other.command_executor_),
      lock_flag_(other.lock_flag_.load(std::memory_order_acquire)) {
  other.command_executor_ = nullptr;
  lock_flag_.store(false, std::memory_order_relaxed);
}

MM::RenderSystem::CommandExecutorLockGuard&
MM::RenderSystem::CommandExecutorLockGuard::operator=(
    MM::RenderSystem::CommandExecutorLockGuard&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  command_executor_ = other.command_executor_;
  lock_flag_ = other.lock_flag_.load(std::memory_order_acquire);

  other.command_executor_ = nullptr;
  other.lock_flag_.store(false, std::memory_order_relaxed);

  return *this;
}

void MM::RenderSystem::CommandExecutorLockGuard::Lock() {
  assert(lock_flag_.load(std::memory_order_acquire) != true);
  lock_flag_.store(true, std::memory_order_release);
  command_executor_->LockExecutor();
}

bool MM::RenderSystem::CommandExecutorLockGuard::IsLocked() const {
  return lock_flag_.load(std::memory_order_acquire);
}

void MM::RenderSystem::CommandExecutorLockGuard::Unlock() {
  if (lock_flag_.load(std::memory_order_acquire)) {
    command_executor_->UnlockExecutor();
    lock_flag_.store(false, std::memory_order_release);
  }
}

bool MM::RenderSystem::CommandExecutorLockGuard::IsValid() const {
  return command_executor_ != nullptr;
}

MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard::
    CommandExecutorGeneralCommandBufferGuard(
        MM::RenderSystem::CommandExecutor& command_executor,
        MM::RenderSystem::CommandBufferType command_buffer_type)
    : command_executor_(&command_executor), general_command_buffer_() {
  if (command_executor_->IsValid()) {
    switch (command_buffer_type) {
      case CommandBufferType::GRAPH:
        command_executor_->AcquireGeneralGraphCommandBuffer(
            general_command_buffer_);
        return;
      case CommandBufferType::COMPUTE:
        command_executor_->AcquireGeneralComputeCommandBuffer(
            general_command_buffer_);
        return;
      case CommandBufferType::TRANSFORM:
        command_executor_->AcquireGeneralTransformCommandBuffer(
            general_command_buffer_);
        return;
      case CommandBufferType::UNDEFINED:
        assert(false);
    }
  }
}

MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard::
    ~CommandExecutorGeneralCommandBufferGuard() {
  Release();
}

MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard::
    CommandExecutorGeneralCommandBufferGuard(
        MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard&&
            other) noexcept
    : command_executor_(other.command_executor_),
      general_command_buffer_(std::move(other.general_command_buffer_)) {
  other.command_executor_ = nullptr;
}

MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard&
MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard::operator=(
    MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard&&
        other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  command_executor_ = other.command_executor_;
  general_command_buffer_ = std::move(general_command_buffer_);

  other.command_executor_ = nullptr;

  return *this;
}

MM::RenderSystem::AllocatedCommandBuffer* MM::RenderSystem::
    CommandExecutorGeneralCommandBufferGuard::GetGeneralCommandBuffer() {
  return general_command_buffer_.get();
}

void MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard::Release() {
  command_executor_->ReleaseGeneralCommandBuffer(
      std::move(general_command_buffer_));
}

bool MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard::IsValid()
    const {
  return general_command_buffer_ != nullptr;
}
