#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/ManagedObjectUnorderedMap.h"

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
    virtual ~BaseHandler() = default;
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
    return ID_to_object_container_.GetSize(object_ID);
  }

  Result<std::string, ErrorResult> GetNameByID(
      ManagedObjectID managed_object_ID) const {
    return GetNameByIDImp(managed_object_ID, ManagedTypeIsSmartPointType{});
  }

  Result<std::vector<ManagedObjectID>, ErrorResult> GetIDByName(
      const std::string& object_name,
      MM::StaticTrait::GetMultiplyObject) const {
    auto name_to_ID_handlers =
        name_to_ID_container_.GetObject(object_name, st_get_multiply_object)
            .Exception();
    if (!name_to_ID_handlers.IsSuccess()) {
      return Result<std::vector<ManagedObjectID>, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    std::vector<ManagedObjectID> IDs;
    IDs.reserve(name_to_ID_handlers.GetResult().size());
    for (const auto& handler : name_to_ID_handlers.GetResult()) {
      IDs.emplace_back(handler.GetObject());
    }

    return Result<std::vector<ManagedObjectID>, ErrorResult>(st_execute_success,
                                                             std::move(IDs));
  }

  Result<ManagedObjectID, ErrorResult> GetIDByName(
      const std::string& object_name, MM::StaticTrait::GetOneObject) const {
    auto name_to_ID_handler =
        name_to_ID_container_.GetObject(object_name, st_get_one_object)
            .Exception();
    if (!name_to_ID_handler.IsSuccess()) {
      return Result<ManagedObjectID, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    return Result<ManagedObjectID, ErrorResult>(
        st_execute_success, name_to_ID_handler.GetResult().GetObject());
  }

  Result<ManagedObjectID, ErrorResult> GetIDByName(
      const std::string& object_name) const {
    return GetIDByName(object_name, MM::st_get_one_object);
  }

  void Reserve(std::uint64_t name_to_ID_size, std::uint64_t ID_to_object_size) {
    name_to_ID_container_.Reserve(name_to_ID_size);
    ID_to_object_container_.Reserve(ID_to_object_size);
  }

 protected:
  Result<typename BaseIDToObjectContainer::HandlerType, ErrorResult>
  GetIDToObjectHandler(ManagedObjectID object_ID) const {
    return ID_to_object_container_.GetObject(object_ID);
  }

  Result<HandlerType, ErrorResult> AddObjectBase(ManagedType&& managed_object) {
    return AddObjectBaseImp(std::move(managed_object),
                            ManagedTypeIsSmartPointType{});
  }

  Result<HandlerType, ErrorResult> GetObjectByIDBase(
      ManagedObjectID object_ID) const {
    return GetObjectByIDBaseImp(object_ID, ManagedTypeIsSmartPointType{});
  }

  Result<HandlerType, ErrorResult> GetObjectByNameBase(
      const std::string& object_name, StaticTrait::GetOneObject) const {
    auto name_ID_handler =
        name_to_ID_container_.GetObject(object_name, st_get_one_object)
            .Exception();
    if (!name_ID_handler.IsSuccess()) {
      return Result<std::vector<HandlerType>, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    auto id_object_handler =
        ID_to_object_container_
            .GetObject(name_ID_handler.GetResult().GetObject())
            .Exception();
    if (!id_object_handler.IsSuccess()) {
      return Result<std::vector<HandlerType>, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    return Result<HandlerType, ErrorResult>(
        st_execute_success,
        HandlerType{std::move(name_ID_handler), std::move(id_object_handler)});
  }

  Result<std::vector<HandlerType>, ErrorResult> GetObjectByNameBase(
      const std::string& object_name, StaticTrait::GetMultiplyObject) const {
    auto name_ID_handlers =
        name_to_ID_container_.GetObject(object_name, st_get_multiply_object)
            .Exception();
    if (!name_ID_handlers.IsSuccess()) {
      return Result<std::vector<HandlerType>, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    std::vector<HandlerType> handlers;
    for (auto& name_ID_handler : name_ID_handlers.GetResult()) {
      auto id_object_handler =
          ID_to_object_container_.GetObject(name_ID_handler.GetObject())
              .Exception();
      if (!id_object_handler.IsSuccess()) {
        continue;
      }

      handlers.emplace_back(std::move(name_ID_handler),
                            std::move(id_object_handler.GetResult()));
    }

    if (handlers.empty()) {
      return Result<std::vector<HandlerType>, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    return Result<std::vector<HandlerType>, ErrorResult>(st_execute_success,
                                                         std::move(handlers));
  }

  Result<HandlerType, ErrorResult> GetObjectByNameBase(
      const std::string& object_name) const {
    return GetObjectByNameBase(object_name, MM::st_get_one_object);
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
  Result<std::string, ErrorResult> GetNameByIDImp(
      ManagedObjectID managed_object_ID, ManagedObjectIsNotSmartPoint) const {
    auto handler =
        ID_to_object_container_.GetObject(managed_object_ID).Exception();
    if (!handler.IsSuccess()) {
      return Result<std::string, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    return Result<std::string, ErrorResult>(
        st_execute_success, handler.GetResult().GetObject().GetObjectName());
  }

  Result<std::string, ErrorResult> GetNameByIDImp(
      ManagedObjectID managed_object_ID, ManagedObjectIsSmartPoint) const {
    auto handler =
        ID_to_object_container_.GetObject(managed_object_ID).Exception();
    if (!handler.IsSuccess()) {
      return Result<std::string, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    return Result<std::string, ErrorResult>(
        st_execute_success, handler.GetResult().GetObject()->GetObjectName());
  }

  Result<HandlerType, ErrorResult> AddObjectBaseImp(
      ManagedType&& managed_object, ManagedObjectIsNotSmartPoint) {
    ManagedObjectID copy_ID = managed_object.GetObjectID();
    auto name_ID_handler =
        name_to_ID_container_
            .AddObject(managed_object.GetObjectName(), std::move(copy_ID))
            .Exception();
    if (!name_ID_handler.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, name_ID_handler.GetError().GetErrorCode());
    }
    auto ID_to_object_handler =
        ID_to_object_container_
            .AddObject(managed_object.GetObjectID(), std::move(managed_object))
            .Exception();
    if (!ID_to_object_handler.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, ID_to_object_handler.GetError().GetErrorCode());
    }

    return Result<HandlerType, ErrorResult>(
        st_execute_success, std::move(name_ID_handler.GetResult()),
        std::move(ID_to_object_handler.GetResult()));
  }

  Result<HandlerType, ErrorResult> AddObjectBaseImp(
      ManagedType&& managed_object, ManagedObjectIsSmartPoint) {
    ManagedObjectID copy_ID = managed_object->GetObjectID();
    auto name_ID_handler =
        name_to_ID_container_
            .AddObject(managed_object->GetObjectName(), std::move(copy_ID))
            .Exception();
    if (!name_ID_handler.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, name_ID_handler.GetError().GetErrorCode());
    }
    auto ID_to_object_handler =
        ID_to_object_container_
            .AddObject(managed_object->GetObjectID(), std::move(managed_object))
            .Exception();
    if (!ID_to_object_handler.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, ID_to_object_handler.GetError().GetErrorCode());
    }

    return Result<HandlerType, ErrorResult>(
        st_execute_success, std::move(name_ID_handler.GetResult()),
        std::move(ID_to_object_handler.GetResult()));
  }

  Result<HandlerType, ErrorResult> GetObjectByIDBaseImp(
      ManagedObjectID object_ID, ManagedObjectIsNotSmartPoint) const {
    auto ID_to_object_handler =
        ID_to_object_container_.GetObject(object_ID).Exception();
    if (!ID_to_object_handler.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, ID_to_object_handler.GetError().GetErrorCode());
    }
    auto name_ID_handlers =
        name_to_ID_container_
            .GetObject(
                ID_to_object_handler.GetResult().GetObject().GetObjectName(),
                st_get_multiply_object)
            .Exception();
    if (!name_ID_handlers.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, name_ID_handlers.GetError().GetErrorCode());
    }

    for (auto& name_ID_handler : name_ID_handlers.GetResult()) {
      if (name_ID_handler.GetObject() == object_ID) {
        return Result<HandlerType, ErrorResult>(
            st_execute_success, std::move(name_ID_handler),
            std::move(ID_to_object_handler.GetResult()));
      }
    }

    assert(false);
    return Result<HandlerType, ErrorResult>(st_execute_error,
                                            ErrorCode::UNDEFINED_ERROR);
  }

  Result<HandlerType, ErrorResult> GetObjectByIDBaseImp(
      ManagedObjectID object_ID, ManagedObjectIsSmartPoint) const {
    auto ID_to_object_handler =
        ID_to_object_container_.GetObject(object_ID).Exception();
    if (!ID_to_object_handler.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, ID_to_object_handler.GetError().GetErrorCode());
    }
    auto name_ID_handlers =
        name_to_ID_container_
            .GetObject(
                ID_to_object_handler.GetResult().GetObject()->GetObjectName(),
                st_get_multiply_object)
            .Exception();
    if (!name_ID_handlers.IsSuccess()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, name_ID_handlers.GetError().GetErrorCode());
    }

    for (auto& name_ID_handler : name_ID_handlers.GetResult()) {
      if (name_ID_handler.GetObject() == object_ID) {
        return Result<HandlerType, ErrorResult>(
            st_execute_success, std::move(name_ID_handler),
            std::move(ID_to_object_handler.GetResult()));
      }
    }

    assert(false);
    return Result<HandlerType, ErrorResult>(st_execute_error,
                                            ErrorCode::UNDEFINED_ERROR);
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
