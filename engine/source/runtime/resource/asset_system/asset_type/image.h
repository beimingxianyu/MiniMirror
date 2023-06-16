#pragma once

#include "runtime/resource/asset_system/asset_type/base/asset_base.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
class Image : public AssetBase {
 public:
  struct StbiImageFree {
    void operator()(void* retval_from_stbi_load) const {
      stbi_image_free(retval_from_stbi_load);
    }
  };

  struct ImageInfo {
    int image_width_{0};
    int image_height_{0};
    int image_channels_{0};
    uint64_t image_size_{0};
    ImageFormat image_format_{ImageFormat::UNDEFINED};

    void Reset() {
      image_width_ = 0;
      image_height_ = 0;
      image_channels_ = 0;
      image_size_ = 0;
      image_format_ = ImageFormat::UNDEFINED;
    }
  };

 public:
  ~Image() override = default;
  explicit Image(const FileSystem::Path& image_path,
                 int desired_channels = STBI_rgb_alpha);
  Image(const std::string& asset_name, AssetID asset_id,
        const ImageInfo& image_info,
        std::unique_ptr<stbi_uc, StbiImageFree>&& image_pixels);
  Image(const Image& other) = delete;
  Image(Image&& other) noexcept;
  Image& operator=(const Image& other) = delete;
  Image& operator=(Image&& other) noexcept;

 public:
  const int& GetImageWidth() const;

  const int& GetImageHeight() const;

  const int& GetImageChannels() const;

  const uint64_t& GetImageSize() const;

  const stbi_uc& GetImagePixels() const;

  const ImageFormat& GetImageFormat() const;

  friend void Swap(Image& lhs, Image& rhs) noexcept;

  friend void swap(Image& lhs, Image& rhs) noexcept;

  bool IsValid() const override;

  AssetType GetAssetType() const override;

  const void* GetPixels() const;

  void Release() override;

 private:
  ImageInfo image_info_{};
  std::unique_ptr<stbi_uc, StbiImageFree> image_pixels_{nullptr};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
