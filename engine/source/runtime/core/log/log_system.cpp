#include "runtime/core/log/log_system.h"

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
      console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

      const spdlog::sinks_init_list sink_list = {console_sink};

      spdlog::init_thread_pool(8192, 1);

      log_system_->logger_ = std::make_shared<spdlog::async_logger>(
          "muggle_logger", sink_list.begin(), sink_list.end(),
          spdlog::thread_pool(), spdlog::async_overflow_policy::block);
      // log_system_->logger_ = std::make_shared<spdlog::logger>(
      //     "muggle_logger", sink_list.begin(), sink_list.end());
      log_system_->SetLevel(LogLevel::TRACE);
      spdlog::register_logger(log_system_->logger_);
    }
  }
  return log_system_;
}

void MM::LogSystem::LogSystem::SetLevel(const LogLevel& level) {
  switch (level) {
    case LogLevel::TRACE:
      logger_->set_level(spdlog::level::trace);
      break;
    case LogLevel::INFO:
      logger_->set_level(spdlog::level::info);
      break;
    case LogLevel::DEBUG:
      logger_->set_level(spdlog::level::debug);
      break;
    case LogLevel::WARN:
      logger_->set_level(spdlog::level::warn);
      break;
    case LogLevel::ERROR:
      logger_->set_level(spdlog::level::err);
      break;
    case LogLevel::FATAL:
      logger_->set_level(spdlog::level::critical);
      break;
  }
}

void MM::LogSystem::LogSystem::CheckResult(MM::ErrorCode result,
                                           const std::string& description,
                                           LogLevel log_level) const {
  switch (result) {
    case ErrorCode::SUCCESS:
      break;
    case ErrorCode::UNDEFINED_ERROR:
      Log(log_level, std::string("[Undefined error]    ") + description);
      break;
    case ErrorCode::OUT_OF_HOST_MEMORY:
      Log(log_level, "[Out of host memory] " + description);
      break;
    case ErrorCode::OUT_OF_DEVICE_MEMORY:
      Log(log_level, "[Out of device memory] " + description);
      break;
    case ErrorCode::OBJECT_IS_INVALID:
      Log(log_level, "[Object is invalid] " + description);
      break;
    case ErrorCode::NO_SUCH_CONFIG:
      Log(log_level, "[The desired setting does not exist] " + description);
      break;
    case ErrorCode::TYPE_CONVERSION_FAILED:
      Log(log_level, "[Type conversion failed] " + description);
      break;
      //    case ErrorCode::INITIALIZATION_FAILED:
    case ErrorCode::CREATE_OBJECT_FAILED:
      Log(log_level, "[Create object/initialization failed] " + description);
      break;
    case ErrorCode::LOAD_CONFIG_FROM_FILE_FAILED:
      Log(log_level, "[Failed to load config from file] " + description);
      break;
    case ErrorCode::FILE_IS_NOT_EXIST:
      Log(log_level, "[File is not exist] " + description);
      break;
    case ErrorCode::DESTROY_FAILED:
      Log(log_level, "[Failed to destroy object] " + description);
      break;
    case ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT:
      Log(log_level,
          "[The parent object does not contain specific child objects] " +
              description);
      break;
    case ErrorCode::RENDER_COMMAND_RECORD_OR_SUBMIT_FAILED:
      Log(log_level,
          "[Failed to submit or record render command] " + description);
      break;
    case ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT:
      Log(log_level, "[The input parameters are incorrect] " + description);
      break;
    case ErrorCode::TIMEOUT:
      Log(log_level, "[The operation timed out] " + description);
      break;
    case ErrorCode::OPERATION_NOT_SUPPORTED:
      Log(log_level,
          "[An unsupported operation was performed] " + description);
      break;
    case ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE:
      Log(log_level,
          "[The input parameters are not suitable] " + description);
      break;
    case ErrorCode::FILE_OPERATION_ERROR:
      Log(log_level, "[File operation error] " + description);
      break;
    case ErrorCode::SYNCHRONIZE_FAILED:
      Log(log_level, "[Synchronize failed] " + description);
      break;
    case ErrorCode::NO_AVAILABLE_ELEMENT:
      Log(log_level, "[No available element] " + description);
      break;
    case ErrorCode::NULL_OBJECT_ERROR:
      Log(log_level, "[Ues null object] " + description);
      break;
    case ErrorCode::CUSTOM_ERROR:
      Log(log_level, "[Custom error] " + description);
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
