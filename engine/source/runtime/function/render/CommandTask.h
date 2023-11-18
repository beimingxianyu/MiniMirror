//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/CommandTaskFlow.h"

namespace MM {
namespace RenderSystem {
class CommandTask {
  friend class CommandExecutor;
  friend class CommandTaskFlow;

 public:
  CommandTask() = delete;
  ~CommandTask();
  CommandTask(const CommandTask& other) = delete;
  CommandTask(CommandTask&& other) noexcept;
  CommandTask& operator=(const CommandTask& other) = delete;
  CommandTask& operator=(CommandTask&& other) noexcept;

 public:
  const CommandBufferType& GetCommandType() const;

  const CommandTaskFlow& GetCommandTaskFlow() const;

  const std::vector<TaskType>& GetCommands() const;

  std::vector<const TaskType*> GetCommandsIncludeSubCommandTasks() const;

  std::uint32_t GetRequireCommandBufferNumber() const;

  std::uint32_t GetRequireCommandBufferNumberIncludeSubCommandTasks() const;

  std::vector<const CommandTask*> GetSubCommandTasks() const;

  std::uint32_t GetSubCommandTasksNumber() const;

  Result<Nil> AddPreCommandTask(CommandTaskID pre_command_task_ID);

  Result<Nil> AddPreCommandTask(
      const std::vector<CommandTaskID>& pre_command_task_IDs);

  Result<Nil> AddPostCommandTask(
      CommandTaskID post_command_task_ID);

  Result<Nil> AddPostCommandTask(
      const std::vector<CommandTaskID>& post_command_task_IDs);

  Result<Nil> Merge(CommandTaskID sub_command_task_ID);

  Result<Nil> Merge(
      const std::vector<CommandTaskID>& sub_command_task_IDs);

  bool HaveSubCommandTasks() const;

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const RenderResourceDataID& render_resource_ID);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      const std::vector<RenderResourceDataID>& render_resource_IDs);

  void AddCrossTaskFLowSyncRenderResourceIDs(
      std::vector<RenderResourceDataID>&& render_resource_IDs);

  CommandTaskID GetCommandTaskID() const;

  bool IsAsyncRecord() const;

  void SetAsyncRecord();

  void SetSyncRecord();

  bool IsSubTask() const;

  bool IsValid() const;

 private:
  CommandTask(CommandTaskFlow* task_flow, const CommandType& command_type,
              const std::vector<TaskType>& commands, bool is_async_record);

 private:
  static std::atomic<CommandTaskID> current_command_tast_ID_;

  CommandTaskFlow* task_flow_{nullptr};
  CommandTaskID command_task_ID_{0};
  CommandType command_type_{CommandType::UNDEFINED};
  std::vector<TaskType> commands_{};
  
  mutable std::vector<CommandTaskID> pre_tasks_{};
  mutable std::vector<CommandTaskID> post_tasks_{};
  mutable std::vector<CommandTaskID> sub_tasks_{};

  bool is_sub_task_{false};
  bool is_async_record_{false};
  std::vector<RenderResourceDataID> cross_task_flow_sync_render_resource_IDs_;
};
}  // namespace RenderSystem
}  // namespace MM