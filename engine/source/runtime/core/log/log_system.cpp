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

MM::ExecuteResult MM::LogSystem::LogSystem::CheckResult(
    ExecuteResult result) const {
  switch (result) {
    case ExecuteResult::SUCCESS:
      break;
    case ExecuteResult::UNDEFINED_ERROR:
      LogError("Undefined error.");
      break;
    case ExecuteResult::OUT_OF_HOST_MEMORY:
      LogError("Out of host memory.");
      break;
    case ExecuteResult::OUT_OF_DEVICE_MEMORY:
      LogError("Out of device memory.");
      break;
    case ExecuteResult::OBJECT_IS_INVALID:
      LogError("Object is invalid");
      break;
    case ExecuteResult::NO_SUCH_CONFIG:
      LogError("The desired setting does not exist.");
      break;
    case ExecuteResult::TYPE_CONVERSION_FAILED:
      LogError("Type conversion failed");
      break;
    case ExecuteResult::INITIALIZATION_FAILED:
      LogError("Initialization failed.");
      break;
    case ExecuteResult::LOAD_CONFIG_FROM_FILE_FAILED:
      LogError("Failed to load config from file.");
      break;
    case ExecuteResult::FILE_IS_NOT_EXIST:
      LogError("File is not exist.");
      break;
    case ExecuteResult::DESTROY_FAILED:
      LogError("Failed to destroy object.");
      break;
    case ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT:
      LogError("The parent object does not contain specific child objects.");
  }

  return result;
}

MM::ExecuteResult MM::LogSystem::LogSystem::CheckMultipleResult(
    ExecuteResult result) const {
  if ((result & ExecuteResult::SUCCESS) == ExecuteResult::SUCCESS) {
    return result;
  }
  if ((result & ExecuteResult::UNDEFINED_ERROR) ==
      ExecuteResult::UNDEFINED_ERROR) {
    LogError("Undefined error.");
  }
  if ((result & ExecuteResult::OUT_OF_HOST_MEMORY) ==
      ExecuteResult::OUT_OF_HOST_MEMORY) {
    LogError("Out of host memory.");
  }
  if ((result & ExecuteResult::OUT_OF_DEVICE_MEMORY) ==
      ExecuteResult::OUT_OF_DEVICE_MEMORY) {
    LogError("Out of device memory.");
  }
  if ((result & ExecuteResult::OBJECT_IS_INVALID) ==
      ExecuteResult::OBJECT_IS_INVALID) {
    LogError("Object is invalid");
  }
  if ((result & ExecuteResult::NO_SUCH_CONFIG) ==
      ExecuteResult::NO_SUCH_CONFIG) {
    LogError("The desired setting does not exist.");
  }
  if ((result & ExecuteResult::TYPE_CONVERSION_FAILED) ==
      ExecuteResult::TYPE_CONVERSION_FAILED) {
    LogError("Type conversion failed");
  }
  if ((result & ExecuteResult::INITIALIZATION_FAILED) ==
      ExecuteResult::INITIALIZATION_FAILED) {
    LogError("Initialization failed.");
  }
  if ((result & ExecuteResult::LOAD_CONFIG_FROM_FILE_FAILED) ==
      ExecuteResult::LOAD_CONFIG_FROM_FILE_FAILED) {
    LogError("Failed to load config from file.");
  }
  if ((result & ExecuteResult::FILE_IS_NOT_EXIST) ==
      ExecuteResult::FILE_IS_NOT_EXIST) {
    LogError("File is not exist.");
  }
  if ((result & ExecuteResult::DESTROY_FAILED) ==
      ExecuteResult::DESTROY_FAILED) {
    LogError("Failed to destroy object.");
  }
  if ((result &
       ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT) ==
      ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT) {
    LogError("The parent object does not contain specific child objects.");
  }

  return result;
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
