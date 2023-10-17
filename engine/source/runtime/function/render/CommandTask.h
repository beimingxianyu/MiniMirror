//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include "runtime/function/render/CommandTaskFlow.h"

namespace MM {
namespace RenderSystem {
class CommandTask {
  friend class CommandExecutor;
  friend class CommandTaskFlow;

 public:
  CommandTask() = delete;
  ~CommandTask();
  CommandTask(const CommandTask& other) = delete;
  CommandTask(CommandTask&& other) noexcept;
  CommandTask& operator=(const CommandTask& other) = delete;
  CommandTask& operator=(CommandTask&& other) noexcept;

 public:
  const CommandBufferType& GetCommandType() const;

  const CommandTaskFlow& GetCommandTaskFlow() const;

  const std::vector<TaskType>& GetCommands() const;

  std::vector<const TaskType*> GetCommandsIncludeSubCommandTasks() const;

  std::uint32_t GetRequireCommandBufferNumber() const;

  std::uint32_t GetRequireCommandBufferNumberIncludeSubCommandTasks() const;

  std::vector<const CommandTask*> GetSubCommandTasks() const;

  std::uint32_t GetSubCommandTasksNumber() const;

  Result<Nil, ErrorResult> AddPreCommandTask(CommandTaskID pre_command_task_ID);

  Result<Nil, ErrorResult> AddPreCommandTask(
      const std::vector<CommandTaskID>& pre_command_task_IDs);

  Result<Nil, ErrorResult> AddPostCommandTask(
      CommandTaskID post_command_task_ID);

  Result<Nil, ErrorResult> AddPostCommandTask(
      const std::vector<CommandTaskID>& post_command_task_IDs);

  Result<Nil, ErrorResult> Merge(CommandTaskID sub_command_task_ID);

  Result<Nil, ErrorResult> Merge(
      const std::vector<CommandTaskID>& sub_command_task_IDs);

  template <typename... CommandTasks>
  Result<Nil, ErrorResult> Merge(CommandTasks&&... command_tasks);

  template <typename... CommandTasks>
  Result<Nil, ErrorResult> IsPreTaskTo(CommandTasks&&... command_tasks);

  template <typename... CommandTasks>
  Result<Nil, ErrorResult> IsPostTaskTo(CommandTasks&&... command_tasks);

  bool HaveSubCommandTasks() const;

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const RenderResourceDataID& render_resource_ID);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const std::vector<RenderResourceDataID>& render_resource_IDs);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      std::vector<RenderResourceDataID>&& render_resource_IDs);

  CommandTaskID GetCommandTaskID() const;

  bool IsAsyncRecord() const;

  void SetAsyncRecord();

  void SetSyncRecord();

  bool IsValid() const;

 private:
  CommandTask(CommandTaskFlow* task_flow, const CommandType& command_type,
              const std::vector<TaskType>& commands, bool is_async_record);

 private:
  static std::atomic<CommandTaskID> current_command_tast_ID_;

  CommandTaskFlow* task_flow_;
  CommandTaskID command_task_ID_{0};
  CommandType command_type_{CommandType::UNDEFINED};
  std::vector<TaskType> commands_{};
  mutable std::vector<CommandTaskID> pre_tasks_{};
  mutable std::vector<CommandTaskID> post_tasks_{};

  mutable std::vector<CommandTaskID> sub_tasks_{};
  bool is_sub_task_{false};
  bool is_async_record_{false};
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
  ((input_parameters_is_right &= (!command_tasks.HaveSubCommandTasks())), ...);
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