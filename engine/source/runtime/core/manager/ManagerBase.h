#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/ManagedObjectUnorderedMap.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace Manager {
using ManagedObjectIsSmartPoint = Utils::TrueType;
using ManagedObjectIsNotSmartPoint = Utils::FalseType;

template <typename ManagedType, typename ManagedTypeIsSmartPointType>
class ManagerBaseImp {
 public:
  class BaseHandler;

  using ThisType = ManagerBaseImp<ManagedType, ManagedTypeIsSmartPointType>;
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
    virtual bool IsValid() const { return ID_to_object_handler_.IsValid(); }

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

    ManagedType* GetObjectPtr() { return ID_to_object_handler_.GetObjectPtr(); }

    const ManagedType* GetObjectPtr() const {
      return ID_to_object_handler_.GetObjectPtr();
    }

    const std::atomic_uint32_t* GetObjectUseCountPtr() const {
      return ID_to_object_handler_.GetUseCountPtr();
    }

    const std::atomic_uint32_t* GetNameToIDHandlerUseCountPtr() const {
      return name_to_ID_handler_.GetUseCountPtr();
    }

    typename BaseNameToIDContainer::HandlerType& GetNameToIDHandler() {
      return name_to_ID_handler_;
    }

    const typename BaseNameToIDContainer::HandlerType& GetNameToIDHandler()
        const {
      return name_to_ID_handler_;
    }

    typename BaseIDToObjectContainer::HandlerType& GetIDToObjectHandler() {
      return ID_to_object_handler_;
    }

    const typename BaseIDToObjectContainer::HandlerType& GetIDToObjectHandler()
        const {
      return ID_to_object_handler_;
    }

    virtual void Release() {
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
  virtual bool IsValid() const {
    return name_to_ID_container_.IsValid() && ID_to_object_container_.IsValid();
  }

  std::uint64_t GetSize() const { return ID_to_object_container_.GetSize(); }

  bool Have(ManagedObjectID object_ID) const {
    return ID_to_object_container_.Have(object_ID);
  }

  std::uint32_t Count(ManagedObjectID object_ID) const {
    return ID_to_object_container_.Count(object_ID);
  }

  ExecuteResult GetNameByID(ManagedObjectID managed_object_ID,
                            std::string& name) const {
    return GetNameByIDImp(managed_object_ID, name,
                          ManagedTypeIsSmartPointType{});
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

  void Reserve(std::uint64_t size) {}

 protected:
  ExecuteResult GetIDToObjectHandler(
      ManagedObjectID object_ID,
      typename BaseIDToObjectContainer::HandlerType& handler) const {
    return ID_to_object_container_.GetObject(object_ID, handler);
  }

  ExecuteResult AddObjectBase(ManagedType&& managed_object,
                              HandlerType& handler) {
    return AddObjectBaseImp(std::move(managed_object), handler,
                            ManagedTypeIsSmartPointType{});
  }

  ExecuteResult GetObjectByIDBase(ManagedObjectID object_ID,
                                  HandlerType& handler) const {
    return GetObjectByIDBaseImp(object_ID, handler,
                                ManagedTypeIsSmartPointType{});
  }

  ExecuteResult GetObjectByNameBase(const std::string& object_name,
                                    std::vector<HandlerType>& handlers) const {
    std::vector<typename BaseNameToIDContainer ::HandlerType> name_ID_handlers;

    MM_CHECK_WITHOUT_LOG(
        name_to_ID_container_.GetObject(object_name, name_ID_handlers),
        return MM_RESULT_CODE;)

    if (name_ID_handlers.empty()) {
      return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (auto& name_ID_handler : name_ID_handlers) {
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
  ExecuteResult GetNameByIDImp(ManagedObjectID managed_object_ID,
                               std::string& name,
                               ManagedObjectIsNotSmartPoint) const {
    typename BaseIDToObjectContainer ::HandlerType handler;

    MM_CHECK_WITHOUT_LOG(
        ID_to_object_container_.GetObject(managed_object_ID, handler),
        return MM_RESULT_CODE;)

    name = handler.GetObject().GetObjectName();

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetNameByIDImp(ManagedObjectID managed_object_ID,
                               std::string& name,
                               ManagedObjectIsSmartPoint) const {
    typename BaseIDToObjectContainer ::HandlerType handler;

    MM_CHECK_WITHOUT_LOG(
        ID_to_object_container_.GetObject(managed_object_ID, handler),
        return MM_RESULT_CODE;)

    name = handler.GetObject()->GetObjectName();

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult AddObjectBaseImp(ManagedType&& managed_object,
                                 HandlerType& handler,
                                 ManagedObjectIsNotSmartPoint) {
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

  ExecuteResult AddObjectBaseImp(ManagedType&& managed_object,
                                 HandlerType& handler,
                                 ManagedObjectIsSmartPoint) {
    typename BaseNameToIDContainer ::HandlerType name_ID_handler;
    typename BaseIDToObjectContainer ::HandlerType ID_to_object_handler;

    ManagedObjectID copy_ID = managed_object->GetObjectID();
    MM_CHECK_WITHOUT_LOG(
        name_to_ID_container_.AddObject(managed_object->GetObjectName(),
                                        std::move(copy_ID), name_ID_handler),
        return MM_RESULT_CODE;)
    MM_CHECK_WITHOUT_LOG(ID_to_object_container_.AddObject(
                             managed_object->GetObjectID(),
                             std::move(managed_object), ID_to_object_handler),
                         return MM_RESULT_CODE;)
    handler = HandlerType{std::move(name_ID_handler),
                          std::move(ID_to_object_handler)};

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetObjectByIDBaseImp(ManagedObjectID object_ID,
                                     HandlerType& handler,
                                     ManagedObjectIsNotSmartPoint) const {
    typename BaseIDToObjectContainer ::HandlerType ID_to_object_handler;
    std::vector<typename BaseNameToIDContainer::HandlerType> name_ID_handlers;

    MM_CHECK_WITHOUT_LOG(
        ID_to_object_container_.GetObject(object_ID, ID_to_object_handler),
        return MM_RESULT_CODE;)
    MM_CHECK_WITHOUT_LOG(
        name_to_ID_container_.GetObject(
            ID_to_object_handler.GetObject().GetObjectName(), name_ID_handlers),
        return MM_RESULT_CODE;)

    for (auto& name_ID_handler : name_ID_handlers) {
      if (name_ID_handler.GetObject() == object_ID) {
        handler = HandlerType{std::move(name_ID_handler),
                              std::move(ID_to_object_handler)};

        return ExecuteResult ::SUCCESS;
      }
    }

    assert(false);
    return ExecuteResult ::UNDEFINED_ERROR;
  }

  ExecuteResult GetObjectByIDBaseImp(ManagedObjectID object_ID,
                                     HandlerType& handler,
                                     ManagedObjectIsSmartPoint) const {
    typename BaseIDToObjectContainer ::HandlerType ID_to_object_handler;
    std::vector<typename BaseNameToIDContainer::HandlerType> name_ID_handlers;

    MM_CHECK_WITHOUT_LOG(
        ID_to_object_container_.GetObject(object_ID, ID_to_object_handler),
        return MM_RESULT_CODE;)
    MM_CHECK_WITHOUT_LOG(name_to_ID_container_.GetObject(
                             ID_to_object_handler.GetObject()->GetObjectName(),
                             name_ID_handlers),
                         return MM_RESULT_CODE;)

    for (auto& name_ID_handler : name_ID_handlers) {
      if (name_ID_handler.GetObject() == object_ID) {
        handler = HandlerType{std::move(name_ID_handler),
                              std::move(ID_to_object_handler)};

        return ExecuteResult ::SUCCESS;
      }
    }

    assert(false);
    return ExecuteResult ::UNDEFINED_ERROR;
  }

 private:
  BaseNameToIDContainer name_to_ID_container_{};
  BaseIDToObjectContainer ID_to_object_container_{};
};

template <typename ManagedType, typename ManagedTypeIsSmartPointType>
struct ManagedBaseValidate;

template <typename ManagedType>
struct ManagedBaseValidate<ManagedType, ManagedObjectIsSmartPoint> {
  using Type = std::enable_if_t<
      std::is_base_of_v<ManagedObjectBase, typename ManagedType::element_type>,
      ManagerBaseImp<ManagedType, ManagedObjectIsSmartPoint>>;
};

template <typename ManagedType>
struct ManagedBaseValidate<ManagedType, ManagedObjectIsNotSmartPoint> {
  using Type = std::enable_if_t<
      std::is_base_of_v<ManagedObjectBase, ManagedType>,
      ManagerBaseImp<ManagedType, ManagedObjectIsNotSmartPoint>>;
  ;
};

template <typename ManagedType,
          typename ManagedTypeIsSmartPointType = ManagedObjectIsNotSmartPoint>
using ManagerBase =
    typename ManagedBaseValidate<ManagedType,
                                 ManagedTypeIsSmartPointType>::Type;
};  // namespace Manager
}  // namespace MM
