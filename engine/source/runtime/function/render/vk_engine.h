#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#ifdef ERROR
#undef ERROR
#endif

#include <string>
#include <vector>

#include "runtime/function/render/AllocatedBuffer.h"
#include "runtime/function/render/DescriptorManager.h"
#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/vk_command.h"
#include "runtime/function/render/vk_utils.h"
#include "runtime/platform/file_system/file_system.h"

namespace MM {
namespace RenderSystem {
static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayerDebugCall(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

class RenderEngine {
  friend class RenderResourceDataBase;
  friend class RenderResourceTexture;
  friend class RenderResourceBuffer;
  friend class RenderResourceMesh;
  friend class RenderResourceFrameBuffer;
  friend class AllocatedCommandBuffer;
  template <typename ConstantType>
  friend class RenderResourceConstants;

 public:
  RenderEngine();
  RenderEngine(const RenderEngine& other) = delete;
  RenderEngine(RenderEngine&& other) = delete;
  RenderEngine& operator=(const RenderEngine& other) = delete;
  RenderEngine& operator=(RenderEngine&& other) = delete;
  ~RenderEngine() { CleanUp(); }

 public:
  void Init();

  void Run() const;

  void CleanUp();

 public:
  /**
   * \brief Judge whether the object is valid (if the object is initialized, it
   * is a valid object). \return Returns true if the object is valid, otherwise
   * returns false.
   */
  bool IsValid() const;

  Result<AllocatedBuffer> CreateBuffer(
      const std::string& object_name,
      const VkBufferCreateInfo& vk_buffer_create_info,
      const VmaAllocationCreateInfo& vma_allocation_create_info,
      VmaAllocationInfo* vma_allocation_info);

  Result<AllocatedBuffer> CreateStageBuffer(VkDeviceSize size,
                                            std::uint32_t queue_index);

  VmaAllocator GetAllocator();

  const VmaAllocator_T* GetAllocator() const;

  VkDevice GetDevice();

  const VkDevice_T* GetDevice() const;

  std::uint32_t GetGraphQueueIndex() const;

  std::uint32_t GetTransformQueueIndex() const;

  std::uint32_t GetPresentQueueIndex() const;

  std::uint32_t GetComputeQueueIndex() const;

  std::uint32_t GetQueueIndex(CommandBufferType command_buffer_type) const;

  VkQueue GetGraphQueue() const;

  VkQueue GetTransformQueue() const;

  VkQueue GetPresentQueue() const;

  VkQueue GetComputeQueue() const;

  VkQueue GetQueue(CommandBufferType command_buffer_type) const;

  Result<Nil> GraphQueueWaitIdle() const;

  Result<Nil> TransformQueueWaitIdle() const;

  Result<Nil> PresentQueueWaitIdle() const;

  Result<Nil> ComputeQueueWaitIdle() const;

  bool CommandExecutorIsFree() const;

  CommandExecutorGeneralCommandBufferGuard GetGeneralCommandBufferGuard(
      CommandBufferType command_buffer_type) const;

  /**
   * \remark The executed \ref command_task_flow will be moved, and no other
   * operations can be performed on \ref command_task_flow after calling this
   * function.
   */
  Result<RenderFuture> RunCommand(CommandTaskFlow&& command_task_flow) const;

  /**
   * \remark The executed \ref command_task_flow will be moved, and no other
   * operations can be performed on \ref command_task_flow after calling this
   * function.
   */
  Result<RenderFutureState> RunCommandAndWait(
      CommandTaskFlow&& command_task_flow) const;

  Result<RenderFuture> RunSingleCommand(CommandType command_type,
                                        bool is_async_record,
                                        const TaskType& commands);

  Result<RenderFuture> RunSingleCommand(
      CommandType command_type, bool is_async_record,
      const std::vector<RenderResourceDataID>&
          cross_task_flow_sync_render_resource_data_ID,
      const TaskType& commands) const;

  Result<RenderFutureState> RunSingleCommandAndWait(CommandType command_type,
                                                    bool is_async_record,
                                                    const TaskType& commands);

  Result<RenderFutureState> RunSingleCommandAndWait(
      CommandType command_type, bool is_async_record,
      const std::vector<RenderResourceDataID>&
          cross_task_flow_sync_render_resource_data_ID,
      const TaskType& commands) const;

  CommandExecutorLockGuard GetCommandExecutorLockGuard() const;

  /**
   * \remark The range specified by \ref regions cannot overlap.
   */
  Result<Nil> CopyBuffer(AllocatedBuffer& src_buffer,
                         AllocatedBuffer& dest_buffer,
                         const std::vector<VkBufferCopy2>& regions);

//  /**
//   * \remark The range specified by \ref regions cannot overlap and src_buffer
//   * can equal to dest_buffer.
//   */
//  Result<Nil> CopyBuffer(const AllocatedBuffer& src_buffer,
//                         AllocatedBuffer& dest_buffer,
//                         const std::vector<VkBufferCopy2>& regions);

  /**
   * \remark The areas specified for each \ref VkBufferCopy2 in the vector
   * cannot overlap, but the areas specified between each \ref VkBufferCopy2 can
   * overlap.
   */
  Result<Nil> CopyBuffer(
      AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
      const std::vector<std::vector<VkBufferCopy2>>& regions_vector);

//  /**
//   * \remark The areas specified for each \ref VkBufferCopy2 in the vector
//   * cannot overlap, but the areas specified between each \ref VkBufferCopy2 can
//   * overlap.
//   */
//  Result<Nil> CopyBuffer(
//      const AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
//      const std::vector<std::vector<VkBufferCopy2>>& regions_vector);

  /**
   * \remark The range specified by \ref regions cannot overlap.
   */
  Result<Nil> CopyImage(AllocatedImage& src_image, AllocatedImage& dest_image,
                        const std::vector<VkImageCopy2>& regions);

//  /**
//   * \remark The range specified by \ref regions cannot overlap and src_image
//   * can not equal to dest_image.
//   */
//  Result<Nil> CopyImage(const AllocatedImage& src_image,
//                        AllocatedImage& dest_image,
//                        const std::vector<VkImageCopy2>& regions);

  /**
   * \remark The areas specified for each \ref VkImageCopy in the vector
   * cannot overlap, but the areas specified between each \ref VkImageCopy can
   * overlap.
   */
  Result<Nil> CopyImage(
      AllocatedImage& src_image, AllocatedImage& dest_image,
      const std::vector<std::vector<VkImageCopy2>>& regions_vector);

//  /**
//   * \remark The areas specified for each \ref VkImageCopy in the vector
//   * cannot overlap, but the areas specified between each \ref VkImageCopy can
//   * overlap.Src_image can not equal to dest_image.
//   */
//  Result<Nil> CopyImage(
//      const AllocatedImage& src_image, AllocatedImage& dest_image,
//      const std::vector<std::vector<VkImageCopy2>>& regions_vector);

  Result<Nil> CopyDataToBuffer(AllocatedBuffer& dest_buffer, const void* data,
                               const VkDeviceSize& copy_offset,
                               const VkDeviceSize& copy_size) const;

  const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const;

  const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const;

  uint32_t GetCurrentFrame() const;

  uint32_t GetFlightFrameNumber() const;

  VkSampleCountFlagBits GetMultiSampleCount() const;

  bool SupportMultiDrawIndirect() const;

  bool FormatSupportStore(VkFormat format, VkImageTiling tiling) const;

  VkPipelineCache GetPipelineCache() const;

  DescriptorManager& GetDescriptorManager();

  const DescriptorManager& GetDescriptorManager() const;

 private:
  void InitGlfw();
  void InitVulkan();
  void InitInfo();
  // void Draw();

  void InitInstance();
  void InitSurface();
  void InitPhysicalDevice();
  void InitGpuProperties();
  void InitGpuFeatures();
  void InitLogicalDevice();
  void InitAllocator();
  void InitSwapChain();
  void InitSwapChainImageView();
  void InitCommandExecutor();
  void FindSupportStorageImageFormat();
  void InitDescriptorManager();
  void InitPipelineCache();

  static std::vector<VkExtensionProperties> GetExtensionProperties();
  static bool CheckExtensionSupport(const std::string& extension_name);
  bool CheckDeviceExtensionSupport(const VkPhysicalDevice& physical_device);
  static bool CheckValidationLayerSupport();
  void GetGlfwRequireExtensions(std::vector<const char*>& extensions) const;
  void GetRequireExtensions(std::vector<const char*>& extensions) const;
  static void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void SetUpDebugMessenger();
  static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                     VkDebugUtilsMessengerEXT debug_messenger,
                                     const VkAllocationCallbacks* allocator);
  int RateDeviceSuitability(const VkPhysicalDevice& physical_device);
  QueueFamilyIndices FindQueueFamily(
      const VkPhysicalDevice& physical_device) const;
  SwapChainSupportDetails QuerySwapChainSupport(
      const VkPhysicalDevice& physical_device) const;
  static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& available_formats);
  static VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& available_present_modes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
  void ChooseMultiSampleCount();
  static Result<Nil> CopyBufferInputParametersCheck(
      AllocatedBuffer& src_buffer, AllocatedBuffer& dest_buffer,
      const std::vector<VkBufferCopy2>& regions);
  static static Result<Nil> CopyImageInputParametersCheck(
      AllocatedImage& src_image, AllocatedImage& dest_image,
      const std::vector<VkImageCopy2>& regions);

 private:
  bool is_initialized_{false};
  uint32_t flight_frame_number_{3};
  VkExtent2D window_extent_{1920, 1080};
  bool enable_point_light_shadow_{true};
  std::vector<const char*> enable_layer_;
  std::vector<const char*> enable_instance_extensions_;
  std::vector<char const*> enable_device_extensions_ = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE3_EXTENSION_NAME,
      VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
      VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
#if defined(__MACH__)
      "VK_KHR_portability_subset"
#endif
  };
  QueueFamilyIndices queue_family_indices_{};
  static const std::string validation_layers_name;
#ifdef NDEBUG
  bool enable_validation_layers_{false};
#else
  bool enable_validation_layers_{true};
#endif
  VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
  std::vector<VkImage> swapchain_images_{};
  VkFormat swapchain_format_{};
  VkExtent2D swapchain_extent{};
  VkRect2D scissor_{};
  std::vector<VkImageView> swapchain_image_views_{};

  GLFWwindow* window_{nullptr};
  VkInstance instance_{nullptr};
  VkPhysicalDevice physical_device_{nullptr};
  VkPhysicalDeviceProperties gpu_properties_{};
  VkPhysicalDeviceFeatures gpu_features_{};
  std::unordered_set<VkFormat> support_storage_format_linear_{};
  std::unordered_set<VkFormat> support_storage_format_optimal_{};
  VkPipelineCache pipeline_cache_{nullptr};

  VkDevice device_{nullptr};
  VmaAllocator allocator_{nullptr};
  VkQueue graphics_queue_{nullptr};
  VkQueue compute_queue_{nullptr};
  VkQueue present_queue_{nullptr};
  VkQueue transform_queue_{nullptr};
  VkSurfaceKHR surface_{nullptr};
  // TODO 添加重置swapchain的函数
  // TODO 重置swapchain的函数也应该更新VkViewport
  // MM::RenderSystem::DefaultViewportState::default_viewport_和VkRect2D
  // MM::RenderSystem::DefaultViewportState::default_scissors_
  VkSwapchainKHR swapchain_{nullptr};
  std::uint64_t rendered_frame_count_{0};
  std::unique_ptr<CommandExecutor> command_executor_{nullptr};
  DescriptorManager descriptor_manager_{};

  RenderEngineInfo render_engine_info_{};
};
}  // namespace RenderSystem
}  // namespace MM
