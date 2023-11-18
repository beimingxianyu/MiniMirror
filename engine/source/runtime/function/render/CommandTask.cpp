//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/CommandTask.h"

std::atomic<MM::RenderSystem::CommandTaskID>
    MM::RenderSystem::CommandTask::current_command_tast_ID_{1};

const MM::RenderSystem::CommandBufferType&
MM::RenderSystem::CommandTask::GetCommandType() const {
  assert(IsValid());
  return command_type_;
}

const MM::RenderSystem::CommandTaskFlow&
MM::RenderSystem::CommandTask::GetCommandTaskFlow() const {
  assert(IsValid());
  return *task_flow_;
}

const std::vector<MM::RenderSystem::TaskType>&
MM::RenderSystem::CommandTask::GetCommands() const {
  assert(IsValid());
  return commands_;
}

std::vector<const MM::RenderSystem::TaskType*>
MM::RenderSystem::CommandTask::GetCommandsIncludeSubCommandTasks() const {
  assert(IsValid());
  Result<std::vector<const TaskType*>> result =
      task_flow_->GetCommandsIncludeSubCommandTask(command_task_ID_);
  assert(result.Exception().IsSuccess());

  return result.GetResult();
}

std::uint32_t MM::RenderSystem::CommandTask::GetRequireCommandBufferNumber()
    const {
  assert(IsValid());
  return commands_.size();
}

std::uint32_t MM::RenderSystem::CommandTask::
    GetRequireCommandBufferNumberIncludeSubCommandTasks() const {
  assert(IsValid());
  Result<std::uint32_t> result =
      task_flow_->GetRequireCommandBufferNumberIncludeSubCommandTasks(
          command_task_ID_);
  assert(result.Exception().IsSuccess());

  return result.GetResult();
}

std::vector<const MM::RenderSystem::CommandTask*>
MM::RenderSystem::CommandTask::GetSubCommandTasks() const {
  assert(IsValid());
  Result<std::vector<const CommandTask*>> result =
      task_flow_->GetSubCommandTask(command_task_ID_);
  assert(result.Exception().IsSuccess());

  return result.GetResult();
}

std::uint32_t MM::RenderSystem::CommandTask::GetSubCommandTasksNumber() const {
  assert(IsValid());
  return sub_tasks_.size();
}

bool MM::RenderSystem::CommandTask::HaveSubCommandTasks() const {
  assert(IsValid());
  return !sub_tasks_.empty();
}

bool MM::RenderSystem::CommandTask::IsValid() const {
  return command_task_ID_ != 0;
}

MM::RenderSystem::CommandTask::CommandTask(
    CommandTaskFlow* task_flow, const CommandType& command_type,
    const std::vector<TaskType>& commands, bool is_async_record)
    : task_flow_(task_flow),
      command_task_ID_(
          current_command_tast_ID_.fetch_add(2, std::memory_order_acq_rel)),
      command_type_(command_type),
      commands_(commands),
      is_sub_task_(false),
      is_async_record_(is_async_record),
      cross_task_flow_sync_render_resource_IDs_() {
  assert(task_flow_ != nullptr && task_flow_->IsValid());
}

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

MM::RenderSystem::CommandTaskID
MM::RenderSystem::CommandTask::GetCommandTaskID() const {
  return command_task_ID_;
}

void MM::RenderSystem::CommandTask::AddCrossTaskFLowSyncRenderResourceIDs(
    const RenderResourceDataID& render_resource_ID) {
  cross_task_flow_sync_render_resource_IDs_.emplace_back(render_resource_ID);
}

MM::RenderSystem::CommandTask::CommandTask(CommandTask&& other) noexcept
    : task_flow_(other.task_flow_),
      command_task_ID_(other.command_task_ID_),
      command_type_(other.command_type_),
      commands_(std::move(other.commands_)),
      pre_tasks_(std::move(other.pre_tasks_)),
      post_tasks_(std::move(other.post_tasks_)),
      sub_tasks_(std::move(other.sub_tasks_)),
      is_sub_task_(other.is_sub_task_),
      is_async_record_(other.is_async_record_),
      cross_task_flow_sync_render_resource_IDs_(
          std::move(other.cross_task_flow_sync_render_resource_IDs_)) {
  other.task_flow_ = nullptr;
  other.command_task_ID_ = 0;
  other.command_type_ = CommandType ::UNDEFINED;
  other.is_sub_task_ = false;
  other.is_async_record_ = false;
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTask::operator=(
    CommandTask&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  task_flow_ = other.task_flow_;
  command_task_ID_ = other.command_task_ID_;
  command_type_ = other.command_type_;
  commands_ = std::move(other.commands_);
  pre_tasks_ = std::move(other.pre_tasks_);
  post_tasks_ = std::move(other.post_tasks_);
  sub_tasks_ = std::move(other.sub_tasks_);
  is_sub_task_ = other.is_sub_task_;
  is_async_record_ = other.is_async_record_;
  cross_task_flow_sync_render_resource_IDs_ =
      std::move(other.cross_task_flow_sync_render_resource_IDs_);

  other.task_flow_ = nullptr;
  other.command_task_ID_ = 0;
  other.command_type_ = CommandType ::UNDEFINED;
  other.is_sub_task_ = false;
  other.is_async_record_ = false;

  return *this;
}

bool MM::RenderSystem::CommandTask::IsAsyncRecord() const {
  return is_async_record_;
}

void MM::RenderSystem::CommandTask::SetAsyncRecord() {
  is_async_record_ = true;
}

void MM::RenderSystem::CommandTask::SetSyncRecord() {
  is_async_record_ = false;
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTask::AddPreCommandTask(
    CommandTaskID pre_command_task_ID) {
  assert(IsValid());
  return task_flow_->AddPreCommandTask(command_task_ID_, pre_command_task_ID);
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTask::AddPreCommandTask(
    const std::vector<CommandTaskID>& pre_command_task_IDs) {
  assert(IsValid());
  return task_flow_->AddPreCommandTask(command_task_ID_, pre_command_task_IDs);
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTask::AddPostCommandTask(
    CommandTaskID post_command_task_ID) {
  assert(IsValid());
  return task_flow_->AddPostCommandTask(command_task_ID_, post_command_task_ID);
}

MM::Result<MM::Nil>
MM::RenderSystem::CommandTask::AddPostCommandTask(
    const std::vector<CommandTaskID>& post_command_task_IDs) {
  assert(IsValid());
  return task_flow_->AddPostCommandTask(command_task_ID_,
                                        post_command_task_IDs);
}

MM::Result<MM::Nil> MM::RenderSystem::CommandTask::Merge(
    CommandTaskID sub_command_task_ID) {
  assert(IsValid());
  return task_flow_->Merge(command_task_ID_, sub_command_task_ID);
}

MM::Result<MM::Nil> MM::RenderSystem::CommandTask::Merge(
    const std::vector<CommandTaskID>& sub_command_task_IDs) {
  assert(IsValid());
  return task_flow_->Merge(command_task_ID_, sub_command_task_IDs);
}

MM::RenderSystem::CommandTask::~CommandTask() = default;
bool MM::RenderSystem::CommandTask::IsSubTask() const { return is_sub_task_; }
