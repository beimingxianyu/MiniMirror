#include "runtime/core/log/log_system.h"

#include <cstdint>

std::shared_ptr<MM::LogSystem::LogSystem> MM::LogSystem::LogSystem::log_system_{nullptr};
std::mutex MM::LogSystem::LogSystem::sync_flag_{};

MM::LogSystem::LogSystem::~LogSystem() {
  logger_->flush();
  spdlog::drop_all();
  log_system_ = nullptr;
}

std::shared_ptr<MM::LogSystem::LogSystem> MM::LogSystem::LogSystem::GetInstance() {
  if (log_system_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!log_system_) {
      log_system_.reset(new LogSystem{});

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

bool MM::LogSystem::LogSystem::Destroy() {
  if (log_system_) {
    if (log_system_.use_count() == 1) {
      log_system_.reset();
      return true;
    }
  }
  return false;
}
