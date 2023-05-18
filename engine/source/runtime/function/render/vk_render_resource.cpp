#include "runtime/function/render/vk_render_resource.h"

#include "runtime/function/render/vk_engine.h"

std::mutex MM::RenderSystem::RenderResourceManager::sync_flag_{};

MM::RenderSystem::RenderResourceManager*
MM::RenderSystem::RenderResourceManager ::GetInstance() {
  if (render_resource_manager_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!render_resource_manager_) {
      render_resource_manager_ = new RenderResourceManager{};
    }
  }
  return render_resource_manager_;
}

bool MM::RenderSystem::RenderResourceManager::IsUseToWrite(
    const std::string& resource_name, bool& result) const {
  std::shared_lock<std::shared_mutex> guard{resource_lock_};
  const auto ID = GetObjectIDFromName(resource_name);
  if (ID == 0) {
    return false;
  }
  result = resource_ID_to_manage_info.at(ID).use_to_write;
  return true;
}

bool MM::RenderSystem::RenderResourceManager::IsUseToWrite(
    const std::uint32_t& resource_ID) const {
  std::shared_lock<std::shared_mutex> guard{resource_lock_};
  return resource_ID_to_manage_info.at(resource_ID).use_to_write;
}

bool MM::RenderSystem::RenderResourceManager::IsShared(
    const std::string& resource_name, bool& result) const {
  std::shared_lock<std::shared_mutex> guard{resource_lock_};
  const auto ID = GetResourceIDFromName(resource_name);
  if (ID == 0) {
    return false;
  }

  result = resource_ID_to_manage_info.at(ID).is_shared_;

  return true;
}

bool MM::RenderSystem::RenderResourceManager::IsShared(
    const std::uint32_t& resource_ID) const {
  std::shared_lock<std::shared_mutex> guard{resource_lock_};
  return resource_ID_to_manage_info.at(resource_ID).is_shared_;
}

bool MM::RenderSystem::RenderResourceManager::HaveResource(
    const std::uint32_t& resource_ID) const {
  return HaveData(resource_ID);
}

uint64_t MM::RenderSystem::RenderResourceManager::GetAssetIDFromResourceID(
    const std::uint32_t& resource_ID) const {
  std::shared_lock<std::shared_mutex> guard{resource_lock_};
  if (resource_ID_to_asset_ID_.count(resource_ID) == 0 ||
      IsUseToWrite(resource_ID)) {
    return 0;
  }

  return resource_ID_to_asset_ID_.at(resource_ID);
}

bool MM::RenderSystem::RenderResourceManager::GetAssetIDFromResourceID(
    const uint32_t& resource_id, std::uint64_t& output_asset_id) const {
  output_asset_id = GetAssetIDFromResourceID(resource_id);

  if (output_asset_id == 0) {
    return false;
  }
  return true;
}

bool MM::RenderSystem::RenderResourceManager::HaveAsset(
    const std::uint64_t& asset_ID) const {
  std::shared_lock<std::shared_mutex> guard{resource_lock_};
  return asset_ID_to_resource_IDs_.count(asset_ID) == 1;
}

uint32_t MM::RenderSystem::RenderResourceManager::GetResourceIDFromName(
    const std::string& resource_name) const {
  return GetObjectIDFromName(resource_name);
}

const std::string&
MM::RenderSystem::RenderResourceManager::GetResourceNameFromID(
    const std::uint32_t& resource_ID) const {
  return GetObjectNameFromID(resource_ID);
}

bool MM::RenderSystem::RenderResourceManager::GetResourceIDsFromAssetID(
    const uint64_t& asset_ID,
    std::vector<uint32_t>& output_resource_IDs) const {
  std::shared_lock<std::shared_mutex> guard{resource_lock_};
  if (!HaveAsset(asset_ID)) {
    return false;
  }

  output_resource_IDs = asset_ID_to_resource_IDs_.at(asset_ID);

  return true;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::GetDeepCopy(
    const std::string& new_name_of_resource, const std::string& resource_name) {
  const auto mapped_ID = GetResourceIDFromName(resource_name);
  if (mapped_ID == 0) {
    LOG_ERROR(
        std::string("Render resource " + resource_name + "is not exist."));
    return 0;
  }

  const auto ID = RenderManageBase<RenderResourceBase>::GetDeepCopy(
      new_name_of_resource, mapped_ID);

  if (ID == 0) {
    LOG_ERROR(
        std::string("Failed to deep copy render resource " + resource_name));
    return 0;
  }

  const auto asset_id = GetAssetIDFromResourceID(ID);
  std::unique_lock<std::shared_mutex> guard{resource_lock_};
  if (asset_id != 0) {
    asset_ID_to_resource_IDs_.at(asset_id).push_back(ID);
    resource_ID_to_asset_ID_.emplace(ID, asset_id);
    resource_ID_to_manage_info.emplace(
        ID, resource_ID_to_manage_info.at(mapped_ID));

    return ID;
  }

  resource_ID_to_manage_info.emplace(ID,
                                     resource_ID_to_manage_info.at(mapped_ID));

  return ID;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::GetDeepCopy(
    const std::string& new_name_of_resource, const std::uint32_t& resource_ID) {
  auto ID = RenderManageBase<RenderResourceBase>::GetDeepCopy(
      new_name_of_resource, resource_ID);

  if (ID == 0) {
    LOG_ERROR(std::string("Render resource " +
                          GetResourceNameFromID(resource_ID) +
                          "is not exist."));
    return 0;
  }

  const auto asset_id = GetAssetIDFromResourceID(ID);
  std::unique_lock<std::shared_mutex> guard{resource_lock_};
  if (asset_id != 0) {
    asset_ID_to_resource_IDs_.at(asset_id).push_back(ID);
    resource_ID_to_asset_ID_.emplace(ID, asset_id);
    resource_ID_to_manage_info.emplace(
        ID, resource_ID_to_manage_info.at(resource_ID));

    return ID;
  }

  resource_ID_to_manage_info.emplace(
      ID, resource_ID_to_manage_info.at(resource_ID));

  return ID;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::GetLightCopy(
    const std::string& new_name_of_resource, const std::string& resource_name) {
  const auto mapped_ID = GetResourceIDFromName(resource_name);
  if (mapped_ID == 0) {
    LOG_ERROR(
        std::string("Render resource " + resource_name + "is not exist."));
    return 0;
  }

  auto ID = RenderManageBase<RenderResourceBase>::GetLightCopy(
      new_name_of_resource, mapped_ID);

  if (ID == 0) {
    LOG_ERROR(
        std::string("Failed to deep copy render resource " + resource_name));
    return 0;
  }

  const auto asset_id = GetAssetIDFromResourceID(ID);
  std::unique_lock<std::shared_mutex> guard{resource_lock_};
  if (asset_id != 0) {
    asset_ID_to_resource_IDs_.at(asset_id).push_back(ID);
    resource_ID_to_asset_ID_.emplace(ID, asset_id);
    resource_ID_to_manage_info.emplace(
        ID,
        resource_ID_to_manage_info.at(GetResourceIDFromName(resource_name)));

    return ID;
  }

  resource_ID_to_manage_info.emplace(
      ID, resource_ID_to_manage_info.at(GetResourceIDFromName(resource_name)));

  return ID;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::GetLightCopy(
    const std::string& new_name_of_resource, const std::uint32_t& resource_ID) {
  auto ID = RenderManageBase<RenderResourceBase>::GetLightCopy(
      new_name_of_resource, resource_ID);
  if (ID == 0) {
    LOG_ERROR(std::string("Render resource " +
                          GetResourceNameFromID(resource_ID) +
                          "is not exist."));
    return 0;
  }
  const auto asset_id = GetAssetIDFromResourceID(ID);
  std::unique_lock<std::shared_mutex> guard{resource_lock_};
  if (asset_id != 0) {
    asset_ID_to_resource_IDs_.at(asset_id).push_back(ID);
    resource_ID_to_asset_ID_.emplace(ID, asset_id);
    resource_ID_to_manage_info.emplace(
        ID, resource_ID_to_manage_info.at(resource_ID));

    return ID;
  }

  resource_ID_to_manage_info.emplace(
      ID, resource_ID_to_manage_info.at(resource_ID));

  return ID;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::SaveData(
    std::unique_ptr<RenderResourceBase>&& resource) {
  if (resource->GetResourceType() == ResourceType::STAGE_BUFFER) {
    return 0;
  }
  auto resource_ID = resource->GetResourceID();

  RenderManageBase<RenderResourceBase>::SaveData(std::move(resource));

  std::unique_lock<std::shared_mutex> guard(resource_lock_);
  resource_ID_to_manage_info.emplace(resource_ID,
                                     RenderResourceManageInfo{false, true});

  return resource_ID;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::SaveData(
    std::unique_ptr<RenderResourceBase>&& resource,
    const RenderResourceManageInfo& manage_info, const uint64_t& asset_ID) {
  if (resource->GetResourceType() == ResourceType::STAGE_BUFFER) {
    return 0;
  }
  auto resource_ID = resource->GetResourceID();

  RenderManageBase<RenderResourceBase>::SaveData(std::move(resource));

  std::unique_lock<std::shared_mutex> guard(resource_lock_);
  resource_ID_to_manage_info.emplace(resource_ID, manage_info);

  if (asset_ID && !manage_info.use_to_write) {
    if (asset_ID_to_resource_IDs_.count(asset_ID)) {
      asset_ID_to_resource_IDs_.at(asset_ID).push_back(resource_ID);
    } else {
      asset_ID_to_resource_IDs_.emplace(
          asset_ID, std::vector<std::uint32_t>(resource_ID));
    }
    resource_ID_to_asset_ID_.emplace(resource_ID, asset_ID);
  }

  resource_ID_to_manage_info.emplace(resource_ID, manage_info);
  return resource_ID;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::AddUseToWrite(
    const std::string& new_name_of_resource, const std::string& resource_name,
    const bool& is_shared) {
  const auto mapped_ID = GetResourceIDFromName(resource_name);
  if (mapped_ID == 0) {
    LOG_ERROR(
        std::string("Render resource " + resource_name + "is not exist."));
    return 0;
  }

  if (is_shared && IsShared(mapped_ID) && IsUseToWrite(mapped_ID)) {
    return RenderManageBase::AddUse(mapped_ID);
  }

  const auto ID = RenderManageBase<RenderResourceBase>::GetDeepCopy(
      new_name_of_resource, mapped_ID);

  std::unique_lock<std::shared_mutex> guard{resource_lock_};
  resource_ID_to_manage_info.emplace(ID,
                                     RenderResourceManageInfo{true, is_shared});

  return ID;
}

std::uint32_t MM::RenderSystem::RenderResourceManager::AddUseToWrite(
    const std::string& new_name_of_resource, const std::uint32_t& resource_ID,
    const bool& is_shared) {
  if (is_shared && IsShared(resource_ID) && IsUseToWrite(resource_ID)) {
    return RenderManageBase::AddUse(resource_ID);
  }

  const auto ID = RenderManageBase<RenderResourceBase>::GetDeepCopy(
      new_name_of_resource, resource_ID);

  std::unique_lock<std::shared_mutex> guard{resource_lock_};
  resource_ID_to_manage_info.emplace(ID,
                                     RenderResourceManageInfo{true, is_shared});

  return ID;
}

bool MM::RenderSystem::RenderResourceManager::AddUse(
    const std::uint32_t& resource_ID) {
  if (IsShared(resource_ID)) {
    RenderManageBase<RenderResourceBase>::AddUse(resource_ID);
    return true;
  }
  return false;
}

bool MM::RenderSystem::RenderResourceManager::AddUse(
    const std::string& resource_name) {
  const auto mapped_ID = GetResourceIDFromName(resource_name);
  if (mapped_ID == 0) {
    return false;
  }
  if (IsShared(mapped_ID)) {
    return RenderManageBase<RenderResourceBase>::AddUse(resource_name);
  }
  return false;
}

void MM::RenderSystem::RenderResourceManager::ReleaseUse(
    const std::uint32_t& object_id) {
  std::unique_lock<std::shared_mutex> guard(resource_lock_);
  const auto asset_ID = GetAssetIDFromResourceID(object_id);
  if (asset_ID) {
    resource_ID_to_asset_ID_.erase(object_id);
    const auto itr =
        std::find(asset_ID_to_resource_IDs_.at(asset_ID).begin(),
                  asset_ID_to_resource_IDs_.at(asset_ID).end(), object_id);
    asset_ID_to_resource_IDs_.at(asset_ID).erase(
        static_cast<std::vector<unsigned>::const_iterator>(itr));
  }

  resource_ID_to_manage_info.erase(object_id);

  RenderManageBase<RenderResourceBase>::ReleaseUse(object_id);
}

void MM::RenderSystem::RenderResourceManager::ReleaseUse(
    const std::string& resource_name) {
  const auto mapped_ID = GetResourceIDFromName(resource_name);
  if (mapped_ID == 0) {
    return;
  }

  std::unique_lock<std::shared_mutex> guard(resource_lock_);
  const auto asset_ID = GetAssetIDFromResourceID(mapped_ID);
  if (asset_ID) {
    resource_ID_to_asset_ID_.erase(mapped_ID);
    const auto itr =
        std::find(asset_ID_to_resource_IDs_.at(asset_ID).begin(),
                  asset_ID_to_resource_IDs_.at(asset_ID).end(), mapped_ID);
    asset_ID_to_resource_IDs_.at(asset_ID).erase(
        static_cast<std::vector<unsigned>::const_iterator>(itr));
  }

  resource_ID_to_manage_info.erase(mapped_ID);

  RenderManageBase<RenderResourceBase>::ReleaseUse(resource_name);
}

MM::RenderSystem::RenderResourceManager::RenderResourceManager()
    : RenderManageBase<MM::RenderSystem::RenderResourceBase>(),
      resource_lock_(),
      asset_ID_to_resource_IDs_(),
      resource_ID_to_asset_ID_(),
      resource_ID_to_manage_info() {}

bool MM::RenderSystem::RenderResourceManager::Destroy() {
  std::lock_guard<std::mutex> guard{sync_flag_};
  if (render_resource_manager_) {
    delete render_resource_manager_;

    render_resource_manager_ = nullptr;

    return true;
  }

  return true;
}

MM::RenderSystem::RenderResourceBase::RenderResourceBase()
    : ManagedObjectBase() {}

MM::RenderSystem::RenderResourceBase::RenderResourceBase(
    const std::string& resource_name)
    : ManagedObjectBase(
          resource_name,
          RenderResourceManager::GetInstance()->GetIncreaseIndex()) {}

MM::RenderSystem::RenderResourceBase::RenderResourceBase(
    RenderResourceBase&& other) noexcept
    : ManagedObjectBase(std::move(other)) {}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::operator=(
    const RenderResourceBase& other) {
  if (&other == this) {
    return *this;
  }
  ManagedObjectBase::operator=(other);

  return *this;
}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::operator=(
    RenderResourceBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  ManagedObjectBase::operator=(std::move(other));

  return *this;
}

const std::string& MM::RenderSystem::RenderResourceBase::GetResourceName()
    const {
  return GetObjectName();
}

const uint32_t& MM::RenderSystem::RenderResourceBase::GetResourceID() const {
  return GetObjectID();
}

MM::RenderSystem::ResourceType
MM::RenderSystem::RenderResourceBase::GetResourceType() const {
  return ResourceType::UNDEFINED;
}

std::unique_ptr<MM::RenderSystem::RenderResourceBase>
MM::RenderSystem::RenderResourceBase::GetLightCopy(
    const std::string& new_name_of_copy_resource) const {
  RenderResourceBase* new_render_resource =
      new RenderResourceBase(new_name_of_copy_resource);
  return std::unique_ptr<RenderResourceBase>(new_render_resource);
}

std::unique_ptr<MM::RenderSystem::RenderResourceBase>
MM::RenderSystem::RenderResourceBase::GetDeepCopy(
    const std::string& new_name_of_copy_resource) const {
  RenderResourceBase* new_render_resource =
      new RenderResourceBase(new_name_of_copy_resource);
  return std::unique_ptr<RenderResourceBase>(new_render_resource);
}

void MM::RenderSystem::RenderResourceBase::SetResourceName(
    const std::string& new_resource_name) {
  SetObjectName(new_resource_name);
}

void MM::RenderSystem::RenderResourceBase::SetResourceID(
    const std::uint32_t& new_resource_ID) {
  SetObjectID(new_resource_ID);
}

MM::RenderSystem::RenderResourceBase::RenderResourceBase(
    const std::string& resource_name, const std::uint32_t& new_resource_ID)
    : ManagedObjectBase(resource_name, new_resource_ID) {}

void MM::RenderSystem::RenderResourceBase::Release() {
  ManagedObjectBase::Release();
}

void MM::RenderSystem::RenderResourceBase::Reset(
    MM::RenderSystem::RenderResourceBase* other) {
  if (other == nullptr) {
    RenderResourceBase::Release();
    return;
  }
  operator=(*other);
}
bool MM::RenderSystem::RenderResourceBase::IsArray() const { return false; }
bool MM::RenderSystem::RenderResourceBase::CanWrite() const { return false; }
VkDeviceSize MM::RenderSystem::RenderResourceBase::GetSize() const { return 0; }
uint32_t MM::RenderSystem::RenderResourceBase::UseCount() const { return 0; }
bool MM::RenderSystem::RenderResourceBase::IsValid() const { return false; }

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    const std::string& resource_name, RenderEngine* engine,
    const VkDescriptorType& descriptor_type,
    const std::shared_ptr<AssetType::Image>& image, VkImageUsageFlags usages,
    const std::vector<std::uint32_t>& queue_index,
    const VkSharingMode& sharing_mode, const VkImageLayout& image_layout,
    const uint32_t& mipmap_levels, const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags)
    : RenderResourceBase(resource_name), render_engine_(engine) {
#ifdef CHECK_PARAMETERS
  if (!CheckInitParameter(engine, descriptor_type, image, mipmap_levels,
                          usages)) {
    return;
  }
#endif

  render_engine_ = engine;

  // wrapper_->bind_->binding = 0; binding will be determined when binding to
  // RenderPass.
  image_bind_info_.bind_.descriptorType = descriptor_type;
  image_bind_info_.bind_.descriptorCount = 1;
  image_bind_info_.bind_.stageFlags = VK_SHADER_STAGE_ALL;
  // wrapper_->bind_->pImmutableSamplers = nullptr; I don't know what to use.
  // I will temporarily use the default value.

  ImageInfo temp_info{};
  temp_info.image_extent_.width = static_cast<uint32_t>(image->GetImageWidth());
  temp_info.image_extent_.height =
      static_cast<uint32_t>(image->GetImageHeight());
  temp_info.image_extent_.depth = 1;
  temp_info.image_layout_ = image_layout;
  temp_info.can_mapped_ = Utils::CanBeMapped(memory_usage, allocation_flags);
  temp_info.is_transform_src_ = Utils::IsTransformSrcImage(usages);
  temp_info.is_transform_dest_ = Utils::IsTransformDestImage(usages);
  temp_info.is_exclusive_ =
      sharing_mode == VK_SHARING_MODE_EXCLUSIVE ? true : false;
  temp_info.queue_index_ = queue_index;

  const uint32_t recommend_mipmap_level =
      static_cast<uint32_t>(std::floor(std::log2(std::max(
          temp_info.image_extent_.width, temp_info.image_extent_.height)))) +
      1;

  temp_info.mipmap_levels_ = mipmap_levels > recommend_mipmap_level
                                 ? recommend_mipmap_level
                                 : mipmap_levels;
  temp_info.array_layers_ = 1;

  AllocatedBuffer stage_buffer;
  if (!LoadImageToStageBuffer(image, stage_buffer, temp_info)) {
    RenderResourceTexture::Release();
    return;
  }

  MM_CHECK(InitImage(stage_buffer, usages, memory_usage, allocation_flags,
                     temp_info),
           RenderResourceTexture::Release();
           return;)

  MM_CHECK(GenerateMipmap(), RenderResourceTexture::Release(); return;)

  if (!InitImageView()) {
    RenderResourceTexture::Release();
    return;
  }

  // Create a sampler if the type referred to by the DescriptorType
  // requires a sampler, otherwise no sampler will be created.
  if (Utils::DescriptorTypeIsImageSampler(descriptor_type)) {
    if (!InitSampler()) {
      RenderResourceTexture::Release();
      return;
    }
  }
}

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    const std::string& resource_name, RenderEngine* engine,
    const AllocatedImage& image, const ImageBindInfo& image_bind_info)
    : RenderResourceBase(resource_name),
      render_engine_(engine),
      image_(image),
      image_bind_info_(image_bind_info) {}

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    const RenderResourceTexture& other)
    : RenderResourceBase(other),
      render_engine_(other.render_engine_),
      image_(other.image_),
      image_bind_info_(other.image_bind_info_) {}

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    RenderResourceTexture&& other) noexcept
    : RenderResourceBase(std::move(other)),
      render_engine_(other.render_engine_),
      image_(std::move(std::move(other.image_))),
      image_bind_info_(std::move(other.image_bind_info_)) {
  other.Release();
}

MM::RenderSystem::RenderResourceTexture&
MM::RenderSystem::RenderResourceTexture::operator=(
    const RenderResourceTexture& other) {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(other);
  render_engine_ = other.render_engine_;
  image_ = other.image_;
  image_bind_info_ = other.image_bind_info_;

  return *this;
}

MM::RenderSystem::RenderResourceTexture&
MM::RenderSystem::RenderResourceTexture::operator=(
    RenderResourceTexture&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(std::move(other));
  render_engine_ = other.render_engine_;
  image_ = std::move(other.image_);
  image_bind_info_ = std::move(other.image_bind_info_);

  other.Release();

  return *this;
}

const VkExtent3D& MM::RenderSystem::RenderResourceTexture::GetExtent() const {
  return image_.GetImageExtent();
}

const VkFormat& MM::RenderSystem::RenderResourceTexture::GetFormat() const {
  return image_.GetImageFormat();
}

const VkImageLayout& MM::RenderSystem::RenderResourceTexture::GetImageLayout()
    const {
  return image_.GetImageLayout();
}

const std::uint32_t& MM::RenderSystem::RenderResourceTexture::GetMipmapLevels()
    const {
  return image_.GetMipmapLevels();
}

const std::uint32_t& MM::RenderSystem::RenderResourceTexture::GetArrayLayers()
    const {
  return image_.GetArrayLayers();
}

const MM::RenderSystem::ImageInfo&
MM::RenderSystem::RenderResourceTexture::GetImageInfo() const {
  return image_.GetImageInfo();
}

const MM::RenderSystem::ImageBindInfo&
MM::RenderSystem::RenderResourceTexture::GetImageBindInfo() const {
  return image_bind_info_;
}

MM::RenderSystem::RenderResourceTexture
MM::RenderSystem::RenderResourceTexture ::GetCopy() const {
  return *this;
}

MM::RenderSystem::RenderResourceTexture
MM::RenderSystem::RenderResourceTexture::GetCopyWithNewImageView(
    const VkImageViewCreateInfo& image_view_create_info) {
  auto new_texture = *this;

  VkImageView temp_image_view{nullptr};
  vkCreateImageView(render_engine_->device_, &image_view_create_info, nullptr,
                    &temp_image_view);

  new_texture.image_bind_info_.image_view_ =
      MM::Utils::MakeSharedWithDestructor<VkImageView>(
          [&engine = render_engine_](VkImageView* value) {
            if (value == nullptr) {
              return;
            }
            vkDestroyImageView(engine->device_, *value, nullptr);

            delete value;
            value = nullptr;
          },
          temp_image_view);

  return new_texture;
}

MM::RenderSystem::RenderResourceTexture
MM::RenderSystem::RenderResourceTexture ::GetCopyWithNewSampler(
    const VkSamplerCreateInfo& sampler_create_info) {
  auto new_texture = *this;
  VkSampler temp_sample{nullptr};
  vkCreateSampler(render_engine_->device_, &sampler_create_info, nullptr,
                  &temp_sample);

  new_texture.image_bind_info_.sampler_ =
      MM::Utils::MakeSharedWithDestructor<VkSampler>(
          [&engine = render_engine_](VkSampler* value) {
            if (value == nullptr) {
              return;
            }
            vkDestroySampler(engine->device_, *value, nullptr);

            delete value;
            value = nullptr;
          },
          temp_sample);

  return new_texture;
}

MM::RenderSystem::RenderResourceTexture
MM::RenderSystem::RenderResourceTexture ::GetCopyWithNewImageView(
    const std::shared_ptr<VkImageView>& image_view) {
  auto new_texture = *this;

  new_texture.image_bind_info_.image_view_ = image_view;

  return new_texture;
}

MM::RenderSystem::RenderResourceTexture
MM::RenderSystem::RenderResourceTexture ::GetCopyWithNewSampler(
    const std::shared_ptr<VkSampler>& sampler) {
  auto new_texture = *this;

  new_texture.image_bind_info_.sampler_ = sampler;

  return new_texture;
}

VkDescriptorType MM::RenderSystem::RenderResourceTexture::GetDescriptorType()
    const {
  return image_bind_info_.bind_.descriptorType;
}

bool MM::RenderSystem::RenderResourceTexture::CanMapped() const {
  return image_.CanMapped();
}

bool MM::RenderSystem::RenderResourceTexture::IsStorage() const {
  return image_bind_info_.bind_.descriptorType ==
         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
}

bool MM::RenderSystem::RenderResourceTexture::HaveSample() const {
  return !IsStorage();
}

bool MM::RenderSystem::RenderResourceTexture::IsValid() const {
  bool result = render_engine_ != nullptr &&
                image_bind_info_.bind_.descriptorCount != 0 &&
                image_.IsValid() && image_bind_info_.image_view_ != nullptr;
  if (Utils::DescriptorTypeIsImageSampler(
          image_bind_info_.bind_.descriptorType)) {
    result = result && image_bind_info_.sampler_ != nullptr;
  }
  return result;
}

std::unique_ptr<MM::RenderSystem::RenderResourceBase>
MM::RenderSystem::RenderResourceTexture::GetLightCopy(
    const std::string& new_name_of_copy_resource) const {
  auto new_resource = std::make_unique<RenderResourceTexture>(*this);
  new_resource->SetResourceName(new_name_of_copy_resource);
  new_resource->SetResourceID(
      MM::RenderSystem::RenderResourceManager::GetInstance()
          ->GetIncreaseIndex());

  return new_resource;
}

std::unique_ptr<MM::RenderSystem::RenderResourceBase>
MM::RenderSystem::RenderResourceTexture::GetDeepCopy(
    const std::string& new_name_of_copy_resource) const {
  VkImageUsageFlags new_image_usage;
  assert(Utils::GetImageUsageFromDescriptorType(
      image_bind_info_.bind_.descriptorType, new_image_usage));
  if (image_.IsTransformSrc()) {
    new_image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  std::vector<std::uint32_t> temp_queue_index = image_.GetQueueIndexes();
  const VkImageCreateInfo new_image_create_info = Utils::GetImageCreateInfo(
      image_.GetImageFormat(),
      new_image_usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      image_.GetImageExtent(), temp_queue_index);

  const VmaAllocationCreateInfo new_allocation_create_info =
      Utils::GetVmaAllocationCreateInfo(
          VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
          image_.CanMapped()
              ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
              : 0);

  VkImage new_image{nullptr};
  VmaAllocation new_allocation{nullptr};

  VK_CHECK(vmaCreateImage(render_engine_->GetAllocator(),
                          &new_image_create_info, &new_allocation_create_info,
                          &new_image, &new_allocation, nullptr),
           LOG_ERROR("Failed to deep copy texture.");
           return std::unique_ptr<RenderResourceBase>();)

  ImageInfo new_image_info = GetImageInfo();
  new_image_info.is_transform_dest_ = true;

  AllocatedImage new_allocated_image{render_engine_->GetAllocator(), new_image,
                                     new_allocation, new_image_info};

  std::vector<VkImageSubresourceLayers> sub_resource_vector(
      static_cast<std::vector<VkImageSubresourceLayers>::size_type>(
          image_.GetMipmapLevels() * image_.GetArrayLayers()));
  for (std::uint32_t i = 0; i < image_.GetMipmapLevels(); ++i) {
    VkImageSubresourceLayers temp_src_region = Utils::GetImageSubResourceLayers(
        VK_IMAGE_ASPECT_COLOR_BIT, i + 1, 1, image_.GetArrayLayers());
    sub_resource_vector.push_back(temp_src_region);
  }

  std::vector<VkImageCopy2> image_region_vector;
  for (const auto& sub_resource : sub_resource_vector) {
    VkImageCopy2 temp_image_copy =
        Utils::GetImageCopy(sub_resource, sub_resource, VkOffset3D{0, 0, 0},
                            VkOffset3D{0, 0, 0}, image_.GetImageExtent());
    image_region_vector.push_back(temp_image_copy);
  }

  MM_CHECK(render_engine_->CopyImage(const_cast<AllocatedImage&>(image_),
                                     new_allocated_image, GetImageLayout(),
                                     GetImageLayout(), image_region_vector),
           LOG_ERROR("Failed to copy imager to new image.");
           return std::unique_ptr<RenderResourceBase>();)

  return std::make_unique<RenderResourceTexture>(
      new_name_of_copy_resource, render_engine_, new_allocated_image,
      image_bind_info_);
}

void MM::RenderSystem::RenderResourceTexture::Release() {
  RenderResourceBase::Release();
  if (!IsValid()) {
    return;
  }

  image_bind_info_.Reset();
  image_.Release();
  render_engine_ = nullptr;
}

void MM::RenderSystem::RenderResourceTexture::Reset(
    MM::RenderSystem::RenderResourceBase* other) {
  if (other == nullptr) {
    Release();
    return;
  }
  if (other->GetResourceType() != ResourceType::Texture) {
    LOG_WARN(
        "The resource type of the reset resource is different from the "
        "original resource type. Only the resources held by the object will be "
        "released, and resources will not be reset.");
    Release();
    return;
  }
  RenderResourceBase::Reset(other);
  operator=(*dynamic_cast<RenderResourceTexture*>(other));
}

uint32_t MM::RenderSystem::RenderResourceTexture::UseCount() const {
  return image_.UseCount();
}

MM::RenderSystem::ResourceType
MM::RenderSystem::RenderResourceTexture::GetResourceType() const {
  return ResourceType::Texture;
}

VkDeviceSize MM::RenderSystem::RenderResourceTexture::GetSize() const {
  return RenderResourceBase::GetSize();
}

bool MM::RenderSystem::RenderResourceTexture::IsArray() const { return false; }

bool MM::RenderSystem::RenderResourceTexture::CanWrite() const {
  return Utils::ResourceImageCanWrite(image_bind_info_.bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceTexture::CheckInitParameter(
    const RenderEngine* engine, const VkDescriptorType& descriptor_type,
    const std::shared_ptr<AssetType::Image>& image,
    const uint32_t& mipmap_level, VkImageUsageFlags usages) const {
  if (engine == nullptr) {
    LOG_ERROR("The incoming engine parameter pointer is null.");
    return false;
  }
  if (!image->IsValid()) {
    LOG_ERROR("The incoming image parameter is not available.");
    return false;
  }
  if (!engine->IsValid()) {
    LOG_ERROR("The rendering engine is not available.");
    return false;
  }
  if (!Utils::DescriptorTypeIsImage(descriptor_type)) {
    LOG_ERROR("Parameter descriptor_type is not  for texture adaptation.");
    return false;
  }
  if (mipmap_level == 0) {
    LOG_ERROR(
        "The value of the parameter mipmap level is 0, and the parameter is "
        "incorrect.");
    return false;
  }
  if (usages & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    LOG_ERROR(
        "Depth and stencil test do not match the RenderResourceTexture "
        "resource type. You should create a RenderResourceFrameBuffer resource "
        "to hold the depth and stencil test resources.");
    return false;
  }
  if (usages & VK_IMAGE_USAGE_STORAGE_BIT &&
      descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
    LOG_ERROR(
        "VkImageUsageFlags specifies that the image is a storage image, but "
        "VkDescriptorType is not a storage image. ");
    return false;
  }
  if (descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE &&
      !(usages & VK_IMAGE_USAGE_STORAGE_BIT)) {
    LOG_ERROR(
        "VkDescriptorType specifies that the image is a storage image, but "
        "VkImageUsageFlags is not a storage image. ");
    return false;
  }
  return true;
}

bool MM::RenderSystem::RenderResourceTexture::LoadImageToStageBuffer(
    const std::shared_ptr<AssetType::Image>& image,
    AllocatedBuffer& stage_buffer, MM::RenderSystem::ImageInfo& image_info) {
  const void* pixels = image->GetImagePixels().get();
  image_info.image_size_ = image->GetImageSize();

  switch (image->GetImageFormat()) {
    case AssetType::ImageFormat::GREY:
      image_info.image_format_ = VK_FORMAT_R8_SRGB;
      break;
    case AssetType::ImageFormat::GREY_ALPHA:
      image_info.image_format_ = VK_FORMAT_R8G8_SRGB;
      break;
    case AssetType::ImageFormat::RGB:
      image_info.image_format_ = VK_FORMAT_R8G8B8_SRGB;
      break;
    case AssetType::ImageFormat::RGB_ALPHA:
      image_info.image_format_ = VK_FORMAT_R8G8B8A8_SRGB;
      break;
    case AssetType::ImageFormat::UNDEFINED:
      LOG_ERROR("Image loading failed, image format not defined.");
      return false;
  }

  stage_buffer = render_engine_->CreateBuffer(
      image_info.image_size_, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_AUTO,
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, false);

  if (!stage_buffer.IsValid()) {
    LOG_ERROR("Failed to create stage buffer.");
  }

  void* stage_data;
  VK_CHECK(vmaMapMemory(render_engine_->allocator_,
                        stage_buffer.GetAllocation(), &stage_data),
           LOG_ERROR("The vmaMapMemory operation failed, unable to complete "
                     "the pointer mapping operation.");
           return false;

  )
  memcpy(stage_data, pixels, image_info.image_size_);
  vmaUnmapMemory(render_engine_->allocator_, stage_buffer.GetAllocation());

  return true;
}

MM::ExecuteResult MM::RenderSystem::RenderResourceTexture::InitImage(
    const AllocatedBuffer& stage_buffer, VkImageUsageFlags usages,
    const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags,
    const ImageInfo& image_info) {
  std::vector<std::uint32_t> temp_queue_index = image_info.queue_index_;
  VkImageCreateInfo image_create_info =
      MM::RenderSystem::Utils::GetImageCreateInfo(
          image_info.image_format_, usages, image_info.image_extent_,
          temp_queue_index);
  image_create_info.mipLevels = image_info.mipmap_levels_;
  image_create_info.samples = render_engine_->GetMultiSampleCount();

  const VmaAllocationCreateInfo image_allocator_create_info =
      MM::RenderSystem::Utils::GetVmaAllocationCreateInfo(memory_usage,
                                                          allocation_flags);

  VkImage temp_image{nullptr};
  VmaAllocation temp_allocation{nullptr};

  usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  if (Utils::DescriptorTypeIsImageSampler(
          image_bind_info_.bind_.descriptorType)) {
    usages |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }
  VK_CHECK(vmaCreateImage(render_engine_->allocator_, &image_create_info,
                          &image_allocator_create_info, &temp_image,
                          &temp_allocation, nullptr),
           LOG_ERROR("Failed to create VkImage.");
           return Utils::VkResultToMMResult(VK_RESULT_CODE);)

  image_ = AllocatedImage{render_engine_->allocator_, temp_image,
                          temp_allocation, image_info};

  MM_CHECK(render_engine_->RunSingleCommandAndWait(
               CommandBufferType::TRANSFORM,
               [&image = image_, &stage = stage_buffer,
                &image_extent = image_.GetImageExtent(),
                &image_layout =
                    image_.GetImageLayout()](AllocatedCommandBuffer& cmd) {
                 MM_CHECK(Utils::BeginCommandBuffer(cmd),
                          LOG_ERROR("Failed to begin command buffer.");
                          return MM_RESULT_CODE;)

                 Utils::AddTransferImageCommands(
                     cmd, image,
                     ImageTransferMode::INIT_TO_TRANSFER_DESTINATION);

                 const auto copy_region = Utils::GetBufferToImageCopyRegion(
                     VK_IMAGE_ASPECT_COLOR_BIT, image_extent);

                 vkCmdCopyBufferToImage(cmd.GetCommandBuffer(),
                                        stage.GetBuffer(), image.GetImage(),
                                        image_layout, 1, &copy_region);

                 MM_CHECK(Utils::EndCommandBuffer(cmd),
                          LOG_ERROR("Failed to end command buffer.");
                          return MM_RESULT_CODE;);

                 return ExecuteResult::SUCCESS;
               }),
           LOG_ERROR("Copying image data to the GPU failed.");
           return MM_RESULT_CODE;)

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderResourceTexture::GenerateMipmap() {
  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM,
          [&image = image_](AllocatedCommandBuffer& cmd) {
            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     LOG_ERROR("Failed to begin command buffer.");
                     return MM_RESULT_CODE;)

            VkImageMemoryBarrier2 barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image.GetImage();
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;

            const auto dependency_info = Utils::GetImageDependencyInfo(barrier);

            int32_t mip_width =
                static_cast<int32_t>(image.GetImageExtent().width);
            int32_t mip_height =
                static_cast<int32_t>(image.GetImageExtent().height);

            for (uint32_t i = 1; i < image.GetMipmapLevels(); ++i) {
              barrier.subresourceRange.baseMipLevel = i - 1;
              barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
              barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
              barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
              barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
              barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
              barrier.dstStageMask = VK_ACCESS_2_TRANSFER_READ_BIT;

              vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

              barrier.subresourceRange.baseMipLevel = i;
              barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
              barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
              barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
              barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
              barrier.srcAccessMask = 0;
              barrier.dstStageMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

              vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

              VkImageBlit blit{};
              blit.srcOffsets[0] = {0, 0, 0};
              blit.srcOffsets[1] = {mip_width, mip_height, 1};
              blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
              blit.srcSubresource.mipLevel = i - 1;
              blit.srcSubresource.baseArrayLayer = 0;
              blit.srcSubresource.layerCount = 1;
              blit.dstOffsets[0] = {0, 0, 0};
              blit.dstOffsets[1] = {mip_width > 1 ? mip_width / 2 : 1,
                                    mip_height > 1 ? mip_height / 2 : 1, 1};
              blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
              blit.dstSubresource.mipLevel = i;
              blit.dstSubresource.baseArrayLayer = 0;
              blit.dstSubresource.layerCount = 1;

              vkCmdBlitImage(cmd.GetCommandBuffer(), image.GetImage(),
                             VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                             image.GetImage(),
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                             VK_FILTER_LINEAR);

              barrier.subresourceRange.baseMipLevel = i - 1;
              barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
              barrier.newLayout = image.GetImageLayout();
              barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
              barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
              barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
              barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

              vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

              if (mip_width > 1) {
                mip_width >>= 1;
              }
              if (mip_height > 1) {
                mip_height >>= 1;
              }
            }

            barrier.subresourceRange.baseMipLevel = image.GetMipmapLevels() - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = image.GetImageLayout();
            barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
            barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     LOG_ERROR("Failed to end command buffer.");
                     return MM_RESULT_CODE;)
            return ExecuteResult::SUCCESS;
          }),
      LOG_ERROR("Mipmap generation failed.");
      return MM_RESULT_CODE;)

  return ExecuteResult::SUCCESS;
}

bool MM::RenderSystem::RenderResourceTexture::InitImageView() {
  auto image_view_create_info = Utils::GetImageViewCreateInfo(
      image_, image_.GetImageFormat(), VK_IMAGE_VIEW_TYPE_2D,
      VK_IMAGE_ASPECT_COLOR_BIT);
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = image_.GetMipmapLevels();

  VkImageView temp_image_view{nullptr};
  VK_CHECK(vkCreateImageView(render_engine_->device_, &image_view_create_info,
                             nullptr, &temp_image_view),
           LOG_ERROR("failed to create VkImageView。");
           RenderResourceTexture::Release(); return false;)
  image_bind_info_.image_view_ =
      MM::Utils::MakeSharedWithDestructor<VkImageView>(
          [&engine = render_engine_](VkImageView* value) {
            if (value == nullptr) {
              return;
            }
            vkDestroyImageView(engine->device_, *value, nullptr);

            delete value;
            value = nullptr;
          },
          temp_image_view);

  return true;
}

bool MM::RenderSystem::RenderResourceTexture::InitSampler() {
  auto sampler_create_info =
      Utils::GetSamplerCreateInfo(render_engine_->physical_device_);
  sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_create_info.mipLodBias = 0.0f;
  sampler_create_info.minLod = 0.0f;
  sampler_create_info.maxLod = static_cast<float>(image_.GetMipmapLevels());

  VkSampler temp_sampler{nullptr};
  VK_CHECK(vkCreateSampler(render_engine_->device_, &sampler_create_info,
                           nullptr, &temp_sampler),
           LOG_ERROR("Failed to create VkSampler");
           return false;)

  image_bind_info_.sampler_ = MM::Utils::MakeSharedWithDestructor<VkSampler>(
      [engine = render_engine_](VkSampler* value) {
        if (value == nullptr) {
          return;
        }
        vkDestroySampler(engine->device_, *value, nullptr);

        delete value;
        value = nullptr;
      },
      temp_sampler);

  return true;
}

MM::RenderSystem::RenderResourceBuffer::RenderResourceBuffer(
    const std::string& resource_name, RenderEngine* engine,
    const VkDescriptorType& descriptor_type, VkBufferUsageFlags buffer_usage,
    const VkDeviceSize& size, const VkDeviceSize& offset,
    const std::vector<std::uint32_t>& queue_index,
    const VkSharingMode& sharing_mode, const VkDeviceSize& size_range,
    const VkDeviceSize& dynamic_offset, const DataToBufferInfo& data_info,
    const VkBufferCreateFlags& buffer_flags, const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags)
    : RenderResourceBase(resource_name), render_engine_(engine) {
  if (size_range == VK_WHOLE_SIZE) {
    buffer_bind_info_.range_size_ = size - offset;
  }
#ifdef CHECK_PARAMETERS
  if (!CheckInitParameter(engine, descriptor_type, buffer_usage, size,
                          buffer_bind_info_.range_size_, offset, dynamic_offset,
                          data_info.data_, data_info.copy_offset_,
                          data_info.copy_offset_)) {
    RenderResourceBuffer::Release();
    return;
  }
#endif

  render_engine_ = engine;

  buffer_bind_info_.bind_.descriptorType = descriptor_type;
  buffer_bind_info_.bind_.descriptorCount = 1;
  buffer_bind_info_.bind_.pImmutableSamplers = nullptr;
  buffer_bind_info_.bind_.stageFlags = VK_SHADER_STAGE_ALL;

  // If there is data that needs to be copied to a buffer and the buffer cannot
  // be mapped, VK_BUFFER_USAGE_TRANSFER_DST_BIT will be automatically added to
  // the buffer_usage to prevent exceptions from being thrown.
  if (data_info.data_ != nullptr) {
    buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  }

  if (!InitBuffer(size, buffer_usage, buffer_flags, memory_usage,
                  allocation_flags, queue_index, sharing_mode)) {
    RenderResourceBuffer::Release();
    return;
  }

  MM_CHECK(CopyDataToBuffer(data_info.data_, offset, size),
           RenderResourceBuffer::Release();
           return;)
}

MM::RenderSystem::RenderResourceBuffer::RenderResourceBuffer(
    const std::string& resource_name, RenderEngine* engine,
    const BufferBindInfo& buffer_info, const AllocatedBuffer& buffer)
    : RenderResourceBase(resource_name),
      render_engine_(engine),
      buffer_bind_info_(buffer_info),
      buffer_(buffer) {}

MM::RenderSystem::RenderResourceBuffer::RenderResourceBuffer(
    RenderResourceBuffer&& other) noexcept
    : RenderResourceBase(std::move(other)),
      render_engine_(other.render_engine_),
      buffer_bind_info_(std::move(other.buffer_bind_info_)),
      buffer_(std::move(other.buffer_)) {}

MM::RenderSystem::RenderResourceBuffer&
MM::RenderSystem::RenderResourceBuffer::operator=(
    const RenderResourceBuffer& other) {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(other);

  render_engine_ = other.render_engine_;
  buffer_bind_info_ = other.buffer_bind_info_;
  buffer_ = other.buffer_;

  return *this;
}

MM::RenderSystem::RenderResourceBuffer&
MM::RenderSystem::RenderResourceBuffer::operator=(
    RenderResourceBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(std::move(other));

  render_engine_ = other.render_engine_;
  buffer_ = std::move(other.buffer_);
  buffer_bind_info_ = std::move(other.buffer_bind_info_);

  render_engine_ = nullptr;

  return *this;
}

const VkDescriptorType&
MM::RenderSystem::RenderResourceBuffer::GetDescriptorType() const {
  return buffer_bind_info_.bind_.descriptorType;
}

// MM::RenderSystem::RenderResourceBuffer
// MM::RenderSystem::RenderResourceBuffer::GetCopyWithNewOffset(
//     const std::string& new_render_resource_name,
//     const VkDeviceSize& new_offset) const {
//   VkDeviceSize new_offset_in = new_offset;
//   if (IsDynamic()) {
//     if (new_offset_in > UINT64_MAX - buffer_bind_info_.dynamic_offset_) {
//       LOG_WARN("New offset is too large.");
//       new_offset_in =
//           buffer_.GetBufferSize() - buffer_bind_info_.dynamic_offset_;
//     }
//     if (new_offset_in + buffer_bind_info_.dynamic_offset_ >
//         buffer_.GetBufferSize()) {
//       LOG_WARN("New offset is too large.");
//       new_offset_in =
//           buffer_.GetBufferSize() - buffer_bind_info_.dynamic_offset_;
//     }
//   } else {
//     if (new_offset_in > buffer_.GetBufferSize()) {
//       new_offset_in = buffer_.GetBufferSize();
//     }
//   }
//
//   std::unique_ptr<RenderResourceBuffer> new_buffer{
//       dynamic_cast<RenderResourceBuffer*>(
//           GetLightCopy(new_render_resource_name).release())};
//
//   new_buffer->buffer_bind_info_.offset_ = new_offset_in;
//
//   std::unique_ptr<RenderResourceBuffer> a{new_buffer.release()};
//
//   return *new_buffer;
// }
//
// MM::RenderSystem::RenderResourceBuffer
// MM::RenderSystem::RenderResourceBuffer::GetCopyWithNewDynamicOffset(
//     const VkDeviceSize& new_dynamic_offset) const {
//   VkDeviceSize new_dynamic_offset_in = new_dynamic_offset;
//
//   if (new_dynamic_offset > UINT64_MAX - buffer_bind_info_.offset_) {
//     LOG_WARN("New_Dynamic_offset is too larger.");
//     new_dynamic_offset_in = buffer_.GetBufferSize() -
//     buffer_bind_info_.offset_;
//   }
//   if (new_dynamic_offset_in + buffer_bind_info_.offset_ >
//       buffer_.GetBufferSize()) {
//     LOG_WARN("New_Dynamic_offset is too large.");
//     new_dynamic_offset_in = buffer_.GetBufferSize() -
//     buffer_bind_info_.offset_;
//   }
//
//   auto new_buffer = GetLightCopy();
//   new_buffer.buffer_bind_info_.dynamic_offset_ = new_dynamic_offset_in;
//
//   return new_buffer;
// }
//
// MM::RenderSystem::RenderResourceBuffer
// MM::RenderSystem::RenderResourceBuffer::GetCopyWithNewOffsetAndDynamicOffset(
//     const VkDeviceSize& new_offset,
//     const VkDeviceSize& new_dynamic_offset) const {
//   VkDeviceSize new_offset_in = new_offset;
//   VkDeviceSize new_dynamic_offset_in = new_dynamic_offset;
//   if (IsDynamic()) {
//     if (new_offset_in > UINT64_MAX - buffer_bind_info_.dynamic_offset_) {
//       LOG_WARN("The sum of new offset and new dynamic offset is too large.");
//       new_offset_in =
//           buffer_.GetBufferSize() - buffer_bind_info_.dynamic_offset_;
//       new_dynamic_offset_in = buffer_bind_info_.dynamic_offset_;
//     }
//     if (new_offset_in + new_dynamic_offset_in > buffer_.GetBufferSize()) {
//       LOG_WARN("The sum of new offset and new dynamic offset is too large.");
//       new_offset_in =
//           buffer_.GetBufferSize() - buffer_bind_info_.dynamic_offset_;
//       new_dynamic_offset_in = buffer_bind_info_.dynamic_offset_;
//     }
//   } else {
//     if (new_offset_in > buffer_.GetBufferSize()) {
//       new_offset_in = buffer_.GetBufferSize();
//     }
//   }
//
//   auto new_buffer = GetLightCopy();
//   new_buffer.buffer_bind_info_.offset_ = new_offset_in;
//
//   if (IsDynamic()) {
//     new_buffer.buffer_bind_info_.dynamic_offset_ = new_dynamic_offset_in;
//   }
//
//   return new_buffer;
// }
//
// MM::RenderSystem::RenderResourceBuffer
// MM::RenderSystem::RenderResourceBuffer::GetCopyWithDynamicBuffer(
//     const VkDeviceSize& new_offset,
//     const VkDeviceSize& new_dynamic_offset) const {
//   if (IsTexel()) {
//     return *this;
//   }
//
//   if (IsDynamic()) {
//     return *this;
//   }
//
//   auto new_buffer = GetLightCopy();
//   if (IsStorage()) {
//     new_buffer.buffer_bind_info_.bind_.descriptorType =
//         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
//   } else {
//     new_buffer.buffer_bind_info_.bind_.descriptorType =
//         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//   }
//
//   if (new_offset < UINT64_MAX - new_dynamic_offset &&
//       new_offset + new_dynamic_offset < buffer_.GetBufferSize()) {
//     new_buffer.buffer_bind_info_.offset_ = new_offset;
//     new_buffer.buffer_bind_info_.dynamic_offset_ = new_dynamic_offset;
//   }
//
//   return new_buffer;
// }

bool MM::RenderSystem::RenderResourceBuffer::IsDynamic() const {
  return Utils::DescriptorTypeIsDynamicBuffer(
      buffer_bind_info_.bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceBuffer::IsStorage() const {
  return Utils::DescriptorTypeIsStorageBuffer(
      buffer_bind_info_.bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceBuffer::IsUniform() const {
  return Utils::DescriptorTypeIsUniformBuffer(
      buffer_bind_info_.bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceBuffer::IsTexel() const {
  return Utils::DescriptorTypeIsTexelBuffer(
      buffer_bind_info_.bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceBuffer::IsTransformSrc() const {
  return buffer_.IsTransformSrc();
}

bool MM::RenderSystem::RenderResourceBuffer::IsTransformDest() const {
  return buffer_.IsTransformDest();
}

MM::RenderSystem::BufferInfo
MM::RenderSystem::RenderResourceBuffer::GetBufferInfo() const {
  return buffer_.GetBufferInfo();
}

const VkDeviceSize& MM::RenderSystem::RenderResourceBuffer::GetBufferSize()
    const {
  return buffer_.GetBufferSize();
}

const VkDeviceSize& MM::RenderSystem::RenderResourceBuffer::GetOffset() const {
  return buffer_bind_info_.offset_;
}

const VkDeviceSize& MM::RenderSystem::RenderResourceBuffer::GetDynamicOffset()
    const {
  return buffer_bind_info_.dynamic_offset_;
}

const VkDeviceSize& MM::RenderSystem::RenderResourceBuffer::GetRangeSize()
    const {
  return buffer_bind_info_.range_size_;
}

bool MM::RenderSystem::RenderResourceBuffer::CanMapped() const {
  return buffer_.CanMapped();
}

const MM::RenderSystem::BufferBindInfo&
MM::RenderSystem::RenderResourceBuffer::GetBufferBindInfo() const {
  return buffer_bind_info_;
}

bool MM::RenderSystem::RenderResourceBuffer::IsValid() const {
  return render_engine_ != nullptr && buffer_bind_info_.IsValid() &&
         buffer_.IsValid();
}

void MM::RenderSystem::RenderResourceBuffer::Release() {
  RenderResourceBase::Release();

  render_engine_ = nullptr;
  buffer_bind_info_.Reset();
  buffer_.Release();
}

void MM::RenderSystem::RenderResourceBuffer::Reset(
    MM::RenderSystem::RenderResourceBase* other) {
  if (other == nullptr) {
    Release();
    return;
  }
  if (other->GetResourceType() != ResourceType::BUFFER) {
    LOG_WARN(
        "The resource type of the reset resource is different from the "
        "original resource type. Only the resources held by the object will be "
        "released, and resources will not be reset.");
    Release();
    return;
  }
  RenderResourceBase::Reset(other);
  operator=(*dynamic_cast<RenderResourceBuffer*>(other));
}

uint32_t MM::RenderSystem::RenderResourceBuffer::UseCount() const {
  return buffer_.UseCount();
}

MM::RenderSystem::ResourceType
MM::RenderSystem::RenderResourceBuffer::GetResourceType() const {
  return ResourceType::BUFFER;
}

VkDeviceSize MM::RenderSystem::RenderResourceBuffer::GetSize() const {
  return buffer_.GetBufferSize();
}

bool MM::RenderSystem::RenderResourceBuffer::IsArray() const { return false; }

bool MM::RenderSystem::RenderResourceBuffer::CanWrite() const {
  return Utils::ResourceBufferCanWrite(buffer_bind_info_.bind_.descriptorType);
}

std::unique_ptr<MM::RenderSystem::RenderResourceBase>
MM::RenderSystem::RenderResourceBuffer::GetLightCopy(
    const std::string& new_name_of_copy_resource) const {
  auto new_buffer = std::make_unique<RenderResourceBuffer>(*this);
  new_buffer->SetResourceID(
      RenderResourceManager::GetInstance()->GetIncreaseIndex());
  new_buffer->SetResourceName(new_name_of_copy_resource);

  return new_buffer;
}

std::unique_ptr<MM::RenderSystem::RenderResourceBase>
MM::RenderSystem::RenderResourceBuffer::GetDeepCopy(
    const std::string& new_name_of_copy_resource) const {
  VkBufferUsageFlags new_buffer_usage;
  assert(Utils::GetBufferUsageFromDescriptorType(GetDescriptorType(),
                                                 new_buffer_usage));

  if (IsTransformSrc()) {
    new_buffer_usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  }
  new_buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  std::vector<std::uint32_t> temp_queue_index = buffer_.GetQueueIndexes();
  const VkBufferCreateInfo new_buffer_create_info = Utils::GetBufferCreateInfo(
      GetBufferSize(), new_buffer_usage, temp_queue_index, 0);

  const VmaAllocationCreateInfo new_allocation_create_info =
      Utils::GetVmaAllocationCreateInfo(
          VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
          CanMapped() ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                      : 0);
  VkBuffer new_buffer{nullptr};
  VmaAllocation new_allocation{nullptr};

  VK_CHECK(vmaCreateBuffer(render_engine_->GetAllocator(),
                           &new_buffer_create_info, &new_allocation_create_info,
                           &new_buffer, &new_allocation, nullptr),
           LOG_ERROR("Failed to create buffer.");
           return std::unique_ptr<RenderResourceBuffer>())

  BufferInfo new_buffer_info = GetBufferInfo();
  new_buffer_info.is_transform_dest_ = true;

  AllocatedBuffer new_allocated_buffer{render_engine_->GetAllocator(),
                                       new_buffer, new_allocation,
                                       new_buffer_info};

  const VkBufferCopy2 buffer_copy = Utils::GetBufferCopy(GetBufferSize(), 0, 0);

  MM_CHECK(render_engine_->CopyBuffer(buffer_, new_allocated_buffer,
                                      std::vector<VkBufferCopy2>{buffer_copy}),
           LOG_ERROR("Failed to copy buffer.");
           return std::unique_ptr<RenderResourceBuffer>{};)

  return std::make_unique<RenderResourceBuffer>(
      new_name_of_copy_resource, render_engine_, buffer_bind_info_,
      new_allocated_buffer);
}

bool MM::RenderSystem::RenderResourceBuffer::CheckInitParameter(
    const RenderEngine* engine, const VkDescriptorType& descriptor_type,
    const VkBufferUsageFlags& buffer_usage, const VkDeviceSize& size,
    const VkDeviceSize& range_size, const VkDeviceSize& offset,
    const VkDeviceSize& dynamic_offset, const void* data,
    const VkDeviceSize& copy_offset, const VkDeviceSize& copy_size) const {
  if (engine == nullptr) {
    LOG_ERROR("The incoming engine parameter pointer is null.");
    return false;
  }
  if (!engine->IsValid()) {
    LOG_ERROR("The rendering engine is not available.");
    return false;
  }

  if (size > 65536 && Utils::DescriptorTypeIsUniformBuffer(descriptor_type)) {
    LOG_ERROR(
        "The maximum allowable uniform buffer size for most desktop clients is "
        "64KB (1024 * 64=65536).");
    return false;
  }

  if (!Utils::DescriptorTypeIsBuffer(descriptor_type)) {
    LOG_ERROR("Parameter descriptor_type is not  for buffer adaptation.");
    return false;
  }
  if (buffer_usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.");
      return false;
    }
    if (render_engine_->gpu_properties_.limits.maxUniformBufferRange <
        range_size) {
      LOG_ERROR(
          "The uniform buffer range size you want to create is too large.");
      return false;
    }
  } else if (buffer_usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.");
      return false;
    }
  } else if (buffer_usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
        descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.");
      return false;
    }
    if (render_engine_->gpu_properties_.limits.maxUniformBufferRange <
        range_size) {
      LOG_ERROR("The uniform buffer you want to create is too large.");
      return false;
    }
  } else if (buffer_usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER &&
        descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.");
      return false;
    }
  } else {
    LOG_ERROR("The buffer_usage not currently supported.");
    return false;
  }

  if (size == 0 || range_size == 0) {
    LOG_ERROR("Buffer size pr range size must great than 0.");
    return false;
  }

  if (Utils::DescriptorTypeIsDynamicBuffer(descriptor_type)) {
    if (offset > ULLONG_MAX - dynamic_offset) {
      LOG_ERROR("The sum of the offset and dynamic_offset too lager.");
      return false;
    }
    if (offset + dynamic_offset > size) {
      LOG_ERROR("The sum of offset and dynamic_offset is greater than size.");
      return false;
    }
  } else {
    if (offset > size) {
      LOG_ERROR("The offset is greater than size.");
      return false;
    }
  }

  if (!OffsetIsAlignment(engine, descriptor_type, offset, dynamic_offset)) {
    LOG_ERROR("The offset value does not meet memory alignment requirements.");
    return false;
  }

  if (data != nullptr) {
    if (copy_offset > ULLONG_MAX - copy_offset) {
      LOG_ERROR("The sum of the copy_offset and copy_offset too lager.");
      return false;
    }
    if (copy_offset + copy_size > size) {
      LOG_ERROR("The sum of copy_offset and copy_size is greater than size.");
      return false;
    }
  }

  return true;
}

bool MM::RenderSystem::RenderResourceBuffer::InitBuffer(
    const VkDeviceSize& size, const VkBufferUsageFlags& buffer_usage,
    const VkBufferCreateFlags& buffer_flags, const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags,
    const std::vector<std::uint32_t>& queue_index,
    const VkSharingMode& sharing_mode) {
  std::vector<std::uint32_t> temp_queue_index = queue_index;
  const auto buffer_create_info = Utils::GetBufferCreateInfo(
      size, buffer_usage, temp_queue_index, buffer_flags);
  const auto allocation_create_info =
      Utils::GetVmaAllocationCreateInfo(memory_usage, allocation_flags);

  VkBuffer temp_buffer{nullptr};
  VmaAllocation temp_allocation{nullptr};

  VK_CHECK(vmaCreateBuffer(render_engine_->allocator_, &buffer_create_info,
                           &allocation_create_info, &temp_buffer,
                           &temp_allocation, nullptr),
           LOG_ERROR("Failed to create VkBuffer.");
           return false;)

  BufferInfo temp_buffer_info{};
  temp_buffer_info.buffer_size_ = size;
  temp_buffer_info.can_mapped_ =
      Utils::CanBeMapped(memory_usage, allocation_flags);
  temp_buffer_info.is_transform_src_ =
      Utils::IsTransformSrcBuffer(buffer_flags);
  temp_buffer_info.is_transform_dest_ =
      Utils::IsTransformDestBuffer(buffer_flags);
  temp_buffer_info.is_exclusive_ =
      sharing_mode == VK_SHARING_MODE_EXCLUSIVE ? true : false;
  temp_buffer_info.queue_index_ = std::move(temp_queue_index);

  buffer_ = AllocatedBuffer{render_engine_->allocator_, temp_buffer,
                            temp_allocation, temp_buffer_info};

  return false;
}

MM::ExecuteResult MM::RenderSystem::RenderResourceBuffer::CopyDataToBuffer(
    void* data, const VkDeviceSize& offset, const VkDeviceSize& size) {
  if (data == nullptr) {
    return ExecuteResult::SUCCESS;
  }

  if (buffer_.CanMapped()) {
    char* buffer_ptr{nullptr};
    VK_CHECK(vmaMapMemory(render_engine_->allocator_, buffer_.GetAllocation(),
                          reinterpret_cast<void**>(&buffer_ptr)),
             LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
             return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;)

    buffer_ptr = buffer_ptr + offset;
    memcpy(buffer_ptr, data, size);

    vmaUnmapMemory(render_engine_->allocator_, buffer_.GetAllocation());

    return ExecuteResult::SUCCESS;
  }

  auto stage_buffer = render_engine_->CreateBuffer(
      size,
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, false);

  if (!stage_buffer.IsValid()) {
    LOG_ERROR("Failed to create stage buffer.");
    return ExecuteResult::CREATE_OBJECT_FAILED;
  }

  const auto buffer_copy_region = Utils::GetBufferCopy(size, 0, offset);
  std::vector<VkBufferCopy2> buffer_copy_regions{buffer_copy_region};
  auto buffer_copy_info =
      Utils::GetCopyBufferInfo(stage_buffer, buffer_, buffer_copy_regions);

  void* stage_buffer_ptr{nullptr};

  VK_CHECK(vmaMapMemory(render_engine_->allocator_,
                        stage_buffer.GetAllocation(), &stage_buffer_ptr),
           LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
           return Utils::VkResultToMMResult(VK_RESULT_CODE);)

  memcpy(stage_buffer_ptr, data, size);

  vmaUnmapMemory(render_engine_->allocator_, stage_buffer.GetAllocation());

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM,
          [&buffer_copy_info = buffer_copy_info](AllocatedCommandBuffer& cmd) {
            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     LOG_ERROR("Failed to begin command buffer.");
                     return MM_RESULT_CODE;)

            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &buffer_copy_info);

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     LOG_ERROR("Failed to end command buffer.");
                     return MM_RESULT_CODE;)

            return ExecuteResult::SUCCESS;
          }),
      return MM_RESULT_CODE;)

  return ExecuteResult::SUCCESS;
}

bool MM::RenderSystem::RenderResourceBuffer::OffsetIsAlignment(
    const RenderEngine* engine, const VkDescriptorType& descriptor_type,
    const VkDeviceSize& offset, const VkDeviceSize& dynamic_offset) const {
  switch (descriptor_type) {
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      if (offset <
          engine->gpu_properties_.limits.minTexelBufferOffsetAlignment) {
        return true;
      }
      return !(offset %
               engine->gpu_properties_.limits.minTexelBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      if (offset <
          engine->gpu_properties_.limits.minUniformBufferOffsetAlignment) {
        return true;
      }
      return !(offset %
               engine->gpu_properties_.limits.minUniformBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      if (offset <
          engine->gpu_properties_.limits.minStorageBufferOffsetAlignment) {
        return true;
      }
      return !(offset %
               engine->gpu_properties_.limits.minStorageBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
      if (offset + dynamic_offset <
          engine->gpu_properties_.limits.minUniformBufferOffsetAlignment) {
        return true;
      }
      return !(offset + dynamic_offset % engine->gpu_properties_.limits
                                             .minUniformBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      if (offset + dynamic_offset <
          engine->gpu_properties_.limits.minStorageBufferOffsetAlignment) {
        return true;
      }
      return !(offset + dynamic_offset % engine->gpu_properties_.limits
                                             .minStorageBufferOffsetAlignment);
    default:
      LOG_ERROR(
          "The type referred to by descriptor_type is not currently "
          "supported.");
      return false;
  }
}

MM::RenderSystem::RenderResourceMesh::RenderResourceMesh(
    const std::string& resource_name, RenderEngine* engine,
    const std::vector<AssetType::Mesh>& meshes)
    : RenderResourceBase(resource_name),
      render_engine_(engine),
      vertex_input_state_(0) {}

MM::RenderSystem::RenderResourceMesh::RenderResourceMesh(
    RenderResourceMesh&& other) noexcept
    : RenderResourceBase(std::move(other)),
      render_engine_(other.render_engine_),
      vertex_input_state_(std::move(other.vertex_input_state_)),
      index_buffer_(std::move(other.index_buffer_)),
      index_buffer_info_(other.index_buffer_info_),
      vertex_buffer_(std::move(other.vertex_buffer_)),
      vertex_buffer_info(other.vertex_buffer_info),
      instance_buffers_(std::move(other.instance_buffers_)),
      instance_buffers_info(std::move(other.instance_buffers_info)) {}

MM::RenderSystem::RenderResourceMesh&
MM::RenderSystem::RenderResourceMesh::operator=(
    const RenderResourceMesh& other) {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(other);
  render_engine_ = other.render_engine_;
  vertex_input_state_ = other.vertex_input_state_;
  index_buffer_ = other.index_buffer_;
  index_buffer_info_ = other.index_buffer_info_;
  vertex_buffer_ = other.vertex_buffer_;
  vertex_buffer_info = other.vertex_buffer_info;
  instance_buffers_ = other.instance_buffers_;
  instance_buffers_info = other.instance_buffers_info;

  return *this;
}

MM::RenderSystem::RenderResourceMesh&
MM::RenderSystem::RenderResourceMesh::operator=(
    RenderResourceMesh&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(std::move(other));
  render_engine_ = other.render_engine_;
  vertex_input_state_ = std::move(other.vertex_input_state_);
  index_buffer_ = std::move(other.index_buffer_);
  index_buffer_info_ = other.index_buffer_info_;
  vertex_buffer_ = std::move(other.vertex_buffer_);
  vertex_buffer_info = other.vertex_buffer_info;
  instance_buffers_ = std::move(other.instance_buffers_);
  instance_buffers_info = std::move(other.instance_buffers_info);

  other.render_engine_ = nullptr;
  other.index_buffer_info_.Reset();
  other.vertex_buffer_info.Reset();

  return *this;
}

bool MM::RenderSystem::RenderResourceMesh::IsValid() const {
  return !static_cast<bool>(
      render_engine_ == nullptr || !vertex_input_state_.IsValid() ||
      !index_buffer_.IsValid() || index_buffer_info_.buffer_size_ == 0 ||
      !vertex_buffer_.IsValid() || vertex_buffer_info.buffer_size_ == 0);
}

// void MM::RenderSystem::RenderResourceTexture::RenderResourceTextureWrapper::
// RenderResourceImageWrapperDestructor(RenderResourceTextureWrapper* value) {
//   if (value == nullptr) {
//     return;
//   }
//
//   vkDestroyImageView(value->render_engine_->device_, *(value->image_view_),
//                      nullptr);
//   vkDestroySampler(value->render_engine_->device_, *(value->sampler_),
//                    nullptr);
//   vmaDestroyImage(value->render_engine_->allocator_, value->image_->image_,
//                   value->image_->allocation_);
//
//   value->render_engine_.reset();
//   value->bind_.reset();
//   value->image_view_.reset();
//   value->image_.reset();
// }
