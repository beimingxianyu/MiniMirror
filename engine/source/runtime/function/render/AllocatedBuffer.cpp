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

VmaAllocator MM::RenderSystem::AllocatedBufferWrapper::GetAllocator() const {
  return allocator_;
}

VkBuffer MM::RenderSystem::AllocatedBufferWrapper::GetBuffer() {
  return buffer_;
}

VmaAllocation MM::RenderSystem::AllocatedBufferWrapper::GetAllocation() {
  return allocation_;
}

void MM::RenderSystem::AllocatedBufferWrapper::Release() {
  if (IsValid()) {
    vmaDestroyBuffer(allocator_, buffer_, allocation_);

    allocator_ = nullptr;
    buffer_ = nullptr;
    allocation_ = nullptr;
  }
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
    AllocatedBufferWrapper&& other) noexcept
    : allocator_(other.allocator_),
      buffer_(other.buffer_),
      allocation_(other.allocation_) {
  allocator_ = nullptr;
  buffer_ = nullptr;
  allocation_ = nullptr;
}

MM::RenderSystem::AllocatedBufferWrapper&
MM::RenderSystem::AllocatedBufferWrapper::operator=(
    AllocatedBufferWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  Release();

  allocator_ = other.allocator_;
  buffer_ = other.buffer_;
  allocation_ = other.allocation_;

  other.allocator_ = nullptr;
  other.buffer_ = nullptr;
  other.allocation_ = nullptr;

  return *this;
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

MM::Result<MM::Nil> MM::RenderSystem::AllocatedBuffer::CheckInitParameters(
    RenderEngine* render_engine,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }


  if (Result<Nil> check_result = CheckVkBufferCreateInfo(vk_buffer_create_info);
    check_result.IsError()) {
    return check_result;
  }


  if (Result<Nil> check_result = CheckVmaAllocationCreateInfo(vma_allocation_create_info);
      check_result.IsError()) {
    return check_result;
  }

  if (*vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetGraphQueueIndex() &&
      *vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetTransformQueueIndex() &&
      *vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetPresentQueueIndex() &&
      *vk_buffer_create_info->pQueueFamilyIndices !=
          render_engine->GetComputeQueueIndex()) {
    MM_LOG_ERROR("The queue family index is error.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }

  return ResultS<Nil>{};
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    const std::string& name, RenderEngine* render_engine,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info)
    : RenderResourceDataBase(name, RenderResourceDataID()),
      render_engine_(render_engine),
      buffer_data_info_(),
      wrapper_() {
#ifdef MM_CHECK_PARAMETERS
  if (CheckInitParameters(render_engine, vk_buffer_create_info,
                          vma_allocation_create_info)
          .Exception(MM_DEBUG_DESCRIPTION("MM::RenderSystem::AllocatedBuffer "
                                          "construct parameters are error."))
          .IsError()) {
    return;
  }
#endif

  buffer_data_info_.SetBufferCreateInfo(*vk_buffer_create_info);
  buffer_data_info_.SetAllocationCreateInfo(*vma_allocation_create_info);
  buffer_data_info_.buffer_sub_resource_attributes_.emplace_back(
      0, buffer_data_info_.buffer_create_info_.size_,
      buffer_data_info_.buffer_create_info_.queue_family_indices_[0]);

  Result<RenderResourceDataAttributeID> render_resource_data_attribute_ID_result = buffer_data_info_.GetRenderResourceDataAttributeID().Exception("Failed to get RenderResourceDataAttributeID.");
  if (render_resource_data_attribute_ID_result.Exception(MM_ERROR_DESCRIPTION(Failed to get RenderResourceDataAttributeID.)).IsError()) {
    RenderResourceDataBase::Release();
    render_engine_ = nullptr;
    buffer_data_info_.Reset();
    return;
  }
  RenderResourceDataAttributeID& render_resource_data_attribute_ID = render_resource_data_attribute_ID_result.GetResult();

  SetRenderResourceDataID(RenderResourceDataID{
      GetObjectID().GetHash(), render_resource_data_attribute_ID});

  if (InitBuffer(render_engine, *vk_buffer_create_info,
                 *vma_allocation_create_info)
          .Exception([functon_name = MM_FUNCTION_NAME,
                      this_object = this](ErrorResult error_result) {
            MM_LOG_DESCRIPTION_CHECK_RESULT2(error_result.GetErrorCode(),
                                             functon_name, "Failed to initialization buffer.", ERROR);
            this_object->RenderResourceDataBase::Release();
            this_object->render_engine_ = nullptr;
            this_object->buffer_data_info_.Reset();
          })
          .IsError()) {
    return;
  }
}

MM::Result<MM::Nil> MM::RenderSystem::AllocatedBuffer::InitBuffer(
    RenderEngine* render_engine,
    const VkBufferCreateInfo& vk_buffer_create_info,
    const VmaAllocationCreateInfo& vma_allocation_create_info) {
  VkBuffer temp_buffer{nullptr};
  VmaAllocation temp_allocation{nullptr};

  if (ConvertVkResultToMMResult(
          vmaCreateBuffer(render_engine->GetAllocator(), &vk_buffer_create_info,
                          &vma_allocation_create_info, &temp_buffer,
                          &temp_allocation, nullptr))
          .Exception(MM_ERROR_DESCRIPTION(Failed to create VkBuffer.))
          .IsError()) {
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }

  wrapper_.SetAllocator(render_engine->GetAllocator());
  wrapper_.SetBuffer(temp_buffer);
  wrapper_.SetAllocation(temp_allocation);

  return ResultS<Nil>{};
}

VmaAllocator MM::RenderSystem::AllocatedBuffer::GetAllocator() const {
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

const MM::RenderSystem::BufferDataInfo&
MM::RenderSystem::AllocatedBuffer::GetBufferInfo() const {
  return buffer_data_info_;
}

bool MM::RenderSystem::AllocatedBuffer::IsTransformDest() const {
  return IsTransformDestBuffer(
      buffer_data_info_.buffer_create_info_.usage_);
}

bool MM::RenderSystem::AllocatedBuffer::IsTransformSrc() const {
  return IsTransformSrcBuffer(
      buffer_data_info_.buffer_create_info_.usage_);
}

bool MM::RenderSystem::AllocatedBuffer::CanMapped() const {
  return CanBeMapped(buffer_data_info_.allocation_create_info_.usage_,
                            buffer_data_info_.allocation_create_info_.flags_);
}

const VkDeviceSize& MM::RenderSystem::AllocatedBuffer::GetBufferSize() const {
  return buffer_data_info_.buffer_create_info_.size_;
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    const std::string& name,
    const RenderResourceDataID& render_resource_data_ID,
    RenderEngine* render_engine,
    const BufferDataInfo& buffer_data_info,
    VmaAllocator vma_allocator, VkBuffer vk_buffer, VmaAllocation vk_allocation)
    : RenderResourceDataBase(name, render_resource_data_ID),
      render_engine_(render_engine),
      buffer_data_info_(buffer_data_info),
      wrapper_(vma_allocator, vk_buffer, vk_allocation) {}

MM::Result<MM::Nil> MM::RenderSystem::AllocatedBuffer::CopyDataToBuffer(
    std::uint64_t dest_offset, void* data, std::uint64_t src_offset,
    std::uint64_t size) {
  if (!IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  if (data == nullptr) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  // The copied data cannot exceed the buffer range.
  if (size > buffer_data_info_.buffer_create_info_.size_ - dest_offset) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  if (CanMapped()) {
    char* buffer_ptr{nullptr};
    if (auto result = ConvertVkResultToMMResult(
            vmaMapMemory(GetAllocator(), GetAllocation(),
                         reinterpret_cast<void**>(&buffer_ptr)));
        result
            .Exception(MM_ERROR_DESCRIPTION2(
                "Unable to obtain a pointer mapped to a buffer"))
            .IsError()) {
      return ResultE<>{result.GetError().GetErrorCode()};
    }

    const char* data_ptr = static_cast<char*>(data) + src_offset;

    buffer_ptr = buffer_ptr + dest_offset;
    memcpy(buffer_ptr, data_ptr, size);

    vmaUnmapMemory(GetAllocator(), GetAllocation());

    if (IsAssetResource()) {
      MarkThisUseForWrite();
    }

    return ResultS<Nil>{};
  }

  if (!IsTransformDest()) {
    MM_LOG_ERROR(
        "If you want to copy data into an unmapped AllocatedBuffer, it must be "
        "an AllocatedBuffer that can be specified as the transform "
        "destination.");
    return ResultE<>{ErrorCode::OPERATION_NOT_SUPPORTED};
  }

  Result<AllocatedBuffer> stage_buffer_result = GetRenderEnginePtr()->CreateStageBuffer(
               size, render_engine_->GetTransformQueueIndex());
  if (stage_buffer_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create stage buffer.")).IsError()) {
    return ResultE<>{ErrorCode::CREATE_OBJECT_FAILED};
  }
  AllocatedBuffer& stage_buffer = stage_buffer_result.GetResult();

  const auto buffer_copy_region = GetVkBufferCopy2(size, 0, dest_offset);
  auto buffer_copy_info = GetVkCopyBufferInfo2(stage_buffer, *this, 1, &buffer_copy_region);

  void* stage_buffer_ptr{nullptr};

  if (auto result = ConvertVkResultToMMResult(vmaMapMemory(render_engine_->GetAllocator(),
                           stage_buffer.GetAllocation(), &stage_buffer_ptr)); result.Exception(MM_ERROR_DESCRIPTION2("Unable to obtain a pointer mapped to a buffer")).IsError()) {
    return ResultE<>{result.GetError().GetErrorCode()};
  }

  memcpy(stage_buffer_ptr, static_cast<char*>(data) + src_offset, size);

  vmaUnmapMemory(render_engine_->GetAllocator(), stage_buffer.GetAllocation());

  if (auto result = render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, false,
          std::vector<RenderResourceDataID>{
              stage_buffer.GetRenderResourceDataID()},
          [&buffer_copy_info = buffer_copy_info, render_engine = render_engine_,
           this_buffer = this](AllocatedCommandBuffer& cmd) -> Result<Nil> {
            if (buffer_copy_info.pRegions->size >
                this_buffer->GetSize() - buffer_copy_info.pRegions->dstOffset) {
              return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
            }

            if (auto if_result = BeginCommandBuffer(cmd);
                if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer.")).IsError()) {
              return ResultE<>{if_result.GetError().GetErrorCode()};
            }

            std::vector<VkBufferMemoryBarrier2> barriers;
            std::uint64_t affected_sub_resource_index = 0;
            std::uint64_t affected_sub_resource_count = 0;
            const auto& sub_resource_attributes =
                this_buffer->GetSubResourceAttributes();
            for (std::uint64_t i = 0; i < sub_resource_attributes.size(); ++i) {
              if (sub_resource_attributes[i].GetChunkInfo().GetOffset() <
                  buffer_copy_info.pRegions->srcOffset) {
                affected_sub_resource_index = i;
                for (; i < sub_resource_attributes.size(); ++i) {
                  if (sub_resource_attributes[i].GetChunkInfo().GetOffset() <
                      buffer_copy_info.pRegions->srcOffset +
                          buffer_copy_info.pRegions->size) {
                    barriers.emplace_back(GetVkBufferMemoryBarrier2(
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

            VkDependencyInfo dependency_info = GetVkDependencyInfo(
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

            if (auto if_result = EndCommandBuffer(cmd);
                if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer.")).IsError()) {
              return ResultE<>{if_result.GetError().GetErrorCode()};
            }

            return ResultS<Nil>{};
          }); result.Exception(MM_ERROR_DESCRIPTION2("Failed to copy data to allocated buffer.")).IsError()) {
    return ResultE<>{result.GetError().GetErrorCode()};
  }

  if (IsAssetResource()) {
    MarkThisUseForWrite();
  }

  return ResultS<Nil>();
}

MM::RenderSystem::AllocatedBuffer::AllocatedBuffer(
    AllocatedBuffer&& other) noexcept
    : RenderResourceDataBase(std::move(other)),
      render_engine_(other.render_engine_),
      buffer_data_info_(std::move(other.buffer_data_info_)),
      wrapper_(std::move(other.wrapper_)) {
  other.render_engine_ = nullptr;
}

MM::RenderSystem::AllocatedBuffer& MM::RenderSystem::AllocatedBuffer::operator=(
    AllocatedBuffer&& other) noexcept {
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

MM::Result<MM::Nil> MM::RenderSystem::AllocatedBuffer::CopyAssetDataToBuffer(
    AssetSystem::AssetManager::HandlerType asset_handler,
    std::uint32_t queue_index) {
  if (!IsValid() && !asset_handler.IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  if (IsAssetResource()) {
    MM_LOG_ERROR(
        "It is not supported to rewrite asset data to an AllocatedBuffer that "
        "has already written asset data.");
    return ResultE<>{ErrorCode::OPERATION_NOT_SUPPORTED};
  }

  auto datas = asset_handler.GetAsset().GetDatas();
  std::uint64_t asset_datas_size = 0;
  for (const auto& data : datas) {
    asset_datas_size += data.second;
  }
  if (asset_datas_size > GetBufferSize()) {
    MM_LOG_ERROR("Asset size lager than buffer size.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  if (buffer_data_info_.buffer_sub_resource_attributes_.size() == 1 &&
      buffer_data_info_.buffer_sub_resource_attributes_[0].GetQueueIndex() ==
          queue_index &&
      CanMapped()) {
    char* buffer_ptr{nullptr};
    if (auto if_result = ConvertVkResultToMMResult(
            vmaMapMemory(GetAllocator(), GetAllocation(),
                         reinterpret_cast<void**>(&buffer_ptr)));
        if_result
            .Exception(MM_ERROR_DESCRIPTION2(
                "Unable to obtain a pointer mapped to a buffer"))
            .IsError()) {
      return ResultE<>{if_result.GetError().GetErrorCode()};
    }

    std::uint64_t src_offset = 0;
    for (const auto& data : datas) {
      const char* data_ptr = static_cast<char*>(data.first);

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
    return ResultE<>{ErrorCode::OPERATION_NOT_SUPPORTED};
  }

  CommandBufferType command_buffer_type =
      ChooseCommandBufferType(render_engine_, queue_index);
  if (command_buffer_type == CommandBufferType::UNDEFINED) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  Result<AllocatedBuffer> stage_buffer_result = GetRenderEnginePtr()->CreateStageBuffer(asset_datas_size, queue_index);
  if (stage_buffer_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create stage buffer.")).IsError()) {
    return ResultE<>{ErrorCode::CREATE_OBJECT_FAILED};
  }
  AllocatedBuffer& stage_buffer = stage_buffer_result.GetResult();

  std::vector<BufferSubResourceAttribute> old_sub_resource_attribute;
  if (auto if_result =
      render_engine_->RunSingleCommandAndWait(
          command_buffer_type, false,
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()},
          [this_buffer = this, &datas, queue_index, &asset_datas_size,
           &stage_buffer,
           &old_sub_resource_attribute](AllocatedCommandBuffer& cmd) mutable -> Result<Nil> {
            std::vector<VkBufferMemoryBarrier2> barrier;
            for (const auto& sub_resource_attribute :
                 this_buffer->GetSubResourceAttributes()) {
              barrier.emplace_back(GetVkBufferMemoryBarrier2(
                  VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                  VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                  VK_ACCESS_2_TRANSFER_WRITE_BIT,
                  sub_resource_attribute.GetQueueIndex(), queue_index,
                  this_buffer->GetBuffer(),
                  sub_resource_attribute.GetChunkInfo().GetOffset(),
                  sub_resource_attribute.GetChunkInfo().GetSize()));
            }

            VkDependencyInfo dependency_info{
                GetVkDependencyInfo(nullptr, &barrier, nullptr, 0)};

            void* stage_buffer_ptr{nullptr};
            VkDeviceSize src_offset{0};
            if (auto if_result2 =
                ConvertVkResultToMMResult(vmaMapMemory(this_buffer->GetAllocator(),
                             stage_buffer.GetAllocation(), &stage_buffer_ptr));
                             if_result2.Exception(MM_ERROR_DESCRIPTION2("Unable to obtain a pointer mapped to a buffer")).IsError()) {
              return ResultE<>{if_result2.GetError().GetErrorCode()};
            }
            for (const auto& asset_data : datas) {
              memcpy(static_cast<char*>(stage_buffer_ptr) + src_offset,
                     asset_data.first, asset_data.second);
            }

            vmaUnmapMemory(this_buffer->GetAllocator(),
                           stage_buffer.GetAllocation());

            if (auto if_result2 = BeginCommandBuffer(cmd);
                if_result2.Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer.")).IsError()) {
              return ResultE<>{if_result2.GetError().GetErrorCode()};
            }

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            old_sub_resource_attribute = std::move(
                this_buffer->buffer_data_info_.buffer_sub_resource_attributes_);
            this_buffer->buffer_data_info_.buffer_sub_resource_attributes_ =
                std::vector<BufferSubResourceAttribute>{
                    BufferSubResourceAttribute{0, this_buffer->GetBufferSize(),
                                               queue_index}};

            auto buffer_copy_region =
                GetVkBufferCopy2(asset_datas_size, 0, 0);
            auto buffer_copy_info = GetVkCopyBufferInfo2(
                nullptr, stage_buffer.GetBuffer(), this_buffer->GetBuffer(), 1,
                &buffer_copy_region);

            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &buffer_copy_info);

            if (auto if_result2 = EndCommandBuffer(cmd);
                if_result2.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer.")).IsError()) {
              return ResultE<>{if_result2.GetError().GetErrorCode()};
            }

            return ResultS<Nil>{};
          });
          if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to copy asset data to AllocatedBuffer.")).IsError()) {
      if (!old_sub_resource_attribute.empty()) {
        buffer_data_info_.buffer_sub_resource_attributes_ =
            std::move(old_sub_resource_attribute);
      }
     return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  MarkThisIsAssetResource();

  return ResultS<Nil>{};
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
    const std::string& name, RenderEngine* render_engine,
    AssetSystem::AssetManager::HandlerType asset_handler,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info)
    : RenderResourceDataBase(name, RenderResourceDataID()),
      render_engine_(render_engine),
      buffer_data_info_(),
      wrapper_() {
#ifdef MM_CHECK_PARAMETERS
  if (auto if_result = CheckInitParametersWhenInitFromAnAsset(
          render_engine, asset_handler, vk_buffer_create_info,
          vma_allocation_create_info);
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("MM::RenderSystem::AllocatedBuffer "
                                           "construct parameters are error."))
          .IsError()) {
    return;
  }
#endif

  buffer_data_info_.SetBufferCreateInfo(*vk_buffer_create_info);
  buffer_data_info_.SetAllocationCreateInfo(*vma_allocation_create_info);
  buffer_data_info_.buffer_sub_resource_attributes_.emplace_back(
      0, buffer_data_info_.buffer_create_info_.size_,
      buffer_data_info_.buffer_create_info_.queue_family_indices_[0]);

  Result<RenderResourceDataAttributeID> render_resource_data_attribute_ID_result = buffer_data_info_.GetRenderResourceDataAttributeID();
  if (render_resource_data_attribute_ID_result.Exception(MM_ERROR_DESCRIPTION2("Failed to get RenderResourceDataAttributeID.")).IsError()) {
    RenderResourceDataBase::Release();
    render_engine_ = nullptr;
    buffer_data_info_.Reset();
    return;
  }
  RenderResourceDataAttributeID& render_resource_data_attribute_ID = render_resource_data_attribute_ID_result.GetResult();
  SetRenderResourceDataID(RenderResourceDataID{
      GetObjectID().GetHash(), render_resource_data_attribute_ID});

  if (InitBuffer(render_engine, *vk_buffer_create_info,
                      *vma_allocation_create_info).Exception(MM_ERROR_DESCRIPTION2("Failed to initialization allocated buffer.")).IsError()) {
    RenderResourceDataBase::Release();
    render_engine_ = render_engine;
    buffer_data_info_.Reset();
    return;
  }

  if (CopyAssetDataToBuffer(asset_handler,
                            GetSubResourceAttributes()[0].GetQueueIndex())
          .Exception(MM_ERROR_DESCRIPTION2(
              "Failed to copy asset data to allocated buffer."))
          .IsError()) {
    RenderResourceDataBase::Release();
    render_engine_ = render_engine;
    buffer_data_info_.Reset();
    wrapper_.Release();
    return;
  }
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

MM::Result<MM::Nil>
MM::RenderSystem::AllocatedBuffer::CheckInitParametersWhenInitFromAnAsset(
    RenderEngine* render_engine,
    AssetSystem::AssetManager::HandlerType asset_handler,
    const VkBufferCreateInfo* vk_buffer_create_info,
    const VmaAllocationCreateInfo* vma_allocation_create_info) {
  if (auto if_result = CheckInitParameters(render_engine, vk_buffer_create_info,
                                           vma_allocation_create_info); if_result.Exception(MM_ERROR_DESCRIPTION2("Construct parameters are error.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  if (!asset_handler.IsValid()) {
    MM_LOG_ERROR("The asset handler is invalid.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }
  if (!CanBeMapped(vma_allocation_create_info->usage,
                          vma_allocation_create_info->flags) &&
      !(vk_buffer_create_info->usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
    MM_LOG_ERROR(
        "Load data from asset must can be mapped or specify transform "
        "destination.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }

  return ResultS<Nil>{};
}

MM::Result<std::vector<VkBufferMemoryBarrier2>>
MM::RenderSystem::AllocatedBuffer::GetVkBufferMemoryBarriber2(
    VkDeviceSize offset, VkDeviceSize size,
    VkPipelineStageFlags2 src_stage_mask, VkAccessFlags2 src_access_mask,
    VkPipelineStageFlags2 dst_stage_mask, VkAccessFlags2 dst_access_mask,
    QueueIndex new_index) const {
  if (size == 0 || offset + size >= GetSize()) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  VkBufferMemoryBarrier2 barrier = GetVkBufferMemoryBarrier2(
      src_stage_mask, src_access_mask, dst_stage_mask, dst_access_mask,
      UINT32_MAX, new_index, const_cast<VkBuffer>(GetBuffer()), UINT64_MAX,
      UINT64_MAX);
  std::vector<VkBufferMemoryBarrier2> barriers{};
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

  return ResultS{std::move(barriers)};
}

MM::Result<MM::RenderSystem::AllocatedBuffer> MM::RenderSystem::AllocatedBuffer::GetCopy(
    const std::string& new_name) const {
  if (IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  VkBuffer new_buffer{nullptr};
  VmaAllocation new_allocation{nullptr};

  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  VkBufferCreateInfo buffer_create_info{GetVkBufferCreateInfo(
          GetBufferCreateInfo().next_, GetBufferCreateInfo().flags_,
          GetBufferCreateInfo().size_, GetBufferCreateInfo().usage_,
          GetBufferCreateInfo().sharing_mode_, 1, &transform_queue_index)};
  VmaAllocationCreateInfo allocation_create_info =
      buffer_data_info_.allocation_create_info_.GetVmaAllocationCreateInfo();

  if (auto if_result = ConvertVkResultToMMResult(
          vmaCreateBuffer(
          wrapper_.GetAllocator(),
                          &buffer_create_info, &allocation_create_info,
                          &new_buffer, &new_allocation, nullptr));
      if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkBuffer."))
          .IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  if (auto if_result =
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, false,std::vector<RenderResourceDataID>{GetRenderResourceDataID()},
          [this_buffer = this, new_buffer](AllocatedCommandBuffer& cmd) {
            if (this_buffer->GetSubResourceAttributes().size() == 1) {
              return this_buffer->AddCopyBufferCommandsWhenOneSubResource(
                  cmd, new_buffer);
            } else {
              return this_buffer->AddCopyBufferCommandseWhenMultSubResource(
                  cmd, new_buffer);
            }
          }
          );if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to copy data to new buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS{AllocatedBuffer{
      new_name, GetRenderResourceDataID(), render_engine_, buffer_data_info_,
      (wrapper_.GetAllocator()), new_buffer,
      new_allocation}};
}

MM::Result<std::vector<MM::RenderSystem::AllocatedBuffer>> MM::RenderSystem::AllocatedBuffer::GetCopy(
    std::vector<std::string>& new_names) const {
  if ((new_names.empty())) {
    MM_LOG_ERROR("The size of input parameters are error.");
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  if (IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  VkBufferCreateInfo buffer_create_info{GetVkBufferCreateInfo(
          GetBufferCreateInfo().next_, GetBufferCreateInfo().flags_,
          GetBufferCreateInfo().size_, GetBufferCreateInfo().usage_,
          GetBufferCreateInfo().sharing_mode_, 1, &transform_queue_index)};
  VmaAllocationCreateInfo allocation_create_info =
      buffer_data_info_.allocation_create_info_.GetVmaAllocationCreateInfo();

  std::vector<VkBuffer> new_buffers(new_names.size());
  std::vector<VmaAllocation> new_allocations(new_names.size());
  for (std::uint64_t i = 0; i != new_names.size(); ++i) {
    if (auto if_result = ConvertVkResultToMMResult(
        vmaCreateBuffer(wrapper_.GetAllocator(),
                        &buffer_create_info, &allocation_create_info,
                        &new_buffers[i], &new_allocations[i], nullptr));
                        if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create VkBuffer.")).IsError()) {
        for (std::uint64_t j = 0; j != i; ++j) {
          vmaDestroyBuffer(wrapper_.GetAllocator(),
                           new_buffers[j], new_allocations[j]);
        };
      return ResultE<>{if_result.GetError().GetErrorCode()};
    }
  }

  if (auto if_result =
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, false,
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()},
          [this_buffer = this, &new_buffers](AllocatedCommandBuffer& cmd) {
            if (this_buffer->GetSubResourceAttributes().size() == 1) {
              return this_buffer->AddCopyBufferCommandsWhenOneSubResource(
                  cmd, new_buffers);
            } else {
              return this_buffer->AddCopyBufferCommandseWhenMultSubResource(
                  cmd, new_buffers);
            }
          }
          );if_result.Exception(MM_ERROR_DESCRIPTION2("Failed to copy data to new buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  std::vector<AllocatedBuffer> new_allocated_buffers{};
  new_allocated_buffers.reserve(new_names.size());
  for (std::uint64_t i = 0; i != new_names.size(); ++i) {
    new_allocated_buffers.emplace_back(new_names[i],
                        GetRenderResourceDataID(),
                        render_engine_,
                        buffer_data_info_,
                        wrapper_.GetAllocator(),
                        new_buffers[i],
                        new_allocations[i]);
  }

  return ResultS{std::move(new_allocated_buffers)};
}

MM::Result<MM::Nil>
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandsWhenOneSubResource(
    AllocatedCommandBuffer& cmd, VkBuffer new_buffer) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  const BufferSubResourceAttribute& sub_resource =
      GetSubResourceAttributes()[0];
  std::array<VkBufferMemoryBarrier2, 2> barriers{
      GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
          transform_queue_index, *this, sub_resource.GetOffset(),
          sub_resource.GetSize()),
      GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
          transform_queue_index, new_buffer,
          sub_resource.GetOffset(), sub_resource.GetSize())};
  VkDependencyInfo dependency_info{GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  VkBufferCopy2 buffer_copy{GetVkBufferCopy2(GetBufferSize(), 0, 0)};
  VkCopyBufferInfo2 copy_buffer_info{GetVkCopyBufferInfo2(
          nullptr, const_cast<VkBuffer>(GetBuffer()), new_buffer, 1,
          &buffer_copy)};

  if (auto if_result = BeginCommandBuffer(cmd);
    if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &copy_buffer_info);

  barriers[0].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  std::swap(barriers[0].srcQueueFamilyIndex, barriers[0].dstQueueFamilyIndex);
  barriers[1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  barriers[1].dstQueueFamilyIndex = barriers[0].dstQueueFamilyIndex;
  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  if (auto if_result = EndCommandBuffer(cmd);
      if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandseWhenMultSubResource(
    AllocatedCommandBuffer& cmd, VkBuffer new_buffer) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();

  std::vector<VkBufferMemoryBarrier2> barriers{};
  barriers.reserve(GetSubResourceAttributes().size() * 2);
  // old image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    barriers.emplace_back(GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
        transform_queue_index, const_cast<VkBuffer>(GetBuffer()),
        sub_resource.GetOffset(), sub_resource.GetSize()));
  }
  // new image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    barriers.emplace_back(GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
        transform_queue_index, new_buffer, sub_resource.GetOffset(),
        sub_resource.GetSize()));
  }
  VkDependencyInfo dependency_info{GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  if (auto if_result = BeginCommandBuffer(cmd);
      if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to begine command buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkBufferCopy2> buffer_copy{};
  buffer_copy.reserve(GetSubResourceAttributes().size());
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    buffer_copy.emplace_back(GetVkBufferCopy2(
        sub_resource.GetSize(), sub_resource.GetOffset(),
        sub_resource.GetOffset()));
  }
  VkCopyBufferInfo2 copy_buffer_info{
      GetVkCopyBufferInfo2(
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

  if (auto if_result = EndCommandBuffer(cmd);
    if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandsWhenOneSubResource(
    AllocatedCommandBuffer& cmd, std::vector<VkBuffer>& new_buffers) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();
  const BufferSubResourceAttribute& sub_resource =
      GetSubResourceAttributes()[0];
  std::vector<VkBufferMemoryBarrier2> barriers{};
  barriers.reserve(1 + new_buffers.size());
  barriers.emplace_back(GetVkBufferMemoryBarrier2(
      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
      transform_queue_index, *this, sub_resource.GetOffset(),
      sub_resource.GetSize()));
  for (VkBuffer new_buffer : new_buffers) {
    barriers.emplace_back(GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
        transform_queue_index, new_buffer, sub_resource.GetOffset(),
        sub_resource.GetSize()));
  }
  VkDependencyInfo dependency_info{GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  VkBufferCopy2 buffer_copy{GetVkBufferCopy2(GetBufferSize(), 0, 0)};

  if (auto if_result = BeginCommandBuffer(cmd);
    if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  for (VkBuffer new_buffer : new_buffers) {
    VkCopyBufferInfo2 copy_buffer_info{
        GetVkCopyBufferInfo2(
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

  if (auto if_result = EndCommandBuffer(cmd);
    if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::AllocatedBuffer::AddCopyBufferCommandseWhenMultSubResource(
    AllocatedCommandBuffer& cmd, std::vector<VkBuffer>& new_buffers) const {
  QueueIndex transform_queue_index = render_engine_->GetTransformQueueIndex();

  std::uint64_t new_buffer_number = new_buffers.size();
  std::vector<VkBufferMemoryBarrier2> barriers{};
  barriers.reserve(GetSubResourceAttributes().size() * 2);
  // old image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    barriers.emplace_back(GetVkBufferMemoryBarrier2(
        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_ACCESS_2_TRANSFER_READ_BIT, sub_resource.GetQueueIndex(),
        transform_queue_index, const_cast<VkBuffer>(GetBuffer()),
        sub_resource.GetOffset(), sub_resource.GetSize()));
  }
  // new image barrier
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    for (VkBuffer new_buffer : new_buffers) {
      barriers.emplace_back(GetVkBufferMemoryBarrier2(
          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
          VK_ACCESS_2_TRANSFER_WRITE_BIT, transform_queue_index,
          transform_queue_index, new_buffer, sub_resource.GetOffset(),
          sub_resource.GetSize()));
    }
  }

  VkDependencyInfo dependency_info{GetVkDependencyInfo(
      0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};

  if (auto if_result = BeginCommandBuffer(cmd);
    if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to begine command buffer.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

  std::vector<VkBufferCopy2> buffer_copy{};
  buffer_copy.reserve(GetSubResourceAttributes().size());
  for (const auto& sub_resource : GetSubResourceAttributes()) {
    buffer_copy.emplace_back(GetVkBufferCopy2(
        sub_resource.GetSize(), sub_resource.GetOffset(),
        sub_resource.GetOffset()));
  }

  for (VkBuffer new_buffer : new_buffers) {
    VkCopyBufferInfo2 copy_buffer_info{GetVkCopyBufferInfo2(
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

  if (auto if_result = EndCommandBuffer(cmd);
    if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::AllocatedBuffer::TransformSubResourceAttribute(
    const std::vector<BufferSubResourceAttribute>& new_sub_resource_attribute) {
  if (!IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }
  if (new_sub_resource_attribute ==
      buffer_data_info_.buffer_sub_resource_attributes_) {
    return ResultS<Nil>{};
  }

  if (auto if_result = CheckTransformInputParameter(new_sub_resource_attribute);
      if_result.Exception(MM_ERROR_DESCRIPTION2("New sub resource attribute is error.")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  if (auto if_result =
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, false,
          std::vector<RenderResourceDataID>{GetRenderResourceDataID()},
          [this_buffer = this,
           &new_sub_resource_attribute](AllocatedCommandBuffer& cmd) -> Result<Nil> {
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
                    GetVkBufferMemoryBarrier2(
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_READ_BIT,
                        old_sub_resource.GetQueueIndex(),
                        this_buffer->render_engine_->GetTransformQueueIndex(),
                        this_buffer->GetBuffer(), transform_offset,
                        transform_size));
                transform_to_new_barriers.emplace_back(
                    GetVkBufferMemoryBarrier2(
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
                      GetVkBufferMemoryBarrier2(
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_READ_BIT,
                          old_sub_resource.GetQueueIndex(),
                          this_buffer->render_engine_->GetTransformQueueIndex(),
                          this_buffer->GetBuffer(), transform_offset,
                          transform_size));
                  transform_to_new_barriers.emplace_back(
                      GetVkBufferMemoryBarrier2(
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

            VkDependencyInfo dependency_info1{GetVkDependencyInfo(
                    0, nullptr, current_to_transform_barriers.size(),
                    current_to_transform_barriers.data(), 0, nullptr, 0)},
                dependency_info2{GetVkDependencyInfo(
                    0, nullptr, transform_to_new_barriers.size(),
                    transform_to_new_barriers.data(), 0, nullptr, 0)};

            if (auto if_result2 = BeginCommandBuffer(cmd);
               if_result2.Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer.")).IsError()) {
              return ResultE<>{if_result2.GetError().GetErrorCode()};
            }

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info1);
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info2);

            if (auto if_result2 = EndCommandBuffer(cmd);
              if_result2.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer.")).IsError()) {
              return ResultE<>{if_result2.GetError().GetErrorCode()};
            }

            this_buffer->MarkThisUseForWrite();

            this_buffer->buffer_data_info_.buffer_sub_resource_attributes_ =
                std::move(new_sub_resource_attribute);

            return ResultS<Nil>{};
          }
          ); if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to transform sub resource attribute.")).IsError()) {
        return ResultE<>{if_result.GetError().GetErrorCode()};
          }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
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
      return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
    }

    // Check overlap
    if ((sub_resource.GetOffset() < start &&
         sub_resource.GetOffset() + sub_resource.GetSize() > start) ||
        (sub_resource.GetOffset() >= start &&
         sub_resource.GetOffset() < start + size)) {
      return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
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
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil> MM::RenderSystem::AllocatedBuffer::CopyDataToBuffer(
    const std::tuple<std::uint64_t, void*, std::uint64_t, std::uint64_t>*
        copy_info,
    std::uint32_t count) {
  if (!IsValid()) {
    return ResultE<>{ErrorCode::OBJECT_IS_INVALID};
  }

  if (copy_info == nullptr) {
    return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
  }

  // The copied data cannot exceed the buffer range.
  for (std::uint32_t index = 0; index != count; ++index) {
    if (std::get<3>(copy_info[index]) >
        buffer_data_info_.buffer_create_info_.size_ -
            std::get<0>(copy_info[index])) {
      return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
    }
  }

  if (CanMapped()) {
    char* buffer_ptr{nullptr};
    if (auto if_result =
        ConvertVkResultToMMResult(vmaMapMemory(GetAllocator(), GetAllocation(),
                     reinterpret_cast<void**>(&buffer_ptr)));
                     if_result.Exception(MM_ERROR_DESCRIPTION2("Unable to obtain a pointer mapped to a buffer")).IsError()) {
      return ResultE<>{ErrorCode::UNDEFINED_ERROR};
    }

    for (std::uint32_t index = 0; index != count; ++index) {
      char* data_ptr = reinterpret_cast<char*>(std::get<1>(copy_info[index])) +
                       std::get<2>(copy_info[index]);

      char* new_buffer_ptr = buffer_ptr + std::get<0>(copy_info[index]);
      memcpy(new_buffer_ptr, data_ptr, std::get<3>(copy_info[index]));
    }

    vmaUnmapMemory(GetAllocator(), GetAllocation());

    if (IsAssetResource()) {
      MarkThisUseForWrite();
    }

    return ResultS<Nil>{};
  }

  if (!IsTransformDest()) {
    MM_LOG_ERROR(
        "If you want to copy data into an unmapped AllocatedBuffer, it must be "
        "an AllocatedBuffer that can be specified as the transform "
        "destination.");
    return ResultE<>{ErrorCode::OPERATION_NOT_SUPPORTED};
  }

  VkDeviceSize total_size = 0;
  for (std::uint32_t index = 0; index != count; ++index) {
    total_size += std::get<3>(copy_info[index]);
  }

  Result<AllocatedBuffer> stage_buffer_result = GetRenderEnginePtr()->CreateStageBuffer(
          total_size, render_engine_->GetTransformQueueIndex());
  if (stage_buffer_result.Exception(MM_ERROR_DESCRIPTION2("Failed to create stage buffer.")).IsError()) {
    return ResultE<>{ErrorCode::CREATE_OBJECT_FAILED};
  }
  AllocatedBuffer& stage_buffer = stage_buffer_result.GetResult();

  void* stage_buffer_ptr{nullptr};

  if (auto if_result = ConvertVkResultToMMResult(vmaMapMemory(render_engine_->GetAllocator(),
                           stage_buffer.GetAllocation(), &stage_buffer_ptr));
                           if_result.Exception(MM_ERROR_DESCRIPTION2("Unable to obtain a pointer mapped to a buffer")).IsError()) {
    return ResultE<>{if_result.GetError().GetErrorCode()};
  }

  std::vector<VkBufferCopy2> buffer_copy_regions{};
  buffer_copy_regions.reserve(count);

  VkDeviceSize src_offset = 0;
  for (std::uint32_t index = 0; index != count; ++index) {
    buffer_copy_regions.emplace_back(
        GetVkBufferCopy2(std::get<3>(copy_info[index]), src_offset,
                                std::get<0>(copy_info[index])));
    memcpy(static_cast<char*>(stage_buffer_ptr) + src_offset,
           static_cast<char*>(std::get<1>(copy_info[index])) +
               std::get<2>(copy_info[index]),
           std::get<3>(copy_info[index]));
    src_offset += std::get<3>(copy_info[index]);
  }

  vmaUnmapMemory(render_engine_->GetAllocator(), stage_buffer.GetAllocation());

  auto buffer_copy_info =
      GetVkCopyBufferInfo2(stage_buffer, *this, buffer_copy_regions);

  Result<RenderFutureState> command_execute_result =
      render_engine_->RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, false,
          std::vector<RenderResourceDataID>{
              stage_buffer.GetRenderResourceDataID()},
          [&buffer_copy_info = buffer_copy_info, render_engine = render_engine_,
           this_buffer = this]  (AllocatedCommandBuffer& cmd) -> Result<Nil> {
            if (buffer_copy_info.pRegions->size >
                this_buffer->GetSize() - buffer_copy_info.pRegions->dstOffset) {
              return ResultE<>{ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE};
            }

            if (auto if_result = BeginCommandBuffer(cmd);
              if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to begin command buffer.")).IsError()) {
              return ResultE<>{if_result.GetError().GetErrorCode()};
            }

            std::vector<VkBufferMemoryBarrier2> barriers;
            std::uint64_t affected_sub_resource_index = 0;
            std::uint64_t affected_sub_resource_count = 0;
            const auto& sub_resource_attributes =
                this_buffer->GetSubResourceAttributes();
            for (std::uint64_t i = 0; i < sub_resource_attributes.size(); ++i) {
              if (sub_resource_attributes[i].GetChunkInfo().GetOffset() <
                  buffer_copy_info.pRegions->srcOffset) {
                affected_sub_resource_index = i;
                for (; i < sub_resource_attributes.size(); ++i) {
                  if (sub_resource_attributes[i].GetChunkInfo().GetOffset() <
                      buffer_copy_info.pRegions->srcOffset +
                          buffer_copy_info.pRegions->size) {
                    barriers.emplace_back(GetVkBufferMemoryBarrier2(
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

            VkDependencyInfo dependency_info = GetVkDependencyInfo(
                0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0);

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &buffer_copy_info);

            for (std::uint64_t in_count = 0; in_count != affected_sub_resource_count;
                 ++in_count) {
              barriers[in_count].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
              barriers[in_count].srcQueueFamilyIndex =
                  render_engine->GetTransformQueueIndex();
              barriers[in_count].dstQueueFamilyIndex =
                  sub_resource_attributes[affected_sub_resource_index + in_count]
                      .GetQueueIndex();
            }

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            if (auto if_result = EndCommandBuffer(cmd);
               if_result.Exception(MM_FATAL_DESCRIPTION2("Failed to end command buffer.")).GetError()) {
              return ResultE<>{if_result.GetError().GetErrorCode()};
            }

            return ResultS<Nil>{};
          }
          );
  if (command_execute_result.Exception(MM_ERROR_DESCRIPTION2("Failed to copy data to buffer.")).IsError()) {
    return ResultE<>{command_execute_result.GetError().GetErrorCode()};
  }

  if (IsAssetResource()) {
    MarkThisUseForWrite();
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil> MM::RenderSystem::AllocatedBuffer::CopyDataToBuffer(
    const std::vector<std::tuple<std::uint64_t, void*, std::uint64_t,
                                 std::uint64_t>>& copy_info) {
  return CopyDataToBuffer(copy_info.data(),
                          static_cast<std::uint32_t>(copy_info.size()));
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
