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

void MM::Utils::SpinSharedMutex::Lock() {
  while (is_write_.load(std::memory_order_acquire))
    ;
  is_write_.store(true, std::memory_order_release);
  while (read_count_.load(std::memory_order_acquire) != 0)
    ;
}

void MM::Utils::SpinSharedMutex::Unlock() {
  is_write_.store(false, std::memory_order_release);
}

void MM::Utils::SpinSharedMutex::SharedLock() {
  while (is_write_.load(std::memory_order_acquire))
    ;
  read_count_.fetch_add(1, std::memory_order_acq_rel);
  while (is_write_.load(std::memory_order_acquire)) {
    read_count_.fetch_sub(1, std::memory_order_acq_rel);
    while (is_write_.load(std::memory_order_acquire))
      ;
    read_count_.fetch_add(1, std::memory_order_acq_rel);
  }
}

void MM::Utils::SpinSharedMutex::SharedUnlock() {
  read_count_.fetch_sub(1, std::memory_order_acq_rel);
}

MM::Utils::SpinSharedLock::~SpinSharedLock() {
  spin_shared_mutex_.SharedUnlock();
}

MM::Utils::SpinSharedLock::SpinSharedLock(
    MM::Utils::SpinSharedMutex& spin_shared_mutex)
    : spin_shared_mutex_(spin_shared_mutex) {
  spin_shared_mutex_.SharedLock();
}

void MM::Utils::SpinSharedLock::Lock() { spin_shared_mutex_.SharedLock(); }

void MM::Utils::SpinSharedLock::Unlock() { spin_shared_mutex_.SharedUnlock(); }

MM::Utils::SpinUniqueLock::~SpinUniqueLock() { spin_shared_mutex_.Unlock(); }

MM::Utils::SpinUniqueLock::SpinUniqueLock(
    MM::Utils::SpinSharedMutex& spin_shared_mutex)
    : spin_shared_mutex_(spin_shared_mutex) {
  spin_shared_mutex_.Lock();
}

void MM::Utils::SpinUniqueLock::Lock() { spin_shared_mutex_.Lock(); }
void MM::Utils::SpinUniqueLock::Unlock() { spin_shared_mutex_.Unlock(); }
