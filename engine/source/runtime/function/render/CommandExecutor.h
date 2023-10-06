//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include "runtime/function/render/CommandTask.h"
#include "runtime/function/render/CommandTaskFlow.h"
#include "runtime/function/render/RenderFuture.h"

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

  void AcquireGeneralGraphCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>& output);

  Result<Nil, ErrorResult> AcquireGeneralComputeCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>& output);

  Result<Nil, ErrorResult> AcquireGeneralTransformCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>& output);

  void ReleaseGeneralCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>&& output);

  /**
   * \remark \ref command_task_flow is invalid after call this function.
   */
  RenderFuture Run(CommandTaskFlow&& command_task_flow);

  /**
   * \remark The \ref command_task_flow is invalid after call this function.
   */
  Result<Nil, ErrorResult> RunAndWait(CommandTaskFlow&& command_task_flow);

  /**
   * \remark \ref command_task_flow is invalid after call this function.
   */
  RenderFuture Run(CommandTaskFlow& command_task_flow);

  /**
   * \remark The \ref command_task_flow is invalid after call this function.
   */
  Result<Nil, ErrorResult> RunAndWait(CommandTaskFlow& command_task_flow);

  // TODO RunOneFrame
  ///**
  // * \brief The \ref command_task_flow contain all render commands in one
  // frame,
  // * and will be use in next frame if the render state not change.
  // * \remark This function must be the last call to the rendering call,
  // * otherwise this call is invalid.
  // * \remark Default wait this call.
  // */
  // RenderFuture RunOneFrame(CommandTaskFlow& command_task_flow);

  void LockExecutor();

  void UnlockExecutor();

  bool IsValid() const;

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

  struct CommandTaskFlowToBeRun {
    CommandTaskFlowToBeRun() = delete;
    ~CommandTaskFlowToBeRun() = default;
    CommandTaskFlowToBeRun(
        CommandTaskFlow&& command_task_flow, const std::uint32_t& task_flow_ID,
        const std::shared_ptr<Result<Nil, ErrorResult>>& execute_result,
        const std::shared_ptr<CommandCompleteState>& complete_state);
    CommandTaskFlowToBeRun(const CommandTaskFlowToBeRun& other) = delete;
    CommandTaskFlowToBeRun(CommandTaskFlowToBeRun&& other) noexcept;
    CommandTaskFlowToBeRun& operator=(const CommandTaskFlowToBeRun& other) =
        delete;
    CommandTaskFlowToBeRun& operator=(CommandTaskFlowToBeRun&& other) noexcept;

    CommandTaskFlow command_task_flow_{};
    std::uint32_t task_flow_ID_{0};
    std::shared_ptr<Result<Nil, ErrorResult>> execute_result_{};
    std::weak_ptr<CommandCompleteState> complete_state_{};

    std::uint32_t the_maximum_number_of_graph_buffers_required_for_one_task_{0};
    std::uint32_t the_maximum_number_of_compute_buffers_required_for_one_task_{
        0};
    std::uint32_t
        the_maximum_number_of_transform_buffers_required_for_one_task_{0};
  };

  enum class CommandBufferState {
    Recording,
    Executiong,
  };

  struct CommandTaskRenderResourceState {
    std::uint32_t task_flow_ID_{0};
    std::uint32_t task_ID_{0};
    CommandType command_type_{};
    CommandBufferState command_buffer_state_{};
  };

  struct CommandTaskToBeSubmit;

  struct ExecutingCommandTaskFlow {
    ExecutingCommandTaskFlow() = delete;
    ~ExecutingCommandTaskFlow() = default;
    explicit ExecutingCommandTaskFlow(
        CommandTaskFlowToBeRun&& command_task_flow_to_be_run);
    ExecutingCommandTaskFlow(const ExecutingCommandTaskFlow& other) = delete;
    ExecutingCommandTaskFlow(ExecutingCommandTaskFlow&& other) noexcept;
    ExecutingCommandTaskFlow& operator=(const ExecutingCommandTaskFlow& other) =
        delete;
    ExecutingCommandTaskFlow& operator=(
        ExecutingCommandTaskFlow&& other) noexcept;

    std::uint32_t GetRequireGraphCommandBufferNumber() const;

    std::uint32_t GetRequireComputeCommandBufferNumber() const;

    std::uint32_t GetRequireTransformCommandBufferNumber() const;

    std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>
    GetRequireCommandBufferNumber() const;

    CommandTaskFlow command_task_flow_{};
    std::uint32_t task_flow_ID_{0};
    bool initialize_or_not_{false};
    bool have_wait_one_task_{false};
    std::shared_ptr<Result<Nil, ErrorResult>> execute_result_{};
    std::weak_ptr<CommandCompleteState> complete_state_{};

    std::unordered_set<std::unique_ptr<CommandTask>*>
        task_that_have_already_been_accessed_;
    // TODO Use reference counting instead of this container to improve
    // performance when reference counting equal parent task number, this task
    // can be submit.
    std::unordered_set<std::unique_ptr<CommandTask>*> submitted_task_;
    std::list<CommandTaskToBeSubmit> can_be_submitted_tasks_;
    std::list<CommandTaskToBeSubmit> pre_task_not_submit_task_;

    std::mutex submitted_task_mutex_{};
  };

  static void UpdateCommandTaskLine(
      ExecutingCommandTaskFlow& command_task_flow,
      std::unique_ptr<CommandTask>& new_command_task,
      std::unique_ptr<CommandTask>* old_command_task_ptr);

  struct CommandTaskToBeSubmit {
    CommandTaskToBeSubmit() = delete;
    ~CommandTaskToBeSubmit() = default;
    CommandTaskToBeSubmit(
        CommandExecutor* command_executor,
        const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
        std::unique_ptr<CommandTask>&& command_task,
        const std::vector<VkSemaphore>& default_wait_semaphore,
        const std::vector<VkSemaphore>& default_signal_semaphore);
    CommandTaskToBeSubmit(
        CommandExecutor* command_executor,
        const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
        std::unique_ptr<CommandTask>&& command_task,
        const std::vector<VkSemaphore>& default_wait_semaphore,
        const std::vector<VkSemaphore>& default_signal_semaphore,
        std::vector<std::uint32_t>&& post_task_sub_task_numbers);
    CommandTaskToBeSubmit(const CommandTaskToBeSubmit& other) = delete;
    CommandTaskToBeSubmit(CommandTaskToBeSubmit&& other) noexcept;
    CommandTaskToBeSubmit& operator=(const CommandTaskToBeSubmit& other) =
        delete;
    CommandTaskToBeSubmit& operator=(CommandTaskToBeSubmit&& other) noexcept;

    CommandExecutor* command_executor_{nullptr};
    std::uint32_t task_flow_ID_{0};
    std::weak_ptr<ExecutingCommandTaskFlow> command_task_flow_;
    std::unique_ptr<CommandTask> command_task_{nullptr};
    std::vector<std::vector<VkSemaphore>> default_wait_semaphore_{};
    std::vector<std::vector<VkSemaphore>> default_signal_semaphore_{};
    std::vector<std::uint32_t> post_task_sub_task_numbers_{};

    std::uint32_t require_command_buffer_{0};
    std::uint32_t require_command_buffer_include_sub_task_{0};

    // When the \ref waiting_coefficient is greater than the number of command
    // buffers required by the task, it will wait for the task.
    std::uint32_t wait_coefficient_{0};

    bool operator<(const CommandTaskToBeSubmit& other) const;
  };

  struct ExecutingTask {
    ExecutingTask() = delete;
    ~ExecutingTask() = default;
    ExecutingTask(
        RenderEngine* engine, CommandExecutor* command_executor,
        const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
        std::vector<std::unique_ptr<AllocatedCommandBuffer>>&& command_buffer,
        std::unique_ptr<CommandTask>&& command_task,
        const std::weak_ptr<Result<Nil, ErrorResult>>& execute_result,
        const std::weak_ptr<CommandCompleteState>& complete_state,
        std::vector<std::vector<VkSemaphore>>&& default_wait_semaphore,
        std::vector<std::vector<VkSemaphore>>&& default_signal_semaphore);
    ExecutingTask(const ExecutingTask& other) = delete;
    ExecutingTask(ExecutingTask&& other) noexcept;
    ExecutingTask& operator=(ExecutingTask&& other) noexcept;
    ExecutingTask& operator=(const ExecutingTask& other) = delete;

    RenderEngine* render_engine_;
    CommandExecutor* command_executor_;
    std::uint32_t task_flow_ID_{0};
    std::weak_ptr<ExecutingCommandTaskFlow> command_task_flow_;
    std::vector<std::unique_ptr<AllocatedCommandBuffer>> command_buffers_;
    std::unique_ptr<CommandTask> command_task_{};
    std::weak_ptr<CommandCompleteState> complete_state_{};

    std::vector<std::vector<VkSemaphore>> wait_semaphore_;
    std::vector<std::vector<VkPipelineStageFlags>> wait_semaphore_stages_;
    std::vector<std::vector<VkSemaphore>> signal_semaphore_;
    std::uint32_t default_wait_semaphore_number_{0};
    std::uint32_t default_signal_semaphore_number_{0};

    bool IsComplete() const;
  };

  std::vector<VkSemaphore> GetSemaphore(std::uint32_t require_number);

  void RecycledSemaphoreThatSubmittedFailed();

  void ProcessCompleteTask();

  void ProcessWaitTask();

  void ProcessCommandFlowList();

  void ProcessPreTaskNoSubmitTask(
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      const CommandTaskToBeSubmit& to_be_submit_task);

  void ProcessRootTaskAndSubTask(
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow);

  void ChooseVariableByCommandType(
      CommandType command_type, std::uint32_t*& free_command_buffer_number,
      std::stack<std::unique_ptr<AllocatedCommandBuffer>>*&
          free_command_buffers,
      std::list<ExecutingTask>*& executing_task_list,
      std::atomic_uint32_t*& recording_command_buffer_number,
      std::uint32_t free_graph_command_buffer_number,
      std::uint32_t free_compute_command_buffer_number,
      std::uint32_t free_transform_command_buffer_number);

  void ProcessCrossTaskFlowRenderResourceSync(
      std::list<CommandTaskToBeSubmit>::iterator& command_task_to_be_submit,
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      bool& command_submit_condition);

  void ProcessOneCanSubmitTask(
      std::list<CommandTaskToBeSubmit>::iterator& command_task_to_be_submit,
      std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      std::uint32_t& number_of_blocked_cross_task_flow);

  void ProcessWhenOneFailedSubmit(
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow);

  void ProcessNextStepCanSubmitTask(
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow);

  Result<Nil, ErrorResult> RecordAndSubmitCommandSync(
      std::unique_ptr<ExecutingTask>& input_tasks);

  Result<Nil, ErrorResult> RecordAndSubmitCommandASync(
      std::unique_ptr<ExecutingTask>& input_tasks);

  void PostProcessOfSubmitTask(
      std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      std::unique_ptr<ExecutingTask>& input_tasks,
      Result<Nil, ErrorResult>& result);

  void SubmitTasksSync(
      std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      std::unique_ptr<ExecutingTask>&& input_tasks);

  void SubmitTaskAsync(
      std::shared_ptr<ExecutingCommandTaskFlow> command_task_flow,
      std::unique_ptr<ExecutingTask>&& input_tasks);

  void SubmitTasks(
      std::shared_ptr<
          MM::RenderSystem::CommandExecutor::ExecutingCommandTaskFlow>&
          command_task_flow,
      std::unique_ptr<ExecutingTask>&& input_tasks);

  void ProcessTask();

 private:
  RenderEngine* render_engine_{nullptr};
  std::uint32_t graph_command_number_{0};
  std::uint32_t compute_command_number_{0};
  std::uint32_t transform_command_number_{0};

  bool valid_{true};

  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_graph_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_compute_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>>
      free_transform_command_buffers_{};

  std::array<std::array<std::unique_ptr<AllocatedCommandBuffer>, 3>, 3>
      general_command_buffers_{};
  std::mutex general_command_buffers_acquire_release_mutex_{};
  std::condition_variable
      general_command_buffers_acquire_release_condition_variable_{};
  bool general_garph_command_wait_flag_{false};
  bool general_compute_command_wait_flag_{false};
  bool general_transform_command_wait_flag_{false};

  std::atomic_uint32_t recoding_graph_command_buffer_number_{0};
  std::atomic_uint32_t recording_compute_command_buffer_number_{0};
  std::atomic_uint32_t recording_transform_command_buffer_number_{0};

  std::list<ExecutingTask> executing_graph_command_buffers_{};
  std::list<ExecutingTask> executing_compute_command_buffers_{};
  std::list<ExecutingTask> executing_transform_command_buffers_{};
  mutable std::mutex executing_graph_command_buffers_mutex_{};
  mutable std::mutex executing_compute_command_buffers_mutex_{};
  mutable std::mutex executing_transform_command_buffers_mutex_{};

  std::list<CommandTaskFlowToBeRun> task_flow_queue_{};
  std::mutex task_flow_queue_mutex_{};

  // bool last_run_is_run_one_frame_call_{false};
  std::atomic_bool processing_task_flow_queue_{false};

  std::list<std::uint32_t> wait_tasks_;
  std::mutex wait_tasks_mutex_{};

  std::list<std::shared_ptr<ExecutingCommandTaskFlow>>
      executing_command_task_flows_{};
  std::unordered_map<RenderResourceDataID,
                     std::list<CommandTaskRenderResourceState>>
      command_task_render_resource_states_{};
  std::mutex command_task_render_resource_states_mutex_{};

  /*std::set<std::unique_ptr<CommandTask>*>
  task_that_have_already_been_accessed_;
  std::set<std::unique_ptr<CommandTask>*> submitted_task_;
  std::list<CommandTaskToBeSubmit> can_be_submitted_tasks_;
  std::list<CommandTaskToBeSubmit> pre_task_not_submit_task_;*/

  std::stack<VkSemaphore> semaphores_;

  std::array<std::set<VkSemaphore>, 2> submit_failed_to_be_recycled_semaphore_;
  std::uint32_t submit_failed_to_be_recycled_semaphore_current_index_;
  std::mutex submit_failed_to_be_recycled_semaphore_mutex_;
  std::list<VkSemaphore> recycled_semaphore_;
  std::mutex recycled_semaphore_mutex_;

  std::list<std::unique_ptr<AllocatedCommandBuffer>>
      submit_failed_to_be_recycled_command_buffer_{};
  std::mutex submit_failed_to_be_recycled_command_buffer_mutex_{};

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