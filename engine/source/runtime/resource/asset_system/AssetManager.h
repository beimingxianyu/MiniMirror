#pragma once

#include <cstdint>
#include <iterator>
#include <map>
#include <set>
#include <shared_mutex>

#include "runtime/core/manager/ManagerBase.h"
#include "runtime/resource/asset_system/asset_type/Image.h"
#include "runtime/resource/asset_system/asset_type/Mesh.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "runtime/resource/asset_system/import_other_system.h"
#include "utils/marco.h"

namespace MM {
namespace AssetSystem {
class AssetSystem;

class AssetManager final
    : public Manager::ManagerBase<std::unique_ptr<AssetType::AssetBase>,
                                  Manager::ManagedObjectIsSmartPoint> {
  friend class AssetSystem;

 public:
  class AssetHandler;

  using ManagedType = std::unique_ptr<AssetType::AssetBase>;
  using BaseManagerType =
      Manager::ManagerBase<ManagedType, Manager::ManagedObjectIsSmartPoint>;
  using HandlerType = AssetHandler;
  using BaseHandlerType = typename BaseManagerType ::HandlerType;
  using AssetIDToObjectIDContainerType =
      Manager::ManagedObjectUnorderedMap<AssetType::AssetID,
                                         Manager::ManagedObjectID>;

 public:
  AssetManager(const AssetManager& other) = delete;
  AssetManager(AssetManager&& other) = delete;
  AssetManager& operator=(const AssetManager& other) = delete;
  AssetManager& operator=(AssetManager&& other) = delete;

 public:
  class AssetHandler final : public BaseHandlerType {
   public:
    AssetHandler() = default;
    ~AssetHandler() = default;
    AssetHandler(BaseHandlerType&& base_handler,
                 typename AssetIDToObjectIDContainerType::HandlerType&&
                     asset_id_to_object_id_handler);
    AssetHandler(const AssetHandler& other) = default;
    AssetHandler(AssetHandler&& other) noexcept;
    AssetHandler& operator=(const AssetHandler& other);
    AssetHandler& operator=(AssetHandler&& other) noexcept;

   public:
    bool IsValid() const override;

    std::uint32_t GetUseCount();

    AssetType::AssetID GetAssetID() const;

    const std::string& GetAssetName() const;

    AssetType::AssetBase& GetAsset();

    const AssetType::AssetBase& GetAsset() const;

    AssetType::AssetBase* GetAssetPtr();

    const AssetType::AssetBase* GetAssetPtr() const;

    void Release() override;

    typename AssetIDToObjectIDContainerType ::HandlerType&
    GetAssetIDToObjectIDHandler();

    const typename AssetIDToObjectIDContainerType ::HandlerType&
    GetAssetIDToObjectIDHandler() const;

   private:
    AssetIDToObjectIDContainerType ::HandlerType asset_ID_to_object_handler_{};
  };

 public:
  bool IsValid() const override;

  static AssetManager* GetInstance();

  bool Have(AssetType::AssetID asset_ID) const;

  ExecuteResult AddAsset(std::unique_ptr<AssetType::AssetBase>&& asset,
                         HandlerType& handler);

  ExecuteResult AddImage(FileSystem::Path image_path, int desired_channels,
                         HandlerType& handler);

  ExecuteResult AddImage(
      const FileSystem::Path& asset_path, AssetType::AssetID asset_id,
      const AssetType::Image::ImageInfo& image_info,
      std::unique_ptr<stbi_uc, AssetType::Image::StbiImageFree>&& image_pixels,
      HandlerType& handler);

  ExecuteResult AddMesh(const FileSystem::Path& mesh_path, uint32_t mesh_index,
                        HandlerType& handler);

  ExecuteResult AddMesh(
      const FileSystem::Path& asset_path, AssetType::AssetID asset_ID,
      std::unique_ptr<AssetType::RectangleBox>&& aabb_box,
      std::unique_ptr<std::vector<uint32_t>>&& indexes,
      std::unique_ptr<std::vector<AssetType::Vertex>>&& vertices,
      HandlerType& handler);

  ExecuteResult AddMesh(
      const FileSystem::Path& asset_path, AssetType::AssetID asset_ID,
      std::unique_ptr<AssetType::CapsuleBox>&& capsule_box,
      std::unique_ptr<std::vector<uint32_t>>&& indexes,
      std::unique_ptr<std::vector<AssetType::Vertex>>&& vertices,
      HandlerType& handler);

  ExecuteResult GetAssetByID(Manager::ManagedObjectID managed_object_ID,
                             HandlerType& handler) const;

  ExecuteResult GetAssetByAssetID(AssetType::AssetID asset_ID,
                                  HandlerType& handler) const;

  ExecuteResult GetAssetByName(const std::string& name,
                               std::vector<HandlerType>& handlers) const;

  ExecuteResult GetAssetByAssetName(const std::string& asset_name,
                                    std::vector<HandlerType>& handler) const;

  ExecuteResult GetIDByAssetID(AssetType::AssetID asset_ID,
                               Manager::ManagedObjectID& object_ID) const;

  ExecuteResult GetNameByAssetID(AssetType::AssetID asset_ID,
                                 std::string& name) const;

  ExecuteResult GetAssetNameByAssetID(AssetType::AssetID asset_ID,
                                      std::string& asset_name) const;

  ExecuteResult GetAssetIDByID(Manager::ManagedObjectID object_ID,
                               AssetType::AssetID& asset_ID) const;

  ExecuteResult GetAssetIDByName(
      const std::string& name,
      std::vector<AssetType::AssetID>& asset_IDs) const;

  ExecuteResult GetAssetIDByAssetName(
      const std::string& asset_name,
      std::vector<AssetType::AssetID>& asset_IDs) const;

 protected:
  AssetManager() = default;
  AssetManager(std::uint64_t size);
  static AssetManager* asset_manager_;

 private:
  ~AssetManager() = default;

  static bool Destroy();

 private:
  AssetIDToObjectIDContainerType asset_ID_to_object_ID_{};

  static std::mutex sync_flag_;
};
}  // namespace AssetSystem
}  // namespace MM
