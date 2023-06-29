#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <functional>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {
class RenderResourceDataBase : public Manager::ManagedObjectBase {
 public:
  RenderResourceDataBase(const std::string& resource_name,
                         const RenderResourceDataID& render_resource_data_ID);
  RenderResourceDataBase() = default;
  ~RenderResourceDataBase() override = default;
  RenderResourceDataBase(const RenderResourceDataBase& other) = delete;
  RenderResourceDataBase(RenderResourceDataBase&& other) noexcept = default;
  RenderResourceDataBase& operator=(const RenderResourceDataBase& other) =
      delete;
  RenderResourceDataBase& operator=(RenderResourceDataBase&& other) noexcept;

 public:
  bool operator==(const RenderResourceDataBase& rhs) const;

  bool operator!=(const RenderResourceDataBase& rhs) const;

  bool operator<(const RenderResourceDataBase& rhs) const;

  bool operator>(const RenderResourceDataBase& rhs) const;

  bool operator<=(const RenderResourceDataBase& rhs) const;

  bool operator>=(const RenderResourceDataBase& rhs) const;

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

  virtual std::unique_ptr<RenderResourceDataBase> GetCopy(
      const std::string& new_name_of_copy_resource) const;

 private:
  RenderResourceDataID render_resource_data_ID_{};

  bool used_for_write_{false};
};
}  // namespace RenderSystem
}  // namespace MM
