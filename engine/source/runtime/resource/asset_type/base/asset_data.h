#pragma once

#include <cstddef>
#include <vector>

#include "runtime/platform/base/MMObject.h"
#include "runtime/platform/base/error.h"
#include "runtime/resource/asset_type/base/asset_type_define.h"

namespace MM {
namespace AssetType {
class AssetDataBase : virtual public MMObject {
 public:
  AssetDataBase() = delete;
  virtual ~AssetDataBase() = default;
  explicit AssetDataBase(std::size_t data_size);
  AssetDataBase(const AssetDataBase& other) = delete;
  AssetDataBase(AssetDataBase&& other) noexcept = default;
  AssetDataBase& operator=(const AssetDataBase& other) = delete;
  AssetDataBase& operator=(AssetDataBase&& other) noexcept;

 public:
  bool IsValid() const;

  std::size_t GetDataSize() const;

  AssetDataID GetAssetDataID() const;

  virtual const void* GetData() const;

 private:
  std::size_t data_size_{0};
};

template <typename ObjectType>
class AssetData : public AssetDataBase {
 public:
  virtual ~AssetData() = default;
  explicit AssetData(std::vector<ObjectType>&& asset_data);
  AssetData(const AssetData& other) = delete;
  AssetData(AssetData&& other) noexcept = default;
  AssetData<ObjectType>& operator=(const AssetData& other) = delete;
  AssetData<ObjectType>& operator=(AssetData&& other) noexcept;

 public:
  const void* GetData() const;

 private:
  std::vector<ObjectType> asset_data_{};
};

template <typename ObjectType>
const void* AssetData<ObjectType>::GetData() const {
  return asset_data_.data();
}

template <typename ObjectType>
AssetData<ObjectType>& AssetData<ObjectType>::operator=(
    AssetData&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  AssetDataBase::operator=(std::move(other));
  asset_data_ = std::move(other.asset_data_);

  return *this;
}

template <typename ObjectType>
AssetData<ObjectType>::AssetData(std::vector<ObjectType>&& asset_data)
    : AssetDataBase(sizeof(ObjectType) * asset_data.size()),
      asset_data_(std::move(asset_data)) {}
}  // namespace AssetType
}  // namespace MM
