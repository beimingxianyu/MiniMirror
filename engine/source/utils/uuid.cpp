//
// Created by beimingxianyu on 23-5-11.
//

#include "uuid.h"

std::uint32_t MM::Utils::UUID::clock_sequence_{0};

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

bool MM::Utils::UUID::operator==(const MM::Utils::UUID& other) const {
  return first_part_ == other.first_part_ && second_part_ == other.second_part_;
}

bool MM::Utils::UUID::operator!=(const MM::Utils::UUID& other) const {
  return !(other == *this);
}

bool MM::Utils::UUID::operator<(const MM::Utils::UUID& other) const {
  if (first_part_ < other.first_part_) return true;
  if (other.first_part_ < first_part_) return false;
  return second_part_ < other.second_part_;
}

bool MM::Utils::UUID::operator>(const MM::Utils::UUID& other) const {
  return other < *this;
}

bool MM::Utils::UUID::operator<=(const MM::Utils::UUID& other) const {
  return !(other < *this);
}

bool MM::Utils::UUID::operator>=(const MM::Utils::UUID& other) const {
  return !(*this < other);
}

std::ostream& MM::Utils::operator<<(std::ostream& os,
                                    const MM::Utils::UUID& uuid) {
  os << uuid.ToString();
  return os;
}

std::string MM::Utils::UUID::ToString() const {
  std::string uuid_string;
  uuid_string.reserve(36);
}
