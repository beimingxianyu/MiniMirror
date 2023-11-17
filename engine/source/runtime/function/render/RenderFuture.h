//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "runtime/function/render/CommandExecutor.h"
#include "runtime/function/render/RenderFuture.h"
#include "runtime/function/render/vk_command_pre.h"
#include "runtime/function/render/vk_enum.h"
#include "runtime/function/render/vk_type_define.h"
#include "utils/error.h"

namespace MM {
namespace RenderSystem {
class RenderFuture {
  friend class CommandExecutor;

 private:
  struct RenderFutureCVM {
    std::mutex mutex_;
    std::condition_variable condition_variable_;
  };

  class RenderFutureStateManager {
    friend class CommandExecutor;

   public:
    RenderFutureStateManager() = default;
    ~RenderFutureStateManager() = default;
    RenderFutureStateManager(const RenderFutureStateManager& other) = delete;
    RenderFutureStateManager(RenderFutureStateManager&& other) noexcept;
    RenderFutureStateManager& operator=(const RenderFutureStateManager& other) =
        delete;
    RenderFutureStateManager& operator=(
        RenderFutureStateManager&& other) noexcept;

   public:
    bool IsValid() const;

    void Notify();

    void SetState(RenderFutureState state);

    RenderFutureState GetState() const;

    void Wait();

    template <class Rep, class Period>
    bool WaitFor(const std::chrono::duration<Rep, Period>& rel_time) {
      std::unique_lock<std::mutex> um{cvm_->mutex_};
      CommandTaskFlowExecutingState state =
          state_.load(std::memory_order_relaxed);

      if (state == CommandTaskFlowExecutingState::WAIT ||
          state == CommandTaskFlowExecutingState::RUNNING) {
        return cvm_->condition_variable_.wait_for(um, rel_time) !=
               std::cv_status::timeout;
      }
    }

    template <class Rep, class Period>
    bool WaitUntil(const std::chrono::duration<Rep, Period>& rel_time) {
      std::unique_lock<std::mutex> um{cvm_->mutex_};
      CommandTaskFlowExecutingState state =
          state_.load(std::memory_order_relaxed);

      if (state == CommandTaskFlowExecutingState::WAIT ||
          state == CommandTaskFlowExecutingState::RUNNING) {
        return cvm_->condition_variable_.wait_until(um, rel_time) !=
               std::cv_status::timeout;
      }
    }

   private:
    AtomicRenderFutureState state_{RenderFutureState::WAIT};
    RenderFutureCVM* cvm_{new RenderFutureCVM{}};
  };

 public:
  using RenderFutureStateManagerRef = std::shared_ptr<RenderFutureStateManager>;

 public:
  RenderFuture() = default;
  ~RenderFuture() = default;
  explicit RenderFuture(CommandExecutor* command_executor,
                        CommandTaskFlowID command_task_flow_ID,
                        RenderFutureStateManagerRef state_manager);
  RenderFuture(const RenderFuture& other) = delete;
  RenderFuture(RenderFuture&& other) noexcept;
  RenderFuture& operator=(const RenderFuture& other) = delete;
  RenderFuture& operator=(RenderFuture&& other) noexcept;

 private:
  void AddToWaitList();

 public:
  RenderFutureState Wait();

  template <class Rep, class Period>
  std::pair<RenderFutureState, bool> WaitFor(
      const std::chrono::duration<Rep, Period>& rel_time) {
    assert(IsValid());

    bool wait_result = true;
    RenderFutureState state;
    ;
    if (state_manager_->GetState() == RenderFutureState::WAIT) {
      AddToWaitList();
      wait_result = state_manager_->WaitFor(rel_time);
      state = state_manager_->GetState();
    } else if (state_manager_->GetState() == RenderFutureState::RUNNING) {
      wait_result = state_manager_->WaitFor(rel_time);
      state = state_manager_->GetState();
    }

    return std::make_pair(state, wait_result);
  }

  template <class Rep, class Period>
  std::pair<RenderFutureState, bool> WaitUntil(
      const std::chrono::duration<Rep, Period>& rel_time) {
    assert(IsValid());

    bool wait_result = true;
    RenderFutureState state;
    if (state_manager_->GetState() == RenderFutureState::WAIT) {
      AddToWaitList();
      wait_result = state_manager_->WaitUntil(rel_time);
      state = state_manager_->GetState();
    } else if (state_manager_->GetState() == RenderFutureState::RUNNING) {
      wait_result = state_manager_->WaitUntil(rel_time);
      state = state_manager_->GetState();
    }

    return std::make_pair(state, wait_result);
  }

  void Cancel();

  bool IsValid() const;

 private:
  CommandExecutor* command_executor_{nullptr};
  CommandTaskFlowID command_task_flow_ID_{0};
  RenderFutureStateManagerRef state_manager_{nullptr};
};
}  // namespace RenderSystem
}  // namespace MM
