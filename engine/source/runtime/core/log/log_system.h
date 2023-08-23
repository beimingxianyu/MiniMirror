#pragma once

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <mutex>

#include "runtime/platform/base/error.h"

namespace MM {
namespace LogSystem {
class LogSystem {
 public:
  enum class LogLevel : uint8_t { TRACE, INFO, DEBUG, WARN, ERROR, FATAL };

 public:
  LogSystem(const LogSystem& other) = delete;
  LogSystem(LogSystem&& other) = delete;
  LogSystem& operator=(const LogSystem& other) = delete;
  LogSystem& operator=(LogSystem&& other) = delete;

 public:
  static LogSystem* GetInstance();

  void SetLevel(const LogLevel& level);

  template <typename... ARGS>
  void Log(const LogLevel& level, ARGS&&... args) const {
    switch (level) {
      case LogLevel::TRACE:
        LogSystem::logger_->trace(std::forward<ARGS>(args)...);
        break;
      case LogLevel::INFO:
        LogSystem::logger_->info(std::forward<ARGS>(args)...);
        break;
      case LogLevel::DEBUG:
        LogSystem::logger_->debug(std::forward<ARGS>(args)...);
        break;
      case LogLevel::WARN:
        LogSystem::logger_->warn(std::forward<ARGS>(args)...);
        break;
      case LogLevel::ERROR:
        LogSystem::logger_->error(std::forward<ARGS>(args)...);
        break;
      case LogLevel::FATAL:
        LogSystem::logger_->critical(std::forward<ARGS>(args)...);
        FatalCallback(std::forward<ARGS>(args)...);
        break;
      default:
        break;
    }
  }

  template <typename... ARGS>
  void LogTrace(ARGS&&... args) const {
    logger_->trace(std::forward<ARGS>(args)...);
  }

  template <typename... ARGS>
  void LogInfo(ARGS&&... args) const {
    logger_->info(std::forward<ARGS>(args)...);
  }

  template <typename... ARGS>
  void LogDebug(ARGS&&... args) const {
    logger_->debug(std::forward<ARGS>(args)...);
  }

  template <typename... ARGS>
  void LogWarn(ARGS&&... args) const {
    logger_->warn(std::forward<ARGS>(args)...);
  }

  template <typename... ARGS>
  void LogError(ARGS&&... args) const {
    logger_->error(std::forward<ARGS>(args)...);
  }

  template <typename... ARGS>
  void LogFatal(ARGS&&... args) const {
    logger_->critical(std::forward<ARGS>(args)...);
    FatalCallback(std::forward<ARGS>(args)...);
  }

  template <typename... Targets>
  void FatalCallback(Targets&&... args) const {
    const std::string format_str = fmt::format(std::forward<Targets>(args)...);
    throw std::runtime_error(format_str);
  }

  ExecuteResult CheckResult(ExecuteResult result, const std::string& log_prefix,
                            LogLevel log_level = LogLevel::ERROR) const;

 private:
  /**
   * \brief Destroy the instance. If it is successfully destroyed, it returns
   * true, otherwise it returns false. \return If it is successfully destroyed,
   * it returns true, otherwise it returns false.
   */
  static bool Destroy();

 protected:
  LogSystem() = default;
  ~LogSystem();
  static LogSystem* log_system_;

 private:
  static std::mutex sync_flag_;
  std::shared_ptr<spdlog::logger> logger_{nullptr};
};

#define MM_LOG_SYSTEM MM_log_system

#define MM_IMPORT_LOG_SYSTEM                       \
  inline MM::LogSystem::LogSystem* MM_LOG_SYSTEM { \
    MM::LogSystem::LogSystem::GetInstance()        \
  }

#define MM_LOG(log_level, ...) \
  MM_LOG_SYSTEM->Log(log_level, MM_CODE_LOCATION + __VA_ARGS__)

#define MM_LOG_DEBUG(...) \
  MM_LOG(MM::LogSystem::LogSystem::LogLevel::DEBUG, __VA_ARGS__)

#define MM_LOG_TRACE(...) \
  MM_LOG(MM::LogSystem::LogSystem::LogLevel::TRACE, __VA_ARGS__)

#define MM_LOG_INFO(...) \
  MM_LOG(MM::LogSystem::LogSystem::LogLevel::INFO, __VA_ARGS__)

#define MM_LOG_WARN(...) \
  MM_LOG(MM::LogSystem::LogSystem::LogLevel::WARN, __VA_ARGS__)

#define MM_LOG_ERROR(...) \
  MM_LOG(MM::LogSystem::LogSystem::LogLevel::ERROR, __VA_ARGS__)

#define MM_LOG_FATAL(...) \
  MM_LOG(MM::LogSystem::LogSystem::LogLevel::FATAL, __VA_ARGS__)

#define MM_RESULT_CODE __MM__result_code_name

#define MM_CHECK(executor, failed_callback)                                  \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = (executor);                           \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::ERROR); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_TRACE(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = (executor);                           \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::TRACE); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_INFO(executor, failed_callback)                        \
  {                                                                         \
    if (ExecuteResult MM_RESULT_CODE = (executor);                          \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                     \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,          \
                                 MM::LogSystem::LogSystem::LogLevel::INFO); \
      failed_callback                                                       \
    }                                                                       \
  }

#define MM_CHECK_LOG_DEBUG(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = (executor);                           \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::DEBUG); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_WARN(executor, failed_callback)                        \
  {                                                                         \
    if (ExecuteResult MM_RESULT_CODE = (executor);                          \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                     \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,          \
                                 MM::LogSystem::LogSystem::LogLevel::WARN); \
      failed_callback                                                       \
    }                                                                       \
  }

#define MM_CHECK_LOG_ERROR(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = (executor);                           \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::ERROR); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_FATAL(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = (executor);                           \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::FATAL); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_WITHOUT_LOG(executor, failed_callback) \
  {                                                     \
    if (ExecuteResult MM_RESULT_CODE = (executor);      \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) { \
      failed_callback                                   \
    }                                                   \
  }

#define MM_RESULT_CODE_EQUAL(result, target_error_code) \
  (result) == (target_error_code)
}  // namespace LogSystem
}  // namespace MM
