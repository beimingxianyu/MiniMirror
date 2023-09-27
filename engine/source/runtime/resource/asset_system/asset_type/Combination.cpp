//
// Created by beimingxianyu on 23-6-16.
//

#include "runtime/resource/asset_system/asset_type/Combination.h"

#include <cstring>
#include <vector>

#include "Image.h"
#include "base/asset_base.h"
#include "base/asset_type_define.h"
#include "base/bounding_box.h"
#include "runtime/platform/base/error.h"
#include "runtime/platform/file_system/file_system.h"
#include "utils/error.h"
#include "utils/marco.h"

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

MM::Result<MM::Utils::Json::Document, ErrorResult> Combination::GetJson()
    const {
  MM_LOG_FATAL("This function should not be called.");
  return ResultE<ErrorResult>{ErrorCode::UNDEFINED_ERROR};
}

void Combination::Release() {
  AssetBase::Release();
  asset_handlers_.clear();
}
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
    MM_LOG_ERROR(
        std::string("Failed to load the combination asset with path ") +
        combination_path.StringView().data() +
        ",because the file does not exist.");
    return;
  }

  std::ifstream combination_json_fstream(combination_path.StringView().data());
  if (!combination_json_fstream.is_open()) {
    AssetBase::Release();
    MM_LOG_ERROR(combination_path.String() + "can't open.");
    return;
  }
  std::ostringstream combination_json_string_stream;
  std::string one_line_string;
  while (std::getline(combination_json_fstream, one_line_string)) {
    combination_json_string_stream << one_line_string;
  }
  combination_json_fstream.close();
  Utils::Json::Document combination_json;
  if (combination_json.Parse(combination_json_string_stream.str().c_str())
          .HasParseError()) {
    AssetBase::Release();
    MM_LOG_ERROR(std::string("Combination asset has parse error."));
    return;
  }

  std::vector<AssetManager::AssetHandler> images{};
  std::vector<AssetManager::AssetHandler> meshes{};

  TaskSystem::Taskflow taskflow;
  TaskSystem::Future<void>* future{new TaskSystem::Future<void>{}};
  bool load_result = true;

  Result<Nil, ErrorResult> images_load_result =
      LoadImages(combination_path, combination_json, taskflow, future, images,
                 load_result);
  images_load_result.Exception([function_name = MM_FUNCTION_NAME,
                    this_object = this](ErrorResult error_result) {
    MM_LOG_SYSTEM->CheckResult(
        error_result.GetErrorCode(),
        MM_LOG_DESCRIPTION_MESSAGE(function_name, Failed to load images.),
        LogSystem::LogSystem::LogLevel::ERROR);
    this_object->AssetBase::Release();
  });
  if (images_load_result.IsError()) {
    return;
  }

  Result<Nil, ErrorResult> meshes_load_result =
      LoadMeshes(combination_path, combination_json, taskflow, future, meshes,
                 load_result);
  meshes_load_result.Exception([function_name = MM_FUNCTION_NAME,
                   this_object = this](ErrorResult error_result) {
    MM_LOG_SYSTEM->CheckResult(
        error_result.GetErrorCode(),
        MM_LOG_DESCRIPTION_MESSAGE(function_name, Failed to load meshes.),
        LogSystem::LogSystem::LogLevel::ERROR);
    this_object->AssetBase::Release();
  });
  if (meshes_load_result.IsError()) {
    return;
  }

  *future = MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Common, taskflow);
  future->wait();

  if (!load_result) {
    Combination::Release();
    return;
  }

  asset_handlers_.reserve(images.size() +
                          meshes.size());
  for (auto& image : images) {
    asset_handlers_.emplace_back(std::move(image));
  }
  for (auto& mesh : meshes) {
    asset_handlers_.emplace_back(std::move(mesh));
  }
}

MM::Result<MM::Nil, MM::ErrorResult> Combination::LoadImages(
    const FileSystem::Path& json_path,
    const Utils::Json::Document& combination_json,
    TaskSystem::Taskflow& taskflow, TaskSystem::Future<void>* future,
    std::vector<AssetManager::AssetHandler>& images, bool& load_result) {
  auto images_iter = combination_json.FindMember("images");
  if (images_iter == combination_json.MemberEnd() ||
      !images_iter->value.IsArray()) {
    load_result = false;
    return ResultE<ErrorResult>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }
  std::vector<FileSystem::Path> image_paths;
  std::vector<std::uint32_t> image_desired_channels;
  for (Utils::Json::Value::ConstValueIterator image_iter =
           images_iter->value.Begin();
       image_iter != images_iter->value.End(); ++image_iter) {
    if (!image_iter->IsObject()) {
      load_result = false;
      return ResultE<ErrorResult>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
    }

    auto image_path = image_iter->FindMember("image path");
    if (image_path == image_iter->MemberEnd() ||
        !image_path->value.IsString()) {
      load_result = false;
      return ResultE<ErrorResult>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
    }

    auto image_desired_channel = image_iter->FindMember("image channel");
    if (image_desired_channel == image_iter->MemberEnd() ||
        !image_desired_channel->value.IsUint()) {
      load_result = false;
      return ResultE<ErrorResult>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
    }

    image_paths.emplace_back(
        json_path.GetParentDirPath() +
        (std::string("/") + image_path->value.GetString()));
    image_desired_channels.emplace_back(image_desired_channel->value.GetUint());
  }
  AssetManager* asset_manager = AssetManager::GetInstance();
  images.resize(image_paths.size());
  for (std::uint64_t index = 0; index != images.size(); ++index) {
    taskflow.emplace([asset_manager, image_path_in = image_paths[index],
                      image_desired_channel_in = image_desired_channels[index],
                      &images, index, future, &load_result]() {
      Result<FileSystem::LastWriteTime, ErrorResult> last_write_time = MM_FILE_SYSTEM->GetLastWriteTime(image_path_in);
      last_write_time.Exception([function_name = MM_FUNCTION_NAME, &load_result, &future](ErrorResult error_result) {
        MM_LOG_SYSTEM->CheckResult(error_result.GetErrorCode(),
                                   MM_LOG_DESCRIPTION_MESSAGE(function_name, Faied to get last write time.),
                                   LogSystem::LogSystem::LogLevel::ERROR);

        load_result = false;
        future->cancel();
      });
      if (last_write_time.IsError()) return;
      std::uint64_t path_hash = image_path_in.GetHash();
      Result<AssetID> asset_id;
      MM::AssetSystem::AssetType::Image::CalculateAssetID(
          image_path_in, image_desired_channel_in, asset_id);
      ErrorCode result = ErrorCode::UNDEFINED_ERROR;
      while ((result =
                  asset_manager->GetAssetByAssetID(asset_id, images[index])) ==
             ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT) {
        std::unique_ptr<Image> image =
            std::make_unique<Image>(image_path_in, image_desired_channel_in);
        if (!image->IsValid()) {
          load_result = false;
          future->cancel();
          return;
        }
        result = asset_manager->AddAsset(std::move(image), images[index]);
        if (result == Utils::ExecuteResult::SUCCESS) {
          return;
        }
        if (asset_manager->Have(asset_id)) {
          MM_CHECK_WITHOUT_LOG(
              MM::AssetSystem::AssetType::Image::CalculateAssetID(
                  image_path_in, image_desired_channel_in, asset_id),
              load_result = false;
              future->cancel(); return;);
          continue;
        }
        load_result = false;
        future->cancel();
        return;
      }
      if (result == Utils::ExecuteResult::SUCCESS) {
        return;
      } else {
        load_result = false;
        future->cancel();
        return;
      }
    });
  }

  return Utils::ExecuteResult ::SUCCESS;
}

Utils::ExecuteResult Combination::LoadMeshes(
    const FileSystem::Path& json_path,
    const rapidjson::Document& combination_json, TaskSystem::Taskflow& taskflow,
    TaskSystem::Future<void>* future,
    std::vector<AssetManager::AssetHandler>& meshes, bool& load_result) {
  auto meshes_iter = combination_json.FindMember("meshes");
  if (meshes_iter == combination_json.MemberEnd() ||
      !meshes_iter->value.IsArray()) {
    load_result = false;
    return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }
  std::vector<FileSystem::Path> mesh_paths;
  std::vector<std::uint32_t> mesh_indexes;
  std::vector<BoundingBox::BoundingBoxType> mesh_bounding_box_types;
  for (Utils::Json::Value::ConstValueIterator mesh_iter =
           meshes_iter->value.Begin();
       mesh_iter != meshes_iter->value.End(); ++mesh_iter) {
    if (!mesh_iter->IsObject()) {
      load_result = false;
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    auto mesh_path = mesh_iter->FindMember("mesh path");
    if (mesh_path == mesh_iter->MemberEnd() || !mesh_path->value.IsString()) {
      load_result = false;
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    auto mesh_index = mesh_iter->FindMember("mesh index");
    if (mesh_index == mesh_iter->MemberEnd() || !mesh_index->value.IsUint()) {
      load_result = false;
      return Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    mesh_paths.emplace_back(json_path.GetParentDirPath() +
                            (std::string("/") + mesh_path->value.GetString()));
    mesh_indexes.emplace_back(mesh_index->value.GetUint());

    auto mesh_bounding_box_type = mesh_iter->FindMember("bounding box type");
    if (mesh_bounding_box_type != mesh_iter->MemberEnd()) {
      if (!mesh_bounding_box_type->value.IsString()) {
        load_result = false;
        return Utils::ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
      }

      if (std::strcmp(mesh_bounding_box_type->value.GetString(), "AABB") == 0) {
        mesh_bounding_box_types.emplace_back(
            BoundingBox::BoundingBoxType::AABB);
      } else if (std::strcmp(mesh_bounding_box_type->value.GetString(),
                             "CAPSULE") == 0) {
        mesh_bounding_box_types.emplace_back(
            BoundingBox::BoundingBoxType::CAPSULE);
      } else {
        load_result = false;
        return Utils::ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
      }
    } else {
      mesh_bounding_box_types.emplace_back(BoundingBox::BoundingBoxType::AABB);
    }
  }
  AssetManager* asset_manager = AssetManager::GetInstance();
  meshes.resize(mesh_paths.size());
  for (std::uint64_t index = 0; index != meshes.size(); ++index) {
    taskflow.emplace([asset_manager, mesh_path_in = mesh_paths[index], &meshes,
                      mesh_index_in = mesh_indexes[index],
                      mesh_bounding_box_type_in =
                          mesh_bounding_box_types[index],
                      index, future, &load_result]() {
      FileSystem::LastWriteTime last_write_time;
      AssetID asset_id;
      MM_CHECK_WITHOUT_LOG(
          MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
              mesh_path_in, mesh_index_in, mesh_bounding_box_type_in, asset_id),
          load_result = false;
          future->cancel(); return;);
      ExecuteResult result = ExecuteResult ::UNDEFINED_ERROR;
      while ((result =
                  asset_manager->GetAssetByAssetID(asset_id, meshes[index])) ==
             ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT) {
        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(
            mesh_path_in, mesh_index_in, mesh_bounding_box_type_in);
        if (!mesh->IsValid()) {
          load_result = false;
          future->cancel();
          return;
        }
        result = asset_manager->AddAsset(std::move(mesh), meshes[index]);
        if (result == Utils::ExecuteResult::SUCCESS) {
          return;
        }
        if (asset_manager->Have(asset_id)) {
          MM_CHECK_WITHOUT_LOG(
              MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                  mesh_path_in, mesh_index_in, mesh_bounding_box_type_in,
                  asset_id),
              load_result = false;
              future->cancel(); return;);
          continue;
        }
        load_result = false;
        future->cancel();
        return;
      }
      if (result == Utils::ExecuteResult::SUCCESS) {
        return;
      } else {
        load_result = false;
        future->cancel();
      }
    });
  }

  return Utils::ExecuteResult ::SUCCESS;
}

Combination::Combination(
    std::vector<AssetManager::AssetHandler>&& asset_handlers)
    : asset_handlers_(std::move(asset_handlers)) {}

std::vector<std::pair<void*, std::uint64_t>> Combination::GetDatas() {
  std::vector<std::pair<void*, std::uint64_t>> datas;
  datas.reserve(asset_handlers_.size());
  for (auto& asset_handler : asset_handlers_) {
    for (auto sub_datas : asset_handler.GetAsset().GetDatas()) {
      datas.emplace_back(sub_datas);
    }
  }

  return datas;
}

std::vector<std::pair<const void*, std::uint64_t>> Combination::GetDatas()
    const {
  std::vector<std::pair<const void*, std::uint64_t>> datas;
  datas.reserve(asset_handlers_.size());
  for (const auto& asset_handler : asset_handlers_) {
    for (auto sub_datas : asset_handler.GetAsset().GetDatas()) {
      datas.emplace_back(sub_datas);
    }
  }

  return datas;
}

std::uint64_t Combination::GetSize() const {
  std::uint64_t size;
  for (const auto& handler : asset_handlers_) {
    size += handler.GetAsset().GetSize();
  }

  return size;
}

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