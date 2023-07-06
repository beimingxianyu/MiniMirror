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
#include "runtime/function/render/import_other_system.h"
#include "runtime/platform/base/error.h"
#include "utils/marco.h"

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

  struct Policy {
    struct OnlyWrittenType {};
    struct OnlyNotWrittenType {};
    struct BothType {};

    static OnlyNotWrittenType only_not_written_;
    static OnlyWrittenType only_written_;
    static BothType both_;
  };

 public:
  bool IsValid() const override {
    return render_resource_data_ID_to_object_ID_.IsValid() &&
           BaseManagerType::IsValid();
  }

  bool Have(const RenderResourceDataID& render_resource_data_ID) {
    return render_resource_data_ID_to_object_ID_.Have(render_resource_data_ID);
  }

  ExecuteResult AddRenderResourceData(ManagedType&& render_resource_data,
                                      HandlerType& handler) {
    RenderResourceDataID render_resource_data_ID =
        render_resource_data->GetRenderResourceDataID();

    BaseHandlerType base_handler;
    MM_CHECK_WITHOUT_LOG(
        AddObjectBase(std::move(render_resource_data), base_handler),
        return MM_RESULT_CODE;)

    RenderResourceDataIDToObjectIDContainerType ::HandlerType
        render_resource_ID_to_object_ID_handler;
    MM_CHECK_WITHOUT_LOG(
        render_resource_data_ID_to_object_ID_.AddObject(
            render_resource_data_ID, base_handler.GetObjectID(),
            render_resource_ID_to_object_ID_handler),
        return MM_RESULT_CODE;)

    handler = HandlerType{std::move(base_handler),
                          std::move(render_resource_ID_to_object_ID_handler)};

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler) const {
    return GetRenderResourceDataByID(object_ID, handler, Policy::both_);
  }

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler,
      typename Policy::BothType) const {
    BaseHandlerType base_handler;
    MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(object_ID, base_handler),
                         return MM_RESULT_CODE;)

    RenderResourceDataIDToObjectIDContainerType ::HandlerType
        render_resource_data_ID_to_object_ID_handler;
    MM_CHECK_WITHOUT_LOG(
        render_resource_data_ID_to_object_ID_.GetObject(
            base_handler.GetObject()->GetRenderResourceDataID(),
            render_resource_data_ID_to_object_ID_handler),
        return MM_RESULT_CODE;)

    handler =
        HandlerType{std::move(base_handler),
                    std::move(render_resource_data_ID_to_object_ID_handler)};

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler,
      typename Policy::OnlyWrittenType) const {
    BaseHandlerType base_handler;
    MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(object_ID, base_handler),
                         return MM_RESULT_CODE;)

    if (!base_handler.GetObject()->IsUseForWrite()) {
      return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    RenderResourceDataIDToObjectIDContainerType ::HandlerType
        render_resource_data_ID_to_object_ID_handler;
    MM_CHECK_WITHOUT_LOG(
        render_resource_data_ID_to_object_ID_.GetObject(
            base_handler.GetObject()->GetRenderResourceDataID(),
            render_resource_data_ID_to_object_ID_handler),
        return MM_RESULT_CODE;)

    handler =
        HandlerType{std::move(base_handler),
                    std::move(render_resource_data_ID_to_object_ID_handler)};

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler,
      typename Policy::OnlyNotWrittenType) const {
    BaseHandlerType base_handler;
    MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(object_ID, base_handler),
                         return MM_RESULT_CODE;)

    if (base_handler.GetObject()->IsUseForWrite()) {
      return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    RenderResourceDataIDToObjectIDContainerType ::HandlerType
        render_resource_data_ID_to_object_ID_handler;
    MM_CHECK_WITHOUT_LOG(
        render_resource_data_ID_to_object_ID_.GetObject(
            base_handler.GetObject()->GetRenderResourceDataID(),
            render_resource_data_ID_to_object_ID_handler),
        return MM_RESULT_CODE;)

    handler =
        HandlerType{std::move(base_handler),
                    std::move(render_resource_data_ID_to_object_ID_handler)};

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers) const {
    return GetRenderResourceDataByRenderResourceDataID(render_resource_data_ID,
                                                       handlers, Policy::both_);
  }

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers, typename Policy::BothType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    std::vector<BaseHandlerType> base_handlers(
        render_resource_data_ID_to_object_ID_handlers.size());
    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      MM_CHECK_WITHOUT_LOG(
          GetObjectByIDBase(
              render_resource_data_ID_to_object_ID_handlers[i].GetObject(),
              base_handlers[i]),
          return MM_RESULT_CODE;)
    }

    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      handlers.emplace_back(
          std::move(base_handlers[i]),
          std::move(render_resource_data_ID_to_object_ID_handlers[i]));
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers,
      typename Policy::OnlyWrittenType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    std::vector<BaseHandlerType> base_handlers(
        render_resource_data_ID_to_object_ID_handlers.size());
    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      MM_CHECK_WITHOUT_LOG(
          GetObjectByIDBase(
              render_resource_data_ID_to_object_ID_handlers[i].GetObject(),
              base_handlers[i]),
          return MM_RESULT_CODE;)
    }

    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      if (base_handlers[i].GetObject()->IsUseForWrite()) {
        handlers.emplace_back(
            std::move(base_handlers[i]),
            std::move(render_resource_data_ID_to_object_ID_handlers[i]));
      }
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers,
      typename Policy::OnlyNotWrittenType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    std::vector<BaseHandlerType> base_handlers(
        render_resource_data_ID_to_object_ID_handlers.size());
    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      MM_CHECK_WITHOUT_LOG(
          GetObjectByIDBase(
              render_resource_data_ID_to_object_ID_handlers[i].GetObject(),
              base_handlers[i]),
          return MM_RESULT_CODE;)
    }

    for (std::uint64_t i = 0; i != base_handlers.size(); ++i) {
      if (!(base_handlers[i].GetObject()->IsUseForWrite())) {
        handlers.emplace_back(
            std::move(base_handlers[i]),
            std::move(render_resource_data_ID_to_object_ID_handlers[i]));
      }
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByName(
      const std::string& name, std::vector<HandlerType>& handlers) const {
    return GetRenderResourceDataByName(name, handlers, Policy::both_);
  }

  ExecuteResult GetRenderResourceDataByName(const std::string& name,
                                            std::vector<HandlerType>& handlers,
                                            typename Policy::BothType) const {
    std::vector<BaseHandlerType> base_handlers;
    MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                         return MM_RESULT_CODE;)

    bool is_add = false;
    for (auto& base_handler : base_handlers) {
      RenderResourceDataIDToObjectIDContainerType ::HandlerType
          render_resource_data_ID_to_object_ID_handler;
      MM_CHECK_WITHOUT_LOG(
          render_resource_data_ID_to_object_ID_.GetObject(
              base_handler.GetObject()->GetRenderResourceDataID(),
              base_handler.GetObjectUseCountPtr(),
              render_resource_data_ID_to_object_ID_handler),
          continue;)
      handlers.emplace_back(
          std::move(base_handler),
          std::move(render_resource_data_ID_to_object_ID_handler));
      is_add = true;
    }

    if (is_add) {
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetRenderResourceDataByName(
      const std::string& name, std::vector<HandlerType>& handlers,
      typename Policy::OnlyWrittenType) const {
    std::vector<BaseHandlerType> base_handlers;
    MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                         return MM_RESULT_CODE;)

    bool is_add = false;
    for (auto& base_handler : base_handlers) {
      if (base_handler.GetObject()->IsUseForWrite()) {
        RenderResourceDataIDToObjectIDContainerType ::HandlerType
            render_resource_data_ID_to_object_ID_handler;
        MM_CHECK_WITHOUT_LOG(
            render_resource_data_ID_to_object_ID_.GetObject(
                base_handler.GetObject()->GetRenderResourceDataID(),
                base_handler.GetObjectUseCountPtr(),
                render_resource_data_ID_to_object_ID_handler),
            continue;)
        handlers.emplace_back(
            std::move(base_handler),
            std::move(render_resource_data_ID_to_object_ID_handler));
        is_add = true;
      }
    }

    if (is_add) {
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetRenderResourceDataByName(
      const std::string& name, std::vector<HandlerType>& handlers,
      typename Policy::OnlyNotWrittenType) const {
    std::vector<BaseHandlerType> base_handlers;
    MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                         return MM_RESULT_CODE;)

    bool is_add = false;
    for (auto& base_handler : base_handlers) {
      if (!base_handler.GetObject()->IsUseForWrite()) {
        RenderResourceDataIDToObjectIDContainerType ::HandlerType
            render_resource_data_ID_to_object_ID_handler;
        MM_CHECK_WITHOUT_LOG(
            render_resource_data_ID_to_object_ID_.GetObject(
                base_handler.GetObject()->GetRenderResourceDataID(),
                base_handler.GetObjectUseCountPtr(),
                render_resource_data_ID_to_object_ID_handler),
            continue;)
        handlers.emplace_back(
            std::move(base_handler),
            std::move(render_resource_data_ID_to_object_ID_handler));
        is_add = true;
      }
    }

    if (is_add) {
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs) const {
    return GetIDByRenderResourceDataID(render_resource_data_ID,
                                       managed_object_IDs, Policy::both_);
  }

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      typename Policy::BothType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    managed_object_IDs.resize(
        managed_object_IDs.size() +
        render_resource_data_ID_to_object_ID_handlers.size());
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers) {
      managed_object_IDs.emplace_back(
          render_resource_data_ID_to_object_ID_handler.GetObject());
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      typename Policy::OnlyWrittenType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    bool is_add{false};
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers) {
      typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType
          ID_to_object_handler;
      MM_CHECK_WITHOUT_LOG(
          GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject(),
              ID_to_object_handler),
          continue;)
      if (ID_to_object_handler.GetObject()->IsUseForWrite()) {
        managed_object_IDs.emplace_back(
            render_resource_data_ID_to_object_ID_handler.GetObject());
        is_add = true;
      }
    }

    if (is_add) {
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      typename Policy::OnlyNotWrittenType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    bool is_add{false};
    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers) {
      typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType
          ID_to_object_handler;
      MM_CHECK_WITHOUT_LOG(
          GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject(),
              ID_to_object_handler),
          continue;)
      if (!ID_to_object_handler.GetObject()->IsUseForWrite()) {
        managed_object_IDs.emplace_back(
            render_resource_data_ID_to_object_ID_handler.GetObject());
        is_add = true;
      }
    }

    if (is_add) {
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names) const {
    return GetNameByRenderResourceID(render_resource_data_ID, names,
                                     Policy::both_);
  }

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, typename Policy::BothType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers) {
      typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType
          ID_to_object_handler;
      MM_CHECK_WITHOUT_LOG(
          GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject(),
              ID_to_object_handler),
          assert(false);)
      names.emplace_back(ID_to_object_handler.GetObject()->GetObjectName());
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, typename Policy::OnlyWrittenType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers) {
      typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType
          ID_to_object_handler;
      MM_CHECK_WITHOUT_LOG(
          GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject(),
              ID_to_object_handler),
          assert(false);)
      if (ID_to_object_handler.GetObject()->IsUseForWrite()) {
        names.emplace_back(ID_to_object_handler.GetObject()->GetObjectName());
      }
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names,
      typename Policy::OnlyNotWrittenType) const {
    std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
        render_resource_data_ID_to_object_ID_handlers;
    MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                             render_resource_data_ID,
                             render_resource_data_ID_to_object_ID_handlers),
                         return MM_RESULT_CODE;)

    for (const auto& render_resource_data_ID_to_object_ID_handler :
         render_resource_data_ID_to_object_ID_handlers) {
      typename BaseManagerType ::BaseIDToObjectContainer ::HandlerType
          ID_to_object_handler;
      MM_CHECK_WITHOUT_LOG(
          GetIDToObjectHandler(
              render_resource_data_ID_to_object_ID_handler.GetObject(),
              ID_to_object_handler),
          assert(false);)
      if (!ID_to_object_handler.GetObject()->IsUseForWrite()) {
        names.emplace_back(ID_to_object_handler.GetObject()->GetObjectName());
      }
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID) const {
    return GetRenderResourceDataIDByID(managed_object_ID,
                                       render_resource_data_ID, Policy::both_);
  }

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID,
      typename Policy::BothType) const {
    typename BaseManagerType ::BaseIDToObjectContainer::HandlerType
        ID_to_object_handler;
    MM_CHECK_WITHOUT_LOG(
        GetIDToObjectHandler(managed_object_ID, ID_to_object_handler),
        return MM_RESULT_CODE;)

    render_resource_data_ID =
        ID_to_object_handler.GetObject()->GetRenderResourceDataID();

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID,
      typename Policy::OnlyWrittenType) const {
    typename BaseManagerType ::BaseIDToObjectContainer::HandlerType
        ID_to_object_handler;
    MM_CHECK_WITHOUT_LOG(
        GetIDToObjectHandler(managed_object_ID, ID_to_object_handler),
        return MM_RESULT_CODE;)

    if (ID_to_object_handler.GetObject()->IsUseForWrite()) {
      render_resource_data_ID =
          ID_to_object_handler.GetObject()->GetRenderResourceDataID();

      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID,
      typename Policy::OnlyNotWrittenType) const {
    typename BaseManagerType ::BaseIDToObjectContainer::HandlerType
        ID_to_object_handler;
    MM_CHECK_WITHOUT_LOG(
        GetIDToObjectHandler(managed_object_ID, ID_to_object_handler),
        return MM_RESULT_CODE;)

    if (!ID_to_object_handler.GetObject()->IsUseForWrite()) {
      render_resource_data_ID =
          ID_to_object_handler.GetObject()->GetRenderResourceDataID();

      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs) const {
    return GetRenderResourceDataIDByName(name, render_resource_data_IDs,
                                         Policy::both_);
  }

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs,
      typename Policy::BothType) const {
    std::vector<BaseHandlerType> base_handlers;
    MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                         return MM_RESULT_CODE;)

    for (const auto& base_handler : base_handlers) {
      render_resource_data_IDs.emplace_back(
          base_handler.GetObject()->GetRenderResourceDataID());
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs,
      typename Policy::OnlyWrittenType) const {
    std::vector<BaseHandlerType> base_handlers;
    MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                         return MM_RESULT_CODE;)

    bool is_add{false};
    for (const auto& base_handler : base_handlers) {
      if (base_handler.GetObject()->IsUseForWrite()) {
        render_resource_data_IDs.emplace_back(
            base_handler.GetObject()->GetRenderResourceDataID());

        is_add = true;
      }
    }

    if (is_add) {
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs,
      typename Policy::OnlyNotWrittenType) const {
    std::vector<BaseHandlerType> base_handlers;
    MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                         return MM_RESULT_CODE;)

    bool is_add{false};
    for (const auto& base_handler : base_handlers) {
      if (!base_handler.GetObject()->IsUseForWrite()) {
        render_resource_data_IDs.emplace_back(
            base_handler.GetObject()->GetRenderResourceDataID());

        is_add = true;
      }
    }

    if (is_add) {
      return ExecuteResult ::SUCCESS;
    }

    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
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
