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

  MM_CHECK(LoadShader(shader_path), MM_LOG_ERROR("Failed to load shader.");
           AssetBase::Release();)
}

MM::AssetSystem::AssetType::Shader::Shader(
    MM::AssetSystem::AssetType::Shader &&other) noexcept
    : AssetBase(std::move(other)),
      size_(other.size_),
      data_(std::move(other.data_)) {
  other.size_ = 0;
}

MM::AssetSystem::AssetType::Shader &
MM::AssetSystem::AssetType::Shader::operator=(
    MM::AssetSystem::AssetType::Shader &&other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  AssetBase::operator=(std::move(other));
  size_ = other.size_;
  data_ = std::move(data_);

  other.size_ = 0;

  return *this;
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
  MM_CHECK(MM_FILE_SYSTEM->ReadFile(shader_path, data_),
           MM_LOG_ERROR("Failed to read shader data.");
           return MM_RESULT_CODE;)

  Utils::ShadercShaderKind kind;
  MM_CHECK(ChooseShaderKind(shader_path, kind),
           MM_LOG_ERROR("The file extension is error.");
           return MM_RESULT_CODE;)

  MM_CHECK(Utils::CompileShader(data_, shader_path.CStr(), std::string("main"),
                                kind, data_.data(), data_.size(), true,
                                shaderc_optimization_level_performance),
           MM_LOG_ERROR("Failed to compile shader.");
           return MM_RESULT_CODE;)

  return MM::ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::AssetSystem::AssetType::Shader::ChooseShaderKind(
    const MM::FileSystem::Path &shader_path,
    MM::Utils::ShadercShaderKind &output_kind) {
  std::string_view sub_fix = shader_path.GetExtensionView();
  if (sub_fix.empty()) {
    return ExecuteResult ::FILE_OPERATION_ERROR;
  }

  if (sub_fix.compare(".vert") == 0) {
    output_kind = shaderc_vertex_shader;
    return Utils::ExecuteResult ::SUCCESS;
  }
  if (sub_fix.compare(".frag") == 0) {
    output_kind = shaderc_fragment_shader;
    return Utils::ExecuteResult ::SUCCESS;
  }
  if (sub_fix.compare(".comp") == 0) {
    output_kind = shaderc_compute_shader;
    return Utils::ExecuteResult ::SUCCESS;
  }
  if (sub_fix.compare(".geom") == 0) {
    output_kind = shaderc_geometry_shader;
    return Utils::ExecuteResult ::SUCCESS;
  }
  if (sub_fix.compare(".tesc") == 0) {
    output_kind = shaderc_tess_control_shader;
    return Utils::ExecuteResult ::SUCCESS;
  }
  if (sub_fix.compare(".tese") == 0) {
    output_kind = shaderc_tess_evaluation_shader;
    return Utils::ExecuteResult ::SUCCESS;
  }

  return ExecuteResult ::FILE_OPERATION_ERROR;
}
