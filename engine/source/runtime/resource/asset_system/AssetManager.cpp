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

      if (MM_CONFIG_SYSTEM->GetConfig("manager_size_asset_manager",
                                      asset_size).Exception(MM_WARN_DESCRIPTION2("The number of managed asset was not specified")).IsError()) {
        MM_CONFIG_SYSTEM->GetConfig("manager_size", asset_size).Exception(MM_FATAL_DESCRIPTION2("The number of managed object was not specified."));
      }
      asset_manager_ = new AssetManager{asset_size};
    }
  }

  return asset_manager_;
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::AddAsset(
    std::unique_ptr<AssetType::AssetBase>&& asset) {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  if (!asset->IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::INPUT_PARAMETERS_ARE_INCORRECT};
  }

  if (asset->GetAssetType() == AssetType::AssetType::UNDEFINED) {
    return ResultE<ErrorResult>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  AssetType::AssetID asset_id = asset->GetAssetID();
  Manager::ManagedObjectID managed_object_id = asset->GetObjectID();

  Result<BaseHandlerType, ErrorResult> base_handler = AddObjectBase(std::move(asset)).Exception();
  if (base_handler.IsError()) {
    return ResultE<ErrorResult>{base_handler.GetError().GetErrorCode()};
  }

  Result<AssetIDToObjectIDContainerType::HandlerType, ErrorResult> asset_ID_ID_handler = asset_ID_to_object_ID_.AddObject(asset_id, std::move(managed_object_id)).Exception();
  if (asset_ID_ID_handler.IsError()) {
    return ResultE<ErrorResult>{asset_ID_ID_handler.GetError().GetErrorCode()};
  }

  return ResultS<HandlerType>{std::move(base_handler.GetResult()), std::move(asset_ID_ID_handler.GetResult())};
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::AddImage(
    MM::FileSystem::Path image_path, int desired_channels) {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  std::unique_ptr<AssetType::Image> image(
      std::make_unique<AssetType::Image>(image_path, desired_channels));

  return AddAsset(std::move(image));
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::AddImage(
    const FileSystem::Path& asset_path, AssetType::AssetID asset_id,
    const AssetType::Image::ImageInfo& image_info,
    std::unique_ptr<stbi_uc, AssetType::Image::StbiImageFree>&& image_pixels) {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  std::unique_ptr<AssetType::Image> image(std::make_unique<AssetType::Image>(
      asset_path, asset_id, image_info, std::move(image_pixels)));

  return AddAsset(std::move(image));
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::AddMesh(
    const MM::FileSystem::Path& mesh_path, uint32_t mesh_index) {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  std::unique_ptr<AssetType::Mesh> mesh(
      std::make_unique<AssetType::Mesh>(mesh_path, mesh_index));

  return AddAsset(std::move(mesh));
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::AddMesh(
    const FileSystem::Path& asset_path, AssetType::AssetID asset_ID,
    std::unique_ptr<AssetType::RectangleBox>&& aabb_box,
    std::vector<uint32_t>&& indexes, std::vector<AssetType::Vertex>&& vertices) {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  std::unique_ptr<AssetType::Mesh> mesh(std::make_unique<AssetType::Mesh>(
      asset_path, asset_ID, std::move(aabb_box), std::move(indexes),
      std::move(vertices)));

  return AddAsset(std::move(mesh));
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::AddMesh(
    const FileSystem::Path& asset_path, AssetType::AssetID asset_ID,
    std::unique_ptr<AssetType::CapsuleBox>&& capsule_box,
    std::vector<uint32_t>&& indexes, std::vector<AssetType::Vertex>&& vertices) {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  std::unique_ptr<AssetType::Mesh> mesh(std::make_unique<AssetType::Mesh>(
      asset_path, asset_ID, std::move(capsule_box), std::move(indexes),
      std::move(vertices)));

  return AddAsset(std::move(mesh));
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::GetAssetByID(
    MM::Manager::ManagedObjectID managed_object_ID) const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Result<BaseHandlerType, ErrorResult> base_handler = GetObjectByIDBase(managed_object_ID).Exception();
  if (base_handler.IsError()) {
    return ResultE<ErrorResult>{base_handler.GetError().GetErrorCode()};
  }

  Result<AssetIDToObjectIDContainerType::HandlerType, ErrorResult> asset_ID_ID_handler = asset_ID_to_object_ID_.GetObject(base_handler.GetResult().GetObject()->GetAssetID()).Exception();
  if (asset_ID_ID_handler.IsError()) {
    return ResultE<ErrorResult>{base_handler.GetError().GetErrorCode()};
  }

  return ResultS<HandlerType>{std::move(base_handler.GetResult()), std::move(asset_ID_ID_handler.GetResult())};
}

MM::Result<MM::AssetSystem::AssetManager::HandlerType, ErrorResult> MM::AssetSystem::AssetManager::GetAssetByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID) const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Result<AssetIDToObjectIDContainerType::HandlerType, ErrorResult> asset_ID_ID_handler = asset_ID_to_object_ID_.GetObject(asset_ID).Exception();
  if (asset_ID_ID_handler.IsError()) {
    return ResultE<ErrorResult>{asset_ID_ID_handler.GetError().GetErrorCode()};
  }

  Result<BaseHandlerType, ErrorResult> base_handler = GetObjectByIDBase(asset_ID_ID_handler.GetResult().GetObject()).Exception();
  if (base_handler.IsError()) {
    return ResultE<ErrorResult>{base_handler.GetError().GetErrorCode()};
  }

  return ResultS<HandlerType>{std::move(base_handler.GetResult()), std::move(asset_ID_ID_handler.GetResult())};
}

MM::Result<std::vector<MM::AssetSystem::AssetManager::HandlerType>, MM::ErrorResult> MM::AssetSystem::AssetManager::GetAssetByName(
    const std::string& name) const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Result<std::vector<BaseHandlerType>, ErrorResult> base_handlers = GetObjectByNameBase(name, st_get_multiply_object).Exception();
  if (base_handlers.IsError()) {
    return ResultE<ErrorResult>{base_handlers.GetError().GetErrorCode()};
  }

  std::vector<MM::AssetSystem::AssetManager::HandlerType> handlers{};
  for (auto& base_handler : base_handlers.GetResult()) {
    Result<AssetIDToObjectIDContainerType::HandlerType> asset_ID_ID_handler = asset_ID_to_object_ID_.GetObject(base_handler.GetObject()->GetAssetID()).Exception();
    if (asset_ID_ID_handler.IsError()) {
      continue;
    }
    handlers.emplace_back(std::move(base_handler),
                          std::move(asset_ID_ID_handler.GetResult()));
  }

  if (handlers.empty()) {
    return ResultE<>{ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  return Result{std::move(handlers)};
}

MM::Result<std::vector<MM::AssetSystem::AssetManager::HandlerType>> MM::AssetSystem::AssetManager::GetAssetByAssetName(
    const std::string& asset_name) const {
  return GetAssetByName(asset_name);
}

MM::Result<MM::Manager::ManagedObjectID, ErrorResult> MM::AssetSystem::AssetManager::GetIDByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID) const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Result<AssetIDToObjectIDContainerType::HandlerType, ErrorResult> handler = asset_ID_to_object_ID_.GetObject(asset_ID).Exception();
  if (handler.IsError()) {
    return ResultE<>{handler.GetError().GetErrorCode()};
  }

  return Result{handler.GetResult().GetObject()};
}

MM::Result<std::string, ErrorResult> MM::AssetSystem::AssetManager::GetNameByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID) const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Result<AssetIDToObjectIDContainerType::HandlerType> handler = asset_ID_to_object_ID_.GetObject(asset_ID).Exception();
  if (handler.IsError()) {
    return ResultE<>{handler.GetError().GetErrorCode()};
  }

  auto result2 = GetNameByID(handler.GetResult().GetObject()).Exception();
  if (result2.IsSuccess()) {
    return Result{result2};
  } else {
    return ResultE<>{handler.GetError().GetErrorCode()};
  }
}

MM::Result<std::string, ErrorResult> MM::AssetSystem::AssetManager::GetAssetNameByAssetID(
    MM::AssetSystem::AssetType::AssetID asset_ID) const {
  return GetNameByAssetID(asset_ID);
}

Result<MM::AssetSystem::AssetType::AssetID, ErrorResult> MM::AssetSystem::AssetManager::GetAssetIDByID(
    MM::Manager::ManagedObjectID object_ID) const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Result<BaseHandlerType, ErrorResult> handler = GetObjectByIDBase(object_ID).Exception();
  if (handler.IsError()) {
    return ResultE<>{handler.GetError().GetErrorCode()};
  }

  return Result{handler.GetResult().GetObject()->GetAssetID()};
}

MM::Result<std::vector<AssetType::AssetID>, ErrorResult> MM::AssetSystem::AssetManager::GetAssetIDByName(
    const std::string& name) const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Result<std::vector<BaseHandlerType>> handlers = GetObjectByNameBase(name, st_get_multiply_object).Exception();
  if (handlers.IsError()) {
    return ResultE<>{handlers.GetError().GetErrorCode()};
  }

  std::vector<AssetType::AssetID> asset_IDs;
  asset_IDs.reserve(handlers.GetResult().size());
  for (auto& handler : handlers.GetResult()) {
    asset_IDs.emplace_back(handler.GetObject()->GetAssetID());
  }

  return Result{std::move(asset_IDs)};
}

MM::Result<std::vector<AssetType::AssetID>, ErrorResult> MM::AssetSystem::AssetManager::GetAssetIDByAssetName(
    const std::string& asset_name) const {
  return GetAssetIDByName(asset_name);
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