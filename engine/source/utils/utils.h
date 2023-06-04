#pragma once

#include <cmath>
#include <memory>
#include <string>

namespace MM {
namespace Utils {
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

bool IsPrime(std::uint32_t x);

bool IsPrime(std::uint64_t x);

std::uint32_t MinPrime(std::uint32_t n);

std::uint64_t MinPrime(std::uint64_t n);

bool IsPrime32(std::uint32_t x);

bool IsPrime64(std::uint64_t x);

std::uint32_t MinPrime32(std::uint32_t n);

std::uint64_t MinPrime64(std::uint64_t n);
}  // namespace Utils
}  // namespace MM
