//
// Created by beimingxianyu on 23-7-21.
//

#include "runtime/resource/asset_system/asset_type/Shader.h"

MM::AssetSystem::AssetType::Shader::Shader(
    const MM::FileSystem::Path &shader_path)
    : AssetBase(shader_path) {
  if (!AssetBase::IsValid()) {
    MM_LOG_ERROR(std::string("Failed to load the mesh with path ") +
                 shader_path.StringView().data() +
                 ",because the file does not exist.");
    return;
  }

  LoadShader(shader_path);

  if (!(AssetBase::IsValid() && size_ != 0 && !data_.empty())) {
    AssetBase::Release();
    return;
  }
}

const std::vector<char> &MM::AssetSystem::AssetType::Shader::GetShaderData()
    const {
  return data_;
}

bool MM::AssetSystem::AssetType::Shader::IsValid() const {
  return AssetBase::IsValid() && size_ != 0 && !data_.empty();
}

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::Shader::GetAssetType() const {
  return AssetType::SHADER;
}

std::string MM::AssetSystem::AssetType::Shader::GetAssetTypeString() const {
  return MM_ASSET_TYPE_SHADER;
}

uint64_t MM::AssetSystem::AssetType::Shader::GetSize() const { return size_; }

MM::ExecuteResult MM::AssetSystem::AssetType::Shader::GetJson(
    rapidjson::Document &document) const {
  // TODO complete
  // name
  // path
  // asset id
  // bin data path
  return AssetBase::GetJson(document);
}

std::vector<std::pair<void *, std::uint64_t>>
MM::AssetSystem::AssetType::Shader::GetDatas() {
  return std::vector<std::pair<void *, std::uint64_t>>{
      std::make_pair(data_.data(), size_)};
}

std::vector<std::pair<const void *, std::uint64_t>>
MM::AssetSystem::AssetType::Shader::GetDatas() const {
  return std::vector<std::pair<const void *, std::uint64_t>>{
      std::make_pair(data_.data(), size_)};
}

void MM::AssetSystem::AssetType::Shader::Release() {
  AssetBase::Release();
  size_ = 0;
  data_.clear();
}

MM::ExecuteResult MM::AssetSystem::AssetType::Shader::LoadShader(
    const MM::FileSystem::Path &shader_path) {
  std::string cmd { MM_CAT() }
  int result = std::system();
  if (result != 0) {
  }

  MM_CHECK(MM_FILE_SYSTEM->ReadFile(shader_path, data_),
           MM_LOG_ERROR("Failed to read shader data.");
           return MM_RESULT_CODE;)

  return MM::ExecuteResult::SUCCESS;
}
