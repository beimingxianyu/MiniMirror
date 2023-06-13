#pragma once

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/ManagedObjectUnorderedMap.h"

namespace MM {
namespace Manager {
template <
    typename ManagedType,
    typename IsDeriveFromManagedObjectBase = typename std::enable_if<
        std::is_base_of<ManagedObjectBase, ManagedType>::value, void>::type>
class ManagerBaseImp {
 public:
  class BaseHandler;

  using ThisType = ManagerBaseImp<ManagedType, IsDeriveFromManagedObjectBase>;
  using BaseNameToIDContainer =
      ManagedObjectUnorderedMultiMap<std::string, ManagedObjectID>;
  using BaseIDToObjectContainer =
      ManagedObjectUnorderedMap<ManagedObjectID, ManagedType>;
  using HandlerType = BaseHandler;

 public:
  class BaseHandler {
   public:
    BaseHandler() = default;
    ~BaseHandler() = default;
    BaseHandler(
        typename BaseNameToIDContainer::HandlerType&& name_to_ID_handler,
        typename BaseIDToObjectContainer::HandlerType&& ID_to_object_handler)
        : name_to_ID_handler_(name_to_ID_handler),
          ID_to_object_handler_(ID_to_object_handler) {}
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

   public:
    bool IsValid() const { return ID_to_object_handler_.IsValid(); }

    ManagedObjectID GetObjectID() const {
      return name_to_ID_handler_.GetObject();
    }

    const std::string& GetObjectName() const {
      return name_to_ID_handler_.GetKey();
    }

    ManagedType& GetObject() { return ID_to_object_handler_.GetObject(); }

    const ManagedType& GetObject() const {
      return ID_to_object_handler_.GetObject();
    }

    typename BaseNameToIDContainer::HandlerType& GetNameToIDHandler() {
      return name_to_ID_handler_;
    }

    const typename BaseNameToIDContainer::HandlerType& GetNameToIDHandler()
        const {
      return name_to_ID_handler_;
    }

    BaseIDToObjectContainer& GetIDToObjectHandler() {
      return ID_to_object_handler_;
    }

    const BaseIDToObjectContainer& GetIDToObjectHandler() const {
      return ID_to_object_handler_;
    }

    void Release() {
      ID_to_object_handler_.Release();
      name_to_ID_handler_.Release();
    }

   private:
    typename BaseNameToIDContainer::HandlerType name_to_ID_handler_{};
    typename BaseIDToObjectContainer::HandlerType ID_to_object_handler_{};
  };

 public:
  ManagerBaseImp(const ManagerBaseImp& other) = delete;
  ManagerBaseImp(ManagerBaseImp&& other) = delete;
  ManagerBaseImp& operator=(const ManagerBaseImp& other) = delete;
  ManagerBaseImp& operator=(ManagerBaseImp&& other) = delete;

 public:
  std::uint64_t GetSize() const { return ID_to_object_container_.GetSize(); }

  bool Have(ManagedObjectID object_ID) const {
    return ID_to_object_container_.Have(object_ID);
  }

  std::uint32_t Count(ManagedObjectID object_id) const {
    return ID_to_object_container_.Count(object_id);
  }

  ExecuteResult GetIDsByName(const std::string& object_name,
                             std::vector<ManagedObjectID>& IDs) const {
    std::vector<typename BaseNameToIDContainer::HandlerType>
        name_to_ID_handlers;
    MM_CHECK_WITHOUT_LOG(
        name_to_ID_container_.GetObject(object_name, name_to_ID_handlers),
        return MM_RESULT_CODE;)

    if (name_to_ID_handlers.empty()) {
      return Utils::ExecuteResult::
          PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (const auto& handler : name_to_ID_handlers) {
      IDs.emplace_back(handler.GetObject());
    }

    return ExecuteResult ::SUCCESS;
  }

 protected:
  ExecuteResult AddObjectBase(ManagedType&& managed_object,
                              HandlerType& handler) {
    typename BaseNameToIDContainer ::HandlerType name_ID_handler;
    typename BaseIDToObjectContainer ::HandlerType ID_to_object_handler;

    ManagedObjectID copy_ID = managed_object.GetObjectID();
    MM_CHECK_WITHOUT_LOG(
        name_to_ID_container_.AddObject(managed_object.GetObjectName(),
                                        std::move(copy_ID), name_ID_handler),
        return MM_RESULT_CODE;)
    MM_CHECK_WITHOUT_LOG(ID_to_object_container_.AddObject(
                             managed_object.GetObjectID(),
                             std::move(managed_object), ID_to_object_handler),
                         return MM_RESULT_CODE;)
    handler = HandlerType{std::move(name_ID_handler),
                          std::move(ID_to_object_handler)};

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetObjectByIDBase(ManagedObjectID object_id,
                                  HandlerType& handler) const {
    typename BaseIDToObjectContainer ::HandlerType ID_to_object_handler;
    std::vector<typename BaseNameToIDContainer::HandlerType> name_ID_handlers;

    MM_CHECK_WITHOUT_LOG(
        ID_to_object_container_.GetObject(object_id, ID_to_object_handler),
        return MM_RESULT_CODE;)
    MM_CHECK_WITHOUT_LOG(
        name_to_ID_container_.GetObject(
            ID_to_object_handler.GetObject().GetObjectName(), name_ID_handlers),
        return MM_RESULT_CODE;)

    for (auto& name_ID_handler : name_ID_handlers) {
      if (name_ID_handler.GetObject() == object_id) {
        handler = HandlerType{std::move(name_ID_handler),
                              std::move(ID_to_object_handler)};

        return ExecuteResult ::SUCCESS;
      }
    }

    assert(false);
    return ExecuteResult ::UNDEFINED_ERROR;
  }

  ExecuteResult GetObjectByNameBase(const std::string& object_name,
                                    std::vector<HandlerType>& handlers) const {
    std::vector<typename BaseNameToIDContainer ::HandlerType> name_id_handlers;

    MM_CHECK_WITHOUT_LOG(
        name_to_ID_container_.GetObject(object_name, name_id_handlers),
        return MM_RESULT_CODE;)

    if (name_id_handlers.empty()) {
      return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (auto& name_ID_handler : name_id_handlers) {
      typename BaseIDToObjectContainer::HandlerType id_object_handler;
      MM_CHECK_WITHOUT_LOG(ID_to_object_container_.GetObject(
                               name_ID_handler.GetObject(), id_object_handler),
                           continue;)

      handlers.emplace_back(std::move(name_ID_handler),
                            std::move(id_object_handler));
    }

    return ExecuteResult ::SUCCESS;
  }

  //  ExecuteResult GetLightCopyBase(const std::string& new_object_name,
  //                                 HandlerType& handler) const;
  //
  //  ExecuteResult GetDeepCopyBase(const std::string& new_object_name,
  //                                HandlerType& handler) const;

 protected:
  ManagerBaseImp() = default;
  explicit ManagerBaseImp(std::uint64_t size)
      : name_to_ID_container_(size), ID_to_object_container_(size) {}
  virtual ~ManagerBaseImp() = default;

 private:
  BaseNameToIDContainer name_to_ID_container_{};
  BaseIDToObjectContainer ID_to_object_container_{};
};

template <typename ManagedType>
class ManagerBase : public ManagerBaseImp<ManagedType> {
 public:
  using HandlerType = typename ManagerBaseImp<ManagedType>::HandlerType;

 public:
  ManagerBase(const ManagerBase& other) = delete;
  ManagerBase(ManagerBase&& other) = delete;
  ManagerBase& operator=(const ManagerBase& other) = delete;
  ManagerBase& operator=(ManagerBase&& other) = delete;

 protected:
  ManagerBase() = default;
  explicit ManagerBase(std::uint64_t size)
      : ManagerBaseImp<ManagedType>(size) {}
  virtual ~ManagerBase() = default;
};
};  // namespace Manager
}  // namespace MM
