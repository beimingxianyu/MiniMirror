#include "runtime/resource/asset_system/AssetSystem.h"

MM::AssetSystem::AssetSystem* MM::AssetSystem::AssetSystem::asset_system_{
    nullptr};
std::mutex MM::AssetSystem::AssetSystem::sync_flag_{};

const MM::AssetSystem::AssetManager&
MM::AssetSystem::AssetSystem::GetAssetManager() const {
  return *assert_manager_;
}

MM::AssetSystem::AssetSystem::~AssetSystem() {
  MM::AssetSystem::AssetSystem::Destroy();
}

bool MM::AssetSystem::AssetSystem::Destroy() {
  std::lock_guard<std::mutex> guard{sync_flag_};
  if (asset_system_) {
    asset_system_->assert_manager_->Destroy();

    delete asset_system_;
    asset_system_ = nullptr;

    return true;
  }

  return true;
}

MM::AssetSystem::AssetSystem* MM::AssetSystem::AssetSystem::GetInstance() {
  if (asset_system_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!asset_system_) {
      asset_system_ = new AssetSystem{};
      asset_system_->assert_manager_ = AssetManager::GetInstance();
    }
  }
  return asset_system_;
}

MM::AssetSystem::AssetSystem::AssetSystem()
    : assert_manager_(AssetManager::GetInstance()) {}

MM::AssetSystem::AssetManager& MM::AssetSystem::AssetSystem::GetAssetManager() {
  return *assert_manager_;
}
