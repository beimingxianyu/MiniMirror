#include "runtime/resource/asset_system/asset_type/image.h"

MM::AssetSystem::AssetType::Image::Image(const FileSystem::Path& image_path,
                                         int desired_channels)
    : AssetBase(image_path) {
  if (!AssetBase::IsValid()) {
    LOG_ERROR(std::string("Failed to load the image with path ") +
              image_path.String() + ",because the file does not exist.");
  }

  image_pixels_.reset(
      stbi_load(image_path.String().c_str(), &image_info_.image_width_,
                &image_info_.image_height_, &image_info_.image_channels_,
                desired_channels));
  if (!image_pixels_) {
    image_info_.image_width_ = 0;
    image_info_.image_height_ = 0;
    image_info_.image_channels_ = 0;
    image_pixels_.reset();
    LOG_ERROR(std::string("Failed to load the image with path ") +
              image_path.String());
  }
  switch (image_info_.image_channels_) {
    case 1:
      image_info_.image_format_ = ImageFormat::GREY;
      break;
    case 2:
      image_info_.image_format_ = ImageFormat::GREY_ALPHA;
      break;
    case 3:
      image_info_.image_format_ = ImageFormat::RGB;
      break;
    case 4:
      image_info_.image_format_ = ImageFormat::RGB_ALPHA;
      break;
    default:
      image_info_.image_width_ = 0;
      image_info_.image_height_ = 0;
      image_info_.image_channels_ = 0;
      image_pixels_.reset();
      LOG_ERROR(std::string("Failed to load the image with path ") +
                image_path.String() +
                ",because this format are not supported.");
  }
  image_info_.image_size_ = static_cast<uint64_t>(image_info_.image_width_) *
                            static_cast<uint64_t>(image_info_.image_height_) *
                            static_cast<uint64_t>(image_info_.image_channels_);
}

MM::AssetSystem::AssetType::Image::Image(Image&& other) noexcept
    : AssetBase(std::move(other)),
      image_info_(std::move(other.image_info_)),
      image_pixels_(std::move(other.image_pixels_)) {
  other.image_info_.Reset();
}

MM::AssetSystem::AssetType::Image& MM::AssetSystem::AssetType::Image::operator=(
    Image&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  AssetBase::operator=(std::move(other));
  image_info_ = std::move(other.image_info_);
  image_pixels_ = std::move(other.image_pixels_);

  other.image_info_.Reset();

  return *this;
}

const int& MM::AssetSystem::AssetType::Image::GetImageWidth() const {
  return image_info_.image_width_;
}

const int& MM::AssetSystem::AssetType::Image::GetImageHeight() const {
  return image_info_.image_height_;
}

const int& MM::AssetSystem::AssetType::Image::GetImageChannels() const {
  return image_info_.image_channels_;
}

const uint64_t& MM::AssetSystem::AssetType::Image::GetImageSize() const {
  return image_info_.image_size_;
}

const stbi_uc& MM::AssetSystem::AssetType::Image::GetImagePixels() const {
  return *image_pixels_;
}

const MM::AssetSystem::AssetType::ImageFormat&
MM::AssetSystem::AssetType::Image::GetImageFormat() const {
  return image_info_.image_format_;
}

bool MM::AssetSystem::AssetType::Image::IsValid() const {
  return image_pixels_ != nullptr;
}

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::Image::GetAssetType() const {
  return AssetType::IMAGE;
}

void MM::AssetSystem::AssetType::Image::Release() {
  image_info_.Reset();
  image_pixels_.reset();
}

void MM::AssetSystem::AssetType::Swap(Image& lhs, Image& rhs) noexcept {
  using std::swap;
  swap(static_cast<AssetBase&>(lhs), static_cast<AssetBase&>(rhs));
  swap(lhs.image_info_, rhs.image_info_);
  swap(lhs.image_pixels_, rhs.image_pixels_);
}

void MM::AssetSystem::AssetType::swap(Image& lhs, Image& rhs) noexcept {
  using std::swap;
  swap(static_cast<AssetBase&>(lhs), static_cast<AssetBase&>(rhs));
  swap(lhs.image_info_, rhs.image_info_);
  swap(lhs.image_pixels_, rhs.image_pixels_);
}

const void* MM::AssetSystem::AssetType::Image::GetPixels() const {
  return image_pixels_.get();
}

MM::AssetSystem::AssetType::Image::Image(
    const std::string& asset_name, MM::AssetSystem::AssetType::AssetID asset_id,
    const MM::AssetSystem::AssetType::Image::ImageInfo& image_info,
    std::unique_ptr<stbi_uc, StbiImageFree>&& image_pixels)
    : AssetBase(asset_name, asset_id),
      image_info_(image_info),
      image_pixels_(std::move(image_pixels)) {}
