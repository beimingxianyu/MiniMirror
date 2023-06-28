#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <functional>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {
class RenderResourceBase : public Manager::ManagedObjectBase {
  friend class RenderResourceTexture;
  friend class RenderResourceBuffer;
  friend class RenderResourceMesh;
  friend class RenderResourceFrameBuffer;
  template <typename ConstantType>
  friend class RenderResourceConstants;

 public:
  RenderResourceBase() = default;
  ~RenderResourceBase() override = default;
  RenderResourceBase(const RenderResourceBase& other) = delete;
  RenderResourceBase(RenderResourceBase&& other) noexcept = default;
  RenderResourceBase& operator=(const RenderResourceBase& other) = delete;
  RenderResourceBase& operator=(RenderResourceBase&& other) noexcept;

 public:
  bool operator==(const RenderResourceBase& rhs) const;

  bool operator!=(const RenderResourceBase& rhs) const;

  bool operator<(const RenderResourceBase& rhs) const;

  bool operator>(const RenderResourceBase& rhs) const;

  bool operator<=(const RenderResourceBase& rhs) const;

  bool operator>=(const RenderResourceBase& rhs) const;

 public:
  const std::string& GetResourceName() const;

  const RenderResourceDataID& GetRenderResourceDataID() const;

  bool IsUseForWrite() const;

  void MarkThisUseForWrite();

  /**
   * \brief Release ownership of the resources held.
   */
  virtual void Release();

  virtual ResourceType GetResourceType() const;

  /**
   * \brief Gets the memory size occupied by the resource.
   * \return The size of the resource.
   * \remark If this object held is a resource array, the sum of the memory
   * occupied by all resources in the array is returned.
   */
  virtual VkDeviceSize GetSize() const;

  /**
   * \brief Determine whether the resource is a array.
   * \return Returns true if resource is a array, otherwise returns false.
   */
  virtual bool IsArray() const;

  virtual bool CanWrite() const;

  virtual std::unique_ptr<RenderResourceBase> GetCopy(
      const std::string& new_name_of_copy_resource) const;

 protected:
  RenderResourceBase(const std::string& resource_name,
                     const RenderResourceDataID& render_resource_data_ID);

 private:
  RenderResourceDataID render_resource_data_ID_{};

  bool used_for_write_{false};
};
}  // namespace RenderSystem
}  // namespace MM
