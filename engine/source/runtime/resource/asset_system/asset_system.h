#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <map>
#include <set>
#include <shared_mutex>

#include "runtime/resource/asset_system/asset_system.h"
#include "runtime/resource/asset_type/asset_type.h"
#include "runtime/core/log/log_system.h"
#include "runtime/platform/config_system/config_system.h"
#include "runtime/platform/file_system/file_system.h"
#include "utils/marco.h"

namespace MM {
namespace AssetSystem {
IMPORT_CONFIG_SYSTEM
IMPORT_FILE_SYSTEM
IMPORT_LOG_SYSTEM

class AssetSystem;

class AssetManager {
  friend class AssetSystem;

 public:
  AssetManager(const AssetManager& other) = delete;
  AssetManager(AssetManager&& other) = delete;
  AssetManager& operator=(const AssetManager& other) = delete;
  AssetManager& operator=(AssetManager&& other) = delete;

 public:
  static AssetManager* GetInstance();

  bool LoadImage(const std::string& asset_name,
                 const FileSystem::Path& image_path,
                 const int& desired_channels = STBI_rgb_alpha);

  std::vector<std::shared_ptr<AssetType::AssetBase>> GetAssetsByName(
      const std::string& asset_name) const;

  std::shared_ptr<AssetType::AssetBase> GetAssetByID(const uint32_t& asset_ID) const;

  bool Erase(const uint32_t& asset_ID);

  bool ChangeAssetName(const uint32_t& asset_ID,
                       const std::string& new_asset_name);

private:
  ~AssetManager() = default;

  static bool Destroy();

 protected:
  AssetManager() = default;
  static AssetManager* asset_manager_;

 private:
  std::multimap<std::string, uint32_t> asset_name_to_asset_ID_{};
  std::unordered_map<uint32_t, std::shared_ptr<AssetType::AssetBase>> asset_ID_to_asset_{};
  mutable std::shared_mutex writer_mutex_{};

  static std::atomic_uint32_t increase_ID_;
  static std::mutex sync_flag_;
  static std::set<std::string> support_image_format;
};

class AssetSystem {
 public:
  AssetSystem(const AssetSystem& other) = delete;
  AssetSystem(AssetSystem&& other) noexcept = delete;
  AssetSystem& operator=(const AssetSystem& other) = delete;
  AssetSystem& operator=(AssetSystem&& other) = delete;

 public:
  static AssetSystem* GetInstance();
  AssetManager& GetAssetManager() const;

private:
  static bool Destroy();

 protected:
  ~AssetSystem();
  AssetSystem();
  static AssetSystem* asset_system_;

 private:
  AssetManager* assert_manager_;
  static std::mutex sync_flag_;
};
}  // namespace AssetSystem
}  // namespace MM
