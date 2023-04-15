#include "runtime/platform/base/error.h"

MM::ExecuteResult MM::operator
|(ExecuteResult l_result, ExecuteResult r_result) {
  return l_result |= r_result;
}

MM::ExecuteResult MM::operator|=(ExecuteResult l_result,
                                 ExecuteResult r_result) {
  return ExecuteResult{static_cast<std::uint32_t>(l_result) |
                       static_cast<std::uint32_t>(r_result)};
}

MM::ExecuteResult MM::operator
&(ExecuteResult l_result, ExecuteResult r_result) {
  return l_result &= r_result;
}

MM::ExecuteResult MM::operator
&=(ExecuteResult l_result, ExecuteResult r_result) {
  return ExecuteResult{static_cast<std::uint32_t>(l_result) &
                       static_cast<std::uint32_t>(r_result)};
}
