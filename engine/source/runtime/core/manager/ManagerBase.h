#pragma once

#include "runtime/core/manager/ManagedObjectBase.h"

namespace MM {
namespace Manager {
template <
    typename ManagedType,
    typename IsDeriveFromManagedObjectBase = typename std::enable_if<
        std::is_base_of<ManagedObjectBase, ManagedType>::value, void>::type>
class ManagedObjectHandle;

template <
    typename ManagedType,
    typename IsDeriveFromManagedObjectBase = typename std::enable_if<
        std::is_base_of<ManagedObjectBase, ManagedType>::value, void>::type>
class ManagerBase {
 public:
  ManagerBase(const ManagerBase& other) = delete;
  ManagerBase(ManagerBase&& other) = delete;
  ManagerBase& operator=(const ManagerBase& other) = delete;
  ManagerBase& operator=(ManagerBase&& other) = delete;

 public:
  // TODO AddObject 添加参数 为添加对象的Handle
  virtual ExecuteResult AddObject(ManagedType&& managed_object);

  virtual ExecuteResult RemoveObject(ManagedObjectID removed_object_ID);

  virtual bool Have(ManagedObjectID object_ID) const;

  virtual ExecuteResult GetObject(
      ManagedObjectID object_id,
      ManagedObjectHandle<ManagedType>& handle) const;

 protected:
  ManagerBase() = default;
  ~ManagerBase();

 private:
  static bool Destroy();

 private:
};
}  // namespace Manager
}  // namespace MM
