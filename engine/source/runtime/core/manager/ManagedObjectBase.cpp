//
// Created by beimingxianyu on 23-5-14.
//

#include "runtime/core/manager//ManagedObjectBase.h"

namespace MM {
namespace Manager {
std::size_t ManagedObjectBase::GetObjectSize() const { return 0; }

ExecuteResult ManagedObjectBase::GetLightCopy() const {
  return ExecuteResult::SUCCESS;
}

ExecuteResult ManagedObjectBase::GetDeepCopy(
    ManagedObjectID& new_managed_object_ID) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

ManagedObjectBase& ManagedObjectBase::operator=(
    ManagedObjectBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  MM::MMObject::operator=(std::move(other));

  return *this;
}

bool ManagedObjectBase::IsValid() const { return MMObject::IsValid(); }
}  // namespace Manager
}  // namespace MM