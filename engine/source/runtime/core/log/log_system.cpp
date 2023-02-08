#include "runtime/core/log/log_system.h"

#include <cstdint>

#include "spdlog/async.h"

MM::LogSystem* MM::LogSystem::GetInstance() {
  if (log_system_) {

  } else {

    std::lock_guard<std::mutex> guard(sync_flag_);
    if (!log_system_) {
      log_system_ = new LogSystem();

      auto console_sink =
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      console_sink->set_level(spdlog::level::trace);
      console_sink->set_pattern("[%^%l%$] %v");

      const spdlog::sinks_init_list sink_list = {console_sink};

      spdlog::init_thread_pool(8192, 1);

      logger_ = std::make_shared<spdlog::async_logger>(
          "muggle_logger", sink_list.begin(), sink_list.end(),
          spdlog::thread_pool(), spdlog::async_overflow_policy::block);
      logger_->set_level(spdlog::level::trace);

      spdlog::register_logger(logger_);
    }

  }
  return log_system_;
}
