#include "runtime/function/render/vk_render_resource.h"

#include "runtime/function/render/vk_engine.h"

std::mutex MM::RenderSystem::RenderResourceManager::sync_flag{};
std::atomic_uint32_t MM::RenderSystem::RenderResourceManager::increase_index{0};

std::shared_ptr<MM::RenderSystem::RenderResourceManager>
    MM::RenderSystem::RenderResourceManager::render_resource_manager_{nullptr};

std::shared_ptr<MM::RenderSystem::RenderResourceManager>
MM::RenderSystem::RenderResourceManager::GetInstance() {
  if (render_resource_manager_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag};
    if (!render_resource_manager_) {
      render_resource_manager_.reset(new RenderResourceManager{});
    }
  }
  return render_resource_manager_;
}

MM::RenderSystem::RenderResourceBase::RenderResourceBase()
    : resource_name_(), read_pass_set_(), write_pass_set() {}

MM::RenderSystem::RenderResourceBase::RenderResourceBase(
    const std::string& resource_name)
    : resource_name_(resource_name),
      resource_ID_(MM::RenderSystem::RenderResourceManager::increase_index),
      read_pass_set_(),
      write_pass_set() {
  if (MM::RenderSystem::RenderResourceManager::increase_index == 0) {
    ++MM ::RenderSystem::RenderResourceManager::increase_index;
  }
}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::operator=(
    const RenderResourceBase& other) {
  if (&other == this) {
    return *this;
  }
  resource_name_ = other.resource_name_;
  resource_ID_ = other.resource_ID_;
  read_pass_set_ = other.read_pass_set_;
  write_pass_set = other.write_pass_set;
  return *this;
}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::operator=(
    RenderResourceBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  resource_name_ = other.resource_name_;
  resource_ID_ = other.resource_ID_;
  read_pass_set_ = other.read_pass_set_;
  write_pass_set = other.write_pass_set;

  other.resource_name_ = std::string{};
  other.resource_ID_ = 0;
  other.read_pass_set_.reset();
  other.write_pass_set.reset();

  return *this;
}

const std::string& MM::RenderSystem::RenderResourceBase::GetResourceName()
    const {
  return resource_name_;
}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::SetResourceName(
    const std::string& new_resource_name) {
  resource_name_ = new_resource_name;
  return *this;
}

const uint32_t& MM::RenderSystem::RenderResourceBase::GetResourceID() const {
  return resource_ID_;
}

MM::RenderSystem::ResourceType
MM::RenderSystem::RenderResourceBase::GetResourceType() const {
  return ResourceType::UNDEFINED;
}

std::shared_ptr<std::unordered_set<uint32_t>>
MM::RenderSystem::RenderResourceBase::GetReadPassSet() {
  return read_pass_set_;
}

std::shared_ptr<const std::unordered_set<uint32_t>>
MM::RenderSystem::RenderResourceBase::GetReadPassSet() const {
  return read_pass_set_;
}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::AddReadPass(
    const uint32_t& read_pass_index) {
  read_pass_set_->insert(read_pass_index);
  return *this;
}

std::shared_ptr<std::unordered_set<uint32_t>>
MM::RenderSystem::RenderResourceBase::GetWritePassSet() {
  return write_pass_set;
}

std::shared_ptr<const std::unordered_set<uint32_t>>
MM::RenderSystem::RenderResourceBase::GetWritePassSet() const {
  return write_pass_set;
}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::AddWritePass(
    const uint32_t& write_pass_index) {
  write_pass_set->insert(write_pass_index);
  return *this;
}

void MM::RenderSystem::RenderResourceBase::Release() {
  resource_name_ = std::string{};
  read_pass_set_.reset();
  write_pass_set.reset();
}

void MM::RenderSystem::RenderResourceBase::Reset(
    MM::RenderSystem::RenderResourceBase* other) {
  if (other == nullptr) {
    RenderResourceBase::Release();
    return;
  }
  operator=(*other);
}

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    const std::string& resource_name, RenderEngine* engine,
    const VkDescriptorType& descriptor_type,
    const std::shared_ptr<AssetSystem::Image>& image,
    VkImageUsageFlags usages,
    const VkImageLayout& image_layout, const uint32_t& mipmap_levels,
    const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags)
    : RenderResourceBase(resource_name), render_engine_(engine) {
  if (!CheckInitParameter(engine, descriptor_type, image, mipmap_levels,
                          usages)) {
    return;
  }
  render_engine_ = engine;

  // wrapper_->bind_->binding = 0; binding will be determined when binding to
  // RenderPass.
  bind_.descriptorType = descriptor_type;
  bind_.descriptorCount = 1;
  bind_.stageFlags = VK_SHADER_STAGE_ALL;
  // wrapper_->bind_->pImmutableSamplers = nullptr; I don't know what to use.
  // I will temporarily use the default value.

  const uint32_t recommend_mipmap_level =
      static_cast<uint32_t>(
          std::floor(std::log2(max(image_info_.image_extent_.width,
                                   image_info_.image_extent_.height)))) +
      1;
  image_info_.image_extent_.width = image->GetImageWidth();
  image_info_.image_extent_.height = image->GetImageHeight();
  image_info_.image_extent_.depth = 1;
  image_info_.image_layout_ = image_layout;
  image_info_.mipmap_levels = mipmap_levels > recommend_mipmap_level
                                  ? recommend_mipmap_level
                                  : mipmap_levels;
  image_info_.can_mapped_ = Utils::CanBeMapped(memory_usage, allocation_flags);

  AllocatedBuffer stage_buffer;
  if (!LoadImageToStageBuffer(image, stage_buffer)) {
    RenderResourceTexture::Release();
    return;
  }

  if (!InitImage(stage_buffer, usages, memory_usage, allocation_flags)) {
    RenderResourceTexture::Release();
    return;
  }

  if (!GenerateMipmap()) {
    RenderResourceTexture::Release();
    return;
  }

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

  if (!InitSemaphore()) {
    RenderResourceTexture::Release();
    return;
  } }

MM::RenderSystem::RenderResourceTexture::RenderResourceTexture(
    const std::string& resource_name, RenderEngine* engine,
    const VkDescriptorSetLayoutBinding& bind,
    const AllocatedImage& image, const ImageInfo& image_info,
    const std::shared_ptr<VkImageView>& image_view,
    const std::shared_ptr<VkSampler>& sampler,
    const std::shared_ptr<VkSemaphore>& semaphore) :
    RenderResourceBase(resource_name),
    render_engine_(engine),
    bind_(bind),
    image_(image),
    image_info_(image_info),
    image_view_(image_view),
    sampler_(sampler),
    semaphore_(semaphore) {}

MM::RenderSystem::RenderResourceTexture&
MM::RenderSystem::RenderResourceTexture::operator=(
    const RenderResourceTexture& other) {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(other);
  render_engine_ = other.render_engine_;
  bind_ = other.bind_;
  image_ = other.image_;
  image_info_.image_extent_ = other.image_info_.image_extent_;
  image_info_.image_size_ = other.image_info_.image_size_;
  image_info_.image_format_ = other.image_info_.image_format_;
  image_info_.image_layout_ = other.image_info_.image_layout_;
  image_info_.mipmap_levels = other.image_info_.mipmap_levels;
  image_info_.can_mapped_ = other.image_info_.can_mapped_;
  image_view_ = other.image_view_;
  sampler_ = other.sampler_;
  semaphore_ = other.semaphore_;

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
  bind_ = other.bind_;
  image_ = other.image_;
  image_info_.image_extent_ = other.image_info_.image_extent_;
  image_info_.image_size_ = other.image_info_.image_size_;
  image_info_.image_format_ = other.image_info_.image_format_;
  image_info_.image_layout_ = other.image_info_.image_layout_;
  image_info_.mipmap_levels = other.image_info_.mipmap_levels;
  image_info_.can_mapped_ = other.image_info_.can_mapped_;
  sampler_ = other.sampler_;
  image_view_ = other.image_view_;
  semaphore_ = other.semaphore_;

  other.RenderResourceTexture::Release();
  return *this;
}

const VkExtent3D& MM::RenderSystem::RenderResourceTexture::GetExtent() const {
  return image_info_.image_extent_;
}

const VkFormat& MM::RenderSystem::RenderResourceTexture::GetFormat() const {
  return image_info_.image_format_;
}

const VkImageLayout& MM::RenderSystem::RenderResourceTexture::GetImageLayout() const {
  return image_info_.image_layout_;
}

const MM::RenderSystem::ImageInfo& MM::RenderSystem::RenderResourceTexture::
GetImageInfo() const {
  return image_info_;
}

MM::RenderSystem::RenderResourceTexture MM::RenderSystem::RenderResourceTexture
::GetCopy() const {
  return *this;
}

MM::RenderSystem::RenderResourceTexture
MM::RenderSystem::RenderResourceTexture::GetCopyWithNewImageView(
    const VkImageViewCreateInfo& image_view_create_info) {
  auto new_texture = *this;

  VkImageView temp_image_view{nullptr};
  vkCreateImageView(render_engine_->device_, &image_view_create_info, nullptr,
                    &temp_image_view);

  new_texture.image_view_ =
      MM::Utils::MakeSharedWithDestructor<VkImageView>([&engine = render_engine_](VkImageView* value) {
        if (value == nullptr) {
          return;
        }
        vkDestroyImageView(engine->device_, *value, nullptr);

        delete value;
        value = nullptr;
      }, temp_image_view);

  image_info_.image_format_ = image_view_create_info.format;
  image_info_.mipmap_levels =
      image_view_create_info.subresourceRange.levelCount;

  return new_texture;
}

MM::RenderSystem::RenderResourceTexture MM::RenderSystem::RenderResourceTexture
::GetCopyWithNewSampler(const VkSamplerCreateInfo& sampler_create_info) {
  auto new_texture = *this;
  VkSampler temp_sample{nullptr};
  vkCreateSampler(render_engine_->device_, &sampler_create_info, nullptr, &temp_sample);

  new_texture.sampler_ = MM::Utils::MakeSharedWithDestructor<VkSampler>(
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

MM::RenderSystem::RenderResourceTexture MM::RenderSystem::RenderResourceTexture
::GetCopyWithNewImageView(const VkFormat& image_format,
    const VkImageLayout& image_layout, const std::shared_ptr<VkImageView>& image_view) {
  auto new_texture = *this;

  new_texture.image_view_ = image_view;

  image_info_.image_format_ = image_format;
  image_info_.image_layout_ = image_layout;

  return new_texture;
}

MM::RenderSystem::RenderResourceTexture MM::RenderSystem::RenderResourceTexture
::GetCopyWithNewSampler(const std::shared_ptr<VkSampler>& sampler) {
  auto new_texture = *this;

  new_texture.sampler_ = sampler;

  return new_texture;
}

VkDescriptorType MM::RenderSystem::RenderResourceTexture::
GetDescriptorType() const {
  return bind_.descriptorType;
}

std::shared_ptr<VkSemaphore> MM::RenderSystem::RenderResourceTexture::
GetSemaphore() const {
  return semaphore_;
}

bool MM::RenderSystem::RenderResourceTexture::CanMapped() const {
  return image_info_.can_mapped_;
}

bool MM::RenderSystem::RenderResourceTexture::IsStorage() const {
  return bind_.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
}

bool MM::RenderSystem::RenderResourceTexture::HaveSample() const {
  return !IsStorage();
}

bool MM::RenderSystem::RenderResourceTexture::IsValid() const {
  bool result = render_engine_ != nullptr && bind_.descriptorCount != 0 &&
                image_.IsValid() && image_view_ != nullptr;
  if (Utils::DescriptorTypeIsImageSampler(bind_.descriptorType)) {
    result = result && sampler_ != nullptr;
  }
  return result;
}

void MM::RenderSystem::RenderResourceTexture::Release() {
  RenderResourceBase::Release();
  if (!IsValid()) {
    return;
  }

  semaphore_.reset();
  sampler_.reset();
  image_view_.reset();
  bind_.descriptorCount = 0;
  image_info_.image_extent_ = {0, 0, 0};
  image_info_.image_size_ = 0;
  image_info_.image_format_ = VkFormat{};
  image_info_.image_layout_ = VkImageLayout{};
  image_info_.mipmap_levels = 1;
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
        "released, and resources will not be reset.")
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

const VkDeviceSize& MM::RenderSystem::RenderResourceTexture::GetSize() const {
  return RenderResourceBase::GetSize();
  return image_info_.image_size_;
}

bool MM::RenderSystem::RenderResourceTexture::IsArray() const { return false; }

bool MM::RenderSystem::RenderResourceTexture::CheckInitParameter(
    const RenderEngine* engine, const VkDescriptorType& descriptor_type,
    const std::shared_ptr<AssetSystem::Image>& image,
    const uint32_t& mipmap_level, VkImageUsageFlags usages) const {
  if (engine == nullptr) {
    LOG_ERROR("The incoming engine parameter pointer is null.")
    return false;
  }
  if (!image->IsValid()) {
    LOG_ERROR("The incoming image parameter is not available.")
    return false;
  }
  if (!engine->IsValid()) {
    LOG_ERROR("The rendering engine is not available.")
    return false;
  }
  if (!Utils::DescriptorTypeIsImage(descriptor_type)) {
    LOG_ERROR("Parameter descriptor_type is not  for texture adaptation.")
    return false;
  }
  if (mipmap_level == 0) {
    LOG_ERROR(
        "The value of the parameter mipmap level is 0, and the parameter is "
        "incorrect.")
    return false;
  }
  if (usages & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    LOG_ERROR(
        "Depth and stencil test do not match the RenderResourceTexture "
        "resource type. You should create a RenderResourceFrameBuffer resource "
        "to hold the depth and stencil test resources.")
    return false;
  }
  if (usages & VK_IMAGE_USAGE_STORAGE_BIT &&
      descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
    LOG_ERROR(
        "VkImageUsageFlags specifies that the image is a storage image, but "
        "VkDescriptorType is not a storage image. ")
    return false;
  }
  if (descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE &&
      !(usages & VK_IMAGE_USAGE_STORAGE_BIT)) {
    LOG_ERROR(
        "VkDescriptorType specifies that the image is a storage image, but "
        "VkImageUsageFlags is not a storage image. ")
    return false;
  }
  return true;
}

bool MM::RenderSystem::RenderResourceTexture::LoadImageToStageBuffer(
    const std::shared_ptr<AssetSystem::Image>& image,
     AllocatedBuffer& stage_buffer) {
  const void* pixels = image->GetImagePixels().get();
  image_info_.image_size_ = image->GetImageSize();

  switch (image->GetImageFormat()) {
    case AssetSystem::ImageFormat::GREY:
      image_info_.image_format_ = VK_FORMAT_R8_SRGB;
      break;
    case AssetSystem::ImageFormat::GREY_ALPHA:
      image_info_.image_format_ = VK_FORMAT_R8G8_SRGB;
      break;
    case AssetSystem::ImageFormat::RGB:
      image_info_.image_format_ = VK_FORMAT_R8G8B8_SRGB;
      break;
    case AssetSystem::ImageFormat::RGB_ALPHA:
      image_info_.image_format_ = VK_FORMAT_R8G8B8A8_SRGB;
      break;
    case AssetSystem::ImageFormat::UNDEFINED:
      LOG_ERROR("Image loading failed, image format not defined.")
      return false;
  }

  stage_buffer = render_engine_->CreateBuffer(image_info_.image_size_,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VMA_MEMORY_USAGE_AUTO,
                                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

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
  memcpy(stage_data, pixels, image_info_.image_size_);
  vmaUnmapMemory(render_engine_->allocator_, stage_buffer.GetAllocation());

  return true;
}

bool MM::RenderSystem::RenderResourceTexture::InitImage(
    const AllocatedBuffer& stage_buffer,
    VkImageUsageFlags usages,
    const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags) {
  VkImageCreateInfo image_create_info =
      MM::RenderSystem::Utils::GetImageCreateInfo(
          image_info_.image_format_, usages, image_info_.image_extent_);
  image_create_info.mipLevels = image_info_.mipmap_levels;

  const VmaAllocationCreateInfo image_allocator_create_info =
      MM::RenderSystem::Utils::GetVmaAllocationCreateInfo(
          memory_usage, allocation_flags);

  VkImage temp_image{nullptr};
  VmaAllocation temp_allocation{nullptr};

  usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  if (Utils::DescriptorTypeIsImageSampler(bind_.descriptorType)) {
    usages |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }
  VK_CHECK(vmaCreateImage(render_engine_->allocator_, &image_create_info,
                          &image_allocator_create_info, &temp_image,
                          &temp_allocation, nullptr),
           LOG_ERROR("Failed to create VkImage.");
           return false;)

  image_ =
      AllocatedImage{render_engine_->allocator_, temp_image, temp_allocation};

  if (!render_engine_->RecordAndSubmitSingleTimeCommand(
          CommandBufferType::GRAPH,
          [&image = image_, &stage = stage_buffer,
           &image_extent = image_info_.image_extent_,
           &image_layout = image_info_.image_layout_](VkCommandBuffer& cmd) {
            Utils::AddTransferImageCommands(
                cmd, image, ImageTransferMode::INIT_TO_TRANSFER_DESTINATION);

            auto copy_region = Utils::GetBufferToImageCopyRegion(
                VK_IMAGE_ASPECT_COLOR_BIT, image_extent);

            vkCmdCopyImageToBuffer(cmd, image.GetImage(),
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   stage.GetBuffer(), 1, &copy_region);
          },
          true)) {
    LOG_ERROR("Copying image data to the GPU failed.");
    return false;
  }
  return true;
}

bool MM::RenderSystem::RenderResourceTexture::GenerateMipmap() {
  const bool execute_result = render_engine_->RecordAndSubmitSingleTimeCommand(
      CommandBufferType::GRAPH,
      [&](VkCommandBuffer& cmd) {
        VkImageMemoryBarrier2 barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image_.GetImage();
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        const auto dependency_info = Utils::GetImageDependencyInfo(barrier);

        int32_t mip_width =
            static_cast<int32_t>(image_info_.image_extent_.width);
        int32_t mip_height =
            static_cast<int32_t>(image_info_.image_extent_.height);

        for (uint32_t i = 1; i < image_info_.mipmap_levels; ++i) {
          barrier.subresourceRange.baseMipLevel = i - 1;
          barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
          barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
          barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
          barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
          barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
          barrier.dstStageMask = VK_ACCESS_2_TRANSFER_READ_BIT;

          vkCmdPipelineBarrier2(cmd, &dependency_info);

          barrier.subresourceRange.baseMipLevel = i;
          barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
          barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
          barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
          barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
          barrier.srcAccessMask = 0;
          barrier.dstStageMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

          vkCmdPipelineBarrier2(cmd, &dependency_info);

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

          vkCmdBlitImage(
              cmd, image_.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image_.GetImage(),
              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

          barrier.subresourceRange.baseMipLevel = i - 1;
          barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
          barrier.newLayout = image_info_.image_layout_;
          barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
          barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
          barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
          barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

          vkCmdPipelineBarrier2(cmd, &dependency_info);

          if (mip_width > 1) {
            mip_width >>= 1;
          }
          if (mip_height > 1) {
            mip_height >>= 1;
          }
        }

        barrier.subresourceRange.baseMipLevel = image_info_.mipmap_levels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = image_info_.image_layout_;
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

        vkCmdPipelineBarrier2(cmd, &dependency_info);
      },
      true);
  if (!execute_result) {
    LOG_ERROR("Mipmap generation failed.")
  }
  return execute_result;
}

bool MM::RenderSystem::RenderResourceTexture::InitImageView() {
  auto image_view_create_info = Utils::GetImageViewCreateInfo(
      image_, image_info_.image_format_, VK_IMAGE_VIEW_TYPE_2D,
      VK_IMAGE_ASPECT_COLOR_BIT);
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount =
      image_info_.mipmap_levels;

  VkImageView temp_image_view{nullptr};
  VK_CHECK(vkCreateImageView(render_engine_->device_, &image_view_create_info,
             nullptr, &temp_image_view),
           LOG_ERROR("failed to create VkImageView。");
           RenderResourceTexture::Release(); return false;)
  image_view_ = MM::Utils::MakeSharedWithDestructor<VkImageView>(
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
  sampler_create_info.maxLod = static_cast<float>(image_info_.mipmap_levels);

  VkSampler temp_sampler{nullptr};
  VK_CHECK(vkCreateSampler(render_engine_->device_, &sampler_create_info,
             nullptr, &temp_sampler),
           LOG_ERROR("Failed to create VkSampler");
           return false;)

  sampler_ = MM::Utils::MakeSharedWithDestructor<VkSampler>(
      [engine = render_engine_](VkSampler* value) {
        if (value == nullptr) {
          return;
        }
        vkDestroySampler(engine->device_, *value, nullptr);

        delete value;
        value = nullptr;
      }, temp_sampler);

  return true;
}

bool MM::RenderSystem::RenderResourceTexture::InitSemaphore() {
  const auto semaphore_create_info = Utils::GetSemaphoreCreateInfo();

  VkSemaphore temp_semaphore{nullptr};

  VK_CHECK(vkCreateSemaphore(render_engine_->device_, &semaphore_create_info,
                             nullptr, &temp_semaphore),
           LOG_ERROR("Failed to create VkSemaphore.");
           return false;)

  semaphore_ = MM::Utils::MakeSharedWithDestructor<VkSemaphore>(
      [&engine = render_engine_](VkSemaphore* value) {
        if (value == nullptr) {
          return;
        }
        
        vkDestroySemaphore(engine->device_, *value, nullptr);

        delete value;
        value = nullptr;
      },
      temp_semaphore);
  return true;
}

MM::RenderSystem::RenderResourceBuffer::RenderResourceBuffer(
    const std::string& resource_name, RenderEngine* engine,
    const VkDescriptorType& descriptor_type,
    VkBufferUsageFlags buffer_usage,
    const VkDeviceSize& size,
    const VkDeviceSize& offset,
    const VkDeviceSize& dynamic_offset,
    void* data,
    const VkDeviceSize& copy_offset,
    const VkDeviceSize& copy_size,
    const VkBufferCreateFlags& buffer_flags,
    const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags) : RenderResourceBase(resource_name),
      render_engine_(engine),
      buffer_info_{size, offset,dynamic_offset, Utils::CanBeMapped(memory_usage, allocation_flags)} {
  if (!CheckInitParameter(engine, descriptor_type, buffer_usage, size, offset,
                          dynamic_offset, data, copy_offset, copy_size)) {
    RenderResourceBuffer::Release();
    return;
  }

  render_engine_ = engine;

  bind_.descriptorType = descriptor_type;
  bind_.descriptorCount = 1;
  bind_.pImmutableSamplers = nullptr;
  bind_.stageFlags = VK_SHADER_STAGE_ALL;

  // If there is data that needs to be copied to a buffer and the buffer cannot be mapped,
  // VK_BUFFER_USAGE_TRANSFER_DST_BIT will be automatically added to the buffer_usage to
  // prevent exceptions from being thrown.
  if (data != nullptr && !buffer_info_.can_mapped_) {
    buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  }

  if (!InitBuffer(buffer_usage, buffer_flags, memory_usage, allocation_flags)) {
    RenderResourceBuffer::Release();
    return;
  }

  if (!CopyDataToBuffer(data, offset, size)) {
    RenderResourceBuffer::Release();
    return;
  }

  if (!InitSemaphore()) {
    RenderResourceBuffer::Release();
    return;
  }
}

MM::RenderSystem::RenderResourceBuffer& MM::RenderSystem::RenderResourceBuffer::
operator=(const RenderResourceBuffer& other) {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(other);

  render_engine_ = other.render_engine_;
  bind_ = other.bind_;
  buffer_info_ = other.buffer_info_;
  buffer_ = other.buffer_;
  semaphore_ = other.semaphore_;

  return *this;
}

MM::RenderSystem::RenderResourceBuffer& MM::RenderSystem::RenderResourceBuffer::
operator=(RenderResourceBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  RenderResourceBase::operator=(std::move(other));

  render_engine_ = other.render_engine_;
  bind_ = other.bind_;
  buffer_info_ = other.buffer_info_;
  buffer_ = other.buffer_;
  semaphore_ = other.semaphore_;

  other.RenderResourceBuffer::Release();

  return *this;
}

const VkDescriptorType& MM::RenderSystem::RenderResourceBuffer::
GetDescriptorType() const {
  return bind_.descriptorType;
}

MM::RenderSystem::RenderResourceBuffer MM::RenderSystem::RenderResourceBuffer::
GetCopy() const {
  return *this;
}

MM::RenderSystem::RenderResourceBuffer MM::RenderSystem::RenderResourceBuffer::
GetCopyWithNewOffset(const VkDeviceSize& new_offset) const {
  VkDeviceSize new_offset_in = new_offset;
  if (IsDynamic()) {
    if (new_offset_in > UINT64_MAX - buffer_info_.dynamic_offset_) {
      LOG_WARN("New offset is too large.")
      new_offset_in = buffer_info_.buffer_size_ - buffer_info_.dynamic_offset_;
    }
    if (new_offset_in + buffer_info_.dynamic_offset_ > buffer_info_.buffer_size_) {
      LOG_WARN("New offset is too large.")
      new_offset_in = buffer_info_.buffer_size_ - buffer_info_.dynamic_offset_;
    }
  } else {
    if (new_offset_in > buffer_info_.buffer_size_) {
      new_offset_in = buffer_info_.buffer_size_;
    }
  }
  
  auto new_buffer = GetCopy();
  new_buffer.buffer_info_.offset_ = new_offset_in;

  return new_buffer;
}

MM::RenderSystem::RenderResourceBuffer MM::RenderSystem::RenderResourceBuffer::
GetCopyWithNewDynamicOffset(const VkDeviceSize& new_dynamic_offset) const {
  VkDeviceSize new_dynamic_offset_in = new_dynamic_offset;

  if (new_dynamic_offset > UINT64_MAX - buffer_info_.offset_) {
    LOG_WARN("New_Dynamic_offset is too larger.")
    new_dynamic_offset_in = buffer_info_.buffer_size_ - buffer_info_.offset_;
  }
  if (new_dynamic_offset_in + buffer_info_.offset_ > buffer_info_.buffer_size_) {
    LOG_WARN("New_Dynamic_offset is too large.")
    new_dynamic_offset_in = buffer_info_.buffer_size_ - buffer_info_.offset_;
  }

  auto new_buffer = GetCopy();
  new_buffer.buffer_info_.dynamic_offset_ = new_dynamic_offset_in;

  return new_buffer;
}

MM::RenderSystem::RenderResourceBuffer MM::RenderSystem::RenderResourceBuffer::
GetCopyWithNewOffsetAndDynamicOffset(const VkDeviceSize& new_offset,
    const VkDeviceSize& new_dynamic_offset) const {
  VkDeviceSize new_offset_in = new_offset;
  VkDeviceSize new_dynamic_offset_in = new_dynamic_offset;
  if (IsDynamic()) {
    if (new_offset_in > UINT64_MAX - buffer_info_.dynamic_offset_) {
      LOG_WARN("The sum of new offset and new dynamic offset is too large.")
      new_offset_in = buffer_info_.buffer_size_ - buffer_info_.dynamic_offset_;
      new_dynamic_offset_in = buffer_info_.dynamic_offset_;
    }
    if (new_offset_in + new_dynamic_offset_in > buffer_info_.buffer_size_) {
      LOG_WARN("The sum of new offset and new dynamic offset is too large.")
      new_offset_in = buffer_info_.buffer_size_ - buffer_info_.dynamic_offset_;
      new_dynamic_offset_in = buffer_info_.dynamic_offset_;
    }
  } else {
    if (new_offset_in > buffer_info_.buffer_size_) {
      new_offset_in = buffer_info_.buffer_size_;
    }
  }

  auto new_buffer = GetCopy();
  new_buffer.buffer_info_.offset_ = new_offset_in;

  if (IsDynamic()) {
    new_buffer.buffer_info_.dynamic_offset_ = new_dynamic_offset_in;
  }

  return new_buffer;
}

MM::RenderSystem::RenderResourceBuffer MM::RenderSystem::RenderResourceBuffer::
GetCopyWithDynamicBuffer(const VkDeviceSize& new_offset,
    const VkDeviceSize& new_dynamic_offset) const {
  if (IsTexel()) {
    return *this;
  }

  if (IsDynamic()) {
    return *this;
  }

  auto new_buffer = GetCopy();
  if (IsStorage()) {
    new_buffer.bind_.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
  } else {
    new_buffer.bind_.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  }

  if (new_offset < UINT64_MAX - new_dynamic_offset && new_offset + new_dynamic_offset < buffer_info_.buffer_size_) {
    new_buffer.buffer_info_.offset_ = new_offset;
    new_buffer.buffer_info_.dynamic_offset_ = new_dynamic_offset;
  }

  return new_buffer;
}

bool MM::RenderSystem::RenderResourceBuffer::IsDynamic() const {
  return Utils::DescriptorTypeIsDynamicBuffer(bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceBuffer::IsStorage() const {
  return Utils::DescriptorTypeIsStorageBuffer(bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceBuffer::IsUniform() const {
  return Utils::DescriptorTypeIsUniformBuffer(bind_.descriptorType);
}

bool MM::RenderSystem::RenderResourceBuffer::IsTexel() const {
  return Utils::DescriptorTypeIsTexel(bind_.descriptorType);
}

const VkDeviceSize& MM::RenderSystem::RenderResourceBuffer::GetOffset() const {
  return buffer_info_.offset_;
}

const VkDeviceSize&
MM::RenderSystem::RenderResourceBuffer::GetDynamicOffset() const {
  return buffer_info_.dynamic_offset_;
}

const bool& MM::RenderSystem::RenderResourceBuffer::CanMapped() const {
  return buffer_info_.can_mapped_;
}

const MM::RenderSystem::BufferInfo& MM::RenderSystem::RenderResourceBuffer::
GetBufferInfo() const {
  return buffer_info_;
}

bool MM::RenderSystem::RenderResourceBuffer::IsValid() const {
  return render_engine_ != nullptr && bind_.descriptorCount != 0 && buffer_.
         IsValid() && semaphore_.use_count() != 0 && buffer_info_.buffer_size_ != 0;
}

void MM::RenderSystem::RenderResourceBuffer::Release() {
  RenderResourceBase::Release();

  render_engine_ = nullptr;
  bind_.descriptorCount = 0;
  buffer_info_.offset_ = 0;
  buffer_info_.dynamic_offset_ = 0;
  buffer_info_.can_mapped_ = false;
  buffer_info_.buffer_size_ = 0;
  buffer_.Release();
  semaphore_.reset();
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
        "released, and resources will not be reset.")
    Release();
    return;
  }
  RenderResourceBase::Reset(other);
  operator=(*dynamic_cast<RenderResourceBuffer*>(other));
}

uint32_t MM::RenderSystem::RenderResourceBuffer::UseCount() const {
  return buffer_.UseCount();
}

MM::RenderSystem::ResourceType MM::RenderSystem::RenderResourceBuffer::
GetResourceType() const {
  return ResourceType::BUFFER;
}

const VkDeviceSize& MM::RenderSystem::RenderResourceBuffer::GetSize() const {
  return buffer_info_.buffer_size_;
}

bool MM::RenderSystem::RenderResourceBuffer::IsArray() const { return false; }

bool MM::RenderSystem::RenderResourceBuffer::CheckInitParameter(
    const RenderEngine* engine, const VkDescriptorType& descriptor_type,
    const VkBufferUsageFlags& buffer_usage, const VkDeviceSize& size,
    const VkDeviceSize& offset, const VkDeviceSize& dynamic_offset,
    const void* data, const VkDeviceSize& copy_offset,
    const VkDeviceSize& copy_size) const {
  if (engine == nullptr) {
    LOG_ERROR("The incoming engine parameter pointer is null.")
    return false;
  }
  if (!engine->IsValid()) {
    LOG_ERROR("The rendering engine is not available.")
    return false;
  }
  
  if (size > 65536 && Utils::DescriptorTypeIsUniformBuffer(descriptor_type)) {
    LOG_ERROR("The maximum allowable uniform buffer size for most desktop clients is 64KB (1024 * 64=65536).")
    return false;
  }

  if (!Utils::DescriptorTypeIsBuffer(descriptor_type)) {
    LOG_ERROR("Parameter descriptor_type is not  for buffer adaptation.")
    return false;
  }
  if (buffer_usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.")
      return false;
    }
  } else if (buffer_usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.")
      return false;
    }
  } else if (buffer_usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
        descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.")
      return false;
    }
  } else if (buffer_usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
    if (descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER &&
        descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
      LOG_ERROR("The buffer_usage and descriptor_type do not match.")
      return false;
    }
  } else {
    LOG_ERROR("The buffer_usage not currently supported.")
    return false;
  }

  if (size == 0) {
    LOG_ERROR("Buffer size must great than 0.")
    return false;
  }

  if (Utils::DescriptorTypeIsDynamicBuffer(descriptor_type)) {
    if (offset > ULLONG_MAX - dynamic_offset) {
      LOG_ERROR("The sum of the offset and dynamic_offset too lager.")
      return false;
    }
    if (offset + dynamic_offset > size) {
      LOG_ERROR("The sum of offset and dynamic_offset is greater than size.")
      return false;
    }
  } else {
    if (offset > size) {
      LOG_ERROR("The offset is greater than size.")
      return false;
    }
  }

  if (!OffsetIsAlignment(engine, descriptor_type, offset, dynamic_offset)) {
    LOG_ERROR("The offset value does not meet memory alignment requirements.")
    return false;
  }

  if (data != nullptr) {
    if (copy_offset > ULLONG_MAX - copy_offset) {
      LOG_ERROR("The sum of the copy_offset and copy_offset too lager.")
      return false;
    }
    if (copy_offset + copy_size > size) {
      LOG_ERROR("The sum of copy_offset and copy_size is greater than size.")
      return false;
    }
  }

  return true;
}

bool MM::RenderSystem::RenderResourceBuffer::InitBuffer(
    const VkBufferUsageFlags& buffer_usage,
    const VkBufferCreateFlags& buffer_flags, const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags) {
  const auto buffer_create_info =
      Utils::GetBufferCreateInfo(buffer_info_.buffer_size_, buffer_usage, buffer_flags);
  const auto allocation_create_info =
      Utils::GetVmaAllocationCreateInfo(memory_usage, allocation_flags);

  VkBuffer temp_buffer{nullptr};
  VmaAllocation temp_allocation{nullptr};

  VK_CHECK(vmaCreateBuffer(render_engine_->allocator_, &buffer_create_info,
                           &allocation_create_info, &temp_buffer,
                           &temp_allocation, nullptr),
           LOG_ERROR("Failed to create VkBuffer.");
           return false;)

  buffer_ =
      AllocatedBuffer{render_engine_->allocator_, temp_buffer, temp_allocation};

  return false;
}

bool MM::RenderSystem::RenderResourceBuffer::CopyDataToBuffer(void* data,
    const VkDeviceSize& offset, const VkDeviceSize& size) {
  if (data == nullptr) {
    return true;
  }

  if (buffer_info_.can_mapped_) {
    char* buffer_ptr{nullptr};
    VK_CHECK(vmaMapMemory(render_engine_->allocator_, buffer_.GetAllocation(),
                          (void**)&buffer_ptr),
             LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
             return false;)

    buffer_ptr = buffer_ptr + offset;
    memcpy(buffer_ptr, data, size);

    vmaUnmapMemory(render_engine_->allocator_, buffer_.GetAllocation());

    return true;
  }

  const auto stage_buffer = render_engine_->CreateBuffer(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

  if (!stage_buffer.IsValid()) {
    LOG_ERROR("Failed to create stage buffer.")
    return false;
  }

  const auto buffer_copy_region = Utils::GetCopyBufferRegion(size, 0, offset);
  std::vector<VkBufferCopy2> buffer_copy_regions{buffer_copy_region};
  auto buffer_copy_info = Utils::GetCopyBufferInfo(
      stage_buffer, buffer_, buffer_copy_regions);

  void* stage_buffer_ptr{nullptr};

  VK_CHECK(vmaMapMemory(render_engine_->allocator_,
                        stage_buffer.GetAllocation(),
                        &stage_buffer_ptr),
           LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
           return false;)

  memcpy(stage_buffer_ptr, data, size);

  vmaUnmapMemory(render_engine_->allocator_, stage_buffer.GetAllocation());

  if (!render_engine_->RecordAndSubmitSingleTimeCommand(
      CommandBufferType::GRAPH,
      [&buffer_copy_info = buffer_copy_info](VkCommandBuffer& cmd) {
            vkCmdCopyBuffer2(cmd, &buffer_copy_info);
  }, true)) {
    LOG_ERROR("Failed to copy data form stage_buffer to buffer_.")
    return false;
  }

  

  return true;
}

bool MM::RenderSystem::RenderResourceBuffer::InitSemaphore() {
  const auto semaphore_create_info = Utils::GetSemaphoreCreateInfo();

  VkSemaphore temp_semaphore{nullptr};
  VK_CHECK(vkCreateSemaphore(render_engine_->device_, &semaphore_create_info,
                             nullptr, &temp_semaphore),
           LOG_ERROR("Failed to create VkSemaphore.");
           return false;)

  semaphore_ = MM::Utils::MakeSharedWithDestructor<VkSemaphore>(
      [engine = render_engine_](VkSampler* value) {
        if (value == nullptr) {
          return;
        }
        vkDestroySampler(engine->device_, *value, nullptr);
        delete value;
        value = nullptr;
      },
      temp_semaphore);

  return true;
}

bool MM::RenderSystem::RenderResourceBuffer::OffsetIsAlignment(
    const RenderEngine* engine, const VkDescriptorType& descriptor_type, const VkDeviceSize& offset,
    const VkDeviceSize& dynamic_offset) const {
  switch (descriptor_type) {
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      if (offset < engine->gpu_properties_.limits.minTexelBufferOffsetAlignment) {
        return true;
      }
      return !(offset %
               engine->gpu_properties_.limits.minTexelBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      if (offset < engine->gpu_properties_.limits.minUniformBufferOffsetAlignment) {
        return true;
      }
      return !(offset %
               engine->gpu_properties_.limits.minUniformBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      if (offset < engine->gpu_properties_.limits.minStorageBufferOffsetAlignment) {
        return true;
      }
      return !(offset %
               engine->gpu_properties_.limits.minStorageBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
      if (offset + dynamic_offset < engine->gpu_properties_.limits.minUniformBufferOffsetAlignment) {
        return true;
      }
      return !(offset + dynamic_offset %
               engine->gpu_properties_.limits.minUniformBufferOffsetAlignment);
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      if (offset + dynamic_offset < engine->gpu_properties_.limits.minStorageBufferOffsetAlignment) {
        return true;
      }
      return !(offset + dynamic_offset %
               engine->gpu_properties_.limits.minStorageBufferOffsetAlignment);
    default:
      LOG_ERROR("The type referred to by descriptor_type is not currently supported.")
      return false;
  }
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
