//
// Created by beimingxianyu on 23-6-28.
//

#include "runtime/function/render/RenderResourceDataID.h"

MM::RenderSystem::RenderResourceDataID::RenderResourceDataID(
    MM::AssetSystem::AssetType::AssetID asset_ID,
    const RenderResourceDataAttributeID& resource_attribute)
    : asset_ID_(asset_ID), resource_attribute_ID_(resource_attribute) {}
MM::AssetSystem::AssetType::AssetID

MM::RenderSystem::RenderResourceDataID::GetAssetID() const {
  return asset_ID_;
}

MM::RenderSystem::RenderResourceDataAttributeID
MM::RenderSystem::RenderResourceDataID::GetResourceAttributeID() const {
  return resource_attribute_ID_;
}

void MM::RenderSystem::RenderResourceDataID::SetAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID) {
  asset_ID_ = asset_ID;
}

void MM::RenderSystem::RenderResourceDataID::SetResourceAttributeID(
    const MM::RenderSystem::RenderResourceDataAttributeID&
        render_resource_data_attribute_ID) {
  resource_attribute_ID_ = render_resource_data_attribute_ID;
}

bool MM::RenderSystem::RenderResourceDataID::operator==(
    const MM::RenderSystem::RenderResourceDataID& rhs) const {
  return asset_ID_ == rhs.asset_ID_ &&
         resource_attribute_ID_ == rhs.resource_attribute_ID_;
}

bool MM::RenderSystem::RenderResourceDataID::operator!=(
    const MM::RenderSystem::RenderResourceDataID& rhs) const {
  return !(rhs == *this);
}

bool MM::RenderSystem::RenderResourceDataID::operator<(
    const MM::RenderSystem::RenderResourceDataID& rhs) const {
  if (asset_ID_ < rhs.asset_ID_) return true;
  if (rhs.asset_ID_ < asset_ID_) return false;
  return resource_attribute_ID_ < rhs.resource_attribute_ID_;
}

bool MM::RenderSystem::RenderResourceDataID::operator>(
    const MM::RenderSystem::RenderResourceDataID& rhs) const {
  return rhs < *this;
}

bool MM::RenderSystem::RenderResourceDataID::operator<=(
    const MM::RenderSystem::RenderResourceDataID& rhs) const {
  return !(rhs < *this);
}

bool MM::RenderSystem::RenderResourceDataID::operator>=(
    const MM::RenderSystem::RenderResourceDataID& rhs) const {
  return !(*this < rhs);
}

void MM::RenderSystem::RenderResourceDataID::SetResourceAttributeIDAttribute1(
    std::uint64_t render_resource_data_attribute_ID_attribute1) {
  resource_attribute_ID_.SetSubID1(
      render_resource_data_attribute_ID_attribute1);
}

void MM::RenderSystem::RenderResourceDataID::SetResourceAttributeIDAttribute2(
    std::uint64_t render_resource_data_attribute_ID_attribute2) {
  resource_attribute_ID_.SetSubID2(
      render_resource_data_attribute_ID_attribute2);
}

void MM::RenderSystem::swap(
    MM::RenderSystem::RenderResourceDataID& lhs,
    MM::RenderSystem::RenderResourceDataID& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.asset_ID_, rhs.asset_ID_);
  swap(lhs.resource_attribute_ID_, rhs.resource_attribute_ID_);
}

void MM::RenderSystem::Swap(
    MM::RenderSystem::RenderResourceDataID& lhs,
    MM::RenderSystem::RenderResourceDataID& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.asset_ID_, rhs.asset_ID_);
  swap(lhs.resource_attribute_ID_, rhs.resource_attribute_ID_);
}
