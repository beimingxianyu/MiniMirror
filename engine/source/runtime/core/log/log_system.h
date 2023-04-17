#pragma once

#include <iostream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/async.h>

#include <mutex>

#include "runtime/platform/base/error.h"

namespace MM {
namespace LogSystem {
class LogSystem {
public:
  enum class LogLevel : uint8_t { Trace, Info, Debug, Warn, Error, Fatal };

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
      case LogLevel::Trace:
        LogSystem::logger_->trace(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Info:
        LogSystem::logger_->info(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Debug:
        LogSystem::logger_->debug(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Warn:
        LogSystem::logger_->warn(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Error:
        LogSystem::logger_->error(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Fatal:
        LogSystem::logger_->critical(std::forward<ARGS>(args)...);
        FatalCallback(std::forward<ARGS>(args)...);
        break;
      default:
        break;
    }
  }

  template<typename ...ARGS>
  void LogTrace(ARGS&&... args) const {
    logger_->trace(std::forward<ARGS>(args)...);
  }

  template<typename  ...ARGS>
  void LogInfo(ARGS&&... args) const {
    logger_->info(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogDebug(ARGS&&... args) const {
    logger_->debug(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogWarn(ARGS&&... args) const {
    logger_->warn(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogError(ARGS&&... args) const {
    logger_->error(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogFatal(ARGS&&... args) const {
    logger_->critical(std::forward<ARGS>(args)...);
    FatalCallback(std::forward<ARGS>(args)...);
  }

  template <typename... Targets>
  void FatalCallback(Targets&&... args) const {
    const std::string format_str = fmt::format(std::forward<Targets>(args)...);
    throw std::runtime_error(format_str);
  }

  ExecuteResult CheckResult(ExecuteResult result) const;

  ExecuteResult CheckMultipleResult(ExecuteResult result) const;

private:
  /**
   * \brief Destroy the instance. If it is successfully destroyed, it returns true, otherwise it returns false.
   * \return If it is successfully destroyed, it returns true, otherwise it returns false.
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
}
} // namespace MM
