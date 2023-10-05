//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/RenderFuture.h"

std::mutex MM::RenderSystem::RenderFuture::wait_mutex_{};
std::condition_variable
    MM::RenderSystem::RenderFuture::wait_condition_variable_;

MM::RenderSystem::RenderFuture::RenderFuture(
    CommandExecutor* command_executor, const std::uint32_t& task_flow_ID,
    const std::shared_ptr<ExecuteResult>& future_execute_result,
    const std::shared_ptr<CommandCompleteState>& command_complete_state)
    : command_executor_(command_executor),
      task_flow_ID_(task_flow_ID),
      execute_result_(future_execute_result),
      command_complete_state_(command_complete_state) {}

MM::RenderSystem::RenderFuture& MM::RenderSystem::RenderFuture::operator=(
    const RenderFuture& other) {
  if (&other == this) {
    return *this;
  }

  task_flow_ID_ = other.task_flow_ID_;
  execute_result_ = other.execute_result_;
  command_complete_state_ = other.command_complete_state_;

  return *this;
}

MM::RenderSystem::RenderFuture& MM::RenderSystem::RenderFuture::operator=(
    RenderFuture&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  task_flow_ID_ = other.task_flow_ID_;
  execute_result_ = std::move(other.execute_result_);
  command_complete_state_ = std::move(other.command_complete_state_);

  task_flow_ID_ = 0;

  return *this;
}

MM::ExecuteResult MM::RenderSystem::RenderFuture::Get() {
  if (!IsValid()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::unique_lock<std::mutex> guard{command_executor_->wait_tasks_mutex_};
  command_executor_->wait_tasks_.emplace_back(task_flow_ID_);
  guard.unlock();

  if (command_complete_state_->load(std::memory_order_acquire)) {
    std::unique_lock<std::mutex> wait_guard{wait_mutex_};
    wait_condition_variable_.wait(wait_guard, [this_future = this]() {
      return this_future->command_complete_state_->load(
                 std::memory_order_acquire) == 0;
    });
    return *execute_result_;
  }
}

bool MM::RenderSystem::RenderFuture::IsValid() {
  return command_executor_ != nullptr;
}
