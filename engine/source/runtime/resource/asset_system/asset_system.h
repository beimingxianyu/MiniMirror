#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <map>
#include <set>
#include <shared_mutex>

#include "runtime/core/manager/ManagerBase.h"
#include "runtime/resource/asset_system/asset_type/image.h"
#include "runtime/resource/asset_system/asset_type/mesh.h"
#include "runtime/resource/asset_system/import_other_system.h"
#include "utils/marco.h"

namespace MM {
namespace AssetSystem {
class AssetSystem;

class AssetManager
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
  class AssetHandler {
   public:
    AssetHandler() = default;
    ~AssetHandler() = default;
    AssetHandler(BaseHandler&& base_handler,
                 typename AssetIDToObjectIDContainerType::HandlerType&&
                     asset_id_to_object_id_handler)
        : base_handler_(std::move(base_handler)),
          asset_ID_to_object_handler_(
              std::move(asset_id_to_object_id_handler)) {}
    AssetHandler(const AssetHandler& other) = default;
    AssetHandler(AssetHandler&& other) noexcept
        : base_handler_(std::move(other.base_handler_)),
          asset_ID_to_object_handler_(
              std::move(other.asset_ID_to_object_handler_)) {}
    AssetHandler& operator=(const AssetHandler& other) {
      if (&other == this) {
        return *this;
      }

      base_handler_ = other.base_handler_;
      asset_ID_to_object_handler_ = other.asset_ID_to_object_handler_;

      return *this;
    }
    AssetHandler& operator=(AssetHandler&& other) noexcept {
      if (&other == this) {
        return *this;
      }

      base_handler_ = std::move(other.base_handler_);
      asset_ID_to_object_handler_ =
          std::move(other.asset_ID_to_object_handler_);

      return *this;
    }

   public:
    bool IsValid() const { return base_handler_.IsValid(); }

    Manager::ManagedObjectID GetObjectID() const {
      return base_handler_.GetObjectID();
    }

    AssetType::AssetID GetAssetID() const {
      return asset_ID_to_object_handler_.GetKey();
    }

    const std::string& GetObjectName() const {
      return base_handler_.GetObjectName();
    }

    const std::string& GetAssetName() const { return GetObjectName(); }

    ManagedType& GetObject() { return base_handler_.GetObject(); }

    const ManagedType& GetObject() const { return base_handler_.GetObject(); }

    ManagedType* GetObjectPtr() { return base_handler_.GetObjectPtr(); }

    const ManagedType* GetObjectPtr() const {
      return base_handler_.GetObjectPtr();
    }

    AssetType::AssetBase& GetAsset() { return *GetObject(); }

    const AssetType::AssetBase& GetAsset() const { return *GetObject(); }

    AssetType::AssetBase* GetAssetPtr() { return GetObject().get(); }

    const AssetType::AssetBase* GetAssetPtr() const {
      return GetObject().get();
    }

    void Release() {
      base_handler_.Release();
      asset_ID_to_object_handler_.Release();
    }

    typename BaseNameToIDContainer::HandlerType& GetNameToIDHandler() {
      return base_handler_.GetNameToIDHandler();
    }

    const typename BaseNameToIDContainer::HandlerType& GetNameToIDHandler()
        const {
      return base_handler_.GetNameToIDHandler();
    }

    typename BaseIDToObjectContainer::HandlerType& GetIDToObjectHandler() {
      return base_handler_.GetIDToObjectHandler();
    }

    const typename BaseIDToObjectContainer::HandlerType& GetIDToObjectHandler()
        const {
      return base_handler_.GetIDToObjectHandler();
    }

    typename AssetIDToObjectIDContainerType ::HandlerType&
    GetAssetIDToObjectIDHandler() {
      return asset_ID_to_object_handler_;
    }

    const typename AssetIDToObjectIDContainerType ::HandlerType&
    GetAssetIDToObjectIDHandler() const {
      return asset_ID_to_object_handler_;
    }

   private:
    BaseHandlerType base_handler_{};
    AssetIDToObjectIDContainerType ::HandlerType asset_ID_to_object_handler_{};
  };

 public:
  static AssetManager* GetInstance();

  ExecuteResult AddAsset(AssetType::AssetBase&& asset, HandlerType& handler);

  ExecuteResult AddImage(FileSystem::Path image_path, int desired_channels,
                         HandlerType& handler);

  ExecuteResult AddImage(
      const std::string& asset_name, AssetType::AssetID asset_id,
      const AssetType::Image::ImageInfo& image_info,
      std::unique_ptr<stbi_uc, AssetType::Image::StbiImageFree> image_pixels,
      HandlerType& handler);

  ExecuteResult AddMesh(const FileSystem::Path& mesh_path, uint32_t mesh_index,
                        HandlerType& handler);

  ExecuteResult AddMesh(
      const std::string& asset_name, AssetType::AssetID asset_ID,
      std::unique_ptr<AssetType::RectangleBox>&& aabb_box,
      std::unique_ptr<std::vector<uint32_t>>&& indexes,
      std::unique_ptr<std::vector<AssetType::Vertex>>&& vertices,
      HandlerType& handler);

  ExecuteResult AddMesh(
      const std::string& asset_name, AssetType::AssetID asset_ID,
      std::unique_ptr<AssetType::CapsuleBox>&& aabb_box,
      std::unique_ptr<std::vector<uint32_t>>&& indexes,
      std::unique_ptr<std::vector<AssetType::Vertex>>&& vertices,
      HandlerType& handler);

  ExecuteResult GetIDByAssetID(AssetType::AssetID asset_id,
                               Manager::ManagedObjectID& object_id) const;

  ExecuteResult GetNameByAssetID(AssetType::AssetID asset_id,
                                 std::string& name) const;

  ExecuteResult GetAssetNameByAssetID(AssetType::AssetID asset_id,
                                      std::string& asset_name) const;

  ExecuteResult GetAssetIDByID(Manager::ManagedObjectID object_id,
                               AssetType::AssetID asset_id) const;

  ExecuteResult GetAssetIDByName(std::string& name,
                                 AssetType::AssetID asset_id) const;

  ExecuteResult GetAssetIDByAssetName(std::string& asset_name,
                                      AssetType::AssetID asset_id) const;

 private:
  ~AssetManager() = default;

  static bool Destroy();

 protected:
  AssetManager() = default;
  static AssetManager* asset_manager_;

 private:
  AssetIDToObjectIDContainerType asset_ID_to_object_ID_{};

  static std::mutex sync_flag_;
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
