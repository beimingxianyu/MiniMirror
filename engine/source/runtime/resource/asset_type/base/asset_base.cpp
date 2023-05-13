#include "runtime/resource/asset_type//base/asset_base.h"

MM::AssetType::AssetBase::AssetBase(const std::string& asset_name,
                                    const uint64_t& asset_ID)
    : MMObject(),
      asset_name_(asset_name),
      asset_path_and_last_editing_time_hash(asset_ID) {}

MM::AssetType::AssetBase::AssetBase(AssetBase&& other) noexcept
    : MMObject(std::move(other)),
      asset_name_(std::move(other.asset_name_)),
      asset_path_and_last_editing_time_hash(
          other.asset_path_and_last_editing_time_hash) {}

MM::AssetType::AssetBase& MM::AssetType::AssetBase::operator=(
    const AssetBase& other) {
  if (&other == this) {
    return *this;
  }

  MMObject::operator=(other);
  asset_name_ = other.asset_name_;
  asset_path_and_last_editing_time_hash =
      other.asset_path_and_last_editing_time_hash;

  return *this;
}

MM::AssetType::AssetBase& MM::AssetType::AssetBase::operator=(
    AssetBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  MMObject::operator=(std::move(other));
  asset_name_ = other.asset_name_;
  asset_path_and_last_editing_time_hash =
      other.asset_path_and_last_editing_time_hash;

  other.asset_name_ = std::string{};
  other.asset_path_and_last_editing_time_hash = 0;

  return *this;
}

const std::string& MM::AssetType::AssetBase::GetAssetName() const {
  return asset_name_;
}

MM::AssetType::AssetBase& MM::AssetType::AssetBase::SetAssetName(
    const std::string& new_asset_name) {
  if (new_asset_name.empty()) {
    LOG_ERROR("New asset name can't be empty.");
  }
  asset_name_ = new_asset_name;
  return *this;
}

MM::AssetType::AssetID MM::AssetType::AssetBase::GetAssetID() const {
  return GetGuid();
}

void MM::AssetType::Swap(AssetBase& lhs, AssetBase& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  swap(dynamic_cast<MMObject&>(lhs), dynamic_cast<MMObject&>(rhs));
  swap(lhs.asset_path_and_last_editing_time_hash,
       rhs.asset_path_and_last_editing_time_hash);
  swap(lhs.asset_name_, rhs.asset_name_);
}

void MM::AssetType::swap(AssetBase& lhs, AssetBase& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  swap(dynamic_cast<MMObject&>(lhs), dynamic_cast<MMObject&>(rhs));
  swap(lhs.asset_path_and_last_editing_time_hash,
       rhs.asset_path_and_last_editing_time_hash);
  swap(lhs.asset_name_, rhs.asset_name_);
}

bool MM::AssetType::operator==(const MM::AssetType::AssetBase& lhs,
                               const MM::AssetType::AssetBase& rhs) {
  return rhs.GetAssetID() == lhs.GetAssetID();
}

bool MM::AssetType::operator!=(const MM::AssetType::AssetBase& lhs,
                               const MM::AssetType::AssetBase& rhs) {
  return !(lhs == rhs);
}

bool MM::AssetType::AssetBase::IsValid() const { return MMObject::IsValid(); }

MM::AssetType::AssetType MM::AssetType::AssetBase::GetAssetType() const {
  return AssetType::UNDEFINED;
}

std::string MM::AssetType::AssetBase::GetAssetTypeString() const {
  return std::string(MM_ASSET_TYPE_UNDEFINED);
}
