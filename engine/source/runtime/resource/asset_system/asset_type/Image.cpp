#include "base/asset_base.h"
#define STB_IMAGE_IMPLEMENTATION
#include "runtime/resource/asset_system/asset_type/Image.h"

std::uint32_t MM::AssetSystem::AssetType::GetImageFormatSize(
    MM::AssetSystem::AssetType::ImageFormat image_format) {
  switch (image_format) {
    case ImageFormat::UNDEFINED:
      return 0;
    case ImageFormat::GREY:
      return 8;
    case ImageFormat::GREY_ALPHA:
      return 16;
    case ImageFormat::RGB:
      return 24;
    case ImageFormat::RGB_ALPHA:
      return 32;
  }

  return 0;
}

void MM::AssetSystem::AssetType::Image::StbiImageFree::operator()(
    void* retval_from_stbi_load) const {
  stbi_image_free(retval_from_stbi_load);
}

void MM::AssetSystem::AssetType::Image::ImageInfo::Reset() {
  image_width_ = 0;
  image_height_ = 0;
  original_image_channels_ = 0;
  image_channels_ = 0;
  image_size_ = 0;
  image_format_ = ImageFormat::UNDEFINED;
}

MM::AssetSystem::AssetType::Image::Image(const FileSystem::Path& image_path,
                                         std::uint32_t desired_channels)
    : AssetBase(image_path) {
  assert(desired_channels != 0 && desired_channels < 5);
  if (!AssetBase::IsValid()) {
    MM_LOG_ERROR(std::string("Failed to load the image with path ") +
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
    MM_LOG_ERROR(std::string("Failed to load the image with path ") +
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
      MM_LOG_ERROR(std::string("Failed to load the image with path ") +
                   image_path.String() +
                   ",because this format are not supported.");
  }
  SetAssetID(GetAssetID() + image_info_.image_channels_);
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

std::uint32_t MM::AssetSystem::AssetType::Image::GetDepth() const { return 1; }

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

void MM::AssetSystem::AssetType::Image::Swap(Image& lhs, Image& rhs) noexcept {
  using std::swap;
  swap(static_cast<AssetBase&>(lhs), static_cast<AssetBase&>(rhs));
  swap(lhs.image_info_, rhs.image_info_);
  swap(lhs.image_pixels_, rhs.image_pixels_);
}

void MM::AssetSystem::AssetType::Image::swap(Image& lhs, Image& rhs) noexcept {
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

MM::Result<MM::Utils::Json::Document, MM::ErrorResult>
MM::AssetSystem::AssetType::Image::GetJson() const {
  Result<MM::Utils::Json::Document> document_result = AssetBase::GetJson();
  if (document_result.IsError()) {
    return document_result;
  }

  Utils::Json::Document& document = document_result.GetResult();
  auto& allocator = document.GetAllocator();

  document.AddMember("asset id", GetAssetID(), allocator);
  document.AddMember("image width", GetImageWidth(), allocator);
  document.AddMember("image height", GetImageHeight(), allocator);
  document.AddMember("image channels", GetImageChannels(), allocator);
  document.AddMember("image size", GetImageSize(), allocator);
  switch (GetImageFormat()) {
    case ImageFormat::UNDEFINED:
      return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
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

  return document_result;
}

std::uint32_t MM::AssetSystem::AssetType::Image::GetOriginalImageChannels()
    const {
  return image_info_.original_image_channels_;
}

MM::Result<MM::AssetSystem::AssetType::AssetID, MM::ErrorResult>
MM::AssetSystem::AssetType::Image::CalculateAssetID(
    const MM::FileSystem::Path& path, std::uint32_t desired_channels) {
  MM::Result<FileSystem::LastWriteTime, ErrorResult> last_write_time =
      MM_FILE_SYSTEM->GetLastWriteTime(path);
  last_write_time.Exception(
      MM_ERROR_DESCRIPTION(Failed to get last write time.));
  if (last_write_time.IsError()) {
    return ResultE<ErrorResult>{ErrorCode::FILE_OPERATION_ERROR};
  }

  AssetID asset_ID = (path.GetHash() ^
                      last_write_time.GetResult().time_since_epoch().count()) +
                     desired_channels;
  return ResultS{asset_ID};
}

std::vector<std::pair<void*, std::uint64_t>>
MM::AssetSystem::AssetType::Image::GetDatas() {
  return std::vector<std::pair<void*, std::uint64_t>>{
      std::pair<void*, std::uint64_t>(image_pixels_.get(),
                                      image_info_.image_size_)};
}

std::vector<std::pair<const void*, std::uint64_t>>
MM::AssetSystem::AssetType::Image::GetDatas() const {
  return std::vector<std::pair<const void*, std::uint64_t>>{
      std::pair<const void*, std::uint64_t>(image_pixels_.get(),
                                            image_info_.image_size_)};
}

std::uint64_t MM::AssetSystem::AssetType::Image::GetSize() const {
  return GetImageSize();
}

std::uint32_t MM::AssetSystem::AssetType::Image::GetImageFormatSize() const {
  return MM::AssetSystem::AssetType::GetImageFormatSize(GetImageFormat());
}
