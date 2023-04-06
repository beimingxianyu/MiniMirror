#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
#include <utility>

#include "runtime/function/render/pre_header.h"
#include "runtime/resource/asset_type/asset_type.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;
class RenderObject;

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

struct RenderResourceManageInfo {
  bool use_to_write{false};
  bool is_shared_{true};
};

struct DataToBufferInfo {
  void* data_ = nullptr;
  VkDeviceSize copy_offset_ = 0;
  VkDeviceSize copy_size_ = 0;
  bool used_{false};
};

class ManagedObjectBase {
 public:
  ManagedObjectBase() = default;
  virtual ~ManagedObjectBase() = default;
  ManagedObjectBase(const std::string& object_name,
                    const std::uint32_t& object_ID);
  ManagedObjectBase(const ManagedObjectBase& other) = default;
  ManagedObjectBase(ManagedObjectBase&& other) noexcept;
  ManagedObjectBase& operator=(const ManagedObjectBase& other);
  ManagedObjectBase& operator=(ManagedObjectBase&& other) noexcept;

  friend bool operator==(const ManagedObjectBase& lhs,
      const ManagedObjectBase& rhs);

  friend bool operator!=(const ManagedObjectBase& lhs,
                         const ManagedObjectBase& rhs);

public:
  const std::string& GetObjectName() const;

  const std::uint32_t& GetObjectID() const;

  virtual void Release();

protected:
  void SetObjectName(const std::string& new_object_name);
 void SetObjectID(const std::uint32_t& new_object_ID);

private:
 std::string object_name_{};
 std::uint32_t object_ID_{0};
};

template <typename ObjectType,
          typename IsDeriveType = std::enable_if<
              std::is_base_of<ManagedObjectBase, ObjectType>::value, void>::type>
class RenderManageDataWrapper;

template<typename ObjectType>
class RenderManageBase {
  friend class RenderManageDataWrapper<ObjectType>;
 friend class RenderObject;

 public:
  RenderManageBase(const RenderManageBase& other) = delete;
  RenderManageBase(RenderManageBase&& other) = delete;
  RenderManageBase& operator=(const RenderManageBase& other) = delete;
  RenderManageBase& operator=(RenderManageBase&& other) = delete;

 public:
  static RenderManageBase* GetInstanceBase();

  uint32_t GetIncreaseIndex();

  bool HaveData(const std::string& object_name) const;

  bool HaveData(const std::uint32_t& object_ID) const;

  uint32_t GetObjectIDFromName(const std::string& object_name) const;

  const std::string& GetObjectNameFromID(const std::uint32_t& object_ID) const;

  //virtual void UseData(const std::string& object_name);

  //virtual void UseDate(const std::uint32_t& object_ID);

  //virtual void DiscardData(const std::string& object_name);

  //virtual void DiscardData(const std::uint32_t& object_ID);

  //virtual void UpdateManage();

  /**
   * \remark Return value is object id, but will return 0 when \ref object_name is not exist.
   */
  virtual std::uint32_t GetDeepCopy(const std::string& new_name_of_object, const std::string& object_name);

  virtual std::uint32_t GetDeepCopy(const std::string& new_name_of_object,
                                    const std::uint32_t& object_ID);

  /**
   * \remark Return value is object id, but will return 0 when \ref object_name
   * is not exist.
   */
  virtual std::uint32_t GetLightCopy(const std::string& new_name_of_object,
                                     const std::string& object_name);

  virtual std::uint32_t GetLightCopy(const std::string& new_name_of_object,
                                     const std::uint32_t& object_ID);

  virtual std::uint32_t SaveData(std::unique_ptr<ObjectType>&& object);

  virtual bool AddUse(const std::uint32_t& object_id);

  virtual bool AddUse(const std::string& object_name);

  virtual void ReleaseUse(const std::uint32_t& object_id);

 protected:
  RenderManageBase() = default;
  ~RenderManageBase() = default;
  static RenderManageBase* render_manager_;

private:
  static bool DestroyBase();

 private:
  static std::mutex sync_flag_base;

  mutable std::shared_mutex data_lock_{};
  std::uint32_t increase_index_{1};
  std::mutex increase_index_lock{};
  std::unordered_map<std::string, uint32_t> object_name_to_ID_{};
  std::unordered_map<uint32_t, RenderManageDataWrapper<ObjectType>> object_ID_to_object_{};
};

template <typename ObjectType,
          typename IsDeriveType = std::enable_if<
              std::is_base_of<ManagedObjectBase, ObjectType>::value, void>::type>
class RenderManageDataWrapper {
  friend class RenderManageBase<ObjectType>;

 public:
  RenderManageDataWrapper() = delete;
  ~RenderManageDataWrapper() = default;
  RenderManageDataWrapper(const RenderManageDataWrapper& other) = delete;
  RenderManageDataWrapper(RenderManageDataWrapper&& other) noexcept;
  RenderManageDataWrapper& operator=(
      const RenderManageDataWrapper& other) = delete;
  RenderManageDataWrapper& operator=(RenderManageDataWrapper&& other) noexcept = delete;

public:
  ObjectType& GetData();
 const ObjectType& GetData() const;
  std::unique_ptr<ObjectType> GetDataDeepCopy(
      const std::string& new_name_of_copy) const;
 std::unique_ptr<ObjectType> GetDataLightCopy(
     const std::string& new_name_of_copy) const;
  uint32_t GetUseCount() const;

protected:
 template <typename... Args>
 explicit RenderManageDataWrapper(Args args...);

  template <typename DerivedType, typename ...Args>
  explicit RenderManageDataWrapper(Args args...);

  explicit RenderManageDataWrapper(std::unique_ptr<ObjectType>&& object);

private:
 std::uint32_t use_count_{1};
  std::unique_ptr<ObjectType> object_;
};

template <typename ObjectType>
std::mutex RenderManageBase<ObjectType>::sync_flag_base{};

template <typename ObjectType>
RenderManageBase<ObjectType>* RenderManageBase<ObjectType>::
GetInstanceBase() {
  if (render_manager_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_base};
    if (!render_manager_) {
      render_manager_ = new RenderManageBase{};
    }
  }
  return render_manager_;
}

template <typename ObjectType>
uint32_t RenderManageBase<ObjectType>::GetIncreaseIndex() {
  std::lock_guard<std::mutex> guard{increase_index_lock};
  ++increase_index_;
  if (increase_index_ == 0) {
    ++increase_index_;
  }

  while (HaveData(increase_index_)) {
    increase_index_ = increase_index_ * 2;
    if (increase_index_ == 0) {
      ++increase_index_;
    }
  }

  return increase_index_;
}

template <typename ObjectType>
bool RenderManageBase<ObjectType>::HaveData(const std::string& object_name) const {
  std::shared_lock<std::shared_mutex> guard(data_lock_);
  return object_name_to_ID_.count(object_name) == 1;
}

template <typename ObjectType>
bool RenderManageBase<ObjectType>::HaveData(const std::uint32_t& object_ID) const {
  std::shared_lock<std::shared_mutex> guard(data_lock_);
  return object_ID_to_object_.count(object_ID) == 1;
}

template <typename ObjectType>
uint32_t RenderManageBase<ObjectType>::GetObjectIDFromName(
    const std::string& object_name) const {
  if (!HaveData(object_name)) {
    LOG_ERROR(object_name + "is not existent.0 will be return.")
    return 0;
  }
  std::shared_lock<std::shared_mutex> guard{data_lock_};
  return object_name_to_ID_.at(object_name);
}

template <typename ObjectType>
const std::string& RenderManageBase<ObjectType>::GetObjectNameFromID(
    const std::uint32_t& object_ID) const {
  std::shared_lock<std::shared_mutex> guard{data_lock_};
  return object_ID_to_object_.at(object_ID).GetData().GetObjectName();
}

//template <typename ObjectType>
//void RenderManageBase<ObjectType>::UseData(const std::string& object_name) {
//  assert(HaveData(object_name));
//  std::unique_lock<std::shared_mutex> guard(data_lock_);
//  ++update_buffer[object_name_to_ID_[object_name]];
//}
//
//template <typename ObjectType>
//void RenderManageBase<ObjectType>::UseDate(const std::uint32_t& object_ID) {
//  std::unique_lock<std::shared_mutex> guard(data_lock_);
//  ++update_buffer[object_ID];
//}
//
//template <typename ObjectType>
//void RenderManageBase<ObjectType>::DiscardData(const std::string& object_name) {
//  assert(HaveData(object_name));
//  std::unique_lock<std::shared_mutex> guard(data_lock_);
//  const auto use_count = --update_buffer[object_name_to_ID_[object_name]];
//}
//
//template <typename ObjectType>
//void RenderManageBase<ObjectType>::DiscardData(const std::uint32_t& object_ID) {
//  std::unique_lock<std::shared_mutex> guard(data_lock_);
//  --update_buffer[object_ID];
//}
//
//template <typename ObjectType>
//void RenderManageBase<ObjectType>::UpdateManage() {
//  for (const auto& data : update_buffer) {
//    std::unique_lock<std::shared_mutex> guard{data_lock_};
//    assert(data.second >= 0 ||
//           data.second * -1 > object_ID_to_object_.at(data.first).use_count_);
//    object_ID_to_object_[data.first].use_count_ += data.second;
//    if (object_ID_to_object_.at(data.first).use_count_ == 0) {
//      object_name_to_ID_.erase(
//          object_ID_to_object_.at(data.first).GetData().GetObjectName());
//      object_ID_to_object_.erase(data.first);
//    }
//  }
//}

template <typename ObjectType>
std::uint32_t RenderManageBase<ObjectType>::GetDeepCopy(
    const std::string& new_name_of_object, const std::string& object_name) {
  const auto name_to_ID = GetObjectIDFromName(object_name);
  if (name_to_ID == 0) {
    LOG_ERROR(object_name + " is not exist.")
    return 0;
  }
  std::uint32_t ID = GetIncreaseIndex();
  std::unique_ptr<ObjectType> object{nullptr};
  {
    std::shared_lock<std::shared_mutex> guard{data_lock_};
    auto new_object =
        object_ID_to_object_.at(name_to_ID).GetDataDeepCopy(new_name_of_object);

    if (!new_object) {
      return 0;
    }

    object.reset(
        std::move(new_object));
  }

  {
    std::unique_lock<std::shared_mutex> guard{data_lock_};
    object_name_to_ID_.emplace(new_name_of_object, ID);
    object_ID_to_object_.emplace(
        ID, RenderManageDataWrapper<ObjectType>(object.release()));
  }

  return ID;
}

template <typename ObjectType>
std::uint32_t RenderManageBase<ObjectType>::GetDeepCopy(
    const std::string& new_name_of_object, const std::uint32_t& object_ID) {
  assert(HaveData(object_ID));
  std::uint32_t ID = GetIncreaseIndex();
  std::unique_ptr<ObjectType> object{nullptr};
  {
    std::shared_lock<std::shared_mutex> guard{data_lock_};
    auto new_object =
        object_ID_to_object_.at(object_ID).GetDataDeepCopy(new_name_of_object);

    if (!new_object) {
      return 0;
    }

    object.reset(std::move(new_object));
  }

  {
    std::unique_lock<std::shared_mutex> guard{data_lock_};
    object_name_to_ID_.emplace(new_name_of_object, ID);
    object_ID_to_object_.emplace(
        ID, RenderManageDataWrapper<ObjectType>(object.release()));
  }

  return ID;
}

template <typename ObjectType>
std::uint32_t RenderManageBase<ObjectType>::GetLightCopy(
    const std::string& new_name_of_object, const std::string& object_name) {
  const auto name_to_ID = GetObjectIDFromName(object_name);
  if (name_to_ID == 0) {
    LOG_ERROR(object_name + " is not exist.")
    return 0;
  }
  std::uint32_t ID = GetIncreaseIndex();
  std::unique_ptr<ObjectType> object{nullptr};
  {
    std::shared_lock<std::shared_mutex> guard{data_lock_};
    auto new_object = object_ID_to_object_.at(name_to_ID)
                          .GetDataLightCopy(new_name_of_object);
    if (!new_object) {
      return 0; 
    }

    object.reset(std::move(new_object));
  }

  {
    std::unique_lock<std::shared_mutex> guard{data_lock_};
    object_name_to_ID_.emplace(new_name_of_object, ID);
    object_ID_to_object_.emplace(
        ID, RenderManageDataWrapper<ObjectType>(object.release()));
  }

  return ID;
}

template <typename ObjectType>
std::uint32_t RenderManageBase<ObjectType>::GetLightCopy(
    const std::string& new_name_of_object, const std::uint32_t& object_ID) {
  assert(HaveData(object_ID));
  std::uint32_t ID = GetIncreaseIndex();
  std::unique_ptr<ObjectType> object{nullptr};
  {
    std::shared_lock<std::shared_mutex> guard{data_lock_};
    auto new_object = object_ID_to_object_.at(object_ID)
                          .GetDataLightCopy(new_name_of_object);
    if (!new_object) {
      return 0;
    }

    object.reset(std::move(new_object));
  }

  {
    std::unique_lock<std::shared_mutex> guard{data_lock_};
    object_name_to_ID_.emplace(new_name_of_object, ID);
    object_ID_to_object_.emplace(
        ID, RenderManageDataWrapper<ObjectType>(object.release()));
  }

  return ID;
}

template <typename ObjectType>
std::uint32_t RenderManageBase<ObjectType>::SaveData(
    std::unique_ptr<ObjectType>&& object) {
  auto object_ID = object->GetObjectName();
  std::unique_lock<std::shared_mutex> guard{data_lock_};
  object_name_to_ID_.emplace(object_ID, object->GetObjectID());
  object_ID_to_object_.emplace(
      object->GetObjectID(),
      RenderManageDataWrapper<ObjectType>(object.release()));

  return object_ID;
}

template <typename ObjectType>
bool RenderManageBase<ObjectType>::AddUse(const std::uint32_t& object_id) {
  assert(HaveData(object_id));
  std::unique_lock<std::shared_mutex> guard{data_lock_};
  ++(object_ID_to_object_.at(object_id).use_count_);
  return true;
}

template <typename ObjectType>
bool RenderManageBase<ObjectType>::AddUse(const std::string& object_name) {
  const auto ID = GetObjectIDFromName(object_name);
  if (ID == 0) {
    LOG_ERROR(object_name + " is not exist.")
    return false;
  }
  std::unique_lock<std::shared_mutex> guard{data_lock_};
  ++object_ID_to_object_.at(ID).use_count_;

  return true;
}

template <typename ObjectType>
void RenderManageBase<ObjectType>::ReleaseUse(const std::uint32_t& object_id) {
  assert(HaveData(object_id));
  std::unique_lock<std::shared_mutex> guard{data_lock_};
  auto user_count = --object_ID_to_object_.at(object_id).use_count_;
  if (user_count == 0) {
    object_name_to_ID_.erase(
        object_ID_to_object_.at(object_id).GetData().GetObjectName());
    object_ID_to_object_.erase(object_id);
  } }

template <typename ObjectType>
bool RenderManageBase<ObjectType>::DestroyBase() {
  std::lock_guard<std::mutex> guard{sync_flag_base};
  if (render_manager_) {
    delete render_manager_;
    render_manager_ = nullptr;

    return true;
  }

  return true;
}

template <typename ObjectType, typename IsDeriveType>
RenderManageDataWrapper<ObjectType, IsDeriveType>::RenderManageDataWrapper(
    RenderManageDataWrapper&& other) noexcept
  : use_count_(other.use_count_),
    object_(std::move(other.use_count_)) {
  other.use_count_ = 0;
}

template <typename ObjectType, typename IsDeriveType>
ObjectType& RenderManageDataWrapper<ObjectType, IsDeriveType>::GetData() {
  return *object_;
}

template <typename ObjectType, typename IsDeriveType>
const ObjectType& RenderManageDataWrapper<ObjectType, IsDeriveType>::GetData() const {
  return *object_;
}

template <typename ObjectType, typename IsDeriveType>
std::unique_ptr<ObjectType> RenderManageDataWrapper<ObjectType, IsDeriveType>::
GetDataDeepCopy(const std::string& new_name_of_copy) const {
  return object_->GetDeepCopy(new_name_of_copy);
}

template <typename ObjectType, typename IsDeriveType>
std::unique_ptr<ObjectType> RenderManageDataWrapper<ObjectType, IsDeriveType>::
GetDataLightCopy(const std::string& new_name_of_copy) const {
  return object_->GetLightCopy(new_name_of_copy);
}

template <typename ObjectType, typename IsDeriveType>
uint32_t RenderManageDataWrapper<ObjectType, IsDeriveType>::GetUseCount() const {
  return use_count_;
}

template <typename ObjectType, typename IsDeriveType>
template <typename ... Args>
RenderManageDataWrapper<ObjectType, IsDeriveType>::RenderManageDataWrapper(
Args args, ...)
    : use_count_(1),
      object_{new ObjectType{std::forward<Args>(args)...}} {}

template <typename ObjectType, typename IsDeriveType>
template <typename DerivedType, typename ... Args>
RenderManageDataWrapper<ObjectType, IsDeriveType>::RenderManageDataWrapper(
Args args, ...)
  :use_count_(1),
    object_(new DerivedType{std::forward<Args>(args)...}) {}

template <typename ObjectType, typename IsDeriveType>
RenderManageDataWrapper<ObjectType, IsDeriveType>::RenderManageDataWrapper(
    std::unique_ptr<ObjectType>&& object) : object_(std::move(object)){}

struct RenderEngineInfo {
  VkSampleCountFlagBits multi_sample_count_{VK_SAMPLE_COUNT_1_BIT};
};

struct ImageBindInfo {
  ImageBindInfo() = default;
  ~ImageBindInfo() = default;
  ImageBindInfo(const ImageBindInfo& other) = default;
  ImageBindInfo(ImageBindInfo&& other) noexcept;
  ImageBindInfo& operator=(const ImageBindInfo& other);
  ImageBindInfo& operator=(ImageBindInfo&& other) noexcept;

  VkDescriptorSetLayoutBinding bind_{};
  std::shared_ptr<VkImageView> image_view_{nullptr};
  std::shared_ptr<VkSampler> sampler_{nullptr};
  std::shared_ptr<VkSemaphore> semaphore_{nullptr};

  bool IsValid() const;

  void Reset();
};

struct ImageInfo {
  VkExtent3D image_extent_{0, 0, 0};
  VkDeviceSize image_size_{0};
  VkFormat image_format_{VK_FORMAT_UNDEFINED};
  VkImageLayout image_layout_{VK_IMAGE_LAYOUT_UNDEFINED};
  uint32_t mipmap_levels{1};
  bool can_mapped_{false};
  bool is_transform_src_{false};
  bool is_transform_dest_{false};

  void Reset();

  bool IsValid() const;
};

struct BufferBindInfo {
  BufferBindInfo() = default;
  ~BufferBindInfo() = default;
  BufferBindInfo(const BufferBindInfo& other) = default;
  BufferBindInfo(BufferBindInfo&& other) noexcept;
  BufferBindInfo& operator=(const BufferBindInfo& other);
  BufferBindInfo& operator=(BufferBindInfo&& other) noexcept;

  VkDescriptorSetLayoutBinding bind_{};
  VkDeviceSize range_size_{0};
  VkDeviceSize offset_{0};
  VkDeviceSize dynamic_offset_{0};
  std::shared_ptr<VkSemaphore> semaphore_{nullptr};

  void Reset();

  bool IsValid() const;
};

struct BufferInfo {
  VkDeviceSize buffer_size_{0};
  bool can_mapped_{false};
  bool is_transform_src{false};
  bool is_transform_dest{false};

  void Reset();

  bool IsValid() const;
};

class VertexInputState {
 public:
  VertexInputState();
  ~VertexInputState() = default;
  /**
   * \brief Construct object with a vertex buffer offset.(No instance description)
   * \param vertex_buffer_offset The offset of vertex buffer.
   */
  VertexInputState(const VkDeviceSize& vertex_buffer_offset);
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
   * \remark When a layout error occurs, the \ref error_message will be set as an
   * error message, otherwise the \ref error_message will be set to "succeeded".
   */
  bool CheckLayoutIsCorrect(std::string& error_message) const;

  bool CheckLayoutIsCorrect() const;

  const VkVertexInputBindingDescription& GetVertexBind() const;

  const VkDeviceSize& GetVertexBufferOffset() const;

  const std::vector<VkVertexInputAttributeDescription>& GetVertexAttributes()
      const;

  const std::vector<VkVertexInputBindingDescription>& GetInstanceBinds() const;

  const std::vector<VkDeviceSize>& GetInstanceBufferOffset() const;

  const std::vector<VkVertexInputAttributeDescription>& GetInstanceAttributes() const;

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

class AllocatedCommandBuffer {
 public:
  AllocatedCommandBuffer() = default;
  ~AllocatedCommandBuffer() = default;
  AllocatedCommandBuffer(RenderEngine* engine, VkQueue queue,
                         const VkCommandPool& command_pool,
                         const VkCommandBuffer& command_buffer);
  AllocatedCommandBuffer(const AllocatedCommandBuffer& other) = default;
  AllocatedCommandBuffer(AllocatedCommandBuffer&& other) noexcept;
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
   * \param function A function that contains the record operations you want to
   * perform.
   * \param auto_start_end_submit If this item is true, the start
   * command, end command, and submit command (etc.) are automatically recorded.
   * The default value is false. If this item is true, please do not perform
   * automatically completed work in the function again.
   * \param record_new_commands Whether to not use the last submitted command buffer.
   * The default value is true.
   * \param submit_info_ptr Custom VkSubmitInfo.
   * \return If there are no errors in the entire recording and submission
   * process, it returns true, otherwise it returns false. \remark If \ref
   * auto_start_end_submit is set to true and \ref function also has a start
   * command or an end command, an error will occur. \remark Please do not
   * create a VkFence in the \ref function and wait it. Doing so will cause the
   * program to permanently block.
   */
  bool RecordAndSubmitCommand(
      const std::function<void(VkCommandBuffer& cmd)>& function,
      const bool& auto_start_end_submit = false,
      const bool& record_new_commands = true,
      const std::shared_ptr<VkSubmitInfo>& submit_info_ptr = nullptr);

  /**
   * \remark This function is mostly the same as \ref RecordAndSubmitCommand,
   * except that the \ref function can return a value point out the \ref function
   * execute result.If the \ref function return true specifies execution succeeded,
   * otherwise return false.
   */
 bool RecordAndSubmitCommand(
      const std::function<bool(VkCommandBuffer& cmd)>& function,
      const bool& auto_start_end_submit = false,
      const bool& record_new_commands = true,
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

    bool RecordAndSubmitCommand(
        const std::function<void(VkCommandBuffer& cmd)>& function,
        const bool& auto_start_end_submit = false,
        const bool& record_new_command = true,
        std::shared_ptr<VkSubmitInfo> submit_info_ptr = nullptr);

    bool RecordAndSubmitCommand(
        const std::function<bool(VkCommandBuffer& cmd)>& function,
        const bool& auto_start_end_submit = false,
        const bool& record_new_command = true,
        std::shared_ptr<VkSubmitInfo> submit_info_ptr = nullptr);

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
                  const VmaAllocation& allocation, const BufferInfo& buffer_info);
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

   private:
    VmaAllocator allocator_{nullptr};
    VkBuffer buffer_{nullptr};
    VmaAllocation allocation_{nullptr};
  };

 private:
  BufferInfo buffer_info_{};
  std::shared_ptr<AllocatedBufferWrapper> wrapper_{nullptr};
};

class AllocatedImage {
  friend class RenderResourceTexture;

 public:
  AllocatedImage() = default;
  ~AllocatedImage() = default;
  AllocatedImage(const VmaAllocator& allocator, const VkImage& image,
                 const VmaAllocation& allocation, const ImageInfo& image_info);
  AllocatedImage(const AllocatedImage& other) = default;
  AllocatedImage(AllocatedImage&& other) noexcept;
  AllocatedImage& operator=(const AllocatedImage& other);
  AllocatedImage& operator=(AllocatedImage&& other) noexcept;

 public:
  const VkExtent3D& GetImageExtent() const;

  const VkDeviceSize& GetImageSize() const;

  const VkFormat& GetImageFormat() const;

  const VkImageLayout& GetImageLayout() const;

  const uint32_t& GetMipmapLevels() const;

  const bool& CanMapped() const;

  const bool& IsTransformSrc() const;

  const bool& IsTransformDest() const;

  const ImageInfo& GetImageInfo() const;

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
  ImageInfo image_info_{};
  std::shared_ptr<const AllocatedImageWrapper> wrapper_{nullptr};
};

class BufferChunkInfo {
public:
  BufferChunkInfo() = delete;
 ~BufferChunkInfo() = default;
  BufferChunkInfo(const VkDeviceSize& start_offset,
                  const VkDeviceSize& end_offset);
  BufferChunkInfo(const BufferChunkInfo& other) = default;
 BufferChunkInfo(BufferChunkInfo&& other) noexcept = default;
  BufferChunkInfo& operator=(const BufferChunkInfo& other) noexcept;
 BufferChunkInfo& operator=(BufferChunkInfo&& other) noexcept;

public:
 const VkDeviceSize& GetOffset() const;

  const VkDeviceSize& GetSize() const;

private:
  VkDeviceSize offset_{};
  VkDeviceSize size_{};
};

class VertexAndIndexBuffer {
public:
  VertexAndIndexBuffer() = delete;
 ~VertexAndIndexBuffer() = default;
  VertexAndIndexBuffer(RenderEngine* engine);
  VertexAndIndexBuffer(const VertexAndIndexBuffer& other) = delete;
 VertexAndIndexBuffer(VertexAndIndexBuffer&& other) = delete;
  VertexAndIndexBuffer& operator=(const VertexAndIndexBuffer& other) = delete;
 VertexAndIndexBuffer& operator=(VertexAndIndexBuffer&& other) = delete;

public:
 bool IsValid() const;

 const AllocatedBuffer& GetVertexBuffer() const;

 const AllocatedBuffer& GetIndexBuffer() const;

  const BufferBindInfo& GetVertexBufferInfo() const;

  const BufferBindInfo& GetIndexBufferInfo() const;

  /**
   * \brief Determine whether the buffer is accessible at this time.
   * (The buffer is in an unreadable state during the expansion process)
   * \return If the buffer is accessible at this time, it returns true,
   * otherwise it returns false.
   */
  bool Accessible() const;

  bool AllocateBuffer(
      const std::vector<AssetType::Vertex>& vertices,
      const std::vector<uint32_t>& indexes,
      const std::shared_ptr<BufferChunkInfo>& output_buffer_chunk_info);

  void Release();

private:
  bool ReserveVertexBuffer();

  bool ReserveIndexBuffer();

  bool Reserve();

private:
 AllocatedBuffer vertex_buffer_{};
 AllocatedBuffer index_buffer_{};
 BufferBindInfo vertex_buffer_info_{};
 BufferBindInfo index_buffer_info_{};
 std::list<std::shared_ptr<BufferChunkInfo>> vertex_buffer_chunks_info{};
 std::list<std::shared_ptr<BufferChunkInfo>> index_buffer_chunks_info{};
 std::atomic_bool accessible_{false};
};
}  // namespace RenderSystem
}  // namespace MM
