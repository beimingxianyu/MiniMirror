#pragma once

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType>
class ManagedObjectVector
    : public ManagedObjectTableBase<ObjectType, ObjectType> {
 public:
  ~ManagedObjectVector() override = default;

 public:
  size_t GetSize() const override;

  ExecuteResult AddObject(
      const ObjectType& key, ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override;

  ExecuteResult AddObject(
      ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override {
    return ManagedObjectTableBase::AddObject(managed_object, handle);
  }

 private:
  std::vector<ObjectType> data_{};
  std::shared_mutex data_mutex_{};
};

template <typename ObjectType>
ExecuteResult ManagedObjectVector<ObjectType>::AddObject(
    const ObjectType& key, ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  LOG_WARN(
      "The underlying implementation of this container is vector, no key is "
      "required, "
      "and the parameter key is ignored. Please use "
      "template <typename ObjectType>\n"
      "ExecuteResult ManagedObjectVector<ObjectType>::AddObject(\n"
      "    const ObjectType& key, ObjectType&& managed_object,\n"
      "    ManagedObjectHandle<ObjectType, ObjectType>& handle)； instead of "
      "this function.");
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
}

template <typename ObjectType>
size_t ManagedObjectVector<ObjectType>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
