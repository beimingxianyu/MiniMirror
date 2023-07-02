//
// Created by beimingxianyu on 23-6-27.
//

#include "runtime/function/render/RenderResourceManager.h"

MM::RenderSystem::RenderResourceManager*
    MM::RenderSystem::RenderResourceManager::render_resource_manager_{nullptr};
std::mutex MM::RenderSystem::RenderResourceManager::sync_flag_{};

MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType
    MM::RenderSystem::RenderResourceManager::Policy::only_written_{};
MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType
    MM::RenderSystem::RenderResourceManager::Policy::only_not_written_{};
MM::RenderSystem::RenderResourceManager::Policy::BothType
    MM::RenderSystem::RenderResourceManager::Policy::both_{};

bool MM::RenderSystem::RenderResourceManager::IsValid() const {
  return ManagerBaseImp::IsValid() && render_resource_manager_->IsValid();
}

MM::RenderSystem::RenderResourceManager*
MM::RenderSystem::RenderResourceManager::GetInstance() {
  if (render_resource_manager_) {
  } else {
    std::lock_guard<std::mutex> guard{sync_flag_};
    if (!render_resource_manager_) {
      std::uint64_t asset_size = 0;
      if (CONFIG_SYSTEM->GetConfig("manager_size_render_resource_manager",
                                   asset_size) != ExecuteResult::SUCCESS) {
        LOG_WARN("The number of managed asset was not specified.");
        if (CONFIG_SYSTEM->GetConfig("manager_size", asset_size) !=
            ExecuteResult::SUCCESS) {
          LOG_FATAL("The number of managed objects was not specified.");
        }
      }

      render_resource_manager_ = new RenderResourceManager{};
    }
  }
  return render_resource_manager_;
}

MM::RenderSystem::RenderResourceManager::RenderResourceManager(
    std::uint64_t size)
    : Manager::ManagerBase<std::unique_ptr<RenderResourceDataBase>,
                           Manager::ManagedObjectIsSmartPoint>(size),
      render_resource_data_ID_to_object_ID_(size) {}

bool MM::RenderSystem::RenderResourceManager::Have(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID) {
  return render_resource_data_ID_to_object_ID_.Have(render_resource_data_ID);
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::AddRenderResourceData(
    MM::RenderSystem::RenderResourceManager::ManagedType&& render_resource_data,
    MM::RenderSystem::RenderResourceManager::HandlerType& handler) {
  RenderResourceDataID render_resource_data_ID =
      render_resource_data->GetRenderResourceDataID();

  BaseHandlerType base_handler;
  MM_CHECK_WITHOUT_LOG(
      AddObjectBase(std::move(render_resource_data), base_handler),
      return MM_RESULT_CODE;)

  RenderResourceDataIDToObjectIDContainerType ::HandlerType
      render_resource_ID_to_object_ID_handler;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.AddObject(
                           render_resource_data_ID, base_handler.GetObjectID(),
                           render_resource_ID_to_object_ID_handler),
                       return MM_RESULT_CODE;)

  handler = HandlerType{std::move(base_handler),
                        std::move(render_resource_ID_to_object_ID_handler)};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByID(
    const MM::Manager::ManagedObjectID& object_ID,
    MM::RenderSystem::RenderResourceManager::HandlerType& handler) const {
  return GetRenderResourceDataByID(object_ID, handler, Policy::both_);
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByID(
    const MM::Manager::ManagedObjectID& object_ID,
    MM::RenderSystem::RenderResourceManager::HandlerType& handler,
    MM::RenderSystem::RenderResourceManager::Policy::BothType) const {
  BaseHandlerType base_handler;
  MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(object_ID, base_handler),
                       return MM_RESULT_CODE;)

  RenderResourceDataIDToObjectIDContainerType ::HandlerType
      render_resource_data_ID_to_object_ID_handler;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           base_handler.GetObject()->GetRenderResourceDataID(),
                           render_resource_data_ID_to_object_ID_handler),
                       return MM_RESULT_CODE;)

  handler =
      HandlerType{std::move(base_handler),
                  std::move(render_resource_data_ID_to_object_ID_handler)};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByID(
    const MM::Manager::ManagedObjectID& object_ID,
    MM::RenderSystem::RenderResourceManager::HandlerType& handler,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType) const {
  BaseHandlerType base_handler;
  MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(object_ID, base_handler),
                       return MM_RESULT_CODE;)

  if (!base_handler.GetObject()->IsUseForWrite()) {
    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  RenderResourceDataIDToObjectIDContainerType ::HandlerType
      render_resource_data_ID_to_object_ID_handler;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           base_handler.GetObject()->GetRenderResourceDataID(),
                           render_resource_data_ID_to_object_ID_handler),
                       return MM_RESULT_CODE;)

  handler =
      HandlerType{std::move(base_handler),
                  std::move(render_resource_data_ID_to_object_ID_handler)};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByID(
    const MM::Manager::ManagedObjectID& object_ID,
    MM::RenderSystem::RenderResourceManager::HandlerType& handler,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType) const {
  BaseHandlerType base_handler;
  MM_CHECK_WITHOUT_LOG(GetObjectByIDBase(object_ID, base_handler),
                       return MM_RESULT_CODE;)

  if (base_handler.GetObject()->IsUseForWrite()) {
    return ExecuteResult ::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  RenderResourceDataIDToObjectIDContainerType ::HandlerType
      render_resource_data_ID_to_object_ID_handler;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           base_handler.GetObject()->GetRenderResourceDataID(),
                           render_resource_data_ID_to_object_ID_handler),
                       return MM_RESULT_CODE;)

  handler =
      HandlerType{std::move(base_handler),
                  std::move(render_resource_data_ID_to_object_ID_handler)};

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::RenderResourceManager::
    GetRenderResourceDataByRenderResourceDataID(
        const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
        std::vector<HandlerType>& handlers) const {
  return GetRenderResourceDataByRenderResourceDataID(render_resource_data_ID,
                                                     handlers, Policy::both_);
}

MM::ExecuteResult MM::RenderSystem::RenderResourceManager::
    GetRenderResourceDataByRenderResourceDataID(
        const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
        std::vector<HandlerType>& handlers,
        MM::RenderSystem::RenderResourceManager::Policy::BothType) const {
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

MM::ExecuteResult MM::RenderSystem::RenderResourceManager::
    GetRenderResourceDataByRenderResourceDataID(
        const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
        std::vector<HandlerType>& handlers,
        MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType)
        const {
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

MM::ExecuteResult MM::RenderSystem::RenderResourceManager::
    GetRenderResourceDataByRenderResourceDataID(
        const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
        std::vector<HandlerType>& handlers,
        MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType)
        const {
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByName(
    const std::string& name, std::vector<HandlerType>& handlers) const {
  return GetRenderResourceDataByName(name, handlers, Policy::both_);
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByName(
    const std::string& name, std::vector<HandlerType>& handlers,
    MM::RenderSystem::RenderResourceManager::Policy::BothType) const {
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByName(
    const std::string& name, std::vector<HandlerType>& handlers,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType) const {
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataByName(
    const std::string& name, std::vector<HandlerType>& handlers,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType) const {
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetIDByRenderResourceDataID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<Manager::ManagedObjectID>& managed_object_IDs) const {
  return GetIDByRenderResourceDataID(render_resource_data_ID,
                                     managed_object_IDs, Policy::both_);
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetIDByRenderResourceDataID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<Manager::ManagedObjectID>& managed_object_IDs,
    MM::RenderSystem::RenderResourceManager::Policy::BothType) const {
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetIDByRenderResourceDataID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<Manager::ManagedObjectID>& managed_object_IDs,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType) const {
  std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
      render_resource_data_ID_to_object_ID_handlers;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           render_resource_data_ID,
                           render_resource_data_ID_to_object_ID_handlers),
                       return MM_RESULT_CODE;)

  bool is_add{false};
  for (const auto& render_resource_data_ID_to_object_ID_handler :
       render_resource_data_ID_to_object_ID_handlers) {
    BaseManegerType ::BaseIDToObjectContainer ::HandlerType
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetIDByRenderResourceDataID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<Manager::ManagedObjectID>& managed_object_IDs,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType) const {
  std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
      render_resource_data_ID_to_object_ID_handlers;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           render_resource_data_ID,
                           render_resource_data_ID_to_object_ID_handlers),
                       return MM_RESULT_CODE;)

  bool is_add{false};
  for (const auto& render_resource_data_ID_to_object_ID_handler :
       render_resource_data_ID_to_object_ID_handlers) {
    BaseManegerType ::BaseIDToObjectContainer ::HandlerType
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetNameByRenderResourceID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<std::string>& names) const {
  return GetNameByRenderResourceID(render_resource_data_ID, names,
                                   Policy::both_);
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetNameByRenderResourceID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<std::string>& names,
    MM::RenderSystem::RenderResourceManager::Policy::BothType) const {
  std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
      render_resource_data_ID_to_object_ID_handlers;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           render_resource_data_ID,
                           render_resource_data_ID_to_object_ID_handlers),
                       return MM_RESULT_CODE;)

  for (const auto& render_resource_data_ID_to_object_ID_handler :
       render_resource_data_ID_to_object_ID_handlers) {
    BaseManegerType ::BaseIDToObjectContainer ::HandlerType
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetNameByRenderResourceID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<std::string>& names,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType) const {
  std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
      render_resource_data_ID_to_object_ID_handlers;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           render_resource_data_ID,
                           render_resource_data_ID_to_object_ID_handlers),
                       return MM_RESULT_CODE;)

  for (const auto& render_resource_data_ID_to_object_ID_handler :
       render_resource_data_ID_to_object_ID_handlers) {
    BaseManegerType ::BaseIDToObjectContainer ::HandlerType
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetNameByRenderResourceID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    std::vector<std::string>& names,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType) const {
  std::vector<RenderResourceDataIDToObjectIDContainerType ::HandlerType>
      render_resource_data_ID_to_object_ID_handlers;
  MM_CHECK_WITHOUT_LOG(render_resource_data_ID_to_object_ID_.GetObject(
                           render_resource_data_ID,
                           render_resource_data_ID_to_object_ID_handlers),
                       return MM_RESULT_CODE;)

  for (const auto& render_resource_data_ID_to_object_ID_handler :
       render_resource_data_ID_to_object_ID_handlers) {
    BaseManegerType ::BaseIDToObjectContainer ::HandlerType
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByID(
    const MM::Manager::ManagedObjectID& managed_object_ID,
    MM::RenderSystem::RenderResourceDataID& render_resource_data_ID) const {
  return GetRenderResourceDataIDByID(managed_object_ID, render_resource_data_ID,
                                     Policy::both_);
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByID(
    const MM::Manager::ManagedObjectID& managed_object_ID,
    MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    MM::RenderSystem::RenderResourceManager::Policy::BothType) const {
  BaseIDToObjectContainer::HandlerType ID_to_object_handler;
  MM_CHECK_WITHOUT_LOG(
      GetIDToObjectHandler(managed_object_ID, ID_to_object_handler),
      return MM_RESULT_CODE;)

  render_resource_data_ID =
      ID_to_object_handler.GetObject()->GetRenderResourceDataID();

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByID(
    const MM::Manager::ManagedObjectID& managed_object_ID,
    MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType) const {
  BaseIDToObjectContainer::HandlerType ID_to_object_handler;
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByID(
    const MM::Manager::ManagedObjectID& managed_object_ID,
    MM::RenderSystem::RenderResourceDataID& render_resource_data_ID,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType) const {
  BaseIDToObjectContainer::HandlerType ID_to_object_handler;
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByName(
    const std::string& name,
    std::vector<RenderResourceDataID>& render_resource_data_IDs) const {
  return GetRenderResourceDataIDByName(name, render_resource_data_IDs,
                                       Policy::both_);
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByName(
    const std::string& name,
    std::vector<RenderResourceDataID>& render_resource_data_IDs,
    MM::RenderSystem::RenderResourceManager::Policy::BothType) const {
  std::vector<BaseHandlerType> base_handlers;
  MM_CHECK_WITHOUT_LOG(GetObjectByNameBase(name, base_handlers),
                       return MM_RESULT_CODE;)

  for (const auto& base_handler : base_handlers) {
    render_resource_data_IDs.emplace_back(
        base_handler.GetObject()->GetRenderResourceDataID());
  }

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByName(
    const std::string& name,
    std::vector<RenderResourceDataID>& render_resource_data_IDs,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyWrittenType) const {
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

MM::ExecuteResult
MM::RenderSystem::RenderResourceManager::GetRenderResourceDataIDByName(
    const std::string& name,
    std::vector<RenderResourceDataID>& render_resource_data_IDs,
    MM::RenderSystem::RenderResourceManager::Policy::OnlyNotWrittenType) const {
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

bool MM::RenderSystem::RenderResourceManager::Destroy() {
  std::lock_guard<std::mutex> guard(sync_flag_);
  if (render_resource_manager_) {
    delete render_resource_manager_;
    render_resource_manager_ = nullptr;

    return true;
  }
  return false;
}
