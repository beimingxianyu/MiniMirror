//
// Created by beimingxianyu on 23-5-12.
//

#include "MMObject.h"

MM::Utils::GUID MM::MMObject::GetGuid() const { return guid_; }

MM::MMObject::MMObject(MM::MMObject&& other) noexcept
    : guid_(std::move(other.guid_)) {}

void MM::MMObject::Swap(MM::MMObject& lhs, MM::MMObject& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  std::swap(lhs.guid_, rhs.guid_);
}

void MM::MMObject::swap(MM::MMObject& lhs, MM::MMObject& rhs) noexcept {
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

  guid_ = std::move(other.guid_);

  return *this;
}
bool MM::MMObject::IsValid() const { return guid_.IsValid(); }

bool MM::MMObject::operator==(const MM::MMObject& rhs) const {
  return guid_ == rhs.guid_;
}

bool MM::MMObject::operator!=(const MM::MMObject& rhs) const {
  return !(*this == rhs);
}

bool MM::MMObject::operator<(const MM::MMObject& rhs) const {
  return guid_ < rhs.guid_;
}

bool MM::MMObject::operator>(const MM::MMObject& rhs) const {
  return rhs < *this;
}

bool MM::MMObject::operator<=(const MM::MMObject& rhs) const {
  return !(rhs < *this);
}

bool MM::MMObject::operator>=(const MM::MMObject& rhs) const {
  return !(*this < rhs);
}

void MM::MMObject::Reset() { guid_.Reset(); }
