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
  DESTROY_FAILED,
  // Such as child "class A", parent "class B", B have a member std::set<A> A_set.
  // The instance of class B b_instance and the instance of class A a_instance,
  // b.A_set not contain a_instance and call A_set.at(a_instance) will return this error. 
  PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT 
};

ExecuteResult operator|(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator|=(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&=(ExecuteResult l_result, ExecuteResult r_result);
}
