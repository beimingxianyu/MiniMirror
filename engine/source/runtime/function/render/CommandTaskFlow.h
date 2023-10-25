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
  CommandTaskFlow();
  ~CommandTaskFlow() = default;
  CommandTaskFlow(const CommandTaskFlow& other) = delete;
  CommandTaskFlow(CommandTaskFlow&& other) noexcept;
  CommandTaskFlow& operator=(const CommandTaskFlow& other) = delete;
  CommandTaskFlow& operator=(CommandTaskFlow&& other) noexcept;

 public:
  /**
   * \remark The commands must not contain VkQueueSubmit().
   */
  CommandTask& AddTask(CommandType command_type, const TaskType& commands,
                       bool is_async_record);

  /**
   * \remark The commands must not contain VkQueueSubmit().
   */
  CommandTask& AddTask(CommandType command_type,
                       const std::vector<TaskType>& commands,
                       bool is_async_record);

  bool HaveCommandTask(CommandTaskID command_task_ID) const;

  Result<const std::vector<TaskType>*, ErrorResult> GetCommands(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const TaskType*>, ErrorResult> GetSubCommands(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const TaskType*>, ErrorResult>
  GetCommandsIncludeSubCommandTask(CommandTaskID command_task_ID) const;

  Result<const CommandTask*, ErrorResult> GetCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>, ErrorResult> GetSubCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>, ErrorResult> GetPreCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>, ErrorResult> GetPostCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>, ErrorResult>
  GetCommandTaskIncludeSubCommandTask(CommandTaskID command_task_ID) const;

  Result<std::uint32_t, ErrorResult> GetRequireCommandBufferNumber(
      CommandTaskID command_task_ID) const;

  Result<std::uint32_t, ErrorResult>
  GetSubCommandTaskRequireCommandBufferNumber(
      CommandTaskID command_task_ID) const;

  Result<std::uint32_t, ErrorResult>
  GetRequireCommandBufferNumberIncludeSubCommandTasks(
      CommandTaskID command_task_ID) const;

  std::uint32_t GetTaskNumber() const;

  std::uint32_t GetGraphNumber() const;

  std::uint32_t GetComputeNumber() const;

  std::uint32_t GetTransformNumber() const;

  bool IsRootTask(CommandTaskID command_task_ID) const;

  Result<Nil, ErrorResult> AddPreCommandTask(CommandTaskID main_command_task_ID,
                                             CommandTaskID pre_command_task_ID);

  Result<Nil, ErrorResult> AddPreCommandTask(
      CommandTaskID main_command_task_ID,
      const std::vector<CommandTaskID>& pre_command_task_IDs);

  Result<Nil, ErrorResult> AddPostCommandTask(
      CommandTaskID main_command_task_ID, CommandTaskID post_command_task_ID);

  Result<Nil, ErrorResult> AddPostCommandTask(
      CommandTaskID main_command_task_ID,
      const std::vector<CommandTaskID>& post_command_task_IDs);

  Result<Nil, ErrorResult> Merge(CommandTaskID main_command_task_ID,
                                 CommandTaskID sub_command_task_ID); 

  Result<Nil, ErrorResult> Merge(
      CommandTaskID main_command_task_ID,
      const std::vector<CommandTaskID>& sub_command_task_ID);

  void Clear();

  bool IsValidSubmissionObject() const;

  bool IsValid() const;

 private:
  bool HaveRing() const;

  /**
  * \note This function can only be called when there is no ring in the command flow.
  */ 
  bool SubCommandTaskRelationshipIsValid() const;

  void RemoveRootTask(const CommandTask& command_task);

  void RemoveTask(CommandTask& command_task);

  bool HaveRingHelp(std::unordered_set<CommandTaskID>& old_command_task_ID,
                   CommandTaskID current_command_task_ID) const;

  /**
  * \note This function can only be called when there is no ring in the command flow.
  */
  void HelpGetAllPreCommandTaskIDs(
      std::unordered_set<CommandTaskID>& pre_command_task_IDs,
      CommandTaskID current_command_task_ID) const;

  /**
  * \note This function can only be called when there is no ring in the command flow.
  */
  bool HelpCheckAllPostCommandTaskIDs(
      std::unordered_set<CommandTaskID>& pre_command_task_IDs,
      CommandTaskID current_command_task_ID) const;

 private:
  static std::atomic<CommandTaskFlowID> current_command_task_flow_ID_;

  CommandTaskFlowID task_flow_ID_{0};
  std::vector<CommandTaskID> root_command_task_IDs_{};
  std::list<CommandTask> tasks_{};

  std::array<std::uint32_t, 3> task_numbers_{};
};
}  // namespace RenderSystem
}  // namespace MM