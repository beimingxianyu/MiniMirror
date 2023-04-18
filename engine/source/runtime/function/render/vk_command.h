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
  VkSemaphore wait_semaphore_{nullptr};
  VkPipelineStageFlags wait_stage_{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

  struct WaitSemaphoreLessWithoutWaitStage {
    bool operator()(const WaitSemaphore& l_value,
                    const WaitSemaphore& r_value) const;
  };

  bool IsValid() const;
};

struct CommandBufferInfo {
  std::uint32_t queue_index_{0};
  std::atomic_bool is_recorded_{false};

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

  const VkCommandPool& GetCommandPool() const;

  const VkCommandBuffer& GetCommandBuffer() const;

  const VkFence& GetFence() const;

  bool IsRecorded() const;

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
      const std::vector<WaitSemaphore>& wait_semaphores,
      const std::vector<VkSemaphore>& signal_semaphores);

  /**
   * \remark The \ref commands must not contain VkQueueSubmit().
   */
  CommandTask& AddTask(
      const CommandType& command_type,
      const std::vector<
          std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>& commands,
      const std::vector<WaitSemaphore>& wait_semaphores,
      const std::vector<VkSemaphore>& signal_semaphores);

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
    CommandTaskEdge(CommandTask* start, CommandTask* end);

    CommandTask* start_command_task_{nullptr};
    CommandTask* end_command_task_{nullptr};

    bool operator<(const CommandTaskEdge& other) const;
  };

  void RemoveRootTask(const CommandTask& command_task);

  [[deprecated]]
  void GetCommandTaskEdges(std::vector<CommandTaskEdge>& command_task_edges) const;

  std::vector<CommandTaskEdge> GetCommandTaskEdges() const;

 private:
  std::vector<CommandTask*> root_tasks_{};
  std::vector<CommandTask*> tasks_{};
  std::array<std::uint32_t, 3> task_count_{};
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
 const CommandBufferType& GetCommandBufferType() const;

  const CommandTaskFlow& GetCommandTaskFlow() const;

  const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>&
  GetCommands() const;

  std::uint32_t GetUsedCommandBufferNumber() const;

  const std::vector<WaitSemaphore>& GetWaitSemaphore() const;

  const std::vector<VkSemaphore>& GetSignalSemaphore() const;

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
      const std::vector<WaitSemaphore>& wait_semaphore,
      const std::vector<VkSemaphore>& signal_semaphore);

 private:
  CommandTaskFlow* task_flow_;
  CommandType command_type_{CommandType::UNDEFINED};
  std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>> commands_{};
  std::vector<WaitSemaphore> wait_semaphore_{};
  std::vector<VkSemaphore> signal_semaphore_{};
  mutable std::list<CommandTask*> pre_tasks_{};
  mutable std::list<CommandTask*> post_tasks_{};
};

template <typename ... CommandTasks>
void CommandTask::IsPreTaskTo(CommandTasks&&... command_tasks) {
  (assert(&command_tasks != this), ...);
  (command_tasks.pre_tasks_.push_back(this), ...);
  (task_flow_->RemoveRootTask(command_tasks),...);
  (post_tasks_.push_back(&command_tasks), ...);
}

template <typename ... CommandTasks>
void CommandTask::IsPostTaskTo(CommandTasks&&... command_tasks) {
  (assert(!(&command_tasks == this)), ...);
  (command_tasks.post_tasks_.push_nack(this), ...);
  (pre_tasks_.push_back(&command_tasks), ...);
  task_flow_->RemoveRootTask(*this);
}

class RenderFuture {
public:
  RenderFuture() = delete;
  RenderFuture(const std::uint32_t& task_flow_ID,
               const std::shared_ptr<ExecuteResult>& future_execute_result,
               const std::vector<std::shared_ptr<bool>>& command_complete_states);
  ~RenderFuture() = default;
  RenderFuture(const RenderFuture& other) = default;
  RenderFuture(RenderFuture&& other) noexcept = default;
  RenderFuture& operator=(const RenderFuture& other) = default;
  RenderFuture& operator=(RenderFuture&& other) noexcept = default;

public:
  ExecuteResult Get();

private:
  std::uint32_t task_flow_ID_{0};
  std::shared_ptr<ExecuteResult> execute_result_{};
  std::vector<std::shared_ptr<bool>> command_complete_states_{};
};

class CommandExecutor {
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

  std::uint32_t GetUsableGraphCommandNumber() const;

  std::uint32_t GetUsableComputeCommandNumber() const;

  std::uint32_t GetUsableTransformCommandNumber() const;

  ExecuteResult ResetCommandPool(const CommandBufferType& command_type);

  ExecuteResult ResetCommandPool();

  /**
   * \remark \ref command_task_flow is invalid after call this function.
   */
  RenderFuture Run(CommandTaskFlow& command_task_flow);

  /**
   * \remark The \ref command_task_flow is invalid after call this function.
   */
  void RunAndWait(CommandTaskFlow& command_task_flow);

  /**
   * \brief The \ref command_task_flow contain all render commands in one frame,
   * and will be use in next frame if the render state not change.
   * \remark This function must be the last call to the rendering call,
   * otherwise this call is invalid.
   * \remark Default wait this call.
   */
  RenderFuture RunOneFrame(CommandTaskFlow& command_task_flow);

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

  struct CommandTaskFlowToBeRun {
    CommandTaskFlowToBeRun() = delete;
    CommandTaskFlowToBeRun(
        CommandTaskFlow&& command_task_flow, const std::uint32_t& task_flow_ID,
        const std::shared_ptr<ExecuteResult>& execute_result,
        const std::vector<std::shared_ptr<bool>>& is_completes);

    CommandTaskFlow command_task_flow_{};
    std::uint32_t task_flow_ID_{0};
    std::weak_ptr<ExecuteResult> execute_result_{};
    std::stack<std::weak_ptr<bool>> is_completes_{};
  };

  struct ExecutingTask {
    ExecutingTask() = delete;

    RenderEngine* render_engine_;
    std::vector<std::unique_ptr<AllocatedCommandBuffer>> command_buffers_;
    CommandType command_type_;
    CommandTaskFlow command_task_flow_{};
    std::weak_ptr<ExecuteResult> execute_result_{};
    std::optional<std::weak_ptr<bool>> is_complete_{};

    VkSemaphore wait_semaphore_;
    VkSemaphore signal_semaphore_;

    bool IsComplete() const;
  };

  void ProcessCompleteTask();

  void ProcessTask();

 private:
  RenderEngine* render_engine_{nullptr};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>> free_graph_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>> free_compute_command_buffers_{};
  std::stack<std::unique_ptr<AllocatedCommandBuffer>> free_transform_command_buffers_{};

  std::list<ExecutingTask> executing_graph_command_buffers_{};
  std::list<ExecutingTask> executing_compute_command_buffers_{};
  std::list<ExecutingTask> executing_transform_command_buffers_{};

  std::list<CommandTaskFlowToBeRun> task_flow_queue_{};
  std::mutex task_flow_queue_mutex_{};

  bool last_run_is_run_one_frame_call_{false};
  std::atomic_bool processing_task_flow_queue_{false};

  std::list<std::uint32_t> wait_tasks_;
  std::shared_mutex wait_tasks_mutex_{};

  std::stack<VkSemaphore> semaphores_;
};
}  // namespace RenderSystem
}  // namespace MM