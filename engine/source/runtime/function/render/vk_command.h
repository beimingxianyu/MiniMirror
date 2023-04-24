#pragma once

#include "runtime/function/render/vk_utils.h"
#include "runtime/function/render/vk_type.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;
class CommandExecutor;
class CommandTask;
class CommandTaskFlow;

using CommandType = CommandBufferType;

struct WaitSemaphore {
  VkSemaphore wait_semaphore_;
  VkPipelineStageFlags wait_stage_{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

  bool IsValid() const;
};

struct WaitAllocatedSemaphore {
  AllocateSemaphore wait_semaphore_;
  VkPipelineStageFlags wait_stage_{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

  struct WaitSemaphoreLessWithoutWaitStage {
    bool operator()(const WaitAllocatedSemaphore& l_value,
                    const WaitAllocatedSemaphore& r_value) const;
  };

  bool IsValid() const;
};

struct CommandBufferInfo {
  CommandBufferInfo() = default;
  ~CommandBufferInfo() = default;
  CommandBufferInfo(std::uint32_t queue_index, CommandBufferType command_buffer_type);
  CommandBufferInfo(const CommandBufferInfo& other);
  CommandBufferInfo(CommandBufferInfo&& other) noexcept;
  CommandBufferInfo& operator=(const CommandBufferInfo& other);
  CommandBufferInfo& operator=(CommandBufferInfo&& other) noexcept;

  std::uint32_t queue_index_{0};
  CommandBufferType command_buffer_type{};

  void Reset();

  bool IsValid() const;
};

class AllocatedCommandBuffer {
 public:
  AllocatedCommandBuffer() = default;
  ~AllocatedCommandBuffer() = default;
  AllocatedCommandBuffer(RenderEngine* engine, const std::uint32_t& queue_index,
                         const VkQueue& queue,
                         const VkCommandPool& command_pool,
                         const VkCommandBuffer& command_buffer);
  AllocatedCommandBuffer(const AllocatedCommandBuffer& other) = default;
  AllocatedCommandBuffer(AllocatedCommandBuffer&& other) noexcept;
  AllocatedCommandBuffer& operator=(const AllocatedCommandBuffer& other);
  AllocatedCommandBuffer& operator=(AllocatedCommandBuffer&& other) noexcept;

 public:
  const RenderEngine& GetRenderEngine() const;

  const VkQueue& GetQueue() const;

  std::uint32_t GetQueueIndex() const;

  CommandBufferType GetCommandBufferType() const;

  const VkCommandPool& GetCommandPool() const;

  const VkCommandBuffer& GetCommandBuffer() const;

  const VkFence& GetFence() const;

  /**
   * \remark A command pool corresponds to a command buffer, so resetting t
   * he command buffer is equivalent to resetting the command pool.
   */
  bool ResetCommandBuffer();

  bool IsValid() const;

 private:
  class AllocatedCommandBufferWrapper {
   public:
    AllocatedCommandBufferWrapper() = default;
    ~AllocatedCommandBufferWrapper();
    AllocatedCommandBufferWrapper(RenderEngine* engine,
                                  const VkQueue& queue,
                                  const VkCommandPool& command_pool,
                                  const VkCommandBuffer& command_buffer);
    AllocatedCommandBufferWrapper(const AllocatedCommandBufferWrapper& other) =
        delete;
    AllocatedCommandBufferWrapper(AllocatedCommandBufferWrapper&& other) =
        delete;
    AllocatedCommandBufferWrapper& operator=(
        const AllocatedCommandBufferWrapper& other) = delete;
    AllocatedCommandBufferWrapper& operator=(
        AllocatedCommandBufferWrapper&& other) = delete;

   public:
    const RenderEngine& GetRenderEngine() const;

    const VkQueue& GetQueue() const;

    const VkCommandPool& GetCommandPool() const;

    const VkCommandBuffer& GetCommandBuffer() const;

    const VkFence& GetFence() const;

    bool IsValid() const;

    bool ResetCommandBuffer();

   private:
    RenderEngine* engine_{nullptr};
    VkQueue queue_{nullptr};
    VkCommandPool command_pool_{nullptr};
    VkCommandBuffer command_buffer_{nullptr};
    VkFence command_fence_{nullptr};
  };

 private:
  CommandBufferInfo command_buffer_info_{};
  std::shared_ptr<AllocatedCommandBufferWrapper> wrapper_{nullptr};
};

class CommandTaskFlow {
  friend class CommandTask;
  friend class CommandExecutor;

 public:
  CommandTaskFlow() = default;
  ~CommandTaskFlow();
  CommandTaskFlow(const CommandTaskFlow& other) = delete;
  CommandTaskFlow(CommandTaskFlow&& other) noexcept;
  CommandTaskFlow& operator=(const CommandTaskFlow& other) = delete;
  CommandTaskFlow& operator=(CommandTaskFlow&& other) noexcept;

 public:
  /**
   * \remark The \ref commands must not contain VkQueueSubmit().
   */
  CommandTask& AddTask(
      const CommandType& command_type,
      const std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>& commands,
      const std::vector<WaitAllocatedSemaphore>& wait_semaphores,
      const std::vector<AllocateSemaphore>& signal_semaphores);

  /**
   * \remark The \ref commands must not contain VkQueueSubmit().
   */
  CommandTask& AddTask(
      const CommandType& command_type,
      const std::vector<
        std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>& commands,
      const std::vector<WaitAllocatedSemaphore>& wait_semaphores,
      const std::vector<AllocateSemaphore>& signal_semaphores);

  std::uint32_t GetTaskNumber() const;

  std::uint32_t GetGraphNumber() const;

  std::uint32_t GetComputeNumber() const;

  std::uint32_t GetTransformNumber() const;

  bool IsRootTask(const CommandTask& command_task) const;

  void Clear();

  bool HaveRing() const;

  bool IsValid() const;

private:
  struct CommandTaskEdge {
    CommandTaskEdge(std::unique_ptr<CommandTask>* start,
                    std::unique_ptr<CommandTask>* end);

    std::unique_ptr<CommandTask>* start_command_task_{nullptr};
    std::unique_ptr<CommandTask>* end_command_task_{nullptr};

    bool operator<(const CommandTaskEdge& other) const;
  };

  void RemoveRootTask(const CommandTask& command_task);

  [[deprecated]]
  void GetCommandTaskEdges(std::vector<CommandTaskEdge>& command_task_edges) const;

  std::vector<CommandTaskEdge> GetCommandTaskEdges() const;

 private:
  std::vector<std::unique_ptr<CommandTask>*> root_tasks_{};
  std::vector<std::unique_ptr<CommandTask>> tasks_{};

  std::shared_mutex task_sync_{};
};

class CommandTask {
  friend class CommandExecutor;
  friend class CommandTaskFlow;

public:
  CommandTask() = delete;
  ~CommandTask();
  CommandTask(const CommandTask& other) = delete;
  CommandTask(CommandTask&& other) = delete;
  CommandTask& operator=(const CommandTask& other) = delete;
  CommandTask& operator=(CommandTask&& other) = delete; 

public:
 const CommandBufferType& GetCommandType() const;

  const CommandTaskFlow& GetCommandTaskFlow() const;

  const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>&
  GetCommands() const;

  std::uint32_t GetRequireCommandBufferNumber() const;

  const std::vector<WaitAllocatedSemaphore>& GetWaitSemaphore() const;

  const std::vector<AllocateSemaphore>& GetSignalSemaphore() const;

  template <typename... CommandTasks>
  void IsPreTaskTo(CommandTasks&& ...command_tasks);

  template <typename... CommandTasks>
  void IsPostTaskTo(CommandTasks&& ...command_tasks);

  bool IsValid() const;

private:
  CommandTask(
      CommandTaskFlow* task_flow,
      const CommandType& command_type,
      const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>&
              commands,
      const std::vector<WaitAllocatedSemaphore>& wait_semaphore,
      const std::vector<AllocateSemaphore>& signal_semaphore);

 private:
  CommandTaskFlow* task_flow_;
  std::unique_ptr<CommandTask>* this_unique_ptr_{nullptr};
  CommandType command_type_{CommandType::UNDEFINED};
  std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>> commands_{};
  std::vector<WaitAllocatedSemaphore> wait_semaphore_{};
  std::vector<AllocateSemaphore> signal_semaphore_{};
  mutable std::list<std::unique_ptr<CommandTask>*> pre_tasks_{};
  mutable std::list<std::unique_ptr<CommandTask>*> post_tasks_{};
};

template <typename ... CommandTasks>
void CommandTask::IsPreTaskTo(CommandTasks&&... command_tasks) {
  (assert(&command_tasks != this), ...);
  (command_tasks.pre_tasks_.push_back(this_unique_ptr_), ...);
  (task_flow_->RemoveRootTask(command_tasks),...);
  (post_tasks_.push_back(&command_tasks.this_unique_ptr_), ...);
}

template <typename ... CommandTasks>
void CommandTask::IsPostTaskTo(CommandTasks&&... command_tasks) {
  (assert(!(&command_tasks == this)), ...);
  (command_tasks.post_tasks_.push_back(this_unique_ptr_), ...);
  (pre_tasks_.push_back(&command_tasks.this_unique_ptr_), ...);
  task_flow_->RemoveRootTask(*this);
}

class RenderFuture {
public:
  RenderFuture() = default;
  RenderFuture( CommandExecutor* command_executor,
               const std::uint32_t& task_flow_ID,
               const std::shared_ptr<ExecuteResult>& future_execute_result,
               const std::vector<std::shared_ptr<bool>>& command_complete_states);
  ~RenderFuture() = default;
  RenderFuture(const RenderFuture& other) = default;
  RenderFuture(RenderFuture&& other) noexcept = default;
  RenderFuture& operator=(const RenderFuture& other);
  RenderFuture& operator=(RenderFuture&& other) noexcept;

public:
  ExecuteResult Get();

  bool IsValid();

private:
  CommandExecutor* command_executor_{nullptr};
  std::uint32_t task_flow_ID_{0};
  std::shared_ptr<ExecuteResult> execute_result_{};
  std::vector<std::shared_ptr<bool>> command_complete_states_{};
};

class CommandExecutor {
  friend class RenderFuture;

 public:
  CommandExecutor() = delete;
  CommandExecutor(RenderEngine* engine);
  ~CommandExecutor() = default;
  CommandExecutor(RenderEngine* engine,
                 const uint32_t& graph_command_number,
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

   /**
   * \remark \ref command_task_flow is invalid after call this function.
   */
  RenderFuture Run(CommandTaskFlow&& command_task_flow);

  /**
   * \remark The \ref command_task_flow is invalid after call this function.
   */
  ExecuteResult RunAndWait(CommandTaskFlow&& command_task_flow);

  /**
   * \remark \ref command_task_flow is invalid after call this function.
   */
  RenderFuture Run(CommandTaskFlow& command_task_flow);

  /**
   * \remark The \ref command_task_flow is invalid after call this function.
   */
  ExecuteResult RunAndWait(CommandTaskFlow& command_task_flow);

  // TODO RunOneFrame
  ///**
  // * \brief The \ref command_task_flow contain all render commands in one frame,
  // * and will be use in next frame if the render state not change.
  // * \remark This function must be the last call to the rendering call,
  // * otherwise this call is invalid.
  // * \remark Default wait this call.
  // */
  //RenderFuture RunOneFrame(CommandTaskFlow& command_task_flow);

  bool IsValid() const;

private:
  void ClearWhenConstructFailed(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  ExecuteResult InitCommandPolls(
      std::vector<VkCommandPool>& graph_command_pools, std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  ExecuteResult InitCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  ExecuteResult InitAllocateCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  ExecuteResult InitSemaphores(
      const std::uint32_t& need_semaphore_number,
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  ExecuteResult AddCommandBuffer(const CommandType& command_type,
                                 const std::uint32_t& new_command_buffer_num);

  struct CommandTaskFlowToBeRun {
    CommandTaskFlowToBeRun() = delete;
    ~CommandTaskFlowToBeRun() = default;
    CommandTaskFlowToBeRun(
        CommandTaskFlow&& command_task_flow, const std::uint32_t& task_flow_ID,
        const std::shared_ptr<ExecuteResult>& execute_result,
        const std::vector<std::shared_ptr<bool>>& is_completes);
    CommandTaskFlowToBeRun(const CommandTaskFlowToBeRun& other) = delete;
    CommandTaskFlowToBeRun(CommandTaskFlowToBeRun&& other) noexcept;
    CommandTaskFlowToBeRun& operator=(const CommandTaskFlowToBeRun& other) = delete;
    CommandTaskFlowToBeRun& operator=(CommandTaskFlowToBeRun&& other) noexcept;

    CommandTaskFlow command_task_flow_{};
    std::uint32_t task_flow_ID_{0};
    std::shared_ptr<ExecuteResult> execute_result_{};
    std::stack<std::weak_ptr<bool>> is_completes_{};
  };

  static void UpdateCommandTaskLine(
      CommandExecutor* command_executor,
      std::unique_ptr<CommandTask>& new_command_task, std::unique_ptr<CommandTask>
      * old_command_task_ptr);

  struct CommandTaskToBeSubmit {
    CommandTaskToBeSubmit() = delete;
    ~CommandTaskToBeSubmit() = default;
    CommandTaskToBeSubmit(
        CommandExecutor* command_executor,
        std::unique_ptr<CommandTask>&& command_task,
        std::vector<VkSemaphore>&& default_wait_semaphore,
        std::vector<VkSemaphore>&& default_signal_semaphore);
    CommandTaskToBeSubmit(const CommandTaskToBeSubmit& other) = delete;
    CommandTaskToBeSubmit(CommandTaskToBeSubmit&& other) noexcept;
    CommandTaskToBeSubmit& operator=(const CommandTaskToBeSubmit& other) =
        delete;
    CommandTaskToBeSubmit& operator=(CommandTaskToBeSubmit&& other) noexcept;

    CommandExecutor* command_executor_;
    std::unique_ptr<CommandTask> command_task_;
    std::vector<VkSemaphore> default_wait_semaphore_{};
    std::vector<VkSemaphore> default_signal_semaphore_{};

    bool operator<(const CommandTaskToBeSubmit& other) const;
  };

  struct ExecutingTask {
    ExecutingTask() = delete;
    ~ExecutingTask() = default;
    ExecutingTask(RenderEngine* engine,
                  CommandExecutor* command_executor,
                  std::vector<std::unique_ptr<AllocatedCommandBuffer>>&&
                      command_buffer, std::unique_ptr<CommandTask>&& command_task,
                  const std::weak_ptr<ExecuteResult>& execute_result,
                  const std::optional<std::weak_ptr<bool>>& is_complete,
                  std::vector<VkSemaphore>&& default_wait_semaphore,
                  std::vector<VkSemaphore>&& default_signal_semaphore,
                  const std::vector<WaitAllocatedSemaphore>& external_wait_semaphores,
                  const std::vector<AllocateSemaphore>& external_signal_semaphores);
    ExecutingTask(const ExecutingTask& other) = delete;
    ExecutingTask(ExecutingTask&& other) noexcept;
    ExecutingTask& operator=(ExecutingTask&& other) noexcept;
    ExecutingTask& operator=(const ExecutingTask& other) = delete;

    RenderEngine* render_engine_;
    CommandExecutor* command_executor_;
    std::vector<std::unique_ptr<AllocatedCommandBuffer>> command_buffers_;
    std::unique_ptr<CommandTask> command_task_{};
    std::weak_ptr<ExecuteResult> execute_result_{};
    std::optional<std::weak_ptr<bool>> is_complete_{};

    std::vector<VkSemaphore> wait_semaphore_;
    std::vector<VkSemaphore> signal_semaphore_;
    std::uint32_t default_wait_semaphore_number_{0};
    std::uint32_t default_signal_semaphore_number_{0};
    // Prevent external semaphores from being destroyed during render runtime
    std::vector<WaitAllocatedSemaphore> external_wait_semaphores_;
    std::vector<AllocateSemaphore> external_signal_semaphores_;

    bool IsComplete() const;
  };

  std::vector<VkSemaphore> GetSemaphore(std::uint32_t require_number);

  void RecycledSemaphoreThatSubmittedFailed();

  void ProcessCompleteTask();

  void ProcessWaitTask();

  void ProcessRequireCommandBufferNumberLagerThanExecutorHaveCommandBufferNumber(
      const CommandTaskFlowToBeRun& command_task_flow, bool& skip_task_flow);

  void ProcessRootTaskAndSubTask(CommandTaskFlowToBeRun& command_task_flow);

  void ChooseVariableByCommandType(
      CommandType command_type, std::uint32_t*& free_command_buffer_number,
      std::stack<std::unique_ptr<AllocatedCommandBuffer>>*&
      free_command_buffers,
      std::list<ExecutingTask>*& executing_task_list,
      std::atomic_uint32_t*& recording_command_buffer_number,
      std::uint32_t free_graph_command_buffer_number,
      std::uint32_t free_compute_command_buffer_number,
      std::uint32_t free_transform_command_buffer_number);

  void ProcessOneCanSubmitTask(
      std::list<CommandTaskToBeSubmit>::iterator& command_task_to_be_submit,
      CommandTaskFlowToBeRun& command_task_flow,
      std::uint32_t free_graph_command_buffer_number,
      std::uint32_t free_compute_command_buffer_number,
      std::uint32_t free_transform_command_buffer_number);

  void ProcessNextStepCanSubmitTask(
      std::list<CommandTaskToBeSubmit>& next_can_be_submitted_tasks,
      CommandTaskFlowToBeRun& command_task_flow);

  ExecuteResult SubmitTasks(ExecutingTask* input_tasks);

  void ProcessTask();

 private:
  RenderEngine* render_engine_{nullptr};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>> free_graph_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>> free_compute_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>> free_transform_command_buffers_{};

  std::atomic_uint32_t recoding_graph_command_buffer_number_{0};
  std::atomic_uint32_t recording_compute_command_buffer_number_{0};
  std::atomic_uint32_t recording_transform_command_buffer_number_{0};

  std::list<ExecutingTask> executing_graph_command_buffers_{};
  std::list<ExecutingTask> executing_compute_command_buffers_{};
  std::list<ExecutingTask> executing_transform_command_buffers_{};
  std::mutex executing_graph_command_buffers_mutex_{};
  std::mutex executing_compute_command_buffers_mutex_{};
  std::mutex executing_transform_command_buffers_mutex_{};

  std::list<CommandTaskFlowToBeRun> task_flow_queue_{};
  std::mutex task_flow_queue_mutex_{};

  // bool last_run_is_run_one_frame_call_{false};
  std::atomic_bool processing_task_flow_queue_{false};

  std::list<std::uint32_t> wait_tasks_;
  std::mutex wait_tasks_mutex_{};

  std::set<std::unique_ptr<CommandTask>*> task_that_have_already_been_accessed_;
  std::set<std::unique_ptr<CommandTask>*> submitted_task_;
  std::list<CommandTaskToBeSubmit> can_be_submitted_tasks_;
  std::list<CommandTaskToBeSubmit> pre_task_not_submit_task_;

  std::stack<VkSemaphore> semaphores_;


  std::array<std::set<VkSemaphore>, 2> submit_failed_to_be_recycled_semaphore_;
  std::uint32_t submit_failed_to_be_recycled_semaphore_current_index_;
  std::mutex submit_failed_to_be_recycled_semaphore_mutex_;
  std::list<VkSemaphore> recycled_semaphore_;
  std::mutex recycled_semaphore_mutex_;

  std::list<std::unique_ptr<AllocatedCommandBuffer>>
      submit_failed_to_be_recycled_command_buffer_{};
  std::mutex submit_failed_to_be_recycled_command_buffer_mutex_{};
};
}  // namespace RenderSystem
}  // namespace MM