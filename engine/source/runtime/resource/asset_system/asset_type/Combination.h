#pragma once

#include "runtime/resource/asset_system/AssetManager.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {

class Combination : public AssetBase {
 public:
  Combination() = default;
  ~Combination() = default;
  explicit Combination(const FileSystem::Path& combination_path);
  explicit Combination(
      std::vector<AssetManager::AssetHandler>&& asset_handlers);
  Combination(const Combination& other) = delete;
  Combination(Combination&& other) noexcept;
  Combination& operator=(const Combination& other) = delete;
  Combination& operator=(Combination&& other) noexcept;

 public:
  bool IsValid() const override;

  AssetType GetAssetType() const override;

  std::string GetAssetTypeString() const override;

  ExecuteResult GetJson(rapidjson::Document& document) const override;

  void Release() override;

 protected:
  std::vector<AssetManager::AssetHandler>& GetAssetHandlers();

  const std::vector<AssetManager::AssetHandler>& GetAssetHandlers() const;

  template <std::uint64_t index>
  AssetManager::AssetHandler& Get();

  template <std::uint64_t index>
  const AssetManager::AssetHandler& Get() const;

  AssetManager::AssetHandler& Get(std::uint64_t index);

  const AssetManager::AssetHandler& Get(std::uint64_t index) const;

 private:
  static Utils::ExecuteResult LoadImages(
      const FileSystem::Path& json_path,
      const Utils::Json::Document& combination_json,
      TaskSystem::Taskflow& taskflow, TaskSystem::Future<void>* future,
      std::vector<AssetManager::AssetHandler>& images);

  static Utils::ExecuteResult LoadMeshes(
      const FileSystem::Path& json_path,
      const Utils::Json::Document& combination_json,
      TaskSystem::Taskflow& taskflow, TaskSystem::Future<void>* future,
      std::vector<AssetManager::AssetHandler>& meshes);

 private:
  std::vector<AssetManager::AssetHandler> asset_handlers_{};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
