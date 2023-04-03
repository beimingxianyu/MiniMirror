#include "runtime/function/render/vk_engine.h"

#include <set>

const std::string MM::RenderSystem::RenderEngine::validation_layers_name{
    "VK_LAYER_KHRONOS_validation"};

void MM::RenderSystem::RenderEngine::Init() {
  InitGlfw();
  InitVulkan();
  

  is_initialized_ = true;
}

void MM::RenderSystem::RenderEngine::Run() {
  while (!glfwWindowShouldClose(window_)) {
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

MM::RenderSystem::AllocatedBuffer MM::RenderSystem::RenderEngine::CreateBuffer(
    const size_t& alloc_size, const VkBufferUsageFlags& usage,
    const VmaMemoryUsage& memory_usage,
    const VmaAllocationCreateFlags& allocation_flags) const {
  return Utils::CreateBuffer(this, alloc_size, usage, memory_usage, allocation_flags);
}

const VmaAllocator& MM::RenderSystem::RenderEngine::GetAllocator() const {
  return allocator_;
}

const VkDevice& MM::RenderSystem::RenderEngine::GetDevice() const {
  return device_;
}

bool MM::RenderSystem::RenderEngine::RecordAndSubmitCommand(
    const CommandBufferType& command_buffer_type,
    const std::function<void(VkCommandBuffer& cmd)>& function,
    const bool& auto_start_end_submit, const bool& record_new_command,
    const std::shared_ptr<VkSubmitInfo>& submit_info_ptr) {
  if (!IsValid()) {
    return false;
  }
  if (command_buffer_type == CommandBufferType::GRAPH) {
    return graph_command_executors_[GetCurrentFrame()].RecordAndSubmitCommand(
        function, auto_start_end_submit, record_new_command, submit_info_ptr);
  }
  if (command_buffer_type == CommandBufferType::COMPUTE) {
    return compute_command_executors_[GetCurrentFrame()].RecordAndSubmitCommand(
        function, auto_start_end_submit, record_new_command, submit_info_ptr);
  }
  return false;
}

bool MM::RenderSystem::RenderEngine::RecordAndSubmitCommand(
    const CommandBufferType& command_buffer_type,
    const std::function<bool(VkCommandBuffer& cmd)>& function,
    const bool& auto_start_end_submit, const bool& record_new_command,
    const std::shared_ptr<VkSubmitInfo>& submit_info_ptr) {
  if (!IsValid()) {
    return false;
  }
  if (command_buffer_type == CommandBufferType::GRAPH) {
    return graph_command_executors_[GetCurrentFrame()].RecordAndSubmitCommand(
        function, auto_start_end_submit, record_new_command, submit_info_ptr);
  }
  if (command_buffer_type == CommandBufferType::COMPUTE) {
    return compute_command_executors_[GetCurrentFrame()].RecordAndSubmitCommand(
        function, auto_start_end_submit, record_new_command, submit_info_ptr);
  }
  return false;
}

bool MM::RenderSystem::RenderEngine::RecordAndSubmitSingleTimeCommand(
    const CommandBufferType& command_buffer_type,
    const std::function<void(VkCommandBuffer& cmd)>& function,
    const bool& auto_start_end_submit_wait) {
  if (!IsValid()) {
    return false;
  }
  AllocatedCommandBuffer command_executor =
      command_buffer_type == CommandBufferType::GRAPH
          ? graph_command_executors_[GetCurrentFrame()]
          : compute_command_executors_[GetCurrentFrame()];
  const VkCommandBufferAllocateInfo allocInfo =
      Utils::GetCommandBufferAllocateInfo(command_executor.GetCommandPool(), 1);

  VkCommandBuffer command_buffer;
  VK_CHECK(vkAllocateCommandBuffers(device_, &allocInfo, &command_buffer),
           LOG_ERROR("Failed to create single Time command buffer.");
           return false;)

  if (auto_start_end_submit_wait) {
    VkCommandBufferBeginInfo beginInfo = Utils::GetCommandBufferBeginInfo(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(command_buffer, &beginInfo),
             LOG_ERROR("Failed to begin single time command buffer.");
             vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                                  &command_buffer);
             return false;)
  }

  function(command_buffer);

  if (auto_start_end_submit_wait) {
    VK_CHECK(vkEndCommandBuffer(command_buffer),
             LOG_ERROR("Failed to end single time command buffer.");
             vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                                  &command_buffer);
             return false;)

    const VkSubmitInfo submit_info =
        Utils::GetCommandSubmitInfo(command_buffer);

    const VkFenceCreateInfo fence_create_info = Utils::GetFenceCreateInfo();
    VkFence temp_fence{nullptr};
    VK_CHECK(vkCreateFence(device_, &fence_create_info, nullptr, &temp_fence),
             LOG_ERROR("Failed to create VkFence.");
             vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                                  &command_buffer);
             return false;)

    VK_CHECK(
        vkWaitForFences(device_, 1, &temp_fence, VK_FALSE, 99999999999),
        LOG_FATAL(
            "The wait time for VkFence timed out.An error is expected in the "
            "program, and the render system will be restarted.(single "
            "buffer)");)

    VK_CHECK(
        vkQueueSubmit(command_executor.GetQueue(), 1, &submit_info, temp_fence),
        LOG_ERROR("Failed to submit single time command buffer.");
        vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                             &command_buffer);
        return false;)
  }

  vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                       &command_buffer);

  return true;
}

bool MM::RenderSystem::RenderEngine::RecordAndSubmitSingleTimeCommand(
    const CommandBufferType& command_buffer_type,
    const std::function<bool(VkCommandBuffer& cmd)>& function,
    const bool& auto_start_end_submit_wait) {
  if (!IsValid()) {
    return false;
  }
  AllocatedCommandBuffer command_executor =
      command_buffer_type == CommandBufferType::GRAPH
          ? graph_command_executors_[GetCurrentFrame()]
          : compute_command_executors_[GetCurrentFrame()];
  const VkCommandBufferAllocateInfo allocInfo =
      Utils::GetCommandBufferAllocateInfo(command_executor.GetCommandPool(), 1);

  VkCommandBuffer command_buffer;
  VK_CHECK(vkAllocateCommandBuffers(device_, &allocInfo, &command_buffer),
           LOG_ERROR("Failed to create single Time command buffer.");
           return false;)

  if (auto_start_end_submit_wait) {
    VkCommandBufferBeginInfo beginInfo = Utils::GetCommandBufferBeginInfo(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(command_buffer, &beginInfo),
             LOG_ERROR("Failed to begin single time command buffer.");
             vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                                  &command_buffer);
             return false;)
  }

  if (!function(command_buffer)) {
    LOG_ERROR(
        "Failed to execute function that input to "
        "RecordAndSubmitSingleTimeCommand.");
    return false;
  }

  if (auto_start_end_submit_wait) {
    VK_CHECK(vkEndCommandBuffer(command_buffer),
             LOG_ERROR("Failed to end single time command buffer.");
             vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                                  &command_buffer);
             return false;)

    const VkSubmitInfo submit_info =
        Utils::GetCommandSubmitInfo(command_buffer);

    const VkFenceCreateInfo fence_create_info = Utils::GetFenceCreateInfo();
    VkFence temp_fence{nullptr};
    VK_CHECK(vkCreateFence(device_, &fence_create_info, nullptr, &temp_fence),
             LOG_ERROR("Failed to create VkFence.");
             vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                                  &command_buffer);
             return false;)

    VK_CHECK(
        vkWaitForFences(device_, 1, &temp_fence, VK_FALSE, 99999999999),
        LOG_FATAL(
            "The wait time for VkFence timed out.An error is expected in the "
            "program, and the render system will be restarted.(single "
            "buffer)");)

    VK_CHECK(
        vkQueueSubmit(command_executor.GetQueue(), 1, &submit_info, temp_fence),
        LOG_ERROR("Failed to submit single time command buffer.");
        vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                             &command_buffer);
        return false;)
  }

  vkFreeCommandBuffers(device_, command_executor.GetCommandPool(), 1,
                       &command_buffer);

  return true;
}

bool MM::RenderSystem::RenderEngine::CopyBuffer(AllocatedBuffer& src_buffer,
                                                AllocatedBuffer& dest_buffer, const VkDeviceSize& src_offset,
                                                const VkDeviceSize& dest_offset, const VkDeviceSize& size) {
  if (size == 0) {
    return true;
  }
  if (dest_buffer.GetBuffer() == src_buffer.GetBuffer() && dest_offset - src_offset < size) {
    const auto stage_buffer = CreateBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);

    const auto src_to_stage_buffer_copy_region =
        Utils::GetCopyBufferRegion(size, src_offset, 0);
    const auto src_to_stage_buffer_copy_info = Utils::GetCopyBufferInfo(
        src_buffer, stage_buffer,
        std::vector<VkBufferCopy2>{src_to_stage_buffer_copy_region});

    if (!RecordAndSubmitSingleTimeCommand(CommandBufferType::GRAPH, [&copy_info = src_to_stage_buffer_copy_info](VkCommandBuffer& cmd) {
              vkCmdCopyBuffer2(cmd, &copy_info);
    }, true)) {
      LOG_ERROR("Failed to copy data form src_buffer to dest_buffer.")
      return false;
    }

    return true;
  }

  const auto src_to_stage_buffer_copy_region =
      Utils::GetCopyBufferRegion(size, src_offset, dest_offset);
  const auto src_to_stage_buffer_copy_info = Utils::GetCopyBufferInfo(
      src_buffer, dest_buffer,
      std::vector<VkBufferCopy2>{src_to_stage_buffer_copy_region});

  if (!RecordAndSubmitSingleTimeCommand(
          CommandBufferType::GRAPH,
          [&copy_info = src_to_stage_buffer_copy_info](VkCommandBuffer& cmd) {
            vkCmdCopyBuffer2(cmd, &copy_info);
          },
          true)) {
    LOG_ERROR("Failed to copy data form src_buffer to dest_buffer.")
    return false;
  }

  return true;
}

bool MM::RenderSystem::RenderEngine::CopyBuffer(AllocatedBuffer& src_buffer,
    AllocatedBuffer& dest_buffer, const std::vector<VkDeviceSize>& src_offsets,
    const std::vector<VkDeviceSize>& dest_offsets,
    const std::vector<VkDeviceSize>& sizes) {
  if (src_offsets.size() != dest_offsets.size() || dest_offsets.size() != sizes.size()) {
    LOG_ERROR(
        "The size of src_offsets,dest_offsets and sizes vector is not equal.")
    return false;
  }
  const std::size_t all_element = src_offsets.size();
  for (std::size_t i = 0; i < all_element; ++i) {
    if (!CopyBuffer(src_buffer, dest_buffer, src_offsets[i], dest_offsets[i],
                   sizes[i])) {
      LOG_ERROR(std::string("Copying the ") + std::to_string(i) +
                " buffer failed.")
      return false;
    }
  }

  return true;
}

const VkPhysicalDeviceFeatures& MM::RenderSystem::RenderEngine::
GetPhysicalDeviceFeatures() const {
  return gpu_features_;
}

const VkPhysicalDeviceProperties& MM::RenderSystem::RenderEngine::
GetPhysicalDeviceProperties() const {
  return gpu_properties_;
}

uint32_t MM::RenderSystem::RenderEngine::GetCurrentFrame() const {
  return rendered_frame_count_ % flight_frame_number_;
}

VkSampleCountFlagBits MM::RenderSystem::RenderEngine::GetMultiSampleCount() const {
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
  if (config_system->GetConfig("multi_sample_count", count)) {
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
    return;
  }
  render_engine_info_.multi_sample_count_ = VK_SAMPLE_COUNT_1_BIT;
}

void MM::RenderSystem::RenderEngine::InitInstance() {
  uint32_t version_major = 0;
  uint32_t version_minor = 0;
  uint32_t version_patch = 0;
  if (!(config_system->GetConfig("version_major", version_major) &&
        config_system->GetConfig("version_minor", version_minor) &&
        config_system->GetConfig("version_patch", version_patch))) {
    version_major = 0;
    version_minor = 0;
    version_patch = 0;
    LOG_WARN(
        "Failed to get the engine version, set to the default version of "
        "0.0.0.")
  }
  if (enable_validation_layers_) {
    if (CheckValidationLayerSupport()) {
      enable_layer_.push_back(validation_layers_name.c_str());
    } else {
      LOG_ERROR("Validation layer extension not found!")
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
    instance_create_info.pNext =
        (VkDebugUtilsMessengerCreateInfoEXT*)&debug_messenger_create_info;
  }

  if (vkCreateInstance(&instance_create_info, nullptr, &instance_) !=
      VK_SUCCESS) {
    LOG_FATAL("Failed to create VkInstance!");
  }
}

void MM::RenderSystem::RenderEngine::InitSurface() {
  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) !=
      VK_SUCCESS) {
    LOG_FATAL("failed to create window surface!");
  }
}

void MM::RenderSystem::RenderEngine::InitPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

  if (deviceCount == 0) {
    LOG_FATAL("failed to find GPUs with Vulkan support!");
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
    LOG_FATAL("failed to find a suitable GPU!");
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
      queue_family_indices_.present_family_.value()};
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
  VkPhysicalDeviceFeatures physical_device_features = {};

  physical_device_features.samplerAnisotropy = VK_TRUE;

  // support inefficient readback storage buffer
  physical_device_features.fragmentStoresAndAtomics = VK_TRUE;

  // support independent blending
  physical_device_features.independentBlend = VK_TRUE;

  // support geometry shader
  if (enable_point_light_shadow_) {
    physical_device_features.geometryShader = VK_TRUE;
  }

  // device create info
  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.queueCreateInfoCount =
      static_cast<uint32_t>(queue_create_infos.size());
  device_create_info.pEnabledFeatures = &physical_device_features;
  device_create_info.enabledExtensionCount =
      static_cast<uint32_t>(enable_device_extensions_.size());
  device_create_info.ppEnabledExtensionNames = enable_device_extensions_.data();
  device_create_info.enabledLayerCount =
      static_cast<uint32_t>(enable_layer_.size());
  device_create_info.ppEnabledLayerNames = enable_layer_.data();

  if (vkCreateDevice(physical_device_, &device_create_info, nullptr,
                     &device_) != VK_SUCCESS) {
    LOG_FATAL("failed to create logical device!");
  }

  vkGetDeviceQueue(device_, queue_family_indices_.graphics_family_.value(), 0,
                   &graphics_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.compute_family_.value(), 0,
                   &compute_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.present_family_.value(), 0,
                   &present_queue_);
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
    LOG_FATAL("Failed to create swap chain!");
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
      LOG_FATAL("Failed to create swapchain image views!");
    }
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

void MM::RenderSystem::RenderEngine::InitCommandExecutors() {
  VkCommandPool graph_command_pool{nullptr};
  VkCommandPool compute_command_pool{nullptr};
  // graph command pool
  const VkCommandPoolCreateInfo graph_command_pool_create_info =
      RenderSystem::Utils::GetCommandPoolCreateInfo(
          queue_family_indices_.graphics_family_.value(),
          VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  VK_CHECK(vkCreateCommandPool(device_, &graph_command_pool_create_info,
                               nullptr, &graph_command_pool),
           LOG_FATAL("Failed to create graph command pool!"))

  // compute command pool
  const VkCommandPoolCreateInfo compute_command_pool_create_info =
      RenderSystem::Utils::GetCommandPoolCreateInfo(
          queue_family_indices_.compute_family_.value(),
          VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  VK_CHECK(vkCreateCommandPool(device_, &compute_command_pool_create_info,
                               nullptr, &compute_command_pool),
           LOG_FATAL("Failed to create compute pool!"))

  graph_command_executors_.resize(flight_frame_number_);
  compute_command_executors_.resize(flight_frame_number_);

  // graph command buffers
  for (uint32_t i = 0; i < flight_frame_number_; ++i) {
    VkCommandBuffer temp_buffer{nullptr};

    VkCommandBufferAllocateInfo command_allocate_info =
        RenderSystem::Utils::GetCommandBufferAllocateInfo(graph_command_pool);
    VK_CHECK(
        vkAllocateCommandBuffers(device_, &command_allocate_info, &temp_buffer),
        LOG_FATAL("Failed to allocate graph command buffer!"))
    graph_command_executors_[i] = AllocatedCommandBuffer(
        this, graphics_queue_, graph_command_pool, temp_buffer);
  }

  // compute command buffers
  for (uint32_t i = 0; i < flight_frame_number_; ++i) {
    VkCommandBuffer temp_buffer{nullptr};

    VkCommandBufferAllocateInfo command_allocate_info =
        RenderSystem::Utils::GetCommandBufferAllocateInfo(compute_command_pool);

    VK_CHECK(
        vkAllocateCommandBuffers(device_, &command_allocate_info, &temp_buffer),
        LOG_FATAL("Failed to allocate compute command buffer!"))
    compute_command_executors_[i] = AllocatedCommandBuffer(
        this, compute_queue_, compute_command_pool, temp_buffer);
  }
}


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
      LOG_FATAL("Failed to set up Debug Messenger!");
      return;
    }
    return;
  }
  LOG_FATAL("Failed to set up Debug Messenger!");
}

void MM::RenderSystem::RenderEngine::InitGlfw() {
  // Read the initialization settings.
  if (!config_system->GetConfig("window_extent_width", window_extent_.width)) {
    LOG_WARN("\"Window_extent_width.\" is not set.");
  }
  if (!config_system->GetConfig("window_extent_height",
                                window_extent_.height)) {
    LOG_WARN("\"Window_extent_height.\" is not set.");
  }

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
  InitCommandExecutors();
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
        log_level = LogSystem::LogSystem::LogLevel::Warn;
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        log_level = LogSystem::LogSystem::LogLevel::Error;
        break;
      default:
        log_level = LogSystem::LogSystem::LogLevel::Warn;
    }
    LOG(log_level, std::string("validation layer: ") + callback_data->pMessage);
  }
  return VK_FALSE;
}

MM::RenderSystem::RenderEngine::RenderEngine() { Init(); }
