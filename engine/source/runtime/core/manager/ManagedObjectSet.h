#pragma once

#include <cassert>
#include <set>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Compare = std::less<ObjectType>,
          typename Allocator = std::allocator<ObjectType>>
class ManagedObjectSet : public ManagedObjectTableBase<ObjectType, ObjectType> {
 public:
  ManagedObjectSet() = default;
  ~ManagedObjectSet() = default;
  ManagedObjectSet(const ManagedObjectSet& other) = delete;
  ManagedObjectSet(ManagedObjectSet&& other) noexcept;
  ManagedObjectSet& operator=(const ManagedObjectSet& other) = delete;
  ManagedObjectSet& operator=(ManagedObjectSet&& other) noexcept;

 public:
  size_t GetSize() const override;

  ExecuteResult AddObjectImp(const ObjectType& key, ObjectType&& managed_object,
                             ManagedObjectHandle& handle) override;

  ExecuteResult AddObject(
      ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override {
    std::unique_lock<std::shared_mutex> guard{data_mutex_};
    data_.emplace(std::move(managed_object));
  }

 private:
  std::set<ObjectType> data_;
  std::shared_mutex data_mutex_;
};

template <typename ObjectType, typename Compare, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Compare, Allocator>::AddObjectImp(
    const ObjectType& key, ObjectType&& managed_object,
    ManagedObjectHandle& handle) {
  LOG_ERROR("This function should not be used.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Compare, typename Allocator>
size_t ManagedObjectSet<ObjectType, Compare, Allocator>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
