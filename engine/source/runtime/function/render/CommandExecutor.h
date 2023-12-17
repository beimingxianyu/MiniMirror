//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <atomic>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

#include "runtime/function/render/CommandTaskFlow.h"
#include "runtime/function/render/RenderFuture.h"
#include "runtime/function/render/RenderResourceDataID.h"
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
  CommandExecutor(RenderEngine* engine,
                  const std::uint32_t& graph_command_number,
                  const std::uint32_t& compute_command_number,
                  const std::uint32_t& transform_command_number,
                  const std::uint32_t& waiting_coefficient = 3,
                  const std::uint32_t& max_post_command_task_number = 1024);
  CommandExecutor(const CommandExecutor& other) = delete;
  CommandExecutor(CommandExecutor&& other) = delete;
  CommandExecutor& operator=(const CommandExecutor& other) = delete;
  CommandExecutor& operator=(CommandExecutor&& other) = delete;

 public:
  std::uint32_t GetGraphCommandNumber() const;

  std::uint32_t GetComputeCommandNumber() const;

  std::uint32_t GetTransformCommandNumber() const;

  std::uint32_t GetFreeGraphCommandBufferNumber() const;

  std::uint32_t GetFreeComputeCommandBufferNumber() const;

  std::uint32_t GetFreeTransformCommandBufferNumber() const;

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
  Result<RenderFuture> Run(CommandTaskFlow&& command_task_flow);

  /**
   * \remark The \ref command_task_flow is invalid after call this function.
   */
  Result<RenderFutureState> RunAndWait(
      CommandTaskFlow&& command_task_flow);

  void LockExecutor();

  void UnlockExecutor();

  bool IsValid() const;

 private:
  struct CommandTaskFlowToBeRunExternalInfo {
    RenderFuture::RenderFutureStateManagerRef state_manager_{nullptr};

    std::uint32_t current_need_graph_command_buffer_count_{0};
    std::uint32_t current_need_compute_command_buffer_count_{0};
    std::uint32_t current_need_transform_command_buffer_count_{0};

    std::uint32_t reject_number_{0};
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
    CommandTaskExecutingExternalInfo() = default;
    ~CommandTaskExecutingExternalInfo() = default;
    CommandTaskExecutingExternalInfo(
        CommandTaskExecutingExternalInfo&& other) noexcept;
    CommandTaskExecutingExternalInfo& operator=(CommandTaskExecutingExternalInfo&& other);
    CommandTaskExecutingExternalInfo(
        const CommandTaskExecutingState& state,
        uint32_t requireCommandBufferCount,
        const std::uint32_t& preCommandTaskNotSubmitCount,
        std::vector<VkSemaphore>&& waitSemaphore,
        std::vector<VkSemaphore>&& signalSemaphore,
        std::vector<std::unique_ptr<AllocatedCommandBuffer>>&& commandBuffers,
        uint32_t waitFreeBufferCount);

    AtomicCommandTaskExecutingState state_{
        CommandTaskExecutingState::UNDEFINED};

    // include sub command task require command buffer.
    std::uint32_t require_command_buffer_count_{0};

    // include sub command task pre command not submit count.
    std::atomic_uint32_t pre_command_task_not_submit_count_{0};
    std::vector<VkSemaphore> wait_semaphore_{};
    std::vector<VkSemaphore> signal_semaphore_{};

    std::vector<std::unique_ptr<AllocatedCommandBuffer>> command_buffers_{};

    std::uint32_t wait_free_buffer_count_{0};
  };

  struct CommandTaskExecuting;
  struct CommandTaskFlowExecuting;

  struct CommandTaskExecutingContent {
    CommandTaskFlowExecuting* task_flow_{nullptr};
    CommandTaskID command_task_ID_{0};
    CommandType command_type_{CommandType::UNDEFINED};
    std::vector<TaskType> commands_{};
    std::vector<CommandTaskExecuting*> post_tasks_{};
    std::vector<CommandTaskExecuting*> sub_tasks_{};
    bool is_sub_task_{false};
    bool is_async_record_{false};
    std::vector<RenderResourceDataID> cross_task_flow_sync_render_resource_IDs_;
  };

  struct CommandTaskExecuting {
    using ExternalInfoType = CommandTaskExecutingExternalInfo;
    using ContentType = CommandTaskExecutingContent;

    CommandTaskExecuting()
      : command_task_(), external_info_() {
    }
    ~CommandTaskExecuting() = default;
    CommandTaskExecuting(CommandTaskFlowExecuting* command_task_flow,
                         CommandTask&& command_task);
    CommandTaskExecuting(const CommandTaskExecuting& other) = delete;
    CommandTaskExecuting(CommandTaskExecuting&& other) noexcept = default;
    CommandTaskExecuting& operator=(const CommandTaskExecuting& other) = delete;
    CommandTaskExecuting& operator=(CommandTaskExecuting&& other) noexcept = default;

    ContentType command_task_{};
    ExternalInfoType external_info_{};

    void ComputeRequireCommandBufferCountAndPreCommandTaskNotSubmitCount();

    void LinkPostCommandTaskAndSubTask(CommandTask&& command_task);

    bool AllPreCommandSubmit() const;
  };

  struct CommandTaskFlowExecutingExternalInfo {
    CommandExecutor* command_executor_{nullptr};
    RenderFuture::RenderFutureStateManagerRef state_manager_{nullptr};
    std::uint32_t completed_command_task_count_{0};

    std::uint32_t current_need_graph_command_buffer_count_{0};
    std::uint32_t current_need_compute_command_buffer_count_{0};
    std::uint32_t current_need_transform_command_buffer_count_{0};
  };

  struct CommandTaskFlowExecutingContent {
    CommandTaskFlowID task_flow_ID_{0};
    std::vector<CommandTaskExecuting> command_tasks_{};
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

  Result<Nil> InitCommandPolls(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  Result<Nil> InitCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  Result<Nil> InitGeneralAllocatedCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  Result<Nil> InitAllocatedCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  Result<Nil> InitSemaphores(
      const std::uint32_t& need_semaphore_number,
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  Result<Nil> AddCommandBuffer(
      const CommandType& command_type,
      const std::uint32_t& new_command_buffer_num);

 private:
  bool ExecutingCommandTaskIsComplete(
      const CommandTaskExecuting& command_task) const;

  std::vector<VkSemaphore> GetSemaphore(std::uint32_t require_number);

  bool WaitCroosCommandTaskFlowSync(
      const std::vector<RenderResourceDataID>&
          cross_task_flow_sync_render_resource_IDs_);

  std::stack<std::unique_ptr<AllocatedCommandBuffer>>&
  GetFreeCommandBufferStack(CommandBufferType command_buffer_type);

  Result<Nil> SubmitCommandTask(
      CommandTaskExecuting& command_task);

  bool HaveCommandTaskToBeProcess() const;

  void ProcessCompleteTask();

  void ProcessExecutingFailedOrCancelled();

  void ProcessWaitTaskFlow();

  std::array<std::uint32_t, 3> ProcessCurrentNeedCommandBufferCount();

  void ProcessWaitCommandTaskFlowQueue(
      std::array<std::uint32_t, 3> total_current_need_command_buffer_count);

  void ProcessExecutingCommandTaskFlowQueue();

  Result<Nil> RecordAndSubmitCommandSync(
      CommandTaskExecuting& command_task);

  void RecordAndSubmitCommandASync(CommandTaskExecuting& command_task);

  void ProcessTask();

 private:
  RenderEngine* render_engine_{nullptr};
  std::uint32_t graph_command_number_{0};
  std::uint32_t compute_command_number_{0};
  std::uint32_t transform_command_number_{0};
  std::uint32_t wait_coefficient_{3};

  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_graph_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_compute_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_transform_command_buffers_{};
  std::stack<VkSemaphore> free_semaphores_;

  std::vector<VkPipelineStageFlags> wait_dst_stage_mask_{
      1024,
      static_cast<VkPipelineStageFlags>(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)};

  std::array<std::array<std::unique_ptr<AllocatedCommandBuffer>, 3>, 3>
      general_command_buffers_{};
  std::mutex general_command_buffers_acquire_release_mutex_{};
  std::condition_variable
      general_command_buffers_acquire_release_condition_variable_{};

  bool processing_task_flow_queue_{false};

  std::mutex wait_task_flow_queue_mutex_{};
  WaitCommandTaskFlowQueueType wait_task_flow_queue_{};
  std::list<CommandTaskFlowID> need_wait_task_flow_IDs_{};
  WaitCommandTaskFlowQueueType need_wait_task_flow_queue_{};
  std::mutex wait_task_flows_mutex_{};

  ExecutingCommandTaskFlowQueueType executing_command_task_flow_queue_{};
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
