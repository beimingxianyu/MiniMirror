#include "runtime/core/log/log_system.h"

#include <cstdint>

MM::LogSystem::LogSystem* MM::LogSystem::LogSystem::log_system_{nullptr};
std::mutex MM::LogSystem::LogSystem::sync_flag_{};

MM::LogSystem::LogSystem::~LogSystem() {
  logger_->flush();
  spdlog::drop_all();
  log_system_ = nullptr;
}

MM::LogSystem::LogSystem* MM::LogSystem::LogSystem::GetInstance() {
  if (log_system_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!log_system_) {
      log_system_ = new LogSystem{};

      auto console_sink =
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      console_sink->set_level(spdlog::level::trace);
      console_sink->set_pattern("[%^%l%$] %v");

      const spdlog::sinks_init_list sink_list = {console_sink};

      spdlog::init_thread_pool(8192, 1);

      log_system_->logger_ = std::make_shared<spdlog::async_logger>(
          "muggle_logger", sink_list.begin(), sink_list.end(),
          spdlog::thread_pool(), spdlog::async_overflow_policy::block);
      //log_system_->logger_ = std::make_shared<spdlog::logger>(
      //    "muggle_logger", sink_list.begin(), sink_list.end());
      log_system_->SetLevel(LogLevel::Trace);
      spdlog::register_logger(log_system_->logger_);
    }
  }
  return log_system_;
}

void MM::LogSystem::LogSystem::SetLevel(const LogLevel& level) {
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

bool MM::LogSystem::LogSystem::Destroy() {
  std::lock_guard<std::mutex> guard(sync_flag_);
  if (log_system_) {
    delete log_system_;
    log_system_ = nullptr;

    return true;
  }
  return false;
}
