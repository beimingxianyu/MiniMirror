#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-identifier-naming"
#pragma once

#include <condition_variable>
#include <utility>

#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/vk_type_define.h"
#include "runtime/function/render/vk_utils.h"

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
  CommandBufferInfo(std::uint32_t queue_index,
                    CommandBufferType command_buffer_type);
  CommandBufferInfo(const CommandBufferInfo& other) = default;
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
  AllocatedCommandBuffer(const AllocatedCommandBuffer& other) = delete;
  AllocatedCommandBuffer(AllocatedCommandBuffer&& other) noexcept;
  AllocatedCommandBuffer& operator=(const AllocatedCommandBuffer& other) =
      delete;
  AllocatedCommandBuffer& operator=(AllocatedCommandBuffer&& other) noexcept;

 public:
  const RenderEngine& GetRenderEngine() const;

  VkQueue GetQueue();

  const VkQueue_T* GetQueue() const;

  std::uint32_t GetQueueIndex() const;

  CommandBufferType GetCommandBufferType() const;

  VkCommandPool GetCommandPool();

  const VkCommandPool_T* GetCommandPool() const;

  VkCommandBuffer GetCommandBuffer();

  const VkCommandBuffer_T* GetCommandBuffer() const;

  VkFence GetFence();

  const VkFence_T* GetFence() const;

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
    AllocatedCommandBufferWrapper(RenderEngine* engine, const VkQueue& queue,
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
  std::unique_ptr<AllocatedCommandBufferWrapper> wrapper_{nullptr};
};

class CommandTaskFlow {
  friend class CommandTask;
  friend class CommandExecutor;

 public:
  CommandTaskFlow() = default;
  ~CommandTaskFlow() = default;
  CommandTaskFlow(const CommandTaskFlow& other) = delete;
  CommandTaskFlow(CommandTaskFlow&& other) noexcept;
  CommandTaskFlow& operator=(const CommandTaskFlow& other) = delete;
  CommandTaskFlow& operator=(CommandTaskFlow&& other) noexcept;

 public:
  /**
   * \remark The commands must not contain VkQueueSubmit().
   */
  MM::RenderSystem::CommandTask& AddTask(
      CommandType command_type,
      const std::function<MM::ExecuteResult(
          MM::RenderSystem::AllocatedCommandBuffer&)>& commands,
      std::uint32_t use_render_resource_count,
      const std::vector<MM::RenderSystem::WaitAllocatedSemaphore>&
          wait_semaphores,
      const std::vector<MM::RenderSystem::AllocateSemaphore>&
          signal_semaphores);

  /**
   * \remark The commands must not contain VkQueueSubmit().
   */
  CommandTask& AddTask(
      CommandType command_type,
      const std::vector<
          std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>& commands,
      std::uint32_t use_render_resource_count,
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
    CommandTaskEdge(const std::unique_ptr<CommandTask>* start,
                    const std::unique_ptr<CommandTask>* end);

    const std::unique_ptr<CommandTask>* start_command_task_{nullptr};
    const std::unique_ptr<CommandTask>* end_command_task_{nullptr};

    bool operator<(const CommandTaskEdge& other) const;
  };

  void RemoveRootTask(const CommandTask& command_task);

  std::vector<CommandTaskEdge> GetCommandTaskEdges() const;

  void RemoveTask(CommandTask& command_task);

 private:
  std::list<std::unique_ptr<CommandTask>*> root_tasks_{};
  std::list<std::unique_ptr<CommandTask>> tasks_{};

  std::array<std::uint32_t, 3> task_numbers_{};

  std::uint32_t increase_task_ID_{0};

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

  std::vector<const std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>*>
  GetCommandsIncludeSubTasks() const;

  std::uint32_t GetRequireCommandBufferNumber() const;

  std::uint32_t GetRequireCommandBufferNumberIncludeSuBTasks() const;

  const std::vector<WaitAllocatedSemaphore>& GetWaitSemaphore() const;

  std::vector<const WaitAllocatedSemaphore*> GetWaitSemaphoreIncludeSubTasks()
      const;

  const std::vector<AllocateSemaphore>& GetSignalSemaphore() const;

  std::vector<const AllocateSemaphore*> GetSignalSemaphoreIncludeTasks() const;

  const std::vector<std::unique_ptr<CommandTask>>& GetSubTasks() const;

  std::uint32_t GetSubTasksNumber() const;

  template <typename... CommandTasks>
  ExecuteResult Merge(CommandTasks&&... command_tasks);

  template <typename... CommandTasks>
  ExecuteResult IsPreTaskTo(CommandTasks&&... command_tasks);

  template <typename... CommandTasks>
  ExecuteResult IsPostTaskTo(CommandTasks&&... command_tasks);

  bool HaveSubTasks() const;

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const RenderResourceDataID& render_resource_ID);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const std::vector<RenderResourceDataID>& render_resource_IDs);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      std::vector<RenderResourceDataID>&& render_resource_IDs);

  std::uint32_t GetCommandTaskID();

  std::uint32_t GetUseRenderResourceCount();

  void SetUseRenderResourceCount(std::uint32_t new_use_render_resource_count);

  bool IsValid() const;

 private:
  CommandTask(
      CommandTaskFlow* task_flow, uint32_t command_task_ID,
      const CommandType& command_type,
      const std::vector<
          std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>& commands,
      std::uint32_t use_render_resource_count,
      const std::vector<WaitAllocatedSemaphore>& wait_semaphore,
      const std::vector<AllocateSemaphore>& signal_semaphore);

 private:
  CommandTaskFlow* task_flow_;
  std::uint32_t command_task_ID_{0};
  std::unique_ptr<CommandTask>* this_unique_ptr_{nullptr};
  CommandType command_type_{CommandType::UNDEFINED};
  std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>
      commands_{};
  std::vector<WaitAllocatedSemaphore> wait_semaphore_{};
  std::vector<AllocateSemaphore> signal_semaphore_{};
  mutable std::list<std::unique_ptr<CommandTask>*> pre_tasks_{};
  mutable std::list<std::unique_ptr<CommandTask>*> post_tasks_{};

  mutable std::vector<std::unique_ptr<CommandTask>> sub_tasks_{};
  bool is_sub_task_{false};

  std::uint32_t use_render_resource_count_{1};
  std::vector<RenderResourceDataID> cross_task_flow_sync_render_resource_IDs_;
};

template <typename... CommandTasks>
ExecuteResult CommandTask::Merge(CommandTasks&&... command_tasks) {
  bool input_parameters_is_right = true;
  ((input_parameters_is_right &= (&command_tasks != this)), ...);
  if (!input_parameters_is_right) {
    LOG_ERROR(
        "The predecessor of an object of type MM::RenderSystem::CommandTask "
        "cannot be itself.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }
  ((input_parameters_is_right &= (task_flow_ == command_tasks.task_flow_)),
   ...);
  if (!input_parameters_is_right) {
    LOG_ERROR("Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }
  // TODO Optimize the algorithm to remove this limitation.
  ((input_parameters_is_right &= (command_tasks.pre_tasks_.size() == 0 &&
                                  command_tasks.post_tasks_.size() == 0)),
   ...);
  if (!input_parameters_is_right) {
    LOG_ERROR("Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }
  ((input_parameters_is_right &=
    (command_tasks.command_type_ == command_type_)),
   ...);
  if (!input_parameters_is_right) {
    LOG_ERROR(
        "Two MM::RenderSystem::CommandTask's command_type_ are not equal.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }
  // TODO Optimize the algorithm to remove this limitation.
  ((input_parameters_is_right &= (!command_tasks.HaveSubTasks())), ...);
  if (!input_parameters_is_right) {
    LOG_ERROR("Cannot nest merge MM::RenderSystem::CommandTask.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }

  std::uint32_t new_cross_task_flow_sync_render_resource_IDs_vector_size =
      cross_task_flow_sync_render_resource_IDs_.size();
  ((new_cross_task_flow_sync_render_resource_IDs_vector_size +=
    command_tasks.cross_task_flow_sync_render_resource_IDs_.size()),
   ...);
  cross_task_flow_sync_render_resource_IDs_.reserve(
      new_cross_task_flow_sync_render_resource_IDs_vector_size);
  ((cross_task_flow_sync_render_resource_IDs_.insert(
       cross_task_flow_sync_render_resource_IDs_.begin(),
       command_tasks.cross_task_flow_sync_render_resource_IDs_.begin(),
       command_tasks.cross_task_flow_sync_render_resource_IDs_.end())),
   ...);

  (task_flow_->RemoveRootTask(command_tasks), ...);
  (sub_tasks_.emplace_back(std::move(*command_tasks.this_unique_ptr_)), ...);
  ((command_tasks.is_sub_task_ = true), ...);

  (task_flow_->RemoveTask(command_tasks), ...);

  return ExecuteResult::SUCCESS;
}

template <typename... CommandTasks>
ExecuteResult CommandTask::IsPreTaskTo(CommandTasks&&... command_tasks) {
  bool input_parameters_is_right = true;
  ((input_parameters_is_right &= (&command_tasks != this)), ...);
  if (!input_parameters_is_right) {
    LOG_ERROR(
        "The predecessor of an object of type MM::RenderSystem::CommandTask "
        "cannot be itself.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }
  ((input_parameters_is_right &= (task_flow_ == command_tasks.task_flow_)),
   ...);
  if (!input_parameters_is_right) {
    LOG_ERROR("Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }

  (command_tasks.pre_tasks_.push_back(this_unique_ptr_), ...);
  (task_flow_->RemoveRootTask(command_tasks), ...);
  (post_tasks_.push_back(&command_tasks.this_unique_ptr_), ...);

  return ExecuteResult::SUCCESS;
}

template <typename... CommandTasks>
ExecuteResult CommandTask::IsPostTaskTo(CommandTasks&&... command_tasks) {
  bool input_parameters_is_right = true;
  ((input_parameters_is_right &= (&command_tasks != this)), ...);
  if (!input_parameters_is_right) {
    LOG_ERROR(
        "The post task of an object of type MM::RenderSystem::CommandTask "
        "cannot be itself.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }
  ((input_parameters_is_right &= (task_flow_ == command_tasks.task_flow_)),
   ...);
  if (!input_parameters_is_right) {
    LOG_ERROR("Two MM::RenderSystem::CommandTask's task_flow_ are not equal.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }

  (command_tasks.post_tasks_.push_back(this_unique_ptr_), ...);
  (pre_tasks_.push_back(&command_tasks.this_unique_ptr_), ...);
  task_flow_->RemoveRootTask(*this);

  return ExecuteResult::SUCCESS;
}

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

  ExecuteResult AcquireGeneralGraphCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>& output) {
    std::lock_guard guard{general_command_buffers_acquire_release_mutex_};
    if (general_command_buffers_[0][0] != nullptr) {
      output = std::move(general_command_buffers_[0][0]);
      return ExecuteResult ::SUCCESS;
    }
    if (general_command_buffers_[0][1] != nullptr) {
      output = std::move(general_command_buffers_[0][1]);
      return ExecuteResult ::SUCCESS;
    }
    if (general_command_buffers_[0][2] != nullptr) {
      output = std::move(general_command_buffers_[0][2]);
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::SYNCHRONIZE_FAILED;
  }

  ExecuteResult AcquireGeneralComputeCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>& output) {
    std::lock_guard guard{general_command_buffers_acquire_release_mutex_};
    if (general_command_buffers_[1][0] != nullptr) {
      output = std::move(general_command_buffers_[1][0]);
      return ExecuteResult ::SUCCESS;
    }
    if (general_command_buffers_[1][1] != nullptr) {
      output = std::move(general_command_buffers_[1][1]);
      return ExecuteResult ::SUCCESS;
    }
    if (general_command_buffers_[1][2] != nullptr) {
      output = std::move(general_command_buffers_[1][2]);
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::SYNCHRONIZE_FAILED;
  }

  ExecuteResult AcquireGeneralTransformCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>& output) {
    std::lock_guard guard{general_command_buffers_acquire_release_mutex_};
    if (general_command_buffers_[2][0] != nullptr) {
      output = std::move(general_command_buffers_[2][0]);
      return ExecuteResult ::SUCCESS;
    }
    if (general_command_buffers_[2][1] != nullptr) {
      output = std::move(general_command_buffers_[2][1]);
      return ExecuteResult ::SUCCESS;
    }
    if (general_command_buffers_[2][2] != nullptr) {
      output = std::move(general_command_buffers_[2][2]);
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::SYNCHRONIZE_FAILED;
  }

  void ReleaseGeneralCommandBuffer(
      std::unique_ptr<AllocatedCommandBuffer>& output) {
    if (output->IsValid()) {
      return;
    }
    std::uint64_t general_command_type_index = UINT32_MAX;
    switch (output->GetCommandBufferType()) {
      case CommandBufferType::GRAPH:
        general_command_type_index = 0;
        break;
      case CommandBufferType::COMPUTE:
        general_command_type_index = 1;
        break;
      case CommandBufferType::TRANSFORM:
        general_command_type_index = 2;
        break;
      case CommandBufferType::UNDEFINED:
        return;
    }

    std::lock_guard guard{general_command_buffers_acquire_release_mutex_};
    if (general_command_buffers_[general_command_type_index][0] == nullptr) {
      output->ResetCommandBuffer();
      general_command_buffers_[general_command_type_index][0] =
          std::move(output);
      return;
    }
    if (general_command_buffers_[general_command_type_index][1] == nullptr) {
      output->ResetCommandBuffer();
      general_command_buffers_[general_command_type_index][1] =
          std::move(output);
      return;
    }
    if (general_command_buffers_[general_command_type_index][2] != nullptr) {
      output->ResetCommandBuffer();
      general_command_buffers_[general_command_type_index][2] =
          std::move(output);
      return;
    }
  }

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

  ExecuteResult InitCommandPolls(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools);

  ExecuteResult InitCommandBuffers(
      std::vector<VkCommandPool>& graph_command_pools,
      std::vector<VkCommandPool>& compute_command_pools,
      std::vector<VkCommandPool>& transform_command_pools,
      std::vector<VkCommandBuffer>& graph_command_buffers,
      std::vector<VkCommandBuffer>& compute_command_buffers,
      std::vector<VkCommandBuffer>& transform_command_buffers);

  ExecuteResult InitGeneralAllocatedCommandBuffers(
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
        const std::shared_ptr<CommandCompleteState>& complete_state);
    CommandTaskFlowToBeRun(const CommandTaskFlowToBeRun& other) = delete;
    CommandTaskFlowToBeRun(CommandTaskFlowToBeRun&& other) noexcept;
    CommandTaskFlowToBeRun& operator=(const CommandTaskFlowToBeRun& other) =
        delete;
    CommandTaskFlowToBeRun& operator=(CommandTaskFlowToBeRun&& other) noexcept;

    CommandTaskFlow command_task_flow_{};
    std::uint32_t task_flow_ID_{0};
    std::shared_ptr<ExecuteResult> execute_result_{};
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
    std::shared_ptr<ExecuteResult> execute_result_{};
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
        const std::weak_ptr<ExecuteResult>& execute_result,
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
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      std::uint32_t& number_of_blocked_cross_task_flow);

  void ProcessWhenOneFailedSubmit(
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow);

  void ProcessNextStepCanSubmitTask(
      const std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow);

  MM::ExecuteResult RecordAndSubmitCommandSync(
      std::unique_ptr<ExecutingTask>& input_tasks);

  MM::ExecuteResult RecordAndSubmitCommandASync(
      std::unique_ptr<ExecutingTask>& input_tasks);

  void PostProcessOfSubmitTask(
      std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      std::unique_ptr<ExecutingTask>& input_tasks, ExecuteResult& result);

  ExecuteResult SubmitTasksSync(
      std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      std::unique_ptr<ExecutingTask>& input_tasks);

  ExecuteResult SubmitTaskAsync(
      std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
      std::unique_ptr<ExecutingTask>& input_tasks);

  ExecuteResult SubmitTasks(
      std::shared_ptr<ExecutingCommandTaskFlow>& command_task_flow,
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
  ;
  explicit CommandExecutorLockGuard(CommandExecutor& command_executor);
  CommandExecutorLockGuard(const CommandExecutorLockGuard& other) = delete;
  CommandExecutorLockGuard(CommandExecutorLockGuard&& other) noexcept;
  CommandExecutorLockGuard& operator=(const CommandExecutorLockGuard& other) =
      delete;
  CommandExecutorLockGuard& operator=(
      CommandExecutorLockGuard&& other) noexcept;

 public:
  void Lock();

  void Unlock();

  bool IsValid() const;

 private:
  CommandExecutor* command_executor_{nullptr};
};
}  // namespace RenderSystem
}  // namespace MM
#pragma clang diagnostic pop