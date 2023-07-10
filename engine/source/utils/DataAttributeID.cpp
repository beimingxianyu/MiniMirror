//
// Created by beimingxianyu on 23-6-30.
//

#include "utils/DataAttributeID.h"

MM::Utils::DataAttributeID2::DataAttributeID2(std::uint64_t resource_attribute1,
                                              std::uint64_t resource_attribute2)
    : resource_attribute1_(resource_attribute1),
      resource_attribute2_(resource_attribute2) {}

bool MM::Utils::DataAttributeID2::operator==(
    const MM::Utils::DataAttributeID2& rhs) const {
  return resource_attribute1_ == rhs.resource_attribute1_ &&
         resource_attribute2_ == rhs.resource_attribute2_;
}

bool MM::Utils::DataAttributeID2::operator!=(
    const MM::Utils::DataAttributeID2& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::DataAttributeID2::operator<(
    const MM::Utils::DataAttributeID2& rhs) const {
  if (resource_attribute1_ < rhs.resource_attribute1_) return true;
  if (rhs.resource_attribute1_ < resource_attribute1_) return false;
  return resource_attribute2_ < rhs.resource_attribute2_;
}

bool MM::Utils::DataAttributeID2::operator>(
    const MM::Utils::DataAttributeID2& rhs) const {
  return rhs < *this;
}

bool MM::Utils::DataAttributeID2::operator<=(
    const MM::Utils::DataAttributeID2& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::DataAttributeID2::operator>=(
    const MM::Utils::DataAttributeID2& rhs) const {
  return !(*this < rhs);
}

void MM::Utils::Swap(MM::Utils::DataAttributeID2& lhs,
                     MM::Utils::DataAttributeID2& rhs) noexcept {
  using std::swap;

  std::swap(lhs.resource_attribute1_, rhs.resource_attribute1_);
  std::swap(lhs.resource_attribute2_, rhs.resource_attribute2_);
}

void MM::Utils::swap(MM::Utils::DataAttributeID2& lhs,
                     MM::Utils::DataAttributeID2& rhs) noexcept {
  using std::swap;

  std::swap(lhs.resource_attribute1_, rhs.resource_attribute1_);
  std::swap(lhs.resource_attribute2_, rhs.resource_attribute2_);
}

std::uint64_t MM::Utils::DataAttributeID2::GetResourceAttribute1() const {
  return resource_attribute1_;
}

std::uint64_t MM::Utils::DataAttributeID2::GetResourceAttribute2() const {
  return resource_attribute2_;
}

void MM::Utils::DataAttributeID2::SetAttribute1(
    std::uint64_t new_resource_attribute1) {
  resource_attribute1_ = new_resource_attribute1;
}

void MM::Utils::DataAttributeID2::SetAttribute2(
    std::uint64_t new_resource_attribute2) {
  resource_attribute2_ = new_resource_attribute2;
}

std::uint64_t MM::Utils::DataAttributeID2::GetHashCode() const {
  return resource_attribute1_ ^ resource_attribute2_;
}

void MM::Utils::DataAttributeID2::Reset() {
  resource_attribute1_ = 0;
  resource_attribute2_ = 0;
}

MM::Utils::DataAttributeID3::DataAttributeID3(std::uint64_t resource_attribute1,
                                              std::uint64_t resource_attribute2,
                                              std::uint64_t resource_attribute3)
    : resource_attribute1_(resource_attribute1),
      resource_attribute2_(resource_attribute2),
      resoutce_attribute3_(resource_attribute3) {}

bool MM::Utils::DataAttributeID3::operator==(
    const MM::Utils::DataAttributeID3& rhs) const {
  return resource_attribute1_ == rhs.resource_attribute1_ &&
         resource_attribute2_ == rhs.resource_attribute2_ &&
         resoutce_attribute3_ == rhs.resoutce_attribute3_;
}

bool MM::Utils::DataAttributeID3::operator!=(
    const MM::Utils::DataAttributeID3& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::DataAttributeID3::operator<(
    const MM::Utils::DataAttributeID3& rhs) const {
  if (resource_attribute1_ < rhs.resource_attribute1_) return true;
  if (rhs.resource_attribute1_ < resource_attribute1_) return false;
  if (resource_attribute2_ < rhs.resource_attribute2_) return true;
  if (rhs.resource_attribute2_ < resource_attribute2_) return false;
  return resoutce_attribute3_ < rhs.resoutce_attribute3_;
}

bool MM::Utils::DataAttributeID3::operator>(
    const MM::Utils::DataAttributeID3& rhs) const {
  return rhs < *this;
}

bool MM::Utils::DataAttributeID3::operator<=(
    const MM::Utils::DataAttributeID3& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::DataAttributeID3::operator>=(
    const MM::Utils::DataAttributeID3& rhs) const {
  return !(*this < rhs);
}

std::uint64_t MM::Utils::DataAttributeID3::GetResourceAttribute1() const {
  return resource_attribute1_;
}

std::uint64_t MM::Utils::DataAttributeID3::GetResourceAttribute2() const {
  return resource_attribute2_;
}

std::uint64_t MM::Utils::DataAttributeID3::GetResourceAttribute3() const {
  return resoutce_attribute3_;
}

void MM::Utils::DataAttributeID3::SetAttribute1(
    std::uint64_t new_resource_attribute1) {
  resource_attribute1_ = new_resource_attribute1;
}

void MM::Utils::DataAttributeID3::SetAttribute2(
    std::uint64_t new_resource_attribute2) {
  resource_attribute2_ = new_resource_attribute2;
}

void MM::Utils::DataAttributeID3::SetAttribute3(
    std::uint64_t new_resource_attribute3) {
  resoutce_attribute3_ = new_resource_attribute3;
}

void MM::Utils::swap(MM::Utils::DataAttributeID3& lhs,
                     MM::Utils::DataAttributeID3& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.resource_attribute1_, rhs.resource_attribute1_);
  swap(lhs.resource_attribute2_, rhs.resource_attribute2_);
  swap(lhs.resoutce_attribute3_, rhs.resoutce_attribute3_);
}

void MM::Utils::Swap(MM::Utils::DataAttributeID3& lhs,
                     MM::Utils::DataAttributeID3& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.resource_attribute1_, rhs.resource_attribute1_);
  swap(lhs.resource_attribute2_, rhs.resource_attribute2_);
  swap(lhs.resoutce_attribute3_, rhs.resoutce_attribute3_);
}

std::uint64_t MM::Utils::DataAttributeID3::GetHashCode() const {
  return resource_attribute1_ ^ resource_attribute2_ ^ resoutce_attribute3_;
}

void MM::Utils::DataAttributeID3::Reset() {
  resource_attribute1_ = 0;
  resource_attribute2_ = 0;
  resoutce_attribute3_ = 0;
}
