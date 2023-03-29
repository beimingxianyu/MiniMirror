#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <atomic>
#include <optional>
#include <string>
#include <vector>

#include "runtime/core/log/log_system.h"
#include "runtime/function/render/import_other_system.h"
#include "runtime/function/render/vulkan_type.h"
#include "runtime/function/render/vulkan_utils.h"
#include "runtime/platform/config_system/config_system.h"
#include "runtime/platform/file_system/file_system.h"
#include "utils/utils.h"

namespace MM {
namespace RenderSystem {
static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayerDebugCall(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

class RenderEngine {
  friend class RenderResourceBase;
  friend class RenderResourceTexture;
  friend class RenderResourceBuffer;
  friend class RenderResourceVertexBuffer;
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

  void Run();

  void CleanUp();

  public:
  /**
   * \brief Judge whether the object is valid (if the object is initialized, it
   * is a valid object). \return Returns true if the object is valid, otherwise
   * returns false.
   */
  bool IsValid() const;

  AllocatedBuffer CreateBuffer(const size_t& alloc_size,
                               const VkBufferUsageFlags& usage,
                               const VmaMemoryUsage& memory_usage,
                               const VmaAllocationCreateFlags& allocation_flags = 0) const;

  const VmaAllocator& GetAllocator() const;

  const VkDevice& GetDevice() const;

  /**
   * \brief Record commands in the command buffer.
   * \param command_buffer_type The type of command buffer that the command to
   * be record belongs to
   * \param function A function that contains the record
   * operations you want to perform.
   * \param auto_start_end_submit If this item
   * is true, the start command, end command, and submit command (etc.) are
   * automatically recorded. The default value is false.
   * \param record_new_command Whether to not use the last submitted command buffer.
   * The default value is true.
   * \param submit_info_ptr Custom VkSubmitInfo.
   * \return If there are no errors in the entire recording and submission
   * process, it returns true, otherwise it returns false.
   * \remark If \ref auto_start_end_submit is set to true and \ref function also has a start
   * command or an end command, an error will occur.
   */
  bool RecordAndSubmitCommand(
      const CommandBufferType& command_buffer_type,
      const std::function<void(VkCommandBuffer& cmd)>& function,
      const bool& auto_start_end_submit = false,
      const bool& record_new_command = true,
      const std::shared_ptr<VkSubmitInfo>& submit_info_ptr = nullptr);

  bool RecordAndSubmitSingleTimeCommand(
      const CommandBufferType& command_buffer_type,
      const std::function<void(VkCommandBuffer& cmd)>& function,
      const bool& auto_start_end_submit = false);

  uint32_t GetCurrentFrame() const;

 private:
  void InitGlfw();
  void InitVulkan();
  // void Draw();

  void InitInstance();
  void InitSurface();
  void InitPhysicalDevice();
  void InitGpuProperties();
  void InitLogicalDevice();
  void InitAllocator();
  void InitSwapChain();
  void InitSwapChainImageView();
  // void InitCommandPool();
  // void InitCommandBuffers();
  void InitCommandExecutors();

  std::vector<VkExtensionProperties> GetExtensionProperties();
  bool CheckExtensionSupport(const std::string& extension_name);
  bool CheckDeviceExtensionSupport(const VkPhysicalDevice& physical_device);
  bool CheckValidationLayerSupport();
  void GetGlfwRequireExtensions(std::vector<const char*>& extensions) const;
  void GetRequireExtensions(std::vector<const char*>& extensions);
  void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void SetUpDebugMessenger();
  void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                     VkDebugUtilsMessengerEXT debug_messenger,
                                     const VkAllocationCallbacks* allocator);
  int RateDeviceSuitability(const VkPhysicalDevice& physical_device);
  QueueFamilyIndices FindQueueFamily(
      const VkPhysicalDevice& physical_device) const;
  SwapChainSupportDetails QuerySwapChainSupport(
      const VkPhysicalDevice& physical_device) const;
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& available_formats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& available_present_modes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

 private:
  bool is_initialized_{false};
  uint32_t flight_frame_number_{3};
  VkExtent2D window_extent_{1920, 1080};
  bool enable_point_light_shadow_{true};
  std::vector<const char*> enable_layer_;
  std::vector<const char*> enable_instance_extensions_;
  std::vector<char const*> enable_device_extensions_ = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
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
  VkPhysicalDeviceProperties gpu_properties_;
  VkDevice device_{nullptr};
  VmaAllocator allocator_{nullptr};
  VkQueue graphics_queue_{nullptr};
  VkQueue compute_queue_{nullptr};
  VkQueue present_queue_{nullptr};
  VkSurfaceKHR surface_{nullptr};
  // TODO 添加重置swapchain的函数
  VkSwapchainKHR swapchain_{nullptr};
  // VkCommandPool graph_command_pool_{nullptr};
  // VkCommandPool compute_command_pool_{nullptr};
  uint32_t rendered_frame_count_{0};
  // std::vector<VkCommandBuffer> graph_command_buffers_{};
  // std::vector<VkCommandBuffer> compute_command_buffers_{};
  // std::vector<std::mutex> graph_command_record_mutex_{3};
  // std::vector<std::mutex> compute_command_record_mutex_{3};
  std::vector<AllocatedCommandBuffer> graph_command_executors_{};
  std::vector<AllocatedCommandBuffer> compute_command_executors_{};
};
}  // namespace RenderSystem
}  // namespace MM