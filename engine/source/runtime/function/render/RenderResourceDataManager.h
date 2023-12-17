#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/ManagedObjectUnorderedMap.h"
#include "runtime/core/manager/ManagerBase.h"
#include "runtime/function/render/AllocatedBuffer.h"
#include "runtime/function/render/AllocatedImage.h"
#include "runtime/function/render/RenderResourceDataBase.h"
#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/pre_header.h"
#include "utils/error.h"

namespace MM {
namespace RenderSystem {
template <typename ManagedType>
class RenderResourceDataManagerImp : public Manager::ManagerBase<ManagedType> {
 public:
  class RenderResourceHandler;

  using BaseManagerType = Manager::ManagerBase<ManagedType>;
  using HandlerType = RenderResourceHandler;
  using BaseHandlerType = typename BaseManagerType ::HandlerType;
  using RenderResourceDataIDToObjectIDContainerType =
      Manager::ManagedObjectUnorderedMultiMap<RenderResourceDataID,
                                              Manager::ManagedObjectID>;

 public:
  RenderResourceDataManagerImp() = default;
  ~RenderResourceDataManagerImp() = default;
  explicit RenderResourceDataManagerImp(std::uint64_t size)
      : Manager::ManagerBase<std::unique_ptr<RenderResourceDataBase>,
                             Manager::ManagedObjectIsSmartPoint>(size),
        render_resource_data_ID_to_object_ID_(size) {}
  RenderResourceDataManagerImp(const RenderResourceDataManagerImp& other) =
      delete;
  RenderResourceDataManagerImp(RenderResourceDataManagerImp&& other) noexcept =
      delete;
  RenderResourceDataManagerImp& operator=(
      const RenderResourceDataManagerImp& other) = delete;
  RenderResourceDataManagerImp& operator=(
      RenderResourceDataManagerImp&& other) noexcept = delete;

 public:
  class RenderResourceHandler : public BaseHandlerType {
   public:
    RenderResourceHandler() = default;
    ~RenderResourceHandler() = default;
    RenderResourceHandler(
        BaseHandlerType&& base_handler,
        RenderResourceDataIDToObjectIDContainerType::HandlerType&&
            render_resource_ID_to_object_ID_handler)
        : BaseHandlerType(std::move(base_handler)),
          render_resource_ID_to_object_ID_handler_(
              std::move(render_resource_ID_to_object_ID_handler)) {}
    RenderResourceHandler(const RenderResourceHandler& other) = default;
    RenderResourceHandler(RenderResourceHandler&& other) noexcept = default;
    RenderResourceHandler& operator=(const RenderResourceHandler& other) {
      if (&other == this) {
        return *this;
      }

      BaseHandlerType ::operator=(other);
      render_resource_ID_to_object_ID_handler_ =
          other.render_resource_ID_to_object_ID_handler_;

      return *this;
    }
    RenderResourceHandler& operator=(RenderResourceHandler&& other) noexcept {
      if (&other == this) {
        return *this;
      }

      BaseHandlerType ::operator=(std::move(other));
      render_resource_ID_to_object_ID_handler_ =
          std::move(other.render_resource_ID_to_object_ID_handler_);

      return *this;
    }

   public:
    bool IsValid() const override {
      return BaseHandlerType ::IsValid() &&
             render_resource_ID_to_object_ID_handler_.IsValid();
    }

    std::uint32_t GetUseCount() {
      return BaseManagerType::GetIDToObjectHandler().GetUseCount();
    }

    const RenderResourceDataID& GetRenderResourceDataID() const {
      return render_resource_ID_to_object_ID_handler_.GetKey();
    }

    const std::string& GetRenderResourceDataName() const {
      return BaseManagerType::GetObjectName();
    }

    RenderResourceDataBase& GetRenderResourceData() {
      return BaseManagerType::GetObject();
    }

    const RenderResourceDataBase& GetRenderResourceData() const {
      return BaseManagerType::GetObject();
    }

    RenderResourceDataBase* GetRenderResourceDataPtr() {
      return &BaseManagerType::GetObject();
    }

    const RenderResourceDataBase* GetRenderResourceDataPtr() const {
      return &BaseManagerType::GetObject();
    }

    void Release() override {
      render_resource_ID_to_object_ID_handler_.Release();
      BaseHandlerType ::Release();
    }

    typename RenderResourceDataIDToObjectIDContainerType ::HandlerType&
    GetRenderResourceIDToObjectIDHandler() {
      return render_resource_ID_to_object_ID_handler_;
    }

    const typename RenderResourceDataIDToObjectIDContainerType ::HandlerType&
    GetRenderResourceIDToObjectIDHandler() const {
      return render_resource_ID_to_object_ID_handler_;
    }

   private:
    RenderResourceDataIDToObjectIDContainerType ::HandlerType
        render_resource_ID_to_object_ID_handler_;
  };

 public:
  bool IsValid() const override {
    return render_resource_data_ID_to_object_ID_.IsValid() &&
           BaseManagerType::IsValid();
  }

  bool Have(const RenderResourceDataID& render_resource_data_ID) {
    return render_resource_data_ID_to_object_ID_.Have(render_resource_data_ID);
  }

  Result<HandlerType> AddRenderResourceData(
      ManagedType&& render_resource_data) {
    RenderResourceDataID render_resource_data_ID =
        render_resource_data->GetRenderResourceDataID();

    Result<BaseHandlerType> base_handler_result =
        AddObjectBase(std::move(render_resource_data));
    if (base_handler_result.IgnoreException().IsError()) {
      return ResultE{base_handler_result.GetError()};
    }
    BaseHandlerType& base_handler = base_handler_result.GetResult();

    Result<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_ID_to_object_ID_handler_result =
            render_resource_data_ID_to_object_ID_.AddObject(
                render_resource_data_ID, base_handler.GetObjectID());
    if (render_resource_ID_to_object_ID_handler_result.IgnoreException()
            .IsError()) {
      return ResultE{render_resource_ID_to_object_ID_handler_result.GetError()};
    }
    RenderResourceDataIDToObjectIDContainerType ::HandlerType&
        render_resource_ID_to_object_ID_handler =
            render_resource_ID_to_object_ID_handler_result.GetResult();

    HandlerType handler =
        HandlerType{std::move(base_handler),
                    std::move(render_resource_ID_to_object_ID_handler)};

    handler.GetObject().MarkThisIsManaged();

    return ResultS{std::move(handler)};
  }

  Result<HandlerType> GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID) const {
    return GetRenderResourceDataByID(object_ID, StaticTrait::read_and_write);
  }

  Result<HandlerType> GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID,
      typename StaticTrait::ReadAndWriteType) const {
    Result<BaseHandlerType> base_handler_result =
        BaseManagerType::GetObjectByIDBase(object_ID);
    if (base_handler_result.IgnoreException().IsError()) {
      return ResultE{base_handler_result.GetError()};
    }
    BaseHandlerType& base_handler = base_handler_result.GetResult();

    Result<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handler_result =
            render_resource_data_ID_to_object_ID_.GetObject(
                base_handler.GetObject()->GetRenderResourceDataID());
    if (render_resource_data_ID_to_object_ID_handler_result.IgnoreException()
            .IsError()) {
      return ResultE<>{
          render_resource_data_ID_to_object_ID_handler_result.GetError()
              .GetErrorCode()};
    }
    RenderResourceDataIDToObjectIDContainerType ::HandlerType&
        render_resource_data_ID_to_object_ID_handler =
            render_resource_data_ID_to_object_ID_handler_result.GetResult();

    return ResultS{std::move(base_handler),
                   std::move(render_resource_data_ID_to_object_ID_handler)};
  }

  Result<HandlerType> GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID,
      typename StaticTrait::OnlyWriteType) const {
    Result<BaseHandlerType> base_handler_result =
        BaseManagerType::GetObjectByIDBase(object_ID);
    if (base_handler_result.IgnoreException().IsError()) {
      return ResultE{base_handler_result.GetError()};
    }
    BaseHandlerType& base_handler = base_handler_result.GetResult();

    if (!base_handler.GetObject()->IsUseForWrite()) {
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }

    Result<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handler_result =
            render_resource_data_ID_to_object_ID_.GetObject(
                base_handler.GetObject()->GetRenderResourceDataID());
    if (render_resource_data_ID_to_object_ID_handler_result.IgnoreException()
            .IsError()) {
      return ResultE{
          render_resource_data_ID_to_object_ID_handler_result.GetError()};
    }
    RenderResourceDataIDToObjectIDContainerType ::HandlerType&
        render_resource_data_ID_to_object_ID_handler =
            render_resource_data_ID_to_object_ID_handler_result.GetResult();

    return ResultS{std::move(base_handler),
                   std::move(render_resource_data_ID_to_object_ID_handler)};
  }

  Result<HandlerType> GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID,
      typename StaticTrait::OnlyReadType) const {
    Result<BaseHandlerType> base_handler_result =
        BaseManagerType::GetObjectByIDBase(object_ID);
    if (base_handler_result.IgnoreException().IsError()) {
      return ResultE{base_handler_result.GetError()};
    }
    BaseHandlerType& base_handler = base_handler_result.GetResult();

    if (base_handler.GetObject()->IsUseForWrite()) {
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }

    Result<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handler_result =
            render_resource_data_ID_to_object_ID_.GetObject(
                base_handler.GetObject()->GetRenderResourceDataID());
    if (render_resource_data_ID_to_object_ID_handler_result.IgnoreException()
            .IsError()) {
      return ResultE{
          render_resource_data_ID_to_object_ID_handler_result.GetError()};
    }
    RenderResourceDataIDToObjectIDContainerType ::HandlerType&
        render_resource_data_ID_to_object_ID_handler =
            render_resource_data_ID_to_object_ID_handler_result.GetResult();

    return ResultS{std::move(base_handler),
                   std::move(render_resource_data_ID_to_object_ID_handler)};
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID) const {
    return GetRenderResourceDataByRenderResourceDataID(
        render_resource_data_ID, StaticTrait::read_and_write);
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::ReadAndWriteType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers_result =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, st_get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers_result.IgnoreException()
            .IsError()) {
      return ResultE{
          render_resource_data_ID_to_object_ID_handlers_result.GetError()};
    }
    std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>&
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_handlers_result.GetResult();

    std::vector<BaseHandlerType> base_handlers{};
    base_handlers.reserve(render_resource_data_ID_to_object_ID_handlers.size());
    for (std::uint64_t i = 0;
         i != render_resource_data_ID_to_object_ID_handlers.size(); ++i) {
      Result<BaseHandlerType> base_handler_result =
          BaseManagerType::GetObjectByIDBase(
              render_resource_data_ID_to_object_ID_handlers[i].GetObject());
      if (base_handler_result.IgnoreException().IsError()) {
        return ResultE{base_handler_result.GetError()};
      }
      base_handlers.emplace_back(std::move(base_handler_result.GetResult()));
    }

    std::vector<HandlerType> handlers{};
    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      handlers.emplace_back(
          std::move(base_handlers[i]),
          std::move(render_resource_data_ID_to_object_ID_handlers[i]));
    }

    return ResultS{std::move(handlers)};
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::OnlyWriteType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers_result =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, st_get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers_result.IgnoreException()
            .IsError()) {
      return ResultE{
          render_resource_data_ID_to_object_ID_handlers_result.GetError()};
    }
    std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>&
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_handlers_result.GetResult();

    std::vector<BaseHandlerType> base_handlers{};
    base_handlers.reserve(render_resource_data_ID_to_object_ID_handlers.size());
    for (std::uint64_t i = 0;
         i != render_resource_data_ID_to_object_ID_handlers.size(); ++i) {
      Result<BaseHandlerType> base_handler_result =
          BaseManagerType::GetObjectByIDBase(
              render_resource_data_ID_to_object_ID_handlers[i].GetObject());
      if (base_handler_result.IgnoreException().IsError()) {
        return ResultE{std::move(base_handler_result.GetError())};
      }
      if (!base_handler_result.GetResult().IsUseForWrite()) {
        continue;
      }
      base_handlers.emplace_back(std::move(base_handler_result.GetResult()));
    }

    if (base_handlers.empty()) {
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }

    std::vector<HandlerType> handlers{};
    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      if (base_handlers[i].GetObject()->IsUseForWrite()) {
        handlers.emplace_back(
            std::move(base_handlers[i]),
            std::move(render_resource_data_ID_to_object_ID_handlers[i]));
      }
    }

    return ResultS{std::move(handlers)};
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::OnlyReadType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers_result =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, st_get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers_result.IgnoreException()
            .IsError()) {
      return ResultE{
          render_resource_data_ID_to_object_ID_handlers_result.GetError()};
    }
    std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>&
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_handlers_result.GetResult();

    std::vector<BaseHandlerType> base_handlers{};
    base_handlers.reserve(render_resource_data_ID_to_object_ID_handlers.size());
    for (std::uint64_t i = 0;
         i != render_resource_data_ID_to_object_ID_handlers.size(); ++i) {
      Result<BaseHandlerType> base_handler_result =
          BaseManagerType::GetObjectByIDBase(
              render_resource_data_ID_to_object_ID_handlers[i].GetObject());
      if (base_handler_result.IgnoreException().IsError()) {
        return ResultE{base_handler_result.GetError()};
      }
      if (base_handler_result.GetResult().IsUseForWrite()) {
        continue;
      }
      base_handlers.emplace_back(std::move(base_handler_result.GetResult()));
    }

    if (base_handlers.empty()) {
      return ResultE{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }

    std::vector<HandlerType> handlers{};
    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      if (!(base_handlers[i].GetObject()->IsUseForWrite())) {
        handlers.emplace_back(
            std::move(base_handlers[i]),
            std::move(render_resource_data_ID_to_object_ID_handlers[i]));
      }
    }

    return ResultS{std::move(handlers)};
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByName(
      const std::string& name) const {
    return GetRenderResourceDataByName(name, StaticTrait::read_and_write);
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByName(
      const std::string& name, typename StaticTrait::ReadAndWriteType) const {
    Result<std::vector<BaseHandlerType>> base_handlers_result =
        BaseManagerType ::GetObjectByNameBase(name);
    if (base_handlers_result.IgnoreException().IsError()) {
      return ResultE{base_handlers_result.GetError()};
    }

    std::vector<HandlerType> handlers{};
    for (auto& base_handler : base_handlers_result.GetResult()) {
      Result<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
          render_resource_data_ID_to_object_ID_handler_result =
              render_resource_data_ID_to_object_ID_.GetObject(
                  base_handler.GetObject()->GetRenderResourceDataID(),
                  base_handler.GetObjectUseCountPtr());
      if (render_resource_data_ID_to_object_ID_handler_result.IgnoreException()
              .IsError()) {
        continue;
      }
      handlers.emplace_back(
          std::move(base_handler),
          std::move(
              render_resource_data_ID_to_object_ID_handler_result.GetResult()));
    }

    if (!handlers.empty()) {
      return ResultS{std::move(handlers)};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByName(
      const std::string& name, typename StaticTrait::OnlyWriteType) const {
    Result<std::vector<BaseHandlerType>> base_handlers =
        BaseManagerType ::GetObjectByNameBase(name);
    if (base_handlers.IgnoreException().IsError()) {
      return ResultE{base_handlers.GetError()};
    }

    std::vector<HandlerType> handlers{};
    for (auto& base_handler : base_handlers.GetResult()) {
      if (base_handler.GetObject()->IsUseForWrite()) {
        Result<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
            render_resource_data_ID_to_object_ID_handler =
                render_resource_data_ID_to_object_ID_.GetObject(
                    base_handler.GetObject()->GetRenderResourceDataID(),
                    base_handler.GetObjectUseCountPtr());
        if (render_resource_data_ID_to_object_ID_handler.IgnoreException()
                .IsError()) {
          continue;
        }
        handlers.emplace_back(
            std::move(base_handler),
            std::move(
                render_resource_data_ID_to_object_ID_handler.GetResult()));
      }
    }

    if (!handlers.empty()) {
      return ResultS{std::move(handlers)};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<std::vector<HandlerType>> GetRenderResourceDataByName(
      const std::string& name, typename StaticTrait::OnlyReadType) const {
    Result<std::vector<BaseHandlerType>> base_handlers;
    GetObjectByNameBase(name, base_handlers);
    if (base_handlers.IgnoreException().IsError()) {
      return ResultE{base_handlers.GetError()};
    }

    std::vector<HandlerType> handlers;
    for (auto& base_handler : base_handlers.GetResult()) {
      if (!base_handler.GetObject()->IsUseForWrite()) {
        Result<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
            render_resource_data_ID_to_object_ID_handler =
                render_resource_data_ID_to_object_ID_.GetObject(
                    base_handler.GetObject()->GetRenderResourceDataID(),
                    base_handler.GetObjectUseCountPtr());
        if (render_resource_data_ID_to_object_ID_handler.IgnoreException()
                .IsError()) {
          continue;
        }
        handlers.emplace_back(
            std::move(base_handler),
            std::move(
                render_resource_data_ID_to_object_ID_handler.GetResult()));
      }
    }

    if (!handlers.empty()) {
      return ResultS{std::move(handlers)};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<std::vector<Manager::ManagedObjectID>> GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID) const {
    return GetIDByRenderResourceDataID(render_resource_data_ID,
                                       StaticTrait::read_and_write);
  }

  Result<std::vector<Manager::ManagedObjectID>> GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::ReadAndWriteType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, StaticTrait::get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers.IgnoreException()
            .IsError()) {
      return ResultE{render_resource_data_ID_to_object_ID_handlers.GetError()};
    }

    std::vector<Manager::ManagedObjectID> managed_object_IDs{};
    managed_object_IDs.reserve(
        render_resource_data_ID_to_object_ID_handlers.GetResult().size());
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers.GetResult()) {
      managed_object_IDs.emplace_back(
          render_resource_data_ID_to_object_ID_handler.GetObject());
    }

    return ResultS{std::move(managed_object_IDs)};
  }

  Result<std::vector<Manager::ManagedObjectID>> GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::OnlyWriteType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, StaticTrait::get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers.IgnoreException()
            .IsError()) {
      return ResultE{render_resource_data_ID_to_object_ID_handlers.GetError()};
    }

    std::vector<Manager::ManagedObjectID> managed_object_IDs{};
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers.GetResult()) {
      Result<typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType>
          ID_to_object_handler = BaseManagerType ::GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject());
      if (ID_to_object_handler.IgnoreException().IsError()) {
        continue;
      }
      if (ID_to_object_handler.GetResult().GetObject()->IsUseForWrite()) {
        managed_object_IDs.emplace_back(
            render_resource_data_ID_to_object_ID_handler.GetObject());
      }
    }

    if (!managed_object_IDs.empty()) {
      return ResultS{std::move(managed_object_IDs)};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<std::vector<Manager::ManagedObjectID>> GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::OnlyReadType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, StaticTrait::get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers.IgnoreException()
            .IsError()) {
      return ResultE{render_resource_data_ID_to_object_ID_handlers.GetError()};
    }

    std::vector<Manager::ManagedObjectID> managed_object_IDs{};
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers.GetResult()) {
      Result<typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType>
          ID_to_object_handler = BaseManagerType ::GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject());
      if (ID_to_object_handler.IgnoreException().IsError()) {
        continue;
      }

      if (!ID_to_object_handler.GetResult().GetObject()->IsUseForWrite()) {
        managed_object_IDs.emplace_back(
            render_resource_data_ID_to_object_ID_handler.GetObject());
      }
    }

    if (!managed_object_IDs.empty()) {
      return ResultS{std::move(managed_object_IDs)};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<std::vector<std::string>> GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID) const {
    return GetNameByRenderResourceID(render_resource_data_ID,
                                     StaticTrait::read_and_write);
  }

  Result<std::vector<std::string>> GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::ReadAndWriteType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, StaticTrait::get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers.IgnoreException()
            .IsError()) {
      return ResultE{render_resource_data_ID_to_object_ID_handlers.GetError()};
    }
    std::vector<std::string> names{};
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers.GetResult()) {
      Result<typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType>
          ID_to_object_handler = BaseManagerType ::GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject());
      ID_to_object_handler.IgnoreException();
      assert(ID_to_object_handler.IsSuccess());
      names.emplace_back(
          ID_to_object_handler.GetResult().GetObject()->GetObjectName());
    }

    return ResultS{std::move(names)};
  }

  Result<std::vector<std::string>> GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::OnlyWriteType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, StaticTrait::get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers.IgnoreException()
            .IsError()) {
      return ResultE{render_resource_data_ID_to_object_ID_handlers.GetError()};
    }

    std::vector<std::string> names{};
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers.GetResult()) {
      Result<typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType>
          ID_to_object_handler = BaseManagerType ::GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject());
      ID_to_object_handler.IgnoreException();
      assert(ID_to_object_handler.IsSuccess());
      if (ID_to_object_handler.GetResult().GetObject()->IsUseForWrite()) {
        names.emplace_back(
            ID_to_object_handler.GetResult().GetObject()->GetObjectName());
      }
    }

    if (names.empty()) {
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }

    return ResultS{std::move(names)};
  }

  Result<std::vector<std::string>> GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      typename StaticTrait::OnlyReadType) const {
    Result<
        std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>>
        render_resource_data_ID_to_object_ID_handlers =
            render_resource_data_ID_to_object_ID_.GetObject(
                render_resource_data_ID, StaticTrait::get_multiply_object);
    if (render_resource_data_ID_to_object_ID_handlers.IgnoreException()
            .IsError()) {
      return ResultE{render_resource_data_ID_to_object_ID_handlers.GetError()};
    }

    std::vector<std::string> names{};
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers.GetResult()) {
      Result<typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType>
          ID_to_object_handler = BaseManagerType ::GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject());
      ID_to_object_handler.IgnoreException();
      assert(ID_to_object_handler.IsSuccess());
      if (!ID_to_object_handler.GetResult().GetObject()->IsUseForWrite()) {
        names.emplace_back(
            ID_to_object_handler.GetResult().GetObject()->GetObjectName());
      }
    }

    if (names.empty()) {
      return ResultE<>{
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
    }

    return ResultS{std::move(names)};
  }

  Result<RenderResourceDataID> GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID) const {
    return GetRenderResourceDataIDByID(managed_object_ID,
                                       StaticTrait::read_and_write);
  }

  Result<RenderResourceDataID> GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      typename StaticTrait::ReadAndWriteType) const {
    Result<typename BaseManagerType ::BaseIDToObjectContainer::HandlerType>
        ID_to_object_handler =
            BaseManagerType ::GetIDToObjectHandler(managed_object_ID);
    if (ID_to_object_handler.IgnoreException().IsError()) {
      return ResultE{ID_to_object_handler.GetError()};
    }

    return ResultS{ID_to_object_handler.GetObject()->GetRenderResourceDataID()};
  }

  Result<RenderResourceDataID> GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      typename StaticTrait::OnlyWriteType) const {
    Result<typename BaseManagerType ::BaseIDToObjectContainer::HandlerType>
        ID_to_object_handler =
            BaseManagerType ::GetIDToObjectHandler(managed_object_ID);
    if (ID_to_object_handler.IgnoreException().IsError()) {
      return ResultE{ID_to_object_handler.GetError()};
    }

    if (ID_to_object_handler.GetResult().GetObject()->IsUseForWrite()) {
      return ResultS{
          ID_to_object_handler.GetObject()->GetRenderResourceDataID()};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<RenderResourceDataID> GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      typename StaticTrait::OnlyReadType) const {
    Result<typename BaseManagerType ::BaseIDToObjectContainer::HandlerType>
        ID_to_object_handler =
            BaseManagerType ::GetIDToObjectHandler(managed_object_ID);
    if (ID_to_object_handler.IgnoreException().IsError()) {
      return ResultE{ID_to_object_handler.GetError()};
    }

    if (!ID_to_object_handler.GetResult().GetObject()->IsUseForWrite()) {
      return ResultS{
          ID_to_object_handler.GetObject()->GetRenderResourceDataID()};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<std::vector<RenderResourceDataID>> GetRenderResourceDataIDByName(
      const std::string& name) const {
    return GetRenderResourceDataIDByName(name, StaticTrait::read_and_write);
  }

  Result<std::vector<RenderResourceDataID>> GetRenderResourceDataIDByName(
      const std::string& name, typename StaticTrait::ReadAndWriteType) const {
    Result<std::vector<BaseHandlerType>> base_handlers =
        BaseManagerType ::GetObjectByNameBase(name,
                                              StaticTrait::get_multiply_object);
    if (base_handlers.IgnoreException().IsError()) {
      return ResultE{base_handlers.GetError()};
    }

    std::vector<RenderResourceDataID> render_resource_data_IDs;
    for (const auto& base_handler : base_handlers.GetResult()) {
      render_resource_data_IDs.emplace_back(
          base_handler.GetObject()->GetRenderResourceDataID());
    }

    return ResultS{std::move(render_resource_data_IDs)};
  }

  Result<std::vector<RenderResourceDataID>> GetRenderResourceDataIDByName(
      const std::string& name, typename StaticTrait::OnlyWriteType) const {
    Result<std::vector<BaseHandlerType>> base_handlers =
        BaseManagerType ::GetObjectByNameBase(name,
                                              StaticTrait::get_multiply_object);
    if (base_handlers.IgnoreException().IsError()) {
      return ResultE{base_handlers.GetError()};
    }

    std::vector<RenderResourceDataID> render_resource_data_IDs{};
    for (const auto& base_handler : base_handlers.GetResult()) {
      if (base_handler.GetObject()->IsUseForWrite()) {
        render_resource_data_IDs.emplace_back(
            base_handler.GetObject()->GetRenderResourceDataID());
      }
    }

    if (!render_resource_data_IDs.empty()) {
      return ResultS{std::move(render_resource_data_IDs)};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

  Result<std::vector<RenderResourceDataID>> GetRenderResourceDataIDByName(
      const std::string& name, typename StaticTrait::OnlyReadType) const {
    Result<std::vector<BaseHandlerType>> base_handlers =
        BaseManagerType ::GetObjectByNameBase(name,
                                              StaticTrait::get_multiply_object);
    if (base_handlers.IgnoreException().IsError()) {
      return ResultE{base_handlers.GetError()};
    }

    std::vector<RenderResourceDataID> render_resource_data_IDs{};
    for (const auto& base_handler : base_handlers.GetResult()) {
      if (!base_handler.GetObject()->IsUseForWrite()) {
        render_resource_data_IDs.emplace_back(
            base_handler.GetObject()->GetRenderResourceDataID());
      }
    }

    if (!render_resource_data_IDs.empty()) {
      return ResultS{std::move(render_resource_data_IDs)};
    }

    return ResultE<>{
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT};
  }

 private:
  RenderResourceDataIDToObjectIDContainerType
      render_resource_data_ID_to_object_ID_{};
};

template <typename ManagedType,
          typename IsHaveBaseTypeOfRenderResourceDataBase = std::enable_if_t<
              std::is_base_of_v<RenderResourceDataBase, ManagedType>, void>,
          typename IsHaveDefaultConstructor = std::enable_if_t<
              std::is_default_constructible_v<ManagedType>, void>>
struct RenderResourceDataManagerValidate {
  using Type = RenderResourceDataManagerImp<ManagedType>;
};

template <typename ManagedType>
using RenderResourceDataManager =
    typename RenderResourceDataManagerValidate<ManagedType>::Type;

using RenderBufferDataManager = RenderResourceDataManager<AllocatedBuffer>;
using RenderImageDataManager = RenderResourceDataManager<AllocatedImage>;
}  // namespace RenderSystem
}  // namespace MM
