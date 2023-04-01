#include "runtime/resource/asset_type/image.h"

MM::AssetType::Image::Image(const std::string& asset_name,
                            const FileSystem::Path& image_path,
                            const int& desired_channels)
    : AssetBase(asset_name, MM::Utils::StringHash(image_path.RelativePath(
                                config_system->GetConfig("bin_dir")))) {
  if (!image_path.Exists()) {
    LOG_ERROR(std::string("Failed to load the image with path ") +
              image_path.String() + ",because the file does not exist.");
  }
  image_pixels_ = std::shared_ptr<stbi_uc>{
      stbi_load(image_path.String().c_str(), &image_width_, &image_height_,
                &image_channels_, desired_channels),
      stbi_image_free};
  if (!image_pixels_) {
    image_width_ = 0;
    image_height_ = 0;
    image_channels_ = 0;
    image_pixels_.reset();
    LOG_ERROR(std::string("Failed to load the image with path ") +
              image_path.String());
  }
  switch (image_channels_) {
    case 1:
      image_format_ = ImageFormat::GREY;
      break;
    case 2:
      image_format_ = ImageFormat::GREY_ALPHA;
      break;
    case 3:
      image_format_ = ImageFormat::RGB;
      break;
    case 4:
      image_format_ = ImageFormat::RGB_ALPHA;
      break;
    default:
      image_width_ = 0;
      image_height_ = 0;
      image_channels_ = 0;
      image_pixels_.reset();
      LOG_ERROR(std::string("Failed to load the image with path ") +
                image_path.String() +
                ",because this format are not supported.");
  }
  image_size_ = static_cast<uint64_t>(image_width_) *
                static_cast<uint64_t>(image_height_) *
                static_cast<uint64_t>(image_channels_);
}

MM::AssetType::Image::Image(Image&& other) noexcept
  : image_width_(other.image_width_),
    image_height_(other.image_height_),
    image_channels_(other.image_channels_),
    image_size_(other.image_size_),
    image_format_(other.image_format_),
    image_pixels_(std::move(image_pixels_)) {
  other.image_width_ = 0;
  other.image_height_ = 0;
  other.image_channels_ = 0;
  other.image_size_ = 0;
  image_format_ = ImageFormat::UNDEFINED;
}

MM::AssetType::Image& MM::AssetType::Image::operator=(
    const MM::AssetType::Image& other) {
  if (&other == this) {
    return *this;
  }
  AssetBase::operator=(other);
  image_width_ = other.image_width_;
  image_height_ = other.image_height_;
  image_channels_ = other.image_channels_;
  image_size_ = other.image_size_;
  image_format_ = other.image_format_;
  image_pixels_ = other.image_pixels_;

  return *this;
}

MM::AssetType::Image& MM::AssetType::Image::operator=(Image&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  AssetBase::operator=(std::move(other));
  image_width_ = other.image_width_;
  image_height_ = other.image_height_;
  image_channels_ = other.image_channels_;
  image_size_ = other.image_size_;
  image_format_ = other.image_format_;
  image_pixels_ = std::move(other.image_pixels_);

  other.image_width_ = 0;
  other.image_height_ = 0;
  other.image_channels_ = 0;
  other.image_size_ = 0;
  image_format_ = ImageFormat::UNDEFINED;

  return *this;
}

const int& MM::AssetType::Image::GetImageWidth() const { return image_width_; }

const int& MM::AssetType::Image::GetImageHeight() const {
  return image_height_;
}

const int& MM::AssetType::Image::GetImageChannels() const {
  return image_channels_;
}

const uint64_t& MM::AssetType::Image::GetImageSize() const {
  return image_size_;
}

std::shared_ptr<const stbi_uc> MM::AssetType::Image::GetImagePixels() const {
  return image_pixels_;
}

const MM::AssetType::ImageFormat& MM::AssetType::Image::GetImageFormat() const {
  return image_format_;
}

bool MM::AssetType::Image::IsValid() const { return image_pixels_ != nullptr; }

MM::AssetType::AssetType MM::AssetType::Image::GetAssetType() {
  return AssetType::IMAGE;
}

void MM::AssetType::Image::Release() {
  image_width_ = 0;
  image_height_ = 0;
  image_channels_ = 0;
  image_size_ = 0;
  image_format_ = ImageFormat::UNDEFINED;
  image_pixels_.reset();
}

void MM::AssetType::Swap(Image& lhs, Image& rhs) noexcept {
  using std::swap;
  swap(static_cast<AssetBase&>(lhs), static_cast<AssetBase&>(rhs));
  swap(lhs.image_width_, rhs.image_width_);
  swap(lhs.image_height_, rhs.image_height_);
  swap(lhs.image_channels_, rhs.image_channels_);
  swap(lhs.image_size_, rhs.image_size_);
  swap(lhs.image_format_, rhs.image_format_);
  swap(lhs.image_pixels_, rhs.image_pixels_);
}

void MM::AssetType::swap(Image& lhs, Image& rhs) noexcept {
  using std::swap;
  swap(static_cast<AssetBase&>(lhs), static_cast<AssetBase&>(rhs));
  swap(lhs.image_width_, rhs.image_width_);
  swap(lhs.image_height_, rhs.image_height_);
  swap(lhs.image_channels_, rhs.image_channels_);
  swap(lhs.image_size_, rhs.image_size_);
  swap(lhs.image_format_, rhs.image_format_);
  swap(lhs.image_pixels_, rhs.image_pixels_);
}
