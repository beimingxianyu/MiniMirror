//
// Created by beimingxianyu on 23-10-5.
//
#include "runtime/function/render/vk_command_pre.h"

namespace MM {
namespace RenderSystem {
// It is convenient for future expansion.
using CommandCompleteState = std::atomic_uint32_t;

class RenderFuture {
  friend class CommandExecutor;

 public:
  RenderFuture() = default;
  ~RenderFuture() = default;
  RenderFuture(const RenderFuture& other) = default;
  RenderFuture(RenderFuture&& other) noexcept = default;
  RenderFuture& operator=(const RenderFuture& other);
  RenderFuture& operator=(RenderFuture&& other) noexcept;

 public:
  ExecuteResult Get();

  bool IsValid();

 private:
  RenderFuture(
      CommandExecutor* command_executor, const std::uint32_t& task_flow_ID,
      const std::shared_ptr<ExecuteResult>& future_execute_result,
      const std::shared_ptr<CommandCompleteState>& command_complete_states);

 private:
  static std::mutex wait_mutex_;
  static std::condition_variable wait_condition_variable_;

 private:
  CommandExecutor* command_executor_{nullptr};
  std::uint32_t task_flow_ID_{0};
  std::shared_ptr<ExecuteResult> execute_result_{};
  std::shared_ptr<CommandCompleteState> command_complete_state_{0};
};
}  // namespace RenderSystem
}  // namespace MM