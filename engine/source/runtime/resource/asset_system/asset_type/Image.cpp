#include "base/asset_base.h"
#define STB_IMAGE_IMPLEMENTATION
#include "runtime/resource/asset_system/asset_type/Image.h"

MM::AssetSystem::AssetType::Image::Image(const FileSystem::Path& image_path,
                                         std::uint32_t desired_channels)
    : AssetBase(image_path) {
  assert(desired_channels != 0 && desired_channels < 5);
  if (!AssetBase::IsValid()) {
    LOG_ERROR(std::string("Failed to load the image with path ") +
              image_path.StringView().data() +
              ",because the file does not exist.");
    return;
  }

  int image_width, image_height, image_channels;
  image_pixels_.reset(stbi_load(image_path.String().c_str(), &image_width,
                                &image_height, &image_channels,
                                desired_channels));
  if (!image_pixels_) {
    image_info_.image_width_ = 0;
    image_info_.image_height_ = 0;
    image_info_.image_channels_ = 0;
    image_pixels_.reset();
    LOG_ERROR(std::string("Failed to load the image with path ") +
              image_path.String());
  }
  image_info_.image_channels_ = desired_channels;
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
  image_info_.image_width_ = image_width;
  image_info_.image_height_ = image_height;
  image_info_.original_image_channels_ = image_channels;
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

std::uint32_t MM::AssetSystem::AssetType::Image::GetImageWidth() const {
  return image_info_.image_width_;
}

std::uint32_t MM::AssetSystem::AssetType::Image::GetImageHeight() const {
  return image_info_.image_height_;
}

std::uint32_t MM::AssetSystem::AssetType::Image::GetImageChannels() const {
  return image_info_.image_channels_;
}

std::uint64_t MM::AssetSystem::AssetType::Image::GetImageSize() const {
  return image_info_.image_size_;
}

const stbi_uc& MM::AssetSystem::AssetType::Image::GetImagePixels() const {
  return *image_pixels_;
}

MM::AssetSystem::AssetType::ImageFormat
MM::AssetSystem::AssetType::Image::GetImageFormat() const {
  return image_info_.image_format_;
}

bool MM::AssetSystem::AssetType::Image::IsValid() const {
  return AssetBase::IsValid() && image_pixels_ != nullptr;
}

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::Image::GetAssetType() const {
  return AssetType::IMAGE;
}

void MM::AssetSystem::AssetType::Image::Release() {
  image_info_.Reset();
  image_pixels_.reset();
  AssetBase::Release();
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

const void* MM::AssetSystem::AssetType::Image::GetPixelsData() const {
  return image_pixels_.get();
}

MM::AssetSystem::AssetType::Image::Image(
    const FileSystem::Path& asset_path, AssetID asset_id,
    const ImageInfo& image_info,
    std::unique_ptr<stbi_uc, StbiImageFree>&& image_pixels)
    : AssetBase(asset_path, asset_id),
      image_info_(image_info),
      image_pixels_(std::move(image_pixels)) {}

std::string MM::AssetSystem::AssetType::Image::GetAssetTypeString() const {
  return std::string(MM_ASSET_TYPE_IMAGE);
}

MM::ExecuteResult MM::AssetSystem::AssetType::Image::GetJson(
    rapidjson::Document& document) const {
  if (!document.IsObject()) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }
  if (!(document.MemberBegin() == document.MemberEnd())) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  auto& allocator = document.GetAllocator();
  document.AddMember("name",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetName().c_str()),
                     allocator);
  document.AddMember("path",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetPath().StringView().data()),
                     allocator);
  document.AddMember("asset id", GetAssetID(), allocator);
  document.AddMember("image width", GetImageWidth(), allocator);
  document.AddMember("image height", GetImageHeight(), allocator);
  document.AddMember("image channels", GetImageChannels(), allocator);
  document.AddMember("image size", GetImageSize(), allocator);
  switch (GetImageFormat()) {
    case ImageFormat::UNDEFINED:
      assert(false);
      break;
    case ImageFormat::GREY:
      document.AddMember("image format", "GREY", allocator);
      break;
    case ImageFormat::GREY_ALPHA:
      document.AddMember("image format", "GREY_ALPHA", allocator);
      break;
    case ImageFormat::RGB:
      document.AddMember("image format", "RGB", allocator);
      break;
    case ImageFormat::RGB_ALPHA:
      document.AddMember("image format", "RGB_ALPHA", allocator);
      break;
  }

  return ExecuteResult ::SUCCESS;
}

std::uint32_t MM::AssetSystem::AssetType::Image::GetOriginalImageChannels()
    const {
  return image_info_.original_image_channels_;
}
