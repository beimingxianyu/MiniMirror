//
// Created by beimingxianyu on 23-5-14.
//

#include "runtime/core/manager//ManagedObjectBase.h"

namespace MM {
namespace Manager {
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

void Swap(ManagedObjectBase& lhs, ManagedObjectBase& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(dynamic_cast<MMObject&>(lhs), dynamic_cast<MMObject&>(rhs));
  swap(lhs.object_name_, rhs.object_name_);
}

void swap(ManagedObjectBase& lhs, ManagedObjectBase& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(dynamic_cast<MMObject&>(lhs), dynamic_cast<MMObject&>(rhs));
  swap(lhs.object_name_, rhs.object_name_);
}

bool ManagedObjectBase::LowLevelEqual(const ManagedObjectBase& other) const {
  return GetObjectID() == other.GetObjectID();
}

void ManagedObjectBase::Reset() {
  MMObject::Reset();
  object_name_.clear();
}
}  // namespace Manager
}  // namespace MM