//
// Created by beimingxianyu on 23-6-23.
//

#include "AssetManager.h"

#include <cstdint>

#include "runtime/platform/base/error.h"

namespace MM {
namespace AssetSystem {
MM::AssetSystem::AssetManager* MM::AssetSystem::AssetManager::asset_manager_{
    nullptr};
std::mutex MM::AssetSystem::AssetManager::sync_flag_{};

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
      std::uint64_t asset_size = 0;
      if (CONFIG_SYSTEM->GetConfig("manager_size_asset_manager", asset_size) !=
          ExecuteResult::SUCCESS) {
        LOG_WARN("The number of managed asset was not specified.");
        if (CONFIG_SYSTEM->GetConfig("manager_size", asset_size) !=
            ExecuteResult::SUCCESS) {
          LOG_FATAL("The number of managed objects was not specified.");
        }
      }
      asset_manager_ = new AssetManager{asset_size};
    }
  }

  return asset_manager_;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::AddAsset(
    std::unique_ptr<AssetType::AssetBase>&& asset,
    MM::AssetSystem::AssetManager::HandlerType& handler) {
  if (!asset->IsValid()) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_INCORRECT;
  }

  if (asset->GetAssetType() == AssetType::AssetType::UNDEFINED) {
    return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  AssetType::AssetID asset_id = asset->GetAssetID();
  Manager::ManagedObjectID managed_object_id = asset->GetObjectID();

  BaseHandlerType base_handler;

  MM_CHECK_WITHOUT_LOG(AddObjectBase(std::move(asset), base_handler),
                       return MM_RESULT_CODE;)

  AssetIDToObjectIDContainerType ::HandlerType asset_ID_ID_handler;

  MM_CHECK_WITHOUT_LOG(
      asset_ID_to_object_ID_.AddObject(asset_id, std::move(managed_object_id),
                                       asset_ID_ID_handler),
      return MM_RESULT_CODE;)

  handler =
      HandlerType{std::move(base_handler), std::move(asset_ID_ID_handler)};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::AddImage(
    MM::FileSystem::Path image_path, int desired_channels,
    MM::AssetSystem::AssetManager::HandlerType& handler) {
  std::unique_ptr<AssetType::Image> image(
      std::make_unique<AssetType::Image>(image_path, desired_channels));

  return AddAsset(std::move(image), handler);
}

MM::ExecuteResult MM::AssetSystem::AssetManager::AddImage(
    const FileSystem::Path& asset_path, AssetType::AssetID asset_id,
    const AssetType::Image::ImageInfo& image_info,
    std::unique_ptr<stbi_uc, AssetType::Image::StbiImageFree>&& image_pixels,
    HandlerType& handler) {
  std::unique_ptr<AssetType::Image> image(std::make_unique<AssetType::Image>(
      asset_path, asset_id, image_info, std::move(image_pixels)));

  return AddAsset(std::move(image), handler);
}

MM::ExecuteResult MM::AssetSystem::AssetManager::AddMesh(
    const MM::FileSystem::Path& mesh_path, uint32_t mesh_index,
    MM::AssetSystem::AssetManager::HandlerType& handler) {
  std::unique_ptr<AssetType::Mesh> mesh(
      std::make_unique<AssetType::Mesh>(mesh_path, mesh_index));

  return AddAsset(std::move(mesh), handler);
}

MM::ExecuteResult MM::AssetSystem::AssetManager::AddMesh(
    const FileSystem::Path& asset_path, AssetType::AssetID asset_ID,
    std::unique_ptr<AssetType::RectangleBox>&& aabb_box,
    std::unique_ptr<std::vector<uint32_t>>&& indexes,
    std::unique_ptr<std::vector<AssetType::Vertex>>&& vertices,
    HandlerType& handler) {
  std::unique_ptr<AssetType::Mesh> mesh(std::make_unique<AssetType::Mesh>(
      asset_path, asset_ID, std::move(aabb_box), std::move(indexes),
      std::move(vertices)));

  return AddAsset(std::move(mesh), handler);
}

MM::ExecuteResult MM::AssetSystem::AssetManager::AddMesh(
    const FileSystem::Path& asset_path, AssetType::AssetID asset_ID,
    std::unique_ptr<AssetType::CapsuleBox>&& capsule_box,
    std::unique_ptr<std::vector<uint32_t>>&& indexes,
    std::unique_ptr<std::vector<AssetType::Vertex>>&& vertices,
    HandlerType& handler) {
  std::unique_ptr<AssetType::Mesh> mesh(std::make_unique<AssetType::Mesh>(
      asset_path, asset_ID, std::move(capsule_box), std::move(indexes),
      std::move(vertices)));

  return AddAsset(std::move(mesh), handler);
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetByID(
    MM::Manager::ManagedObjectID managed_object_ID,
    MM::AssetSystem::AssetManager::HandlerType& handler) const {
  BaseHandlerType base_handler;
  MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(managed_object_ID, base_handler),
                       return MM_RESULT_CODE;)

  AssetIDToObjectIDContainerType ::HandlerType asset_ID_ID_handler;
  MM_CHECK_WITHOUT_LOG(
      asset_ID_to_object_ID_.GetObject(base_handler.GetObject()->GetAssetID(),
                                       asset_ID_ID_handler),
      return MM_RESULT_CODE;)

  handler =
      HandlerType{std::move(base_handler), std::move(asset_ID_ID_handler)};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID,
    MM::AssetSystem::AssetManager::HandlerType& handler) const {
  AssetIDToObjectIDContainerType ::HandlerType asset_ID_ID_handler;
  MM_CHECK_WITHOUT_LOG(
      asset_ID_to_object_ID_.GetObject(asset_ID, asset_ID_ID_handler),
      return MM_RESULT_CODE;)

  BaseHandlerType base_handler;
  MM_CHECK_WITHOUT_LOG(
      GetObjectByIDBase(asset_ID_ID_handler.GetObject(), base_handler),
      return MM_RESULT_CODE;)

  handler =
      HandlerType{std::move(base_handler), std::move(asset_ID_ID_handler)};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetByName(
    const std::string& name, std::vector<HandlerType>& handlers) const {
  std::vector<BaseHandlerType> base_handlers;
  MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                       return MM_RESULT_CODE;)

  bool is_add = false;
  for (auto& base_handler : base_handlers) {
    AssetIDToObjectIDContainerType ::HandlerType asset_ID_ID_handler;
    MM_CHECK_WITHOUT_LOG(
        asset_ID_to_object_ID_.GetObject(base_handler.GetObject()->GetAssetID(),
                                         asset_ID_ID_handler),
        continue;)
    handlers.emplace_back(std::move(base_handler),
                          std::move(asset_ID_ID_handler));
    is_add = true;
  }

  if (is_add) {
    return ExecuteResult ::SUCCESS;
  }

  return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetByAssetName(
    const std::string& asset_name, std::vector<HandlerType>& handler) const {
  return GetAssetByName(asset_name, handler);
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetIDByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID,
    MM::Manager::ManagedObjectID& object_ID) const {
  AssetIDToObjectIDContainerType ::HandlerType handler;
  MM_CHECK_WITHOUT_LOG(asset_ID_to_object_ID_.GetObject(asset_ID, handler),
                       return MM_RESULT_CODE;)

  object_ID = handler.GetObject();

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetNameByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID, std::string& name) const {
  AssetIDToObjectIDContainerType ::HandlerType handler;
  MM_CHECK_WITHOUT_LOG(asset_ID_to_object_ID_.GetObject(asset_ID, handler),
                       return MM_RESULT_CODE;)

  MM_CHECK_WITHOUT_LOG(GetNameByID(handler.GetObject(), name),
                       return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetNameByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID,
    std::string& asset_name) const {
  return GetNameByAssetID(asset_ID, asset_name);
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetIDByID(
    MM::Manager::ManagedObjectID object_ID,
    MM::AssetSystem::AssetType::AssetID& asset_ID) const {
  BaseHandlerType handler;

  MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(object_ID, handler),
                       return MM_RESULT_CODE;)

  asset_ID = handler.GetObject()->GetAssetID();

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetIDByName(
    const std::string& name, std::vector<AssetType::AssetID>& asset_IDs) const {
  std::vector<BaseHandlerType> handlers;

  MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, handlers),
                       return MM_RESULT_CODE;)

  asset_IDs.reserve(asset_IDs.size() + handlers.size());
  for (auto& handler : handlers) {
    asset_IDs.emplace_back(handler.GetObject()->GetAssetID());
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::AssetSystem::AssetManager::GetAssetIDByAssetName(
    const std::string& asset_name,
    std::vector<AssetType::AssetID>& asset_IDs) const {
  return GetAssetIDByName(asset_name, asset_IDs);
}

bool AssetManager::Have(AssetType::AssetID asset_ID) const {
  return asset_ID_to_object_ID_.Have(asset_ID);
}

AssetManager::AssetManager(std::uint64_t size)
    : Manager::ManagerBase<std::unique_ptr<AssetType::AssetBase>,
                           Manager::ManagedObjectIsSmartPoint>(size),
      asset_ID_to_object_ID_(size) {}

bool AssetManager::IsValid() const {
  return ManagerBaseImp::IsValid() & asset_ID_to_object_ID_.IsValid();
}

MM::AssetSystem::AssetManager::AssetHandler::AssetHandler(
    MM::AssetSystem::AssetManager::AssetHandler::BaseHandler&& base_handler,
    MM::AssetSystem::AssetManager::AssetIDToObjectIDContainerType ::
        HandlerType&& asset_id_to_object_id_handler)
    : BaseHandlerType(std::move(base_handler)),
      asset_ID_to_object_handler_(std::move(asset_id_to_object_id_handler)) {}

MM::AssetSystem::AssetManager::AssetHandler::AssetHandler(
    MM::AssetSystem::AssetManager::AssetHandler&& other) noexcept
    : BaseHandlerType(std::move(other)),
      asset_ID_to_object_handler_(
          std::move(other.asset_ID_to_object_handler_)) {}

MM::AssetSystem::AssetManager::AssetHandler&
MM::AssetSystem::AssetManager::AssetHandler::operator=(
    const MM::AssetSystem::AssetManager::AssetHandler& other) {
  if (&other == this) {
    return *this;
  }

  BaseHandlerType::operator=(other);
  asset_ID_to_object_handler_ = other.asset_ID_to_object_handler_;

  return *this;
}

MM::AssetSystem::AssetManager::AssetHandler&
MM::AssetSystem::AssetManager::AssetHandler::operator=(
    MM::AssetSystem::AssetManager::AssetHandler&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  BaseHandlerType ::operator=(std::move(other));
  asset_ID_to_object_handler_ = std::move(other.asset_ID_to_object_handler_);

  return *this;
}

bool MM::AssetSystem::AssetManager::AssetHandler::IsValid() const {
  return BaseHandlerType ::IsValid() && asset_ID_to_object_handler_.IsValid();
}

MM::AssetSystem::AssetType::AssetID
MM::AssetSystem::AssetManager::AssetHandler::GetAssetID() const {
  return asset_ID_to_object_handler_.GetKey();
}

const std::string& MM::AssetSystem::AssetManager::AssetHandler::GetAssetName()
    const {
  return GetObjectName();
}

MM::AssetSystem::AssetType::AssetBase&
MM::AssetSystem::AssetManager::AssetHandler::GetAsset() {
  return *GetObject();
}

const MM::AssetSystem::AssetType::AssetBase&
MM::AssetSystem::AssetManager::AssetHandler::GetAsset() const {
  return *GetObject();
}

MM::AssetSystem::AssetType::AssetBase*
MM::AssetSystem::AssetManager::AssetHandler::GetAssetPtr() {
  return GetObject().get();
}

const MM::AssetSystem::AssetType::AssetBase*
MM::AssetSystem::AssetManager::AssetHandler::GetAssetPtr() const {
  return GetObject().get();
}

void MM::AssetSystem::AssetManager::AssetHandler::Release() {
  BaseHandlerType::Release();
  asset_ID_to_object_handler_.Release();
}

MM::Manager::ManagedObjectUnorderedMap<
    MM::AssetSystem::AssetType::AssetID,
    MM::Manager::ManagedObjectID>::HandlerType&
MM::AssetSystem::AssetManager::AssetHandler::GetAssetIDToObjectIDHandler() {
  return asset_ID_to_object_handler_;
}

const MM::Manager::ManagedObjectUnorderedMap<
    MM::AssetSystem::AssetType::AssetID,
    MM::Manager::ManagedObjectID>::HandlerType&
MM::AssetSystem::AssetManager::AssetHandler::GetAssetIDToObjectIDHandler()
    const {
  return asset_ID_to_object_handler_;
}

std::uint32_t AssetManager::AssetHandler::GetUseCount() {
  return GetIDToObjectHandler().GetUseCount();
}
}  // namespace AssetSystem
}  // namespace MM