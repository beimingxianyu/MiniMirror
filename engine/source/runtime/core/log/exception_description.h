//
// Created by 北冥咸鱼 on 2023/8/23.
//
#include "runtime/core/log/log_system.h"
#include "runtime/platform/base/cross_platform_header.h"
#include "utils/error.h"
#include "utils/utils.h"

namespace MM {
namespace LogSystem {
MM_IMPORT_LOG_SYSTEM;

std::string ConstructLogMessage(const char* function_name,
                                const char* description);

template <LogSystem::LogLevel log_level>
class LogDescription {
 public:
  LogDescription() = delete;
  ~LogDescription() = default;
  LogDescription(const char* function_name, const char* description)
      : function_name_(function_name), description_(description) {}
  LogDescription(const LogDescription& other) = default;
  LogDescription(LogDescription&& other) noexcept = default;
  LogDescription& operator=(const LogDescription& other) = default;
  LogDescription& operator=(LogDescription&& other) noexcept = default;

  void operator()(ErrorResult error_result) {
    MM_LOG_SYSTEM->CheckResult(
        error_result.GetErrorCode(),
        ConstructLogMessage(function_name_, description_), log_level);
  }

 private:
  const char* function_name_{nullptr};
  const char* description_{nullptr};
};

using TraceLogDescription = LogDescription<LogSystem::LogLevel::TRACE>;
using InfoLogDescription = LogDescription<LogSystem::LogLevel::INFO>;
using DebugLogDescription = LogDescription<LogSystem::LogLevel::DEBUG>;
using WarnLogDescription = LogDescription<LogSystem::LogLevel::WARN>;
using ErrorLogDescription = LogDescription<LogSystem::LogLevel::ERROR>;
using FatalLogDescription = LogDescription<LogSystem::LogLevel::FATAL>;
}  // namespace LogSystem
}  // namespace MM

#define MM_LOG_DESCRIPTION_MESSAGE(function_name, description) \
  MM::LogSystem::ConstructLogMessage(function_name, #description)

#define MM_LOG_DESCRIPTION_MESSAGE2(function_name, description) \
  MM::LogSystem::ConstructLogMessage(function_name, #description)

#define MM_LOG_DESCRIPTION_CHECK_RESULT(error_result, function_name, \
                                        description, log_level)      \
  MM_LOG_SYSTEM->CheckResult(                                        \
      (error_result).GetErrorCode(),                                 \
      MM_LOG_DESCRIPTION_MESSAGE(function_name, #description), log_level);

#define MM_LOG_DESCRIPTION_CHECK_RESULT2(error_result, function_name, \
                                         description, log_level)      \
  MM_LOG_SYSTEM->CheckResult(                                         \
      (error_result).GetErrorCode(),                                  \
      MM_LOG_DESCRIPTION_MESSAGE2(function_name, description), log_level);

#define MM_TRACE_DESCRIPTION(description)                                      \
  [function_name = MM_FUNCTION_NAME](MM::ErrorResult error_result) {           \
    MM_LOG_DESCRIPTION_CHECK_RESULT(error_result, function_name, description,  \
                                    MM::LogSystem::LogSystem::LogLevel::TRACE) \
  }

#define MM_INFO_DESCRIPTION(description)                                      \
  [function_name = MM_FUNCTION_NAME](MM::ErrorResult error_result) {          \
    MM_LOG_DESCRIPTION_CHECK_RESULT(error_result, function_name, description, \
                                    MM::LogSystem::LogSystem::LogLevel::INFO) \
  }

#define MM_DEBUG_DESCRIPTION(description)                                      \
  [function_name = MM_FUNCTION_NAME](MM::ErrorResult error_result) {           \
    MM_LOG_DESCRIPTION_CHECK_RESULT(error_result, function_name, description,  \
                                    MM::LogSystem::LogSystem::LogLevel::DEBUG) \
  }

#define MM_WARN_DESCRIPTION(description)                                      \
  [function_name = MM_FUNCTION_NAME](MM::ErrorResult error_result) {          \
    MM_LOG_DESCRIPTION_CHECK_RESULT(error_result, function_name, description, \
                                    MM::LogSystem::LogSystem::LogLevel::WARN) \
  }

#define MM_ERROR_DESCRIPTION(description)                                      \
  [function_name = MM_FUNCTION_NAME](MM::ErrorResult error_result) {           \
    MM_LOG_DESCRIPTION_CHECK_RESULT(error_result, function_name, description,  \
                                    MM::LogSystem::LogSystem::LogLevel::ERROR) \
  }

#define MM_FATAL_DESCRIPTION(description)                                      \
  [function_name = MM_FUNCTION_NAME](MM::ErrorResult error_result) {           \
    MM_LOG_DESCRIPTION_CHECK_RESULT(error_result, function_name, description,  \
                                    MM::LogSystem::LogSystem::LogLevel::FATAL) \
  }

#define MM_TRACE_DESCRIPTION2(description)                           \
  [function_name = MM_FUNCTION_NAME,                                 \
   description_info = (description)](MM::ErrorResult error_result) { \
    MM_LOG_DESCRIPTION_CHECK_RESULT2(                                \
        error_result, function_name, description,                    \
        MM::LogSystem::LogSystem::LogLevel::TRACE)                   \
  }

#define MM_INFO_DESCRIPTION2(description)                                      \
  [function_name = MM_FUNCTION_NAME,                                           \
   description_info = (description)](MM::ErrorResult error_result) {           \
    MM_LOG_DESCRIPTION_CHECK_RESULT2(error_result, function_name, description, \
                                     MM::LogSystem::LogSystem::LogLevel::INFO) \
  }

#define MM_DEBUG_DESCRIPTION2(description)                           \
  [function_name = MM_FUNCTION_NAME,                                 \
   description_info = (description)](MM::ErrorResult error_result) { \
    MM_LOG_DESCRIPTION_CHECK_RESULT2(                                \
        error_result, function_name, description,                    \
        MM::LogSystem::LogSystem::LogLevel::DEBUG)                   \
  }

#define MM_WARN_DESCRIPTION2(description)                                      \
  [function_name = MM_FUNCTION_NAME,                                           \
   description_info = (description)](MM::ErrorResult error_result) {           \
    MM_LOG_DESCRIPTION_CHECK_RESULT2(error_result, function_name, description, \
                                     MM::LogSystem::LogSystem::LogLevel::WARN) \
  }

#define MM_ERROR_DESCRIPTION2(description)                           \
  [function_name = MM_FUNCTION_NAME,                                 \
   description_info = (description)](MM::ErrorResult error_result) { \
    MM_LOG_DESCRIPTION_CHECK_RESULT2(                                \
        error_result, function_name, description,                    \
        MM::LogSystem::LogSystem::LogLevel::ERROR)                   \
  }

#define MM_FATAL_DESCRIPTION2(description)                           \
  [function_name = MM_FUNCTION_NAME,                                 \
   description_info = (description)](MM::ErrorResult error_result) { \
    MM_LOG_DESCRIPTION_CHECK_RESULT2(                                \
        error_result, function_name, description,                    \
        MM::LogSystem::LogSystem::LogLevel::FATAL)                   \
  }
