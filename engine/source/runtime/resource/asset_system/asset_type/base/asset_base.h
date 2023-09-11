#pragma once

#include <memory>
#include <string>

#include "runtime/core/manager/ManagerBase.h"
#include "runtime/platform/base/MMObject.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "runtime/resource/asset_system/import_other_system.h"
#include "utils/Json.h"
#include "utils/utils.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
class AssetBase : public Manager::ManagedObjectBase {
 public:
  AssetBase() = default;
  virtual ~AssetBase() = default;
  explicit AssetBase(const FileSystem::Path& asset_path);
  AssetBase(const FileSystem::Path& asset_path, AssetID asset_id);
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

  const FileSystem::Path& GetAssetPath() const;

  virtual std::uint64_t GetSize() const;

  virtual Result<MM::Utils::Json::Document, MM::ErrorResult> GetJson() const;

  virtual std::vector<std::pair<void*, std::uint64_t>> GetDatas();

  virtual std::vector<std::pair<const void*, std::uint64_t>> GetDatas() const;

  virtual void Release();

  friend void Swap(AssetBase& lhs, AssetBase& rhs) noexcept;

  friend void swap(AssetBase& lhs, AssetBase& rhs) noexcept;

 protected:
  void SetAssetID(AssetID asset_ID);

 private:
  FileSystem::Path asset_path_{""};
  AssetID asset_path_and_last_editing_time_hash{0};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
