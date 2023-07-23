//
// Created by beimingxianyu on 23-7-2.
//

#include "runtime/function/render/AllocatedBuffer.h"

#include <vulkan/vulkan_core.h>

#include <vector>

#include "RenderResourceDataBase.h"
#include "RenderResourceDataID.h"
#include "runtime/function/render/vk_engine.h"
#include "vk_type_define.h"
#include "vk_utils.h"

MM::RenderSystem::AllocatedBufferWrapper::~AllocatedBufferWrapper() {
  Release();
}

MM::RenderSystem::AllocatedBufferWrapper::AllocatedBufferWrapper(
    const VmaAllocator& allocator, const VkBuffer& buffer,
    const VmaAllocation& allocation)
    : allocator_(allocator), buffer_(buffer), allocation_(allocation) {
  if (allocator_ == nullptr || buffer_ == nullptr || allocation_ == nullptr) {
    allocator_ = nullptr;
    buffer_ = nullptr;
    allocation_ = nullptr;
  }
}

const VmaAllocator_T* MM::RenderSystem::AllocatedBufferWrapper::GetAllocator()
    const {
  return allocator_;
}

const VkBuffer_T* MM::RenderSystem::AllocatedBufferWrapper::GetBuffer() const {
  return buffer_;
}

const VmaAllocation_T* MM::RenderSystem::AllocatedBufferWrapper::GetAllocation()
    const {
  return allocation_;
}

bool MM::RenderSystem::AllocatedBufferWrapper::IsValid() const {
  return allocator_ != nullptr && buffer_ != nullptr && allocation_ != nullptr;
}

VmaAllocator MM::RenderSystem::AllocatedBufferWrapper::GetAllocator() {
  return allocator_;
}

VkBuffer MM::RenderSystem::AllocatedBufferWrapper::GetBuffer() {
  return buffer_;
}

VmaAllocation MM::RenderSystem::AllocatedBufferWrapper::GetAllocation() {
  return allocation_;
}

void MM::RenderSystem::AllocatedBufferWrapper::Release() {
  if (allocator_ == nullptr) {
    return;
  }

  vmaDestroyBuffer(allocator_, buffer_, allocation_);

  allocator_ = nullptr;
  buffer_ = nullptr;
  allocation_ = nullptr;
}

void MM::RenderSystem::AllocatedBufferWrapper::SetAllocator(
    VmaAllocator allocator) {
  allocator_ = allocator;
}

void MM::RenderSystem::AllocatedBufferWrapper::SetBuffer(VkBuffer buffer) {
  buffer_ = buffer;
}

void MM::RenderSystem::AllocatedBufferWrapper::SetAllocation(
    VmaAllocation allocation) {
  allocation_ = allocation;
}

MM::RenderSystem::AllocatedBufferWrapper::AllocatedBufferWrapper(
    MM::RenderSystem::AllocatedBufferWrapper&& other) noexcept
    : allocator_(other.allocator_),
      buffer_(other.buffer_),
      allocation_(other.allocation_) {
  allocator_ = nullptr;
  buffer_ = nullptr;
  allocation_ = nullptr;
}

MM::RenderSystem::AllocatedBufferWrapper&
MM::RenderSystem::AllocatedBufferWrapper::operator=(
    MM::RenderSystem::AllocatedBufferWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  allocator_ = other.allocator_;
  buffer_ = other.buffer_;
  allocation_ = other.allocation_;

  other.allocator_ = nullptr;
  other.buffer_ = nullptr;
  other.allocation_ = nullptr;
}

MM::RenderSystem::ResourceType
MM::RenderSystem::AllocatedBuffer::GetResourceType() const {
  return ResourceType::BUFFER;
}

VkDeviceSize MM::RenderSystem::AllocatedBuffer::GetSize() const {
  return buffer_data_info_.buffer_create_info_.size_;
}

bool MM::RenderSystem::AllocatedBuffer::IsArray() const { return false; }

bool MM::RenderSystem::AllocatedBuffer::CanWrite() const {
  switch (buffer_data_info_.buffer_create_info_.usage_) {
    case VK_BUFFER_USAGE_STORAGE_BUFFER_BIT:
    case VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT:
      return true;
    default:
      return false;
  }
}

MM::ExecuteResult MM::RenderSystem::AllocatedBuffer::CheckInitParameters(
    MM::RenderSystem::RenderEngine* render_engine,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    return ExecuteResult ::OBJECT_IS_INVALID;
  }

  MM_CHECK_WITHOUT_LOG(Utils::CheckVkBufferCreateInfo(vk_buffer_create_info),
                       return ExecuteResult::INITIALIZATION_FAILED;)

  MM_CHECK_WITHOUT_LOG(
      Utils::CheckVmaAllocationCreateInfo(vma_allocation_create_info),
      return ExecuteResult::INITIALIZATION_FAILED;)

  if (*vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetGraphQueueIndex() &&
      *vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetTransformQueueIndex() &&
      *vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetPresentQueueIndex() &&
      *vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetComputeQueueIndex()) {
    MM_LOG_ERROR("The queue family index is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return ExecuteResult ::SUCCESS;
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    const std::string& name, MM::RenderSystem::RenderEngine* render_engine,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info)
    : RenderResourceDataBase(name, RenderResourceDataID()),
      render_engine_(render_engine),
      buffer_data_info_(),
      wrapper_() {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(render_engine, vk_buffer_create_info,
                               vma_allocation_create_info),
           return;)
#endif

  buffer_data_info_.SetBufferCreateInfo(*vk_buffer_create_info);
  buffer_data_info_.SetAllocationCreateInfo(*vma_allocation_create_info);
  buffer_data_info_.buffer_sub_resource_attributes_.emplace_back(
      0, buffer_data_info_.buffer_create_info_.size_,
      buffer_data_info_.buffer_create_info_.queue_family_indices_[0]);

  RenderResourceDataAttributeID render_resource_data_attribute_ID;
  MM_CHECK(buffer_data_info_.GetRenderResourceDataAttributeID(
               render_resource_data_attribute_ID),
           MM_LOG_ERROR("Failed to get RenderResourceDataAttributeID.");
           RenderResourceDataBase::Release(); render_engine_ = nullptr;
           buffer_data_info_.Reset(); return;)
  SetRenderResourceDataID(RenderResourceDataID{
      GetObjectID().GetHash(), render_resource_data_attribute_ID});

  MM_CHECK(InitBuffer(render_engine, *vk_buffer_create_info,
                      *vma_allocation_create_info),
           RenderResourceDataBase::Release();
           render_engine_ = render_engine; buffer_data_info_.Reset(); return;)
}

MM::ExecuteResult MM::RenderSystem::AllocatedBuffer::InitBuffer(
    MM::RenderSystem::RenderEngine* render_engine,
    const VkBufferCreateInfo& vk_buffer_create_info,
    const VmaAllocationCreateInfo& vma_allocation_create_info) {
  VkBuffer temp_buffer{nullptr};
  VmaAllocation temp_allocation{nullptr};

  MM_VK_CHECK(
      vmaCreateBuffer(render_engine->GetAllocator(), &vk_buffer_create_info,
                      &vma_allocation_create_info, &temp_buffer,
                      &temp_allocation, nullptr),
      MM_LOG_ERROR("Failed to create VkBuffer.");
      return ExecuteResult ::INITIALIZATION_FAILED;)

  wrapper_.SetAllocator(render_engine->GetAllocator());
  wrapper_.SetBuffer(temp_buffer);
  wrapper_.SetAllocation(temp_allocation);

  return ExecuteResult ::SUCCESS;
}

VmaAllocator MM::RenderSystem::AllocatedBuffer::GetAllocator() {
  return wrapper_.GetAllocator();
}

VkBuffer MM::RenderSystem::AllocatedBuffer::GetBuffer() {
  return wrapper_.GetBuffer();
}

VmaAllocation MM::RenderSystem::AllocatedBuffer::GetAllocation() {
  return wrapper_.GetAllocation();
}

void MM::RenderSystem::AllocatedBuffer::Release() {
  RenderResourceDataBase::Release();
  render_engine_ = nullptr;
  wrapper_.Release();
  buffer_data_info_.Reset();
}

bool MM::RenderSystem::AllocatedBuffer::IsValid() const {
  return render_engine_ != nullptr && render_engine_->IsValid() &&
         wrapper_.IsValid();
}

const std::vector<MM::RenderSystem::BufferSubResourceAttribute>&
MM::RenderSystem::AllocatedBuffer::GetSubResourceAttributes() const {
  return buffer_data_info_.buffer_sub_resource_attributes_;
}

const VmaAllocation_T* MM::RenderSystem::AllocatedBuffer::GetAllocation()
    const {
  return wrapper_.GetAllocation();
}

const VkBuffer_T* MM::RenderSystem::AllocatedBuffer::GetBuffer() const {
  return wrapper_.GetBuffer();
}

const VmaAllocator_T* MM::RenderSystem::AllocatedBuffer::GetAllocator() const {
  return wrapper_.GetAllocator();
}

const MM::RenderSystem::BufferDataInfo&
MM::RenderSystem::AllocatedBuffer::GetBufferInfo() const {
  return buffer_data_info_;
}

bool MM::RenderSystem::AllocatedBuffer::IsTransformDest() const {
  return Utils::IsTransformDestBuffer(
      buffer_data_info_.buffer_create_info_.usage_);
}

bool MM::RenderSystem::AllocatedBuffer::IsTransformSrc() const {
  return Utils::IsTransformSrcBuffer(
      buffer_data_info_.buffer_create_info_.usage_);
}

bool MM::RenderSystem::AllocatedBuffer::CanMapped() const {
  return Utils::CanBeMapped(buffer_data_info_.allocation_create_info_.usage_,
                            buffer_data_info_.allocation_create_info_.flags_);
}

const VkDeviceSize& MM::RenderSystem::AllocatedBuffer::GetBufferSize() const {
  return buffer_data_info_.buffer_create_info_.size_;
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    const std::string& name,
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    RenderEngine* render_engine,
    const MM::RenderSystem::BufferDataInfo& buffer_data_info,
    VmaAllocator vma_allocator, VkBuffer vk_buffer, VmaAllocation vk_allocation)
    : RenderResourceDataBase(name, render_resource_data_ID),
      render_engine_(render_engine),
      buffer_data_info_(buffer_data_info),
      wrapper_(vma_allocator, vk_buffer, vk_allocation) {}

MM::ExecuteResult MM::RenderSystem::AllocatedBuffer::CopyDataToBuffer(
    std::uint64_t dest_offset, void* data, std::uint64_t src_offset,
    std::uint64_t size) {
  if (!IsValid()) {
    return MM::Utils::ExecuteResult ::OBJECT_IS_INVALID;
  }

  if (data == nullptr) {
    return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  // The copied data cannot exceed the buffer range.
  if (size > buffer_data_info_.buffer_create_info_.size_ - dest_offset) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  if (CanMapped()) {
    char* buffer_ptr{nullptr};
    MM_VK_CHECK_WITHOUT_LOG(
        vmaMapMemory(GetAllocator(), GetAllocation(),
                     reinterpret_cast<void**>(&buffer_ptr)),
        MM_LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
        return ExecuteResult::UNDEFINED_ERROR;)

    char* data_ptr = reinterpret_cast<char*>(data) + src_offset;

    buffer_ptr = buffer_ptr + src_offset;
    memcpy(buffer_ptr, data_ptr, size);

    vmaUnmapMemory(GetAllocator(), GetAllocation());

    if (IsAssetResource()) {
      MarkThisUseForWrite();
    }

    return ExecuteResult::SUCCESS;
  }

  if (!IsTransformDest()) {
    MM_LOG_ERROR(
        "If you want to copy data into an unmapped AllocatedBuffer, it must be "
        "an AllocatedBuffer that can be specified as the transform "
        "destination.");
    return MM::Utils::ExecuteResult ::OPERATION_NOT_SUPPORTED;
  }

  AllocatedBuffer stage_buffer{};
  MM_CHECK(GetRenderEnginePtr()->CreateStageBuffer(
               size, render_engine_->GetTransformQueueIndex(), stage_buffer),
           MM_LOG_ERROR("Failed to create stage buffer.");
           return MM::Utils::ExecuteResult ::CREATE_OBJECT_FAILED;)

  const auto buffer_copy_region =
      Utils::GetVkBufferCopy2(size, src_offset, dest_offset);
  std::vector<VkBufferCopy2> buffer_copy_regions{buffer_copy_region};
  auto buffer_copy_info =
      Utils::GetVkCopyBufferInfo2(stage_buffer, *this, buffer_copy_regions);

  void* stage_buffer_ptr{nullptr};

  MM_VK_CHECK(vmaMapMemory(render_engine_->GetAllocator(),
                           stage_buffer.GetAllocation(), &stage_buffer_ptr),
              MM_LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
              return Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)

  memcpy(stage_buffer_ptr, data, size);

  vmaUnmapMemory(render_engine_->GetAllocator(), stage_buffer.GetAllocation());

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, 1,
          [&buffer_copy_info = buffer_copy_info, render_engine = render_engine_,
           this_buffer = this](AllocatedCommandBuffer& cmd) {
            if (buffer_copy_info.pRegions->size >
                this_buffer->GetSize() - buffer_copy_info.pRegions->dstOffset) {
              return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
            }

            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to begin command buffer.");
                     return MM_RESULT_CODE;)

            std::vector<VkBufferMemoryBarrier2> barriers;
            std::uint64_t affected_sub_resource_index = 0;
            std::uint64_t affected_sub_resource_count = 0;
            auto& sub_resource_attributes =
                this_buffer->GetSubResourceAttributes();
            for (std::uint64_t i = 0; i < sub_resource_attributes.size(); ++i) {
              if (sub_resource_attributes[i].GetChunkInfo().GetOffset() <
                  buffer_copy_info.pRegions->srcOffset) {
                affected_sub_resource_index = i;
                for (; i < sub_resource_attributes.size(); ++i) {
                  if (sub_resource_attributes[i].GetChunkInfo().GetOffset() <
                      buffer_copy_info.pRegions->srcOffset +
                          buffer_copy_info.pRegions->size) {
                    barriers.emplace_back(Utils::GetVkBufferMemoryBarrier2(
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_WRITE_BIT,
                        sub_resource_attributes[i].GetQueueIndex(),
                        render_engine->GetTransformQueueIndex(), *this_buffer,
                        sub_resource_attributes[i].GetChunkInfo().GetOffset(),
                        sub_resource_attributes[i].GetChunkInfo().GetSize()));
                    ++affected_sub_resource_count;
                  } else {
                    break;
                  }
                }
                break;
              }
            }

            VkDependencyInfo dependency_info = Utils::GetVkDependencyInfo(
                0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0);

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &buffer_copy_info);

            for (std::uint64_t count = 0; count != affected_sub_resource_count;
                 ++count) {
              barriers[count].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
              barriers[count].srcQueueFamilyIndex =
                  render_engine->GetTransformQueueIndex();
              barriers[count].dstQueueFamilyIndex =
                  sub_resource_attributes[affected_sub_resource_index + count]
                      .GetQueueIndex();
            }

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to end command buffer.");
                     return MM_RESULT_CODE;)

            return ExecuteResult::SUCCESS;
          },
          std::vector<RenderResourceDataID>{
              stage_buffer.GetRenderResourceDataID()}),
      return MM_RESULT_CODE;)

  if (IsAssetResource()) {
    MarkThisUseForWrite();
  }

  return ExecuteResult::SUCCESS;
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    MM::RenderSystem::AllocatedBuffer&& other) noexcept
    : RenderResourceDataBase(std::move(other)),
      render_engine_(other.render_engine_),
      buffer_data_info_(std::move(other.buffer_data_info_)),
      wrapper_(std::move(other.wrapper_)) {
  other.render_engine_ = nullptr;
}

MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::AllocatedBuffer::operator=(
    MM::RenderSystem::AllocatedBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  RenderResourceDataBase::operator=(std::move(other));
  render_engine_ = other.render_engine_;
  buffer_data_info_ = std::move(buffer_data_info_);
  wrapper_ = std::move(other.wrapper_);

  other.render_engine_ = nullptr;

  return *this;
}

MM::ExecuteResult MM::RenderSystem::AllocatedBuffer::CopyAssetDataToBuffer(
    MM::AssetSystem::AssetManager::HandlerType asset_handler,
    std::uint32_t queue_index) {
  if (!IsValid() && !asset_handler.IsValid()) {
    return MM::Utils::ExecuteResult ::OBJECT_IS_INVALID;
  }

  if (IsAssetResource()) {
    MM_LOG_ERROR(
        "It is not supported to rewrite asset data to an AllocatedBuffer that "
        "has already written asset data.");
    return MM::Utils::ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  auto datas = asset_handler.GetAsset().GetDatas();
  std::uint64_t asset_datas_size = 0;
  for (const auto& data : datas) {
    asset_datas_size += data.second;
  }
  if (asset_datas_size > GetBufferSize()) {
    MM_LOG_ERROR("Asset size lager than buffer size.");
    return MM::Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  if (buffer_data_info_.buffer_sub_resource_attributes_.size() == 1 &&
      buffer_data_info_.buffer_sub_resource_attributes_[0].GetQueueIndex() ==
          queue_index &&
      CanMapped()) {
    char* buffer_ptr{nullptr};
    MM_VK_CHECK_WITHOUT_LOG(
        vmaMapMemory(GetAllocator(), GetAllocation(),
                     reinterpret_cast<void**>(&buffer_ptr)),
        MM_LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
        return ExecuteResult::UNDEFINED_ERROR;)

    std::uint64_t src_offset = 0;
    for (const auto& data : datas) {
      char* data_ptr = reinterpret_cast<char*>(data.first);

      buffer_ptr = buffer_ptr + src_offset;
      memcpy(buffer_ptr, data_ptr, data.second);
      src_offset += data.second;
    }

    vmaUnmapMemory(GetAllocator(), GetAllocation());
  }

  if (!IsTransformDest()) {
    MM_LOG_ERROR(
        "If you want to copy data into an unmapped AllocatedBuffer, it must be "
        "an AllocatedBuffer that can be specified as the transform "
        "destination.");
    return MM::Utils::ExecuteResult ::OPERATION_NOT_SUPPORTED;
  }

  CommandBufferType command_buffer_type =
      Utils::ChooseCommandBufferType(render_engine_, queue_index);
  if (command_buffer_type == CommandBufferType::UNDEFINED) {
    return MM::Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  AllocatedBuffer stage_buffer{};
  MM_CHECK(GetRenderEnginePtr()->CreateStageBuffer(asset_datas_size,
                                                   queue_index, stage_buffer),
           MM_LOG_ERROR("Failed to create stage buffer.");
           return MM::Utils::ExecuteResult ::CREATE_OBJECT_FAILED;)

  std::vector<BufferSubResourceAttribute> old_sub_resource_attribute;
  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          command_buffer_type, 1,
          [this_buffer = this, &datas, queue_index, &asset_datas_size,
           &stage_buffer,
           &old_sub_resource_attribute](AllocatedCommandBuffer& cmd) mutable {
            std::vector<VkBufferMemoryBarrier2> barrier;
            for (const auto& sub_resource_attribute :
                 this_buffer->GetSubResourceAttributes()) {
              barrier.emplace_back(Utils::GetVkBufferMemoryBarrier2(
                  VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                  VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                  VK_ACCESS_2_TRANSFER_WRITE_BIT,
                  sub_resource_attribute.GetQueueIndex(), queue_index,
                  this_buffer->GetBuffer(),
                  sub_resource_attribute.GetChunkInfo().GetOffset(),
                  sub_resource_attribute.GetChunkInfo().GetSize()));
            }

            VkDependencyInfo dependency_info{
                Utils::GetVkDependencyInfo(nullptr, &barrier, nullptr, 0)};

            void* stage_buffer_ptr{nullptr};
            VkDeviceSize src_offset{0};
            MM_VK_CHECK(
                vmaMapMemory(this_buffer->GetAllocator(),
                             stage_buffer.GetAllocation(), &stage_buffer_ptr),
                MM_LOG_ERROR("Unable to obtain a pointer mapped to a buffer");
                return Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)
            for (const auto& asset_data : datas) {
              memcpy(static_cast<char*>(stage_buffer_ptr) + src_offset,
                     asset_data.first, asset_data.second);
            }

            vmaUnmapMemory(this_buffer->GetAllocator(),
                           stage_buffer.GetAllocation());

            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to begin command buffer.");
                     return MM_RESULT_CODE;)

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            old_sub_resource_attribute = std::move(
                this_buffer->buffer_data_info_.buffer_sub_resource_attributes_);
            this_buffer->buffer_data_info_.buffer_sub_resource_attributes_ =
                std::vector<BufferSubResourceAttribute>{
                    BufferSubResourceAttribute{0, this_buffer->GetBufferSize(),
                                               queue_index}};

            auto buffer_copy_region =
                Utils::GetVkBufferCopy2(asset_datas_size, 0, 0);
            auto buffer_copy_info = Utils::GetVkCopyBufferInfo2(
                nullptr, stage_buffer.GetBuffer(), this_buffer->GetBuffer(), 1,
                &buffer_copy_region);

            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &buffer_copy_info);

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to end command buffer.");
                     return MM_RESULT_CODE;)

            return MM::Utils::ExecuteResult ::SUCCESS;
          },
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
      MM_LOG_ERROR("Failed to copy asset data to AllocatedBuffer.");
      if (!old_sub_resource_attribute.empty()) {
        buffer_data_info_.buffer_sub_resource_attributes_ =
            std::move(old_sub_resource_attribute);
      } return MM_RESULT_CODE;)

  MarkThisIsAssetResource();

  return MM::Utils::ExecuteResult ::SUCCESS;
}

MM::RenderSystem::RenderEngine*
MM::RenderSystem::AllocatedBuffer::GetRenderEnginePtr() {
  return render_engine_;
}

const MM::RenderSystem::RenderEngine*
MM::RenderSystem::AllocatedBuffer::GetRenderEnginePtr() const {
  return render_engine_;
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    const std::string& name, MM::RenderSystem::RenderEngine* render_engine,
    MM::AssetSystem::AssetManager::HandlerType asset_handler,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info)
    : RenderResourceDataBase(name, RenderResourceDataID()),
      render_engine_(render_engine),
      buffer_data_info_(),
      wrapper_() {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParametersWhenInitFromAnAsset(render_engine, asset_handler,
                                                  vk_buffer_create_info,
                                                  vma_allocation_create_info),
           return;)
#endif

  buffer_data_info_.SetBufferCreateInfo(*vk_buffer_create_info);
  buffer_data_info_.SetAllocationCreateInfo(*vma_allocation_create_info);
  buffer_data_info_.buffer_sub_resource_attributes_.emplace_back(
      0, buffer_data_info_.buffer_create_info_.size_,
      buffer_data_info_.buffer_create_info_.queue_family_indices_[0]);

  RenderResourceDataAttributeID render_resource_data_attribute_ID;
  MM_CHECK(buffer_data_info_.GetRenderResourceDataAttributeID(
               render_resource_data_attribute_ID),
           MM_LOG_ERROR("Failed to get RenderResourceDataAttributeID.");
           RenderResourceDataBase::Release(); render_engine_ = nullptr;
           buffer_data_info_.Reset(); return;)
  SetRenderResourceDataID(RenderResourceDataID{
      GetObjectID().GetHash(), render_resource_data_attribute_ID});

  MM_CHECK(InitBuffer(render_engine, *vk_buffer_create_info,
                      *vma_allocation_create_info),
           RenderResourceDataBase::Release();
           render_engine_ = render_engine; buffer_data_info_.Reset(); return;)

  MM_CHECK(CopyAssetDataToBuffer(asset_handler,
                                 GetSubResourceAttributes()[0].GetQueueIndex()),
           RenderResourceDataBase::Release();
           render_engine_ = render_engine; buffer_data_info_.Reset();
           wrapper_.Release(); return;)
}

const MM::RenderSystem::BufferCreateInfo&
MM::RenderSystem::AllocatedBuffer::GetBufferCreateInfo() const {
  return buffer_data_info_.buffer_create_info_;
}

const MM::RenderSystem::AllocationCreateInfo&
MM::RenderSystem::AllocatedBuffer::GetAllocationCreateInfo() const {
  return buffer_data_info_.allocation_create_info_;
}

const MM::RenderSystem::BufferDataInfo&
MM::RenderSystem::AllocatedBuffer::GetBufferDataInfo() const {
  return buffer_data_info_;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedBuffer::CheckInitParametersWhenInitFromAnAsset(
    RenderEngine* render_engine,
    AssetSystem::AssetManager::HandlerType asset_handler,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  MM_CHECK_WITHOUT_LOG(CheckInitParameters(render_engine, vk_buffer_create_info,
                                           vma_allocation_create_info),
                       return MM_RESULT_CODE;)

  if (!asset_handler.IsValid()) {
    MM_LOG_ERROR("The asset handler is invalid.");
    return MM::Utils::ExecuteResult ::INITIALIZATION_FAILED;
  }
  if (!Utils::CanBeMapped(vma_allocation_create_info->usage,
                          vma_allocation_create_info->flags) &&
      !(vk_buffer_create_info->usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
    MM_LOG_ERROR(
        "Load data from asset must can be mapped or specify transform "
        "destination.");
    return MM::Utils::ExecuteResult ::INITIALIZATION_FAILED;
  }

  return MM::Utils::ExecuteResult ::SUCCESS;
}
MM::Utils::ExecuteResult
MM::RenderSystem::AllocatedBuffer::GetVkBufferMemoryBarriber2(
    VkDeviceSize offset, VkDeviceSize size,
    VkPipelineStageFlags2 src_stage_mask, VkAccessFlags2 src_access_mask,
    VkPipelineStageFlags2 dst_stage_mask, VkAccessFlags2 dst_access_mask,
    MM::RenderSystem::QueueIndex new_index,
    std::vector<VkBufferMemoryBarrier2>& barriers) const {
  if (size == 0 || offset + size >= GetSize()) {
    return MM::Utils::ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  VkBufferMemoryBarrier2 barrier = Utils::GetVkBufferMemoryBarrier2(
      src_stage_mask, src_access_mask, dst_stage_mask, dst_access_mask,
      UINT32_MAX, new_index, const_cast<VkBuffer>(GetBuffer()), UINT64_MAX,
      UINT64_MAX);
  for (std::uint64_t i = 0;
       i != buffer_data_info_.buffer_sub_resource_attributes_.size(); ++i) {
    if (buffer_data_info_.buffer_sub_resource_attributes_[i]
            .GetChunkInfo()
            .GetOffset() <= offset) {
      for (; i != buffer_data_info_.buffer_sub_resource_attributes_.size();
           ++i) {
        VkDeviceSize sub_resource_end =
                         buffer_data_info_.buffer_sub_resource_attributes_[i]
                             .GetChunkInfo()
                             .GetSize() +
                         buffer_data_info_.buffer_sub_resource_attributes_[i]
                             .GetChunkInfo()
                             .GetOffset(),
                     dest_end = offset + size;
        if (buffer_data_info_.buffer_sub_resource_attributes_[i]
                .GetChunkInfo()
                .GetOffset() < dest_end) {
          barrier.srcQueueFamilyIndex =
              buffer_data_info_.buffer_sub_resource_attributes_[i]
                  .GetQueueIndex();
          barrier.offset = buffer_data_info_.buffer_sub_resource_attributes_[i]
                               .GetChunkInfo()
                               .GetOffset();
          if (sub_resource_end >= dest_end) {
            barrier.size =
                dest_end - buffer_data_info_.buffer_sub_resource_attributes_[i]
                               .GetChunkInfo()
                               .GetOffset();
            barriers.emplace_back(barrier);
            break;
          } else {
            barrier.size = buffer_data_info_.buffer_sub_resource_attributes_[i]
                               .GetChunkInfo()
                               .GetSize();
            barriers.emplace_back(barrier);
          }
        }
      }
      break;
    }
  }

  return MM::Utils::ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::AllocatedBuffer::GetCopy(
    const std::string& new_name,
    MM::RenderSystem::AllocatedBuffer& new_allocated_buffer) const {
  if (IsValid()) {
    return MM::Utils::ExecuteResult::OBJECT_IS_INVALID;
  }

  VkBuffer new_buffer{nullptr};
  VmaAllocation new_allocation{nullptr};

  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  VkBufferCreateInfo buffer_create_info{
      MM::RenderSystem::Utils::GetVkBufferCreateInfo(
          GetBufferCreateInfo().next_, GetBufferCreateInfo().flags_,
          GetBufferCreateInfo().size_, GetBufferCreateInfo().usage_,
          GetBufferCreateInfo().sharing_mode_, 1, &transform_queue_index)};
  buffer_data_info_.buffer_create_info_.GetVkBufferCreateInfo();
  VmaAllocationCreateInfo allocation_create_info =
      buffer_data_info_.allocation_create_info_.GetVmaAllocationCreateInfo();

  MM_VK_CHECK(
      vmaCreateBuffer(const_cast<VmaAllocator>(wrapper_.GetAllocator()),
                      &buffer_create_info, &allocation_create_info, &new_buffer,
                      &new_allocation, nullptr),
      MM_LOG_ERROR("Failed to create VkBuffer.");
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, 1,
          [this_buffer = this, new_buffer](AllocatedCommandBuffer& cmd) {
            if (this_buffer->GetSubResourceAttributes().size() == 1) {
              return this_buffer->AddCopyBufferCommandsWhenOneSubResource(
                  cmd, new_buffer);
            } else {
              return this_buffer->AddCopyBufferCommandseWhenMultSubResource(
                  cmd, new_buffer);
            }
          },
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
      MM_LOG_ERROR("Failed to copy data to new buffer.");
      return MM_RESULT_CODE;)

  new_allocated_buffer =
      AllocatedBuffer{new_name,
                      GetRenderResourceDataID(),
                      render_engine_,
                      buffer_data_info_,
                      const_cast<VmaAllocator>(wrapper_.GetAllocator()),
                      new_buffer,
                      new_allocation};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::AllocatedBuffer::GetCopy(
    std::vector<std::string>& new_names,
    std::vector<AllocatedBuffer>& new_allocated_buffers) const {
  if ((new_names.empty()) ||
      (new_names.size() != new_allocated_buffers.size())) {
    MM_LOG_ERROR("The size of input parameters are error.");
    return MM::Utils::ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  if (IsValid()) {
    return MM::Utils::ExecuteResult::OBJECT_IS_INVALID;
  }

  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  VkBufferCreateInfo buffer_create_info{
      MM::RenderSystem::Utils::GetVkBufferCreateInfo(
          GetBufferCreateInfo().next_, GetBufferCreateInfo().flags_,
          GetBufferCreateInfo().size_, GetBufferCreateInfo().usage_,
          GetBufferCreateInfo().sharing_mode_, 1, &transform_queue_index)};
  VmaAllocationCreateInfo allocation_create_info =
      buffer_data_info_.allocation_create_info_.GetVmaAllocationCreateInfo();

  std::vector<VkBuffer> new_buffers(new_allocated_buffers.size());
  std::vector<VmaAllocation> new_allocations(new_allocated_buffers.size());
  for (std::uint64_t i = 0; i != new_allocated_buffers.size(); ++i) {
    MM_VK_CHECK(
        vmaCreateBuffer(const_cast<VmaAllocator>(wrapper_.GetAllocator()),
                        &buffer_create_info, &allocation_create_info,
                        &new_buffers[i], &new_allocations[i], nullptr),
        for (std::uint64_t j = 0; j != i; ++j) {
          vmaDestroyBuffer(const_cast<VmaAllocator>(wrapper_.GetAllocator()),
                           new_buffers[j], new_allocations[j]);
        } MM_LOG_ERROR("Failed to create VkBuffer.");
        return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE);)
  }

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, 1,
          [this_buffer = this, &new_buffers](AllocatedCommandBuffer& cmd) {
            if (this_buffer->GetSubResourceAttributes().size() == 1) {
              return this_buffer->AddCopyBufferCommandsWhenOneSubResource(
                  cmd, new_buffers);
            } else {
              return this_buffer->AddCopyBufferCommandseWhenMultSubResource(
                  cmd, new_buffers);
            }
          },
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
      MM_LOG_ERROR("Failed to copy data to new buffer.");
      return MM_RESULT_CODE;)

  for (std::uint64_t i = 0; i != new_allocated_buffers.size(); ++i) {
    new_allocated_buffers[i] =
        AllocatedBuffer{new_names[i],
                        GetRenderResourceDataID(),
                        render_engine_,
                        buffer_data_info_,
                        const_cast<VmaAllocator>(wrapper_.GetAllocator()),
                        new_buffers[i],
                        new_allocations[i]};
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandsWhenOneSubResource(
    MM::RenderSystem::AllocatedCommandBuffer& cmd, VkBuffer new_buffer) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  const BufferSubResourceAttribute& sub_resource =
      GetSubResourceAttributes()[0];
  std::array<VkBufferMemoryBarrier2, 2> barriers{
      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
          transform_queue_index, *this, sub_resource.GetOffset(),
          sub_resource.GetSize()),
      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
          transform_queue_index, const_cast<VkBuffer>(new_buffer),
          sub_resource.GetOffset(), sub_resource.GetSize())};
  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  VkBufferCopy2 buffer_copy{
      MM::RenderSystem::Utils::GetVkBufferCopy2(GetBufferSize(), 0, 0)};
  VkCopyBufferInfo2 copy_buffer_info{
      MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
          nullptr, const_cast<VkBuffer>(GetBuffer()), new_buffer, 1,
          &buffer_copy)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begin command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &copy_buffer_info);

  barriers[0].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  std::swap(barriers[0].srcQueueFamilyIndex, barriers[0].dstQueueFamilyIndex);
  barriers[1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[1].dstQueueFamilyIndex = barriers[0].dstQueueFamilyIndex;
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer.");
           return MM_RESULT_CODE;)

  return MM::Utils::ExecuteResult::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandseWhenMultSubResource(
    MM::RenderSystem::AllocatedCommandBuffer& cmd, VkBuffer new_buffer) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();

  std::vector<VkBufferMemoryBarrier2> barriers{};
  barriers.reserve(GetSubResourceAttributes().size() * 2);
  // old image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
        transform_queue_index, const_cast<VkBuffer>(GetBuffer()),
        sub_resource.GetOffset(), sub_resource.GetSize()));
  }
  // new image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
        transform_queue_index, new_buffer, sub_resource.GetOffset(),
        sub_resource.GetSize()));
  }
  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begine command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkBufferCopy2> buffer_copy{};
  buffer_copy.reserve(GetSubResourceAttributes().size());
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    buffer_copy.emplace_back(MM::RenderSystem::Utils::GetVkBufferCopy2(
        sub_resource.GetSize(), sub_resource.GetOffset(),
        sub_resource.GetOffset()));
  }
  VkCopyBufferInfo2 copy_buffer_info{
      MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
          nullptr, const_cast<VkBuffer>(GetBuffer()), new_buffer,
          buffer_copy.size(), buffer_copy.data())};
  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &copy_buffer_info);

  std::uint64_t sub_resource_count = GetSubResourceAttributes().size();
  for (std::uint64_t i = 0; i != sub_resource_count; ++i) {
    std::swap(barriers[i].dstQueueFamilyIndex, barriers[i].srcQueueFamilyIndex);
    barriers[i].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[i].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    barriers[i + sub_resource_count].srcQueueFamilyIndex =
        barriers[i].srcQueueFamilyIndex;
    barriers[i + sub_resource_count].dstQueueFamilyIndex =
        barriers[i].dstQueueFamilyIndex;
    barriers[i + sub_resource_count].srcAccessMask =
        VK_ACCESS_2_TRANSFER_WRITE_BIT;
  }
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandsWhenOneSubResource(
    AllocatedCommandBuffer& cmd, std::vector<VkBuffer>& new_buffers) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  const BufferSubResourceAttribute& sub_resource =
      GetSubResourceAttributes()[0];
  std::vector<VkBufferMemoryBarrier2> barriers{};
  barriers.reserve(1 + new_buffers.size());
  barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
      transform_queue_index, *this, sub_resource.GetOffset(),
      sub_resource.GetSize()));
  for (VkBuffer new_buffer : new_buffers) {
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
        transform_queue_index, new_buffer, sub_resource.GetOffset(),
        sub_resource.GetSize()));
  }
  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  VkBufferCopy2 buffer_copy{
      MM::RenderSystem::Utils::GetVkBufferCopy2(GetBufferSize(), 0, 0)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begin command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  for (VkBuffer new_buffer : new_buffers) {
    VkCopyBufferInfo2 copy_buffer_info{
        MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
            nullptr, const_cast<VkBuffer>(GetBuffer()), new_buffer, 1,
            &buffer_copy)};
    vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &copy_buffer_info);
  }

  barriers[0].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  std::swap(barriers[0].srcQueueFamilyIndex, barriers[0].dstQueueFamilyIndex);
  for (std::uint64_t i = 1; i != barriers.size(); ++i) {
    barriers[i].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[i].dstQueueFamilyIndex = barriers[i].dstQueueFamilyIndex;
  }
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer.");
           return MM_RESULT_CODE;)

  return MM::Utils::ExecuteResult::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandseWhenMultSubResource(
    AllocatedCommandBuffer& cmd, std::vector<VkBuffer>& new_buffers) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();

  std::uint64_t new_buffer_number = new_buffers.size();
  std::vector<VkBufferMemoryBarrier2> barriers{};
  barriers.reserve(GetSubResourceAttributes().size() * 2);
  // old image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
        transform_queue_index, const_cast<VkBuffer>(GetBuffer()),
        sub_resource.GetOffset(), sub_resource.GetSize()));
  }
  // new image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    for (VkBuffer new_buffer : new_buffers) {
      barriers.emplace_back(MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
          transform_queue_index, new_buffer, sub_resource.GetOffset(),
          sub_resource.GetSize()));
    }
  }

  VkDependencyInfo dependency_info{MM::RenderSystem::Utils::GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to begine command buffer.");
           return MM_RESULT_CODE;)

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkBufferCopy2> buffer_copy{};
  buffer_copy.reserve(GetSubResourceAttributes().size());
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    buffer_copy.emplace_back(MM::RenderSystem::Utils::GetVkBufferCopy2(
        sub_resource.GetSize(), sub_resource.GetOffset(),
        sub_resource.GetOffset()));
  }

  for (VkBuffer new_buffer : new_buffers) {
    VkCopyBufferInfo2 copy_buffer_info{
        MM::RenderSystem::Utils::GetVkCopyBufferInfo2(
            nullptr, const_cast<VkBuffer>(GetBuffer()), new_buffer,
            buffer_copy.size(), buffer_copy.data())};
    vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &copy_buffer_info);
  }

  std::uint64_t sub_resource_count = GetSubResourceAttributes().size();
  for (std::uint64_t i = 0; i != sub_resource_count; ++i) {
    std::swap(barriers[i].dstQueueFamilyIndex, barriers[i].srcQueueFamilyIndex);
    barriers[i].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barriers[i].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    for (std::uint64_t j = 0; j != new_buffers.size(); ++j) {
      barriers[i + sub_resource_count + j + i * new_buffer_number]
          .srcQueueFamilyIndex = barriers[i].srcQueueFamilyIndex;
      barriers[i + sub_resource_count + j + i * new_buffer_number]
          .dstQueueFamilyIndex = barriers[i].dstQueueFamilyIndex;
      barriers[i + sub_resource_count + j + i * new_buffer_number]
          .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    }
  }
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
           MM_LOG_FATAL("Failed to end command buffer");
           return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedBuffer::TransformSubResourceAttribute(
    const std::vector<BufferSubResourceAttribute>& new_sub_resource_attribute) {
  if (!IsValid()) {
    return ExecuteResult ::OBJECT_IS_INVALID;
  }
  if (new_sub_resource_attribute ==
      buffer_data_info_.buffer_sub_resource_attributes_) {
    return ExecuteResult ::SUCCESS;
  }

  MM_CHECK(CheckTransformInputParameter(new_sub_resource_attribute),
           MM_LOG_ERROR("New sub resource attribute is error.");
           return MM_RESULT_CODE;)

  MM_CHECK(
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, 1,
          [this_buffer = this,
           &new_sub_resource_attribute](AllocatedCommandBuffer& cmd) {
            std::vector<VkBufferMemoryBarrier2> current_to_transform_barriers{},
                transform_to_new_barriers{};

            std::uint64_t new_sub_resource_index = 0;
            for (const auto& old_sub_resource :
                 this_buffer->GetSubResourceAttributes()) {
              VkDeviceSize
                  old_sub_resource_end =
                      old_sub_resource.GetOffset() + old_sub_resource.GetSize(),
                  new_sub_resource_end =
                      new_sub_resource_attribute[new_sub_resource_index]
                          .GetOffset() +
                      new_sub_resource_attribute[new_sub_resource_index]
                          .GetSize();
              if (new_sub_resource_attribute[new_sub_resource_index]
                      .GetQueueIndex() != old_sub_resource.GetQueueIndex()) {
                VkDeviceSize
                    transform_offset =
                        old_sub_resource.GetOffset() >
                                new_sub_resource_attribute
                                    [new_sub_resource_index]
                                        .GetOffset()
                            ? old_sub_resource.GetOffset()
                            : new_sub_resource_attribute[new_sub_resource_index]
                                  .GetOffset(),
                    transform_size =
                        old_sub_resource_end < new_sub_resource_end
                            ? old_sub_resource_end - transform_offset
                            : new_sub_resource_end - transform_offset;
                current_to_transform_barriers.emplace_back(
                    MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_READ_BIT,
                        old_sub_resource.GetQueueIndex(),
                        this_buffer->render_engine_->GetTransformQueueIndex(),
                        this_buffer->GetBuffer(), transform_offset,
                        transform_size));
                transform_to_new_barriers.emplace_back(
                    MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_READ_BIT,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_WRITE_BIT,
                        this_buffer->render_engine_->GetTransformQueueIndex(),
                        new_sub_resource_attribute[new_sub_resource_index]
                            .GetQueueIndex(),
                        this_buffer->GetBuffer(), transform_offset,
                        transform_size));
              }

              if (new_sub_resource_end == old_sub_resource_end) {
                ++new_sub_resource_index;
                continue;
              }

              while (new_sub_resource_end < old_sub_resource_end) {
                ++new_sub_resource_index;
                new_sub_resource_end =
                    new_sub_resource_attribute[new_sub_resource_index]
                        .GetOffset() +
                    new_sub_resource_attribute[new_sub_resource_index]
                        .GetSize();
                if (new_sub_resource_attribute[new_sub_resource_index]
                        .GetQueueIndex() != old_sub_resource.GetQueueIndex()) {
                  VkDeviceSize transform_offset =
                                   old_sub_resource.GetOffset() >
                                           new_sub_resource_attribute
                                               [new_sub_resource_index]
                                                   .GetOffset()
                                       ? old_sub_resource.GetOffset()
                                       : new_sub_resource_attribute
                                             [new_sub_resource_index]
                                                 .GetOffset(),
                               transform_size =
                                   old_sub_resource_end < new_sub_resource_end
                                       ? old_sub_resource_end - transform_offset
                                       : new_sub_resource_end -
                                             transform_offset;
                  current_to_transform_barriers.emplace_back(
                      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_READ_BIT,
                          old_sub_resource.GetQueueIndex(),
                          this_buffer->render_engine_->GetTransformQueueIndex(),
                          this_buffer->GetBuffer(), transform_offset,
                          transform_size));
                  transform_to_new_barriers.emplace_back(
                      MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_READ_BIT,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_WRITE_BIT,
                          this_buffer->render_engine_->GetTransformQueueIndex(),
                          new_sub_resource_attribute[new_sub_resource_index]
                              .GetQueueIndex(),
                          this_buffer->GetBuffer(), transform_offset,
                          transform_size));
                }

                if (new_sub_resource_end == old_sub_resource_end) {
                  ++new_sub_resource_index;
                  break;
                }
              }
            }

            VkDependencyInfo dependency_info1{
                MM::RenderSystem::Utils::GetVkDependencyInfo(
                    0, nullptr, current_to_transform_barriers.size(),
                    current_to_transform_barriers.data(), 0, nullptr, 0)},
                dependency_info2{MM::RenderSystem::Utils::GetVkDependencyInfo(
                    0, nullptr, transform_to_new_barriers.size(),
                    transform_to_new_barriers.data(), 0, nullptr, 0)};

            MM_CHECK(MM::RenderSystem::Utils::BeginCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to begin command buffer.");
                     return MM_RESULT_CODE;)

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info1);
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info2);

            MM_CHECK(MM::RenderSystem::Utils::EndCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to end command buffer.");
                     return MM_RESULT_CODE;)

            this_buffer->MarkThisUseForWrite();

            this_buffer->buffer_data_info_.buffer_sub_resource_attributes_ =
                std::move(new_sub_resource_attribute);

            return ExecuteResult ::SUCCESS;
          },
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
      MM_LOG_ERROR("Failed to transform sub resource attribute.");
      return MM_RESULT_CODE;)

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::AllocatedBuffer::CheckTransformInputParameter(
    const std::vector<BufferSubResourceAttribute>& new_sub_resource_attribute)
    const {
  QueueIndex graph_queue_index = render_engine_->GetGraphQueueIndex(),
             compute_queue_index = render_engine_->GetComputeQueueIndex(),
             transform_queue_index = render_engine_->GetTransformQueueIndex(),
             present_queue_index = render_engine_->GetPresentQueueIndex();

  // [start, end)
  VkDeviceSize start = UINT64_MAX, size = 0;
  for (const auto& sub_resource : new_sub_resource_attribute) {
    // Check queue index is valid
    if (sub_resource.GetQueueIndex() != graph_queue_index &&
        sub_resource.GetQueueIndex() != compute_queue_index &&
        sub_resource.GetQueueIndex() != transform_queue_index &&
        sub_resource.GetQueueIndex() != present_queue_index) {
      return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    // Check overlap
    if ((sub_resource.GetOffset() < start &&
         sub_resource.GetOffset() + sub_resource.GetSize() > start) ||
        (sub_resource.GetOffset() >= start &&
         sub_resource.GetOffset() < start + size)) {
      return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }
    if (sub_resource.GetOffset() < start) {
      start = sub_resource.GetOffset();
    }
    if (sub_resource.GetOffset() + sub_resource.GetSize() > start + size) {
      size = sub_resource.GetOffset() + sub_resource.GetSize() - start;
    }
  }
  // Check that all ranges are covered.
  if (start != 0 || size != GetBufferSize()) {
    return ExecuteResult ::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  return ExecuteResult ::SUCCESS;
}

// MM::ExecuteResult MM::RenderSystem::AllocatedBuffer::TransformQueueFamily(
//     const BufferChunkInfo& buffer_chunk_info,
//     std::uint32_t new_queue_family_index) {
//   if (buffer_chunk_info.GetOffset() + buffer_chunk_info.GetSize() >
//       buffer_data_info_.buffer_create_info_.size_) {
//     return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
//   }
//
//   CommandBufferType command_buffer_type;
//   if (new_queue_family_index == render_engine_->GetGraphQueueIndex()) {
//     command_buffer_type = CommandBufferType::GRAPH;
//   } else if (new_queue_family_index ==
//              render_engine_->GetTransformQueueIndex()) {
//     command_buffer_type = CommandBufferType::TRANSFORM;
//   } else if (new_queue_family_index ==
//   render_engine_->GetComputeQueueIndex()) {
//     command_buffer_type = CommandBufferType::COMPUTE;
//   } else {
//     return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
//   }
//
//   std::vector<BufferSubResourceAttribute>
//   new_buffer_sub_resource_attribute{};
//
//   MM_CHECK(
//       render_engine_->RunSingleCommandAndWait(
//           command_buffer_type,
//           [this_buffer = this, &new_buffer_sub_resource_attribute,
//            &buffer_chunk_info, new_queue_family_index,
//            command_buffer_type](AllocatedCommandBuffer& cmd) mutable {
//             if (buffer_chunk_info.GetOffset() + buffer_chunk_info.GetSize() >
//                 this_buffer->buffer_data_info_.buffer_create_info_.size_) {
//               return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
//             }
//
//             new_buffer_sub_resource_attribute.reserve(
//                 this_buffer->buffer_data_info_.buffer_sub_resource_attributes_
//                     .size());
//
//             bool change_flag = false;
//             std::uint64_t old_queue_index;
//             for (std::uint64_t i = 0;
//                  i < this_buffer->buffer_data_info_
//                          .buffer_sub_resource_attributes_.size();
//                  ++i) {
//               std::vector<BufferSubResourceAttribute>&
//                   buffer_sub_resource_attributes =
//                       this_buffer->buffer_data_info_
//                           .buffer_sub_resource_attributes_;
//               std::vector<VkBufferMemoryBarrier2> vk_buffer_memory_barrier2s;
//
//               if (!change_flag) {
//                 if (buffer_chunk_info.GetOffset() >=
//                     buffer_sub_resource_attributes[i]
//                         .GetChunkInfo()
//                         .GetOffset()) {
//                   old_queue_index =
//                       buffer_sub_resource_attributes[i].GetQueueIndex();
//
//                   if (buffer_chunk_info.GetOffset() !=
//                       buffer_sub_resource_attributes[i]
//                           .GetChunkInfo()
//                           .GetOffset()) {
//                     new_buffer_sub_resource_attribute.emplace_back(
//                         buffer_sub_resource_attributes[i]
//                             .GetChunkInfo()
//                             .GetOffset(),
//                         buffer_chunk_info.GetOffset() -
//                             buffer_sub_resource_attributes[i]
//                                 .GetChunkInfo()
//                                 .GetOffset(),
//                         buffer_sub_resource_attributes[i].GetQueueIndex());
//                   }
//
//                   new_buffer_sub_resource_attribute.emplace_back(
//                       buffer_chunk_info.GetOffset(),
//                       buffer_chunk_info.GetSize(), new_queue_family_index);
//
//                   BufferChunkInfo change_chunk{buffer_chunk_info.GetOffset(),
//                                                0};
//                   std::uint32_t current_queue_index =
//                       buffer_sub_resource_attributes[i].GetQueueIndex();
//
//                   std::uint64_t end_offset = buffer_chunk_info.GetOffset() +
//                                              buffer_chunk_info.GetSize();
//
//                   for (; i != buffer_sub_resource_attributes.size(); ++i) {
//                     if (buffer_sub_resource_attributes[i].GetQueueIndex() !=
//                         old_queue_index) {
//                       LOG_ERROR(
//                           "The ownership of resources within the scope of a "
//                           "ownership conversion operation must be the
//                           same.");
//                       return
//                       ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
//                     }
//
//                     if (buffer_sub_resource_attributes[i]
//                                 .GetChunkInfo()
//                                 .GetOffset() +
//                             buffer_sub_resource_attributes[i]
//                                 .GetChunkInfo()
//                                 .GetSize() <=
//                         end_offset) {
//                       continue;
//                     }
//
//                     new_buffer_sub_resource_attribute.emplace_back(
//                         end_offset,
//                         buffer_sub_resource_attributes[i]
//                                 .GetChunkInfo()
//                                 .GetSize() -
//                             (end_offset - buffer_sub_resource_attributes[i]
//                                               .GetChunkInfo()
//                                               .GetOffset()),
//                         buffer_sub_resource_attributes[i].GetQueueIndex());
//
//                     break;
//                   }
//
//                   continue;
//                 }
//               }
//
//               new_buffer_sub_resource_attribute.emplace_back(
//                   buffer_sub_resource_attributes[i].GetChunkInfo().GetOffset(),
//                   buffer_sub_resource_attributes[i].GetChunkInfo().GetSize(),
//                   buffer_sub_resource_attributes[i].GetQueueIndex());
//             }
//
//             MM_CHECK(Utils::BeginCommandBuffer(cmd),
//                      LOG_ERROR("Failed to begin command buffer.");
//                      return MM_RESULT_CODE;)
//
//             VkBufferMemoryBarrier2 buffer_memory_barrier;
//             if (command_buffer_type == CommandBufferType::TRANSFORM) {
//               buffer_memory_barrier = Utils::GetVkBufferMemoryBarrier2(
//                   VK_PIPELINE_STAGE_2_TRANSFER_BIT,
//                   VK_ACCESS_2_TRANSFER_READ_BIT,
//                   VK_PIPELINE_STAGE_2_TRANSFER_BIT,
//                   VK_ACCESS_2_TRANSFER_READ_BIT, old_queue_index,
//                   new_queue_family_index, *this_buffer,
//                   buffer_chunk_info.GetOffset(),
//                   buffer_chunk_info.GetSize());
//             } else if (command_buffer_type == CommandBufferType::GRAPH) {
//               buffer_memory_barrier = Utils::GetVkBufferMemoryBarrier2(
//                   VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, 0,
//                   VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, 0, old_queue_index,
//                   new_queue_family_index, *this_buffer,
//                   buffer_chunk_info.GetOffset(),
//                   buffer_chunk_info.GetSize());
//             } else if (command_buffer_type == CommandBufferType::COMPUTE) {
//               buffer_memory_barrier = Utils::GetVkBufferMemoryBarrier2(
//                   VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, 0,
//                   VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, 0, old_queue_index,
//                   new_queue_family_index, *this_buffer,
//                   buffer_chunk_info.GetOffset(),
//                   buffer_chunk_info.GetSize());
//             } else {
//               return ExecuteResult ::UNDEFINED_ERROR;
//             }
//
//             VkDependencyInfo dependency_info = Utils::GetVkDependencyInfo(
//                 0, nullptr, 0, &buffer_memory_barrier, 0, nullptr, 0);
//
//             vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
//
//             MM_CHECK(Utils::EndCommandBuffer(cmd),
//                      LOG_ERROR("Failed to end command buffer.");
//                      return MM_RESULT_CODE;)
//
//             return ExecuteResult::SUCCESS;
//           },
//           std::vector<RenderResourceDataID>{GetRenderResourceDataID()}),
//       return MM_RESULT_CODE;)
//
//   buffer_data_info_.buffer_sub_resource_attributes_ =
//       std::move(new_buffer_sub_resource_attribute);
//
//   return ExecuteResult::SUCCESS;
// }
