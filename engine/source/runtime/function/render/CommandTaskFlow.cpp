//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/CommandTaskFlow.h"

#include <cassert>
#include <unordered_set>
#include <vector>

#include "runtime/core/log/log_system.h"
#include "runtime/function/render/CommandTask.h"
#include "runtime/function/render/vk_command_pre.h"
#include "utils/error.h"
#include "utils/type_utils.h"

std::atomic<MM::RenderSystem::CommandTaskFlowID>
    MM::RenderSystem::CommandTaskFlow::current_command_task_flow_ID_{1};

MM::RenderSystem::CommandTaskFlow::CommandTaskFlow()
    : task_flow_ID_(current_command_task_flow_ID_.fetch_add(
          2, std::memory_order_acq_rel)),
      tasks_(),
      task_numbers_{0, 0, 0} {}

MM::RenderSystem::CommandTaskFlow::CommandTaskFlow(
    CommandTaskFlow&& other) noexcept
    : task_flow_ID_(other.task_flow_ID_),
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
    default:
      assert(false && "command_type can not be CommandType::UNDEFINED.");
  }

  return tasks_.back();
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    CommandType command_type, const std::vector<TaskType>& commands,
    bool is_async_record) {
  assert(!commands.empty() && IsValid());

  CommandTask new_command_task(this, command_type, commands, is_async_record);
  tasks_.emplace_back(std::move(new_command_task));

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
    default:
      assert(false && "command_type can not be CommandType::UNDEFINED.");
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
    CommandTaskID command_task_ID) const {
  assert(IsValid());

  Result<const CommandTask*> command_task = GetCommandTask(command_task_ID);
  if (command_task.IgnoreException().IsError()) {
    return false;
  }

  if (!(command_task.GetResult()->is_sub_task_) &&
      command_task.GetResult()->pre_tasks_.empty()) {
    return true;
  }

  return false;
}

void MM::RenderSystem::CommandTaskFlow::Clear() {
  tasks_.clear();
  task_numbers_.fill(0);
}

bool MM::RenderSystem::CommandTaskFlow::HaveRing() const {
  assert(IsValid());
  const std::uint32_t task_count = GetTaskNumber();
  if (task_count == 0 || task_count == 1) {
    return false;
  }

  std::unordered_set<CommandTaskID> old_command_task_ID{};
  for (const CommandTask& command_task : tasks_) {
    if (!(command_task.is_sub_task_) && command_task.pre_tasks_.empty()) {
      if (!HaveRingHelp(old_command_task_ID, command_task.GetCommandTaskID())) {
        return false;
      }
    }
  }

  return true;
}

bool MM::RenderSystem::CommandTaskFlow::HaveRingHelp(
    std::unordered_set<CommandTaskID>& old_command_task_ID,
    CommandTaskID current_command_task_ID) const {
  if (old_command_task_ID.count(current_command_task_ID)) {
    return false;
  }
  old_command_task_ID.insert(current_command_task_ID);

  Result<std::vector<const CommandTask*>> post_cemmand_tasks =
      GetPostCommandTask(current_command_task_ID);
  post_cemmand_tasks.IgnoreException();
  assert(post_cemmand_tasks.IsSuccess());

  for (const CommandTask* post_command_task : post_cemmand_tasks.GetResult()) {
    if (!HaveRingHelp(old_command_task_ID,
                      post_command_task->GetCommandTaskID())) {
      return false;
    }
  }

  old_command_task_ID.erase(current_command_task_ID);

  return true;
}

bool MM::RenderSystem::CommandTaskFlow::IsValid() const {
  return task_flow_ID_ != 0;
}

MM::Result<const std::vector<MM::RenderSystem::TaskType>*>
MM::RenderSystem::CommandTaskFlow::GetCommands(
    CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      return Result<const std::vector<TaskType>*>{&command_task.GetCommands()};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

MM::Result<std::vector<const MM::RenderSystem::TaskType*>>
MM::RenderSystem::CommandTaskFlow::GetCommandsIncludeSubCommandTask(
    CommandTaskID command_task_ID) const {
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

MM::Result<const MM::RenderSystem::CommandTask*>
MM::RenderSystem::CommandTaskFlow::GetCommandTask(
    CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      return ResultS{&command_task};
    }
  }

  return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
}

MM::Result<std::vector<const MM::RenderSystem::CommandTask*>>
MM::RenderSystem::CommandTaskFlow::GetSubCommandTask(
    CommandTaskID command_task_ID) const {
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
  for (const CommandTaskID sub_task_ID : sub_task_IDs) {
    Result<const CommandTask*> sub_command_task = GetCommandTask(sub_task_ID);
    sub_command_task.IgnoreException();
    assert(sub_command_task.IsSuccess());
    result.emplace_back(sub_command_task.GetResult());
  }

  return ResultS{std::move(result)};
}

MM::Result<std::vector<const MM::RenderSystem::CommandTask*>>
MM::RenderSystem::CommandTaskFlow::GetPreCommandTask(
    CommandTaskID command_task_ID) const {
  assert(IsValid());
  Result<const CommandTask*> main_command_task =
      GetCommandTask(command_task_ID);
  if (main_command_task
          .Exception(MM_ERROR_DESCRIPTION(
              Failed to get main MM::RenderSystem::CommandTask.))
          .IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  const std::vector<CommandTaskID>& pre_task_IDs =
      main_command_task.GetResult()->pre_tasks_;
  std::vector<const CommandTask*> result{};
  result.reserve(pre_task_IDs.size());
  for (const CommandTaskID pre_task_ID : pre_task_IDs) {
    Result<const CommandTask*> pre_command_task = GetCommandTask(pre_task_ID);
    pre_command_task.IgnoreException();
    assert(pre_command_task.IsSuccess());
    result.emplace_back(pre_command_task.GetResult());
  }

  return ResultS{std::move(result)};
}

MM::Result<std::vector<const MM::RenderSystem::CommandTask*>>
MM::RenderSystem::CommandTaskFlow::GetPostCommandTask(
    CommandTaskID command_task_ID) const {
  assert(IsValid());
  Result<const CommandTask*> main_command_task =
      GetCommandTask(command_task_ID);
  if (main_command_task
          .Exception(MM_ERROR_DESCRIPTION(
              Failed to get main MM::RenderSystem::CommandTask.))
          .IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  const std::vector<CommandTaskID>& post_task_IDs =
      main_command_task.GetResult()->post_tasks_;
  std::vector<const CommandTask*> result{};
  result.reserve(post_task_IDs.size());
  for (const CommandTaskID post_task_ID : post_task_IDs) {
    Result<const CommandTask*> post_command_task = GetCommandTask(post_task_ID);
    post_command_task.IgnoreException();
    assert(post_command_task.IsSuccess());
    result.emplace_back(post_command_task.GetResult());
  }

  return ResultS{std::move(result)};
}

MM::Result<uint32_t>
MM::RenderSystem::CommandTaskFlow::GetRequireCommandBufferNumber(
    CommandTaskID command_task_ID) const {
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

MM::Result<std::vector<const MM::RenderSystem::CommandTask*>>
MM::RenderSystem::CommandTaskFlow::GetCommandTaskIncludeSubCommandTask(
    CommandTaskID command_task_ID) const {
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
  for (const CommandTaskID sub_task_ID : sub_task_IDs) {
    Result<const CommandTask*> sub_command_task = GetCommandTask(sub_task_ID);
    sub_command_task.IgnoreException();
    assert(sub_command_task.IsSuccess());
    result.emplace_back(sub_command_task.GetResult());
  }

  return ResultS{std::move(result)};
}

MM::Result<std::vector<const MM::RenderSystem::TaskType*>>
MM::RenderSystem::CommandTaskFlow::GetSubCommands(
    CommandTaskID command_task_ID) const {
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

MM::Result<uint32_t> MM::RenderSystem::CommandTaskFlow::
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

MM::Result<uint32_t>
MM::RenderSystem::CommandTaskFlow::GetSubCommandTaskRequireCommandBufferNumber(
    CommandTaskID command_task_ID) const {
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

bool MM::RenderSystem::CommandTaskFlow::IsValidSubmissionObject() const {
  if (HaveRing()) {
    return false;
  }

  if (!SubCommandTaskRelationshipIsValid()) {
    return false;
  }

  return true;
}

bool MM::RenderSystem::CommandTaskFlow::HaveCommandTask(
    CommandTaskID command_task_ID) const {
  assert(IsValid());
  for (const CommandTask& command_task : tasks_) {
    if (command_task_ID == command_task.GetCommandTaskID()) {
      return true;
    }
  }

  return false;
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTaskFlow::AddPreCommandTask(
    CommandTaskID main_command_task_ID, CommandTaskID pre_command_task_ID) {
  assert(IsValid());

  Result<const CommandTask*> main_command_task =
      GetCommandTask(main_command_task_ID);
  main_command_task.Exception(
      MM_ERROR_DESCRIPTION(Main command task is not exist.));
  if (main_command_task.IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  if (!HaveCommandTask(pre_command_task_ID)) {
    MM_LOG_ERROR("Pre command task is not exist.");
    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  main_command_task.GetResult()->pre_tasks_.push_back(pre_command_task_ID);

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTaskFlow::AddPreCommandTask(
    CommandTaskID main_command_task_ID,
    const std::vector<CommandTaskID>& pre_command_task_IDs) {
  assert(IsValid());

  Result<const CommandTask*> main_command_task =
      GetCommandTask(main_command_task_ID);
  main_command_task.Exception(
      MM_ERROR_DESCRIPTION(Main command task is not exist.));
  if (main_command_task.IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  for (const CommandTaskID pre_command_task_ID : pre_command_task_IDs) {
    if (!HaveCommandTask(pre_command_task_ID)) {
      MM_LOG_ERROR("Pre command task is not exist.");
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }
  }

  for (CommandTaskID pre_command_task_ID : pre_command_task_IDs) {
    main_command_task.GetResult()->pre_tasks_.push_back(pre_command_task_ID);
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTaskFlow::AddPostCommandTask(
    CommandTaskID main_command_task_ID, CommandTaskID post_command_task_ID) {
  assert(IsValid());

  Result<const CommandTask*> main_command_task =
      GetCommandTask(main_command_task_ID);
  main_command_task.Exception(
      MM_ERROR_DESCRIPTION(Main command task is not exist.));
  if (main_command_task.IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  if (!HaveCommandTask(post_command_task_ID)) {
    MM_LOG_ERROR("post command task is not exist.");
    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  main_command_task.GetResult()->post_tasks_.push_back(post_command_task_ID);

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTaskFlow::AddPostCommandTask(
    CommandTaskID main_command_task_ID,
    const std::vector<CommandTaskID>& post_command_task_IDs) {
  assert(IsValid());

  Result<const CommandTask*> main_command_task =
      GetCommandTask(main_command_task_ID);
  main_command_task.Exception(
      MM_ERROR_DESCRIPTION(Main command task is not exist.));
  if (main_command_task.IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  for (const CommandTaskID post_command_task_ID : post_command_task_IDs) {
    if (!HaveCommandTask(post_command_task_ID)) {
      MM_LOG_ERROR("post command task is not exist.");
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }
  }

  for (CommandTaskID post_command_task_ID : post_command_task_IDs) {
    main_command_task.GetResult()->post_tasks_.push_back(post_command_task_ID);
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil> MM::RenderSystem::CommandTaskFlow::Merge(
    CommandTaskID main_command_task_ID, CommandTaskID sub_command_task_ID) {
  assert(IsValid());

  Result<const CommandTask*> main_command_task =
      GetCommandTask(main_command_task_ID);
  main_command_task.Exception(
      MM_ERROR_DESCRIPTION(Main command task is not exist.));
  if (main_command_task.IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  Result<const CommandTask*> sub_command_task =
      GetCommandTask(sub_command_task_ID);
  sub_command_task.Exception("Sub command task is not exist.");
  if (sub_command_task.IsError()) {
    return ResultE<>{sub_command_task.GetError().GetErrorCode()};
  }

  if (sub_command_task.GetResult()->IsSubTask() ||
      sub_command_task.GetResult()->GetCommandType() !=
          main_command_task.GetResult()->GetCommandType()) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  for (const CommandTaskID old_sub_command_task_ID :
       main_command_task.GetResult()->sub_tasks_) {
    if (old_sub_command_task_ID == sub_command_task_ID) {
      return ResultS<Nil>{};
    }
  }

  main_command_task.GetResult()->sub_tasks_.push_back(sub_command_task_ID);
  const_cast<CommandTask*>(sub_command_task.GetResult())->is_sub_task_ = true;

  return ResultS<Nil>{};
}
MM::Result<MM::Nil> MM::RenderSystem::CommandTaskFlow::Merge(
    CommandTaskID main_command_task_ID,
    const std::vector<CommandTaskID>& sub_command_task_IDs) {
  assert(IsValid());

  Result<const CommandTask*> main_command_task =
      GetCommandTask(main_command_task_ID);
  main_command_task.Exception(
      MM_ERROR_DESCRIPTION(Main command task is not exist.));
  if (main_command_task.IsError()) {
    return ResultE<>{main_command_task.GetError().GetErrorCode()};
  }

  for (const CommandTaskID sub_command_task_ID : sub_command_task_IDs) {
    if (!HaveCommandTask(sub_command_task_ID)) {
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }
  }

  for (CommandTaskID sub_command_task_ID : sub_command_task_IDs) {
    Result<const CommandTask*> sub_command_task =
        GetCommandTask(sub_command_task_ID);
    sub_command_task.IgnoreException();
    assert(sub_command_task.IsSuccess());
    if (sub_command_task.GetResult()->IsSubTask() ||
        sub_command_task.GetResult()->GetCommandType() !=
            main_command_task.GetResult()->GetCommandType()) {
      return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
    }

    for (const CommandTaskID old_sub_command_task_ID :
         main_command_task.GetResult()->sub_tasks_) {
      if (old_sub_command_task_ID == sub_command_task_ID) {
        continue;
      }
    }

    main_command_task.GetResult()->sub_tasks_.push_back(sub_command_task_ID);
    const_cast<CommandTask*>(sub_command_task.GetResult())->is_sub_task_ = true;
  }

  return ResultS<Nil>{};
}

bool MM::RenderSystem::CommandTaskFlow::SubCommandTaskRelationshipIsValid()
    const {
  assert(IsValid());

  std::unordered_set<CommandTaskID> pre_command_task_IDs{};

  for (const CommandTask& main_command_task : tasks_) {
    if (!main_command_task.sub_tasks_.empty()) {
      continue;
    }

    for (CommandTaskID pre_command_task_ID : main_command_task.pre_tasks_) {
      pre_command_task_IDs.insert(pre_command_task_ID);
    }

    for (const CommandTaskID sub_command_task_ID : main_command_task.sub_tasks_) {
      HelpGetAllPreCommandTaskIDs(pre_command_task_IDs, sub_command_task_ID);
    }

    for (const CommandTaskID post_command_task_ID : main_command_task.post_tasks_) {
      if (!HelpCheckAllPostCommandTaskIDs(pre_command_task_IDs,
                                          post_command_task_ID)) {
        return false;
      }
    }

    pre_command_task_IDs.clear();
  }

  return true;
}

void MM::RenderSystem::CommandTaskFlow::HelpGetAllPreCommandTaskIDs(
    std::unordered_set<CommandTaskID>& pre_command_task_IDs,
    CommandTaskID current_command_task_ID) const {
  pre_command_task_IDs.insert(current_command_task_ID);

  Result<std::vector<const CommandTask*>> pre_command_tasks =
      GetPreCommandTask(current_command_task_ID);
  pre_command_tasks.IgnoreException();
  assert(pre_command_tasks.IsSuccess());

  for (const CommandTask* pre_command_task : pre_command_tasks.GetResult()) {
    HelpGetAllPreCommandTaskIDs(pre_command_task_IDs,
                                pre_command_task->GetCommandTaskID());
  }
}

bool MM::RenderSystem::CommandTaskFlow::HelpCheckAllPostCommandTaskIDs(
    std::unordered_set<CommandTaskID>& pre_command_task_IDs,
    CommandTaskID current_command_task_ID) const {
  if (pre_command_task_IDs.count(current_command_task_ID)) {
    return false;
  }

  Result<std::vector<const CommandTask*>> post_command_tasks =
      GetPostCommandTask(current_command_task_ID);
  post_command_tasks.IgnoreException();
  assert(post_command_tasks.IsSuccess());

  bool result = true;
  for (const CommandTask* post_command_task : post_command_tasks.GetResult()) {
    result &= HelpCheckAllPostCommandTaskIDs(
        pre_command_task_IDs, post_command_task->GetCommandTaskID());
    if (!result) {
      return result;
    }
  }

  return result;
}
