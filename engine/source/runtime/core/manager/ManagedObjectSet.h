#pragma once

#include <cassert>
#include <set>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Less>
struct WrapperLess {
  bool operator()(const ManagedObjectWrapper<ObjectType>& lhs,
                  const ManagedObjectWrapper<ObjectType>& rhs) const {
    return Less{}(lhs.GetObject(), rhs.GetObject());
  }
};

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
  friend bool operator<(const ManagedObjectWrapper<ObjectType>& lhs,
                        const ObjectType& rhs) {
    return Compare{}(lhs.GetObject(), rhs);
  }

  friend bool operator<(const ObjectType& lhs,
                        const ManagedObjectWrapper<ObjectType>& rhs) {
    return Compare{}(lhs, rhs.GetObject());
  }

 public:
  size_t GetSize() const override;

  bool Have(const ObjectType& key) const override;

  uint32_t Count(const ObjectType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

 protected:
 private:
  std::set<ManagedObjectWrapper<ObjectType>, WrapperLess<ObjectType, Compare>,
           Allocator>
      data_;
  mutable std::shared_mutex data_mutex_;
};

template <typename ObjectType, typename Compare, typename Allocator>
bool ManagedObjectSet<ObjectType, Compare, Allocator>::Have(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  return data_.find(key);
}

template <typename ObjectType, typename Compare, typename Allocator>
bool ManagedObjectSet<ObjectType, Compare, Allocator>::IsRelationshipContainer()
    const {
  return false;
}

template <typename ObjectType, typename Compare, typename Allocator>
bool ManagedObjectSet<ObjectType, Compare, Allocator>::IsMultiContainer()
    const {
  return false;
}

template <typename ObjectType, typename Compare, typename Allocator>
uint32_t ManagedObjectSet<ObjectType, Compare, Allocator>::Count(
    const ObjectType& key) const {
  if (Have(key)) {
    return 1;
  }

  return 0;
}

template <typename ObjectType, typename Compare, typename Allocator>
size_t ManagedObjectSet<ObjectType, Compare, Allocator>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
