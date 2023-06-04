#include "utils//error.h"

MM::Utils::ExecuteResult MM::Utils::operator|(ExecuteResult l_result,
                                              ExecuteResult r_result) {
  return l_result |= r_result;
}

MM::Utils::ExecuteResult MM::Utils::operator|=(ExecuteResult l_result,
                                               ExecuteResult r_result) {
  return ExecuteResult{static_cast<std::uint32_t>(l_result) |
                       static_cast<std::uint32_t>(r_result)};
}

MM::Utils::ExecuteResult MM::Utils::operator&(ExecuteResult l_result,
                                              ExecuteResult r_result) {
  return l_result &= r_result;
}

MM::Utils::ExecuteResult MM::Utils::operator&=(ExecuteResult l_result,
                                               ExecuteResult r_result) {
  return ExecuteResult{static_cast<std::uint32_t>(l_result) &
                       static_cast<std::uint32_t>(r_result)};
}
