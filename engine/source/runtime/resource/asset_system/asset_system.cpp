#include "runtime/resource/asset_system/asset_system.h"

std::shared_ptr<MM::AssetSystem::AssetManager>
    MM::AssetSystem::AssetManager::asset_manager_{nullptr};
std::atomic_uint32_t MM::AssetSystem::AssetManager::increase_ID_{0};
std::mutex MM::AssetSystem::AssetManager::sync_flag_{};
std::set<std::string> MM::AssetSystem::AssetManager::support_image_format{
    {"jpg", "png"}};

std::shared_ptr<MM::AssetSystem::AssetSystem>
    MM::AssetSystem::AssetSystem::asset_system_{nullptr};
std::mutex MM::AssetSystem::AssetSystem::sync_flag_{};

bool MM::AssetSystem::AssetManager::LoadImage(const std::string& asset_name,
    const FileSystem::Path& image_path, const int& desired_channels) {
  std::shared_ptr<AssetBase> image =
      std::make_shared<Image>(asset_name, image_path, desired_channels);
  if (!(image->IsValid())) {
    return false;
  }
  {
    std::unique_lock<std::shared_mutex> guard(writer_mutex_);
    asset_name_to_asset_ID_.emplace(asset_name, image->GetAssetID());
    asset_ID_to_asset_.emplace(image->GetAssetID(), image);
  }
  return true;
}

std::vector<std::shared_ptr<MM::AssetSystem::AssetBase>>
MM::AssetSystem::AssetManager::GetAssetsByName(const std::string& asset_name) const {
  std::shared_lock<std::shared_mutex> guard(writer_mutex_);
  const std::size_t count = asset_name_to_asset_ID_.count(asset_name);
  if (count == 0) {
    return std::vector<std::shared_ptr<MM::AssetSystem::AssetBase>>{};
  }
  std::vector<std::shared_ptr<MM::AssetSystem::AssetBase>> result(count);
  const auto equal_ID = asset_name_to_asset_ID_.equal_range(asset_name);
  auto beg = equal_ID.first;
  for (std::size_t i = 0; i < count; ++i) {
    result[i] = asset_ID_to_asset_.at(beg->second);
    ++beg;
  }
  return result;
}

std::shared_ptr<MM::AssetSystem::AssetBase> MM::AssetSystem::AssetManager::
GetAssetByID(const uint32_t& asset_ID) const {
  std::shared_lock<std::shared_mutex> guard(writer_mutex_);
  if (asset_ID_to_asset_.count(asset_ID)) {
    return asset_ID_to_asset_.at(asset_ID);
  }
  return nullptr;
}

bool MM::AssetSystem::AssetManager::Erase(const uint32_t& asset_ID) {
  std::unique_lock<std::shared_mutex> guard{writer_mutex_};
  if (asset_ID_to_asset_.count(asset_ID)) {
    const auto asset = asset_ID_to_asset_[asset_ID];
    const auto equal_element = asset_name_to_asset_ID_.equal_range(asset->GetAssetName());
    auto beg = equal_element.first;
    while (beg != equal_element.second) {
      if (beg->second == asset_ID) {
        asset_name_to_asset_ID_.erase(beg);
        break;
      }
      ++beg;
    }
    assert(beg == equal_element.second,
           "Logic error, it is expected to find the desired element.");
    asset_ID_to_asset_.erase(asset_ID);
    return true;
  }
  return false;
}

bool MM::AssetSystem::AssetManager::ChangeAssetName(
    const uint32_t& asset_ID, const std::string& new_asset_name) {
  if (asset_ID_to_asset_.count(asset_ID)) {
    std::unique_lock<std::shared_mutex> guard{writer_mutex_};
    const auto asset = asset_ID_to_asset_[asset_ID];
    const auto equal_element =
        asset_name_to_asset_ID_.equal_range(asset->GetAssetName());
    auto beg = equal_element.first;
    while (beg != equal_element.second) {
      if (beg->second == asset_ID) {
        std::pair < std::string, uint32_t> new_pair = *beg;
        new_pair.first = new_asset_name;
        asset_name_to_asset_ID_.erase(beg);
        asset_name_to_asset_ID_.emplace(new_pair);
        return true;
      }
      ++beg;
    }
    assert(false, "Logic error, it is expected to find the desired element.");
  }
  return false;
}

std::shared_ptr<MM::AssetSystem::AssetManager> MM::AssetSystem::AssetManager::
GetInstance() {
  if (asset_manager_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!asset_manager_) {
      asset_manager_.reset(new AssetManager{});
    }
  }
  return asset_manager_;
}

MM::AssetSystem::AssetBase::AssetBase(const std::string& resource_name) :
  asset_name_(resource_name),
  asset_ID_(++MM::AssetSystem::AssetManager::increase_ID_) {
  if (AssetManager::increase_ID_ == 0) {
    ++AssetManager::increase_ID_;
  }
}

MM::AssetSystem::AssetBase& MM::AssetSystem::AssetBase::operator=(
    const AssetBase& other) {
  if (&other == this) {
    return *this;
  }
  asset_name_ = other.asset_name_;
  asset_ID_ = other.asset_ID_;
  return *this;
}

MM::AssetSystem::AssetBase& MM::AssetSystem::AssetBase::operator=(
    AssetBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  asset_name_ = other.asset_name_;
  asset_ID_ = other.asset_ID_;

  other.asset_name_ = std::string{};
  other.asset_ID_ = 0;

  return *this;
}

const std::string& MM::AssetSystem::AssetBase::GetAssetName() const {
  return asset_name_;
}

MM::AssetSystem::AssetBase& MM::AssetSystem::AssetBase::SetAssetName(
    const std::string& new_asset_name) {
  asset_name_ = new_asset_name;
  return *this;
}

const uint32_t& MM::AssetSystem::AssetBase::GetAssetID() const {
  return asset_ID_;
}

MM::AssetSystem::Image::Image(const std::string& asset_name,
                              const FileSystem::Path& image_path,
                              const int& desired_channels)
    : AssetBase(asset_name) {
  if (!image_path.Exists()) {
    LOG_ERROR(std::string("Failed to load the image with path ") +
              image_path.String() + ",because the file does not exist.");
  }
  image_pixels_ = std::shared_ptr<stbi_uc>{
      stbi_load(image_path.String().c_str(), &image_width_, &image_height_,
                &image_channels_, desired_channels), stbi_image_free};
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
                image_path.String() + ",because this format are not supported.");
  }
  image_size_ = static_cast<uint64_t>(image_width_) * 
                static_cast<uint64_t>(image_height_) *
                static_cast<uint64_t>(image_channels_);
}

MM::AssetSystem::Image& MM::AssetSystem::Image::operator=(const Image& other) {
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

MM::AssetSystem::Image& MM::AssetSystem::Image::operator=(
    Image&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  AssetBase::operator=(std::move(other));
  image_width_ = other.image_width_;
  image_height_ = other.image_height_;
  image_channels_ = other.image_channels_;
  image_size_ = other.image_size_;
  image_format_ = other.image_format_;
  image_pixels_ = other.image_pixels_;

  other.image_width_ = 0;
  other.image_height_ = 0;
  other.image_channels_ = 0;
  other.image_size_ = 0;
  image_format_ = ImageFormat::UNDEFINED;
  other.image_pixels_.reset();

  return *this;
}

const int& MM::AssetSystem::Image::GetImageWidth() const {
  return image_width_;
}

const int& MM::AssetSystem::Image::GetImageHeight() const {
  return image_height_;
}

const int& MM::AssetSystem::Image::GetImageChannels() const {
  return image_channels_;
}

const uint64_t& MM::AssetSystem::Image::GetImageSize() const {
  return image_size_;
}

std::shared_ptr<const stbi_uc> MM::AssetSystem::Image::GetImagePixels() const {
  return image_pixels_;
}

const MM::AssetSystem::ImageFormat& MM::AssetSystem::Image::
GetImageFormat() const {
  return image_format_;
}

void MM::AssetSystem::Image::Swap(Image& object1, Image& object2) {
  const Image old_object = object1;
  object1 = object2;
  object2 = old_object;
}

bool MM::AssetSystem::Image::IsValid() const {
  return image_pixels_ != nullptr;
}

MM::AssetSystem::AssetType MM::AssetSystem::Image::GetAssetType() {
  return AssetType::IMAGE;
}

std::shared_ptr<MM::AssetSystem::AssetManager> MM::AssetSystem::AssetSystem::
GetAssetManager() const {
  return assert_manager_;
}

std::shared_ptr<MM::AssetSystem::AssetSystem> MM::AssetSystem::AssetSystem::
GetInstance() {
  if (asset_system_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!asset_system_) {
      asset_system_.reset(new AssetSystem{});
    }
  }
  return asset_system_;
}

MM::AssetSystem::AssetSystem::AssetSystem() : assert_manager_(AssetManager::GetInstance()) {}
