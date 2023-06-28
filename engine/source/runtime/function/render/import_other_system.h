#pragma once

#include "runtime/core/log/log_system.h"
#include "runtime/core/task_system/task_system.h"
#include "runtime/platform/config_system/config_system.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/resource/asset_system/AssetSystem.h"
#include "utils/marco.h"

namespace MM {
namespace RenderSystem {
IMPORT_CONFIG_SYSTEM;

IMPORT_FILE_SYSTEM;

IMPORT_LOG_SYSTEM;

IMPORT_ASSET_SYSTEM;

IMPORT_TASK_SYSTEM;
}  // namespace RenderSystem
}  // namespace MM
