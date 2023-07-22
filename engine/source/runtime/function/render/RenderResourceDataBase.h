#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <functional>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/vk_type_define.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace RenderSystem {
class RenderResourceDataBase : public Manager::ManagedObjectBase {
 public:
  RenderResourceDataBase(const std::string& resource_name,
                         const RenderResourceDataID& render_resource_data_ID);
  RenderResourceDataBase() = default;
  ~RenderResourceDataBase() override = default;
  RenderResourceDataBase(const RenderResourceDataBase& other) = delete;
  RenderResourceDataBase(RenderResourceDataBase&& other) noexcept;
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

  bool IsManaged() const;

  bool IsAssetResource() const;

  void MarkThisUseForWrite();

  void MarkThisIsManaged();

  void MarkThisIsAssetResource();

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

 protected:
  void SetRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID);

 private:
  RenderResourceDataID render_resource_data_ID_{};

  bool used_for_write_{false};
  bool is_managed_{false};
  bool is_asset_resource_{false};
};
}  // namespace RenderSystem
}  // namespace MM