#pragma once

#include <stb_image.h>

#include <cstdint>

#include "runtime/platform/base/error.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/resource/asset_system/asset_type/base/asset_base.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
std::uint32_t GetImageFormatSize(ImageFormat image_format);

class Image : public AssetBase {
 public:
  struct StbiImageFree {
    void operator()(void* retval_from_stbi_load) const;
  };

  struct ImageInfo {
    std::uint32_t image_width_{0};
    std::uint32_t image_height_{0};
    std::uint32_t original_image_channels_{0};
    std::uint32_t image_channels_{0};
    std::uint64_t image_size_{0};
    ImageFormat image_format_{ImageFormat::UNDEFINED};

    void Reset();
  };

 public:
  ~Image() override = default;
  explicit Image(const FileSystem::Path& image_path,
                 std::uint32_t desired_channels = STBI_rgb_alpha);
  Image(const FileSystem::Path& asset_path, AssetID asset_id,
        const ImageInfo& image_info,
        std::unique_ptr<stbi_uc, StbiImageFree>&& image_pixels);
  Image(const Image& other) = delete;
  Image(Image&& other) noexcept;
  Image& operator=(const Image& other) = delete;
  Image& operator=(Image&& other) noexcept;

 public:
  std::uint32_t GetImageWidth() const;

  std::uint32_t GetImageHeight() const;

  std::uint32_t GetDepth() const;

  std::uint32_t GetImageChannels() const;

  std::uint32_t GetOriginalImageChannels() const;

  std::uint64_t GetImageSize() const;

  const stbi_uc& GetImagePixels() const;

  ImageFormat GetImageFormat() const;

  std::uint32_t GetImageFormatSize() const;

  std::string GetAssetTypeString() const override;

  ExecuteResult GetJson(rapidjson::Document& document) const override;

  friend void Swap(Image& lhs, Image& rhs) noexcept;

  friend void swap(Image& lhs, Image& rhs) noexcept;

  std::uint64_t GetSize() const override;

  std::vector<std::pair<void*, std::uint64_t>> GetDatas() override;

  std::vector<std::pair<const void*, std::uint64_t>> GetDatas() const override;

  bool IsValid() const override;

  AssetType GetAssetType() const override;

  const void* GetPixelsData() const;

  static ExecuteResult CalculateAssetID(const FileSystem::Path& path,
                                        std::uint32_t desired_channels,
                                        AssetID& asset_ID);

  void Release() override;

 private:
  ImageInfo image_info_{};
  std::unique_ptr<stbi_uc, StbiImageFree> image_pixels_{nullptr};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
