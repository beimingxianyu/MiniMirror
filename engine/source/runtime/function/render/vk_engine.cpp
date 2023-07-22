#include "runtime/function/render/vk_engine.h"

#include <set>

const std::string MM::RenderSystem::RenderEngine::validation_layers_name{
    "VK_LAYER_KHRONOS_validation"};

void MM::RenderSystem::RenderEngine::Init() {
  InitGlfw();
  InitVulkan();
  InitInfo();

  is_initialized_ = true;
}

void MM::RenderSystem::RenderEngine::Run() {
  while (!glfwWindowShouldClose(window_)) {
    // TODO Update resource
    // TODO Update render graph
    // Draw();
    glfwPollEvents();
  }
}

void MM::RenderSystem::RenderEngine::CleanUp() {
  if (is_initialized_) {
    // vkDestroyCommandPool(device_, compute_command_pool_, nullptr);
    // vkDestroyCommandPool(device_, graph_command_pool_, nullptr);

    for (VkImageView swapchain_image_view : swapchain_image_views_) {
      vkDestroyImageView(device_, swapchain_image_view, nullptr);
    }

    vkDestroySwapchainKHR(device_, swapchain_, nullptr);

    vmaDestroyAllocator(allocator_);

    vkDestroyDevice(device_, nullptr);

    if (enable_validation_layers_) {
      DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
    }

    vkDestroySurfaceKHR(instance_, surface_, nullptr);

    vkDestroyInstance(instance_, nullptr);

    glfwDestroyWindow(window_);
    glfwTerminate();

    window_ = nullptr;
    instance_ = nullptr;
    physical_device_ = nullptr;
    allocator_ = nullptr;
    device_ = nullptr;
    graphics_queue_ = nullptr;
    compute_queue_ = nullptr;
    present_queue_ = nullptr;
    surface_ = nullptr;
    swapchain_ = nullptr;
  }

  is_initialized_ = false;
}

bool MM::RenderSystem::RenderEngine::IsValid() const { return is_initialized_; }

VmaAllocator MM::RenderSystem::RenderEngine::GetAllocator() {
  return allocator_;
}

const VmaAllocator_T* MM::RenderSystem::RenderEngine::GetAllocator() const {
  return allocator_;
}

VkDevice MM::RenderSystem::RenderEngine::GetDevice() { return device_; }

const VkDevice_T* MM::RenderSystem::RenderEngine::GetDevice() const {
  return device_;
}

const std::uint32_t& MM::RenderSystem::RenderEngine::GetGraphQueueIndex()
    const {
  return queue_family_indices_.graphics_family_.value();
}

const std::uint32_t& MM::RenderSystem::RenderEngine::GetTransformQueueIndex()
    const {
  return queue_family_indices_.transform_family_.value();
}

const std::uint32_t& MM::RenderSystem::RenderEngine::GetPresentQueueIndex()
    const {
  return queue_family_indices_.present_family_.value();
}

const std::uint32_t& MM::RenderSystem::RenderEngine::GetComputeQueueIndex()
    const {
  return queue_family_indices_.compute_family_.value();
}

const VkQueue& MM::RenderSystem::RenderEngine::GetGraphQueue() const {
  return graphics_queue_;
}

const VkQueue& MM::RenderSystem::RenderEngine::GetTransformQueue() const {
  return transform_queue_;
}

const VkQueue& MM::RenderSystem::RenderEngine::GetPresentQueue() const {
  return present_queue_;
}

const VkQueue& MM::RenderSystem::RenderEngine::GetComputeQueue() const {
  return compute_queue_;
}

MM::RenderSystem::RenderFuture MM::RenderSystem::RenderEngine::RunCommand(
    CommandTaskFlow&& command_task_flow) {
  assert(IsValid());
  return command_executor_->Run(std::move(command_task_flow));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::RunCommandAndWait(
    CommandTaskFlow&& command_task_flow) {
  assert(IsValid());
  return command_executor_->RunAndWait(std::move(command_task_flow));
}

MM::RenderSystem::RenderFuture MM::RenderSystem::RenderEngine::RunCommand(
    CommandTaskFlow& command_task_flow) {
  assert(IsValid());
  return command_executor_->Run(std::move(command_task_flow));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::RunCommandAndWait(
    CommandTaskFlow& command_task_flow) {
  assert(IsValid());
  return command_executor_->RunAndWait(std::move(command_task_flow));
}

MM::RenderSystem::RenderFuture MM::RenderSystem::RenderEngine::RunSingleCommand(
    CommandBufferType command_type, std::uint32_t use_render_resource_count,
    const std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>& commands) {
  assert(IsValid());
  CommandTaskFlow command_task_flow;
  command_task_flow.AddTask(command_type, commands, use_render_resource_count,
                            std::vector<WaitAllocatedSemaphore>(),
                            std::vector<AllocateSemaphore>());

  return command_executor_->Run(std::move(command_task_flow));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::RunSingleCommandAndWait(
    CommandBufferType command_type, std::uint32_t use_render_resource_count,
    const std::function<MM::ExecuteResult(
        MM::RenderSystem::AllocatedCommandBuffer& cmd)>& commands) {
  assert(IsValid());
  CommandTaskFlow command_task_flow;
  command_task_flow.AddTask(command_type, commands, use_render_resource_count,
                            std::vector<WaitAllocatedSemaphore>(),
                            std::vector<AllocateSemaphore>());

  return command_executor_->RunAndWait(std::move(command_task_flow));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyBuffer(
    AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<VkBufferCopy2>& regions) {
  assert(IsValid());
#ifdef CHECK_PARAMETERS
  MM_CHECK_WITHOUT_LOG(
      CopyBufferInputParametersCheck(src_buffer, dest_buffer, regions),
      return MM_RESULT_CODE;)
#endif
  VkCopyBufferInfo2 copy_buffer =
      Utils::GetVkCopyBufferInfo2(src_buffer, dest_buffer, regions);
  std::uint32_t use_buffer_count;
  std::vector<RenderResourceDataID> sync_resource_data_IDs;
  if (src_buffer.GetBuffer() == dest_buffer.GetBuffer()) {
    use_buffer_count = 1;
    sync_resource_data_IDs.emplace_back(src_buffer.GetRenderResourceDataID());
  } else {
    use_buffer_count = 2;
    sync_resource_data_IDs.emplace_back(src_buffer.GetRenderResourceDataID());
    sync_resource_data_IDs.emplace_back(dest_buffer.GetRenderResourceDataID());
  }
  MM_CHECK(
      RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, use_buffer_count,
          [&copy_buffer, &src_buffer, &dest_buffer,
           use_buffer_count](AllocatedCommandBuffer& cmd) {
            std::vector<VkBufferMemoryBarrier2> barriers;
            if (use_buffer_count == 1) {
              VkDeviceSize transform_offset = UINT32_MAX, transform_size = 0,
                           end_size = 0;
              for (std::uint64_t i = 0; i != copy_buffer.regionCount; ++i) {
                if (copy_buffer.pRegions[i].srcOffset < transform_offset) {
                  transform_offset = copy_buffer.pRegions[i].srcOffset;
                }
                if (copy_buffer.pRegions[i].dstOffset < transform_offset) {
                  transform_offset = copy_buffer.pRegions[i].dstOffset;
                }
                if (copy_buffer.pRegions[i].srcOffset +
                        copy_buffer.pRegions[i].size >
                    end_size) {
                  end_size = copy_buffer.pRegions[i].srcOffset +
                             copy_buffer.pRegions[i].size;
                }
                if (copy_buffer.pRegions[i].dstOffset +
                        copy_buffer.pRegions[i].size >
                    end_size) {
                  end_size = copy_buffer.pRegions[i].dstOffset +
                             copy_buffer.pRegions[i].size;
                }
              }
              transform_size = end_size - transform_offset;

              MM_CHECK(
                  src_buffer.GetVkBufferMemoryBarriber2(
                      transform_offset, transform_size,
                      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                      VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                      VK_ACCESS_2_TRANSFER_READ_BIT,
                      src_buffer.GetRenderEnginePtr()->GetTransformQueueIndex(),
                      barriers),
                  return MM_RESULT_CODE;)
            } else {
              VkDeviceSize src_transform_offset = UINT32_MAX,
                           src_transform_size = 0, src_end_size = 0,
                           dest_transform_offset = UINT32_MAX,
                           dest_transform_size = 0, dest_end_size = 0;
              for (std::uint64_t i = 0; i != copy_buffer.regionCount; ++i) {
                if (copy_buffer.pRegions[i].srcOffset < src_transform_offset) {
                  src_transform_offset = copy_buffer.pRegions[i].srcOffset;
                }
                if (copy_buffer.pRegions[i].dstOffset < src_transform_offset) {
                  dest_transform_offset = copy_buffer.pRegions[i].dstOffset;
                }
                if (copy_buffer.pRegions[i].srcOffset +
                        copy_buffer.pRegions[i].size >
                    src_end_size) {
                  src_end_size = copy_buffer.pRegions[i].srcOffset +
                                 copy_buffer.pRegions[i].size;
                }
                if (copy_buffer.pRegions[i].dstOffset +
                        copy_buffer.pRegions[i].size >
                    src_end_size) {
                  dest_end_size = copy_buffer.pRegions[i].dstOffset +
                                  copy_buffer.pRegions[i].size;
                }
              }
              src_transform_size = src_end_size - src_transform_offset;
              dest_transform_size = dest_end_size - dest_transform_offset;

              MM_CHECK(
                  src_buffer.GetVkBufferMemoryBarriber2(
                      src_transform_offset, src_transform_size,
                      VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                      VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                      VK_ACCESS_2_TRANSFER_READ_BIT,
                      src_buffer.GetRenderEnginePtr()->GetTransformQueueIndex(),
                      barriers),
                  return MM_RESULT_CODE;)
              MM_CHECK(src_buffer.GetVkBufferMemoryBarriber2(
                           dest_transform_offset, dest_transform_size,
                           VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                           VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                           VK_ACCESS_2_TRANSFER_READ_BIT,
                           dest_buffer.GetRenderEnginePtr()
                               ->GetTransformQueueIndex(),
                           barriers),
                       return MM_RESULT_CODE;)
            }

            VkDependencyInfo dependency_info{Utils::GetVkDependencyInfo(
                0, nullptr, barriers.size(), barriers.data(), 0, nullptr, 0)};
            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to begin command buffer.");
                     return MM_RESULT_CODE;);

            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &copy_buffer);

            for (auto& barrier : barriers) {
              barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
              barrier.dstQueueFamilyIndex = VK_ACCESS_2_TRANSFER_WRITE_BIT;
              barrier.srcQueueFamilyIndex =
                  src_buffer.GetRenderEnginePtr()->GetTransformQueueIndex();
              barrier.dstQueueFamilyIndex = barrier.srcQueueFamilyIndex;
            }
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to end command buffer.");
                     return MM_RESULT_CODE;);
            return ExecuteResult::SUCCESS;
          },
          sync_resource_data_IDs),
      MM_LOG_ERROR("Failed to copy buffer.");
      return MM_RESULT_CODE;)

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyBuffer(
    const AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<VkBufferCopy2>& regions) {
  return CopyBuffer(src_buffer, dest_buffer, regions);
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyBuffer(
    AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<std::vector<VkBufferCopy2>>& regions_vector) {
  assert(IsValid());
  for (const auto& regions : regions_vector) {
    MM_CHECK(CopyBuffer(src_buffer, dest_buffer, regions),
             return MM_RESULT_CODE;)
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyBuffer(
    const AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
    const std::vector<std::vector<VkBufferCopy2>>& regions_vector) {
  assert(IsValid());
  for (const auto& regions : regions_vector) {
    MM_CHECK(CopyBuffer(src_buffer, dest_buffer, regions),
             return MM_RESULT_CODE;)
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyImage(
    AllocatedImage& src_image, AllocatedImage& dest_image,
    const std::vector<VkImageCopy2>& regions) {
  assert(IsValid());
#ifdef CHECK_PARAMETERS
  MM_CHECK_WITHOUT_LOG(
      CopyImageInputParametersCheck(src_image, dest_image, regions),
      return MM_RESULT_CODE;)
#endif

  std::vector<RenderResourceDataID> sync_render_resource_data_IDs;
  if (src_image.GetImage() == dest_image.GetImage()) {
    sync_render_resource_data_IDs.emplace_back(
        src_image.GetRenderResourceDataID());
  } else {
    sync_render_resource_data_IDs.emplace_back(
        src_image.GetRenderResourceDataID());
    sync_render_resource_data_IDs.emplace_back(
        dest_image.GetRenderResourceDataID());
  }

  MM_CHECK(
      RunSingleCommandAndWait(
          CommandBufferType::TRANSFORM, 1,
          [&src_image, &dest_image, &regions,
           this_render_engine = this](AllocatedCommandBuffer& cmd) {
            MM_CHECK(Utils::BeginCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to begin command buffer");
                     return MM_RESULT_CODE;)

            bool is_same = (src_image.GetImage() == dest_image.GetImage());
            std::vector<std::pair<std::uint64_t,
                                  std::pair<VkImageLayout, VkImageLayout>>>
                split_infos;
            std::set<const ImageSubResourceAttribute*> effect_src_image;
            std::set<const ImageSubResourceAttribute*> effect_dest_image;
            VkImageLayout src_layout, dest_layout;
            for (const auto& src_sub_resource :
                 src_image.GetSubResourceAttributes()) {
              if (regions[0].srcSubresource.mipLevel >=
                      src_sub_resource.GetBaseMipmapLevel() &&
                  regions[0].srcSubresource.mipLevel <
                      src_sub_resource.GetBaseMipmapLevel() +
                          src_sub_resource.GetMipmapCount()) {
                src_layout = src_sub_resource.GetImageLayout();
                effect_src_image.insert(&src_sub_resource);
              }
            }
            for (const auto& dest_sub_resource :
                 dest_image.GetSubResourceAttributes()) {
              if (regions[0].srcSubresource.mipLevel >=
                      dest_sub_resource.GetBaseMipmapLevel() &&
                  regions[0].srcSubresource.mipLevel <
                      dest_sub_resource.GetBaseMipmapLevel() +
                          dest_sub_resource.GetMipmapCount()) {
                dest_layout = dest_sub_resource.GetImageLayout();
                if (!is_same) {
                  effect_dest_image.insert(&dest_sub_resource);
                }
              }
            }
            split_infos.emplace_back(0,
                                     std::make_pair(src_layout, dest_layout));

            for (std::uint64_t i = 1; i != regions.size(); ++i) {
              VkImageLayout new_src_layout, new_dest_layout;
              for (const auto& src_sub_resource :
                   src_image.GetSubResourceAttributes()) {
                if (regions[i].srcSubresource.mipLevel >=
                        src_sub_resource.GetBaseMipmapLevel() &&
                    regions[1].srcSubresource.mipLevel <
                        src_sub_resource.GetBaseMipmapLevel() +
                            src_sub_resource.GetMipmapCount()) {
                  new_src_layout = src_sub_resource.GetImageLayout();
                  effect_src_image.insert(&src_sub_resource);
                }
              }
              for (const auto& dest_sub_resource :
                   dest_image.GetSubResourceAttributes()) {
                if (regions[1].srcSubresource.mipLevel >=
                        dest_sub_resource.GetBaseMipmapLevel() &&
                    regions[1].srcSubresource.mipLevel <
                        dest_sub_resource.GetBaseMipmapLevel() +
                            dest_sub_resource.GetMipmapCount()) {
                  new_dest_layout = dest_sub_resource.GetImageLayout();
                  if (!is_same) {
                    effect_dest_image.insert(&dest_sub_resource);
                  }
                }
              }
              if (new_src_layout != src_layout ||
                  new_dest_layout != dest_layout) {
                split_infos.emplace_back(
                    i, std::make_pair(new_src_layout, new_dest_layout));
                src_layout = new_src_layout;
                dest_layout = new_dest_layout;
              }
            }

            QueueIndex transform_queue_index =
                this_render_engine->GetTransformQueueIndex();
            std::vector<VkImageMemoryBarrier2> barriers;
            for (const auto& sub_resource :
                 src_image.GetSubResourceAttributes()) {
              if (effect_src_image.count(&sub_resource)) {
                VkImageAspectFlags aspect_flag =
                    MM::RenderSystem::Utils::ChooseImageAspectFlags(
                        sub_resource.GetImageLayout());
                barriers.emplace_back(
                    MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        VK_ACCESS_2_TRANSFER_READ_BIT,
                        sub_resource.GetImageLayout(),
                        sub_resource.GetImageLayout(),
                        sub_resource.GetQueueIndex(), transform_queue_index,
                        src_image.GetImage(),
                        VkImageSubresourceRange{
                            aspect_flag, sub_resource.GetBaseMipmapLevel(),
                            sub_resource.GetMipmapCount(),
                            sub_resource.GetBaseArrayLevel(),
                            sub_resource.GetArrayCount()}));
              }
            }
            if (!is_same) {
              for (const auto& dest_resource :
                   dest_image.GetSubResourceAttributes()) {
                if (effect_src_image.count(&dest_resource)) {
                  VkImageAspectFlags aspect_flag =
                      MM::RenderSystem::Utils::ChooseImageAspectFlags(
                          dest_resource.GetImageLayout());
                  barriers.emplace_back(
                      MM::RenderSystem::Utils::GetVkImageMemoryBarrier2(
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                          VK_ACCESS_2_TRANSFER_WRITE_BIT,
                          dest_resource.GetImageLayout(),
                          dest_resource.GetImageLayout(),
                          dest_resource.GetQueueIndex(), transform_queue_index,
                          dest_image.GetImage(),
                          VkImageSubresourceRange{
                              aspect_flag, dest_resource.GetBaseMipmapLevel(),
                              dest_resource.GetMipmapCount(),
                              dest_resource.GetBaseArrayLevel(),
                              dest_resource.GetArrayCount()}));
                }
              }
            }

            VkDependencyInfo dependency_info =
                MM::RenderSystem::Utils::GetVkDependencyInfo(
                    0, nullptr, 0, nullptr, barriers.size(), barriers.data(),
                    0);
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            VkCopyImageInfo2 copy_image_info{};
            copy_image_info.srcImage = src_image.GetImage();
            copy_image_info.dstImage = dest_image.GetImage();
            for (std::uint64_t i = 0; i != split_infos.size(); ++i) {
              copy_image_info.srcImageLayout = split_infos[i].second.first;
              copy_image_info.dstImageLayout = split_infos[i].second.second;
              if (i + 1 != split_infos.size()) {
                copy_image_info.regionCount =
                    split_infos[i + 1].first - split_infos[i].first;
              } else {
                copy_image_info.regionCount =
                    regions.size() - split_infos[i].first;
              }
              copy_image_info.pRegions = regions.data() + split_infos[i].first;
              vkCmdCopyImage2(cmd.GetCommandBuffer(), &copy_image_info);
            }

            for (VkImageMemoryBarrier2& barrier : barriers) {
              barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
              barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
              barrier.dstQueueFamilyIndex = barrier.srcQueueFamilyIndex;
              barrier.srcQueueFamilyIndex = transform_queue_index;
            }
            vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);

            MM_CHECK(Utils::EndCommandBuffer(cmd),
                     MM_LOG_FATAL("Failed to end command buffer.");
                     return MM_RESULT_CODE;)
            return ExecuteResult::SUCCESS;
          },
          sync_render_resource_data_IDs),
      MM_LOG_ERROR("Failed to copy image.");
      return MM_RESULT_CODE;)

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyImage(
    const AllocatedImage& src_image, AllocatedImage& dest_image,
    const std::vector<VkImageCopy2>& regions) {
  return CopyImage(const_cast<AllocatedImage&>(src_image), dest_image, regions);
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyImage(
    AllocatedImage& src_image, AllocatedImage& dest_image,
    const std::vector<std::vector<VkImageCopy2>>& regions_vector) {
  assert(IsValid());
  for (const auto& regions : regions_vector) {
    MM_CHECK(CopyImage(src_image, dest_image, regions), return MM_RESULT_CODE;)
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyImage(
    const AllocatedImage& src_image, AllocatedImage& dest_image,
    const std::vector<std::vector<VkImageCopy2>>& regions_vector) {
  assert(IsValid());
  for (const auto& regions : regions_vector) {
    MM_CHECK(CopyImage(src_image, dest_image, regions), return MM_RESULT_CODE;)
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyDataToBuffer(
    AllocatedBuffer& dest_buffer, const void* data,
    const VkDeviceSize& copy_offset, const VkDeviceSize& copy_size) {
  assert(IsValid());
#ifdef CHECK_PARAMETERS
  if (!dest_buffer.IsValid()) {
    MM_LOG_ERROR("Dest_buffer is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  if (!dest_buffer.CanMapped()) {
    MM_LOG_ERROR("Dest_buffer can not mapped.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }

  if (data == nullptr) {
    MM_LOG_ERROR("data is nullptr.");
  }

  if (copy_offset + copy_size > dest_buffer.GetBufferSize()) {
    MM_LOG_ERROR("The sum of copy_offset and copy_size is too large.");
  }
#endif

  char* map_data;
  vmaMapMemory(dest_buffer.GetAllocator(), dest_buffer.GetAllocation(),
               reinterpret_cast<void**>(&map_data));
  map_data += copy_offset;
  memcpy(map_data, data, copy_size);
  vmaUnmapMemory(dest_buffer.GetAllocator(), dest_buffer.GetAllocation());

  return ExecuteResult::SUCCESS;
}

// MM::ExecuteResult MM::RenderSystem::RenderEngine::RemoveBufferFragmentation(
//     AllocatedBuffer& buffer,
//     std::vector<BufferSubResourceAttribute>& buffer_chunks_info) {
//   assert(IsValid());
// #ifdef CHECK_PARAMETERS
//   if (!buffer.IsValid()) {
//     LOG_ERROR("buffer is invalid.");
//     return ExecuteResult::OBJECT_IS_INVALID;
//   }
//   if (buffer_chunks_info.empty()) {
//     return ExecuteResult::SUCCESS;
//   }
// #endif
//
//   // Used to mark buffer chunk that will be moved to the stage buffer
//   std::vector<int> stage_flag(buffer_chunks_info.size(), 0);
//   VkDeviceSize pre_valid_size = 0;
//   VkDeviceSize stage_buffer_size = 0;
//   std::vector<VkBufferCopy2> self_copy_regions;
//   std::vector<VkBufferCopy2> self_copy_to_stage_regions;
//   std::vector<VkBufferCopy2> stage_copy_to_self_regions;
//
//   for (std::size_t i = 0; i < buffer_chunks_info.size(); ++i) {
//     if (pre_valid_size + buffer_chunks_info[i].GetChunkInfo().GetSize() >=
//         buffer_chunks_info[i].GetChunkInfo().GetOffset()) {
//       stage_flag[i] = 1;
//       self_copy_to_stage_regions.push_back(Utils::GetBufferCopy(
//           buffer_chunks_info[i].GetChunkInfo().GetSize(),
//           buffer_chunks_info[i].GetChunkInfo().GetOffset(),
//           stage_buffer_size));
//       stage_copy_to_self_regions.push_back(
//           Utils::GetBufferCopy(buffer_chunks_info[i].GetChunkInfo().GetSize(),
//                                stage_buffer_size, pre_valid_size));
//       pre_valid_size += buffer_chunks_info[i].GetChunkInfo().GetSize();
//       stage_buffer_size += buffer_chunks_info[i].GetChunkInfo().GetSize();
//       continue;
//     }
//
//     // TODO Optimize to binary search.
//     const VkDeviceSize new_offset =
//         buffer_chunks_info[i].GetChunkInfo().GetOffset() - pre_valid_size;
//     for (std::size_t j = 0; j < i; ++j) {
//       if (stage_flag[j] != 1) {
//         if (new_offset <= buffer_chunks_info[j].GetChunkInfo().GetOffset() +
//                               buffer_chunks_info[j].GetChunkInfo().GetSize())
//                               {
//           if (new_offset + buffer_chunks_info[i].GetChunkInfo().GetSize() >=
//               buffer_chunks_info[j].GetChunkInfo().GetOffset()) {
//             stage_flag[i] = 1;
//             self_copy_to_stage_regions.push_back(Utils::GetBufferCopy(
//                 buffer_chunks_info[i].GetChunkInfo().GetSize(),
//                 buffer_chunks_info[i].GetChunkInfo().GetOffset(),
//                 stage_buffer_size));
//             stage_copy_to_self_regions.push_back(Utils::GetBufferCopy(
//                 buffer_chunks_info[i].GetChunkInfo().GetSize(),
//                 stage_buffer_size, pre_valid_size));
//             pre_valid_size += buffer_chunks_info[i].GetChunkInfo().GetSize();
//             stage_buffer_size +=
//             buffer_chunks_info[i].GetChunkInfo().GetSize(); continue;
//           }
//         }
//       }
//     }
//
//     self_copy_regions.push_back(Utils::GetBufferCopy(
//         buffer_chunks_info[i].GetChunkInfo().GetSize(),
//         buffer_chunks_info[i].GetChunkInfo().GetOffset(),
//         buffer_chunks_info[i].GetChunkInfo().GetOffset() - pre_valid_size));
//     pre_valid_size += buffer_chunks_info[i].GetChunkInfo().GetSize();
//   }
//
//   QueueIndex transform_queue_index = GetTransformQueueIndex();
//   VkBufferCreateInfo buffer_create_info{
//       MM::RenderSystem::Utils::GetVkBufferCreateInfo(
//           nullptr, 0, stage_buffer_size,
//           VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
//           VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE, 1,
//           &transform_queue_index)};
//   VmaAllocationCreateInfo vma_allocation_create_info{
//       0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, 0, 0, nullptr, nullptr,
//       0.5};
//   AllocatedBuffer stage_buffer;
//   MM_CHECK(CreateBuffer(buffer_create_info, vma_allocation_create_info,
//   nullptr,
//                         stage_buffer),
//            LOG_ERROR("Failed to create stage buffer.");
//            return MM_RESULT_CODE;)
//
//   auto self_copy_info =
//       Utils::GetVkCopyBufferInfo2(buffer, buffer, self_copy_regions);
//   auto self_copy_to_stage_info = Utils::GetVkCopyBufferInfo2(
//       buffer, stage_buffer, self_copy_to_stage_regions);
//   auto stage_copy_to_self_info = Utils::GetVkCopyBufferInfo2(
//       stage_buffer, buffer, stage_copy_to_self_regions);
//
//   MM_CHECK(
//       RunSingleCommandAndWait(
//           CommandBufferType::TRANSFORM, 1,
//           [&self_copy_info, &self_copy_to_stage_info,
//           &stage_copy_to_self_info,
//            &buffer_chunks_info,
//            this_render_engine = this](AllocatedCommandBuffer& cmd) {
//             MM_CHECK(Utils::BeginCommandBuffer(cmd),
//                      LOG_ERROR("Failed to begin command buffer.");
//                      return MM_RESULT_CODE;)
//
//             QueueIndex transform_queue_index =
//                 this_render_engine->GetTransformQueueIndex();
//             std::vector<VkBufferMemoryBarrier2> barriers;
//             barriers.reserve(buffer_chunks_info.size());
//             for (const auto& buffer_chunk_info : buffer_chunks_info) {
//               barriers.emplace_back(
//                   MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
//                       VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
//                       VK_PIPELINE_STAGE_2_TRANSFER_BIT,
//                       VK_ACCESS_2_TRANSFER_READ_BIT,
//                       buffer_chunk_info.GetQueueIndex(),
//                       transform_queue_index, self_copy_info.srcBuffer,
//                       buffer_chunk_info.GetChunkInfo().GetOffset(),
//                       buffer_chunk_info.GetChunkInfo().GetSize()));
//             }
//             VkDependencyInfo dependency_info{
//                 MM::RenderSystem::Utils::GetVkDependencyInfo(
//                     0, nullptr, barriers.size(), barriers.data(), 0, nullptr,
//                     0)};
//
//             vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
//
//             vkCmdCopyBuffer2(cmd.GetCommandBuffer(),
//             &self_copy_to_stage_info);
//
//             vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &self_copy_info);
//
//             vkCmdCopyBuffer2(cmd.GetCommandBuffer(),
//             &stage_copy_to_self_info);
//
//             for (auto& barrier : barriers) {
//               barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
//               barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
//               barrier.dstQueueFamilyIndex = barrier.srcQueueFamilyIndex;
//               barrier.srcQueueFamilyIndex = transform_queue_index;
//             }
//
//             vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
//
//             MM_CHECK(Utils::EndCommandBuffer(cmd),
//                      LOG_ERROR("Failed to end command buffer.");
//                      return MM_RESULT_CODE;)
//
//             return ExecuteResult::SUCCESS;
//           },
//           std::vector<RenderResourceDataID>{buffer.GetRenderResourceDataID()}),
//       LOG_ERROR("An error occurred during buffer move while removing the "
//                 "fragmentation "
//                 "buffer.");
//       return MM_RESULT_CODE;)
//
//   VkDeviceSize new_offset = 0;
//   for (auto& buffer_chunk_info : buffer_chunks_info) {
//     buffer_chunk_info.SetOffset(new_offset);
//     new_offset += buffer_chunk_info.GetSize();
//   }
//
//   return ExecuteResult::SUCCESS;
// }
//
// MM::ExecuteResult MM::RenderSystem::RenderEngine::RemoveBufferFragmentation(
//     AllocatedBuffer& buffer,
//     std::list<BufferSubResourceAttribute>& buffer_chunks_info) {
//   assert(IsValid());
// #ifdef CHECK_PARAMETERS
//   if (!buffer.IsValid()) {
//     LOG_ERROR("buffer is invalid.");
//     return ExecuteResult::OBJECT_IS_INVALID;
//   }
//   if (buffer_chunks_info.empty()) {
//     return ExecuteResult::SUCCESS;
//   }
// #endif
//
//   // Used to mark buffer chunk that will be moved to the stage buffer
//   std::vector<int> stage_flag(buffer_chunks_info.size(), 0);
//   VkDeviceSize pre_valid_size = 0;
//   VkDeviceSize stage_buffer_size = 0;
//   std::vector<VkBufferCopy2> self_copy_regions;
//   std::vector<VkBufferCopy2> self_copy_to_stage_regions;
//   std::vector<VkBufferCopy2> stage_copy_to_self_regions;
//
//   std::uint32_t index = 0;
//   for (auto buffer_chunk_info = buffer_chunks_info.begin();
//        buffer_chunk_info != buffer_chunks_info.end();
//        ++buffer_chunk_info, ++index) {
//     if (pre_valid_size + buffer_chunk_info->GetChunkInfo().GetSize() >=
//         buffer_chunk_info->GetChunkInfo().GetOffset()) {
//       stage_flag[index] = 1;
//       self_copy_to_stage_regions.push_back(Utils::GetBufferCopy(
//           buffer_chunk_info->GetChunkInfo().GetSize(),
//           buffer_chunk_info->GetChunkInfo().GetOffset(), stage_buffer_size));
//       stage_copy_to_self_regions.push_back(
//           Utils::GetBufferCopy(buffer_chunk_info->GetChunkInfo().GetSize(),
//                                stage_buffer_size, pre_valid_size));
//       pre_valid_size += buffer_chunk_info->GetChunkInfo().GetSize();
//       stage_buffer_size += buffer_chunk_info->GetChunkInfo().GetSize();
//       continue;
//     }
//
//     // TODO Optimize to binary search.
//     const VkDeviceSize new_offset =
//         buffer_chunk_info->GetChunkInfo().GetOffset() - pre_valid_size;
//     std::uint32_t index2 = 0;
//     for (auto buffer_chunk_info2 = buffer_chunks_info.begin();
//          buffer_chunk_info2 != buffer_chunk_info;
//          ++buffer_chunk_info2, ++index2) {
//       if (stage_flag[index2] != 1) {
//         if (new_offset <= buffer_chunk_info2->GetChunkInfo().GetOffset() +
//                               buffer_chunk_info2->GetChunkInfo().GetSize()) {
//           if (new_offset + buffer_chunk_info->GetChunkInfo().GetSize() >=
//               buffer_chunk_info2->GetChunkInfo().GetOffset()) {
//             stage_flag[index] = 1;
//             self_copy_to_stage_regions.push_back(Utils::GetBufferCopy(
//                 buffer_chunk_info->GetChunkInfo().GetSize(),
//                 buffer_chunk_info->GetChunkInfo().GetOffset(),
//                 stage_buffer_size));
//             stage_copy_to_self_regions.push_back(Utils::GetBufferCopy(
//                 buffer_chunk_info->GetChunkInfo().GetSize(),
//                 stage_buffer_size, pre_valid_size));
//             pre_valid_size += buffer_chunk_info->GetChunkInfo().GetSize();
//             stage_buffer_size += buffer_chunk_info->GetChunkInfo().GetSize();
//             continue;
//           }
//         }
//       }
//     }
//
//     self_copy_regions.push_back(Utils::GetBufferCopy(
//         buffer_chunk_info->GetChunkInfo().GetSize(),
//         buffer_chunk_info->GetChunkInfo().GetOffset(),
//         buffer_chunk_info->GetChunkInfo().GetOffset() - pre_valid_size));
//     pre_valid_size += buffer_chunk_info->GetChunkInfo().GetSize();
//   }
//
//   QueueIndex transform_queue_index = GetTransformQueueIndex();
//   VkBufferCreateInfo buffer_create_info{
//       MM::RenderSystem::Utils::GetVkBufferCreateInfo(
//           nullptr, 0, stage_buffer_size,
//           VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
//           VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE, 1,
//           &transform_queue_index)};
//   VmaAllocationCreateInfo vma_allocation_create_info{
//       0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0, 0, 0, nullptr, nullptr,
//       0.5};
//   AllocatedBuffer stage_buffer;
//   MM_CHECK(CreateBuffer(buffer_create_info, vma_allocation_create_info,
//   nullptr,
//                         stage_buffer),
//            LOG_ERROR("Failed to create stage buffer.");
//            return MM_RESULT_CODE;)
//
//   auto self_copy_info =
//       Utils::GetVkCopyBufferInfo2(buffer, buffer, self_copy_regions);
//   auto self_copy_to_stage_info = Utils::GetVkCopyBufferInfo2(
//       buffer, stage_buffer, self_copy_to_stage_regions);
//   auto stage_copy_to_self_info = Utils::GetVkCopyBufferInfo2(
//       stage_buffer, buffer, stage_copy_to_self_regions);
//
//   MM_CHECK(
//       RunSingleCommandAndWait(
//           CommandBufferType::TRANSFORM, 1,
//           [&self_copy_info, &self_copy_to_stage_info,
//           &stage_copy_to_self_info,
//            &buffer_chunks_info,
//            this_render_engine = this](AllocatedCommandBuffer& cmd) {
//             MM_CHECK(Utils::BeginCommandBuffer(cmd),
//                      LOG_ERROR("Failed to begin command buffer.");
//                      return MM_RESULT_CODE;)
//
//             QueueIndex transform_queue_index =
//                 this_render_engine->GetTransformQueueIndex();
//             std::vector<VkBufferMemoryBarrier2> barriers;
//             barriers.reserve(buffer_chunks_info.size());
//             for (const auto& buffer_chunk_info : buffer_chunks_info) {
//               barriers.emplace_back(
//                   MM::RenderSystem::Utils::GetVkBufferMemoryBarrier2(
//                       VK_PIPELINE_STAGE_2_TRANSFER_BIT, 0,
//                       VK_PIPELINE_STAGE_2_TRANSFER_BIT,
//                       VK_ACCESS_2_TRANSFER_READ_BIT,
//                       buffer_chunk_info.GetQueueIndex(),
//                       transform_queue_index, self_copy_info.srcBuffer,
//                       buffer_chunk_info.GetChunkInfo().GetOffset(),
//                       buffer_chunk_info.GetChunkInfo().GetSize()));
//             }
//             VkDependencyInfo dependency_info{
//                 MM::RenderSystem::Utils::GetVkDependencyInfo(
//                     0, nullptr, barriers.size(), barriers.data(), 0, nullptr,
//                     0)};
//             vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
//
//             vkCmdCopyBuffer2(cmd.GetCommandBuffer(),
//             &self_copy_to_stage_info);
//
//             vkCmdCopyBuffer2(cmd.GetCommandBuffer(), &self_copy_info);
//
//             vkCmdCopyBuffer2(cmd.GetCommandBuffer(),
//             &stage_copy_to_self_info);
//
//             for (auto& barrier : barriers) {
//               barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
//               barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
//               barrier.dstQueueFamilyIndex = barrier.srcQueueFamilyIndex;
//               barrier.srcQueueFamilyIndex = transform_queue_index;
//             }
//
//             vkCmdPipelineBarrier2(cmd.GetCommandBuffer(), &dependency_info);
//
//             MM_CHECK(Utils::EndCommandBuffer(cmd),
//                      LOG_ERROR("Failed to end command buffer.");
//                      return MM_RESULT_CODE;)
//
//             return ExecuteResult::SUCCESS;
//           },
//           std::vector<RenderResourceDataID>{buffer.GetRenderResourceDataID()}),
//       LOG_ERROR("An error occurred during buffer move while removing the "
//                 "fragmentation "
//                 "buffer.");
//       return MM_RESULT_CODE;)
//
//   VkDeviceSize new_offset = 0;
//   for (auto& buffer_chunk_info : buffer_chunks_info) {
//     buffer_chunk_info.SetOffset(new_offset);
//     new_offset += buffer_chunk_info.GetSize();
//   }
//
//   return ExecuteResult::SUCCESS;
// }

const VkPhysicalDeviceFeatures&
MM::RenderSystem::RenderEngine::GetPhysicalDeviceFeatures() const {
  return gpu_features_;
}

const VkPhysicalDeviceProperties&
MM::RenderSystem::RenderEngine::GetPhysicalDeviceProperties() const {
  return gpu_properties_;
}

uint32_t MM::RenderSystem::RenderEngine::GetCurrentFrame() const {
  return rendered_frame_count_ % flight_frame_number_;
}

uint32_t MM::RenderSystem::RenderEngine::GetFlightFrameNumber() const {
  return flight_frame_number_;
}

VkSampleCountFlagBits MM::RenderSystem::RenderEngine::GetMultiSampleCount()
    const {
  return render_engine_info_.multi_sample_count_;
}

bool MM::RenderSystem::RenderEngine::SupportMultiDrawIndirect() const {
  return gpu_features_.multiDrawIndirect;
}

void MM::RenderSystem::RenderEngine::ChooseMultiSampleCount() {
  uint32_t count{0};
  auto max_sample_count = static_cast<VkSampleCountFlagBits>(
      gpu_properties_.limits.framebufferColorSampleCounts &
      gpu_properties_.limits.framebufferDepthSampleCounts);
  if (max_sample_count & VK_SAMPLE_COUNT_64_BIT) {
    max_sample_count = VK_SAMPLE_COUNT_64_BIT;
  } else if (max_sample_count & VK_SAMPLE_COUNT_32_BIT) {
    max_sample_count = VK_SAMPLE_COUNT_32_BIT;
  } else if (max_sample_count & VK_SAMPLE_COUNT_16_BIT) {
    max_sample_count = VK_SAMPLE_COUNT_16_BIT;
  } else if (max_sample_count & VK_SAMPLE_COUNT_8_BIT) {
    max_sample_count = VK_SAMPLE_COUNT_8_BIT;
  } else if (max_sample_count & VK_SAMPLE_COUNT_4_BIT) {
    max_sample_count = VK_SAMPLE_COUNT_4_BIT;
  } else if (max_sample_count & VK_SAMPLE_COUNT_2_BIT) {
    max_sample_count = VK_SAMPLE_COUNT_2_BIT;
  } else {
    render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_1_BIT;
    return;
  }
  MM_CHECK(MM_CONFIG_SYSTEM->GetConfig("multi_sample_count", count),
           MM_LOG_FATAL("The setting of \"multi_sample_count\" is not exist.");
           return;)

  if (count < 2) {
    render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_1_BIT;
    return;
  }
  if (count < 4) {
    if (VK_SAMPLE_COUNT_2_BIT > max_sample_count) {
      render_engine_info_.multi_sample_count_ = max_sample_count;
      return;
    }
    render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_2_BIT;
    return;
  }
  if (count < 8) {
    if (VK_SAMPLE_COUNT_4_BIT > max_sample_count) {
      render_engine_info_.multi_sample_count_ = max_sample_count;
      return;
    }
    render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_4_BIT;
    return;
  }
  if (count < 16) {
    if (VK_SAMPLE_COUNT_8_BIT > max_sample_count) {
      render_engine_info_.multi_sample_count_ = max_sample_count;
      return;
    }
    render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_8_BIT;
    return;
  }
  if (count < 32) {
    if (VK_SAMPLE_COUNT_16_BIT > max_sample_count) {
      render_engine_info_.multi_sample_count_ = max_sample_count;
      return;
    }
    render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_16_BIT;
    return;
  }
  if (count < 64) {
    if (VK_SAMPLE_COUNT_32_BIT > max_sample_count) {
      render_engine_info_.multi_sample_count_ = max_sample_count;
      return;
    }
    render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_32_BIT;
    return;
  }
  if (VK_SAMPLE_COUNT_64_BIT > max_sample_count) {
    render_engine_info_.multi_sample_count_ = max_sample_count;
    return;
  }
  render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_64_BIT;
}

void MM::RenderSystem::RenderEngine::InitInstance() {
  uint32_t version_major = 0;
  uint32_t version_minor = 0;
  uint32_t version_patch = 0;
  MM_CHECK(MM_CONFIG_SYSTEM->GetConfig("version_major", version_major) |
               MM_CONFIG_SYSTEM->GetConfig("version_minor", version_minor) |
               MM_CONFIG_SYSTEM->GetConfig("version_patch", version_patch),
           MM_LOG_FATAL("The software version number is not set.");
           return;)
  if (enable_validation_layers_) {
    if (CheckValidationLayerSupport()) {
      enable_layer_.push_back(validation_layers_name.c_str());
    } else {
      MM_LOG_ERROR("Validation layer extension not found!");
    }
  }
  GetRequireExtensions(enable_instance_extensions_);

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "MiniMirror RenderSystem System";
  app_info.pEngineName = "MiniMirror";
  app_info.apiVersion = VK_API_VERSION_1_3;
  app_info.engineVersion =
      VK_MAKE_VERSION(version_major, version_minor, version_patch);
  app_info.applicationVersion = app_info.engineVersion;

  VkInstanceCreateInfo instance_create_info{};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pApplicationInfo = &app_info;
  instance_create_info.enabledExtensionCount =
      static_cast<uint32_t>(enable_instance_extensions_.size());
  instance_create_info.ppEnabledExtensionNames =
      enable_instance_extensions_.data();
  instance_create_info.enabledLayerCount =
      static_cast<uint32_t>(enable_layer_.size());
  instance_create_info.ppEnabledLayerNames = enable_layer_.data();
  instance_create_info.flags |=
      VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
  if (enable_validation_layers_) {
    PopulateDebugMessengerCreateInfo(debug_messenger_create_info);
    instance_create_info.pNext = &debug_messenger_create_info;
  }

  if (vkCreateInstance(&instance_create_info, nullptr, &instance_) !=
      VK_SUCCESS) {
    MM_LOG_FATAL("Failed to create VkInstance!");
  }
}

void MM::RenderSystem::RenderEngine::InitSurface() {
  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) !=
      VK_SUCCESS) {
    MM_LOG_FATAL("failed to create window surface!");
  }
}

void MM::RenderSystem::RenderEngine::InitPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

  if (deviceCount == 0) {
    MM_LOG_FATAL("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

  // The score of the physical device with the highest fitness.
  int score = 0;
  for (const auto& device : devices) {
    const int temp_score = RateDeviceSuitability(device);
    if (temp_score > score) {
      physical_device_ = device;
      score = temp_score;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    MM_LOG_FATAL("failed to find a suitable GPU!");
  }
}

void MM::RenderSystem::RenderEngine::InitGpuProperties() {
  vkGetPhysicalDeviceProperties(physical_device_, &gpu_properties_);
}

void MM::RenderSystem::RenderEngine::InitGpuFeatures() {
  vkGetPhysicalDeviceFeatures(physical_device_, &gpu_features_);
}

void MM::RenderSystem::RenderEngine::InitLogicalDevice() {
  queue_family_indices_ = FindQueueFamily(physical_device_);

  const float queue_priority = 1.0f;
  std::vector<VkDeviceQueueCreateInfo>
      queue_create_infos;  // all queues that need to be created
  const std::set<uint32_t> queue_families{
      queue_family_indices_.graphics_family_.value(),
      queue_family_indices_.compute_family_.value(),
      queue_family_indices_.present_family_.value(),
      queue_family_indices_.transform_family_.value()};
  for (const uint32_t queue_family : queue_families)  // for every queue family
  {
    // queue create info
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // physical device features
  VkPhysicalDeviceFeatures2 physical_device_features{};
  physical_device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  physical_device_features.features.samplerAnisotropy = VK_TRUE;
  // support inefficient readback storage buffer
  physical_device_features.features.fragmentStoresAndAtomics = VK_TRUE;
  // support independent blending
  physical_device_features.features.independentBlend = VK_TRUE;
  // support geometry shader
  if (enable_point_light_shadow_) {
    physical_device_features.features.geometryShader = VK_TRUE;
  }
  // support shader 64bit integer
  physical_device_features.features.shaderInt64 = VK_TRUE;

  VkPhysicalDeviceVulkan12Features device_vulkan12_features{};
  device_vulkan12_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  device_vulkan12_features.runtimeDescriptorArray = VK_TRUE;
  device_vulkan12_features.descriptorIndexing = VK_TRUE;
  device_vulkan12_features.descriptorBindingPartiallyBound = VK_TRUE;
  device_vulkan12_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
  device_vulkan12_features.descriptorBindingSampledImageUpdateAfterBind =
      VK_TRUE;
  device_vulkan12_features.shaderStorageImageArrayNonUniformIndexing = VK_TRUE;
  device_vulkan12_features.descriptorBindingStorageImageUpdateAfterBind =
      VK_TRUE;
  device_vulkan12_features.bufferDeviceAddress = VK_TRUE;
  // Enable partially bound descriptor bindings
  device_vulkan12_features.descriptorBindingPartiallyBound = true;
  // Enable non-uniform indexing and update after bind
  // binding flags for textures, uniforms, and buffers
  device_vulkan12_features.shaderSampledImageArrayNonUniformIndexing = true;
  device_vulkan12_features.descriptorBindingSampledImageUpdateAfterBind = true;
  device_vulkan12_features.shaderUniformBufferArrayNonUniformIndexing = true;
  device_vulkan12_features.descriptorBindingUniformBufferUpdateAfterBind = true;
  device_vulkan12_features.shaderStorageBufferArrayNonUniformIndexing = true;
  device_vulkan12_features.descriptorBindingStorageBufferUpdateAfterBind = true;

  physical_device_features.pNext = &device_vulkan12_features;

  // device create info
  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = &physical_device_features;
  device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.queueCreateInfoCount =
      static_cast<uint32_t>(queue_create_infos.size());
  // device_create_info.pEnabledFeatures;
  device_create_info.enabledExtensionCount =
      static_cast<uint32_t>(enable_device_extensions_.size());
  device_create_info.ppEnabledExtensionNames = enable_device_extensions_.data();
  device_create_info.enabledLayerCount =
      static_cast<uint32_t>(enable_layer_.size());
  device_create_info.ppEnabledLayerNames = enable_layer_.data();
  device_create_info.pNext = &device_vulkan12_features;

  if (vkCreateDevice(physical_device_, &device_create_info, nullptr,
                     &device_) != VK_SUCCESS) {
    MM_LOG_FATAL("failed to create logical device!");
  }

  vkGetDeviceQueue(device_, queue_family_indices_.graphics_family_.value(), 0,
                   &graphics_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.compute_family_.value(), 0,
                   &compute_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.present_family_.value(), 0,
                   &present_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.transform_family_.value(), 0,
                   &transform_queue_);
}

void MM::RenderSystem::RenderEngine::InitAllocator() {
  VmaVulkanFunctions vulkanFunctions = {};
  vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
  vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo allocatorCreateInfo = {};
  allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
  allocatorCreateInfo.physicalDevice = physical_device_;
  allocatorCreateInfo.device = device_;
  allocatorCreateInfo.instance = instance_;
  allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

  vmaCreateAllocator(&allocatorCreateInfo, &allocator_);
}

void MM::RenderSystem::RenderEngine::InitSwapChain() {
  const SwapChainSupportDetails swap_chain_detail =
      QuerySwapChainSupport(physical_device_);

  VkExtent2D swap_chain_extent =
      ChooseSwapExtent(swap_chain_detail.capabilities_);
  VkSurfaceFormatKHR swap_chain_format =
      ChooseSwapSurfaceFormat(swap_chain_detail.formats_);
  VkPresentModeKHR swap_chain_present_mode =
      ChooseSwapPresentMode(swap_chain_detail.presentModes_);

  uint32_t image_count = swap_chain_detail.capabilities_.minImageCount + 1;
  if (swap_chain_detail.capabilities_.maxImageCount > 0 &&
      image_count > swap_chain_detail.capabilities_.maxImageCount) {
    image_count = swap_chain_detail.capabilities_.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swap_chain_create_info{};
  swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_create_info.surface = surface_;
  swap_chain_create_info.minImageCount = image_count;
  swap_chain_create_info.imageFormat = swap_chain_format.format;
  swap_chain_create_info.imageColorSpace = swap_chain_format.colorSpace;
  swap_chain_create_info.imageExtent = swap_chain_extent;
  swap_chain_create_info.imageArrayLayers = 1;
  swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const uint32_t queue_family_indices[] = {
      queue_family_indices_.graphics_family_.value(),
      queue_family_indices_.present_family_.value()};

  if (queue_family_indices_.graphics_family_ !=
      queue_family_indices_.present_family_) {
    swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swap_chain_create_info.queueFamilyIndexCount = 2;
    swap_chain_create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  swap_chain_create_info.preTransform =
      swap_chain_detail.capabilities_.currentTransform;
  swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_chain_create_info.presentMode = swap_chain_present_mode;
  swap_chain_create_info.clipped = VK_TRUE;

  swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_, &swap_chain_create_info, nullptr,
                           &swapchain_) != VK_SUCCESS) {
    MM_LOG_FATAL("Failed to create swap chain!");
  }

  swapchain_format_ = swap_chain_format.format;
  swapchain_extent = swap_chain_extent;

  vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, nullptr);
  swapchain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swapchain_, &image_count,
                          swapchain_images_.data());
  scissor_ = {{0, 0}, {swapchain_extent.width, swapchain_extent.height}};
}

void MM::RenderSystem::RenderEngine::InitSwapChainImageView() {
  swapchain_image_views_.resize(swapchain_images_.size());

  for (size_t i = 0; i < swapchain_images_.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapchain_images_[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapchain_format_;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device_, &createInfo, nullptr,
                          &swapchain_image_views_[i]) != VK_SUCCESS) {
      MM_LOG_FATAL("Failed to create swapchain image views!");
    }
  }
}

void MM::RenderSystem::RenderEngine::InitCommandExecutor() {
  std::uint32_t single_frame_graph_command_buffer_number = 0;
  std::uint32_t single_frame_compute_command_buffer_number = 0;
  std::uint32_t single_frame_transform_command_buffer_number = 0;
  if (MM_CONFIG_SYSTEM->GetConfig("single_frame_graph_command_buffer_number",
                                  single_frame_graph_command_buffer_number) ==
          ExecuteResult::SUCCESS &&
      MM_CONFIG_SYSTEM->GetConfig("single_frame_compute_command_buffer_number",
                                  single_frame_compute_command_buffer_number) ==
          ExecuteResult::SUCCESS &&
      MM_CONFIG_SYSTEM->GetConfig(
          "single_frame_transform_command_buffer_number",
          single_frame_transform_command_buffer_number) ==
          ExecuteResult::SUCCESS) {
    command_executor_ = std::make_unique<CommandExecutor>(
        this, single_frame_graph_command_buffer_number,
        single_frame_compute_command_buffer_number,
        single_frame_transform_command_buffer_number);
  } else {
    command_executor_ = std::make_unique<CommandExecutor>(this);
  }
}

// void MM::RenderSystem::RenderEngine::InitCommandPool() {
//   // graph command pool
//   const VkCommandPoolCreateInfo graph_command_pool_create_info =
//       RenderSystem::Utils::GetCommandPoolCreateInfo(
//           queue_family_indices_.graphics_family_.value(),
//           VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
//   if (vkCreateCommandPool(device_, &graph_command_pool_create_info, nullptr,
//                           &graph_command_pool_) != VK_SUCCESS) {
//     LOG_FATAL("Failed to create graph command pool!");
//   }
//
//   // compute command pool
//   const VkCommandPoolCreateInfo compute_command_pool_create_info =
//       RenderSystem::Utils::GetCommandPoolCreateInfo(
//           queue_family_indices_.compute_family_.value(),
//           VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
//   if (vkCreateCommandPool(device_, &compute_command_pool_create_info,
//   nullptr, &compute_command_pool_) != VK_SUCCESS) {
//     LOG_FATAL("Failed to create compute pool!");
//   } }

// void MM::RenderSystem::RenderEngine::InitCommandBuffers() {
//   graph_command_buffers_.resize(flight_frame_number_);
//   compute_command_buffers_.resize(flight_frame_number_);
//
//   // graph command buffers
//   for (uint32_t i = 0; i < flight_frame_number_; ++i) {
//     VkCommandBufferAllocateInfo command_allocate_info =
//         RenderSystem::Utils::GetCommandBufferAllocateInfo(graph_command_pool_);
//     if (vkAllocateCommandBuffers(device_, &command_allocate_info,
//                                  &graph_command_buffers_[i]) != VK_SUCCESS) {
//       LOG_FATAL("Failed to allocate graph command buffer!");
//     }
//   }
//
//   // compute command buffers
//   for (uint32_t i = 0; i < flight_frame_number_; ++i) {
//     VkCommandBufferAllocateInfo command_allocate_info =
//         RenderSystem::Utils::GetCommandBufferAllocateInfo(compute_command_pool_);
//     if (vkAllocateCommandBuffers(device_, &command_allocate_info,
//                                  &compute_command_buffers_[i]) != VK_SUCCESS)
//                                  {
//       LOG_FATAL("Failed to allocate compute command buffer!");
//     }
//   }
// }

void MM::RenderSystem::RenderEngine::SetUpDebugMessenger() {
  if (!enable_validation_layers_) {
    return;
  }

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  PopulateDebugMessengerCreateInfo(createInfo);

  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT"));
  if (func) {
    if (func(instance_, &createInfo, nullptr, &debug_messenger_) !=
        VK_SUCCESS) {
      MM_LOG_FATAL("Failed to set up Debug Messenger!");
      return;
    }
    return;
  }
  MM_LOG_FATAL("Failed to set up Debug Messenger!");
}

void MM::RenderSystem::RenderEngine::InitGlfw() {
  // Read the initialization settings.
  MM_CHECK(
      MM_CONFIG_SYSTEM->GetConfig("window_extent_width", window_extent_.width),
      MM_LOG_FATAL("Setting item \"Window_extent_width.\" does not exist.");
      return;)
  MM_CHECK(
      MM_CONFIG_SYSTEM->GetConfig("window_extent_height",
                                  window_extent_.height),
      MM_LOG_FATAL("Setting iterm \"Window_extent_height.\" does not exist.");
      return;)

  // Initialize glfw.
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  window_ = glfwCreateWindow(static_cast<int>(window_extent_.width),
                             static_cast<int>(window_extent_.height),
                             "MinMirror", nullptr, nullptr);
}

void MM::RenderSystem::RenderEngine::InitVulkan() {
  InitInstance();
  SetUpDebugMessenger();
  InitSurface();
  InitPhysicalDevice();
  InitGpuProperties();
  InitLogicalDevice();
  InitSwapChain();
  InitCommandExecutor();
}

void MM::RenderSystem::RenderEngine::InitInfo() { ChooseMultiSampleCount(); }

std::vector<VkExtensionProperties>
MM::RenderSystem::RenderEngine::GetExtensionProperties() {
  uint32_t extension_count;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extensions.data());
  return extensions;
}

bool MM::RenderSystem::RenderEngine::CheckExtensionSupport(
    const std::string& extension_name) {
  const auto extension_properties = GetExtensionProperties();
  for (const auto& extension_property : extension_properties) {
    if (strcmp(extension_name.c_str(), extension_property.extensionName) != 0) {
      return true;
    }
  }
  return false;
}

bool MM::RenderSystem::RenderEngine::CheckDeviceExtensionSupport(
    const VkPhysicalDevice& physical_device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                       &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string> requiredExtensions(enable_device_extensions_.begin(),
                                           enable_device_extensions_.end());

  for (const auto& extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

bool MM::RenderSystem::RenderEngine::CheckValidationLayerSupport() {
  return CheckExtensionSupport("VK_LAYER_KHRONOS_validation");
}

void MM::RenderSystem::RenderEngine::GetGlfwRequireExtensions(
    std::vector<const char*>& extensions) const {
  uint32_t glfw_extensions_count = 0;
  const char** glfw_extensions = nullptr;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
  for (uint32_t i = 0; i < glfw_extensions_count; ++i) {
    std::size_t j = 0;
    for (; j < extensions.size(); ++j) {
      if (strcmp(extensions[j], *(glfw_extensions + i)) == 0) {
        break;
      }
    }
    if (j == extensions.size()) {
      extensions.push_back(*(glfw_extensions + i));
    }
  }
  if (enable_validation_layers_) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
}

void MM::RenderSystem::RenderEngine::GetRequireExtensions(
    std::vector<const char*>& extensions) {
  GetGlfwRequireExtensions(extensions);
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
}

void MM::RenderSystem::RenderEngine::PopulateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = ValidationLayerDebugCall;
}

void MM::RenderSystem::RenderEngine::DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* allocator) {
  const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    func(instance, debug_messenger, allocator);
  }
}

int MM::RenderSystem::RenderEngine::RateDeviceSuitability(
    const VkPhysicalDevice& physical_device) {
  auto queue_families = FindQueueFamily(physical_device);
  auto extension_support = CheckDeviceExtensionSupport(physical_device);
  bool swap_chain_adequate = false;
  if (extension_support) {
    auto swap_chain_detail = QuerySwapChainSupport(physical_device);
    swap_chain_adequate = !swap_chain_detail.formats_.empty() &&
                          !swap_chain_detail.presentModes_.empty();
  }
  if (!queue_families.isComplete() || !extension_support ||
      !swap_chain_adequate) {
    return 0;
  }

  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceProperties(physical_device, &device_properties);
  vkGetPhysicalDeviceFeatures(physical_device, &device_features);

  if (device_properties.limits.maxStorageBufferRange < 1073741820) {
    return 0;
  }

  int score = 0;

  // Discrete GPUs have a significant performance advantage
  if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  // Maximum possible size of textures affects graphics quality
  score += device_properties.limits.maxImageDimension2D;

  score += device_properties.limits.framebufferColorSampleCounts +
           device_properties.limits.framebufferDepthSampleCounts;

  if (device_properties.limits.maxUniformBufferRange != 0) {
    score += 20;
  }

  if (device_features.multiDrawIndirect) {
    score += 100;
  }

  // Application can't function without geometry shaders
  if (!device_features.geometryShader) {
    return 0;
  }

  return score;
}

MM::RenderSystem::QueueFamilyIndices
MM::RenderSystem::RenderEngine::FindQueueFamily(
    const VkPhysicalDevice& physical_device) const {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family_ = i;
    }

    if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      indices.compute_family_ = i;
    }

    if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      indices.compute_family_ = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface_,
                                         &presentSupport);

    if (presentSupport) {
      indices.present_family_ = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  if (!indices.transform_family_.has_value() &&
      indices.graphics_family_.has_value()) {
    indices.transform_family_ = indices.graphics_family_;
  }

  return indices;
}

MM::RenderSystem::SwapChainSupportDetails
MM::RenderSystem::RenderEngine::QuerySwapChainSupport(
    const VkPhysicalDevice& physical_device) const {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface_,
                                            &details.capabilities_);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface_, &formatCount,
                                       nullptr);

  if (formatCount != 0) {
    details.formats_.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface_,
                                         &formatCount, details.formats_.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface_,
                                            &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes_.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface_,
                                              &presentModeCount,
                                              details.presentModes_.data());
  }

  return details;
}

VkSurfaceFormatKHR MM::RenderSystem::RenderEngine::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
  for (const auto& available_format : available_formats) {
    if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available_format;
    }
  }

  return available_formats[0];
}

VkPresentModeKHR MM::RenderSystem::RenderEngine::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& available_present_modes) {
  for (const auto& available_present_mode : available_present_modes) {
    if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return available_present_mode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D MM::RenderSystem::RenderEngine::ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

VkBool32 MM::RenderSystem::ValidationLayerDebugCall(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
  if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    LogSystem::LogSystem::LogLevel log_level;
    switch (message_severity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        log_level = LogSystem::LogSystem::LogLevel::WARN;
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        log_level = LogSystem::LogSystem::LogLevel::ERROR;
        break;
      default:
        log_level = LogSystem::LogSystem::LogLevel::WARN;
    }
    MM_LOG(log_level,
           std::string("validation layer: ") + callback_data->pMessage);
  }
  return VK_FALSE;
}

MM::RenderSystem::RenderEngine::RenderEngine() { Init(); }

MM::ExecuteResult MM::RenderSystem::RenderEngine::CreateBuffer(
    const VkBufferCreateInfo& vk_buffer_create_info,
    const VmaAllocationCreateInfo& vma_allocation_create_info,
    VmaAllocationInfo* vma_allocation_info,
    MM::RenderSystem::AllocatedBuffer& allocated_buffer) {
  assert(IsValid());
  return Utils::CreateBuffer(this, vk_buffer_create_info,
                             vma_allocation_create_info, vma_allocation_info,
                             allocated_buffer);
}

MM::RenderSystem::RenderFuture MM::RenderSystem::RenderEngine::RunSingleCommand(
    CommandBufferType command_type, std::uint32_t use_render_resource_count,
    const std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>& commands,
    const std::vector<RenderResourceDataID>&
        cross_task_flow_sync_render_resource_data_ID) {
  assert(IsValid());
  CommandTaskFlow command_task_flow;
  command_task_flow
      .AddTask(command_type, commands, use_render_resource_count,
               std::vector<WaitAllocatedSemaphore>(),
               std::vector<AllocateSemaphore>())
      .AddCrossTaskFLowSyncRenderResourceIDs(
          cross_task_flow_sync_render_resource_data_ID);

  return command_executor_->Run(std::move(command_task_flow));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::RunSingleCommandAndWait(
    CommandBufferType command_type, std::uint32_t use_render_resource_count,
    const std::function<ExecuteResult(AllocatedCommandBuffer& cmd)>& commands,
    const std::vector<RenderResourceDataID>&
        cross_task_flow_sync_render_resource_data_ID) {
  assert(IsValid());
  CommandTaskFlow command_task_flow;
  command_task_flow
      .AddTask(command_type, commands, use_render_resource_count,
               std::vector<WaitAllocatedSemaphore>(),
               std::vector<AllocateSemaphore>())
      .AddCrossTaskFLowSyncRenderResourceIDs(
          cross_task_flow_sync_render_resource_data_ID);

  return command_executor_->RunAndWait(std::move(command_task_flow));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CreateStageBuffer(
    VkDeviceSize size, std::uint32_t queue_index,
    AllocatedBuffer& stage_buffer) {
  assert(IsValid());
  VkBufferCreateInfo stage_buffer_create_info = Utils::GetVkBufferCreateInfo(
      nullptr, 0, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE, 1, &queue_index);
  VmaAllocationCreateInfo stage_allocation_create_info =
      Utils::GetVmaAllocationCreateInfo(
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
          VMA_MEMORY_USAGE_AUTO, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0, nullptr, nullptr, 1);
  MM_CHECK_WITHOUT_LOG(
      CreateBuffer(stage_buffer_create_info, stage_allocation_create_info,
                   nullptr, stage_buffer),
      MM_LOG_ERROR("Failed to create stage buffer.");
      return MM_RESULT_CODE;)

  return MM::Utils::ExecuteResult ::SUCCESS;
}

MM::RenderSystem::CommandExecutorLockGuard
MM::RenderSystem::RenderEngine::GetCommandExecutorLockGuard() {
  assert(IsValid());
  return CommandExecutorLockGuard{*command_executor_};
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::GraphQueueWaitIdle() {
  return MM::RenderSystem::Utils::VkResultToMMResult(
      vkQueueWaitIdle(graphics_queue_));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::TransformQueueWaitIdle() {
  return MM::RenderSystem::Utils::VkResultToMMResult(
      vkQueueWaitIdle(transform_queue_));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::PresentQueueWaitIdle() {
  return MM::RenderSystem::Utils::VkResultToMMResult(
      vkQueueWaitIdle(present_queue_));
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::ComputeQueueWaitIdle() {
  return MM::RenderSystem::Utils::VkResultToMMResult(
      vkQueueWaitIdle(compute_queue_));
}

bool MM::RenderSystem::RenderEngine::CommandExecutorIsFree() const {
  return command_executor_->IsFree();
}

MM::ExecuteResult
MM::RenderSystem::RenderEngine::CopyBufferInputParametersCheck(
    MM::RenderSystem::AllocatedBuffer& src_buffer,
    MM::RenderSystem::AllocatedBuffer& dest_buffer,
    const std::vector<VkBufferCopy2>& regions) {
  if (!src_buffer.IsValid()) {
    MM_LOG_ERROR("Src_buffer is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  if (!dest_buffer.IsValid()) {
    MM_LOG_ERROR("Dest_buffer is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  if (!src_buffer.IsTransformSrc() || !dest_buffer.IsTransformDest()) {
    MM_LOG_ERROR(
        "Src_buffer or dest_buffer is not Transform src buffer or Transform "
        "dest buffer.");
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  if (regions.empty()) {
    return ExecuteResult::SUCCESS;
  }

  for (const auto& region : regions) {
    if (region.size == 0) {
      MM_LOG_ERROR("VkBufferCopy2::size must greater than 0.");
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }
  }

  if (src_buffer.GetBuffer() == dest_buffer.GetBuffer()) {
    std::list<BufferChunkInfo> write_areas{};
    // Check for any overlap and oversize.
    for (const auto& region : regions) {
      if (region.srcOffset + region.size > src_buffer.GetBufferSize() ||
          region.dstOffset + region.size > dest_buffer.GetBufferSize()) {
        MM_LOG_ERROR(
            "The sum of size and src_offset/dest_offset is too large.");
        return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
      }
      if (write_areas.empty()) {
        write_areas.emplace_back(region.dstOffset, region.size);
        continue;
      }
      bool have_greater_than = false;
      for (auto write_area = write_areas.begin();
           write_area != write_areas.end(); ++write_area) {
        if (region.dstOffset <= write_area->GetOffset()) {
          if (region.dstOffset + region.size >= write_area->GetOffset()) {
            MM_LOG_ERROR(
                "During the buffer copy process, there is an overlap in the "
                "specified copy area.");
            return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
          }
          write_areas.emplace(write_area, region.dstOffset, region.size);
          have_greater_than = true;
          break;
        }
      }
      if (!have_greater_than) {
        const auto last_area = --write_areas.end();
        if (last_area->GetOffset() + last_area->GetSize() >= region.dstOffset) {
          MM_LOG_ERROR(
              "During the buffer copy process, there is an overlap in the "
              "specified copy area.");
          return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
        }
        write_areas.emplace_back(region.dstOffset, region.size);
      }
    }
    for (const auto& region : regions) {
      for (const auto& write_area : write_areas) {
        if (region.srcOffset <= write_area.GetOffset()) {
          if (region.srcOffset + region.size >= write_area.GetOffset()) {
            MM_LOG_ERROR(
                "During the buffer copy process, there is an overlap in the "
                "specified copy area.");
            return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
          }
          break;
        }
      }
    }
  } else {
    for (const auto& region : regions) {
      if (region.srcOffset + region.size > src_buffer.GetBufferSize() ||
          region.dstOffset + region.size > dest_buffer.GetBufferSize()) {
        MM_LOG_ERROR(
            "The sum of size and src_offset/dest_offset is too large.");
        return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
      }
    }
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderEngine::CopyImageInputParametersCheck(
    MM::RenderSystem::AllocatedImage& src_image,
    MM::RenderSystem::AllocatedImage& dest_image,
    const std::vector<VkImageCopy2>& regions) {
  if (!src_image.IsValid()) {
    MM_LOG_ERROR("Src_image is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  if (!dest_image.IsValid()) {
    MM_LOG_ERROR("Dest_image is invalid.");
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  if (!src_image.IsTransformSrc() || !dest_image.IsTransformDest()) {
    MM_LOG_ERROR(
        "Src_buffer or dest_buffer is not Transform src buffer or Transform "
        "dest buffer.");
    return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  if (regions.empty()) {
    return ExecuteResult::SUCCESS;
  }

  std::unordered_map<std::uint64_t, std::vector<ImageChunkInfo>>
      image_write_areas;
  for (const auto& region : regions) {
    if (region.extent.width == 0 || region.extent.height == 0 ||
        region.extent.depth == 0) {
      MM_LOG_ERROR(
          "VkImageCopy::extent::width/height/depth must greater than 0.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    if (region.dstSubresource.aspectMask != region.srcSubresource.aspectMask) {
      MM_LOG_ERROR("Src_image aspect mask not equal dest_image aspect mask.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    if (region.srcSubresource.aspectMask & VK_IMAGE_ASPECT_METADATA_BIT ||
        region.dstSubresource.aspectMask & VK_IMAGE_ASPECT_METADATA_BIT) {
      MM_LOG_ERROR(
          "Src_image or dest_image include "
          "VK_IMAGE_ASPECT_METADATA_BIT.aspectMask must not contain "
          "VK_IMAGE_ASPECT_METADATA_BIT.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    if (region.srcSubresource.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT &&
        (region.srcSubresource.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT ||
         region.srcSubresource.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)) {
      MM_LOG_ERROR(
          "If aspectMask contains VK_IMAGE_ASPECT_COLOR_BIT, it must not "
          "contain either of VK_IMAGE_ASPECT_DEPTH_BIT or "
          "VK_IMAGE_ASPECT_STENCIL_BIT.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    if (region.dstSubresource.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT &&
        (region.dstSubresource.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT ||
         region.dstSubresource.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)) {
      MM_LOG_ERROR(
          "If aspectMask contains VK_IMAGE_ASPECT_COLOR_BIT, it must not "
          "contain either of VK_IMAGE_ASPECT_DEPTH_BIT or "
          "VK_IMAGE_ASPECT_STENCIL_BIT.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    if (region.srcSubresource.mipLevel > src_image.GetMipmapLevels() ||
        region.dstSubresource.mipLevel > dest_image.GetMipmapLevels()) {
      MM_LOG_ERROR(
          "The specified number of mipmap level is greater than the number of "
          "mipmap level contained in the image itself.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }
    // layer_count must == 1
    //    if (region.srcSubresource.layerCount !=
    //    region.dstSubresource.layerCount) {
    //      LOG_ERROR(
    //          "The array layer number of src_image and dest_image is
    //          different.");
    //      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    //    }
    //    if (region.srcSubresource.baseArrayLayer +
    //                region.srcSubresource.layerCount >
    //            src_image.GetImageLayout() ||
    //        region.dstSubresource.baseArrayLayer +
    //                region.dstSubresource.layerCount >
    //            dest_image.GetImageLayout()) {
    //      LOG_ERROR(
    //          "The specified number of array level is greater than the number
    //          of " "array level contained in the image itself.");
    //      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    //    }
    if (region.srcSubresource.layerCount != 1 ||
        region.srcSubresource.baseArrayLayer != 0 ||
        region.dstSubresource.layerCount != 1 ||
        region.dstSubresource.baseArrayLayer != 0) {
      MM_LOG_ERROR("The layerCount must 1 and baseArrayLayer must 0.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    if (!Utils::ImageRegionAreaLessThanImageExtent(region.srcOffset,
                                                   region.extent, src_image) ||
        !Utils::ImageRegionAreaLessThanImageExtent(region.dstOffset,
                                                   region.extent, dest_image)) {
      MM_LOG_ERROR(
          "The specified area is lager than src_image/dest_image extent.");
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    if (src_image.GetImage() == dest_image.GetImage()) {
      image_write_areas[region.dstSubresource.mipLevel].emplace_back(
          region.dstOffset, region.extent);
    }
  }
  if (src_image.GetImage() == dest_image.GetImage()) {
    for (const auto& region : regions) {
      for (const auto& image_write_area :
           image_write_areas[region.srcSubresource.mipLevel]) {
        if (Utils::ImageRegionIsOverlap(region.srcOffset,
                                        image_write_area.GetOffset(),
                                        image_write_area.GetExtent())) {
          MM_LOG_ERROR(
              "During the buffer copy process, there is an overlap in the "
              "specified copy area.");
          return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
        }
      }
    }
  }

  return ExecuteResult ::SUCCESS;
}

MM::RenderSystem::CommandExecutorGeneralCommandBufferGuard
MM::RenderSystem::RenderEngine::GetGeneralCommandBufferGuard(
    MM::RenderSystem::CommandBufferType command_buffer_type) {
  assert(IsValid());
  return CommandExecutorGeneralCommandBufferGuard(*command_executor_,
                                                  command_buffer_type);
}
