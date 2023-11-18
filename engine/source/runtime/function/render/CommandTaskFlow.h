//
// Created by beimingxianyu on 23-10-5.
//
#pragma once

#include <unordered_set>

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

  Result<const std::vector<TaskType>*> GetCommands(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const TaskType*>> GetSubCommands(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const TaskType*>>
  GetCommandsIncludeSubCommandTask(CommandTaskID command_task_ID) const;

  Result<const CommandTask*> GetCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>> GetSubCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>> GetPreCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>> GetPostCommandTask(
      CommandTaskID command_task_ID) const;

  Result<std::vector<const CommandTask*>>
  GetCommandTaskIncludeSubCommandTask(CommandTaskID command_task_ID) const;

  Result<std::uint32_t> GetRequireCommandBufferNumber(
      CommandTaskID command_task_ID) const;

  Result<std::uint32_t>
  GetSubCommandTaskRequireCommandBufferNumber(
      CommandTaskID command_task_ID) const;

  Result<std::uint32_t>
  GetRequireCommandBufferNumberIncludeSubCommandTasks(
      CommandTaskID command_task_ID) const;

  std::uint32_t GetTaskNumber() const;

  std::uint32_t GetGraphNumber() const;

  std::uint32_t GetComputeNumber() const;

  std::uint32_t GetTransformNumber() const;

  bool IsRootTask(CommandTaskID command_task_ID) const;

  Result<Nil> AddPreCommandTask(CommandTaskID main_command_task_ID,
                                             CommandTaskID pre_command_task_ID);

  Result<Nil> AddPreCommandTask(
      CommandTaskID main_command_task_ID,
      const std::vector<CommandTaskID>& pre_command_task_IDs);

  Result<Nil> AddPostCommandTask(
      CommandTaskID main_command_task_ID, CommandTaskID post_command_task_ID);

  Result<Nil> AddPostCommandTask(
      CommandTaskID main_command_task_ID,
      const std::vector<CommandTaskID>& post_command_task_IDs);

  Result<Nil> Merge(CommandTaskID main_command_task_ID,
                                 CommandTaskID sub_command_task_ID); 

  Result<Nil> Merge(
      CommandTaskID main_command_task_ID,
      const std::vector<CommandTaskID>& sub_command_task_IDs);

  void Clear();

  bool IsValidSubmissionObject() const;

  bool IsValid() const;

 private:
  bool HaveRing() const;

  /**
  * \note This function can only be called when there is no ring in the command flow.
  */ 
  bool SubCommandTaskRelationshipIsValid() const;

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
  std::vector<CommandTask> tasks_{};

  std::array<std::uint32_t, 3> task_numbers_{};
};
}  // namespace RenderSystem
}  // namespace MM