#pragma once

#include "runtime/core/log/log_system.h"
#include "runtime/core/task_system/task_system.h"
#include "runtime/platform/config_system/config_system.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/resource/asset_system/AssetSystem.h"
#include "utils/marco.h"

namespace MM {
namespace RenderSystem {
MM_IMPORT_CONFIG_SYSTEM;

MM_IMPORT_FILE_SYSTEM;

MM_IMPORT_LOG_SYSTEM;

MM_IMPORT_ASSET_SYSTEM;

MM_IMPORT_TASK_SYSTEM;
}  // namespace RenderSystem
}  // namespace MM
