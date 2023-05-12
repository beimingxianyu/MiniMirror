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
  UUID();
  ~UUID() = default;
  UUID(const UUID& other) = default;
  UUID(UUID&& other) noexcept = default;
  UUID& operator=(const UUID& other);
  UUID& operator=(UUID&& other) noexcept;

  // for unit test,
  UUID(std::uint64_t clock, std::uint64_t mac_address);

 public:
  bool operator==(const UUID& other) const;
  bool operator!=(const UUID& other) const;
  bool operator<(const UUID& other) const;
  bool operator>(const UUID& other) const;
  bool operator<=(const UUID& other) const;
  bool operator>=(const UUID& other) const;
  friend std::ostream& operator<<(std::ostream& os, const UUID& uuid);

 public:
  static std::uint32_t GetClockSequence();

  std::string ToString() const;

 private:
  std::uint64_t first_part_;
  std::uint64_t second_part_;

 private:
  static std::atomic_uint32_t clock_sequence_;
};
}  // namespace Utils
}  // namespace MM

namespace std {
template <>
struct hash<MM::Utils::UUID> {
  size_t operator()(const MM::Utils::UUID& uuid) const {
    return std::hash<std::uint64_t>{}(uuid.first_part_ ^ uuid.second_part_);
    ;
  }
};
}  // namespace std