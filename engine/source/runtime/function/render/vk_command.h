#pragma once

#include "runtime/function/render/vk_utils.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;
class CommandExecutor;
class CommandTaskFlow;

using CommandType = CommandBufferType;

struct WaitSemaphore {
  VkSemaphore wait_semaphore_{nullptr};
  VkPipelineStageFlags wait_stage_{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

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
                         const std::shared_ptr<VkQueue>& queue,
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
                                  const std::shared_ptr<VkQueue>& queue,
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
    std::shared_ptr<VkQueue> queue_{nullptr};
    VkCommandPool command_pool_{nullptr};
    VkCommandBuffer command_buffer_{nullptr};
    VkFence command_fence_{nullptr};
  };

 private:
  CommandBufferInfo command_buffer_info_{};
  std::shared_ptr<AllocatedCommandBufferWrapper> wrapper_{nullptr};
};

class CommandTask {
  friend class CommandExecutor;
  friend class CommandTaskFlow;

public:
 ~CommandTask() = default;

public:
 const CommandBufferType& GetCommandBufferType() const;

  const std::uint32_t& GetCommandBufferIndex() const;

  bool IsSubmitted() const;

  template<typename  ...CommandTasks, typename IsCommandTask = typename std::enable_if<std::is_same<const CommandTask*, CommandTasks>..., void>::type>
  void Precede(CommandTasks ...command_tasks);

  template <typename... CommandTasks,
            typename IsCommandTask = typename std::enable_if<
                std::is_same<const CommandTask*, CommandTasks>..., void>::type>
  void Succeed(CommandTasks ...command_tasks);

  void Reset();

  bool IsValid() const;

private:
  CommandTask() = default;

  CommandTask(
      const CommandType& command_type,
      std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>&
              commands,
      std::vector<WaitSemaphore> wait_semaphore,
      std::vector<VkSemaphore> signal_semaphore);

 private:
  CommandType command_type_{CommandType::UNDEFINED};
  std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>> recorded_command_{};
  std::vector<WaitSemaphore> wait_semaphore_{};
  std::vector<VkSemaphore> signal_semaphore_{};
  std::atomic_bool submitted_{false};
  mutable std::list<CommandTask*> pre_tasks_{};
  mutable std::list<CommandTask*> post_tasks_{};
};

template <typename ... CommandTasks, typename IsCommandTask>
void CommandTask::Precede(CommandTasks... command_tasks) {
  command_tasks->
}

class CommandTaskFlow {
public:
  CommandTaskFlow() = default;
 ~CommandTaskFlow();
  CommandTaskFlow(const CommandTaskFlow& other) = delete;
  CommandTaskFlow(CommandTaskFlow&& other) noexcept;
  CommandTaskFlow& operator=(const CommandTaskFlow& other) = delete;
  CommandTaskFlow& operator=(CommandTaskFlow&& other) noexcept;

public:
  const CommandTask* AddTask(
      const CommandType& command_type,
      std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>& commands,
      const std::vector<WaitSemaphore>& wait_semaphores,
      const std::vector<VkSemaphore>& signal_semaphores);

  const CommandTask* AddTask(
      const CommandType& command_type,
      const std::vector<std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>>&
          commands,
      const std::vector<WaitSemaphore>& wait_semaphores,
      const std::vector<VkSemaphore>& signal_semaphores);

private:
  CommandTask root_task{};
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

  bool ResetCommandPool(const CommandBufferType& command_type);

  bool ResetCommandPool();

  bool IsValid() const;

 private:
  std::list<CommandTask> not_submitted_commands{};

  RenderEngine* render_engine{nullptr};
  std::vector<AllocatedCommandBuffer> graph_command_buffers_{};
  std::vector<AllocatedCommandBuffer> compute_command_buffers_{};
  std::vector<AllocatedCommandBuffer> transform_command_buffers_{};
};
}  // namespace RenderSystem
}  // namespace MM