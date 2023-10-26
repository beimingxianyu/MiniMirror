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

      void SetState(RenderFutureState state);

      RenderFutureState GetState() const;

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
      AtomicRenderFutureState state_{RenderFutureState::RUNNING};
      RenderFutureCVM* cvm_{new RenderFutureCVM{}};
  };

 public:
  using RenderFutureStateManagerRef = std::shared_ptr<RenderFutureStateManager>;

 public:
  RenderFuture() = default;
  ~RenderFuture() = default;
  RenderFuture(const RenderFuture& other) = delete;
  RenderFuture(RenderFuture&& other) noexcept;
  RenderFuture& operator=(const RenderFuture& other) = delete;
  RenderFuture& operator=(RenderFuture&& other) noexcept;

 public:
  RenderFutureState Wait();

  template< class Rep, class Period >
  RenderFutureState WaitFor(const std::chrono::duration<Rep, Period>& rel_time) {
    assert(IsValid());

    RenderFutureState state = state_manager_->GetState();
    if (state == RenderFutureState::RUNNING) {
      state_manager_->WaitFor(rel_time); 
      state = state_manager_->GetState();
    }
    
    return state;
  }

  template< class Rep, class Period >
  RenderFutureState WaitUntil(const std::chrono::duration<Rep, Period>& rel_time) {
    assert(IsValid());

    RenderFutureState state = state_manager_->GetState();
    if (state == RenderFutureState::RUNNING) {
      state_manager_->WaitUntil(rel_time); 
      state = state_manager_->GetState();
    }
    
    return state;
  }

  void Cancel();

  bool IsValid() const;

 private:
  explicit RenderFuture(RenderFutureStateManagerRef state_manager);

 private:
  static std::mutex wait_mutex_;
  static std::condition_variable wait_condition_variable_;

 private:
  RenderFutureStateManagerRef state_manager_{nullptr};
};
}  // namespace RenderSystem
}  // namespace MM