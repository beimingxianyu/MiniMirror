//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/RenderFuture.h"
#include <atomic>

#include "runtime/function/render/CommandExecutor.h"
#include "runtime/function/render/vk_command_pre.h"
#include "runtime/function/render/vk_type_define.h"
#include "utils/error.h"

MM::RenderSystem::RenderFuture::RenderFutureStateManager::
    RenderFutureStateManager(RenderFutureStateManager&& other) noexcept
    : state_(other.state_.load(std::memory_order_acquire)), cvm_(other.cvm_) {
  other.state_.store(RenderFutureStateEnum::UNDEFINED,
                     std::memory_order_release);
  other.cvm_ = nullptr;
}

MM::RenderSystem::RenderFuture::RenderFutureStateManager&
MM::RenderSystem::RenderFuture::RenderFutureStateManager::operator=(
    RenderFutureStateManager&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  state_.store(other.state_.load(std::memory_order_acquire),
               std::memory_order_release);
  cvm_ = other.cvm_;

  other.state_.store(RenderFutureStateEnum::UNDEFINED,
                     std::memory_order_release);
  other.cvm_ = nullptr;

  return *this;
}
bool MM::RenderSystem::RenderFuture::RenderFutureStateManager::IsValid() const {
  return cvm_ != nullptr;
}
void MM::RenderSystem::RenderFuture::RenderFutureStateManager::Notify() {
  assert(IsValid());

  cvm_->condition_variable_.notify_one();
}
void MM::RenderSystem::RenderFuture::RenderFutureStateManager::SetState(
    RenderFutureStateEnum state) {
  assert(IsValid());

  state_.store(state, std::memory_order_release);
}
MM::RenderSystem::RenderFutureStateEnum
MM::RenderSystem::RenderFuture::RenderFutureStateManager::GetState() const {
  assert(IsValid());

  return state_.load(std::memory_order_acquire);
}
void MM::RenderSystem::RenderFuture::RenderFutureStateManager::Wait() {
  std::unique_lock<std::mutex> um{cvm_->mutex_};
  cvm_->condition_variable_.wait(um);
}
MM::RenderSystem::RenderFuture::RenderFuture(RenderFuture&& other) noexcept
    : command_executor_(other.command_executor_),
      task_flow_ID_(other.task_flow_ID_),
      state_manager_(std::move(other.state_manager_)) {
  other.command_executor_ = nullptr;
  other.task_flow_ID_ = 0;
}
MM::RenderSystem::RenderFuture& MM::RenderSystem::RenderFuture::operator=(
    RenderFuture&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  command_executor_ = other.command_executor_;
  task_flow_ID_ = other.task_flow_ID_;
  state_manager_ = std::move(other.state_manager_);

  other.command_executor_ = nullptr;
  other.task_flow_ID_ = 0;

  return *this;
}
MM::RenderSystem::RenderFutureStateEnum MM::RenderSystem::RenderFuture::Wait() {
  assert(IsValid());

  RenderFutureStateEnum state = state_manager_->GetState();
  if (state == RenderFutureStateEnum::RUNNING) {
    state_manager_->Wait();
    state = state_manager_->GetState();
    assert(state != RenderFutureStateEnum::RUNNING);
  }

  return state;
}
void MM::RenderSystem::RenderFuture::Cancel() {
  assert(IsValid());

  state_manager_->SetState(RenderFutureStateEnum::CANCELLED);
}
bool MM::RenderSystem::RenderFuture::IsValid() const {
  return state_manager_ != nullptr;
}
MM::RenderSystem::RenderFuture::RenderFuture(
    CommandExecutor* command_executor, const CommandTaskFlowID& task_flow_ID,
    std::shared_ptr<RenderFutureStateManager> state_manager)
    : command_executor_(command_executor),
      task_flow_ID_(task_flow_ID),
      state_manager_(std::move(state_manager)) {
  assert(IsValid());
}
