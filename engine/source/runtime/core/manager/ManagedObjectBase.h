#pragma once

#include "runtime/platform/base/MMObject.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace Manager {

using ManagedObjectID = MM::Utils::GUID;

class ManagedObjectBase : virtual public MMObject {
 public:
  ManagedObjectBase() = default;
  virtual ~ManagedObjectBase() = default;
  ManagedObjectBase(const ManagedObjectBase& other) = delete;
  ManagedObjectBase(ManagedObjectBase&& other) noexcept = default;
  ManagedObjectBase& operator=(const ManagedObjectBase& other) = delete;
  ManagedObjectBase& operator=(ManagedObjectBase&& other) noexcept;

 public:
  bool IsValid() const override;

  virtual std::size_t GetObjectSize() const;

  virtual ExecuteResult GetLightCopy() const;

  virtual ExecuteResult GetDeepCopy(
      ManagedObjectID& new_managed_object_ID) const;
};

}  // namespace Manager
}  // namespace MM
