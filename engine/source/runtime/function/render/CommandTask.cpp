//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/CommandTask.h"

#include "runtime/function/render/CommandTaskFlow.h"

const MM::RenderSystem::CommandBufferType&
MM::RenderSystem::CommandTask::GetCommandType() const {
  return command_type_;
}

const MM::RenderSystem::CommandTaskFlow&
MM::RenderSystem::CommandTask::GetCommandTaskFlow() const {
  return *task_flow_;
}

const std::vector<MM::RenderSystem::CommandTask::TaskType>&
MM::RenderSystem::CommandTask::GetCommands() const {
  return commands_;
}

std::vector<const MM::RenderSystem::CommandTask::TaskType*>
MM::RenderSystem::CommandTask::GetCommandsIncludeSubTasks() const {
  std::vector<const TaskType*> result;
  result.reserve(commands_.size());

  for (const auto& main_command : commands_) {
    result.emplace_back(&main_command);
  }

  for (const auto& sub_task : sub_tasks_) {
    for (const auto& sub_command : sub_task->commands_) {
      result.emplace_back(&sub_command);
    }
  }

  return result;
}

std::uint32_t MM::RenderSystem::CommandTask::GetRequireCommandBufferNumber()
    const {
  return commands_.size();
}

std::uint32_t
MM::RenderSystem::CommandTask::GetRequireCommandBufferNumberIncludeSuBTasks()
    const {
  std::uint32_t require_command_number = commands_.size();
  for (const auto& sub_task : sub_tasks_) {
    require_command_number += sub_task->GetRequireCommandBufferNumber();
  }
  return require_command_number;
}

const std::vector<MM::RenderSystem::WaitAllocatedSemaphore>&
MM::RenderSystem::CommandTask::GetWaitSemaphore() const {
  return wait_semaphore_;
}

std::vector<const MM::RenderSystem::WaitAllocatedSemaphore*>
MM::RenderSystem::CommandTask::GetWaitSemaphoreIncludeSubTasks() const {
  std::vector<const MM::RenderSystem::WaitAllocatedSemaphore*> result;
  result.reserve(wait_semaphore_.size());

  for (const auto& main_semaphore : wait_semaphore_) {
    result.emplace_back(&main_semaphore);
  }

  for (const auto& sub_task : sub_tasks_) {
    for (const auto& sub_semaphore : sub_task->wait_semaphore_) {
      result.emplace_back(&sub_semaphore);
    }
  }

  return result;
}

const std::vector<MM::RenderSystem::AllocateSemaphore>&
MM::RenderSystem::CommandTask::GetSignalSemaphore() const {
  return signal_semaphore_;
}

std::vector<const MM::RenderSystem::AllocateSemaphore*>
MM::RenderSystem::CommandTask::GetSignalSemaphoreIncludeTasks() const {
  std::vector<const AllocateSemaphore*> result;
  result.reserve(signal_semaphore_.size());

  for (const auto& main_semaphore : signal_semaphore_) {
    result.emplace_back(&main_semaphore);
  }

  for (const auto& sub_task : sub_tasks_) {
    for (const auto& sub_semaphore : sub_task->signal_semaphore_) {
      result.emplace_back(&sub_semaphore);
    }
  }

  return result;
}

const std::vector<std::unique_ptr<MM::RenderSystem::CommandTask>>&
MM::RenderSystem::CommandTask::GetSubTasks() const {
  return sub_tasks_;
}

std::uint32_t MM::RenderSystem::CommandTask::GetSubTasksNumber() const {
  return sub_tasks_.size();
}

bool MM::RenderSystem::CommandTask::HaveSubTasks() const {
  return !sub_tasks_.empty();
}

bool MM::RenderSystem::CommandTask::IsValid() const {
  return task_flow_ != nullptr && !commands_.empty();
}

MM::RenderSystem::CommandTask::CommandTask(
    CommandTaskFlow* task_flow, uint32_t command_task_ID,
    const CommandType& command_type, const std::vector<TaskType>& commands,
    std::uint32_t use_render_resource_count,
    const std::vector<WaitAllocatedSemaphore>& wait_semaphore,
    const std::vector<AllocateSemaphore>& signal_semaphore)
    : task_flow_(task_flow),
      command_task_ID_(command_task_ID),
      command_type_(command_type),
      commands_(commands),
      wait_semaphore_(wait_semaphore),
      signal_semaphore_(signal_semaphore),
      use_render_resource_count_(use_render_resource_count),
      cross_task_flow_sync_render_resource_IDs_() {}

void MM::RenderSystem::CommandTask::AddCrossTaskFLowSyncRenderResourceIDs(
    const std::vector<RenderResourceDataID>& render_resource_IDs) {
  cross_task_flow_sync_render_resource_IDs_.insert(
      cross_task_flow_sync_render_resource_IDs_.end(),
      render_resource_IDs.begin(), render_resource_IDs.end());
}
void MM::RenderSystem::CommandTask::AddCrossTaskFLowSyncRenderResourceIDs(
    std::vector<RenderResourceDataID>&& render_resource_IDs) {
  if (cross_task_flow_sync_render_resource_IDs_.empty()) {
    cross_task_flow_sync_render_resource_IDs_ = std::move(render_resource_IDs);
    return;
  }

  cross_task_flow_sync_render_resource_IDs_.insert(
      cross_task_flow_sync_render_resource_IDs_.end(),
      render_resource_IDs.begin(), render_resource_IDs.end());
}
std::uint32_t MM::RenderSystem::CommandTask::GetCommandTaskID() const {
  return command_task_ID_;
}

void MM::RenderSystem::CommandTask::AddCrossTaskFLowSyncRenderResourceIDs(
    const MM::RenderSystem::RenderResourceDataID& render_resource_ID) {
  cross_task_flow_sync_render_resource_IDs_.emplace_back(render_resource_ID);
}

std::uint32_t MM::RenderSystem::CommandTask::GetUseRenderResourceCount() const {
  return use_render_resource_count_;
}

void MM::RenderSystem::CommandTask::SetUseRenderResourceCount(
    std::uint32_t new_use_render_resource_count) {
  use_render_resource_count_ = new_use_render_resource_count;
}

MM::RenderSystem::CommandTask::~CommandTask() = default;
