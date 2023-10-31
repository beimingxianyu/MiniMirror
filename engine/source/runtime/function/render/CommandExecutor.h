//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include <vulkan/vulkan_core.h>

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

#include "runtime/core/log/log_system.h"
#include "runtime/function/render/CommandTask.h"
#include "runtime/function/render/CommandTaskFlow.h"
#include "runtime/function/render/RenderFuture.h"
#include "runtime/function/render/vk_command_pre.h"
#include "runtime/function/render/vk_enum.h"
#include "runtime/function/render/vk_type_define.h"
#include "utils/error.h"

namespace MM {
namespace RenderSystem {
using CommandType = CommandBufferType;
class CommandExecutor {
  friend class RenderFuture;

 public:
  CommandExecutor() = delete;
  explicit CommandExecutor(RenderEngine* engine);
  ~CommandExecutor();
  CommandExecutor(RenderEngine* engine, const uint32_t& graph_command_number,
                  const uint32_t& compute_command_number,
                  const uint32_t& transform_command_number);
  CommandExecutor(const CommandExecutor& other) = delete;
  CommandExecutor(CommandExecutor&& other) = delete;
  CommandExecutor& operator=(const CommandExecutor& other) = delete;
  CommandExecutor& operator=(CommandExecutor&& other) = delete;

 public:
  std::uint32_t GetGraphCommandNumber() const;

  std::uint32_t GetComputeCommandNumber() const;

  std::uint32_t GetTransformCommandNumber() const;

  std::uint32_t GetFreeGraphCommandNumber() const;

  std::uint32_t GetFreeComputeCommandNumber() const;

  std::uint32_t GetFreeTransformCommandNumber() const;

  std::uint32_t GetFreeCommandNumber(CommandType command_type) const;

  bool IsFree() const;

  std::unique_ptr<AllocatedCommandBuffer> AcquireGeneralGraphCommandBuffer();

  std::unique_ptr<AllocatedCommandBuffer> AcquireGeneralComputeCommandBuffer();

  std::unique_ptr<AllocatedCommandBuffer>
  AcquireGeneralTransformCommandBuffer();

  void ReleaseGeneralCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>&& output);

  /**
   * \remark \ref command_task_flow is invalid after call this function.
   */
  Result<RenderFuture, ErrorResult> Run(CommandTaskFlow&& command_task_flow);

  /**
   * \remark The \ref command_task_flow is invalid after call this function.
   */
  Result<RenderFutureState, ErrorResult> RunAndWait(
      CommandTaskFlow&& command_task_flow);

  void LockExecutor();

  void UnlockExecutor();

  bool IsValid() const;

 private:
  struct CommandTaskFlowToBeRunExternalInfo {
    RenderFuture::RenderFutureStateManagerRef state_manager_{nullptr};
  };

  struct CommandTaskFlowToBeRun {
    using ExternalInfoType = CommandTaskFlowToBeRunExternalInfo;
    using ContentType = CommandTaskFlow;

    CommandTaskFlowToBeRun() = delete;
    ~CommandTaskFlowToBeRun() = default;
    explicit CommandTaskFlowToBeRun(CommandTaskFlow&& command_task_flow);
    CommandTaskFlowToBeRun(const CommandTaskFlowToBeRun& other) = delete;
    CommandTaskFlowToBeRun(CommandTaskFlowToBeRun&& other) noexcept = default;
    CommandTaskFlowToBeRun& operator=(const CommandTaskFlowToBeRun& other) =
        delete;
    CommandTaskFlowToBeRun& operator=(CommandTaskFlowToBeRun&& other) noexcept =
        default;

    ContentType command_task_flow_{};
    ExternalInfoType external_info_{};
  };

  struct CommandTaskExecutingExternalInfo {
    AtomicCommandTaskRunningState state_{CommandTaskExecutingState::UNDEFINED};

    // include sub command task require command buffer.
    std::uint32_t require_command_buffer_count_{0};

    std::atomic_uint32_t pre_command_task_not_submit_count_{0};
    std::vector<VkSemaphore> wait_semaphore_{};
    std::vector<VkSemaphore> signal_semaphore_{};
    std::vector<std::unique_ptr<AllocatedCommandBuffer>> command_buffers_{};

    // When the \ref waiting_coefficient is greater than the number of command
    // buffers required by the task, it will wait for the task.
    std::uint32_t wait_coefficient_{0};
  };

  struct CommandTaskExecuting;
  struct CommandTaskFlowExecuting;

  struct CommandTaskContent {
    CommandTaskFlowExecuting* task_flow_{nullptr};
    CommandTaskID command_task_ID_{0};
    CommandType command_type_{CommandType::UNDEFINED};
    std::vector<TaskType> commands_{};
    std::vector<CommandTaskID> post_tasks_{};
    std::vector<CommandTaskID> sub_tasks_{};
    bool is_sub_task_{false};
    bool is_async_record_{false};
    std::vector<RenderResourceDataID> cross_task_flow_sync_render_resource_IDs_;
  };

  struct CommandTaskExecuting {
    using ExternalInfoType = CommandTaskExecutingExternalInfo;
    using ContentType = CommandTaskContent;

    CommandTaskExecuting() = default;
    ~CommandTaskExecuting() = default;
    CommandTaskExecuting(CommandTaskFlowExecuting* command_task_flow,
                         MM::RenderSystem::CommandTask&& command_task);
    CommandTaskExecuting(const CommandTaskExecuting& other) = delete;
    CommandTaskExecuting(CommandTaskExecuting&& other) noexcept = delete;
    CommandTaskExecuting& operator=(const CommandTaskExecuting& other) = delete;
    CommandTaskExecuting& operator=(CommandTaskExecuting&& other) noexcept =
        delete;

    ContentType command_task_{};
    ExternalInfoType external_info_{};

    void ComputeRequireCommandBufferCount();
  };

  struct CommandTaskFlowExecutingExternalInfo {
    CommandExecutor* command_executor_{nullptr};
    RenderFuture::RenderFutureStateManagerRef state_manager_{nullptr};
    std::uint32_t completed_command_task_count_{0};

    std::uint32_t current_need_graph_command_buffers_{0};
    std::uint32_t current_need_compute_command_buffers_{0};
    std::uint32_t current_need_transform_command_buffers_{0};
  };

  struct CommandTaskFlowExecutingContent {
    CommandTaskFlowID task_flow_ID_{0};
    std::vector<CommandTaskID> command_task_IDs_{};
  };

  struct CommandTaskFlowExecuting {
    using ContentType = CommandTaskFlowExecutingContent;
    using ExternalInfoType = CommandTaskFlowExecutingExternalInfo;

    CommandTaskFlowExecuting() = default;
    ~CommandTaskFlowExecuting();
    CommandTaskFlowExecuting(
        CommandExecutor* command_executor,
        CommandTaskFlowToBeRun&& command_task_flow_to_be_run);
    CommandTaskFlowExecuting(const CommandTaskFlowExecuting& other) = delete;
    CommandTaskFlowExecuting(CommandTaskFlowExecuting&& other) noexcept =
        default;
    CommandTaskFlowExecuting& operator=(const CommandTaskFlowExecuting& other) =
        delete;
    CommandTaskFlowExecuting& operator=(
        CommandTaskFlowExecuting&& other) noexcept = default;

    ContentType command_task_flow_{};
    ExternalInfoType external_info_{};
  };

  using WaitCommandTaskFlowQueueType = std::list<CommandTaskFlowToBeRun>;
  using ExecutingCommandTaskFlowQueueType = std::list<CommandTaskFlowExecuting>;
  using ExecutingCommandTaskMapType =
      std::unordered_map<CommandTaskID, CommandTaskExecuting>;

 private:
  void ClearWhenConstructFailed(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  Result<Nil, ErrorResult> InitCommandPolls(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  Result<Nil, ErrorResult> InitCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  Result<Nil, ErrorResult> InitGeneralAllocatedCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  Result<Nil, ErrorResult> InitAllocatedCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  Result<Nil, ErrorResult> InitSemaphores(
      const std::uint32_t& need_semaphore_number,
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  Result<Nil, ErrorResult> AddCommandBuffer(
      const CommandType& command_type,
      const std::uint32_t& new_command_buffer_num);

 private:
  bool ExecutingCommandTaskIsComplete(
      const CommandTaskExecuting& command_task) const;

  std::vector<VkSemaphore> GetSemaphore(std::uint32_t require_number);

  bool HaveCommandTaskToBeProcess() const;

  void AddCommandTaskFlowToExecutingQueue(
      WaitCommandTaskFlowQueueType::iterator command_task_flow_to_be_run_iter);

  void ProcessCompleteTask();

  void ProcessExecutingFailedOrCancelled();

  void ProcessWaitTask();

  void ProcessCommandFlowList();

  void ProcessPreTaskNoSubmitTask();

  void ProcessRootTaskAndSubTask();

  void ChooseVariableByCommandType();

  void ProcessCrossTaskFlowRenderResourceSync();

  void ProcessOneCanSubmitTask();

  void ProcessWhenOneFailedSubmit();

  void ProcessNextStepCanSubmitTask();

  Result<Nil, ErrorResult> RecordAndSubmitCommandSync();

  Result<Nil, ErrorResult> RecordAndSubmitCommandASync();

  void PostProcessOfSubmitTask();

  void SubmitTasksSync();

  void SubmitTaskAsync();

  void SubmitTasks();

  void ProcessTask();

 private:
  RenderEngine* render_engine_{nullptr};
  std::uint32_t graph_command_number_{0};
  std::uint32_t compute_command_number_{0};
  std::uint32_t transform_command_number_{0};

  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_graph_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_compute_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_transform_command_buffers_{};
  std::stack<VkSemaphore> free_semaphores_;

  std::array<std::array<std::unique_ptr<AllocatedCommandBuffer>, 3>, 3>
      general_command_buffers_{};
  std::mutex general_command_buffers_acquire_release_mutex_{};
  std::condition_variable
      general_command_buffers_acquire_release_condition_variable_{};

  bool processing_task_flow_queue_{false};
  std::mutex task_flow_queue_mutex_{};

  WaitCommandTaskFlowQueueType wait_task_flow_queue_{};

  std::list<CommandTaskFlowID> wait_task_flows_;
  std::mutex wait_task_flows_mutex_{};

  ExecutingCommandTaskFlowQueueType executing_task_flow_queue_{};
  ExecutingCommandTaskMapType executing_command_task_map_{};
  std::unordered_map<RenderResourceDataID, std::list<CommandTaskID>>
      block_render_resources_;

  std::list<std::unique_ptr<AllocatedCommandBuffer>>
      submit_failed_to_be_recovery_command_buffer_{};
  std::mutex submit_failed_to_be_recovery_command_buffer_mutex_{};

  std::atomic_uint32_t lock_count_{};
  std::mutex task_flow_submit_during_lockdown_mutex_{};
  std::list<CommandTaskFlowToBeRun> task_flow_submit_during_lockdown_{};
};

class CommandExecutorLockGuard {
 public:
  CommandExecutorLockGuard() = default;
  ~CommandExecutorLockGuard();
  explicit CommandExecutorLockGuard(CommandExecutor& command_executor);
  CommandExecutorLockGuard(const CommandExecutorLockGuard& other) = delete;
  CommandExecutorLockGuard(CommandExecutorLockGuard&& other) noexcept;
  CommandExecutorLockGuard& operator=(const CommandExecutorLockGuard& other) =
      delete;
  CommandExecutorLockGuard& operator=(
      CommandExecutorLockGuard&& other) noexcept;

 public:
  void Lock();

  bool IsLocked() const;

  void Unlock();

  bool IsValid() const;

 private:
  CommandExecutor* command_executor_{nullptr};

  std::atomic_bool lock_flag_{};
};

class CommandExecutorGeneralCommandBufferGuard {
 public:
  CommandExecutorGeneralCommandBufferGuard(
      CommandExecutor& command_executor, CommandBufferType command_buffer_type);
  ~CommandExecutorGeneralCommandBufferGuard();
  CommandExecutorGeneralCommandBufferGuard(
      const CommandExecutorGeneralCommandBufferGuard& other) = delete;
  CommandExecutorGeneralCommandBufferGuard(
      CommandExecutorGeneralCommandBufferGuard&& other) noexcept;
  CommandExecutorGeneralCommandBufferGuard& operator=(
      const CommandExecutorGeneralCommandBufferGuard& other) = delete;
  CommandExecutorGeneralCommandBufferGuard& operator=(
      CommandExecutorGeneralCommandBufferGuard&& other) noexcept;

 public:
  AllocatedCommandBuffer* GetGeneralCommandBuffer();

  void Release();

  bool IsValid() const;

 private:
  CommandExecutor* command_executor_{nullptr};
  std::unique_ptr<AllocatedCommandBuffer> general_command_buffer_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM