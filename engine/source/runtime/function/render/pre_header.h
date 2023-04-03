#pragma once

#include "utils/marco.h"
#include "runtime/core/log/log_system.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/platform/config_system/config_system.h"
#include "runtime/resource/asset_system/asset_system.h"

namespace MM {
namespace RenderSystem {
IMPORT_CONFIG_SYSTEM

IMPORT_FILE_SYSTEM

IMPORT_LOG_SYSTEM

IMPORT_ASSET_SYSTEM
}
}
