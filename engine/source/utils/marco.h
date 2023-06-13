#pragma once

namespace MM {
namespace Utils {
#define MM_STR(str) #str

#define MM_CAT_IMP(a, b) a##b

#define MM_CAT(a, b) MM_CAT_IMP(a, b)

#define CODE_LOCATION_IMP_IMP(line)                                    \
  (std::string("[File:") + __FILE__ + " || Function:" + __FUNCTION__ + \
   " || Line:" + #line + "]")

#define CODE_LOCATION_IMP(line) CODE_LOCATION_IMP_IMP(line)

#define CODE_LOCATION CODE_LOCATION_IMP(__LINE__)

#define CONFIG_SYSTEM config_system

#define IMPORT_CONFIG_SYSTEM                             \
  inline MM::ConfigSystem::ConfigSystem* CONFIG_SYSTEM { \
    MM::ConfigSystem::ConfigSystem::GetInstance()        \
  }

#define FILE_SYSTEM file_system

#define IMPORT_FILE_SYSTEM                         \
  inline MM::FileSystem::FileSystem* FILE_SYSTEM { \
    MM::FileSystem::FileSystem::GetInstance()      \
  }

#define TASK_SYSTEM task_system

#define IMPORT_TASK_SYSTEM                         \
  inline MM::TaskSystem::TaskSystem* TASK_SYSTEM { \
    MM::TaskSystem::TaskSystem::GetInstance()      \
  }

#define LOG_SYSTEM log_system

#define IMPORT_LOG_SYSTEM                       \
  inline MM::LogSystem::LogSystem* LOG_SYSTEM { \
    MM::LogSystem::LogSystem::GetInstance()     \
  }

#define LOG(log_level, ...) \
  LOG_SYSTEM->Log(log_level, CODE_LOCATION + __VA_ARGS__)

#define LOG_DEBUG(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Debug, __VA_ARGS__)

#define LOG_INFO(...) LOG(MM::LogSystem::LogSystem::LogLevel::Info, __VA_ARGS__)

#define LOG_WARN(...) LOG(MM::LogSystem::LogSystem::LogLevel::Warn, __VA_ARGS__)

#define LOG_ERROR(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Error, __VA_ARGS__)

#define LOG_FATAL(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Fatal, __VA_ARGS__)

#define MM_RESULT_CODE __MM__result_code_name

#define MM_CHECK(executor, failed_callback)                   \
  {                                                           \
    if (ExecuteResult MM_RESULT_CODE = executor;              \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {       \
      LOG_SYSTEM->CheckResult(MM_RESULT_CODE, CODE_LOCATION); \
      failed_callback                                         \
    }                                                         \
  }

#define MM_CHECK_WITHOUT_LOG(executor, failed_callback) \
  {                                                     \
    if (ExecuteResult MM_RESULT_CODE = executor;        \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) { \
      failed_callback                                   \
    }                                                   \
  }

#define MM_MULTIPLE_CHECK(executor, failed_callback)                  \
  {                                                                   \
    if (ExecuteResult MM_RESULT_CODE = executor;                      \
        MM_RESULT_CODE != MM::ExecuteResult::SUCCESS) {               \
      LOG_SYSTEM->CheckMultipleResult(MM_RESULT_CODE, CODE_LOCATION); \
      failed_callback                                                 \
    }                                                                 \
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

#define ASSET_SYSTEM asset_system

#define IMPORT_ASSET_SYSTEM                           \
  inline MM::AssetSystem::AssetSystem* ASSET_SYSTEM { \
    MM::AssetSystem::AssetSystem::GetInstance()       \
  }

#define VK_RESULT_CODE __MM_vk_result_code_name

// TODO 添加对各种情况的警告（如内存不足）
#define VK_CHECK(vk_executor, failed_callback)                                 \
  {                                                                            \
    if (VkResult VK_RESULT_CODE = vk_executor; VK_RESULT_CODE != VK_SUCCESS) { \
      LOG_SYSTEM->CheckResult(VK_RESULT_CODE, CODE_LOCATION);                  \
      failed_callback;                                                         \
    }                                                                          \
  }

#define VK_CHECK_WITHOUT_LOG(vk_executor, failed_callback)                     \
  {                                                                            \
    if (VkResult VK_RESULT_CODE = vk_executor; VK_RESULT_CODE != VK_SUCCESS) { \
      failed_callback;                                                         \
    }                                                                          \
  }

#define ADD_COUNTER_SUFFIX_IMP_IMP(name, counter) name##counter

#define ADD_COUNTER_SUFFIX_IMP(name, counter) \
  ADD_COUNTER_SUFFIX_IMP_IMP(name, counter)

#define ADD_COUNTER_SUFFIX(name) ADD_COUNTER_SUFFIX_IMP(name, __COUNTER__)

#define Print(object) std::cout << (object) << std::endl

#define HELLO_WORLD Print("hello world")

#define PRINT_HELLO_WORLD_FUNCTION \
  void PrintHelloWorldFunction() { HELLO_WORLD; }

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

#define RUN_TIME(expression, describe)                          \
  RUN_TIME_IMP(expression, describe, ADD_COUNTER_SUFFIX(start), \
               ADD_COUNTER_SUFFIX(end))
}  // namespace Utils
}  // namespace MM