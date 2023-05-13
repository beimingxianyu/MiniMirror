//
// Created by beimingxianyu on 23-5-12.
//

#include "MMObject.h"

MM::Utils::GUID MM::MMObject::GetGuid() const { return guid_; }

void MM::Swap(MM::MMObject& lhs, MM::MMObject& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  std::swap(lhs.guid_, rhs.guid_);
}

void MM::swap(MM::MMObject& lhs, MM::MMObject& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  std::swap(lhs.guid_, rhs.guid_);
}
MM::MMObject& MM::MMObject::operator=(const MM::MMObject& other) {
  if (&other == this) {
    return *this;
  }

  guid_ = other.guid_;

  return *this;
}
MM::MMObject& MM::MMObject::operator=(MM::MMObject&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  guid_ = other.guid_;

  other.guid_.Reset();

  return *this;
}
bool MM::MMObject::IsValid() const { return guid_.IsValid(); }

bool MM::operator==(const MM::MMObject& lhs, const MM::MMObject& rhs) {
  return lhs.guid_ == rhs.guid_;
}

bool MM::operator!=(const MM::MMObject& lhs, const MM::MMObject& rhs) {
  return !(rhs == lhs);
}

bool MM::operator<(const MM::MMObject& lhs, const MM::MMObject& rhs) {
  return lhs.guid_ < rhs.guid_;
}

bool MM::operator>(const MM::MMObject& lhs, const MM::MMObject& rhs) {
  return rhs < lhs;
}

bool MM::operator<=(const MM::MMObject& lhs, const MM::MMObject& rhs) {
  return !(rhs < lhs);
}

bool MM::operator>=(const MM::MMObject& lhs, const MM::MMObject& rhs) {
  return !(lhs < rhs);
}
