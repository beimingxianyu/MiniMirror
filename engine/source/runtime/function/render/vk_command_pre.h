//
// Created by beimingxianyu on 23-10-5.
//
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

  bool ResetFence() { return wrapper_->ResetFence(); }

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

    bool ResetFence();

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

using CommandTaskID = std::uint32_t;
using CommandTaskFlowID = std::uint32_t;
using TaskType =
    std::function<Result<Nil, ErrorResult>(AllocatedCommandBuffer& cmd)>;
}  // namespace RenderSystem
}  // namespace MM