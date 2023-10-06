//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/CommandTaskFlow.h"

#include "runtime/function/render/CommandTask.h"

MM::RenderSystem::CommandTaskFlow::CommandTaskFlow(
    CommandTaskFlow&& other) noexcept {
  std::lock(task_sync_, other.task_sync_);
  std::lock_guard<std::shared_mutex> guard1(task_sync_, std::adopt_lock);
  std::lock_guard<std::shared_mutex> guard2(other.task_sync_, std::adopt_lock);

  root_tasks_ = std::move(other.root_tasks_);
  tasks_ = std::move(other.tasks_);
  task_numbers_ = other.task_numbers_;
}

MM::RenderSystem::CommandTaskFlow& MM::RenderSystem::CommandTaskFlow::operator=(
    CommandTaskFlow&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  std::lock(task_sync_, other.task_sync_);
  std::lock_guard<std::shared_mutex> guard1(task_sync_, std::adopt_lock);
  std::lock_guard<std::shared_mutex> guard2(other.task_sync_, std::adopt_lock);

  root_tasks_ = std::move(other.root_tasks_);
  tasks_ = std::move(other.tasks_);
  task_numbers_ = other.task_numbers_;

  return *this;
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    CommandType command_type, const TaskType& commands,
    std::uint32_t use_render_resource_count,
    const std::vector<MM::RenderSystem::WaitAllocatedSemaphore>&
        wait_semaphores,
    const std::vector<MM::RenderSystem::AllocateSemaphore>& signal_semaphores) {
  assert(command_type != CommandType::UNDEFINED);
  const std::vector<TaskType> temp{commands};
  std::unique_lock<std::shared_mutex> guard(task_sync_);

  tasks_.emplace_back(std::unique_ptr<CommandTask>(new CommandTask(
      this, ++increase_task_ID_, command_type, temp, use_render_resource_count,
      wait_semaphores, signal_semaphores)));

  root_tasks_.emplace_back(&(tasks_.back()));
  tasks_.back()->this_unique_ptr_ = &(tasks_.back());

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

  return *tasks_.back();
}

MM::RenderSystem::CommandTask& MM::RenderSystem::CommandTaskFlow::AddTask(
    CommandType command_type, const std::vector<TaskType>& commands,
    std::uint32_t use_render_resource_count,
    const std::vector<WaitAllocatedSemaphore>& wait_semaphores,
    const std::vector<AllocateSemaphore>& signal_semaphores) {
  assert(!commands.empty());

  std::unique_lock<std::shared_mutex> guard(task_sync_);

  tasks_.emplace_back(std::unique_ptr<CommandTask>(new CommandTask(
      this, ++increase_task_ID_, command_type, commands,
      use_render_resource_count, wait_semaphores, signal_semaphores)));

  root_tasks_.emplace_back(&(tasks_.back()));
  tasks_.back()->this_unique_ptr_ = &(tasks_.back());

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

  return *tasks_.back();
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTaskNumber() const {
  return task_numbers_.at(0) + task_numbers_.at(1) + task_numbers_.at(2);
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetGraphNumber() const {
  return task_numbers_.at(0);
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetComputeNumber() const {
  return task_numbers_.at(1);
}

std::uint32_t MM::RenderSystem::CommandTaskFlow::GetTransformNumber() const {
  return task_numbers_.at(2);
}

bool MM::RenderSystem::CommandTaskFlow::IsRootTask(
    const CommandTask& command_task) const {
  return std::find(root_tasks_.begin(), root_tasks_.end(),
                   command_task.this_unique_ptr_) != root_tasks_.end();
}

void MM::RenderSystem::CommandTaskFlow::Clear() {
  root_tasks_.clear();
  tasks_.clear();
  task_numbers_.fill(0);
}

bool MM::RenderSystem::CommandTaskFlow::HaveRing() const {
  std::uint32_t task_count = GetTaskNumber();
  if (task_count == 0 || task_count == 1) {
    return false;
  }

  const std::vector<CommandTaskEdge> command_task_edges = GetCommandTaskEdges();

  std::unordered_map<const std::unique_ptr<CommandTask>*, std::uint32_t>
      penetrations;
  for (const auto& task_edge : command_task_edges) {
    ++penetrations[task_edge.end_command_task_];
  }

  std::stack<const std::unique_ptr<CommandTask>*> penetration_zero;

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
    const std::unique_ptr<CommandTask>* no_penetration_task =
        penetration_zero.top();
    penetration_zero.pop();
    for (const auto& post_task : (*no_penetration_task)->post_tasks_) {
      if (--penetrations[post_task] == 0) {
        penetration_zero.push(post_task);
        --task_count;
      }
    }
  }

  return task_count == 0;
}

bool MM::RenderSystem::CommandTaskFlow::IsValid() const {
  return GetTaskNumber() != 0 && !HaveRing();
}

MM::RenderSystem::CommandTaskFlow::CommandTaskEdge::CommandTaskEdge(
    const std::unique_ptr<CommandTask>* start,
    const std::unique_ptr<CommandTask>* end)
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

std::vector<MM::RenderSystem::CommandTaskFlow::CommandTaskEdge>
MM::RenderSystem ::CommandTaskFlow::GetCommandTaskEdges() const {
  std::vector<CommandTaskEdge> command_task_edges;
  for (const auto& task : tasks_) {
    if (task->is_sub_task_) {
      continue;
    }
    for (const auto& last_task : task->post_tasks_) {
      command_task_edges.emplace_back(&task, last_task);
    }
  }

  return command_task_edges;
}

void MM::RenderSystem::CommandTaskFlow::RemoveTask(CommandTask& command_task) {
  for (auto iter = tasks_.begin(); iter != tasks_.end();) {
    if (&(*iter) == command_task.this_unique_ptr_) {
      iter = tasks_.erase(iter);
      continue;
    }
    ++iter;
  }
}
