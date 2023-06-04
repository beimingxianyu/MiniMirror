#include "utils/utils.h"

std::size_t MM::Utils::StringHash(const std::string& target) {
  constexpr std::hash<std::string> hash;
  return hash(target);
}

bool MM::Utils::IsPrime(std::uint32_t x) {
  if (x < 2) return false;
  for (std::uint32_t i = 2; i * i <= x; i++) {
    if (x % i == 0) return false;
  }
  return true;
}

bool MM::Utils::IsPrime(std::uint64_t x) {
  if (x < 2) return false;
  for (std::uint64_t i = 2; i * i <= x; i++) {
    if (x % i == 0) return false;
  }
  return true;
}

std::uint32_t MM::Utils::MinPrime(std::uint32_t n) {
  if (IsPrime(n)) return n;
  for (std::uint32_t i = n + 1;; i++) {
    if (IsPrime(i)) return i;
  }
}

std::uint64_t MM::Utils::MinPrime(std::uint64_t n) {
  if (IsPrime(n)) return n;
  for (std::uint64_t i = n + 1;; i++) {
    if (IsPrime(i)) return i;
  }
}

bool MM::Utils::IsPrime32(std::uint32_t x) { return IsPrime(x); }

bool MM::Utils::IsPrime64(std::uint64_t x) { return IsPrime(x); }

std::uint32_t MM::Utils::MinPrime32(std::uint32_t n) { return MinPrime(n); }

std::uint64_t MM::Utils::MinPrime64(std::uint64_t n) { return MinPrime(n); }
