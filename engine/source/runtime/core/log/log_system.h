#pragma once

#include <iostream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/async.h>

#include <mutex>

namespace MM {
namespace LogSystem {
class LogSystem {
public:
  enum class LogLevel : uint8_t { Trace, Info, Debug, Warn, Error, Fatal };

public:
  ~LogSystem();
  LogSystem(const LogSystem& other) = delete;
  LogSystem(LogSystem&& other) = delete;
  LogSystem& operator=(const LogSystem& other) = delete;
  LogSystem& operator=(LogSystem&& other) = delete;


public:
  static std::shared_ptr<LogSystem> GetInstance();

  void SetLevel(const LogLevel& level) {
    switch (level) {
      case LogLevel::Trace:
        logger_->set_level(spdlog::level::trace);
        break;
      case LogLevel::Info:
        logger_->set_level(spdlog::level::info);
        break;
      case LogLevel::Debug:
        logger_->set_level(spdlog::level::debug);
        break;
      case LogLevel::Warn:
        logger_->set_level(spdlog::level::warn);
        break;
      case LogLevel::Error:
        logger_->set_level(spdlog::level::err);
        break;
      case LogLevel::Fatal:
        logger_->set_level(spdlog::level::critical);
        break;
    }
  }

  template <typename... ARGS>
  void Log(const LogLevel& level, ARGS&&... args) {
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
  void LogTrace(ARGS&&... args) {
    logger_->trace(std::forward<ARGS>(args)...);
  }

  template<typename  ...ARGS>
  void LogInfo(ARGS&&... args) {
    logger_->info(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogDebug(ARGS&&... args) {
    logger_->debug(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogWarn(ARGS&&... args) {
    logger_->warn(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogError(ARGS&&... args) {
    logger_->error(std::forward<ARGS>(args)...);
  }

  template<typename ...ARGS>
  void LogFatal(ARGS&&... args) {
    logger_->critical(std::forward<ARGS>(args)...);
    FatalCallback(std::forward<ARGS>(args)...);
  }

  template <typename... Targets>
  void FatalCallback(Targets&&... args) {
    const std::string format_str = fmt::format(std::forward<Targets>(args)...);
    throw std::runtime_error(format_str);
  }

  /**
   * \brief Destroy the instance. If it is successfully destroyed, it returns true, otherwise it returns false.
   * \remark Only when no other module uses this system can it be destroyed successfully.
   * \return If it is successfully destroyed, it returns true, otherwise it returns false.
   */
  bool Destroy();

protected:
  LogSystem() = default;
  static std::shared_ptr<LogSystem> log_system_;

private:
  static std::mutex sync_flag_;
  std::shared_ptr<spdlog::logger> logger_{nullptr};
};
}
} // namespace MM
