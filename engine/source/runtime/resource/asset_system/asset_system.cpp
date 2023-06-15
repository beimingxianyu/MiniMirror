#include "runtime/resource/asset_system/asset_system.h"

MM::AssetSystem::AssetManager* MM::AssetSystem::AssetManager::asset_manager_{
    nullptr};
std::mutex MM::AssetSystem::AssetManager::sync_flag_{};

MM::AssetSystem::AssetSystem* MM::AssetSystem::AssetSystem::asset_system_{
    nullptr};
std::mutex MM::AssetSystem::AssetSystem::sync_flag_{};

bool MM::AssetSystem::AssetManager::LoadImage(
    const std::string& asset_name, const FileSystem::Path& image_path,
    const int& desired_channels) {
  std::shared_ptr<AssetType::AssetBase> image =
      std::make_shared<AssetType::Image>(asset_name, image_path,
                                         desired_channels);
  if (!(image->IsValid())) {
    return false;
  }
  {
    std::unique_lock<std::shared_mutex> guard(writer_mutex_);
    asset_name_to_asset_ID_.emplace(asset_name, image->GetAssetID());
    asset_ID_to_asset_.emplace(image->GetAssetID(), image);
  }
  return true;
}

std::vector<std::shared_ptr<MM::AssetType::AssetBase>>
MM::AssetSystem::AssetManager::GetAssetsByName(
    const std::string& asset_name) const {
  std::shared_lock<std::shared_mutex> guard(writer_mutex_);
  const std::size_t count = asset_name_to_asset_ID_.count(asset_name);
  if (count == 0) {
    return std::vector<std::shared_ptr<MM::AssetType::AssetBase>>{};
  }
  std::vector<std::shared_ptr<MM::AssetType::AssetBase>> result(count);
  const auto equal_ID = asset_name_to_asset_ID_.equal_range(asset_name);
  auto beg = equal_ID.first;
  for (std::size_t i = 0; i < count; ++i) {
    result[i] = asset_ID_to_asset_.at(beg->second);
    ++beg;
  }
  return result;
}

std::shared_ptr<MM::AssetType::AssetBase>
MM::AssetSystem::AssetManager::GetAssetByID(
    const std::uint64_t& asset_ID) const {
  std::shared_lock<std::shared_mutex> guard(writer_mutex_);
  if (asset_ID_to_asset_.count(asset_ID)) {
    return asset_ID_to_asset_.at(asset_ID);
  }
  return nullptr;
}

bool MM::AssetSystem::AssetManager::Erase(const uint64_t& asset_ID) {
  std::unique_lock<std::shared_mutex> guard{writer_mutex_};
  if (asset_ID_to_asset_.count(asset_ID)) {
    const auto asset = asset_ID_to_asset_[asset_ID];
    const auto equal_element =
        asset_name_to_asset_ID_.equal_range(asset->GetAssetName());
    auto beg = equal_element.first;
    while (beg != equal_element.second) {
      if (beg->second == asset_ID) {
        asset_name_to_asset_ID_.erase(beg);
        break;
      }
      ++beg;
    }

    assert(beg == equal_element.second);  // Logic error, it is expected to find
                                          // the desired element.
    asset_ID_to_asset_.erase(asset_ID);
    return true;
  }
  return false;
}

bool MM::AssetSystem::AssetManager::ChangeAssetName(
    const std::uint64_t& asset_ID, const std::string& new_asset_name) {
  if (asset_ID_to_asset_.count(asset_ID)) {
    std::unique_lock<std::shared_mutex> guard{writer_mutex_};
    const auto asset = asset_ID_to_asset_[asset_ID];
    const auto equal_element =
        asset_name_to_asset_ID_.equal_range(asset->GetAssetName());
    auto beg = equal_element.first;
    while (beg != equal_element.second) {
      if (beg->second == asset_ID) {
        std::pair<std::string, uint32_t> new_pair = *beg;
        new_pair.first = new_asset_name;
        asset_name_to_asset_ID_.erase(beg);
        asset_name_to_asset_ID_.emplace(new_pair);
        return true;
      }
      ++beg;
    }
    assert(false);  // Logic error, it is expected to find the desired element.
  }
  return false;
}

bool MM::AssetSystem::AssetManager::Destroy() {
  std::lock_guard<std::mutex> guard{sync_flag_};
  if (asset_manager_) {
    delete asset_manager_;
    asset_manager_ = nullptr;

    return true;
  }

  return true;
}

MM::AssetSystem::AssetManager* MM::AssetSystem::AssetManager::GetInstance() {
  if (asset_manager_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!asset_manager_) {
      asset_manager_ = new AssetManager{};
    }
  }

  return asset_manager_;
}

MM::AssetSystem::AssetManager& MM::AssetSystem::AssetSystem::GetAssetManager()
    const {
  return *assert_manager_;
}

MM::AssetSystem::AssetSystem::~AssetSystem() {
  MM::AssetSystem::AssetSystem::Destroy();
}

bool MM::AssetSystem::AssetSystem::Destroy() {
  std::lock_guard<std::mutex> guard{sync_flag_};
  if (asset_system_) {
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
    }
  }
  return asset_system_;
}

MM::AssetSystem::AssetSystem::AssetSystem()
    : assert_manager_(AssetManager::GetInstance()) {}
