//
// Created by beimingxianyu on 23-5-11.
//

#include "uuid.h"

std::atomic_uint32_t MM::Utils::UUID::clock_sequence_{0};

MM::Utils::UUID& MM::Utils::UUID::operator=(const MM::Utils::UUID& other) {
  if (&other == this) {
    return *this;
  }

  first_part_ = other.first_part_;
  second_part_ = other.second_part_;

  return *this;
}

MM::Utils::UUID& MM::Utils::UUID::operator=(MM::Utils::UUID&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  first_part_ = other.first_part_;
  second_part_ = other.second_part_;

  other.first_part_ = 0;
  other.second_part_ = 0;

  return *this;
}

std::ostream& MM::Utils::operator<<(std::ostream& os,
                                    const MM::Utils::UUID& uuid) {
  os << uuid.ToString();
  return os;
}

std::string MM::Utils::UUID::ToString() const {
  std::string uuid_string;
  uuid_string.reserve(36);

  std::uint32_t first_index = 0;
  std::uint32_t second_index = 0;
  for (std::uint32_t i = 0; i != 36; ++i) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      uuid_string.push_back('-');
      continue;
    }

    std::uint8_t char_value = 0;
    if (i < 18) {
      char_value =
          (first_part_ >> ((15 - first_index) * 4)) & 0x000000000000000F;
      ++first_index;
    } else {
      char_value =
          (second_part_ >> ((15 - second_index) * 4)) & 0x000000000000000F;
      ++second_index;
    }

    if (char_value < 10) {
      uuid_string.push_back(static_cast<char>(char_value + 48));
    } else
      uuid_string.push_back(static_cast<char>(char_value + 87));
  }

  return uuid_string;
}

MM::Utils::UUID::UUID() : first_part_(0), second_part_(0) {
  std::uint64_t nanoseconds_since_1582 =
      std::chrono::system_clock::now().time_since_epoch().count() +
      static_cast<std::uint64_t>(12938496000000000000ULL);

  first_part_ |= nanoseconds_since_1582 << 32;
  first_part_ |= (nanoseconds_since_1582 >> 16) & (0x00000000FFFF0000);
  first_part_ |= (nanoseconds_since_1582 >> 48) & (0x0000000000000FFF);
  first_part_ |= 0x0000000000001000;

  std::uint64_t clock_sequence = ++clock_sequence_;
  if (clock_sequence_ == 0xFFFF) {
    clock_sequence_ = 0;
  }
  second_part_ |= MM_HARDWARE_INFO_MAC_ADDRESS_16;
  second_part_ |= clock_sequence << 48;
}

MM::Utils::UUID::UUID(std::uint64_t clock, std::uint64_t mac_address)
    : first_part_(0), second_part_(0) {
  first_part_ |= clock << 32;
  first_part_ |= (clock >> 16) & (0x00000000FFFF0000);
  first_part_ |= (clock >> 48) & (0x0000000000000FFF);
  first_part_ |= 0x0000000000001000;

  std::uint64_t clock_sequence = ++clock_sequence_;
  if (clock_sequence_ == 0xFFFF) {
    clock_sequence_ = 0;
  }
  second_part_ |= mac_address;
  second_part_ |= clock_sequence << 48;
}

std::uint32_t MM::Utils::UUID::GetClockSequence() { return clock_sequence_; }

void MM::Utils::Swap(MM::Utils::UUID& lhs, MM::Utils::UUID& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  std::swap(lhs.first_part_, rhs.first_part_);
  std::swap(lhs.second_part_, rhs.second_part_);
}

void MM::Utils::swap(MM::Utils::UUID& lhs, MM::Utils::UUID& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  std::swap(lhs.first_part_, rhs.first_part_);
  std::swap(lhs.second_part_, rhs.second_part_);
}

bool MM::Utils::UUID::IsValid() const { return first_part_ != 0; }

void MM::Utils::UUID::Reset() {
  first_part_ = 0;
  second_part_ = 0;
}

bool MM::Utils::operator==(const MM::Utils::UUID& lhs,
                           const MM::Utils::UUID& rhs) {
  return lhs.first_part_ == rhs.first_part_ &&
         lhs.second_part_ == rhs.second_part_;
}

bool MM::Utils::operator!=(const MM::Utils::UUID& lhs,
                           const MM::Utils::UUID& rhs) {
  return !(rhs == lhs);
}

bool MM::Utils::operator<(const MM::Utils::UUID& lhs,
                          const MM::Utils::UUID& rhs) {
  if (lhs.first_part_ < rhs.first_part_) return true;
  if (rhs.first_part_ < lhs.first_part_) return false;
  return lhs.second_part_ < rhs.second_part_;
}

bool MM::Utils::operator>(const MM::Utils::UUID& lhs,
                          const MM::Utils::UUID& rhs) {
  return rhs < lhs;
}

bool MM::Utils::operator<=(const MM::Utils::UUID& lhs,
                           const MM::Utils::UUID& rhs) {
  return !(rhs < lhs);
}

bool MM::Utils::operator>=(const MM::Utils::UUID& lhs,
                           const MM::Utils::UUID& rhs) {
  return !(lhs < rhs);
}

MM::Utils::UUID::UUID(MM::Utils::UUID::UUIDEmptyInit)
    : first_part_(0), second_part_(0) {}

std::uint64_t MM::Utils::UUID::GetHash() const {
  return (first_part_ & 0xFFFFFFFFFFFF0000) | (second_part_ >> 48);
}
