#pragma once

#include <atomic>
#include <chrono>
#include <cmath>
#include <ostream>
#include <string>

namespace MM {
namespace Utils {
class UUID {
  friend struct std::hash<UUID>;

 public:
  struct UUIDEmptyInit {};

 public:
  UUID();
  explicit UUID(UUIDEmptyInit);
  ~UUID() = default;
  UUID(const UUID& other) = default;
  UUID(UUID&& other) noexcept = default;
  UUID& operator=(const UUID& other);
  UUID& operator=(UUID&& other) noexcept;

  // for unit test,
  UUID(std::uint64_t clock, std::uint64_t mac_address);

 public:
  friend bool operator==(const UUID& lhs, const UUID& rhs);
  friend bool operator!=(const UUID& lhs, const UUID& rhs);
  friend bool operator<(const UUID& lhs, const UUID& rhs);
  friend bool operator>(const UUID& lhs, const UUID& rhs);
  friend bool operator<=(const UUID& lhs, const UUID& rhs);
  friend bool operator>=(const UUID& lhs, const UUID& rhs);
  friend std::ostream& operator<<(std::ostream& os, const UUID& uuid);
  friend void Swap(UUID& lhs, UUID& rhs) noexcept;
  friend void swap(UUID& lhs, UUID& rhs) noexcept;

 public:
  bool IsValid() const;

  static std::uint32_t GetClockSequence();

  std::string ToString() const;

  std::uint64_t GetHash() const;

  void Reset();

 private:
  std::uint64_t first_part_;
  std::uint64_t second_part_;

 private:
  static std::atomic_uint32_t clock_sequence_;
};

using GUID = UUID;
}  // namespace Utils
}  // namespace MM

namespace std {
template <>
struct hash<MM::Utils::UUID> {
  size_t operator()(const MM::Utils::UUID& uuid) const {
    return uuid.GetHash();
  }
};
}  // namespace std
