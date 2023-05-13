#pragma once

#include <stb_image.h>

#include <memory>
#include <string>

#include "runtime/platform/base/MMObject.h"
#include "runtime/resource/asset_type/base/asset_data.h"
#include "runtime/resource/asset_type/base/import_other_system.h"
#include "utils/utils.h"

namespace MM {
namespace AssetType {
class AssetBase : virtual public MMObject {
 public:
  AssetBase() = delete;
  virtual ~AssetBase() = default;
  AssetBase(const std::string& asset_name, const uint64_t& asset_ID);
  AssetBase(const AssetBase& other) = default;
  AssetBase(AssetBase&& other) noexcept;
  AssetBase& operator=(const AssetBase& other);
  AssetBase& operator=(AssetBase&& other) noexcept;

  friend bool operator==(const AssetBase& lhs, const AssetBase& rhs);

  friend bool operator!=(const AssetBase& lhs, const AssetBase& rhs);

 public:
  const std::string& GetAssetName() const;

  AssetBase& SetAssetName(const std::string& new_asset_name);

  AssetID GetAssetID() const;

  bool IsValid() const override;

  virtual AssetType GetAssetType() const;

  virtual std::string GetAssetTypeString() const;

  virtual const void* GetData() const = 0;

  virtual void Release() = 0;

  friend void Swap(AssetBase& lhs, AssetBase& rhs) noexcept;

  friend void swap(AssetBase& lhs, AssetBase& rhs) noexcept;

 private:
  std::string asset_name_{};
  std::uint64_t asset_path_and_last_editing_time_hash{};
};
}  // namespace AssetType
}  // namespace MM
