#include <runtime/core/log/exception_description.h>

std::string MM::LogSystem::ConstructLogMessage(const char* function_name,
                                               const char* description) {
  std::string log_string{};
  log_string.reserve(Utils::LengthOfStr(function_name) +
                     Utils::LengthOfStr(description) + 3 + 1);
  log_string += "[";
  log_string += function_name;
  log_string += "] ";
  log_string += description;

  return log_string;
}
