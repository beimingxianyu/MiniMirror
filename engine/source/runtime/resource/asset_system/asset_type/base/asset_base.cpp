#include "asset_base.h"

MM::AssetSystem::AssetType::AssetBase::AssetBase(
    const FileSystem::Path& asset_path)
    : Manager::ManagedObjectBase(asset_path.GetFileName()),
      asset_path_(asset_path),
      asset_path_and_last_editing_time_hash(0) {
  if (asset_path.IsDirectory()) {
    MM_LOG_ERROR(asset_path.String() + " is not a file.");
    asset_path_ = FileSystem::Path("");
    return;
  }

  Result<FileSystem::LastWriteTime, ErrorResult> last_write_time = MM_FILE_SYSTEM->GetLastWriteTime(asset_path).Exception(
          MM_ERROR_DESCRIPTION2(asset_path.String() + "is not exisits")
          );
 if (!last_write_time.Success()) {
     asset_path_ = FileSystem::Path("");
     return;
 }

  asset_path_and_last_editing_time_hash =
      asset_path.GetHash() ^
      static_cast<std::uint64_t>(last_write_time.GetResult().time_since_epoch().count());
  assert(asset_path_and_last_editing_time_hash != 0);
}

MM::AssetSystem::AssetType::AssetBase::AssetBase(AssetBase&& other) noexcept
    : Manager::ManagedObjectBase(std::move(other)),
      asset_path_(std::move(other.asset_path_)),
      asset_path_and_last_editing_time_hash(
          other.asset_path_and_last_editing_time_hash) {}

MM::AssetSystem::AssetType::AssetBase&
MM::AssetSystem::AssetType::AssetBase::operator=(AssetBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  Manager::ManagedObjectBase::operator=(std::move(other));
  asset_path_ = std::move(other.asset_path_);
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
  return (std::strcmp(asset_path_.StringView().data(), "") != 0) &&
         asset_path_and_last_editing_time_hash != 0 &&
         Manager::ManagedObjectBase::IsValid();
}

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::AssetBase::GetAssetType() const {
  return AssetType::UNDEFINED;
}

std::string MM::AssetSystem::AssetType::AssetBase::GetAssetTypeString() const {
  return std::string(MM_ASSET_TYPE_UNDEFINED);
}

void MM::AssetSystem::AssetType::AssetBase::Release() {
  asset_path_and_last_editing_time_hash = 0;
}

MM::AssetSystem::AssetType::AssetBase::AssetBase(
    const MM::FileSystem::Path& asset_path,
    MM::AssetSystem::AssetType::AssetID asset_id)
    : Manager::ManagedObjectBase(asset_path.GetFileName()),
      asset_path_(asset_path),
      asset_path_and_last_editing_time_hash(asset_id) {}

const MM::FileSystem::Path&
MM::AssetSystem::AssetType::AssetBase::GetAssetPath() const {
  return asset_path_;
}

MM::Result<MM::Utils::Json::Document, MM::ErrorResult> MM::AssetSystem::AssetType::AssetBase::GetJson() const {
  MM_LOG_FATAL("This function should not be called.");
  return Result<MM::Utils::Json::Document, MM::ErrorResult>{st_execute_error, MM::ErrorCode::UNDEFINED_ERROR};
}

void MM::AssetSystem::AssetType::AssetBase::SetAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID) {
  asset_path_and_last_editing_time_hash = asset_ID;
}

std::vector<std::pair<void*, std::uint64_t>>
MM::AssetSystem::AssetType::AssetBase::GetDatas() {
  MM_LOG_FATAL("This function should not be called.");
  return std::vector<std::pair<void*, std::uint64_t>>();
}

std::vector<std::pair<const void*, std::uint64_t>>
MM::AssetSystem::AssetType::AssetBase::GetDatas() const {
  MM_LOG_FATAL("This function should not be called.");
  return std::vector<std::pair<const void*, std::uint64_t>>();
}

std::uint64_t MM::AssetSystem::AssetType::AssetBase::GetSize() const {
  MM_LOG_FATAL("This function should not be called.");
  return 0;
}
