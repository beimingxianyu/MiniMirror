//
// Created by beimingxianyu on 23-5-18.
//

#pragma once

#include "runtime/core/log/log_system.h"
#include "utils/marco.h"

MM_IMPORT_LOG_SYSTEM;

namespace MM {
    namespace StaticTrait {
        struct GetMultiplyObject {};
        struct GetOneObject {};
    }

    static StaticTrait::GetMultiplyObject st_get_multiply_object{};
    static StaticTrait::GetOneObject st_get_one_object{};
}