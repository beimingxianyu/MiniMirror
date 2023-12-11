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
        struct OnlyWriteType {};
        struct OnlyReadType {};
        struct ReadAndWriteType {};


      static GetMultiplyObject get_multiply_object{};
      static GetOneObject get_one_object{};
      static OnlyWriteType only_not_write;
      static OnlyReadType only_read;
      static ReadAndWriteType read_and_write;
    }

    static StaticTrait::GetMultiplyObject st_get_multiply_object{};
    static StaticTrait::GetOneObject st_get_one_object{};
    static StaticTrait::OnlyWriteType st_only_not_write;
    static StaticTrait::OnlyReadType st_only_read;
    static StaticTrait::ReadAndWriteType st_read_and_write;
}