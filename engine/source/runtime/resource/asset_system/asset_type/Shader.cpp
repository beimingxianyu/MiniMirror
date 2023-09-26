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

  LoadShader(shader_path).Exception([function_name = MM_FUNCTION_NAME] {
    MM_LOG_SYSTEM->LogError()
  });
  LoadShader(shader_path, MM_ERROR_DESCRIPTION(Failed to load shader.));
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

MM::Result<MM::Utils::Json::Document, MM::ErrorResult> MM::AssetSystem::AssetType::Shader::GetJson() const {
  if (!IsValid()) {
    return Result<Utils::Json::Document, ErrorResult>(st_execute_error, MM::ErrorCode::OBJECT_IS_INVALID);
  }

  Utils::Json::Document document{};
  auto& allocator = document.GetAllocator();
  document.AddMember("name",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetName().c_str()),
                     allocator);
  document.AddMember("path",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetPath().StringView().data()),
                     allocator);
  document.AddMember("asset id", Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(std::to_string(GetAssetID()).c_str()), allocator);
  document.AddMember("bin data path", Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(GetBinPath().CStr()), allocator);

  return Result<Utils::Json::Document, ErrorResult>(st_execute_success, &allocator);
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
  FileSystem::LastWriteTime last_write_time;
  MM_CHECK(MM_FILE_SYSTEM->GetLastWriteTime(shader_path, last_write_time),
           MM_LOG_ERROR("Failed to get shader file last write time.");
           return MM_RESULT_CODE;)

  const FileSystem::Path &cache_dir(MM_FILE_SYSTEM->GetAssetDirCache());
  std::string asset_relative_path =
      shader_path.GetRelativePath(MM_FILE_SYSTEM->GetAssetDir());
  FileSystem::Path compiled_shader_path(
      cache_dir + asset_relative_path +
      std::to_string(last_write_time.time_since_epoch().count() << 26));
  if (compiled_shader_path.IsExists()) {
    MM_CHECK(MM_FILE_SYSTEM->ReadFile(compiled_shader_path, data_),
             MM_LOG_ERROR("Failed to read shader data.");
             return MM_RESULT_CODE;)
    return ExecuteResult ::SUCCESS;
  }

  std::vector<char> shader_data;
  MM_CHECK(MM_FILE_SYSTEM->ReadFile(shader_path, shader_data),
           MM_LOG_ERROR("Failed to read shader data.");
           return MM_RESULT_CODE;)

  Utils::ShadercShaderKind kind;
  MM_CHECK(ChooseShaderKind(shader_path, kind),
           MM_LOG_ERROR("The file extension is error.");
           return MM_RESULT_CODE;)

  MM_CHECK(Utils::CompileShader(data_, shader_path.CStr(), std::string("main"),
                                kind, shader_data.data(), shader_data.size(),
                                true, shaderc_optimization_level_performance),
           MM_LOG_ERROR("Failed to compile shader.");
           return MM_RESULT_CODE;)

  SaveCompiledShaderToFile(compiled_shader_path);

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

MM::ExecuteResult MM::AssetSystem::AssetType::Shader::SaveCompiledShaderToFile(
    MM::FileSystem::Path compiled_path) const {
  static std::uint8_t rand = 0;
  ++rand;
  FileSystem::Path temp_path{compiled_path + std::to_string(rand)};

  MM_CHECK(MM_FILE_SYSTEM->Create(temp_path),
           MM_LOG_ERROR("Failed to create temp file to save compiled shader.");
           return MM_RESULT_CODE;)

  std::ofstream file(temp_path.CStr(), std::ios::out | std::ios::binary);

  if (!file.is_open()) {
    return ExecuteResult ::FILE_OPERATION_ERROR;
  }

  file.seekp(0);
  file.write(data_.data(), data_.size());

  file.close();

  MM_CHECK(MM_FILE_SYSTEM->Rename(temp_path, compiled_path),
           MM_LOG_ERROR(
               "Failed to rename temp shader failed to target compiled path.");
           MM_FILE_SYSTEM->Delete(temp_path); return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::FileSystem::Path MM::AssetSystem::AssetType::Shader::GetBinPath() const {
  if (!IsValid()) {
    return MM::FileSystem::Path{""};
  }


}
