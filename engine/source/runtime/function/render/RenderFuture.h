//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

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
    public:
      RenderFutureStateManager() = default;
      ~RenderFutureStateManager() = default;
      RenderFutureStateManager(const RenderFutureStateManager& other) = delete;
      RenderFutureStateManager(RenderFutureStateManager&& other) noexcept;
      RenderFutureStateManager& operator=(const RenderFutureStateManager& other) = delete;
      RenderFutureStateManager& operator=(
          RenderFutureStateManager&& other) noexcept;

     public:
      bool IsValid() const;

      void Notify();

      void SetState(RenderFutureStateEnum state);

      RenderFutureStateEnum GetState() const;

      void Wait();

      template< class Rep, class Period >
      bool WaitFor(const std::chrono::duration<Rep, Period>& rel_time) {
        std::unique_lock<std::mutex> um{cvm_->mutex_};
        return cvm_->condition_variable_.wait_for(um, rel_time) != std::cv_status::timeout;
      }

      template< class Rep, class Period >
      bool WaitUntil(const std::chrono::duration<Rep, Period>& rel_time) {
        std::unique_lock<std::mutex> um{cvm_->mutex_};
        return cvm_->condition_variable_.wait(um, rel_time) != std::cv_status::timeout;
      }

    private:
      RenderFutureState state_{RenderFutureStateEnum::RUNNING};
      RenderFutureCVM* cvm_{nullptr};
  };

 public:
  RenderFuture() = delete;
  ~RenderFuture() = default;
  RenderFuture(const RenderFuture& other) = delete;
  RenderFuture(RenderFuture&& other) noexcept;
  RenderFuture& operator=(const RenderFuture& other) = delete;
  RenderFuture& operator=(RenderFuture&& other) noexcept;

 public:
  RenderFutureStateEnum Wait();

  template< class Rep, class Period >
  RenderFutureStateEnum WaitFor(const std::chrono::duration<Rep, Period>& rel_time) {
    assert(IsValid());

    RenderFutureStateEnum state = state_manager_->GetState();
    if (state == RenderFutureStateEnum::RUNNING) {
      state_manager_->WaitFor(rel_time); 
      state = state_manager_->GetState();
    }
    
    return state;
  }

  template< class Rep, class Period >
  RenderFutureStateEnum WaitUntil(const std::chrono::duration<Rep, Period>& rel_time) {
    assert(IsValid());

    RenderFutureStateEnum state = state_manager_->GetState();
    if (state == RenderFutureStateEnum::RUNNING) {
      state_manager_->WaitUntil(rel_time); 
      state = state_manager_->GetState();
    }
    
    return state;
  }

  void Cancel();

  bool IsValid() const;

 private:
  RenderFuture(CommandExecutor* command_executor,
               const CommandTaskFlowID& task_flow_ID,
               std::shared_ptr<RenderFutureStateManager> state_manager);

 private:
  static std::mutex wait_mutex_;
  static std::condition_variable wait_condition_variable_;

 private:
  CommandExecutor* command_executor_{nullptr};
  CommandTaskFlowID task_flow_ID_{0};
  std::shared_ptr<RenderFutureStateManager> state_manager_{nullptr};
};
}  // namespace RenderSystem
}  // namespace MM