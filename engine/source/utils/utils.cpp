#include "utils/utils.h"

std::size_t MM::Utils::StringHash(const std::string& target) {
  constexpr std::hash<std::string> hash;
  return hash(target);
}
