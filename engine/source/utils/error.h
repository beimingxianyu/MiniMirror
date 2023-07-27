#pragma once
#include <cstdint>

namespace MM {
namespace Utils {
enum class ExecuteResult : std::uint32_t {
  SUCCESS = 0,
  UNDEFINED_ERROR,
  OUT_OF_HOST_MEMORY,
  OUT_OF_DEVICE_MEMORY,
  OBJECT_IS_INVALID,
  NO_SUCH_CONFIG,
  TYPE_CONVERSION_FAILED,
  LOAD_CONFIG_FROM_FILE_FAILED,
  FILE_IS_NOT_EXIST,
  CREATE_OBJECT_FAILED,
  INITIALIZATION_FAILED,
  DESTROY_FAILED,
  // Such as child "class A", parent "class B", B have a member std::set<A>
  // A_set. The instance of class B b_instance and the instance of class A
  // a_instance, b.A_set not contain a_instance and call A_set.at(a_instance,)
  // will return this error.
  PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT,
  RENDER_COMMAND_RECORD_OR_SUBMIT_FAILED,
  INPUT_PARAMETERS_ARE_INCORRECT,
  TIMEOUT,
  OPERATION_NOT_SUPPORTED,
  INPUT_PARAMETERS_ARE_NOT_SUITABLE,
  // rename/delete/create/remove/etc.
  FILE_OPERATION_ERROR,
  SYNCHRONIZE_FAILED,
  NO_AVAILABLE_ELEMENT,
  CUSTOM_ERROR
};

ExecuteResult operator|(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator|=(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&=(ExecuteResult l_result, ExecuteResult r_result);
}  // namespace Utils
}  // namespace MM
