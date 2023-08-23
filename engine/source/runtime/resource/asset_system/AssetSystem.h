#pragma once

#include "runtime/resource/asset_system/AssetManager.h"
#include "runtime/resource/asset_system/asset_type/Combination.h"

namespace MM {
namespace AssetSystem {

class AssetSystem {
 public:
  AssetSystem(const AssetSystem& other) = delete;
  AssetSystem(AssetSystem&& other) noexcept = delete;
  AssetSystem& operator=(const AssetSystem& other) = delete;
  AssetSystem& operator=(AssetSystem&& other) = delete;

 public:
  static AssetSystem* GetInstance();

  const AssetManager& GetAssetManager() const;

  AssetManager& GetAssetManager();

 private:
  static bool Destroy();

 protected:
  ~AssetSystem();
  AssetSystem();
  static AssetSystem* asset_system_;

 private:
  AssetManager* assert_manager_{nullptr};
  static std::mutex sync_flag_;
};

#define MM_ASSET_SYSTEM MM_asset_system

#define MM_IMPORT_ASSET_SYSTEM                           \
  inline MM::AssetSystem::AssetSystem* MM_ASSET_SYSTEM { \
    MM::AssetSystem::AssetSystem::GetInstance()          \
  }
}  // namespace AssetSystem
}  // namespace MM
