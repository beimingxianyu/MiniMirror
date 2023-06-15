#include "asset_base.h"

MM::AssetSystem::AssetType::AssetBase::AssetBase(
    const FileSystem::Path& assert_path)
    : Manager::ManagedObjectBase(assert_path.GetFileName()),
      asset_path_and_last_editing_time_hash(0) {
  FileSystem::LastWriteTime last_write_time;
  MM_CHECK(FILE_SYSTEM->GetLastWriteTime(assert_path, last_write_time),
           LOG_ERROR(assert_path.String() + "is not exists.");
           return;);
  asset_path_and_last_editing_time_hash =
      std::hash<std::string>{}(
          assert_path.GetRelativePath(CONFIG_SYSTEM->GetConfig("bin_dir"))) ^
      static_cast<std::uint32_t>(last_write_time.time_since_epoch().count());
}

MM::AssetSystem::AssetType::AssetBase::AssetBase(AssetBase&& other) noexcept
    : Manager::ManagedObjectBase(std::move(other)),
      asset_path_and_last_editing_time_hash(
          other.asset_path_and_last_editing_time_hash) {}

MM::AssetSystem::AssetType::AssetBase&
MM::AssetSystem::AssetType::AssetBase::operator=(AssetBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  Manager::ManagedObjectBase::operator=(std::move(other));
  asset_path_and_last_editing_time_hash =
      other.asset_path_and_last_editing_time_hash;

  other.asset_path_and_last_editing_time_hash = 0;

  return *this;
}

const std::string& MM::AssetSystem::AssetType::AssetBase::GetAssetName() const {
  return GetObjectName();
}

std::uint64_t MM::AssetSystem::AssetType::AssetBase::GetAssetID() const {
  return asset_path_and_last_editing_time_hash;
}

void MM::AssetSystem::AssetType::Swap(AssetBase& lhs, AssetBase& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  swap(dynamic_cast<Manager::ManagedObjectBase&>(lhs),
       dynamic_cast<Manager::ManagedObjectBase&>(rhs));
  swap(lhs.asset_path_and_last_editing_time_hash,
       rhs.asset_path_and_last_editing_time_hash);
}

void MM::AssetSystem::AssetType::swap(AssetBase& lhs, AssetBase& rhs) noexcept {
  using std::swap;
  if (&lhs == &rhs) {
    return;
  }

  swap(dynamic_cast<Manager::ManagedObjectBase&>(lhs),
       dynamic_cast<Manager::ManagedObjectBase&>(rhs));
  swap(lhs.asset_path_and_last_editing_time_hash,
       rhs.asset_path_and_last_editing_time_hash);
}

bool MM::AssetSystem::AssetType::operator==(
    const MM::AssetSystem::AssetType::AssetBase& lhs,
    const MM::AssetSystem::AssetType::AssetBase& rhs) {
  return rhs.GetAssetID() == lhs.GetAssetID();
}

bool MM::AssetSystem::AssetType::operator!=(
    const MM::AssetSystem::AssetType::AssetBase& lhs,
    const MM::AssetSystem::AssetType::AssetBase& rhs) {
  return !(lhs == rhs);
}

bool MM::AssetSystem::AssetType::AssetBase::IsValid() const {
  return asset_path_and_last_editing_time_hash != 0 &&
         Manager::ManagedObjectBase::IsValid();
}

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::AssetBase::GetAssetType() const {
  return AssetType::UNDEFINED;
}

std::string MM::AssetSystem::AssetType::AssetBase::GetAssetTypeString() const {
  return std::string(MM_ASSET_TYPE_UNDEFINED);
}

void MM::AssetSystem::AssetType::AssetBase::Release() {}
