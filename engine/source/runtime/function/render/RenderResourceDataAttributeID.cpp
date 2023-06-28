//
// Created by beimingxianyu on 23-6-28.
//

#include "runtime/function/render/RenderResourceDataAttributeID.h"

MM::RenderSystem::RenderResourceDataAttributeID::RenderResourceDataAttributeID(
    std::uint64_t resource_attribute1, std::uint64_t resource_attribute2)
    : resource_attribute1_(resource_attribute1),
      resource_attribute2_(resource_attribute2) {}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator==(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  return resource_attribute1_ == rhs.resource_attribute1_ &&
         resource_attribute2_ == rhs.resource_attribute2_;
}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator!=(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  return !(rhs == *this);
}

bool MM::RenderSystem::RenderResourceDataAttributeID::operator<(
    const MM::RenderSystem::RenderResourceDataAttributeID& rhs) const {
  if (resource_attribute1_ < rhs.resource_attribute1_) return true;
  if (rhs.resource_attribute1_ < resource_attribute1_) return false;
  return resource_attribute2_ < rhs.resource_attribute2_;
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

void MM::RenderSystem::RenderResourceDataAttributeID::SetResourceAttribute1(
    std::uint64_t new_resource_attribute1) {
  resource_attribute1_ = new_resource_attribute1;
}

void MM::RenderSystem::RenderResourceDataAttributeID::SetResourceAttribute2(
    std::uint64_t new_resource_attribute2) {
  resource_attribute2_ = new_resource_attribute2;
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
}
