#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

#include "runtime/function/render/import_other_system.h"

// TODO Split these classes into different files.
namespace MM {
namespace RenderSystem {

struct BufferInfo {
  VkDeviceSize buffer_size_{0};
  bool can_mapped_{false};
  bool is_transform_src_{false};
  bool is_transform_dest_{false};
  bool is_exclusive_{false};
  std::vector<std::uint32_t> queue_index_{0};

  void Reset();

  bool IsValid() const;
};

class VertexInputState {
 public:
  VertexInputState();
  ~VertexInputState() = default;
  /**
   * \brief Construct object with a vertex buffer offset.(No instance
   * description) \param vertex_buffer_offset The offset of vertex buffer.
   */
  explicit VertexInputState(const VkDeviceSize& vertex_buffer_offset);
  VertexInputState(
      const VkDeviceSize& vertex_buffer_offset,
      const std::vector<VkVertexInputBindingDescription>& instance_binds,
      const std::vector<VkDeviceSize>& instance_buffer_offset,
      const std::vector<VkVertexInputAttributeDescription>&
          instance_attributes);
  VertexInputState(const VertexInputState& other) = default;
  VertexInputState(VertexInputState&& other) noexcept;
  VertexInputState& operator=(const VertexInputState& other);
  VertexInputState& operator=(VertexInputState&& other) noexcept;

 public:
  bool IsValid() const;

  void Reset();

  /**
   * \remark When a layout error occurs, the \ref error_message will be set as
   * an error message, otherwise the \ref error_message will be set to
   * "succeeded".
   */
  bool CheckLayoutIsCorrect(std::string& error_message) const;

  bool CheckLayoutIsCorrect() const;

  const VkVertexInputBindingDescription& GetVertexBind() const;

  const VkDeviceSize& GetVertexBufferOffset() const;

  const std::vector<VkVertexInputAttributeDescription>& GetVertexAttributes()
      const;

  const std::vector<VkVertexInputBindingDescription>& GetInstanceBinds() const;

  const std::vector<VkDeviceSize>& GetInstanceBufferOffset() const;

  const std::vector<VkVertexInputAttributeDescription>& GetInstanceAttributes()
      const;

 private:
  void InitDefaultVertexInput();

 private:
  VkVertexInputBindingDescription vertex_bind_{};
  VkDeviceSize vertex_buffer_offset_{0};
  std::vector<VkVertexInputAttributeDescription> vertex_attributes_{5};
  std::vector<VkVertexInputBindingDescription> instance_binds_{};
  std::vector<VkDeviceSize> instance_buffer_offset_{};
  std::vector<VkVertexInputAttributeDescription> instance_attributes_{};
};

class AllocatedBuffer {
 public:
  AllocatedBuffer() = default;
  ~AllocatedBuffer() = default;
  AllocatedBuffer(const VmaAllocator& allocator, const VkBuffer& buffer,
                  const VmaAllocation& allocation,
                  const BufferInfo& buffer_info);
  AllocatedBuffer(const AllocatedBuffer& other) = default;
  AllocatedBuffer(AllocatedBuffer&& other) noexcept;
  AllocatedBuffer& operator=(const AllocatedBuffer& other);
  AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept;

 public:
  const VkDeviceSize& GetBufferSize() const;

  bool CanMapped() const;

  bool IsTransformSrc() const;

  bool IsTransformDest() const;

  const BufferInfo& GetBufferInfo() const;

  VmaAllocator GetAllocator() const;

  VkBuffer GetBuffer() const;

  VmaAllocation GetAllocation() const;

  const std::vector<std::uint32_t>& GetQueueIndexes() const;

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

   private:
    VmaAllocator allocator_{nullptr};
    VkBuffer buffer_{nullptr};
    VmaAllocation allocation_{nullptr};
  };

 private:
  BufferInfo buffer_info_{};
  std::shared_ptr<AllocatedBufferWrapper> wrapper_{nullptr};
};

class ImageChunkInfo {
 public:
  ImageChunkInfo() = default;
  ~ImageChunkInfo() = default;
  ImageChunkInfo(const VkOffset3D& offset, const VkExtent3D& extent);
  ImageChunkInfo(const ImageChunkInfo& other) = default;
  ImageChunkInfo(ImageChunkInfo&& other) noexcept = default;
  ImageChunkInfo& operator=(const ImageChunkInfo& other);
  ImageChunkInfo& operator=(ImageChunkInfo&& other) noexcept;

 public:
  VkOffset3D& GetOffset();
  const VkOffset3D& GetOffset() const;

  VkExtent3D& GetExtent();
  const VkExtent3D& GetExtent() const;

  void SetOffset(const VkOffset3D& new_offset);

  void SetExtent(const VkExtent3D& new_extent);

  void Reset();

  bool IsValid() const;

 private:
  VkOffset3D offset_{0, 0, 0};
  VkExtent3D extent_{0, 0, 0};
};

class BufferChunkInfo {
 public:
  BufferChunkInfo() = default;
  ~BufferChunkInfo() = default;
  BufferChunkInfo(const VkDeviceSize& start_offset,
                  const VkDeviceSize& end_offset);
  BufferChunkInfo(const BufferChunkInfo& other) = default;
  BufferChunkInfo(BufferChunkInfo&& other) noexcept = default;
  BufferChunkInfo& operator=(const BufferChunkInfo& other) noexcept;
  BufferChunkInfo& operator=(BufferChunkInfo&& other) noexcept;

 public:
  VkDeviceSize& GetOffset();
  const VkDeviceSize& GetOffset() const;

  VkDeviceSize& GetSize();
  const VkDeviceSize& GetSize() const;

  void SetOffset(const VkDeviceSize& new_offset);

  void SetSize(const VkDeviceSize& new_size);

  void Reset();

  bool IsValid() const;

 private:
  VkDeviceSize offset_{0};
  VkDeviceSize size_{0};
};

class VertexAndIndexBuffer {
 public:
  VertexAndIndexBuffer() = delete;
  ~VertexAndIndexBuffer() = default;
  explicit VertexAndIndexBuffer(RenderEngine* engine);
  VertexAndIndexBuffer(const VertexAndIndexBuffer& other) = delete;
  VertexAndIndexBuffer(VertexAndIndexBuffer&& other) = delete;
  VertexAndIndexBuffer& operator=(const VertexAndIndexBuffer& other) = delete;
  VertexAndIndexBuffer& operator=(VertexAndIndexBuffer&& other) = delete;

 public:
  bool IsValid() const;

  const AllocatedBuffer& GetVertexBuffer() const;

  const AllocatedBuffer& GetIndexBuffer() const;

  const BufferInfo& GetVertexBufferInfo() const;

  const BufferInfo& GetIndexBufferInfo() const;

  ExecuteResult AllocateBuffer(
      const std::vector<AssetType::Vertex>& vertices,
      const std::vector<uint32_t>& indexes,
      const std::shared_ptr<BufferChunkInfo>& output_vertex_buffer_chunk_info,
      const std::shared_ptr<MM::RenderSystem::BufferChunkInfo>&
          output_index_buffer_chunk_info);

  void Release();

 private:
  enum class BufferType { VERTEX, INDEX };
  bool ChooseVertexBufferReserveSize(const VkDeviceSize& require_size,
                                     VkDeviceSize& output_reserve_size);

  bool ChooseIndexBufferReserveSize(const VkDeviceSize& require_size,
                                    VkDeviceSize& output_reserve_size);

  bool ChooseReserveSize(const BufferType& buffer_type,
                         const VkDeviceSize& require_size,
                         VkDeviceSize& output_reserve_size);

  void GetEndSizeAndOffset(
      const AllocatedBuffer& buffer,
      std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
      VkDeviceSize& output_end_size, VkDeviceSize& output_offset);

  ExecuteResult ScanBufferToFindSuitableArea(
      AllocatedBuffer& buffer,
      std::list<std::shared_ptr<BufferChunkInfo>>& buffer_chunks_info,
      const VkDeviceSize& require_size, VkDeviceSize& output_offset);

  bool ReserveVertexBuffer(const VkDeviceSize& new_buffer_size);

  bool ReserveIndexBuffer(const VkDeviceSize& new_buffer_size);

  bool Reserve(const VkDeviceSize& new_vertex_buffer_size,
               const VkDeviceSize& new_index_buffer_size);

 private:
  RenderEngine* render_engine_{nullptr};
  AllocatedBuffer vertex_buffer_{};
  AllocatedBuffer index_buffer_{};
  std::list<std::shared_ptr<BufferChunkInfo>> vertex_buffer_chunks_info_{};
  std::list<std::shared_ptr<BufferChunkInfo>> index_buffer_chunks_info_{};
};

class AllocateSemaphore {
 public:
  explicit AllocateSemaphore(RenderEngine* engine,
                             VkSemaphoreCreateFlags flags = 0);
  ~AllocateSemaphore() = default;
  AllocateSemaphore(const AllocateSemaphore& other) = default;
  AllocateSemaphore(AllocateSemaphore&& other) noexcept = default;
  AllocateSemaphore& operator=(const AllocateSemaphore& other);
  AllocateSemaphore& operator=(AllocateSemaphore&& other) noexcept;

 public:
  VkSemaphore& GetSemaphore();
  const VkSemaphore& GetSemaphore() const;

  bool IsValid() const;

 private:
  class AllocateSemaphoreWrapper {
   public:
    AllocateSemaphoreWrapper() = default;
    AllocateSemaphoreWrapper(RenderEngine* engine, VkSemaphore semaphore);
    ~AllocateSemaphoreWrapper();
    AllocateSemaphoreWrapper(const AllocateSemaphoreWrapper& other) = delete;
    AllocateSemaphoreWrapper(AllocateSemaphoreWrapper&& other) noexcept;
    AllocateSemaphoreWrapper& operator=(const AllocateSemaphoreWrapper& other) =
        delete;
    AllocateSemaphoreWrapper& operator=(
        AllocateSemaphoreWrapper&& other) noexcept;

   public:
    VkSemaphore& GetSemaphore();
    const VkSemaphore& GetSemaphore() const;

    bool IsValid() const;

   private:
    RenderEngine* render_engine_{nullptr};
    VkSemaphore semaphore_{nullptr};
  };

 private:
  std::shared_ptr<AllocateSemaphoreWrapper> wrapper_{nullptr};
};

class AllocateFence {
 public:
  explicit AllocateFence(RenderEngine* engine, VkFenceCreateFlags flags = 0);
  ~AllocateFence() = default;
  AllocateFence(const AllocateFence& other) = default;
  AllocateFence(AllocateFence&& other) noexcept = default;
  AllocateFence& operator=(const AllocateFence& other);
  AllocateFence& operator=(AllocateFence&& other) noexcept;

 public:
  VkFence& GetFence();
  const VkFence& GetFence() const;

  bool IsValid() const;

 private:
  class AllocateFenceWrapper {
   public:
    AllocateFenceWrapper() = default;
    AllocateFenceWrapper(RenderEngine* engine, VkFence fence);
    ~AllocateFenceWrapper();
    AllocateFenceWrapper(const AllocateFenceWrapper& other) = delete;
    AllocateFenceWrapper(AllocateFenceWrapper&& other) noexcept;
    AllocateFenceWrapper& operator=(const AllocateFenceWrapper& other) = delete;
    AllocateFenceWrapper& operator=(AllocateFenceWrapper&& other) noexcept;

   public:
    VkFence& GetFence();
    const VkFence& GetFence() const;

    bool IsValid() const;

   private:
    RenderEngine* render_engine_{nullptr};
    VkFence fence_{nullptr};
  };

 private:
  std::shared_ptr<AllocateFenceWrapper> wrapper_{nullptr};
};
}  // namespace RenderSystem
}  // namespace MM
