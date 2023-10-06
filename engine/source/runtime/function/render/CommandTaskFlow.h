//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include "runtime/function/render/vk_command_pre.h"

namespace MM {
namespace RenderSystem {
class CommandTaskFlow {
  friend class CommandTask;
  friend class CommandExecutor;

 public:
  using TaskType =
      std::function<Result<Nil, ErrorResult>(AllocatedCommandBuffer& cmd)>;

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
      CommandType command_type, const TaskType& commands,
      std::uint32_t use_render_resource_count,
      const std::vector<MM::RenderSystem::WaitAllocatedSemaphore>&
          wait_semaphores,
      const std::vector<MM::RenderSystem::AllocateSemaphore>&
          signal_semaphores);

  /**
   * \remark The commands must not contain VkQueueSubmit().
   */
  CommandTask& AddTask(
      CommandType command_type, const std::vector<TaskType>& commands,
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
}  // namespace RenderSystem
}  // namespace MM