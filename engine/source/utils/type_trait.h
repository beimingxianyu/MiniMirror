#pragma once

namespace MM {
namespace Utils {
template <typename... BoolArgs>
static bool CheckAllTrue(bool arg1, BoolArgs... args) {
  return arg1 & CheckAllTrue(args...);
}

template <typename... Args>
class LastArg;

template <typename LastArgType>
class LastArg<LastArgType> {
 public:
  using Type = LastArgType;
};

template <typename FirstArg, typename... Args>
class LastArg<FirstArg, Args...> {
 public:
  using Type = typename LastArg<Args...>::Type;
};
}  // namespace Utils
}  // namespace MM
