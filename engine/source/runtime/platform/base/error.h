#pragma once
#include <cstdint>

namespace MM {
enum class ExecuteResult : std::uint32_t{
  SUCCESS = 0u,
  UNDEFINED_ERROR,
  OUT_OF_HOST_MEMORY,
  OUT_OF_DEVICE_MEMORY,
  OBJECT_IS_INVALID,
  NO_SUCH_CONFIG,
  TYPE_CONVERSION_FAILED,
  LOAD_CONFIG_FROM_FILE_FAILED,
  FILE_IS_NOT_EXIST,
  INITIALIZATION_FAILED,
  DESTROY_FAILED
};

ExecuteResult operator|(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator|=(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&=(ExecuteResult l_result, ExecuteResult r_result);
}
