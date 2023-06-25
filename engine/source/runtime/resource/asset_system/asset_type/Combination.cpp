//
// Created by beimingxianyu on 23-6-16.
//

#include "runtime/resource/asset_system/asset_type/Combination.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {

Combination::Combination(Combination&& other) noexcept
    : asset_handlers_(std::move(other.asset_handlers_)) {}

Combination& Combination::operator=(Combination&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  asset_handlers_ = std::move(other.asset_handlers_);

  return *this;
}

bool Combination::IsValid() const { return false; }

AssetType Combination::GetAssetType() const { return AssetType::COMBINATION; }

std::string Combination::GetAssetTypeString() const {
  return MM_ASSET_TYPE_UNDEFINED;
}

ExecuteResult Combination::GetJson(rapidjson::Document&) const {
  LOG_FATAL("This function should not be called.");
  return ExecuteResult ::UNDEFINED_ERROR;
}

void Combination::Release() { asset_handlers_.clear(); }
std::vector<AssetManager::AssetHandler>& Combination::GetAssetHandlers() {
  return asset_handlers_;
}

const std::vector<AssetManager::AssetHandler>& Combination::GetAssetHandlers()
    const {
  return asset_handlers_;
}

AssetManager::AssetHandler& Combination::Get(std::uint64_t index) {
  assert(index < asset_handlers_.size());
  return asset_handlers_[index];
}

const AssetManager::AssetHandler& Combination::Get(std::uint64_t index) const {
  assert(index < asset_handlers_.size());
  return asset_handlers_[index];
}

Combination::Combination(const FileSystem::Path& combination_path)
    : AssetBase(combination_path) {
  if (!AssetBase::IsValid()) {
    LOG_ERROR(std::string("Failed to load the combination asset with path ") +
              combination_path.StringView().data() +
              ",because the file does not exist.");
    return;
  }

  std::ifstream combination_json_fstream(combination_path.StringView().data());
  if (!combination_json_fstream.is_open()) {
    LOG_ERROR(combination_path.String() + "can't open.");
    return;
  }
  std::ostringstream combination_json_string_stream;
  std::string one_line_string;
  while (std::getline(combination_json_fstream, one_line_string)) {
    combination_json_string_stream << one_line_string;
  }
  combination_json_fstream.close();
  Utils::Json::Document combination_json;
  while (combination_json.Parse(combination_json_string_stream.str().c_str())
             .HasParseError()) {
    LOG_ERROR(std::string("Combination asset has parse error."));
    return;
  }

  std::vector<AssetManager::AssetHandler> images;
  std::vector<AssetManager::AssetHandler> meshes;
  TaskSystem::Taskflow taskflow;
  TaskSystem::Future<void>* future{new TaskSystem::Future<void>{}};

  MM_CHECK(
      LoadImages(combination_path, combination_json, taskflow, future, images),
      return;);
  MM_CHECK(
      LoadMeshes(combination_path, combination_json, taskflow, future, meshes),
      return;);

  *future = TASK_SYSTEM->Run(TaskSystem::TaskType::Common, taskflow);
  future->get();

  for (const auto& image : images) {
    if (!image.IsValid()) {
      return;
    }
  }
  for (const auto& mesh : meshes) {
    if (!mesh.IsValid()) {
      return;
    }
  }

  asset_handlers_.reserve(images.size() + meshes.size());
  for (auto& image : images) {
    asset_handlers_.emplace_back(std::move(image));
  }
  for (auto& mesh : meshes) {
    asset_handlers_.emplace_back(std::move(mesh));
  }
}

Utils::ExecuteResult Combination::LoadImages(
    const FileSystem::Path& json_path,
    const Utils::Json::Document& combination_json,
    TaskSystem::Taskflow& taskflow, TaskSystem::Future<void>* future,
    std::vector<AssetManager::AssetHandler>& images) {
  auto images_iter = combination_json.FindMember("images");
  if (images_iter == combination_json.MemberEnd() &&
      images_iter->value.IsArray()) {
    return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }
  std::vector<FileSystem::Path> image_paths;
  std::vector<std::uint32_t> image_desired_channels;
  for (Utils::Json::Value::ConstValueIterator image_iter =
           images_iter->value.Begin();
       image_iter != images_iter->value.End(); ++image_iter) {
    if (!image_iter->IsObject()) {
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    auto image_path = image_iter->FindMember("image path");
    if (image_path == image_iter->MemberEnd() &&
        !image_path->value.IsString()) {
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    auto image_desired_channel = image_iter->FindMember("image channel");
    if (image_desired_channel == image_iter->MemberEnd() &&
        !image_desired_channel->value.IsUint()) {
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    image_paths.emplace_back(json_path + std::string("/") +
                             image_path->value.GetString());
    image_desired_channels.emplace_back(image_desired_channel->value.GetUint());
  }
  AssetManager* asset_manager = AssetManager::GetInstance();
  for (std::uint64_t index = 0; index != images.size(); ++index) {
    taskflow.emplace([asset_manager, &image_paths, &image_desired_channels,
                      &images, index, future]() {
      FileSystem::LastWriteTime last_write_time;
      std::uint64_t path_hash = image_paths[index].GetHash();
      MM_CHECK_WITHOUT_LOG(
          FILE_SYSTEM->GetLastWriteTime(image_paths[index], last_write_time),
          future->cancel();)
      AssetID asset_id =
          path_hash ^ static_cast<std::uint64_t>(
                          last_write_time.time_since_epoch().count());
      ExecuteResult result = ExecuteResult ::UNDEFINED_ERROR;
      while ((result =
                  asset_manager->GetAssetByAssetID(asset_id, images[index])) ==
             ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT) {
        std::unique_ptr<Image> image = std::make_unique<Image>(
            image_paths[index], image_desired_channels[index]);
        if (!image->IsValid()) {
          future->cancel();
        }
        result = asset_manager->AddAsset(std::move(image), images[index]);
        if (result == Utils::ExecuteResult::SUCCESS) {
          return;
        }
        if (result != Utils::ExecuteResult::OPERATION_NOT_SUPPORTED) {
          break;
        }
        MM_CHECK_WITHOUT_LOG(
            FILE_SYSTEM->GetLastWriteTime(image_paths[index], last_write_time),
            future->cancel();)
        asset_id = path_hash ^ last_write_time.time_since_epoch().count();
      }
      if (result == Utils::ExecuteResult::SUCCESS) {
        return;
      } else {
        future->cancel();
      }
    });
  }

  return Utils::ExecuteResult ::SUCCESS;
}

Utils::ExecuteResult Combination::LoadMeshes(
    const FileSystem::Path& json_path,
    const rapidjson::Document& combination_json, TaskSystem::Taskflow& taskflow,
    TaskSystem::Future<void>* future,
    std::vector<AssetManager::AssetHandler>& meshes) {
  auto meshes_iter = combination_json.FindMember("meshes");
  if (meshes_iter == combination_json.MemberEnd() &&
      meshes_iter->value.IsArray()) {
    return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }
  std::vector<FileSystem::Path> mesh_paths;
  std::vector<std::uint32_t> mesh_indexes;
  for (Utils::Json::Value::ConstValueIterator mesh_iter =
           meshes_iter->value.Begin();
       mesh_iter != meshes_iter->value.End(); ++mesh_iter) {
    if (!mesh_iter->IsObject()) {
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    auto mesh_path = mesh_iter->FindMember("mesh path");
    if (mesh_path == mesh_iter->MemberEnd() && !mesh_path->value.IsString()) {
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    auto mesh_index = mesh_iter->FindMember("mesh index");
    if (mesh_index == mesh_iter->MemberEnd() && !mesh_index->value.IsUint()) {
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    mesh_paths.emplace_back(json_path + std::string("/") +
                            mesh_path->value.GetString());
    mesh_indexes.emplace_back(mesh_index->value.GetUint());
  }
  AssetManager* asset_manager = AssetManager::GetInstance();
  for (std::uint64_t index = 0; index != meshes.size(); ++index) {
    taskflow.emplace([asset_manager, &mesh_paths, &meshes, mesh_indexes, index,
                      future]() {
      FileSystem::LastWriteTime last_write_time;
      std::uint64_t path_hash = mesh_paths[index].GetHash();
      MM_CHECK_WITHOUT_LOG(
          FILE_SYSTEM->GetLastWriteTime(mesh_paths[index], last_write_time),
          future->cancel();)
      AssetID asset_id =
          path_hash ^ static_cast<std::uint64_t>(
                          last_write_time.time_since_epoch().count());
      ExecuteResult result = ExecuteResult ::UNDEFINED_ERROR;
      while ((result =
                  asset_manager->GetAssetByAssetID(asset_id, meshes[index])) ==
             ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT) {
        std::unique_ptr<Mesh> mesh =
            std::make_unique<Mesh>(mesh_paths[index], mesh_indexes[index]);
        if (!mesh->IsValid()) {
          future->cancel();
        }
        result = asset_manager->AddAsset(std::move(mesh), meshes[index]);
        if (result == Utils::ExecuteResult::SUCCESS) {
          return;
        }
        if (result != Utils::ExecuteResult::OPERATION_NOT_SUPPORTED) {
          break;
        }
        MM_CHECK_WITHOUT_LOG(
            FILE_SYSTEM->GetLastWriteTime(mesh_paths[index], last_write_time),
            future->cancel();)
        asset_id = path_hash ^ last_write_time.time_since_epoch().count();
      }
      if (result == Utils::ExecuteResult::SUCCESS) {
        return;
      } else {
        future->cancel();
      }
    });
  }

  return Utils::ExecuteResult ::SUCCESS;
}

Combination::Combination(
    std::vector<AssetManager::AssetHandler>&& asset_handlers)
    : asset_handlers_(std::move(asset_handlers)) {}

template <std::uint64_t index>
const AssetManager::AssetHandler& Combination::Get() const {
  assert(index < asset_handlers_.size());
  return asset_handlers_[index];
}

template <std::uint64_t index>
AssetManager::AssetHandler& Combination::Get() {
  assert(index < asset_handlers_.size());
  return asset_handlers_[index];
}
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM