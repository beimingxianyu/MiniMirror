//
// Created by beimingxianyu on 23-9-30.
//

#include "runtime/resource/asset_system/asset_type/base/asset_base.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {

class Common : public AssetBase {
 public:
  Common() = default;
  ~Common() = default;
  explicit Common(
      const FileSystem::Path& path, std::uint64_t offset = 0,
      std::uint64_t size = UINT64_MAX,
      const std::function<Result<std::vector<std::vector<char>>, ErrorResult>(
          const FileSystem::Path&, std::uint64_t, std::uint64_t)>&
          load_function = DefaultLoadFunction);
  Common(const Common& other) = delete;
  Common(Common&& other) noexcept;
  Common& operator=(const Common& other) = delete;
  Common& operator=(Common&& other) noexcept;

 public:
  AssetType GetAssetType() const override;

  std::string GetAssetTypeString() const override;

  std::uint64_t GetSize() const override;

  Result<MM::Utils::Json::Document, MM::ErrorResult> GetJson() const override;

  std::vector<std::pair<const void*, std::uint64_t>> GetDatas() const override;

  void Release() override;

 private:
  static Result<std::vector<std::vector<char>>, ErrorResult>
  DefaultLoadFunction(const FileSystem::Path& path, std::uint64_t offset,
                      std::uint64_t size);

 private:
  std::vector<std::vector<char>> data_;
};

}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM