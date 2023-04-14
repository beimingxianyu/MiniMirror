#pragma once

#include "runtime/function/render/vk_utils.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;
class CommandExecutor;

class CommandTask {
  friend class CommandExecutor;

public:
  CommandTask() = delete;
 ~CommandTask() = default;
  

public:
 const CommandBufferType& GetCommandBufferType() const;

  const std::uint32_t& GetCommandBufferIndex() const;

  bool IsSubmitted() const;

  bool Wait(const std::uint64_t& timeout = 1000000000) const;

  void Reset();

  bool IsValid() const;

private:
  CommandTask(RenderEngine* engine,
              const CommandBufferType& command_type,
              const std::uint32_t& command_buffer_index,
              const VkFence& task_fence);

 private:
  RenderEngine* render_engine_{nullptr};
  CommandBufferType command_type_{CommandBufferType::UNDEFINED};
  std::uint32_t command_buffer_index_{UINT32_MAX};
  // TODO add semaphore vector
  std::atomic_bool submitted_{false};
  VkFence task_fence_;
};

struct SubmitWaitSemaphore {
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

    // bool RecordAndSubmitCommand(
    //     AllocatedCommandBuffer
    //     & allocate_buffer,
    //     const std::function<void(AllocatedCommandBuffer& cmd)>& function,
    //     const bool& auto_start_end_submit = false,
    //     const bool& record_new_command = true, std::shared_ptr<VkSubmitInfo>
    //     submit_info_ptr = nullptr);

    // bool RecordAndSubmitCommand(
    //     AllocatedCommandBuffer
    //     & allocated_buffer,
    //     const std::function<bool(AllocatedCommandBuffer& cmd)>& function,
    //     const bool& auto_start_end_submit = false,
    //     const bool& record_new_command = true, std::shared_ptr<VkSubmitInfo>
    //     submit_info_ptr = nullptr);

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

  bool RecrodCommand(
      const CommandBufferType& command_type,
      std::function<void(AllocatedCommandBuffer& cmd)> function,
      const std::vector<SubmitWaitSemaphore>& wait_semaphores,
      const std::vector<VkSemaphore>& signal_semaphores,
      const CommandTask* output_task);

  bool SubmitTask(const CommandTask& command_task);

  bool SubmitAllTask(const CommandBufferType& command_type);

  bool SubmitAllTask();

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