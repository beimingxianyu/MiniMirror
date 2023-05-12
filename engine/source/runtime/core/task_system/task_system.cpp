#include "runtime/core/task_system/task_system.h"

MM::TaskSystem::TaskSystem* MM::TaskSystem::TaskSystem::task_system_{nullptr};
std::mutex MM::TaskSystem::TaskSystem::sync_flag_{};

MM::TaskSystem::TaskSystem* MM::TaskSystem::TaskSystem::GetInstance() {
  if (task_system_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!task_system_) {
      task_system_ = new TaskSystem{};

      return task_system_;
    }
  }
  return task_system_;
}

tf::Future<void> MM::TaskSystem::TaskSystem::Run(
    const MM::TaskSystem::TaskType& task_type, Taskflow& task_flow) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run(task_flow);
}

tf::Future<void> MM::TaskSystem::TaskSystem::Run(
    const MM::TaskSystem::TaskType& task_type, Taskflow&& task_flow) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run(std::move(task_flow));
}

tf::Future<void> MM::TaskSystem::TaskSystem::RunN(
    const MM::TaskSystem::TaskType& task_type, Taskflow& task_flow, size_t N) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_n(task_flow, N);
}

tf::Future<void> MM::TaskSystem::TaskSystem::RunN(
    const MM::TaskSystem::TaskType& task_type, Taskflow&& task_flow, size_t N) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_n(std::move(task_flow), N);
}

void MM::TaskSystem::TaskSystem::WaitForAll(const TaskType& task_type) {
  auto& executor = ChooseExecutor(task_type);
  executor.wait_for_all();
}

size_t MM::TaskSystem::TaskSystem::NumWorkers(
    const TaskType& task_type) const noexcept {
  auto& executor = ChooseExecutor(task_type);
  return executor.num_workers();
}

size_t MM::TaskSystem::TaskSystem::NumTopologies(
    const TaskType& task_type) const {
  auto& executor = ChooseExecutor(task_type);
  return executor.num_topologies();
}

size_t MM::TaskSystem::TaskSystem::NumTaskFlows(
    const TaskType& task_type) const {
  auto& executor = ChooseExecutor(task_type);
  return executor.num_taskflows();
}

int MM::TaskSystem::TaskSystem::ThisWorkerId(const TaskType& task_type) const {
  auto& executor = ChooseExecutor(task_type);
  return executor.this_worker_id();
}

size_t MM::TaskSystem::TaskSystem::NumObservers(
    const TaskType& task_type) const noexcept {
  auto& executor = ChooseExecutor(task_type);
  return executor.num_observers();
}

bool MM::TaskSystem::TaskSystem::Destroy() {
  std::lock_guard<std::mutex> guard{sync_flag_};
  if (task_system_) {
    delete task_system_;
    task_system_ = nullptr;

    return true;
  }

  return true;
}

tf::Executor& MM::TaskSystem::TaskSystem::ChooseExecutor(
    const TaskType& task_type) {
  switch (task_type) {
    case TaskType::Common:
      return logic_executor_;
    case TaskType::Render:
      return render_executor_;
    case TaskType::Physical:
      return physical_executor_;
    case TaskType::Total:
      return total_executor_;
  }

  return total_executor_;
}

const tf::Executor& MM::TaskSystem::TaskSystem::ChooseExecutor(
    const TaskType& task_type) const {
  switch (task_type) {
    case TaskType::Common:
      return logic_executor_;
    case TaskType::Render:
      return render_executor_;
    case TaskType::Physical:
      return physical_executor_;
    case TaskType::Total:
      return total_executor_;
  }

  return total_executor_;
}
MM::TaskSystem::TaskSystem::TaskSystem()
    : total_executor_(3),
      logic_executor_(4),
      render_executor_(4),
      physical_executor_(4) {}
