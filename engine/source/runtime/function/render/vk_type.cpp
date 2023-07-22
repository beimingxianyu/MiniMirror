#include "runtime/function/render/vk_type.h"

#include <utility>

#include "RenderResourceDataBase.h"
#include "runtime/function/render/vk_engine.h"

void MM::RenderSystem::BufferInfo::Reset() {
  buffer_size_ = 0;
  can_mapped_ = false;
  is_transform_src_ = false;
  is_transform_dest_ = false;
  is_exclusive_ = false;
  queue_index_.clear();
}

bool MM::RenderSystem::BufferInfo::IsValid() const { return buffer_size_ != 0; }

MM::RenderSystem::VertexInputState::VertexInputState()
    : vertex_bind_(),
      vertex_buffer_offset_(0),
      vertex_attributes_(),
      instance_binds_(),
      instance_buffer_offset_(),
      instance_attributes_() {}

MM::RenderSystem::VertexInputState::VertexInputState(
    const VkDeviceSize& vertex_buffer_offset)
    : vertex_bind_{0, sizeof(AssetType::Vertex), VK_VERTEX_INPUT_RATE_VERTEX},
      vertex_buffer_offset_(vertex_buffer_offset),
      vertex_attributes_(),
      instance_binds_(),
      instance_buffer_offset_(),
      instance_attributes_() {
  InitDefaultVertexInput();
}

MM::RenderSystem::VertexInputState::VertexInputState(
    const VkDeviceSize& vertex_buffer_offset,
    const std::vector<VkVertexInputBindingDescription>& instance_binds,
    const std::vector<VkDeviceSize>& instance_buffer_offset,
    const std::vector<VkVertexInputAttributeDescription>& instance_attributes)
    : vertex_bind_{0, sizeof(MM::AssetType::Vertex),
                   VK_VERTEX_INPUT_RATE_VERTEX},
      vertex_buffer_offset_(vertex_buffer_offset),
      vertex_attributes_{5},
      instance_binds_(instance_binds),
      instance_buffer_offset_(instance_buffer_offset),
      instance_attributes_(instance_attributes) {
  std::string error_message;
  if (!CheckLayoutIsCorrect(error_message)) {
    MM_LOG_ERROR(error_message);
    Reset();
    return;
  }
  InitDefaultVertexInput();
}

MM::RenderSystem::VertexInputState::VertexInputState(
    VertexInputState&& other) noexcept
    : vertex_bind_(other.vertex_bind_),
      vertex_buffer_offset_(other.vertex_buffer_offset_),
      vertex_attributes_(std::move(other.vertex_attributes_)),
      instance_binds_(std::move(other.instance_binds_)),
      instance_buffer_offset_(std::move(other.instance_buffer_offset_)),
      instance_attributes_(std::move(other.instance_attributes_)) {
  other.vertex_bind_.stride = 0;
  vertex_buffer_offset_ = 0;
}

MM::RenderSystem::VertexInputState&
MM::RenderSystem::VertexInputState::operator=(const VertexInputState& other) {
  if (&other == this) {
    return *this;
  }
  vertex_bind_ = other.vertex_bind_;
  vertex_buffer_offset_ = other.vertex_buffer_offset_;
  vertex_attributes_ = other.vertex_attributes_;
  instance_binds_ = other.instance_binds_;
  instance_buffer_offset_ = other.instance_buffer_offset_;
  instance_attributes_ = other.instance_attributes_;

  return *this;
}

MM::RenderSystem::VertexInputState&
MM::RenderSystem::VertexInputState::operator=(
    VertexInputState&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  vertex_bind_ = other.vertex_bind_;
  vertex_buffer_offset_ = other.vertex_buffer_offset_;
  vertex_attributes_ = std::move(other.vertex_attributes_);
  instance_binds_ = std::move(other.instance_binds_);
  instance_buffer_offset_ = std::move(other.instance_buffer_offset_);
  instance_attributes_ = std::move(other.instance_attributes_);

  vertex_bind_.stride = 0;
  vertex_buffer_offset_ = 0;

  return *this;
}

void MM::RenderSystem::VertexInputState::InitDefaultVertexInput() {
  vertex_attributes_[0].binding = 0;
  vertex_attributes_[0].location = 0;
  vertex_attributes_[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[0].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfPosition());

  vertex_attributes_[1].binding = 0;
  vertex_attributes_[1].location = 1;
  vertex_attributes_[1].format = VK_FORMAT_R32G32_SFLOAT;
  vertex_attributes_[1].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfTextureCoord());

  vertex_attributes_[2].binding = 0;
  vertex_attributes_[2].location = 2;
  vertex_attributes_[2].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[2].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfNormal());

  vertex_attributes_[3].binding = 0;
  vertex_attributes_[3].location = 3;
  vertex_attributes_[3].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[3].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfTangent());

  vertex_attributes_[4].binding = 0;
  vertex_attributes_[4].location = 4;
  vertex_attributes_[4].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attributes_[4].offset =
      static_cast<uint32_t>(MM::AssetType::Vertex::GetOffsetOfBiTangent());
}

bool MM::RenderSystem::VertexInputState::IsValid() const {
  return vertex_bind_.stride != 0;
}

void MM::RenderSystem::VertexInputState::Reset() {
  vertex_bind_.stride = 0;
  vertex_buffer_offset_ = 0;
  vertex_attributes_.clear();
  instance_binds_.clear();
  instance_buffer_offset_.clear();
  instance_attributes_.clear();
}

bool MM::RenderSystem::VertexInputState::CheckLayoutIsCorrect(
    std::string& error_message) const {
  if (instance_buffer_offset_.size() != instance_binds_.size()) {
    error_message =
        "The number of offset values is not equal to the number of instance "
        "bind.";
    return false;
  }
  std::map<uint32_t, VkDeviceSize> binds_info;
  for (const auto& bind : instance_binds_) {
    if (bind.binding == 0) {
      error_message =
          "The binding of instance input description is 0.0 is the exclusive "
          "binding slot of Vertex input description.";
      return false;
    }
    if (bind.stride == 0) {
      error_message = "The size of one instance data cannot be 0.";
      return false;
    }
    if (bind.inputRate != VK_VERTEX_INPUT_RATE_INSTANCE) {
      error_message =
          "The instance input description input rate is not "
          "VK_VERTEX_INPUT_RATE_INSTANCE";
      return false;
    }
    if (binds_info.find(bind.binding) != binds_info.end()) {
      error_message =
          "Multiple instance input description are repeatedly bound to a same "
          "binding.";
      return false;
    }
    binds_info.emplace(bind.binding, bind.stride);
  }

  for (const auto& attribute : instance_attributes_) {
    if (attribute.location < 5) {
      error_message =
          "The instance attribute location less than 5.0 to 4 is the exclusive "
          "location slot of Vertex input description.";
      return false;
    }
    const auto bind_itr = binds_info.find(attribute.binding);
    if (bind_itr == binds_info.end()) {
      error_message =
          "The instance attribute description is bound to a nonexistent "
          "binding.";
      return false;
    }
    if (attribute.offset > bind_itr->second - 4) {
      error_message = "The instance attribute description offset too larger.";
      return false;
    }
  }

  return true;
}

bool MM::RenderSystem::VertexInputState::CheckLayoutIsCorrect() const {
  std::string temp;
  return CheckLayoutIsCorrect(temp);
}

const VkVertexInputBindingDescription&
MM::RenderSystem::VertexInputState::GetVertexBind() const {
  return vertex_bind_;
}

const VkDeviceSize& MM::RenderSystem::VertexInputState::GetVertexBufferOffset()
    const {
  return vertex_buffer_offset_;
}

const std::vector<VkVertexInputAttributeDescription>&
MM::RenderSystem::VertexInputState::GetVertexAttributes() const {
  return vertex_attributes_;
}

const std::vector<VkVertexInputBindingDescription>&
MM::RenderSystem::VertexInputState::GetInstanceBinds() const {
  return instance_binds_;
}

const std::vector<VkDeviceSize>&
MM::RenderSystem::VertexInputState::GetInstanceBufferOffset() const {
  return instance_buffer_offset_;
}

const std::vector<VkVertexInputAttributeDescription>&
MM::RenderSystem::VertexInputState::GetInstanceAttributes() const {
  return instance_attributes_;
}

uint32_t MM::RenderSystem::AllocatedBuffer::UseCount() const {
  if (wrapper_) {
    return wrapper_.use_count();
  }
  return 0;
}

MM::RenderSystem::VertexAndIndexBuffer::VertexAndIndexBuffer(
    RenderEngine* engine)
    : render_engine_(engine) {
  if (engine == nullptr) {
    return;
  }

  const auto vertex_buffer_size =
      std::stoull(MM_CONFIG_SYSTEM->GetConfig("init_vertex_buffer_size"));

  const auto index_buffer_size =
      std::stoull(MM_CONFIG_SYSTEM->GetConfig("init_index_buffer_size"));

  vertex_buffer_ = engine->CreateBuffer(vertex_buffer_size,
                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
  if (!vertex_buffer_.IsValid()) {
    Release();
    MM_LOG_ERROR("Failed to create vertex total buffer.");
  }

  index_buffer_ = engine->CreateBuffer(index_buffer_size,
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
  if (!index_buffer_.IsValid()) {
    Release();
    MM_LOG_ERROR("Failed to create index total buffer.");
  }
}

bool MM::RenderSystem::VertexAndIndexBuffer::IsValid() const {
  return vertex_buffer_.GetBufferSize() != 0 &&
         index_buffer_.GetBufferSize() != 0 && render_engine_ != nullptr;
}

const MM::RenderSystem::AllocatedBuffer&
MM::RenderSystem::VertexAndIndexBuffer ::GetVertexBuffer() const {
  return vertex_buffer_;
}

const MM::RenderSystem::AllocatedBuffer&
MM::RenderSystem::VertexAndIndexBuffer ::GetIndexBuffer() const {
  return index_buffer_;
}

const MM::RenderSystem::BufferInfo&
MM::RenderSystem::VertexAndIndexBuffer::GetVertexBufferInfo() const {
  return vertex_buffer_.GetBufferInfo();
}

const MM::RenderSystem::BufferInfo&
MM::RenderSystem::VertexAndIndexBuffer::GetIndexBufferInfo() const {
  return index_buffer_.GetBufferInfo();
}

// MM::ExecuteResult MM::RenderSystem::VertexAndIndexBuffer::AllocateBuffer(
//     const std::vector<AssetType::Vertex>& vertices,
//     const std::vector<uint32_t>& indexes,
//     const std::shared_ptr<BufferChunkInfo>& output_vertex_buffer_chunk_info,
//     const std::shared_ptr<MM::RenderSystem::BufferChunkInfo>&
//     output_index_buffer_chunk_info) {
//   if (vertices.empty() || indexes.empty()) {
//     LOG_ERROR("Input Vertices and indexes must not be empty.");
//     return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
//   }
//
//   const VkDeviceSize vertices_size =
//       sizeof(AssetType::Vertex) * vertices.size();
//   VkDeviceSize vertex_offset = 0;
//   const VkDeviceSize indexes_size = sizeof(uint32_t) * indexes.size();
//   VkDeviceSize index_offset = 0;
//   TaskSystem::Taskflow taskflow;
//
//   VkDeviceSize vertex_end_size = 0;
//   GetEndSizeAndOffset(vertex_buffer_, vertex_buffer_chunks_info_,
//                       vertex_end_size, vertex_offset);
//
//   if (vertices_size < vertex_end_size) {
//     *output_vertex_buffer_chunk_info =
//         BufferChunkInfo{vertex_offset, vertices_size};
//   } else {
//     // If the remaining space behind cannot meet the requirements,
//     // scan the entire buffer area to find suitable unused space.
//     if (!ScanBufferToFindSuitableArea(vertex_buffer_,
//                                       vertex_buffer_chunks_info_,
//                                       vertices_size, vertex_offset)) {
//       return false;
//     }
//     *output_vertex_buffer_chunk_info =
//         BufferChunkInfo{vertex_offset, vertices_size};
//   }
//
//   AllocatedBuffer vertex_stage_buffer = render_engine_->CreateBuffer(
//       vertices_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//       VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
//       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
//
//   MM_CHECK(render_engine_->CopyDataToBuffer(vertex_stage_buffer,
//                                             vertices.data(), 0,
//                                             vertices_size),
//            LOG_ERROR("Failed to copy new index data to stage buffer");
//            return MM_RESULT_CODE;)
//
//   const VkBufferCopy2 vertex_region =
//       Utils::GetBufferCopy(vertices_size, 0, vertex_offset);
//   MM_CHECK(
//       render_engine_->CopyBuffer(vertex_stage_buffer, vertex_buffer_,
//                                  std::vector<VkBufferCopy2>{vertex_region}),
//       LOG_ERROR("Failed to copy new vertex data to vertex buffer.");
//       return MM_RESULT_CODE;
//     )
//
//   MM_CHECK(
//       render_engine_->CopyBuffer(vertex_stage_buffer, vertex_buffer_,
//                                  std::vector<VkBufferCopy2>{vertex_region}),
//     )
//
//   vertex_stage_buffer.Release();
//
//   VkDeviceSize index_end_size = 0;
//   GetEndSizeAndOffset(index_buffer_, index_buffer_chunks_info_,
//   index_end_size,
//                       index_offset);
//
//   if (indexes_size < index_end_size) {
//     *output_index_buffer_chunk_info =
//         BufferChunkInfo{index_offset, indexes_size};
//   } else {
//     // If the remaining space behind cannot meet the requirements,
//     // scan the entire buffer area to find suitable unused space.
//     if (!ScanBufferToFindSuitableArea(index_buffer_,
//     index_buffer_chunks_info_,
//                                       indexes_size, index_offset)) {
//       return false;
//     }
//     *output_index_buffer_chunk_info =
//         BufferChunkInfo{index_offset, indexes_size};
//   }
//
//   AllocatedBuffer index_stage_buffer = render_engine_->CreateBuffer(
//       indexes_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//       VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
//       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,);
//
//   std::vector<VkDeviceSize> new_indexes(indexes.size());
//   for (std::size_t i = 0; i < new_indexes.size(); ++i) {
//     new_indexes[i] = indexes[i] + vertex_offset;
//   }
//
//   MM_CHECK(render_engine_->CopyDataToBuffer(
//                index_stage_buffer, new_indexes.data(), 0, indexes_size),
//            LOG_ERROR("Failed to copy new index data to stage buffer");
//            return MM_RESULT_CODE;)
//
//   const VkBufferCopy2 index_region =
//       Utils::GetBufferCopy(indexes_size, 0, index_offset);
//
//   MM_CHECK(render_engine_->CopyBuffer(index_stage_buffer, index_buffer_,
//                                       std::vector<VkBufferCopy2>{index_region}),
//            LOG_ERROR("Failed to copy new index data to index buffer.");
//            return MM_RESULT_CODE;
//     )
//
//   index_stage_buffer.Release();
//
//
//
//   return ExecuteResult::SUCCESS;
// }

void MM::RenderSystem::VertexAndIndexBuffer::Release() {
  render_engine_ = nullptr;
  vertex_buffer_.Release();
  index_buffer_.Release();
  vertex_buffer_chunks_info_.clear();
  index_buffer_chunks_info_.clear();
}

bool MM::RenderSystem::VertexAndIndexBuffer::ChooseVertexBufferReserveSize(
    const VkDeviceSize& require_size, VkDeviceSize& output_reserve_size) {
  VkDeviceSize max_buffer_size = 0;
  MM_CHECK(
      MM_CONFIG_SYSTEM->GetConfig("max_vertex_buffer_size", max_buffer_size),
      MM_LOG_WARN("The max_vertex_buffer_size"
                  " is not set,vertexbuffer_size will become"
                  "infinitely larger.");
      max_buffer_size = VK_WHOLE_SIZE;)

  VkDeviceSize total_used_size = 0;
  for (auto buffer_chunk_info = vertex_buffer_chunks_info_.begin();
       buffer_chunk_info != vertex_buffer_chunks_info_.end();
       ++buffer_chunk_info) {
    while ((*buffer_chunk_info).use_count() == 1) {
      buffer_chunk_info = vertex_buffer_chunks_info_.erase(buffer_chunk_info);
      if (buffer_chunk_info == vertex_buffer_chunks_info_.end()) {
        break;
      }
    }
    if (buffer_chunk_info == vertex_buffer_chunks_info_.end()) {
      break;
    }
    total_used_size += (*buffer_chunk_info)->GetSize();
  }
  if (require_size + total_used_size > max_buffer_size) {
    MM_LOG_ERROR("Insufficient buffer space to load data.");
    return false;
  }

  VkDeviceSize reserve_size = vertex_buffer_.GetBufferSize();
  while (reserve_size *= 2 < require_size + total_used_size) {
  }
  if (reserve_size > max_buffer_size) {
    reserve_size = max_buffer_size;
  }

  output_reserve_size = reserve_size;

  return true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::ChooseIndexBufferReserveSize(
    const VkDeviceSize& require_size, VkDeviceSize& output_reserve_size) {
  VkDeviceSize max_buffer_size = 0;
  MM_CHECK(
      MM_CONFIG_SYSTEM->GetConfig("max_index_buffer_size", max_buffer_size),
      MM_LOG_WARN("The max_index_buffer_size"
                  " is not set,vertexbuffer_size will become"
                  "infinitely larger.");
      max_buffer_size = VK_WHOLE_SIZE;)

  VkDeviceSize total_used_size = 0;
  for (auto buffer_chunk_info = index_buffer_chunks_info_.begin();
       buffer_chunk_info != index_buffer_chunks_info_.end();
       ++buffer_chunk_info) {
    while ((*buffer_chunk_info).use_count() == 1) {
      buffer_chunk_info = index_buffer_chunks_info_.erase(buffer_chunk_info);
      if (buffer_chunk_info == index_buffer_chunks_info_.end()) {
        break;
      }
    }
    if (buffer_chunk_info == index_buffer_chunks_info_.end()) {
      break;
    }
    total_used_size += (*buffer_chunk_info)->GetSize();
  }
  if (require_size + total_used_size > max_buffer_size) {
    MM_LOG_ERROR("Insufficient buffer space to load data.");
    return false;
  }

  VkDeviceSize reserve_size = index_buffer_.GetBufferSize();
  while (reserve_size *= 2 < require_size + total_used_size) {
  }
  if (reserve_size > max_buffer_size) {
    reserve_size = max_buffer_size;
  }

  output_reserve_size = reserve_size;

  return true;
}

bool MM::RenderSystem::VertexAndIndexBuffer::ChooseReserveSize(
    const BufferType& buffer_type, const VkDeviceSize& require_size,
    VkDeviceSize& output_reserve_size) {
  if (buffer_type == BufferType::VERTEX) {
    return ChooseVertexBufferReserveSize(require_size, output_reserve_size);
  }

  return ChooseIndexBufferReserveSize(require_size, output_reserve_size);
}

void MM::RenderSystem::VertexAndIndexBuffer::GetEndSizeAndOffset(
    const AllocatedBuffer& buffer,
    std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
    VkDeviceSize& output_end_size, VkDeviceSize& output_offset) {
  // The reason why MM::RenderSystem::Utils::GetEndSizeAndOffset is not used is
  // because MM::RenderSystem::Utils::GetEndSizeAndOffset checks whether the
  // input parameters are valid, which can cause performance loss, and the place
  // where this function is called can wrap the input parameters to be valid.
  if (buffer_chunks_info.empty()) {
    output_offset = 0;
    output_end_size = buffer.GetBufferSize();
  } else {
    auto end_element = --buffer_chunks_info.end();
    while (end_element->use_count() == 1) {
      end_element = buffer_chunks_info.erase(end_element);
      if (buffer_chunks_info.empty()) {
        break;
      }
      end_element = --end_element;
    }
    if (buffer_chunks_info.empty()) {
      output_offset = 0;
      output_end_size = buffer.GetBufferSize();
    } else {
      output_offset = (*end_element)->GetOffset() + (*end_element)->GetSize();
      output_end_size = buffer.GetBufferSize() - output_offset;
    }
  }
}

// MM::ExecuteResult
// MM::RenderSystem::VertexAndIndexBuffer::ScanBufferToFindSuitableArea(
//     MM::RenderSystem::AllocatedBuffer& buffer,
//     std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
//     const VkDeviceSize& require_size, VkDeviceSize& output_offset) {
//   if (buffer_chunks_info.empty()) {
//     if (require_size > buffer.GetBufferSize()) {
//       VkDeviceSize reserve_size = 0;
//       if (&buffer == &vertex_buffer_) {
//         if (!ChooseVertexBufferReserveSize(require_size, reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//         if (!ReserveVertexBuffer(reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//       } else {
//         if (!ChooseIndexBufferReserveSize(require_size, reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//         if (!ReserveIndexBuffer(reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//       }
//     }
//     output_offset = 0;
//     return true;
//   }
//   auto first_element = buffer_chunks_info.begin();
//   while (first_element->use_count() == 1) {
//     first_element = buffer_chunks_info.erase(first_element);
//     if (first_element == buffer_chunks_info.end()) {
//       break;
//     }
//   }
//   if (first_element == buffer_chunks_info.end()) {
//     if (require_size > buffer.GetBufferSize()) {
//       VkDeviceSize reserve_size = 0;
//       if (&buffer == &vertex_buffer_) {
//         if (!ChooseVertexBufferReserveSize(require_size, reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//         if (!ReserveVertexBuffer(reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//       } else {
//         if (!ChooseIndexBufferReserveSize(require_size, reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//         if (!ReserveIndexBuffer(reserve_size)) {
//           LOG_ERROR("Unable to find a suitable buffer.");
//           return false;
//         }
//       }
//       output_offset = 0;
//       return true;
//     }
//   }
//
//   if ((*first_element)->GetOffset() > require_size) {
//     output_offset = 0;
//     return true;
//   }
//
//   auto next_element = ++first_element;
//   for (; next_element != buffer_chunks_info.end();
//        ++first_element, ++next_element) {
//     const VkDeviceSize first_end =
//         (*first_element)->GetOffset() + (*first_element)->GetSize();
//     if (require_size < (*next_element)->GetOffset - first_end) {
//       output_offset = first_end;
//       return true;
//     }
//   }
//
//   VkDeviceSize reserve_size = 0;
//   if (&buffer == &vertex_buffer_) {
//     if (!ChooseVertexBufferReserveSize(require_size, reserve_size)) {
//       LOG_ERROR("Unable to find a suitable buffer.");
//       return false;
//     }
//     if (!ReserveVertexBuffer(reserve_size)) {
//       LOG_ERROR("Unable to find a suitable buffer.");
//       return false;
//     }
//     const auto the_last_element = (--vertex_buffer_chunks_info_.end());
//     output_offset =
//         (*the_last_element)->GetOffset() + (*the_last_element)->GetSize();
//     return true;
//   }
//   if (!ChooseIndexBufferReserveSize(require_size, reserve_size)) {
//     LOG_ERROR("Unable to find a suitable buffer.");
//     return false;
//   }
//   if (!ReserveIndexBuffer(reserve_size)) {
//     LOG_ERROR("Unable to find a suitable buffer.");
//     return false;
//   }
//   const auto the_last_element = (--index_buffer_chunks_info_.end());
//   output_offset =
//       (*the_last_element)->GetOffset() + (*the_last_element)->GetSize();
//   return true;
// }

// bool MM::RenderSystem::VertexAndIndexBuffer::ReserveVertexBuffer(
//     const VkDeviceSize& new_buffer_size) {
//   bool result_step1 = false;
//   bool result_step3 = false;
//
//   TaskSystem::Taskflow taskflow;
//
//   AllocatedBuffer new_buffer;
//
//   auto step1 = taskflow.emplace([&render_engine = render_engine_,
//   &new_buffer,
//                                  &result_step1, &new_buffer_size,
//                                  &index_buffer_chunks_info =
//                                      index_buffer_chunks_info_]() {
//     new_buffer = render_engine->CreateBuffer(
//         new_buffer_size,
//         VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
//             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//         VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
//     if (!new_buffer.IsValid()) {
//       LOG_ERROR("Failed to create new vertex buffer.");
//       return;
//     }
//     result_step1 = true;
//   });
//
//   auto step2 = taskflow.emplace([&render_engine = render_engine_]() {
//     // TODO 去除碎片化的空间
//   });
//
//   std::vector<VkBufferCopy2> regions;
//
//   std::vector<VkDeviceSize> index_offsets;
//   index_offsets.reserve(vertex_buffer_chunks_info_.size());
//   std::vector<VkDeviceSize> new_buffer_offsets;
//   new_buffer_offsets.reserve(vertex_buffer_chunks_info_.size());
//
//   auto step3 = taskflow.emplace(
//       [&regions, &vertex_buffer_chunks_info = vertex_buffer_chunks_info_,
//        &index_buffer_chunks_info = index_buffer_chunks_info_, &index_offsets,
//        &new_buffer_offsets]() {
//         VkDeviceSize new_buffer_offset = 0;
//         regions.reserve(vertex_buffer_chunks_info.size());
//         auto index_buffer_chunk_info = index_buffer_chunks_info.begin();
//         for (const auto& buffer_chunk_info : vertex_buffer_chunks_info) {
//           regions.emplace_back(Utils::GetBufferCopy(
//               (*buffer_chunk_info).GetSize(),
//               (*buffer_chunk_info).GetOffset(), new_buffer_offset));
//
//           // save offset data and use them after copy succeed
//           index_offsets.emplace_back(buffer_chunk_info->GetOffset() -
//                                      new_buffer_offset);
//           new_buffer_offsets.emplace_back(new_buffer_offset);
//
//           new_buffer_offset += (*buffer_chunk_info).GetSize();
//           ++index_buffer_chunk_info;
//         }
//       });
//
//   auto step4 = taskflow.emplace([&render_engine = render_engine_,
//                                  &vertex_buffer = vertex_buffer_, &regions,
//                                  &new_buffer, &result_step1, &result_step3]()
//                                  {
//     if (!result_step1) {
//       return;
//     }
//     result_step3 =
//         render_engine->CopyBuffer(vertex_buffer, new_buffer, regions);
//     if (!result_step3) {
//       LOG_ERROR("Failed to copy vertiex data to new vertex buffer.");
//     }
//   });
//
//   step3.succeed(step1, step2);
//
//   TASK_SYSTEM->RunAndWait(TaskSystem::TaskType::Render, taskflow);
//
//   if (!result_step3) {
//     LOG_ERROR("Failed to reserve vertex buffer");
//     return false;
//   }
//
//   // The next process is linear and cannot be parallel.
//   if (!stage_index_buffer.IsValid()) {
//     LOG_ERROR("Failed to create stage index buffer");
//     return false;
//   }
//
//   // TODO 直接ReserveIndexBuffer
//
//   vertex_buffer_ = new_buffer;
//
//   return true;
// }

// bool MM::RenderSystem::VertexAndIndexBuffer::ReserveIndexBuffer(
//     const VkDeviceSize& new_buffer_size) {
//   bool result_step1 = false;
//   bool result_step3 = false;
//
//   TaskSystem::Taskflow taskflow;
//
//   AllocatedBuffer new_buffer;
//
//   auto step1 = taskflow.emplace([&render_engine = render_engine_,
//   &new_buffer,
//                                  &result_step1, &new_buffer_size]() {
//     new_buffer = render_engine->CreateBuffer(
//         new_buffer_size,
//         VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
//             VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//         VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
//     if (!new_buffer.IsValid()) {
//       LOG_ERROR("Failed to create new index buffer.");
//       return;
//     }
//     result_step1 = true;
//   });
//
//   std::vector<VkBufferCopy2> regions;
//
//   auto step2 = taskflow.emplace(
//       [&regions, &index_buffer_chunks_info = index_buffer_chunks_info_]() {
//         VkDeviceSize new_buffer_offset = 0;
//         regions.reserve(index_buffer_chunks_info.size());
//         for (const auto& buffer_chunk_info : index_buffer_chunks_info) {
//           regions.emplace_back(Utils::GetBufferCopy(
//               (*buffer_chunk_info).GetSize(),
//               (*buffer_chunk_info).GetOffset(), new_buffer_offset));
//           buffer_chunk_info->SetOffset(new_buffer_offset);
//
//           new_buffer_offset += (*buffer_chunk_info).GetSize();
//         }
//       });
//
//   auto step3 = taskflow.emplace([&render_engine = render_engine_,
//                                  &index_buffer = index_buffer_, &regions,
//                                  &new_buffer, &result_step1, &result_step3]()
//                                  {
//     if (!result_step1) {
//       return;
//     }
//     result_step3 = render_engine->CopyBuffer(index_buffer, new_buffer,
//     regions); if (!result_step3) {
//       LOG_ERROR("Failed to copy vertiex data to new index buffer.");
//     }
//   });
//
//   step3.succeed(step1, step2);
//
//   TASK_SYSTEM->RunAndWait(TaskSystem::TaskType::Render, taskflow);
//
//   if (!result_step3) {
//     LOG_ERROR("Failed to reserve vertex buffer");
//     return false;
//   }
//
//   index_buffer_ = new_buffer;
//
//   return true;
// }

bool MM::RenderSystem::VertexAndIndexBuffer::Reserve(
    const VkDeviceSize& new_vertex_buffer_size,
    const VkDeviceSize& new_index_buffer_size) {
  TaskSystem::Taskflow task_flow;
  bool buffer_result = false, index_result = false;
  task_flow.emplace(
      [object = this, &buffer_result, new_vertex_buffer_size]() {
        buffer_result = (*object).ReserveVertexBuffer(new_vertex_buffer_size);
      },
      [object = this, &index_result, new_index_buffer_size]() {
        index_result = (*object).ReserveIndexBuffer(new_index_buffer_size);
      });

  auto future = MM_TASK_SYSTEM->Run(TaskSystem::TaskType::Render, task_flow);
  future.get();

  if (buffer_result && index_result) {
    return true;
  }
  return false;
}

bool MM::RenderSystem::operator==(const ManagedObjectBase& lhs,
                                  const ManagedObjectBase& rhs) {
  return lhs.object_ID_ == rhs.object_ID_;
}

bool MM::RenderSystem::operator!=(const ManagedObjectBase& lhs,
                                  const ManagedObjectBase& rhs) {
  return !(lhs == rhs);
}
