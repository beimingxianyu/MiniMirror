#pragma once

#include <memory>
#include <string>

#include "runtime/core/manager/ManagerBase.h"
#include "runtime/platform/base/MMObject.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "runtime/resource/asset_system/import_other_system.h"
#include "stb_image.h"
#include "utils/utils.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
class AssetBase : public Manager::ManagedObjectBase {
 public:
  AssetBase() = delete;
  virtual ~AssetBase() = default;
  explicit AssetBase(const FileSystem::Path& asset_path);
  AssetBase(const std::string& asset_name, AssetID asset_id)
      : Manager::ManagedObjectBase(asset_name),
        asset_path_and_last_editing_time_hash(asset_id) {}
  AssetBase(const AssetBase& other) = delete;
  AssetBase(AssetBase&& other) noexcept;
  AssetBase& operator=(const AssetBase& other) = delete;
  AssetBase& operator=(AssetBase&& other) noexcept;

  friend bool operator==(const AssetBase& lhs, const AssetBase& rhs);

  friend bool operator!=(const AssetBase& lhs, const AssetBase& rhs);

 public:
  const std::string& GetAssetName() const;

  std::uint64_t GetAssetID() const;

  bool IsValid() const override;

  virtual AssetType GetAssetType() const;

  virtual std::string GetAssetTypeString() const;

  virtual void Release();

  friend void Swap(AssetBase& lhs, AssetBase& rhs) noexcept;

  friend void swap(AssetBase& lhs, AssetBase& rhs) noexcept;

 private:
  AssetID asset_path_and_last_editing_time_hash{};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
