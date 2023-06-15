#pragma once

#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <thread>

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

class SpinSharedMutex {
 public:
  SpinSharedMutex() = default;
  ~SpinSharedMutex() = default;
  SpinSharedMutex(const SpinSharedMutex& other) = delete;
  SpinSharedMutex(SpinSharedMutex&& other) = delete;
  SpinSharedMutex& operator=(const SpinSharedMutex& other) = delete;
  SpinSharedMutex& operator=(SpinSharedMutex&& other) = delete;

 public:
  void Lock();

  void Unlock();

  void SharedLock();

  void SharedUnlock();

 private:
  std::atomic_bool is_write_{false};
  std::atomic_uint32_t read_count_{0};
};

class SpinSharedLock {
 public:
  SpinSharedLock() = delete;
  ~SpinSharedLock();
  explicit SpinSharedLock(SpinSharedMutex& spin_shared_mutex);
  SpinSharedLock(const SpinSharedLock& other) = delete;
  SpinSharedLock(SpinSharedLock&& other) = delete;
  SpinSharedLock& operator=(const SpinSharedLock& other) = delete;
  SpinSharedLock& operator=(SpinSharedLock&& other) = delete;

 public:
  void Lock();

  void Unlock();

 private:
  SpinSharedMutex& spin_shared_mutex_;
};

class SpinUniqueLock {
 public:
  SpinUniqueLock() = delete;
  ~SpinUniqueLock();
  explicit SpinUniqueLock(SpinSharedMutex& spin_shared_mutex);
  SpinUniqueLock(const SpinUniqueLock& other) = delete;
  SpinUniqueLock(SpinUniqueLock&& other) = delete;
  SpinUniqueLock& operator=(const SpinUniqueLock& other) = delete;
  SpinUniqueLock& operator=(SpinUniqueLock& other) = delete;

 public:
  void Lock();

  void Unlock();

 private:
  SpinSharedMutex& spin_shared_mutex_;
};

template <typename T>
std::uint64_t Hash(T&& value) {
  std::hash<typename std::decay<T>::type> hash;
  return hash(std::forward<T>(value));
}
}  // namespace Utils
}  // namespace MM
