#pragma once

#include <cmath>
#include <memory>
#include <string>

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

template <typename ElementType, typename Destructor, typename... Args>
std::shared_ptr<ElementType> MakeSharedWithDestructor(Destructor destructor,
                                                      Args&&... args) {
  auto ptr = new ElementType(std::forward<Args>(args)...);
  std::shared_ptr<ElementType> shared_p{nullptr};
  shared_p.reset(ptr, destructor);
  return shared_p;
}

std::size_t StringHash(const std::string& target);

template <typename IntegerType,
          typename IsInterType = typename std::enable_if<
              std::is_integral<IntegerType>::value, void>::type>
typename std::decay<IntegerType>::type IntegerMult(IntegerType integer_value,
                                                   float float_value) {
  return static_cast<typename std::decay<IntegerType>::type>(
      std::floor(static_cast<float>(integer_value) * float_value + 0.5));
}
}  // namespace Utils
}  // namespace MM
