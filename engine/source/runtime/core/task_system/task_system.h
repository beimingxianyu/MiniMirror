#pragma once

#include <memory>
#include <mutex>

#include "runtime/core/task_system/pre_header.h"

namespace MM {
namespace TaskSystem {
enum class TaskType { Total, Common, Render, Physical };

class TaskSystem {
 public:
  TaskSystem(const TaskSystem& other) = delete;
  TaskSystem(TaskSystem&& other) = delete;
  TaskSystem& operator=(const TaskSystem& other) = delete;
  TaskSystem& operator=(TaskSystem&& other) = delete;

 public:
  static TaskSystem* GetInstance();

  tf::Future<void> Run(const TaskType& task_type, Taskflow& task_flow);

  tf::Future<void> Run(const TaskType& task_type, Taskflow&& task_flow);

  template <typename C>
  tf::Future<void> Run(const TaskType& task_type, Taskflow& task_flow,
                       C&& callable);

  template <typename C>
  tf::Future<void> Run(const TaskType& task_type, Taskflow&& task_flow,
                       C&& callable);

  tf::Future<void> RunN(const TaskType& task_type, Taskflow& task_flow,
                        size_t N);

  tf::Future<void> RunN(const TaskType& task_type, Taskflow&& task_flow,
                        size_t N);

  template <typename C>
  tf::Future<void> RunN(const TaskType& task_type, Taskflow& task_flow,
                        size_t N, C&& callable);

  template <typename C>
  tf::Future<void> RunN(const TaskType& task_type, Taskflow&& task_flow,
                        size_t N, C&& callable);

  template <typename P>
  tf::Future<void> RunUntil(const TaskType& task_type, Taskflow& task_flow,
                            P&& pred);

  template <typename P>
  tf::Future<void> RunUntil(const TaskType& task_type, Taskflow&& task_flow,
                            P&& pred);

  template <typename P, typename C>
  tf::Future<void> RunUntil(const TaskType& task_type, Taskflow& task_flow,
                            P&& pred, C&& callable);

  template <typename P, typename C>
  tf::Future<void> RunUntil(const TaskType& task_type, Taskflow&& task_flow,
                            P&& pred, C&& callable);

  template <typename T>
  void RunAndWait(const TaskType& task_type, T& target);

  template <typename P>
  void LoopUntil(const TaskType& task_type, P&& predicate);

  void WaitForAll(const TaskType& task_type);

  size_t NumWorkers(const TaskType& task_type) const noexcept;

  size_t NumTopologies(const TaskType& task_type) const;

  size_t NumTaskFlows(const TaskType& task_type) const;

  int ThisWorkerId(const TaskType& task_type) const;

  template <typename F, typename... ArgsT>
  auto Async(const TaskType& task_type, F&& f, ArgsT&&... args);

  template <typename F, typename... ArgsT>
  auto NamedAsync(const TaskType& task_type, const std::string& name, F&& f,
                  ArgsT&&... args);

  template <typename F, typename... ArgsT>
  void SilentAsync(const TaskType& task_type, F&& f, ArgsT&&... args);

  template <typename F, typename... ArgsT>
  void NamedSilentAsync(const TaskType& task_type, const std::string& name,
                        F&& f, ArgsT&&... args);

  template <typename Observer, typename... ArgsT>
  std::shared_ptr<Observer> MakeObserver(const TaskType& task_type,
                                         ArgsT&&... args);

  template <typename Observer>
  void RemoveObserver(const TaskType& task_type,
                      std::shared_ptr<Observer> observer);

  size_t NumObservers(const TaskType& task_type) const noexcept;

 private:
  ~TaskSystem() = default;

  static bool Destroy();

  Executor& ChooseExecutor(const TaskType& task_type);

  const Executor& ChooseExecutor(const TaskType& task_type) const;

 protected:
  TaskSystem();
  static TaskSystem* task_system_;

 private:
  static std::mutex sync_flag_;

  Executor total_executor_;
  Executor logic_executor_;
  Executor render_executor_;
  Executor physical_executor_;
};

template <typename C>
tf::Future<void> TaskSystem::Run(const TaskType& task_type, Taskflow& task_flow,
                                 C&& callable) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run(task_flow, std::forward<C>(callable));
}

template <typename C>
tf::Future<void> TaskSystem::Run(const TaskType& task_type,
                                 Taskflow&& task_flow, C&& callable) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run(std::move(task_flow), std::forward<C>(callable));
}

template <typename C>
tf::Future<void> TaskSystem::RunN(const TaskType& task_type,
                                  Taskflow& task_flow, size_t N, C&& callable) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_n(task_flow, N, std::forward<C>(callable));
}

template <typename C>
tf::Future<void> TaskSystem::RunN(const TaskType& task_type,
                                  Taskflow&& task_flow, size_t N,
                                  C&& callable) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_n(std::move(task_flow), N, std::forward<C>(callable));
}

template <typename P>
tf::Future<void> TaskSystem::RunUntil(const TaskType& task_type,
                                      Taskflow& task_flow, P&& pred) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_until(task_flow, std::forward<P>(pred));
}

template <typename P>
tf::Future<void> TaskSystem::RunUntil(const TaskType& task_type,
                                      Taskflow&& task_flow, P&& pred) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_until(std::move(task_flow), std::forward<P>(pred));
}

template <typename P, typename C>
tf::Future<void> TaskSystem::RunUntil(const TaskType& task_type,
                                      Taskflow& task_flow, P&& pred,
                                      C&& callable) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_until(task_flow, std::forward<P>(pred),
                            std::forward<C>(callable));
}

template <typename P, typename C>
tf::Future<void> TaskSystem::RunUntil(const TaskType& task_type,
                                      Taskflow&& task_flow, P&& pred,
                                      C&& callable) {
  auto& executor = ChooseExecutor(task_type);
  return executor.run_until(std::move(task_flow), std::forward<P>(pred),
                            std::forward<C>(callable));
}

template <typename T>
void TaskSystem::RunAndWait(const TaskType& task_type, T& target) {
  auto& executor = ChooseExecutor(task_type);
  executor.run_and_wait(target);
}

template <typename P>
void TaskSystem::LoopUntil(const TaskType& task_type, P&& predicate) {
  auto& executor = ChooseExecutor(task_type);
  executor.loop_until(std::forward<P>(predicate));
}

template <typename F, typename... ArgsT>
auto TaskSystem::Async(const TaskType& task_type, F&& f, ArgsT&&... args) {
  auto& executor = ChooseExecutor(task_type);
  return executor.async(std::forward<F>(f), std::forward<ArgsT>(args)...);
}

template <typename F, typename... ArgsT>
auto TaskSystem::NamedAsync(const TaskType& task_type, const std::string& name,
                            F&& f, ArgsT&&... args) {
  auto& executor = ChooseExecutor(task_type);
  return executor.named_async(name, std::forward<F>(f),
                              std::forward<ArgsT>(args)...);
}

template <typename F, typename... ArgsT>
void TaskSystem::SilentAsync(const TaskType& task_type, F&& f,
                             ArgsT&&... args) {
  auto& executor = ChooseExecutor(task_type);
  executor.silent_async(std::forward<F>(f), std::forward<ArgsT>(args)...);
}

template <typename F, typename... ArgsT>
void TaskSystem::NamedSilentAsync(const TaskType& task_type,
                                  const std::string& name, F&& f,
                                  ArgsT&&... args) {
  auto& executor = ChooseExecutor(task_type);
  executor.named_silent_async(name, std::forward<F>(f),
                              std::forward<ArgsT>(args)...);
}

template <typename Observer, typename... ArgsT>
std::shared_ptr<Observer> TaskSystem::MakeObserver(const TaskType& task_type,
                                                   ArgsT&&... args) {
  auto& executor = ChooseExecutor(task_type);
  return executor.make_observer<Observer>(std::forward<ArgsT>(args)...);
}

template <typename Observer>
void TaskSystem::RemoveObserver(const TaskType& task_type,
                                std::shared_ptr<Observer> observer) {
  auto& executor = ChooseExecutor(task_type);
  executor.remove_observer(observer);
}
}  // namespace TaskSystem
}  // namespace MM