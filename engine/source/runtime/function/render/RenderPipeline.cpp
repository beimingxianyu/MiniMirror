//
// Created by beimingxianyu on 23-7-28.
//

#include "runtime/function/render/RenderPipeline.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::RenderPipeline::~RenderPipeline() { Releasse(); }

MM::RenderSystem::RenderPipeline::RenderPipeline(RenderEngine* render_engine,
                                                 VkPipeline pipeline)
    : render_engine_(render_engine), pipeline_(pipeline) {
  assert(render_engine_ != nullptr);
  assert(pipeline_ != nullptr);
}

MM::RenderSystem::RenderPipeline::RenderPipeline(
    MM::RenderSystem::RenderPipeline&& other) noexcept
    : render_engine_(other.render_engine_), pipeline_(other.pipeline_) {
  other.render_engine_ = nullptr;
  other.pipeline_ = nullptr;
}

MM::RenderSystem::RenderPipeline& MM::RenderSystem::RenderPipeline::operator=(
    MM::RenderSystem::RenderPipeline&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  render_engine_ = other.render_engine_;
  pipeline_ = other.pipeline_;

  other.render_engine_ = nullptr;
  other.pipeline_ = nullptr;

  return *this;
}

VkPipelineCache MM::RenderSystem::RenderPipeline::GetPipelineCache() const {
  return render_engine_->GetPipelineCache();
}

VkPipeline MM::RenderSystem::RenderPipeline::GetPipeline() const {
  return pipeline_;
}

VkDevice MM::RenderSystem::RenderPipeline::GetDevice() const {
  return render_engine_->GetDevice();
}

VkPipelineLayout MM::RenderSystem::RenderPipeline::GetPipelineLayout() const {
  VkShaderStageFlagBits a;
  return render_engine_->GetDescriptorManager();

}
