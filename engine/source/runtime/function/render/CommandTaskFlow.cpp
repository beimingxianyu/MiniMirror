//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/CommandTaskFlow.h"

#include "runtime/function/render/CommandTask.h"

std::atomic<MM::RenderSystem::CommandTaskFlowID>
    MM::RenderSystem::CommandTaskFlow::current_command_task_flow_ID_{1};

MM::RenderSystem::CommandTaskFlow::CommandTaskFlow()
    : task_flow_ID_(current_command_task_flow_ID_.fetch_add(
          2, std::memory_order_acq_rel)),
      root_command_task_IDs_(),
      tasks_(),
      task_numbers_{0, 0, 0} {}

MM::RenderSystem::CommandTaskFlow::CommandTaskFlow(
    CommandTaskFlow&& other) noexcept
    : task_flow_ID_(other.task_flow_ID_),
      root_command_task_IDs_(std::move(other.root_command_task_IDs_)),
      tasks_(std::move(other.tasks_)),
      task_numbers_(std::move(other.task_numbers_)) {
  other.task_flow_ID_ = 0;
  std::fill(other.task_numbers_.begin(), other.task_numbers_.end(), 0);
}

MM::RenderSystem::CommandTaskFlow& MM::RenderSystem::CommandTaskFlow::operator=(
    CommandTaskFlow&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  task_flow_ID_ = other.task_flow_ID_;
  root_command_task_IDs_ = std::move(other.root_command_task_IDs_);
  tasks_ = std::move(other.tasks_);
  task_numbers_ = std::move(other.task_numbers_);

  other.task_flow_ID_ = 0;
  std::fill(other.task_numbers_.begin(), other.task_numbers_.end(), 0);

  return *this;
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    CommandType command_type, const TaskType& commands, bool is_async_record) {
  assert(command_type != CommandType::UNDEFINED && IsValid());
  const std::vector<TaskType> temp{commands};

  CommandTask new_command_task(this, command_type, temp, is_async_record);
  tasks_.emplace_back(std::move(new_command_task));

  root_command_task_IDs_.emplace_back(tasks_.back().GetCommandTaskID());

  switch (command_type) {
    case CommandType::GRAPH:
      ++task_numbers_.at(0);
      break;
    case CommandType::COMPUTE:
      ++task_numbers_.at(1);
      break;
    case CommandType::TRANSFORM:
      ++task_numbers_.at(2);
      break;
  }

  return tasks_.back();
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    CommandType command_type, const std::vector<TaskType>& commands,
    bool is_async_record) {
  assert(!commands.empty() && IsValid());

  CommandTask new_command_task(this, command_type, commands, is_async_record);
  tasks_.emplace_back(std::move(new_command_task));

  root_command_task_IDs_.emplace_back(tasks_.back().GetCommandTaskID());

  switch (command_type) {
    case CommandType::GRAPH:
      task_numbers_.at(0) += commands.size();
      break;
    case CommandType::COMPUTE:
      task_numbers_.at(1) += commands.size();
      break;
    case CommandType::TRANSFORM:
      task_numbers_.at(2) += commands.size();
      break;
  }

  return tasks_.back();
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTaskNumber() const {
  assert(IsValid());
  return task_numbers_.at(0) + task_numbers_.at(1) + task_numbers_.at(2);
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetGraphNumber() const {
  assert(IsValid());
  return task_numbers_.at(0);
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetComputeNumber() const {
  assert(IsValid());
  return task_numbers_.at(1);
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTransformNumber() const {
  assert(IsValid());
  return task_numbers_.at(2);
}

bool MM::RenderSystem::CommandTaskFlow::IsRootTask(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  return std::find(root_command_task_IDs_.begin(), root_command_task_IDs_.end(),
                   command_task_ID) != root_command_task_IDs_.end();
}

void MM::RenderSystem::CommandTaskFlow::Clear() {
  root_command_task_IDs_.clear();
  tasks_.clear();
  task_numbers_.fill(0);
}

bool MM::RenderSystem::CommandTaskFlow::HaveRing() const {
  assert(IsValid());
  std::uint32_t task_count = GetTaskNumber();
  if (task_count == 0 || task_count == 1) {
    return false;
  }

  std::unordered_set<CommandTaskID> old_command_task_ID{};
  for (CommandTaskID root_command_task_ID : root_command_task_IDs_) {
    if (!HaveRingImp(old_command_task_ID, root_command_task_ID)) {
      return false;
    }
  }

  return true;
}

bool MM::RenderSystem::CommandTaskFlow::HaveRingImp(
    std::unordered_set<CommandTaskID>& old_command_task_ID,
    MM::RenderSystem::CommandTaskID current_command_task_ID) const {
  if (old_command_task_ID.count(current_command_task_ID)) {
    return false;
  }
  old_command_task_ID.insert(current_command_task_ID);

  Result<std::vector<const CommandTask*>, ErrorResult> sub_cemmand_tasks =
      GetSubCommandTask(current_command_task_ID);
  assert(sub_cemmand_tasks.IgnoreException().IsSuccess());

  for (const CommandTask* sub_command_task : sub_cemmand_tasks.GetResult()) {
    if (!HaveRingImp(old_command_task_ID,
                     sub_command_task->GetCommandTaskID())) {
      return false;
    }
  }

  old_command_task_ID.erase(current_command_task_ID);

  return true;
}

bool MM::RenderSystem::CommandTaskFlow::IsValid() const {
  return task_flow_ID_ != 0;
}

MM::Result<const std::vector<MM::RenderSystem::TaskType>*, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetCommands(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      return Result<const std::vector<TaskType>*>{&command_task.GetCommands()};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

MM::Result<std::vector<const MM::RenderSystem::TaskType*>, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetCommandsIncludeSubCommandTask(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      std::vector<const TaskType*> result{};
      if (command_task.HaveSubCommandTasks()) {
        result.reserve(command_task.GetCommands().size() * 2);
      } else {
        result.reserve(command_task.GetCommands().size());
      }
      const std::vector<TaskType>& main_commands = command_task.GetCommands();
      for (const auto& main_command : main_commands) {
        result.emplace_back(&main_command);
      }

      for (const auto& sub_command_task : command_task.GetSubCommandTasks()) {
        const std::vector<TaskType>& sub_commands =
            sub_command_task->GetCommands();
        for (const auto& sub_command : sub_commands) {
          result.emplace_back(&sub_command);
        }
      }

      return ResultS<std::vector<const TaskType*>>{std::move(result)};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

MM::Result<const MM::RenderSystem::CommandTask*, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetCommandTask(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      return ResultS{&command_task};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

MM::Result<std::vector<const MM::RenderSystem::CommandTask*>, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetSubCommandTask(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  Result<const CommandTask*> main_command_task =
      GetCommandTask(command_task_ID);
  if (main_command_task
          .Exception(MM_ERROR_DESCRIPTION(
              Failed to get main MM::RenderSystem::CommandTask.))
          .IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  const std::vector<CommandTaskID>& sub_task_IDs =
      main_command_task.GetResult()->sub_tasks_;
  std::vector<const CommandTask*> result{};
  result.reserve(sub_task_IDs.size());
  for (CommandTaskID sub_task_ID : sub_task_IDs) {
    Result<const CommandTask*> sub_command_task = GetCommandTask(sub_task_ID);
    assert(sub_command_task.IgnoreException().IsSuccess());
    result.emplace_back(sub_command_task.GetResult());
  }

  return ResultS{std::move(result)};
}

MM::Result<uint32_t, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetRequireCommandBufferNumber(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  Result<const CommandTask*> command_task = GetCommandTask(command_task_ID);
  if (command_task
          .Exception(MM_ERROR_DESCRIPTION(
              Failed to get MM::RenderSystem::CommandTask.))
          .IsError()) {
    return ResultE<>{command_task.GetError().GetErrorCode()};
  }

  return ResultS{command_task.GetResult()->GetRequireCommandBufferNumber()};
}

MM::Result<std::vector<const MM::RenderSystem::CommandTask*>, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetCommandTaskIncludeSubCommandTask(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  Result<const CommandTask*> main_command_task =
      GetCommandTask(command_task_ID);
  if (main_command_task
          .Exception(MM_ERROR_DESCRIPTION(
              Failed to get main MM::RenderSystem::CommandTask.))
          .IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  const std::vector<CommandTaskID>& sub_task_IDs =
      main_command_task.GetResult()->sub_tasks_;
  std::vector<const CommandTask*> result{};
  result.reserve(sub_task_IDs.size() + 1);
  result.emplace_back(main_command_task.GetResult());
  for (CommandTaskID sub_task_ID : sub_task_IDs) {
    Result<const CommandTask*> sub_command_task = GetCommandTask(sub_task_ID);
    assert(sub_command_task.IgnoreException().IsSuccess());
    result.emplace_back(sub_command_task.GetResult());
  }

  return ResultS{std::move(result)};
}

MM::Result<std::vector<const MM::RenderSystem::TaskType*>, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetSubCommands(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      std::vector<const TaskType*> result{};

      for (const auto& sub_command_task : command_task.GetSubCommandTasks()) {
        const std::vector<TaskType>& sub_commands =
            sub_command_task->GetCommands();
        for (const auto& sub_command : sub_commands) {
          result.emplace_back(&sub_command);
        }
      }

      return ResultS<std::vector<const TaskType*>>{std::move(result)};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

MM::Result<uint32_t, MM::ErrorResult> MM::RenderSystem::CommandTaskFlow::
    GetRequireCommandBufferNumberIncludeSubCommandTasks(
        CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      std::uint32_t result = 0;
      result += command_task.GetRequireCommandBufferNumber();

      for (const auto& sub_command_task : command_task.GetSubCommandTasks()) {
        result += sub_command_task->GetRequireCommandBufferNumber();
      }

      return ResultS{result};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

MM::Result<uint32_t, MM::ErrorResult>
MM::RenderSystem::CommandTaskFlow::GetSubCommandTaskRequireCommandBufferNumber(
    MM::RenderSystem::CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      std::uint32_t result = 0;

      for (const auto& sub_command_task : command_task.GetSubCommandTasks()) {
        result += sub_command_task->GetRequireCommandBufferNumber();
      }

      return ResultS{result};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

// void MM::RenderSystem::CommandTaskFlow::RemoveRootTask(
//     const CommandTask& command_task) {
//   const auto target_task = std::find(root_command_task_IDs_.begin(),
//   root_command_task_IDs_.end(),
//                                      command_task.this_unique_ptr_);
//   if (target_task == root_command_task_IDs_.end()) {
//     return;
//   }
//   root_command_task_IDs_.erase(target_task);
// }
//
// void MM::RenderSystem::CommandTaskFlow::RemoveTask(CommandTask& command_task)
// {
//   for (auto iter = tasks_.begin(); iter != tasks_.end();) {
//     if (&(*iter) == command_task.this_unique_ptr_) {
//       iter = tasks_.erase(iter);
//       continue;
//     }
//     ++iter;
//   }
// }
