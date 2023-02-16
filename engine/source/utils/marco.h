#pragma once

namespace MM {
namespace Utils {
#define MM_STR(str) #str

#define MM_CAT_IMP(a, b) a##b

#define MM_CAT(a, b) MM_CAT_IMP(a, b)

#define LOG_SYSTEM log_system

#define IMPORT_LOG_SYSTEM                                      \
  inline std::shared_ptr<MM::LogSystem::LogSystem> LOG_SYSTEM{ \
      MM::LogSystem::LogSystem::GetInstance()};

#define LOG(log_level, ...)                                                 \
  LOG_SYSTEM->Log(log_level, std::string("[") + std::string(__FUNCTION__) + \
                                 "]" + __VA_ARGS__);

#define LOG_DEBUG(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Debug, __VA_ARGS__);

#define LOG_INFO(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Info, __VA_ARGS__);

#define LOG_WARN(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Warn, __VA_ARGS__);

#define LOG_ERROR(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Error, __VA_ARGS__);

#define LOG_FATAL(...) \
  LOG(MM::LogSystem::LogSystem::LogLevel::Fatal, __VA_ARGS__);

#define CONFIG_SYSTEM config_system

#define IMPORT_CONFIG_SYSTEM                                            \
  inline std::shared_ptr<MM::ConfigSystem::ConfigSystem> CONFIG_SYSTEM{ \
      MM::ConfigSystem::ConfigSystem::GetInstance()};

#define FILE_SYSTEM file_system

#define IMPORT_FILE_SYSTEM                                        \
  inline std::shared_ptr<MM::FileSystem::FileSystem> FILE_SYSTEM{ \
      MM::FileSystem::FileSystem::GetInstance()};

#define ASSET_SYSTEM asset_system

#define IMPORT_ASSET_SYSTEM                                          \
  inline std::shared_ptr<MM::AssetSystem::AssetSystem> ASSET_SYSTEM{ \
      MM::AssetSystem::AssetSystem::GetInstance()};

// TODO 添加对各种情况的警告（如内存不足）
#define VK_CHECK(vk_executor, failed_callback) \
  if (vk_executor) {                           \
    failed_callback;                           \
  }

}  // namespace Utils
}  // namespace MM