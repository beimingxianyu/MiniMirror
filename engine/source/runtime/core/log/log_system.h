#pragma once

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <mutex>

namespace MM {
class LogSystem {
public:
  enum class LogLevel : uint8_t { Trace, Info, Debug, Warn, Error, Fatal };

public:
  ~LogSystem() = delete;
  LogSystem(const LogSystem& other) = delete;
  LogSystem(LogSystem&& other) = delete;
  LogSystem& operator=(const LogSystem& other) = delete;
  LogSystem& operator=(LogSystem&& other) = delete;


public:
  static LogSystem* GetInstance();

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
        logger_->trace(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Info:
        logger_->info(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Debug:
        logger_->debug(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Warn:
        logger_->warn(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Error:
        logger_->error(std::forward<ARGS>(args)...);
        break;
      case LogLevel::Fatal:
        logger_->critical(std::forward<ARGS>(args)...);
        FatalCallback(std::forward<ARGS>(args)...);
        break;
    }
  }

  template <typename... Targs>
  void FatalCallback(Targs&&... args) {
    const std::string format_str = fmt::format(std::forward<Targs>(args)...);
    throw std::runtime_error(format_str);
  }

private:
  LogSystem() = default;
  static std::mutex sync_flag_;
  static LogSystem* log_system_;
  static std::shared_ptr<spdlog::logger> logger_;
};

std::mutex LogSystem::sync_flag_{std::mutex{}};
LogSystem* LogSystem::log_system_{nullptr};
std::shared_ptr<spdlog::logger> LogSystem::logger_{nullptr};
} // namespace MM
