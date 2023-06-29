//
// Created by beimingxianyu on 23-6-28.
//

#include "runtime/function/render/RenderResourceDataAttributeID.h"

#include <optional>

MM::RenderSystem::RenderResourceDataAttributeID::RenderResourceDataAttributeID(
    std::uint64_t resource_attribute1, std::uint64_t resource_attribute2,
    std::uint64_t resource_attribute3)
    : resource_attribute1_(resource_attribute1),
      resource_attribute2_(resource_attribute2),
      resoutce_attribute3_(resource_attribute3) {}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator==(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  return resource_attribute1_ == rhs.resource_attribute1_ &&
         resource_attribute2_ == rhs.resource_attribute2_ &&
         resoutce_attribute3_ == rhs.resoutce_attribute3_;
}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator!=(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  return !(rhs == *this);
}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator<(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  if (resource_attribute1_ < rhs.resource_attribute1_) return true;
  if (rhs.resource_attribute1_ < resource_attribute1_) return false;
  if (resource_attribute2_ < rhs.resource_attribute2_) return true;
  if (rhs.resource_attribute2_ < resource_attribute2_) return false;
  return resoutce_attribute3_ < rhs.resoutce_attribute3_;
}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator>(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  return rhs < *this;
}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator<=(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  return !(rhs < *this);
}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator>=(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  return !(*this < rhs);
}

std::uint64_t
MM::RenderSystem::RenderResourceDataAttributeID::GetResourceAttribute1() const {
  return resource_attribute1_;
}

std::uint64_t
MM::RenderSystem::RenderResourceDataAttributeID::GetResourceAttribute2() const {
  return resource_attribute2_;
}

std::uint64_t
MM::RenderSystem::RenderResourceDataAttributeID::GetResourceAttribute3() const {
  return resoutce_attribute3_;
}

void MM::RenderSystem::RenderResourceDataAttributeID::SetResourceAttribute1(
    std::uint64_t new_resource_attribute1) {
  resource_attribute1_ = new_resource_attribute1;
}

void MM::RenderSystem::RenderResourceDataAttributeID::SetResourceAttribute2(
    std::uint64_t new_resource_attribute2) {
  resource_attribute2_ = new_resource_attribute2;
}

void MM::RenderSystem::RenderResourceDataAttributeID::SetResourceAttribute3(
    std::uint64_t new_resource_attribute3) {
  resoutce_attribute3_ = new_resource_attribute3;
}

void MM::RenderSystem::swap(
    MM::RenderSystem::RenderResourceDataAttributeID& lhs,
    MM::RenderSystem::RenderResourceDataAttributeID& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.resource_attribute1_, rhs.resource_attribute1_);
  swap(lhs.resource_attribute2_, rhs.resource_attribute2_);
  swap(lhs.resoutce_attribute3_, rhs.resoutce_attribute3_);
}

void MM::RenderSystem::Swap(
    MM::RenderSystem::RenderResourceDataAttributeID& lhs,
    MM::RenderSystem::RenderResourceDataAttributeID& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.resource_attribute1_, rhs.resource_attribute1_);
  swap(lhs.resource_attribute2_, rhs.resource_attribute2_);
  swap(lhs.resoutce_attribute3_, rhs.resoutce_attribute3_);
}

std::uint64_t MM::RenderSystem::RenderResourceDataAttributeID::GetHashCode()
    const {
  return resource_attribute1_ ^ resource_attribute2_ ^ resoutce_attribute3_;
}
