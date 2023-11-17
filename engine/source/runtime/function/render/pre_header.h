#pragma once

#include "runtime/core/log/log_system.h"
#include "runtime/core/task_system/task_system.h"
#include "runtime/platform/config_system/config_system.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/resource/asset_system/AssetSystem.h"
#include "utils/marco.h"

#define MM_VK_RESULT_CODE __MM_vk_result_code_name

// TODO 添加对各种情况的警告（如内存不足）
#define MM_VK_CHECK(vk_executor, failed_callback)   \
  {                                                 \
    if (VkResult MM_VK_RESULT_CODE = (vk_executor); \
        MM_VK_RESULT_CODE != VK_SUCCESS) {          \
      MM_LOG_ERROR("Vulkan runtime error.");        \
      failed_callback;                              \
    }                                               \
  }

#define MM_VK_CHECK_WITHOUT_LOG(vk_executor, failed_callback) \
  {                                                           \
    if (VkResult MM_VK_RESULT_CODE = vk_executor;             \
        MM_VK_RESULT_CODE != VK_SUCCESS) {                    \
      failed_callback;                                        \
    }                                                         \
  }

namespace MM {
namespace RenderSystem {
MM_IMPORT_CONFIG_SYSTEM;

MM_IMPORT_FILE_SYSTEM;

MM_IMPORT_LOG_SYSTEM;

MM_IMPORT_ASSET_SYSTEM;

MM_IMPORT_TASK_SYSTEM;
}  // namespace RenderSystem
}  // namespace MM
