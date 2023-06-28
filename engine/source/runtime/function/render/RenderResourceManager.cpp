//
// Created by beimingxianyu on 23-6-27.
//

#include "runtime/function/render/RenderResourceManager.h"

#include "vk_render_resource.h"

MM::RenderSystem::RenderResourceManager*
    MM::RenderSystem::RenderResourceManager::render_resource_manager_{nullptr};
std::mutex MM::RenderSystem::RenderResourceManager::sync_flag_{};

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
    : Manager::ManagerBase<std::unique_ptr<RenderResourceBase>,
                           Manager::ManagedObjectIsSmartPoint>(size),
      render_resource_data_ID_to_object_ID_(size) {}
