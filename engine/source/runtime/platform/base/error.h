#pragma once
#include <cstdint>

namespace MM {
enum class ExecuteResult : std::uint32_t {
  SUCCESS = 0u,
  UNDEFINED_ERROR = 1u,
  OUT_OF_HOST_MEMORY = 1u << 1,
  OUT_OF_DEVICE_MEMORY = 1u << 2,
  OBJECT_IS_INVALID = 1u << 3,
  NO_SUCH_CONFIG = 1u << 4,
  TYPE_CONVERSION_FAILED = 1u << 5,
  LOAD_CONFIG_FROM_FILE_FAILED = 1u << 6,
  FILE_IS_NOT_EXIST = 1u << 7,
  CREATE_OBJECT_FAILED = 1u << 8,
  INITIALIZATION_FAILED = CREATE_OBJECT_FAILED,
  DESTROY_FAILED = 1u << 9,
  // Such as child "class A", parent "class B", B have a member std::set<A>
  // A_set. The instance of class B b_instance and the instance of class A
  // a_instance, b.A_set not contain a_instance and call A_set.at(a_instance)
  // will return this error.
  PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT = 1u << 10,
  RENDER_COMMAND_RECORD_OR_SUBMIT_FAILED = 1u << 11,
  INPUT_PARAMETERS_ARE_INCORRECT = 1u << 12,
  TIMEOUT = 1u << 13,
  OPERATION_NOT_SUPPORTED = 1U << 14,
  INPUT_PARAMETERS_ARE_NOT_SUITABLE = 1U << 15,
  // rename/delete/create/remove/etc.
  FILE_OPERATION_ERROR = 1U << 16
};

ExecuteResult operator|(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator|=(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&=(ExecuteResult l_result, ExecuteResult r_result);
}  // namespace MM
