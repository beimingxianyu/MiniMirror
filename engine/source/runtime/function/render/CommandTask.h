//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include "runtime/function/render/CommandTaskFlow.h"
#include "runtime/function/render/vk_command_pre.h"

namespace MM {
namespace RenderSystem {
class CommandTask {
  friend class CommandExecutor;
  friend class CommandTaskFlow;

 public:
  using TaskType = CommandTaskFlow::TaskType;

 public:
  CommandTask() = delete;
  ~CommandTask();
  CommandTask(const CommandTask& other) = delete;
  CommandTask(CommandTask&& other) = delete;
  CommandTask& operator=(const CommandTask& other) = delete;
  CommandTask& operator=(CommandTask&& other) = delete;

 public:
  const CommandBufferType& GetCommandType() const;

  const CommandTaskFlow& GetCommandTaskFlow() const;

  const std::vector<TaskType>& GetCommands() const;

  std::vector<const TaskType*> GetCommandsIncludeSubTasks() const;

  std::uint32_t GetRequireCommandBufferNumber() const;

  std::uint32_t GetRequireCommandBufferNumberIncludeSuBTasks() const;

  const std::vector<WaitAllocatedSemaphore>& GetWaitSemaphore() const;

  std::vector<const WaitAllocatedSemaphore*> GetWaitSemaphoreIncludeSubTasks()
      const;

  const std::vector<AllocateSemaphore>& GetSignalSemaphore() const;

  std::vector<const AllocateSemaphore*> GetSignalSemaphoreIncludeTasks() const;

  const std::vector<std::unique_ptr<CommandTask>>& GetSubTasks() const;

  std::uint32_t GetSubTasksNumber() const;

  template <typename... CommandTasks>
  Result<Nil, ErrorResult> Merge(CommandTasks&&... command_tasks);

  template <typename... CommandTasks>
  Result<Nil, ErrorResult> IsPreTaskTo(CommandTasks&&... command_tasks);

  template <typename... CommandTasks>
  Result<Nil, ErrorResult> IsPostTaskTo(CommandTasks&&... command_tasks);

  bool HaveSubTasks() const;

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const RenderResourceDataID& render_resource_ID);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const std::vector<RenderResourceDataID>& render_resource_IDs);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      std::vector<RenderResourceDataID>&& render_resource_IDs);

  std::uint32_t GetCommandTaskID() const;

  std::uint32_t GetUseRenderResourceCount() const;

  void SetUseRenderResourceCount(std::uint32_t new_use_render_resource_count);

  bool IsValid() const;

 private:
  CommandTask(CommandTaskFlow* task_flow, uint32_t command_task_ID,
              const CommandType& command_type,
              const std::vector<TaskType>& commands,
              std::uint32_t use_render_resource_count,
              const std::vector<WaitAllocatedSemaphore>& wait_semaphore,
              const std::vector<AllocateSemaphore>& signal_semaphore);

 private:
  CommandTaskFlow* task_flow_;
  std::uint32_t command_task_ID_{0};
  std::unique_ptr<CommandTask>* this_unique_ptr_{nullptr};
  CommandType command_type_{CommandType::UNDEFINED};
  std::vector<TaskType> commands_{};
  std::vector<WaitAllocatedSemaphore> wait_semaphore_{};
  std::vector<AllocateSemaphore> signal_semaphore_{};
  mutable std::list<std::unique_ptr<CommandTask>*> pre_tasks_{};
  mutable std::list<std::unique_ptr<CommandTask>*> post_tasks_{};

  mutable std::vector<std::unique_ptr<CommandTask>> sub_tasks_{};
  bool is_sub_task_{false};

  std::uint32_t use_render_resource_count_{1};
  std::vector<RenderResourceDataID> cross_task_flow_sync_render_resource_IDs_;
};

template <typename... CommandTasks>
Result<Nil, ErrorResult> CommandTask::Merge(CommandTasks&&... command_tasks) {
  bool input_parameters_is_right = true;
  ((input_parameters_is_right &= (&command_tasks != this)), ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "The predecessor of an object of type MM::RenderSystem::CommandTask "
        "cannot be itself.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }
  ((input_parameters_is_right &= (task_flow_ == command_tasks.task_flow_)),
   ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }
  // TODO Optimize the algorithm to remove this limitation.
  ((input_parameters_is_right &= (command_tasks.pre_tasks_.size() == 0 &&
                                  command_tasks.post_tasks_.size() == 0)),
   ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }
  ((input_parameters_is_right &=
    (command_tasks.command_type_ == command_type_)),
   ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "Two MM::RenderSystem::CommandTask's command_type_ are not equal.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }
  // TODO Optimize the algorithm to remove this limitation.
  ((input_parameters_is_right &= (!command_tasks.HaveSubTasks())), ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR("Cannot nest merge MM::RenderSystem::CommandTask.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }

  std::uint32_t new_cross_task_flow_sync_render_resource_IDs_vector_size =
      cross_task_flow_sync_render_resource_IDs_.size();
  ((new_cross_task_flow_sync_render_resource_IDs_vector_size +=
    command_tasks.cross_task_flow_sync_render_resource_IDs_.size()),
   ...);
  cross_task_flow_sync_render_resource_IDs_.reserve(
      new_cross_task_flow_sync_render_resource_IDs_vector_size);
  ((cross_task_flow_sync_render_resource_IDs_.insert(
       cross_task_flow_sync_render_resource_IDs_.begin(),
       command_tasks.cross_task_flow_sync_render_resource_IDs_.begin(),
       command_tasks.cross_task_flow_sync_render_resource_IDs_.end())),
   ...);

  (task_flow_->RemoveRootTask(command_tasks), ...);
  (sub_tasks_.emplace_back(std::move(*command_tasks.this_unique_ptr_)), ...);
  ((command_tasks.is_sub_task_ = true), ...);

  (task_flow_->RemoveTask(command_tasks), ...);

  return ResultS<Nil>{};
}

template <typename... CommandTasks>
Result<Nil, ErrorResult> CommandTask::IsPreTaskTo(
    CommandTasks&&... command_tasks) {
  bool input_parameters_is_right = true;
  ((input_parameters_is_right &= (&command_tasks != this)), ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "The predecessor of an object of type MM::RenderSystem::CommandTask "
        "cannot be itself.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }
  ((input_parameters_is_right &= (task_flow_ == command_tasks.task_flow_)),
   ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }

  (command_tasks.pre_tasks_.push_back(this_unique_ptr_), ...);
  (task_flow_->RemoveRootTask(command_tasks), ...);
  (post_tasks_.push_back(&command_tasks.this_unique_ptr_), ...);

  return ResultS<Nil>{};
}

template <typename... CommandTasks>
Result<Nil, ErrorResult> CommandTask::IsPostTaskTo(
    CommandTasks&&... command_tasks) {
  bool input_parameters_is_right = true;
  ((input_parameters_is_right &= (&command_tasks != this)), ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "The post task of an object of type MM::RenderSystem::CommandTask "
        "cannot be itself.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }
  ((input_parameters_is_right &= (task_flow_ == command_tasks.task_flow_)),
   ...);
  if (!input_parameters_is_right) {
    MM_LOG_ERROR(
        "Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }

  (command_tasks.post_tasks_.push_back(this_unique_ptr_), ...);
  (pre_tasks_.push_back(&command_tasks.this_unique_ptr_), ...);
  task_flow_->RemoveRootTask(*this);

  return ResultS<Nil>{};
}
}  // namespace RenderSystem
}  // namespace MM