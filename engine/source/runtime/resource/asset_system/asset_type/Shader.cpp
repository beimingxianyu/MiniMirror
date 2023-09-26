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

  LoadShader(shader_path)
      .Exception([function_name = MM_FUNCTION_NAME,
                  this_object = this](ErrorResult error_result) {
        MM_LOG_SYSTEM->CheckResult(
            error_result.GetErrorCode(),
            MM_LOG_DESCRIPTION_MESSAGE(function_name, "Failed to load shader."),
            LogSystem::LogSystem::LogLevel::ERROR);

        this_object->AssetBase::Release();
      });
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

MM::Result<MM::Utils::Json::Document, MM::ErrorResult>
MM::AssetSystem::AssetType::Shader::GetJson() const {
  if (!IsValid()) {
    return Result<Utils::Json::Document, ErrorResult>(
        st_execute_error, MM::ErrorCode::OBJECT_IS_INVALID);
  }

  Result<FileSystem::Path, ErrorResult> bin_path = GetBinPath();
  bin_path.Exception(MM_ERROR_DESCRIPTION(Failed to get bin path.));
  if (!bin_path.IsSuccess()) {
    return Result<Utils::Json::Document, ErrorResult>(st_execute_error,
                                                      bin_path.GetError());
  }

  Utils::Json::Document document{};
  document.SetObject();
  auto &allocator = document.GetAllocator();
  document.AddMember("name",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetName().c_str()),
                     allocator);
  document.AddMember("path",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetPath().StringView().data()),
                     allocator);
  document.AddMember("asset id",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         std::to_string(GetAssetID()).c_str()),
                     allocator);
  document.AddMember("bin data path",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetBinPath().GetResult().CStr()),
                     allocator);

  return Result<Utils::Json::Document, ErrorResult>(st_execute_success,
                                                    std::move(document));
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

MM::Result<MM::Nil, MM::ErrorResult>
MM::AssetSystem::AssetType::Shader::LoadShader(
    const MM::FileSystem::Path &shader_path) {
  Result<FileSystem::Path, ErrorResult> compiled_shader_path = GetBinPath();
  compiled_shader_path.Exception(
      MM_ERROR_DESCRIPTION(Filed to get shander bin path.));
  if (!compiled_shader_path.IsSuccess()) {
    return Result<Nil, ErrorResult>(st_execute_error,
                                    compiled_shader_path.GetError());
  }

  if (compiled_shader_path.GetResult().IsExists()) {
    Result<std::vector<char>, ErrorResult> read_result =
        MM_FILE_SYSTEM->ReadFile(compiled_shader_path.GetResult());
    read_result.Exception(
        MM_ERROR_DESCRIPTION(Failed to read data from compiled file.));
    if (read_result.IsSuccess()) {
      data_ = std::move(read_result.GetResult());
      return Result<Nil, ErrorResult>{st_execute_success};
    }
  }

  Result<std::vector<char>, ErrorResult> shader_read_result =
      MM_FILE_SYSTEM->ReadFile(compiled_shader_path.GetResult());
  shader_read_result.Exception(
      MM_ERROR_DESCRIPTION(Failed to read shader data.));
  if (!shader_read_result.IsSuccess()) {
    return Result<Nil, ErrorResult>{st_execute_error,
                                    shader_read_result.GetError()};
  }

  Result<Utils::ShadercShaderKind, ErrorResult> kind =
      ChooseShaderKind(shader_path);
  kind.Exception(MM_ERROR_DESCRIPTION(The file extension is error.));
  if (!kind.IsSuccess()) {
    return Result<Nil, ErrorResult>{st_execute_error, kind.GetError()};
  }

  Result<std::vector<char>, ErrorResult> compiled_result = Utils::CompileShader(
      shader_path.CStr(), std::string("main"), kind.GetResult(),
      shader_read_result.GetResult().data(),
      shader_read_result.GetResult().size(), true,
      shaderc_optimization_level_performance);
  compiled_result.Exception(MM_ERROR_DESCRIPTION(Failed to compile shader.));
  if (compiled_result.IsError()) {
    return Result<Nil, ErrorResult>{st_execute_error,
                                    compiled_result.GetError()};
  }

  SaveCompiledShaderToFile(compiled_shader_path.GetResult())
      .Exception(MM_WRAN_DESCRIPTION(Failed to save compiled shader to file.));

  return Result<Nil, ErrorResult>{st_execute_success};
}

MM::Result<MM::Utils::ShadercShaderKind, MM::ErrorResult>
MM::AssetSystem::AssetType::Shader::ChooseShaderKind(
    const MM::FileSystem::Path &shader_path) {
  std::string_view sub_fix = shader_path.GetExtensionView();
  if (sub_fix.empty()) {
    return Result<Utils::ShadercShaderKind, ErrorResult>{
        st_execute_error, ErrorCode::FILE_OPERATION_ERROR};
  }

  if (sub_fix.compare(".vert") == 0) {
    return ResultS{shaderc_vertex_shader};
  }
  if (sub_fix.compare(".frag") == 0) {
    return ResultS{shaderc_fragment_shader};
  }
  if (sub_fix.compare(".comp") == 0) {
    return ResultS{shaderc_compute_shader};
  }
  if (sub_fix.compare(".geom") == 0) {
    return ResultS{shaderc_geometry_shader};
  }
  if (sub_fix.compare(".tesc") == 0) {
    return ResultS{shaderc_tess_control_shader};
  }
  if (sub_fix.compare(".tese") == 0) {
    return ResultS{shaderc_tess_evaluation_shader};
  }

  return ResultE<ErrorResult>{ErrorCode::FILE_OPERATION_ERROR};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::AssetSystem::AssetType::Shader::SaveCompiledShaderToFile(
    MM::FileSystem::Path compiled_path) const {
  static std::uint8_t rand = 0;
  ++rand;
  FileSystem::Path temp_path{compiled_path + std::to_string(rand)};

  Result<Nil, ErrorResult> create_result = MM_FILE_SYSTEM->Create(temp_path);
  create_result.Exception(
      MM_WRAN_DESCRIPTION(Failed to create temp file to save compiled shader.));
  if (create_result.IsError()) {
    return ResultE<ErrorResult>{create_result.GetError().GetErrorCode()};
  }

  std::ofstream file(temp_path.CStr(), std::ios::out | std::ios::binary);

  if (!file.is_open()) {
    return ResultE<ErrorResult>{ErrorCode::FILE_OPERATION_ERROR};
  }

  file.seekp(0);
  file.write(data_.data(), data_.size());

  file.close();

  Result<Nil, ErrorResult> rename_result =
      MM_FILE_SYSTEM->Rename(temp_path, compiled_path);
  rename_result.Exception(
      [function_name = MM_FUNCTION_NAME, &temp_path](ErrorResult error_result) {
        MM_LOG_SYSTEM->CheckResult(
            error_result.GetErrorCode(),
            MM_LOG_DESCRIPTION_MESSAGE(
                function_name, Failed to rename_result temp shader failed to
                                   target compiled path.),
            LogSystem::LogSystem::LogLevel::WARN);
        MM_FILE_SYSTEM->Delete(temp_path);
      });
  if (rename_result.IsError()) {
    return ResultE<ErrorResult>{rename_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{Nil()};
}

MM::Result<MM::FileSystem::Path, MM::ErrorResult>
MM::AssetSystem::AssetType::Shader::GetBinPath() const {
  if (!IsValid()) {
    return Result<FileSystem::Path, ErrorResult>(st_execute_error,
                                                 ErrorCode::OBJECT_IS_INVALID);
  }

  Result<FileSystem::LastWriteTime, ErrorResult> last_write_time =
      MM_FILE_SYSTEM->GetLastWriteTime(GetAssetPath());
  last_write_time.Exception(
      MM_ERROR_DESCRIPTION(Failed to get shader file last write time.));
  if (!last_write_time.IsSuccess()) {
    return Result<FileSystem::Path, ErrorResult>(st_execute_error,
                                                 last_write_time.GetError());
  }

  const FileSystem::Path &cache_dir(MM_FILE_SYSTEM->GetAssetDirCache());
  std::string asset_relative_path =
      GetAssetPath().GetRelativePath(MM_FILE_SYSTEM->GetAssetDir());
  FileSystem::Path compiled_shader_path(
      cache_dir + asset_relative_path +
      std::to_string(
          last_write_time.GetResult().time_since_epoch().count() << 26 >> 26));

  return Result<FileSystem::Path, ErrorResult>(st_execute_success,
                                               std::move(compiled_shader_path));
}
