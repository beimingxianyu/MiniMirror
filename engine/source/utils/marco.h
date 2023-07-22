#pragma once

#include <cstddef>

namespace MM {
namespace Utils {
#define OFFSET_OF(class_, member) offsetof(class_, member)

#define MM_CAT_IMP(a, b) a##b

#define MM_CAT(a, b) MM_CAT_IMP(a, b)

#define MM_STR_CAT_IMP_IMP(str) #str

#define MM_STR_CAT_IMP(str1, str2) MM_STR_CAT_IMP_IMP(str1##str2)

#define MM_STR_CAT(str1, str2) MM_STR_CAT_IMP(str1, str2)

#define MM_STR_IMP(str) #str

#define MM_STR(str) MM_STR_IMP(str)

#define MM_CODE_LOCATION_IMP_IMP(line)                                 \
  (std::string("[File:") + __FILE__ + " || Function:" + __FUNCTION__ + \
   " || Line:" + #line + "]")

#define MM_CODE_LOCATION_IMP(line) MM_CODE_LOCATION_IMP_IMP(line)

#define MM_CODE_LOCATION MM_CODE_LOCATION_IMP(__LINE__)

#define MM_CONFIG_SYSTEM MM_config_system

#define MM_IMPORT_CONFIG_SYSTEM                             \
  inline MM::ConfigSystem::ConfigSystem* MM_CONFIG_SYSTEM { \
    MM::ConfigSystem::ConfigSystem::GetInstance()           \
  }

#define MM_FILE_SYSTEM MM_file_system

#define MM_IMPORT_FILE_SYSTEM                               \
  inline const MM::FileSystem::FileSystem* MM_FILE_SYSTEM { \
    MM::FileSystem::FileSystem::GetInstance()               \
  }

#define MM_TASK_SYSTEM MM_task_system

#define MM_IMPORT_TASK_SYSTEM                         \
  inline MM::TaskSystem::TaskSystem* MM_TASK_SYSTEM { \
    MM::TaskSystem::TaskSystem::GetInstance()         \
  }

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
    if (ExecuteResult MM_RESULT_CODE = executor;                             \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::ERROR); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_TRACE(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = executor;                             \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::TRACE); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_INFO(executor, failed_callback)                        \
  {                                                                         \
    if (ExecuteResult MM_RESULT_CODE = executor;                            \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                     \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,          \
                                 MM::LogSystem::LogSystem::LogLevel::INFO); \
      failed_callback                                                       \
    }                                                                       \
  }

#define MM_CHECK_LOG_DEBUG(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = executor;                             \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::DEBUG); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_WARN(executor, failed_callback)                        \
  {                                                                         \
    if (ExecuteResult MM_RESULT_CODE = executor;                            \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                     \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,          \
                                 MM::LogSystem::LogSystem::LogLevel::WARN); \
      failed_callback                                                       \
    }                                                                       \
  }

#define MM_CHECK_LOG_ERROR(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = executor;                             \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::ERROR); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_LOG_FATAL(executor, failed_callback)                        \
  {                                                                          \
    if (ExecuteResult MM_RESULT_CODE = executor;                             \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {                      \
      MM_LOG_SYSTEM->CheckResult(MM_RESULT_CODE, MM_CODE_LOCATION,           \
                                 MM::LogSystem::LogSystem::LogLevel::FATAL); \
      failed_callback                                                        \
    }                                                                        \
  }

#define MM_CHECK_WITHOUT_LOG(executor, failed_callback) \
  {                                                     \
    if (ExecuteResult MM_RESULT_CODE = executor;        \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) { \
      failed_callback                                   \
    }                                                   \
  }

#define MM_MULTIPLE_CHECK(executor, failed_callback)    \
  {                                                     \
    if (ExecuteResult MM_RESULT_CODE = executor;        \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) { \
      MM_LOG_SYSTEM->CheckMultipleResult(               \
          MM_RESULT_CODE, MM_CODE_LOCATION,             \
          MM::LogSystem::LogSystem::LogLevel::ERROR);   \
      failed_callback                                   \
    }                                                   \
  }

#define MM_MULTIPLE_CHECK_LOG_TRACE(executor, failed_callback) \
  {                                                            \
    if (ExecuteResult MM_RESULT_CODE = executor;               \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {        \
      MM_LOG_SYSTEM->CheckMultipleResult(                      \
          MM_RESULT_CODE, MM_CODE_LOCATION,                    \
          MM::LogSystem::LogSystem::LogLevel::TRACE);          \
      failed_callback                                          \
    }                                                          \
  }

#define MM_MULTIPLE_CHECK_LOG_INFO(executor, failed_callback) \
  {                                                           \
    if (ExecuteResult MM_RESULT_CODE = executor;              \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {       \
      MM_LOG_SYSTEM->CheckMultipleResult(                     \
          MM_RESULT_CODE, MM_CODE_LOCATION,                   \
          MM::LogSystem::LogSystem::LogLevel::INFO);          \
      failed_callback                                         \
    }                                                         \
  }

#define MM_MULTIPLE_CHECK_LOG_DEBUG(executor, failed_callback) \
  {                                                            \
    if (ExecuteResult MM_RESULT_CODE = executor;               \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {        \
      MM_LOG_SYSTEM->CheckMultipleResult(                      \
          MM_RESULT_CODE, MM_CODE_LOCATION,                    \
          MM::LogSystem::LogSystem::LogLevel::DEBUG);          \
      failed_callback                                          \
    }                                                          \
  }

#define MM_MULTIPLE_CHECK_LOG_WARN(executor, failed_callback) \
  {                                                           \
    if (ExecuteResult MM_RESULT_CODE = executor;              \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {       \
      MM_LOG_SYSTEM->CheckMultipleResult(                     \
          MM_RESULT_CODE, MM_CODE_LOCATION,                   \
          MM::LogSystem::LogSystem::LogLevel::WARN);          \
      failed_callback                                         \
    }                                                         \
  }

#define MM_MULTIPLE_CHECK_LOG_ERROR(executor, failed_callback) \
  {                                                            \
    if (ExecuteResult MM_RESULT_CODE = executor;               \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {        \
      MM_LOG_SYSTEM->CheckMultipleResult(                      \
          MM_RESULT_CODE, MM_CODE_LOCATION,                    \
          MM::LogSystem::LogSystem::LogLevel::ERROR);          \
      failed_callback                                          \
    }                                                          \
  }

#define MM_MULTIPLE_CHECK_LOG_FATAL(executor, failed_callback) \
  {                                                            \
    if (ExecuteResult MM_RESULT_CODE = executor;               \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {        \
      MM_LOG_SYSTEM->CheckMultipleResult(                      \
          MM_RESULT_CODE, MM_CODE_LOCATION,                    \
          MM::LogSystem::LogSystem::LogLevel::FATAL);          \
      failed_callback                                          \
    }                                                          \
  }

#define MM_MULTIPLE_CHECK_WITHOUT_LOG(executor, failed_callback) \
  {                                                              \
    if (ExecuteResult MM_RESULT_CODE = executor;                 \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {          \
      failed_callback                                            \
    }                                                            \
  }

#define MM_RESULT_CODE_EQUAL(result, target_error_code) \
  (result) == (target_error_code)

#define MM_ASSET_SYSTEM MM_asset_system

#define MM_IMPORT_ASSET_SYSTEM                           \
  inline MM::AssetSystem::AssetSystem* MM_ASSET_SYSTEM { \
    MM::AssetSystem::AssetSystem::GetInstance()          \
  }

#define MM_VK_RESULT_CODE __MM_vk_result_code_name

// TODO 添加对各种情况的警告（如内存不足）
#define MM_VK_CHECK(vk_executor, failed_callback) \
  {                                               \
    if (VkResult MM_VK_RESULT_CODE = vk_executor; \
        MM_VK_RESULT_CODE != VK_SUCCESS) {        \
      MM_LOG_ERROR("Vulkan runtime error.");      \
      failed_callback;                            \
    }                                             \
  }

#define MM_VK_CHECK_WITHOUT_LOG(vk_executor, failed_callback) \
  {                                                           \
    if (VkResult MM_VK_RESULT_CODE = vk_executor;             \
        MM_VK_RESULT_CODE != VK_SUCCESS) {                    \
      failed_callback;                                        \
    }                                                         \
  }

#define MM_ADD_COUNTER_SUFFIX_IMP_IMP(name, counter) name##counter

#define MM_ADD_COUNTER_SUFFIX_IMP(name, counter) \
  MM_ADD_COUNTER_SUFFIX_IMP_IMP(name, counter)

#define MM_ADD_COUNTER_SUFFIX(name) MM_ADD_COUNTER_SUFFIX_IMP(name, __COUNTER__)

#define MM_Print(object) std::cout << (object) << std::endl

#define RUN_TIME_IMP(expression, describe, start, end)                     \
  std::chrono::system_clock::time_point start =                            \
      std::chrono::system_clock::now();                                    \
  expression std::chrono::system_clock::time_point end =                   \
      std::chrono::system_clock::now();                                    \
  std::cout << describe                                                    \
            << std::chrono::duration_cast<std::chrono::nanoseconds>(end -  \
                                                                    start) \
                   .count()                                                \
            << std::endl;

#define RUN_TIME(expression, describe)                             \
  RUN_TIME_IMP(expression, describe, MM_ADD_COUNTER_SUFFIX(start), \
               MM_ADD_COUNTER_SUFFIX(end))
}  // namespace Utils
}  // namespace MM