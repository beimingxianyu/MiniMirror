#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace MM {
namespace RenderSystem {
class RenderEngine;

enum class ResourceType {
  Texture,
  BUFFER,
  VERTEX_BUFFER,
  FRAME_BUFFER,
  CONSTANTS,
  UNDEFINED
};

/**
 * \brief Memory operations allowed for rendering resources.
 */
enum class MemoryOperate { READ, WRITE, READ_AND_WRITE, UNDEFINED };

enum class CommandBufferType {
  GRAPH,
  COMPUTE
};

enum class ImageTransferMode {
  INIT_TO_ATTACHMENT,
  INIT_TO_TRANSFER_DESTINATION,
  TRANSFER_DESTINATION_TO_SHARED_READABLE,
  TRANSFER_DESTINATION_TO_SHARED_PRESENT,
  ATTACHMENT_TO_PRESENT,
  INIT_TO_DEPTH_TEST,
  ATTACHMENT_TO_TRANSFER_SOURCE,
  TRANSFER_DESTINATION_TO_TRANSFER_SOURCE
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family_;
  std::optional<uint32_t> present_family_;
  std::optional<uint32_t> compute_family_;

  bool isComplete() const;
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities_{};
  std::vector<VkSurfaceFormatKHR> formats_{};
  std::vector<VkPresentModeKHR> presentModes_{};
};

struct ImageInfo {
  VkExtent3D image_extent_{0, 0, 0};
  VkDeviceSize image_size_{0};
  VkFormat image_format_{VK_FORMAT_UNDEFINED};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_UNDEFINED};
  uint32_t mipmap_levels{1};
  bool can_mapped_{false};
};

struct BufferInfo {
  VkDeviceSize buffer_size_{0};
  VkDeviceSize offset_{0};
  VkDeviceSize dynamic_offset_{0};
  bool can_mapped_{false};
};

class AllocatedCommandBuffer {
 public:
  AllocatedCommandBuffer() = default;
  ~AllocatedCommandBuffer() = default;
  AllocatedCommandBuffer(RenderEngine* engine,
                         VkQueue queue,
                         const VkCommandPool& command_pool,
                         const VkCommandBuffer& command_buffer);
  AllocatedCommandBuffer(const AllocatedCommandBuffer& other) = default;
  AllocatedCommandBuffer(AllocatedCommandBuffer&& other) noexcept = default;
  AllocatedCommandBuffer& operator=(const AllocatedCommandBuffer& other);
  AllocatedCommandBuffer& operator=(AllocatedCommandBuffer&& other) noexcept;

public:
  const RenderEngine& GetRenderEngine() const;

  const VkQueue& GetQueue() const;

  const VkCommandPool& GetCommandPool() const;

  const VkCommandBuffer& GetCommandBuffer() const;

  const VkFence& GetFence() const;

  
  /**
   * \brief Record commands in the command buffer.
   * \param function A function that contains the record operations you want to perform.
   * \param auto_start_end_submit If this item is true, the start command, end command,
   * and submit command (etc.) are automatically recorded. The default value is false.
   * If this item is true, please do not perform automatically completed work in the function again.
   * \param record_new_command Whether to not use the last submitted command buffer.
   * The default value is true.
   * \param submit_info_ptr Custom VkSubmitInfo.
   * \return If there are no errors in the entire recording and submission process,
   * it returns true, otherwise it returns false.
   * \remark If \ref auto_start_end_submit is set to true and \ref function also has
   * a start command or an end command, an error will occur.
   * \remark Please do not create a VkFence in the \ref function and wait it. Doing so
   * will cause the program to permanently block.
   */
  bool RecordAndSubmitCommand(const std::function<void(VkCommandBuffer& cmd)>&
                                  function,
      const bool& auto_start_end_submit = false,
      const bool& record_new_command = true,
      const std::shared_ptr<VkSubmitInfo>& submit_info_ptr = nullptr);

  bool IsValid() const;

 private:
  class AllocatedCommandBufferWrapper {
   public:
    AllocatedCommandBufferWrapper() = default;
    ~AllocatedCommandBufferWrapper();
    AllocatedCommandBufferWrapper(RenderEngine* engine, const VkQueue& queue,
                                  const VkCommandPool& command_pool,
                                  const VkCommandBuffer& command_buffer);
    AllocatedCommandBufferWrapper(const AllocatedCommandBufferWrapper& other) =
        delete;
    AllocatedCommandBufferWrapper(AllocatedCommandBufferWrapper&& other) =
        delete;
    AllocatedCommandBufferWrapper& operator=(
        const AllocatedCommandBufferWrapper& other) = delete;
    AllocatedCommandBufferWrapper& operator=(
        AllocatedCommandBufferWrapper&& other) = delete;

   public:
    const RenderEngine& GetRenderEngine() const;

    const VkQueue& GetQueue() const;

    const VkCommandPool& GetCommandPool() const;

    const VkCommandBuffer& GetCommandBuffer() const;

    const VkFence& GetFence() const;

    bool RecordAndSubmitCommand(const std::function<void(VkCommandBuffer& cmd)>&
                                    function,
                                const bool& auto_start_end_submit = false,
                                const bool& record_new_command = true,
                                std::shared_ptr<VkSubmitInfo>
                                    submit_info_ptr = nullptr);

    bool IsValid() const;

  private:
    bool ResetCommandBuffer();

   private:
    RenderEngine* engine_{nullptr};
    VkQueue queue_{nullptr};
    std::shared_ptr<VkCommandPool> command_pool_{nullptr};
    VkCommandBuffer command_buffer_{nullptr};
    VkFence command_fence_{nullptr};
    std::mutex record_mutex_{};
  };

private:
  std::shared_ptr<AllocatedCommandBufferWrapper> wrapper_{nullptr};
  
};

class AllocatedBuffer {
 public:
  AllocatedBuffer() = default;
  ~AllocatedBuffer() = default;
  AllocatedBuffer(const VmaAllocator& allocator, const VkBuffer& buffer,
                  const VmaAllocation& allocation);
  AllocatedBuffer(const AllocatedBuffer& other) = default;
  AllocatedBuffer(AllocatedBuffer&& other) noexcept = default;
  AllocatedBuffer& operator=(const AllocatedBuffer& other);
  AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept;

 public:
  const VmaAllocator& GetAllocator() const;

  const VkBuffer& GetBuffer() const;

  const VmaAllocation& GetAllocation() const;

  void Release();

  uint32_t UseCount() const;

  bool IsValid() const;

 private:
  class AllocatedBufferWrapper {
    friend class RenderEngine;
    friend class RenderResourceTexture;

   public:
    AllocatedBufferWrapper() = default;
    ~AllocatedBufferWrapper();
    AllocatedBufferWrapper(const VmaAllocator& allocator,
                           const VkBuffer& buffer,
                           const VmaAllocation& allocation);
    AllocatedBufferWrapper(const AllocatedBufferWrapper& other) = delete;
    AllocatedBufferWrapper(AllocatedBufferWrapper&& other) = delete;
    AllocatedBufferWrapper& operator=(const AllocatedBufferWrapper& other) =
        delete;
    AllocatedBufferWrapper& operator=(AllocatedBufferWrapper&& other) = delete;

   public:
    const VmaAllocator& GetAllocator() const;

    const VkBuffer& GetBuffer() const;

    const VmaAllocation& GetAllocation() const;

    bool IsValid() const;

    void Release();

   private:
    VmaAllocator allocator_{nullptr};
    VkBuffer buffer_{nullptr};
    VmaAllocation allocation_{nullptr};
  };

 private:
  std::shared_ptr<AllocatedBufferWrapper> wrapper_{nullptr};
};

class AllocatedImage {
 public:
  AllocatedImage() = default;
  ~AllocatedImage() = default;
  AllocatedImage(const VmaAllocator& allocator, const VkImage& image,
                 const VmaAllocation& allocation);
  AllocatedImage(const AllocatedImage& other) = default;
  AllocatedImage(AllocatedImage&& other) noexcept = default;
  AllocatedImage& operator=(const AllocatedImage& other);
  AllocatedImage& operator=(AllocatedImage&& other) noexcept;

 public:
  const VmaAllocator& GetAllocator() const;

  const VkImage& GetImage() const;

  const VmaAllocation& GetAllocation() const;

  void Release();

  uint32_t UseCount() const;

  bool IsValid() const;

 private:
  class AllocatedImageWrapper {
    friend class RenderEngine;
    friend class RenderResourceTexture;

   public:
    AllocatedImageWrapper() = default;
    ~AllocatedImageWrapper();
    AllocatedImageWrapper(const VmaAllocator& allocator, const VkImage& image,
                          const VmaAllocation& allocation);
    AllocatedImageWrapper(const AllocatedImageWrapper& other) = delete;
    AllocatedImageWrapper(AllocatedImageWrapper&& other) = delete;
    AllocatedImageWrapper& operator=(const AllocatedImageWrapper& other) =
        delete;
    AllocatedImageWrapper& operator=(AllocatedImageWrapper&& other) = delete;

   public:
    const VmaAllocator& GetAllocator() const;

    const VkImage& GetImage() const;

    const VmaAllocation& GetAllocation() const;

    bool IsValid() const;

   private:
    VmaAllocator allocator_{nullptr};
    VkImage image_{nullptr};
    VmaAllocation allocation_{nullptr};
  };

 private:
  std::shared_ptr<const AllocatedImageWrapper> wrapper_{nullptr};
};
}  // namespace RenderSystem
}  // namespace MM
