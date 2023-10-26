//
// Created by beimingxianyu on 23-10-5.
//

#include "runtime/function/render/RenderFuture.h"
#include <atomic>

#include "runtime/core/log/log_system.h"
#include "runtime/function/render/CommandExecutor.h"
#include "runtime/function/render/vk_command_pre.h"
#include "runtime/function/render/vk_type_define.h"
#include "utils/error.h"

MM::RenderSystem::RenderFuture::RenderFutureStateManager::
    RenderFutureStateManager(RenderFutureStateManager&& other) noexcept
    : state_(other.state_.load(std::memory_order_acquire)), cvm_(other.cvm_) {
  other.state_.store(RenderFutureState::UNDEFINED,
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

  other.state_.store(RenderFutureState::UNDEFINED,
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
    RenderFutureState state) {
  assert(IsValid());

  state_.store(state, std::memory_order_release);
}

MM::RenderSystem::RenderFutureState
MM::RenderSystem::RenderFuture::RenderFutureStateManager::GetState() const {
  assert(IsValid());

  return state_.load(std::memory_order_acquire);
}

void MM::RenderSystem::RenderFuture::RenderFutureStateManager::Wait() {
  std::unique_lock<std::mutex> um{cvm_->mutex_};
  cvm_->condition_variable_.wait(um);
}

MM::RenderSystem::RenderFuture::RenderFuture(RenderFuture&& other) noexcept
    : state_manager_(std::move(other.state_manager_)) {
}

MM::RenderSystem::RenderFuture& MM::RenderSystem::RenderFuture::operator=(
    RenderFuture&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  state_manager_ = std::move(other.state_manager_);

  return *this;
}
MM::RenderSystem::RenderFutureState MM::RenderSystem::RenderFuture::Wait() {
  assert(IsValid());

  RenderFutureState state = state_manager_->GetState();
  if (state == RenderFutureState::RUNNING) {
    state_manager_->Wait();
    state = state_manager_->GetState();
    assert(state != RenderFutureState::RUNNING);
  }

  return state;
}
void MM::RenderSystem::RenderFuture::Cancel() {
  assert(IsValid());

  state_manager_->SetState(RenderFutureState::CANCELLED);
}
bool MM::RenderSystem::RenderFuture::IsValid() const {
  return state_manager_ != nullptr;
}

MM::RenderSystem::RenderFuture::RenderFuture( 
    RenderFutureStateManagerRef state_manager)
    : state_manager_(std::move(state_manager)) {
  assert(IsValid());
}
