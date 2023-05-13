//
// Created by beimingxianyu on 23-5-12.
//

#include "asset_data.h"
MM::AssetType::AssetDataBase& MM::AssetType::AssetDataBase::operator=(
    MM::AssetType::AssetDataBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  data_size_ = other.data_size_;

  other.data_size_ = 0;

  return *this;
}

std::size_t MM::AssetType::AssetDataBase::GetDataSize() const {
  return data_size_;
}

const void* MM::AssetType::AssetDataBase::GetData() const { return nullptr; }

bool MM::AssetType::AssetDataBase::IsValid() const {
  return data_size_ != 0 && GetData() != nullptr;
}

MM::AssetType::AssetDataBase::AssetDataBase(std::size_t data_size)
    : data_size_(data_size) {}

MM::AssetType::AssetDataID MM::AssetType::AssetDataBase::GetAssetDataID()
    const {
  return GetGuid();
}
