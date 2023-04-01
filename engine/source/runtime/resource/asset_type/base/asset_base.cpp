#include "runtime/resource/asset_type//base/asset_base.h"

MM::AssetType::AssetBase::AssetBase(const std::string& asset_name,
                                       const uint64_t& asset_ID)
    : asset_name_(asset_name),
      asset_ID_(asset_ID) {}

MM::AssetType::AssetBase::AssetBase(AssetBase&& other) noexcept : asset_name_(std::move(other.asset_name_)), asset_ID_(other.asset_ID_){}

MM::AssetType::AssetBase& MM::AssetType::AssetBase::operator=(
    const AssetBase& other) {
  if (&other == this) {
    return *this;
  }
  asset_name_ = other.asset_name_;
  asset_ID_ = other.asset_ID_;
  return *this;
}

MM::AssetType::AssetBase& MM::AssetType::AssetBase::operator=(
    AssetBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  asset_name_ = other.asset_name_;
  asset_ID_ = other.asset_ID_;

  other.asset_name_ = std::string{};
  other.asset_ID_ = 0;

  return *this;
}

const std::string& MM::AssetType::AssetBase::GetAssetName() const {
  return asset_name_;
}

MM::AssetType::AssetBase& MM::AssetType::AssetBase::SetAssetName(
    const std::string& new_asset_name) {
  asset_name_ = new_asset_name;
  return *this;
}

const uint32_t& MM::AssetType::AssetBase::GetAssetID() const {
  return asset_ID_;
}

void MM::AssetType::Swap(AssetBase& lhs, AssetBase& rhs) noexcept {
  using std::swap;
  swap(lhs.asset_name_, rhs.asset_name_);
  swap(lhs.asset_ID_, rhs.asset_ID_);
}

void MM::AssetType::swap(AssetBase& lhs, AssetBase& rhs) noexcept {
  using std::swap;
  swap(lhs.asset_name_, rhs.asset_name_);
  swap(lhs.asset_ID_, rhs.asset_ID_);
}
