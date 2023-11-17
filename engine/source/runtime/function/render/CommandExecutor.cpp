//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/CommandExecutor.h"

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <stack>
#include <thread>
#include <vector>

#include "runtime/function/render/CommandTask.h"
#include "runtime/function/render/CommandTaskFlow.h"
#include "runtime/function/render/RenderFuture.h"
#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/vk_command_pre.h"
#include "runtime/function/render/vk_engine.h"
#include "runtime/function/render/vk_enum.h"
#include "runtime/function/render/vk_type_define.h"
#include "runtime/function/render/vk_utils.h"
#include "utils/error.h"

MM::RenderSystem::CommandExecutor::CommandExecutor(RenderEngine* engine)
    : render_engine_(engine) {
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
}

MM::RenderSystem::CommandExecutor::~CommandExecutor() {
  std::unique_lock<std::mutex> guard{wait_task_flows_mutex_};
  wait_task_flow_queue_.clear();
  guard.unlock();
  while (processing_task_flow_queue_) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
  }
}

MM::RenderSystem::CommandExecutor::CommandExecutor(
    RenderEngine* engine, const std::uint32_t& graph_command_number,
    const std::uint32_t& compute_command_number,
    const std::uint32_t& transform_command_number,
    const std::uint32_t& waiting_coefficient,
    const std::uint32_t& max_post_command_task_number)
    : render_engine_(engine),
      graph_command_number_(graph_command_number),
      compute_command_number_(compute_command_number),
      transform_command_number_(transform_command_number),
      wait_coefficient_(waiting_coefficient),
      wait_dst_stage_mask_(max_post_command_task_number,
                           static_cast<VkPipelineStageFlags>(
                               VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)) {
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
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetGraphCommandNumber() const {
  assert(IsValid());

  return graph_command_number_;
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetComputeCommandNumber()
    const {
  assert(IsValid());

  return compute_command_number_;
}

std::uint32_t MM::RenderSystem::CommandExecutor::GetTransformCommandNumber()
    const {
  assert(IsValid());

  return transform_command_number_;
}

std::uint32_t
MM::RenderSystem::CommandExecutor::GetFreeGraphCommandBufferNumber() const {
  assert(IsValid());

  return free_graph_command_buffers_.size();
}

std::uint32_t
MM::RenderSystem::CommandExecutor::GetFreeComputeCommandBufferNumber() const {
  assert(IsValid());

  return free_compute_command_buffers_.size();
}

std::uint32_t
MM::RenderSystem::CommandExecutor::GetFreeTransformCommandBufferNumber() const {
  assert(IsValid());

  return free_transform_command_buffers_.size();
}

MM::Result<MM::RenderSystem::RenderFuture, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::Run(CommandTaskFlow&& command_task_flow) {
  assert(IsValid());

  if (!command_task_flow.IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

#ifndef NDEBUG
  if (!command_task_flow.IsValidSubmissionObject()) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }
#endif

  for (CommandTask& command_task : command_task_flow.tasks_) {
    if (command_task.GetRequireCommandBufferNumber() >
        GetFreeCommandNumber(command_task.GetCommandType())) {
      return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
    }
    if (command_task.post_tasks_.size() >
        GetFreeCommandNumber(command_task.GetCommandType())) {
      return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
    }
  }

  if (lock_count_.load(std::memory_order_acquire)) {
    std::lock_guard guard{task_flow_submit_during_lockdown_mutex_};
    task_flow_submit_during_lockdown_.emplace_back(
        std::move(command_task_flow));
    CommandTaskFlowToBeRun& this_command_task_flow =
        task_flow_submit_during_lockdown_.back();

    return ResultS<RenderFuture>{
        this, this_command_task_flow.command_task_flow_.task_flow_ID_,
        this_command_task_flow.external_info_.state_manager_};
  } else {
    std::lock_guard<std::mutex> guard{wait_task_flow_queue_mutex_};
    wait_task_flow_queue_.emplace_back(std::move(command_task_flow));

    if (!processing_task_flow_queue_) {
      TaskSystem::Taskflow task_flow;
      task_flow.emplace([this_object = this]() { this_object->ProcessTask(); });
      MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, task_flow);
      processing_task_flow_queue_ = true;
    }

    return ResultS<RenderFuture>{
        this, wait_task_flow_queue_.back().command_task_flow_.task_flow_ID_,
        wait_task_flow_queue_.back().external_info_.state_manager_};
  }
}

MM::Result<MM::RenderSystem::RenderFutureState, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::RunAndWait(
    CommandTaskFlow&& command_task_flow) {
  Result<RenderFuture, ErrorResult> run_result =
      Run(std::move(command_task_flow));
  run_result.Exception(MM_ERROR_DESCRIPTION("Failed to run command flow."));
  if (run_result.IsError()) {
    return ResultE<>{run_result.GetError().GetErrorCode()};
  }

  return ResultS{run_result.GetResult().Wait()};
}

bool MM::RenderSystem::CommandExecutor::IsValid() const {
  return render_engine_ != nullptr && render_engine_->IsValid();
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

  while (!free_semaphores_.empty()) {
    VkSemaphore pop_object = free_semaphores_.top();
    vkDestroySemaphore(render_engine_->GetDevice(), pop_object, nullptr);
    free_semaphores_.pop();
  }
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::InitCommandPolls(
    std::vector<VkCommandPool>& graph_command_pools,
    std::vector<VkCommandPool>& compute_command_pools,
    std::vector<VkCommandPool>& transform_command_pools) {
  VkCommandPoolCreateInfo command_buffer_create_info =
      GetCommandPoolCreateInfo(render_engine_->GetGraphQueueIndex());

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
      GetCommandBufferAllocateInfo(graph_command_pools[0], 1);

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
  const VkSemaphoreCreateInfo semaphore_create_info = GetSemaphoreCreateInfo();
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

    free_semaphores_.push(new_semaphore);
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
      GetCommandPoolCreateInfo(command_family_index);
  VkCommandBufferAllocateInfo command_buffer_allocate_info =
      GetCommandBufferAllocateInfo(nullptr);
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
                result = VkResultToMMErrorCode(MM_VK_RESULT_CODE);
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
                result = VkResultToMMErrorCode(MM_VK_RESULT_CODE);
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
    CommandTaskFlowToBeRun(CommandTaskFlow&& command_task_flow)
    : command_task_flow_(std::move(command_task_flow)),
      external_info_{std::make_shared<RenderFuture::RenderFutureStateManager>(),
                     0, 0, 0, 0} {
  for (CommandTask& command_task : command_task_flow.tasks_) {
    if (command_task.pre_tasks_.empty()) {
      switch (command_task.GetCommandType()) {
        case CommandType::GRAPH:
          external_info_.current_need_graph_command_buffer_count_ +=
              command_task
                  .GetRequireCommandBufferNumberIncludeSubCommandTasks();
          break;
        case CommandType::TRANSFORM:
          external_info_.current_need_transform_command_buffer_count_ +=
              command_task
                  .GetRequireCommandBufferNumberIncludeSubCommandTasks();
          break;
        case CommandType::COMPUTE:
          external_info_.current_need_compute_command_buffer_count_ +=
              command_task
                  .GetRequireCommandBufferNumberIncludeSubCommandTasks();
          break;
        default:
          assert(false);
      }
    }
  }
}

MM::RenderSystem::CommandExecutor::CommandTaskExecuting::CommandTaskExecuting(
    CommandTaskFlowExecuting* command_task_flow,
    MM::RenderSystem::CommandTask&& command_task)
    : command_task_{command_task_flow,
                    command_task.command_task_ID_,
                    command_task.command_type_,
                    std::move(command_task.commands_),
                    std::vector<CommandTaskExecuting*>{},
                    std::vector<CommandTaskExecuting*>{},
                    command_task.is_sub_task_,
                    command_task.is_async_record_,
                    std::move(command_task
                                  .cross_task_flow_sync_render_resource_IDs_)},
      external_info_{CommandTaskExecutingState::WAIT,
                     0,
                     command_task.pre_tasks_.size(),
                     std::vector<VkSemaphore>{},
                     std::vector<VkSemaphore>{},
                     std::vector<std::unique_ptr<AllocatedCommandBuffer>>{},
                     0} {
  command_task.command_task_ID_ = 0;
  command_task.task_flow_ = nullptr;
}

void MM::RenderSystem::CommandExecutor::CommandTaskExecuting::
    ComputeRequireCommandBufferCount() {
  assert(command_task_.task_flow_ != nullptr);

  if (command_task_.is_sub_task_) {
    external_info_.require_command_buffer_count_ =
        command_task_.commands_.size();
  } else {
    external_info_.require_command_buffer_count_ =
        command_task_.commands_.size();
    for (CommandTaskExecuting* sub_command_task : command_task_.sub_tasks_) {
      external_info_.require_command_buffer_count_ +=
          sub_command_task->command_task_.commands_.size();
    }
  }
}

void MM::RenderSystem::CommandExecutor::CommandTaskExecuting::
    LinkPostCommandTaskAndSubTask(
        MM::RenderSystem::CommandTask&& old_command_task) {
  command_task_.post_tasks_.reserve(old_command_task.post_tasks_.size());
  command_task_.sub_tasks_.reserve(old_command_task.sub_tasks_.size());
  for (CommandTaskID post_command_task_ID : old_command_task.post_tasks_) {
    for (CommandTaskExecuting& command_task :
         command_task_.task_flow_->command_task_flow_.command_tasks_) {
      if (command_task.command_task_.command_task_ID_ == post_command_task_ID) {
        command_task_.post_tasks_.emplace_back(&command_task);
      }
    }
  }
  for (CommandTaskID sub_command_task_ID : old_command_task.sub_tasks_) {
    for (CommandTaskExecuting& command_task :
         command_task_.task_flow_->command_task_flow_.command_tasks_) {
      if (command_task.command_task_.command_task_ID_ == sub_command_task_ID) {
        command_task_.sub_tasks_.emplace_back(&command_task);
      }
    }
  }
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowExecuting::
    ~CommandTaskFlowExecuting() {
  assert(external_info_.completed_command_task_count_ ==
         command_task_flow_.command_tasks_.size());
}

MM::RenderSystem::CommandExecutor::CommandTaskFlowExecuting::
    CommandTaskFlowExecuting(
        MM::RenderSystem::CommandExecutor* command_executor,
        MM::RenderSystem::CommandExecutor::CommandTaskFlowToBeRun&&
            command_task_flow_to_be_run)
    : command_task_flow_(), external_info_() {
  command_task_flow_.task_flow_ID_ =
      command_task_flow_to_be_run.command_task_flow_.task_flow_ID_;
  command_task_flow_.command_tasks_.reserve(
      command_task_flow_to_be_run.command_task_flow_.tasks_.size());

  external_info_.current_need_graph_command_buffer_count_ =
      command_task_flow_to_be_run.external_info_
          .current_need_graph_command_buffer_count_;
  external_info_.current_need_transform_command_buffer_count_ =
      command_task_flow_to_be_run.external_info_
          .current_need_transform_command_buffer_count_;
  external_info_.current_need_compute_command_buffer_count_ =
      command_task_flow_to_be_run.external_info_
          .current_need_compute_command_buffer_count_;
  command_task_flow_to_be_run.external_info_
      .current_need_graph_command_buffer_count_ = UINT32_MAX;
  command_task_flow_to_be_run.external_info_
      .current_need_transform_command_buffer_count_ = UINT32_MAX;
  command_task_flow_to_be_run.external_info_
      .current_need_compute_command_buffer_count_ = UINT32_MAX;

  for (auto& command_task :
       command_task_flow_to_be_run.command_task_flow_.tasks_) {
    command_task_flow_.command_tasks_.emplace_back(this,
                                                   std::move(command_task));
  }

  auto old_command_task_iter =
      command_task_flow_to_be_run.command_task_flow_.tasks_.begin();
  for (CommandTaskExecuting& command_task : command_task_flow_.command_tasks_) {
    command_task.LinkPostCommandTaskAndSubTask(
        std::move(*old_command_task_iter));
    command_task.ComputeRequireCommandBufferCount();
    ++old_command_task_iter;
  }

  external_info_.command_executor_ = command_executor;
  external_info_.state_manager_ =
      std::move(command_task_flow_to_be_run.external_info_.state_manager_);
  external_info_.completed_command_task_count_ = 0;

  CommandTaskFlowExecutingState state =
      external_info_.state_manager_->state_.exchange(
          CommandTaskFlowExecutingState ::RUNNING);
  if (state == CommandTaskFlowExecutingState::CANCELLED) {
    external_info_.state_manager_->state_.exchange(
        CommandTaskFlowExecutingState::CANCELLED);
  }
}

std::vector<VkSemaphore> MM::RenderSystem::CommandExecutor::GetSemaphore(
    std::uint32_t require_number) {
  assert(IsValid());

  if (free_semaphores_.size() < require_number) {
    const VkSemaphoreCreateInfo semaphore_create_info =
        GetSemaphoreCreateInfo();
    VkSemaphore new_semaphore;
    while (free_semaphores_.size() < require_number) {
      vkCreateSemaphore(render_engine_->GetDevice(), &semaphore_create_info,
                        nullptr, &new_semaphore);
      free_semaphores_.push(new_semaphore);
    }
  }

  std::vector<VkSemaphore> result;
  result.reserve(require_number);

  for (std::uint32_t i = 0; i < require_number; ++i) {
    result.push_back(free_semaphores_.top());
    free_semaphores_.pop();
  }

  return result;
}

void MM::RenderSystem::CommandExecutor::LockExecutor() {
  assert(IsValid());

  lock_count_.fetch_add(1, std::memory_order_acq_rel);
}

void MM::RenderSystem::CommandExecutor::UnlockExecutor() {
  assert(IsValid());

  assert(lock_count_.load(std::memory_order_acquire) != 0);
  if (lock_count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    std::lock(wait_task_flow_queue_mutex_,
              task_flow_submit_during_lockdown_mutex_);
    std::lock_guard guard1(wait_task_flow_queue_mutex_, std::adopt_lock),
        guard2(task_flow_submit_during_lockdown_mutex_, std::adopt_lock);

    wait_task_flow_queue_.splice(wait_task_flow_queue_.end(),
                                 task_flow_submit_during_lockdown_);
  }
}

bool MM::RenderSystem::CommandExecutor::IsFree() const {
  assert(IsValid());

  return !processing_task_flow_queue_;
}

std::unique_ptr<MM::RenderSystem::AllocatedCommandBuffer>
MM::RenderSystem::CommandExecutor::AcquireGeneralGraphCommandBuffer() {
  assert(IsValid());

  std::unique_lock<std::mutex> guard{
      general_command_buffers_acquire_release_mutex_};
  general_command_buffers_acquire_release_condition_variable_.wait(
      guard, [this_executor = this]() {
        return this_executor->general_command_buffers_[0][0] != nullptr ||
               this_executor->general_command_buffers_[0][1] != nullptr ||
               this_executor->general_command_buffers_[0][2] != nullptr;
      });
  if (general_command_buffers_[0][0] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[0][0].release());
  }
  if (general_command_buffers_[0][1] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[0][1].release());
  }
  if (general_command_buffers_[0][2] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[0][2].release());
  }

  assert(false &&
         "Synchronization error, command buffer should not be unreachable.");
}

std::unique_ptr<MM::RenderSystem::AllocatedCommandBuffer>
MM::RenderSystem::CommandExecutor::AcquireGeneralComputeCommandBuffer() {
  assert(IsValid());

  std::unique_lock<std::mutex> guard{
      general_command_buffers_acquire_release_mutex_};
  general_command_buffers_acquire_release_condition_variable_.wait(
      guard, [this_executor = this]() {
        return this_executor->general_command_buffers_[1][0] != nullptr ||
               this_executor->general_command_buffers_[1][1] != nullptr ||
               this_executor->general_command_buffers_[1][2] != nullptr;
      });
  if (general_command_buffers_[1][0] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[1][0].release());
  }
  if (general_command_buffers_[1][1] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[1][1].release());
  }
  if (general_command_buffers_[1][2] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[1][2].release());
  }

  assert(false &&
         "Synchronization error, command buffer should not be unreachable.");
}

std::unique_ptr<MM::RenderSystem::AllocatedCommandBuffer>
MM::RenderSystem::CommandExecutor::AcquireGeneralTransformCommandBuffer() {
  assert(IsValid());

  std::unique_lock<std::mutex> guard{
      general_command_buffers_acquire_release_mutex_};
  general_command_buffers_acquire_release_condition_variable_.wait(
      guard, [this_executor = this]() {
        return this_executor->general_command_buffers_[2][0] != nullptr ||
               this_executor->general_command_buffers_[2][1] != nullptr ||
               this_executor->general_command_buffers_[2][2] != nullptr;
      });
  if (general_command_buffers_[2][0] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[2][0].release());
  }
  if (general_command_buffers_[2][1] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[2][1].release());
  }
  if (general_command_buffers_[2][2] != nullptr) {
    return std::unique_ptr<AllocatedCommandBuffer>(
        general_command_buffers_[2][2].release());
  }

  assert(false &&
         "Synchronization error, command buffer should not be unreachable.");
}

void MM::RenderSystem::CommandExecutor::ReleaseGeneralCommandBuffer(
    std::unique_ptr<AllocatedCommandBuffer>&& output) {
  assert(IsValid());

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
    general_command_buffers_acquire_release_condition_variable_.notify_all();
    return;
  }
  if (general_command_buffers_[general_command_type_index][1] == nullptr) {
    output->ResetCommandBuffer();
    output->ResetFence();
    general_command_buffers_[general_command_type_index][1] = std::move(output);
    general_command_buffers_acquire_release_condition_variable_.notify_all();
    return;
  }
  if (general_command_buffers_[general_command_type_index][2] != nullptr) {
    output->ResetCommandBuffer();
    output->ResetFence();
    general_command_buffers_[general_command_type_index][2] = std::move(output);
    general_command_buffers_acquire_release_condition_variable_.notify_all();
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
        general_command_buffer_ =
            command_executor_->AcquireGeneralGraphCommandBuffer();
        return;
      case CommandBufferType::COMPUTE:
        general_command_buffer_ =
            command_executor_->AcquireGeneralComputeCommandBuffer();
        return;
      case CommandBufferType::TRANSFORM:
        general_command_buffer_ =
            command_executor_->AcquireGeneralTransformCommandBuffer();
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

bool MM::RenderSystem::CommandExecutor::ExecutingCommandTaskIsComplete(
    const CommandTaskExecuting& command_task) const {
  VkResult result = vkGetFenceStatus(
      render_engine_->GetDevice(),
      command_task.external_info_.command_buffers_[0]->GetFence());
  assert(result != VK_ERROR_DEVICE_LOST);

  return result == VK_SUCCESS;
}

bool MM::RenderSystem::CommandExecutor::WaitCroosCommandTaskFlowSync(
    const std::vector<RenderResourceDataID>&
        cross_task_flow_sync_render_resource_IDs) {
  for (const RenderResourceDataID& render_resource_data_ID :
       cross_task_flow_sync_render_resource_IDs) {
    if (block_render_resources_.count(render_resource_data_ID)) {
      return true;
    }
  }

  return false;
}

std::stack<std::unique_ptr<MM::RenderSystem::AllocatedCommandBuffer>>&
MM::RenderSystem::CommandExecutor::GetFreeCommandBufferStack(
    CommandBufferType command_buffer_type) {
  switch (command_buffer_type) {
    case CommandBufferType::GRAPH:
      return free_graph_command_buffers_;
    case CommandBufferType::COMPUTE:
      return free_compute_command_buffers_;
    case CommandBufferType::TRANSFORM:
      return free_transform_command_buffers_;
    case CommandBufferType::UNDEFINED:
      assert(false);
  }
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::SubmitCommandTask(
    CommandTaskExecuting& command_task) {
  if (command_task.command_task_.sub_tasks_.empty()) {
    std::vector<VkCommandBuffer> origin_command_buffer{};
    origin_command_buffer.reserve(
        command_task.external_info_.command_buffers_.size());
    for (std::unique_ptr<AllocatedCommandBuffer>& allocated_command_buffer :
         command_task.external_info_.command_buffers_) {
      origin_command_buffer.emplace_back(
          allocated_command_buffer->GetCommandBuffer());
    }

    const VkSubmitInfo submit_info = GetVkSubmitInfo(
        nullptr, command_task.external_info_.wait_semaphore_.size(),
        command_task.external_info_.wait_semaphore_.data(),
        wait_dst_stage_mask_.data(),
        command_task.external_info_.command_buffers_.size(),
        origin_command_buffer.data(),
        command_task.external_info_.signal_semaphore_.size(),
        command_task.external_info_.signal_semaphore_.data());
    const MM::ErrorCode result_code = VkResultToMMErrorCode(vkQueueSubmit(
        render_engine_->GetQueue(command_task.command_task_.command_type_), 1,
        &submit_info,
        command_task.external_info_.command_buffers_[0]->GetFence()));
    if (result_code != ErrorCode::SUCCESS) {
      command_task.external_info_.state_.store(
          CommandTaskExecutingState::FAILED);
      command_task.command_task_.task_flow_->external_info_.state_manager_
          ->SetState(CommandTaskFlowExecutingState::FAILED);
      return ResultE<>{result_code};
    }

    command_task.external_info_.state_.store(
        CommandTaskExecutingState::RUNNING);
    return ResultS<Nil>{};
  } else {
    std::uint32_t require_command_buffer_count =
        command_task.external_info_.command_buffers_.size();
    for (CommandTaskExecuting* sub_command_task :
         command_task.command_task_.sub_tasks_) {
      require_command_buffer_count +=
          sub_command_task->external_info_.command_buffers_.size();
    }
    std::vector<VkCommandBuffer> origin_command_buffer{};
    origin_command_buffer.reserve(require_command_buffer_count);
    std::vector<VkSubmitInfo> submit_infoes{};
    submit_infoes.reserve(1 + command_task.command_task_.sub_tasks_.size());
    for (std::unique_ptr<AllocatedCommandBuffer>& allocated_command_buffer :
         command_task.external_info_.command_buffers_) {
      origin_command_buffer.emplace_back(
          allocated_command_buffer->GetCommandBuffer());
    }
    submit_infoes.emplace_back(GetVkSubmitInfo(
        nullptr, command_task.external_info_.wait_semaphore_.size(),
        command_task.external_info_.wait_semaphore_.data(),
        wait_dst_stage_mask_.data(),
        command_task.external_info_.command_buffers_.size(),
        origin_command_buffer.data(),
        command_task.external_info_.signal_semaphore_.size(),
        command_task.external_info_.signal_semaphore_.data()));
    std::uint32_t command_buffer_offset =
        command_task.external_info_.command_buffers_.size();
    for (CommandTaskExecuting* sub_command_task :
         command_task.command_task_.sub_tasks_) {
      for (std::unique_ptr<AllocatedCommandBuffer>& allocated_command_buffer :
           command_task.external_info_.command_buffers_) {
        origin_command_buffer.emplace_back(
            allocated_command_buffer->GetCommandBuffer());
      }
      submit_infoes.emplace_back(GetVkSubmitInfo(
          nullptr, sub_command_task->external_info_.wait_semaphore_.size(),
          sub_command_task->external_info_.wait_semaphore_.data(),
          wait_dst_stage_mask_.data(),
          sub_command_task->external_info_.command_buffers_.size(),
          origin_command_buffer.data() + command_buffer_offset,
          sub_command_task->external_info_.signal_semaphore_.size(),
          sub_command_task->external_info_.signal_semaphore_.data()));
      command_buffer_offset +=
          sub_command_task->external_info_.command_buffers_.size();
    }

    const MM::ErrorCode result_code = VkResultToMMErrorCode(vkQueueSubmit(
        render_engine_->GetQueue(command_task.command_task_.command_type_),
        submit_infoes.size(), submit_infoes.data(),
        command_task.external_info_.command_buffers_[0]->GetFence()));
    if (result_code != ErrorCode::SUCCESS) {
      command_task.external_info_.state_.store(
          CommandTaskExecutingState::FAILED);
      command_task.command_task_.task_flow_->external_info_.state_manager_
          ->SetState(CommandTaskFlowExecutingState::FAILED);
      return ResultE<>{result_code};
    }

    command_task.external_info_.state_.store(
        CommandTaskExecutingState::RUNNING);
    return ResultS<Nil>{};
  }
}

/*-------------------------------------------------------------------------------------*/

void MM::RenderSystem::CommandExecutor::ProcessTask() {
  do {
    ProcessCompleteTask();
    ProcessExecutingFailedOrCancelled();
    ProcessWaitTaskFlow();
    std::array<std::uint32_t, 3> curent_need_command_buffer_count =
        ProcessCurrentNeedCommandBufferCount();
    ProcessWaitCommandTaskFlowQueue(curent_need_command_buffer_count);
    ProcessExecutingCommandTaskFlowQueue();

    std::this_thread::yield();
  } while (HaveCommandTaskToBeProcess());

  processing_task_flow_queue_ = false;
}

void MM::RenderSystem::CommandExecutor::ProcessCompleteTask() {
  std::unique_lock<std::mutex> recycled_command_buffer_guard{
      submit_failed_to_be_recovery_command_buffer_mutex_};
  for (auto& command_buffer : submit_failed_to_be_recovery_command_buffer_) {
    command_buffer->ResetCommandBuffer();
    if (command_buffer->GetCommandBufferType() == CommandBufferType::GRAPH) {
      free_graph_command_buffers_.push(std::move(command_buffer));
    } else if (command_buffer->GetCommandBufferType() ==
               CommandBufferType::COMPUTE) {
      free_compute_command_buffers_.push(std::move(command_buffer));
    } else if (command_buffer->GetCommandBufferType() ==
               CommandBufferType::TRANSFORM) {
      free_transform_command_buffers_.push(std::move(command_buffer));
    }
  }
  submit_failed_to_be_recovery_command_buffer_.clear();
  recycled_command_buffer_guard.unlock();

  for (auto command_task_flow_iter = executing_command_task_flow_queue_.begin();
       command_task_flow_iter != executing_command_task_flow_queue_.end();
       ++command_task_flow_iter) {
    CommandTaskFlowExecuting& executing_task_flow = *command_task_flow_iter;
    for (CommandTaskExecuting& executing_command_task :
         executing_task_flow.command_task_flow_.command_tasks_) {
      CommandTaskExecutingState command_task_running_state =
          executing_command_task.external_info_.state_.load(
              std::memory_order_acquire);
      if (command_task_running_state == CommandTaskExecutingState::RUNNING &&
          !executing_command_task.command_task_.is_sub_task_ &&
          ExecutingCommandTaskIsComplete(executing_command_task)) {
        // update post command task \ref pre_command_task_not_submit_count_
        for (CommandTaskExecuting* post_command_task :
             executing_command_task.command_task_.post_tasks_) {
          std::uint32_t old_count =
              post_command_task->external_info_
                  .pre_command_task_not_submit_count_.fetch_sub(
                      std::memory_order_acq_rel);
          assert(old_count != 0);
        }
        for (CommandTaskExecuting* sub_command_task :
             executing_command_task.command_task_.sub_tasks_) {
          // update post command task \ref pre_command_task_not_submit_count_
          for (CommandTaskExecuting* post_command_task :
               sub_command_task->command_task_.post_tasks_) {
            std::uint32_t old_count =
                post_command_task->external_info_
                    .pre_command_task_not_submit_count_.fetch_sub(
                        std::memory_order_acq_rel);
            assert(old_count != 0);
          }
        }

        // recovery wait semaphore
        for (VkSemaphore wait_semaphore :
             executing_command_task.external_info_.wait_semaphore_) {
          free_semaphores_.push(wait_semaphore);
        }
        executing_command_task.external_info_.wait_semaphore_.clear();
        for (CommandTaskExecuting* sub_command_task :
             executing_command_task.command_task_.sub_tasks_) {
          for (VkSemaphore wait_semaphore :
               sub_command_task->external_info_.wait_semaphore_) {
            free_semaphores_.push(wait_semaphore);
          }
          sub_command_task->external_info_.wait_semaphore_.clear();
        }

        // recovery AllocatedCommandBuffer
        assert(executing_command_task.command_task_.command_type_ !=
               CommandType::UNDEFINED);
        if (executing_command_task.command_task_.command_type_ ==
            CommandType::GRAPH) {
          for (std::unique_ptr<AllocatedCommandBuffer>&
                   allocated_command_buffer :
               executing_command_task.external_info_.command_buffers_) {
            allocated_command_buffer->ResetFence();
            allocated_command_buffer->ResetCommandBuffer();
            free_graph_command_buffers_.push(
                std::move(allocated_command_buffer));
          }
        } else if (executing_command_task.command_task_.command_type_ ==
                   CommandType::TRANSFORM) {
          for (std::unique_ptr<AllocatedCommandBuffer>&
                   allocated_command_buffer :
               executing_command_task.external_info_.command_buffers_) {
            allocated_command_buffer->ResetFence();
            allocated_command_buffer->ResetCommandBuffer();
            free_transform_command_buffers_.push(
                std::move(allocated_command_buffer));
          }
        } else if (executing_command_task.command_task_.command_type_ ==
                   CommandType::COMPUTE) {
          for (std::unique_ptr<AllocatedCommandBuffer>&
                   allocated_command_buffer :
               executing_command_task.external_info_.command_buffers_) {
            allocated_command_buffer->ResetFence();
            allocated_command_buffer->ResetCommandBuffer();
            free_compute_command_buffers_.push(
                std::move(allocated_command_buffer));
          }
        }
        executing_command_task.external_info_.command_buffers_.clear();

        for (CommandTaskExecuting* sub_command_task :
             executing_command_task.command_task_.sub_tasks_) {
          assert(sub_command_task->command_task_.command_type_ !=
                 CommandType::UNDEFINED);
          if (sub_command_task->command_task_.command_type_ ==
              CommandType::GRAPH) {
            for (std::unique_ptr<AllocatedCommandBuffer>&
                     allocated_command_buffer :
                 sub_command_task->external_info_.command_buffers_) {
              allocated_command_buffer->ResetFence();
              allocated_command_buffer->ResetCommandBuffer();
              free_graph_command_buffers_.push(
                  std::move(allocated_command_buffer));
            }
          } else if (sub_command_task->command_task_.command_type_ ==
                     CommandType::TRANSFORM) {
            for (std::unique_ptr<AllocatedCommandBuffer>&
                     allocated_command_buffer :
                 sub_command_task->external_info_.command_buffers_) {
              allocated_command_buffer->ResetFence();
              allocated_command_buffer->ResetCommandBuffer();
              free_transform_command_buffers_.push(
                  std::move(allocated_command_buffer));
            }
          } else if (sub_command_task->command_task_.command_type_ ==
                     CommandType::COMPUTE) {
            for (std::unique_ptr<AllocatedCommandBuffer>&
                     allocated_command_buffer :
                 sub_command_task->external_info_.command_buffers_) {
              allocated_command_buffer->ResetFence();
              allocated_command_buffer->ResetCommandBuffer();
              free_compute_command_buffers_.push(
                  std::move(allocated_command_buffer));
            }
          }

          sub_command_task->external_info_.command_buffers_.clear();
        }

        // remvoe block resource
        for (RenderResourceDataID block_resource_ID :
             executing_command_task.command_task_
                 .cross_task_flow_sync_render_resource_IDs_) {
          auto iter = block_render_resources_.find(block_resource_ID);
          assert(iter != block_render_resources_.end());
          for (auto command_task_ID_iter = iter->second.begin();
               command_task_ID_iter != iter->second.end(); ++iter) {
            if (*command_task_ID_iter ==
                executing_command_task.command_task_.command_task_ID_) {
              iter->second.erase(command_task_ID_iter);
              break;
            }
          }
          if (iter->second.empty()) {
            block_render_resources_.erase(iter);
          }
        }
        executing_command_task.command_task_
            .cross_task_flow_sync_render_resource_IDs_.clear();
        for (CommandTaskExecuting* sub_command_task :
             executing_command_task.command_task_.sub_tasks_) {
          for (RenderResourceDataID block_resource_ID :
               sub_command_task->command_task_
                   .cross_task_flow_sync_render_resource_IDs_) {
            auto iter = block_render_resources_.find(block_resource_ID);
            assert(iter != block_render_resources_.end());
            for (auto command_task_ID_iter = iter->second.begin();
                 command_task_ID_iter != iter->second.end(); ++iter) {
              if (*command_task_ID_iter ==
                  sub_command_task->command_task_.command_task_ID_) {
                iter->second.erase(command_task_ID_iter);
                break;
              }
            }
            if (iter->second.empty()) {
              block_render_resources_.erase(iter);
            }
          }
          sub_command_task->command_task_
              .cross_task_flow_sync_render_resource_IDs_.clear();
        }

        // mark this is completed
        executing_command_task.external_info_.state_.store(
            CommandTaskExecutingState::COMPLETED, std::memory_order_relaxed);
        for (CommandTaskExecuting* sub_command_task :
             executing_command_task.command_task_.sub_tasks_) {
          sub_command_task->external_info_.state_.store(
              CommandTaskExecutingState::COMPLETED, std::memory_order_relaxed);
        }
        executing_task_flow.external_info_.completed_command_task_count_ +=
            1 + executing_command_task.command_task_.sub_tasks_.size();
      }
    }

    // remove this task flow when completed
    if (executing_task_flow.external_info_.completed_command_task_count_ ==
        executing_task_flow.command_task_flow_.command_tasks_.size()) {
      executing_task_flow.external_info_.state_manager_->SetState(
          CommandTaskFlowExecutingState::SUCCESS);
      executing_task_flow.external_info_.state_manager_->Notify();
      executing_command_task_flow_queue_.erase(command_task_flow_iter);
    }
  }
}

void MM::RenderSystem::CommandExecutor::ProcessExecutingFailedOrCancelled() {
  for (auto command_task_flow_iter = executing_command_task_flow_queue_.begin();
       command_task_flow_iter != executing_command_task_flow_queue_.end();
       ++command_task_flow_iter) {
    CommandTaskFlowExecuting& executing_task_flow = *command_task_flow_iter;
    CommandTaskFlowExecutingState command_task_flow_executing_state =
        executing_task_flow.external_info_.state_manager_->GetState();
    if (command_task_flow_executing_state ==
            CommandTaskFlowExecutingState::FAILED ||
        command_task_flow_executing_state ==
            CommandTaskFlowExecutingState::CANCELLED) {
      // Wait for all asynchronous tasks to complete before starting
      // recycling.
      bool have_command_task_recording_or_running = false;
      for (CommandTaskExecuting& command_task :
           executing_task_flow.command_task_flow_.command_tasks_) {
        CommandTaskExecutingState state =
            command_task.external_info_.state_.load(std::memory_order_acquire);
        if (state == CommandTaskExecutingState::RECORDING ||
            state == CommandTaskExecutingState::RUNNING) {
          have_command_task_recording_or_running = true;
          break;
        }
      }
      if (have_command_task_recording_or_running) {
        break;
      }

      for (CommandTaskExecuting& command_task :
           executing_task_flow.command_task_flow_.command_tasks_) {
        // destruct VkSemaphore
        for (VkSemaphore signal_semaphore :
             command_task.external_info_.signal_semaphore_) {
          vkDestroySemaphore(render_engine_->GetDevice(), signal_semaphore,
                             nullptr);
        }

        // recovery AllocatedCommandBuffer
        assert(command_task.command_task_.command_type_ !=
               CommandType::UNDEFINED);
        if (command_task.command_task_.command_type_ == CommandType::GRAPH) {
          for (std::unique_ptr<AllocatedCommandBuffer>&
                   allocated_command_buffer :
               command_task.external_info_.command_buffers_) {
            allocated_command_buffer->ResetFence();
            allocated_command_buffer->ResetCommandBuffer();
            free_graph_command_buffers_.push(
                std::move(allocated_command_buffer));
          }
        } else if (command_task.command_task_.command_type_ ==
                   CommandType::TRANSFORM) {
          for (std::unique_ptr<AllocatedCommandBuffer>&
                   allocated_command_buffer :
               command_task.external_info_.command_buffers_) {
            allocated_command_buffer->ResetFence();
            allocated_command_buffer->ResetCommandBuffer();
            free_transform_command_buffers_.push(
                std::move(allocated_command_buffer));
          }
        } else if (command_task.command_task_.command_type_ ==
                   CommandType::COMPUTE) {
          for (std::unique_ptr<AllocatedCommandBuffer>&
                   allocated_command_buffer :
               command_task.external_info_.command_buffers_) {
            allocated_command_buffer->ResetFence();
            allocated_command_buffer->ResetCommandBuffer();
            free_compute_command_buffers_.push(
                std::move(allocated_command_buffer));
          }
        }

        // remvoe block resource
        for (RenderResourceDataID block_resource_ID :
             command_task.command_task_
                 .cross_task_flow_sync_render_resource_IDs_) {
          auto iter = block_render_resources_.find(block_resource_ID);
          assert(iter != block_render_resources_.end());
          for (auto command_task_ID_iter = iter->second.begin();
               command_task_ID_iter != iter->second.end(); ++iter) {
            if (*command_task_ID_iter ==
                command_task.command_task_.command_task_ID_) {
              iter->second.erase(command_task_ID_iter);
              break;
            }
          }
          if (iter->second.empty()) {
            block_render_resources_.erase(iter);
          }
        }
      }

      // remove this task flow when failed or cancelled
      if (executing_task_flow.external_info_.completed_command_task_count_ ==
          executing_task_flow.command_task_flow_.command_tasks_.size()) {
        executing_task_flow.external_info_.state_manager_->Notify();
        executing_command_task_flow_queue_.erase(command_task_flow_iter);
      }
    }
  }
}

void MM::RenderSystem::CommandExecutor::ProcessWaitTaskFlow() {
  {
    std::unique_lock<std::mutex> wait_task_flow_queue_guard{
        wait_task_flow_queue_mutex_};
    for (CommandTaskFlowID wait_task_flow_ID : need_wait_task_flow_IDs_) {
      for (WaitCommandTaskFlowQueueType::iterator wait_command_task_flow =
               wait_task_flow_queue_.begin();
           wait_command_task_flow != wait_task_flow_queue_.end();
           ++wait_command_task_flow) {
        if (wait_task_flow_ID ==
            wait_command_task_flow->command_task_flow_.task_flow_ID_) {
          need_wait_task_flow_queue_.emplace_back(
              std::move(*wait_command_task_flow));
          wait_command_task_flow =
              wait_task_flow_queue_.erase(wait_command_task_flow);
          break;
        }
      }
    }

    need_wait_task_flow_IDs_.clear();
  }

  while (!need_wait_task_flow_queue_.empty()) {
    executing_command_task_flow_queue_.emplace_back(
        this, std::move(need_wait_task_flow_queue_.front()));
    need_wait_task_flow_queue_.erase(need_wait_task_flow_queue_.begin());
  }
}

std::array<std::uint32_t, 3>
MM::RenderSystem::CommandExecutor::ProcessCurrentNeedCommandBufferCount() {
  std::uint32_t total_current_need_graph_command_buffer_count_ = 0;
  std::uint32_t total_current_need_compute_command_buffer_count_ = 0;
  std::uint32_t total_current_need_transform_command_buffer_count_ = 0;

  for (CommandTaskFlowExecuting& executing_command_task_flow :
       executing_command_task_flow_queue_) {
    executing_command_task_flow.external_info_
        .current_need_graph_command_buffer_count_ = 0;
    executing_command_task_flow.external_info_
        .current_need_transform_command_buffer_count_ = 0;
    executing_command_task_flow.external_info_
        .current_need_compute_command_buffer_count_ = 0;
    for (CommandTaskExecuting& executing_command_task :
         executing_command_task_flow.command_task_flow_.command_tasks_) {
      CommandTaskExecutingState state =
          executing_command_task.external_info_.state_.load(
              std::memory_order_acquire);
      if (state == CommandTaskExecutingState::WAIT &&
          executing_command_task.external_info_
                  .pre_command_task_not_submit_count_ == 0 &&
          WaitCroosCommandTaskFlowSync(
              executing_command_task.command_task_
                  .cross_task_flow_sync_render_resource_IDs_)) {
        switch (executing_command_task.command_task_.command_type_) {
          case CommandType::GRAPH:
            total_current_need_graph_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            executing_command_task_flow.external_info_
                .current_need_graph_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            break;
          case CommandType::TRANSFORM:
            total_current_need_transform_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            executing_command_task_flow.external_info_
                .current_need_transform_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            break;
          case CommandType::COMPUTE:
            total_current_need_compute_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            executing_command_task_flow.external_info_
                .current_need_compute_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            break;
          case CommandType::UNDEFINED:
            assert(false);
        }
      } else if (state == CommandTaskExecutingState::RUNNING) {
        switch (executing_command_task.command_task_.command_type_) {
          case CommandType::GRAPH:
            total_current_need_graph_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            executing_command_task_flow.external_info_
                .current_need_graph_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            break;
          case CommandType::TRANSFORM:
            total_current_need_transform_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            executing_command_task_flow.external_info_
                .current_need_transform_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            break;
          case CommandType::COMPUTE:
            total_current_need_compute_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            executing_command_task_flow.external_info_
                .current_need_compute_command_buffer_count_ +=
                executing_command_task.command_task_.commands_.size();
            break;
          case CommandType::UNDEFINED:
            assert(false);
        }
      }
    }
  }

  return std::array<std::uint32_t, 3>{
      total_current_need_graph_command_buffer_count_,
      total_current_need_transform_command_buffer_count_,
      total_current_need_compute_command_buffer_count_};
}

void MM::RenderSystem::CommandExecutor::ProcessWaitCommandTaskFlowQueue(
    std::array<std::uint32_t, 3> total_current_need_command_buffer_count) {
  for (auto wait_command_task_flow_iter = wait_task_flow_queue_.begin();
       wait_command_task_flow_iter != wait_task_flow_queue_.end();) {
    std::uint32_t free_graph_command_buffer_count =
        total_current_need_command_buffer_count[0] >
                free_graph_command_buffers_.size()
            ? 0
            : free_graph_command_buffers_.size() -
                  total_current_need_command_buffer_count[0];
    std::uint32_t free_transform_command_buffer_count =
        total_current_need_command_buffer_count[1] >
                free_transform_command_buffers_.size()
            ? 0
            : free_transform_command_buffers_.size() -
                  total_current_need_command_buffer_count[1];
    std::uint32_t free_compute_command_buffer_count =
        total_current_need_command_buffer_count[2] >
                free_compute_command_buffers_.size()
            ? 0
            : free_compute_command_buffers_.size() -
                  total_current_need_command_buffer_count[2];

    CommandTaskFlowToBeRun::ExternalInfoType& external_info =
        wait_command_task_flow_iter->external_info_;
    if (external_info.current_need_graph_command_buffer_count_ <=
            free_graph_command_buffer_count &&
        external_info.current_need_transform_command_buffer_count_ <=
            free_transform_command_buffer_count &&
        external_info.current_need_compute_command_buffer_count_ <=
            free_compute_command_buffer_count) {
      free_graph_command_buffer_count -=
          external_info.current_need_graph_command_buffer_count_;
      free_transform_command_buffer_count -=
          external_info.current_need_transform_command_buffer_count_;
      free_compute_command_buffer_count -=
          external_info.current_need_compute_command_buffer_count_;

      executing_command_task_flow_queue_.emplace_back(
          this, std::move(*wait_command_task_flow_iter));
      wait_command_task_flow_iter =
          wait_task_flow_queue_.erase(wait_command_task_flow_iter);
      continue;
    } else if (std::uint32_t addition_num =
                   external_info.reject_number_ /
                   executing_command_task_flow_queue_.size();
               external_info.current_need_graph_command_buffer_count_ <=
                   free_graph_command_buffer_count + addition_num &&
               external_info.current_need_transform_command_buffer_count_ <=
                   free_transform_command_buffer_count + addition_num &&
               external_info.current_need_compute_command_buffer_count_ <=
                   free_compute_command_buffer_count + addition_num) {
    } else {
      if (external_info.reject_number_ < 15) {
        external_info.reject_number_ += 1;
      }
    }

    ++wait_command_task_flow_iter;
  }
}

void MM::RenderSystem::CommandExecutor::ProcessExecutingCommandTaskFlowQueue() {
  for (CommandTaskFlowExecuting& executing_command_task_flow :
       executing_command_task_flow_queue_) {
    for (CommandTaskExecuting& executing_command_task :
         executing_command_task_flow.command_task_flow_.command_tasks_) {
      if (GetFreeGraphCommandBufferNumber() +
              GetFreeTransformCommandBufferNumber() +
              GetFreeComputeCommandBufferNumber() ==
          0) {
        break;
      }

      if (!executing_command_task.command_task_.is_sub_task_ &&
          executing_command_task.external_info_.state_.load(
              std::memory_order_acquire) == CommandTaskExecutingState::WAIT) {
        if (executing_command_task.external_info_
                .require_command_buffer_count_ <
            GetFreeCommandNumber(
                executing_command_task.command_task_.command_type_)) {
          if (!WaitCroosCommandTaskFlowSync(
                  executing_command_task.command_task_
                      .cross_task_flow_sync_render_resource_IDs_)) {
            std::stack<std::unique_ptr<AllocatedCommandBuffer>>
                free_command_buffer = GetFreeCommandBufferStack(
                    executing_command_task.command_task_.command_type_);

            executing_command_task.external_info_
                .require_command_buffer_count_ -= free_command_buffer.size();

            // get command buffers
            while (
                executing_command_task.external_info_.command_buffers_.size() !=
                executing_command_task.command_task_.commands_.size()) {
              assert(!free_command_buffer.empty());
              executing_command_task.external_info_.command_buffers_
                  .emplace_back(std::move(free_command_buffer.top()));
              free_command_buffer.pop();
            }

            for (CommandTaskExecuting* sub_command_task :
                 executing_command_task.command_task_.sub_tasks_) {
              while (sub_command_task->external_info_.command_buffers_.size() !=
                     sub_command_task->command_task_.commands_.size()) {
                assert(!free_command_buffer.empty());
                sub_command_task->external_info_.command_buffers_.emplace_back(
                    std::move(free_command_buffer.top()));
                free_command_buffer.pop();
              }
            }

            // get semaphores
            executing_command_task.external_info_.signal_semaphore_ =
                std::move(GetSemaphore(
                    executing_command_task.command_task_.post_tasks_.size()));
            for (std::uint32_t post_task_index = 0;
                 post_task_index !=
                 executing_command_task.command_task_.post_tasks_.size();
                 ++post_task_index) {
              executing_command_task.command_task_.post_tasks_[post_task_index]
                  ->external_info_.wait_semaphore_.emplace_back(
                      executing_command_task.external_info_
                          .signal_semaphore_[post_task_index]);
            }

            for (CommandTaskExecuting* sub_command_task :
                 executing_command_task.command_task_.sub_tasks_) {
              sub_command_task->external_info_.signal_semaphore_ =
                  std::move(GetSemaphore(
                      sub_command_task->command_task_.post_tasks_.size()));
              for (std::uint32_t post_tasks_index = 0;
                   post_tasks_index !=
                   sub_command_task->command_task_.post_tasks_.size();
                   ++post_tasks_index) {
                sub_command_task->command_task_.post_tasks_[post_tasks_index]
                    ->external_info_.wait_semaphore_.emplace_back(
                        sub_command_task->external_info_
                            .signal_semaphore_[post_tasks_index]);
              }
            }

            // update cross command flow sync resource
            for (const RenderResourceDataID& render_resource_data_ID :
                 executing_command_task.command_task_
                     .cross_task_flow_sync_render_resource_IDs_) {
              if (auto iter =
                      block_render_resources_.find(render_resource_data_ID);
                  iter == block_render_resources_.end()) {
                block_render_resources_.emplace(
                    render_resource_data_ID,
                    std::list<CommandTaskID>{
                        executing_command_task.command_task_.command_task_ID_});
              } else {
                iter->second.emplace_back(
                    executing_command_task.command_task_.command_task_ID_);
              }
            }

            for (CommandTaskExecuting* sub_command_task :
                 executing_command_task.command_task_.sub_tasks_) {
              for (const RenderResourceDataID& render_resource_data_ID :
                   sub_command_task->command_task_
                       .cross_task_flow_sync_render_resource_IDs_) {
                if (auto iter =
                        block_render_resources_.find(render_resource_data_ID);
                    iter == block_render_resources_.end()) {
                  block_render_resources_.emplace(
                      render_resource_data_ID,
                      std::list<CommandTaskID>{
                          sub_command_task->command_task_.command_task_ID_});
                } else {
                  iter->second.emplace_back(
                      sub_command_task->command_task_.command_task_ID_);
                }
              }
            }

            // Determine whether to submit asynchronously
            bool async_submit =
                executing_command_task.command_task_.is_async_record_;
            if (!async_submit) {
              for (CommandTaskExecuting* sub_command_task :
                   executing_command_task.command_task_.sub_tasks_) {
                async_submit |=
                    sub_command_task->command_task_.is_async_record_;
              }
            }

            // submit command tasks
            if (async_submit) {
              RecordAndSubmitCommandASync(executing_command_task);
            } else {
              RecordAndSubmitCommandSync(executing_command_task)
                  .Exception(MM_ERROR_DESCRIPTION(
                      "Failed to record and submit command."));
            }
          }
        } else {
          if (executing_command_task.external_info_.wait_free_buffer_count_ >
              3 * executing_command_task.external_info_
                      .require_command_buffer_count_) {
          } else {
            executing_command_task.external_info_.wait_free_buffer_count_ +=
                GetFreeCommandNumber(
                    executing_command_task.command_task_.command_type_);
            if (executing_command_task.external_info_.wait_free_buffer_count_ >
                wait_coefficient_ * executing_command_task.external_info_
                                        .require_command_buffer_count_) {
              std::stack<std::unique_ptr<AllocatedCommandBuffer>>
                  free_command_buffer = GetFreeCommandBufferStack(
                      executing_command_task.command_task_.command_type_);

              executing_command_task.external_info_
                  .require_command_buffer_count_ -= free_command_buffer.size();

              // get command buffers
              while (
                  !free_command_buffer.empty() &&
                  executing_command_task.external_info_.command_buffers_
                          .size() !=
                      executing_command_task.command_task_.commands_.size()) {
                executing_command_task.external_info_.command_buffers_
                    .emplace_back(std::move(free_command_buffer.top()));
                free_command_buffer.pop();
              }

              for (CommandTaskExecuting* sub_command_task :
                   executing_command_task.command_task_.sub_tasks_) {
                if (free_command_buffer.empty()) {
                  break;
                }
                while (
                    !free_command_buffer.empty() &&
                    sub_command_task->external_info_.command_buffers_.size() !=
                        sub_command_task->command_task_.commands_.size()) {
                  sub_command_task->external_info_.command_buffers_
                      .emplace_back(std::move(free_command_buffer.top()));
                  free_command_buffer.pop();
                }
              }
            }
          }
        }
      }
    }
  }
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::RenderSystem::CommandExecutor::RecordAndSubmitCommandSync(
    CommandTaskExecuting& command_task) {
  command_task.external_info_.state_.store(CommandTaskExecutingState::RECORDING,
                                           std::memory_order_release);
  for (std::uint32_t i = 0; i != command_task.command_task_.commands_.size();
       ++i) {
    Result<Nil, ErrorResult> record_result =
        command_task.command_task_.commands_[i](
            *command_task.external_info_.command_buffers_[i]);
    if (record_result.IsError()) {
      command_task.external_info_.state_.store(
          CommandTaskExecutingState::FAILED);
      command_task.command_task_.task_flow_->external_info_.state_manager_
          ->SetState(CommandTaskFlowExecutingState::FAILED);
      return record_result;
    }
  }

  for (CommandTaskExecuting* sub_command_task :
       command_task.command_task_.sub_tasks_) {
    for (std::uint32_t i = 0;
         i != sub_command_task->command_task_.commands_.size(); ++i) {
      Result<Nil, ErrorResult> record_result =
          sub_command_task->command_task_.commands_[i](
              *(sub_command_task->external_info_.command_buffers_[i]));
      if (record_result.IsError()) {
        command_task.external_info_.state_.store(
            CommandTaskExecutingState::FAILED);
        command_task.command_task_.task_flow_->external_info_.state_manager_
            ->SetState(CommandTaskFlowExecutingState::FAILED);
        return record_result;
      }
    }
  }

  return SubmitCommandTask(command_task);
}

void MM::RenderSystem::CommandExecutor::RecordAndSubmitCommandASync(
    CommandTaskExecuting& command_task) {
  command_task.external_info_.state_.store(CommandTaskExecutingState::RECORDING,
                                           std::memory_order_release);
  TaskSystem::Taskflow task_flow{};
  task_flow.emplace([this_executor = this, &command_task = command_task]() {
    for (std::uint32_t i = 0; i != command_task.command_task_.commands_.size();
         ++i) {
      if (command_task.command_task_
              .commands_[i](*command_task.external_info_.command_buffers_[i])
              .Exception(MM_ERROR_DESCRIPTION(
                  "Failed to record and submit command task."))
              .IsError()) {
        command_task.external_info_.state_.store(
            CommandTaskExecutingState::FAILED);
        command_task.command_task_.task_flow_->external_info_.state_manager_
            ->SetState(CommandTaskFlowExecutingState::FAILED);
        return;
      }
    }

    for (CommandTaskExecuting* sub_command_task :
         command_task.command_task_.sub_tasks_) {
      for (std::uint32_t i = 0;
           i != sub_command_task->command_task_.commands_.size(); ++i) {
        if (sub_command_task->command_task_
                .commands_[i](
                    *(sub_command_task->external_info_.command_buffers_[i]))
                .Exception(MM_ERROR_DESCRIPTION(
                    "Failed to record and submit command task."))
                .IsError()) {
          command_task.external_info_.state_.store(
              CommandTaskExecutingState::FAILED);
          command_task.command_task_.task_flow_->external_info_.state_manager_
              ->SetState(CommandTaskFlowExecutingState::FAILED);
          return;
        }
      }
    }

    this_executor->SubmitCommandTask(command_task)
        .Exception(
            MM_ERROR_DESCRIPTION("Failed to record and submit command task."));
  });
  MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, std::move(task_flow));
}

bool MM::RenderSystem::CommandExecutor::HaveCommandTaskToBeProcess() const {
  return !wait_task_flow_queue_.empty() ||
         !executing_command_task_flow_queue_.empty() ||
         !submit_failed_to_be_recovery_command_buffer_.empty();
}

/*-------------------------------------------------------------------------------------*/
