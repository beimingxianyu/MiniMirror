#pragma once

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/ManagedObjectUnorderedMap.h"

namespace MM {
namespace Manager {
template <
    typename ManagedType,
    typename IsDeriveFromManagedObjectBase = typename std::enable_if<
        std::is_base_of<ManagedObjectBase, ManagedType>::value, void>::type>
class ManagedObjectHandler;

template <
    typename ManagedType,
    typename IsDeriveFromManagedObjectBase = typename std::enable_if<
        std::is_base_of<ManagedObjectBase, ManagedType>::value, void>::type>
class ManagerBase {
 public:
  class BaseHandler;

  using ThisType = ManagerBase<ManagedType, IsDeriveFromManagedObjectBase>;
  using BaseNameToIDContainer =
      ManagedObjectUnorderedMultiMap<std::string, ManagedObjectID>;
  using BaseIDToObjectContainer =
      ManagedObjectUnorderedMap<ManagedObjectID, ManagedType>;
  using HandlerType = BaseHandler;

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

  virtual ExecuteResult GetObject(ManagedObjectID object_id,
                                  HandlerType& handle) const;

 public:
  class BaseHandler {
   public:
    BaseHandler() = default;
    BaseHandler(const BaseHandler& other) = default;
    BaseHandler(BaseHandler&& other) noexcept = default;
    BaseHandler& operator=(const BaseHandler& other) {
      if (&other == this) {
        return *this;
      }

      name_to_ID_handler_ = other.name_to_ID_handler_;
      ID_to_object_handler_ = other.ID_to_object_handler_;

      return *this;
    }
    BaseHandler& operator=(BaseHandler&& other) noexcept {
      if (&other == this) {
        return *this;
      }

      name_to_ID_handler_ = std::move(other.name_to_ID_handler_);
      ID_to_object_handler_ = std::move(other.ID_to_object_handler_);

      return *this;
    }

   private:
    typename BaseNameToIDContainer::HandlerType name_to_ID_handler_{};
    typename BaseIDToObjectContainer::HandlerType ID_to_object_handler_{};
  };

 protected:
  ManagerBase() = default;
  ~ManagerBase();

 private:
  static bool Destroy();

 private:
};
}  // namespace Manager
}  // namespace MM
