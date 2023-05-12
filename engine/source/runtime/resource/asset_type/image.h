#pragma once

#include "runtime/resource/asset_type/base/asset_base.h"

namespace MM {
namespace AssetType {
class Image : public AssetBase {
 public:
  Image() = default;
  ~Image() override = default;
  Image(const std::string& asset_name, const FileSystem::Path& image_path,
        const int& desired_channels = STBI_rgb_alpha);
  Image(const Image& other) = default;
  Image(Image&& other) noexcept;
  Image& operator=(const Image& other);
  Image& operator=(Image&& other) noexcept;

 public:
  const int& GetImageWidth() const;

  const int& GetImageHeight() const;

  const int& GetImageChannels() const;

  const uint64_t& GetImageSize() const;

  std::shared_ptr<const stbi_uc> GetImagePixels() const;

  const ImageFormat& GetImageFormat() const;

  friend void Swap(Image& lhs, Image& rhs) noexcept;

  friend void swap(Image& lhs, Image& rhs) noexcept;

  bool IsValid() const override;

  AssetType GetAssetType() const override;

  const void* GetData() const override;

  void Release() override;

 private:
  int image_width_{0};
  int image_height_{0};
  int image_channels_{0};
  uint64_t image_size_{0};
  ImageFormat image_format_{ImageFormat::UNDEFINED};
  std::shared_ptr<stbi_uc> image_pixels_{nullptr, stbi_image_free};
};
}  // namespace AssetType
}  // namespace MM
