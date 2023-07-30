//
// Created by beimingxianyu on 23-7-28.
//

#include "runtime/function/render/RenderPipeline.h"
MM::RenderSystem::RenderPipeline::RenderPipeline(VkPipeline pipeline)
    : pipeline_(pipeline) {
  assert(pipeline_ != nullptr);
}

MM::RenderSystem::RenderPipeline::RenderPipeline(
    MM::RenderSystem::RenderPipeline&& other) noexcept
    : pipeline_(other.pipeline_) {
  other.pipeline_ = nullptr;
}

MM::RenderSystem::RenderPipeline& MM::RenderSystem::RenderPipeline::operator=(
    MM::RenderSystem::RenderPipeline&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  pipeline_ = other.pipeline_;

  other.pipeline_ = nullptr;

  return *this;
}

VkPipelineCache MM::RenderSystem::RenderPipeline::GetPipelineCache() const {
  if (pipeline_cache_) {
  } else {
    std::lock_guard guard{sync_flag_};
    if (!pipeline_cache_) {
      std::vector<char> cache_data;
      MM_CHECK(MM_FILE_SYSTEM->ReadFile(
                   MM_FILE_SYSTEM->GetAssetDirCache() + "./.pipeline_cache",
                   cache_data),
               MM_CHECK(CreatePipelineCache(),
                        MM_LOG_FATAL("Failed to create VkPipelineCache.");
                        return nullptr;))

      MM::RenderSystem::Utils::IsValidPipelineCacheData()
    }
  }

  return pipeline_cache_;
}
