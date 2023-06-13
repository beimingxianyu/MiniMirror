//
// Created by beimingxianyu on 23-5-14.
//

#include "runtime/core/manager//ManagedObjectBase.h"

namespace MM {
namespace Manager {
std::size_t ManagedObjectBase::GetObjectSize() const { return 0; }

// ExecuteResult ManagedObjectBase::GetLightCopy(
//     const std::string&, std::unique_ptr<ManagedObjectBase>&) const {
//   return ExecuteResult::SUCCESS;
// }
//
// ExecuteResult ManagedObjectBase::GetDeepCopy(
//     const std::string&, std::unique_ptr<ManagedObjectBase>&) const {
//   return ExecuteResult::UNDEFINED_ERROR;
// }

ManagedObjectBase& ManagedObjectBase::operator=(
    ManagedObjectBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  MM::MMObject::operator=(std::move(other));
  object_name_ = std::move(other.object_name_);

  return *this;
}

ManagedObjectBase::ManagedObjectBase(const std::string& object_name)
    : MMObject(), object_name_(object_name) {}

const std::string& ManagedObjectBase::GetObjectName() const {
  return object_name_;
}

ManagedObjectID ManagedObjectBase::GetObjectID() const { return GetGuid(); }
ManagedObjectBase& ManagedObjectBase::operator=(
    const ManagedObjectBase& other) {
  if (&other == this) {
    return *this;
  }

  MMObject::operator=(other);
  object_name_ = other.object_name_;

  return *this;
}
}  // namespace Manager
}  // namespace MM