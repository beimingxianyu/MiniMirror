//
// Created by beimingxianyu on 23-7-21.
//

#include "runtime/platform/base/error.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/core/log/exception_description.h"
#include "runtime/resource/asset_system/asset_type/base/asset_base.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "utils/shaderc.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {

class Shader : public AssetBase {
 public:
  Shader() = default;
  ~Shader() = default;
  explicit Shader(const FileSystem::Path& shader_path);
  Shader(const Shader& other) = delete;
  Shader(Shader&& other) noexcept;
  Shader& operator=(const Shader& other) = delete;
  Shader& operator=(Shader&& other) noexcept;

 public:
  const std::vector<char>& GetShaderData() const;

  FileSystem::Path GetBinPath() const;

  bool IsValid() const override;

  AssetType GetAssetType() const override;

  std::string GetAssetTypeString() const override;

  uint64_t GetSize() const override;

  virtual Result<MM::Utils::Json::Document, MM::ErrorResult> GetJson() const override;

  std::vector<std::pair<void*, std::uint64_t>> GetDatas() override;

  std::vector<std::pair<const void*, std::uint64_t>> GetDatas() const override;

  void Release() override;

 private:
  static Result<Utils::ShadercShaderKind, ErrorResult> ChooseShaderKind(
      const MM::FileSystem::Path& shader_path,
      MM::Utils::ShadercShaderKind& output_kind);

  Result<Nil, ErrorResult> LoadShader(const FileSystem::Path& shader_path);

  Result<Nil, ErrorResult> SaveCompiledShaderToFile(
      MM::FileSystem::Path compiled_path) const;

 private:
  std::uint64_t size_{0};
  std::vector<char> data_{};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
