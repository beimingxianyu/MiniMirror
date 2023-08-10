//
// Created by beimingxianyu on 23-7-28.
//

#include "runtime/function/render/RenderPipeline.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::RenderPipeline::~RenderPipeline() { Release(); }

MM::RenderSystem::RenderPipeline::RenderPipeline(RenderEngine* render_engine,
                                                 VkPipeline pipeline)
    : render_engine_(render_engine), pipeline_(pipeline) {
  assert(render_engine_ != nullptr && render_engine_->IsValid());
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

  Release();

  render_engine_ = other.render_engine_;
  pipeline_ = other.pipeline_;

  other.Release();
  other.render_engine_ = nullptr;
  other.pipeline_ = nullptr;

  return *this;
}

VkPipelineCache MM::RenderSystem::RenderPipeline::GetPipelineCache() const {
  assert(IsValid());
  return render_engine_->GetPipelineCache();
}

VkPipeline MM::RenderSystem::RenderPipeline::GetPipeline() const {
  assert(IsValid());
  return pipeline_;
}

VkDevice MM::RenderSystem::RenderPipeline::GetDevice() const {
  assert(IsValid());
  return render_engine_->GetDevice();
}

bool MM::RenderSystem::RenderPipeline::IsValid() const {
  assert(IsValid());
  return render_engine_ != nullptr && pipeline_ != nullptr;
}

void MM::RenderSystem::RenderPipeline::Release() {
  if (IsValid()) {
    vkDestroyPipeline(render_engine_->GetDevice(), pipeline_, nullptr);
  }
}

MM::RenderSystem::GraphicsPipeline::GraphicsPipeline(
    MM::RenderSystem::RenderEngine* render_engine, VkPipeline pipeline,
    const MM::RenderSystem::GraphicsPipelineDataInfo&
        graphics_pipeline_data_info)
    : RenderPipeline(render_engine, pipeline),
      graphics_pipeline_data_info_(graphics_pipeline_data_info) {}

MM::RenderSystem::GraphicsPipeline::GraphicsPipeline(
    MM::RenderSystem::RenderEngine* render_engine, VkPipeline pipeline,
    MM::RenderSystem::GraphicsPipelineDataInfo&& graphics_pipeline_data_info)
    : RenderPipeline(render_engine, pipeline),
      graphics_pipeline_data_info_(std::move(graphics_pipeline_data_info)) {}

MM::RenderSystem::GraphicsPipeline::GraphicsPipeline(
    MM::RenderSystem::RenderEngine* render_engine,
    const VkGraphicsPipelineCreateInfo& vk_graphics_pipeline_create_info)
    : RenderPipeline(), graphics_pipeline_data_info_() {
#ifdef MM_CHECK_PARAMETERS
  MM_CHECK(CheckInitParametes(render_engine, vk_graphics_pipeline_create_info),
           MM_LOG_ERROR("The input parameters are error.");
           return;)
#endif

  VkPipeline created_pipeline{nullptr};
  MM_VK_CHECK(vkCreateGraphicsPipelines(render_engine->GetDevice(),
                                        render_engine->GetPipelineCache(), 1,
                                        &vk_graphics_pipeline_create_info,
                                        nullptr, &created_pipeline),
              MM_LOG_ERROR("Failed to create VkPipeline.");
              return;)

  graphics_pipeline_data_info_.flags_ = vk_graphics_pipeline_create_info.flags;

  graphics_pipeline_data_info_.stage_.stages_ =
      std::vector<VkPipelineShaderStageCreateInfo>{
          vk_graphics_pipeline_create_info.pStages,
          vk_graphics_pipeline_create_info.pStages +
              vk_graphics_pipeline_create_info.stageCount};

  graphics_pipeline_data_info_.vertex_input_state_ =
      PipelineVertexInputStateCreateInfo{
          vk_graphics_pipeline_create_info.pVertexInputState->flags,
          std::vector<VkVertexInputBindingDescription>{
              vk_graphics_pipeline_create_info.pVertexInputState
                  ->pVertexBindingDescriptions,
              vk_graphics_pipeline_create_info.pVertexInputState
                      ->pVertexBindingDescriptions +
                  vk_graphics_pipeline_create_info.pVertexInputState
                      ->vertexBindingDescriptionCount},
          std::vector<VkVertexInputAttributeDescription>{
              vk_graphics_pipeline_create_info.pVertexInputState
                  ->pVertexAttributeDescriptions,
              vk_graphics_pipeline_create_info.pVertexInputState
                      ->pVertexAttributeDescriptions +
                  vk_graphics_pipeline_create_info.pVertexInputState
                      ->vertexAttributeDescriptionCount}};

  graphics_pipeline_data_info_
      .tessellation_state_ = PipelineTessellationStateCreateInfo{
      vk_graphics_pipeline_create_info.pTessellationState->flags,
      vk_graphics_pipeline_create_info.pTessellationState->patchControlPoints};

  graphics_pipeline_data_info_.viewport_state_.flags_ =
      vk_graphics_pipeline_create_info.pViewportState->flags;
  graphics_pipeline_data_info_.viewport_state_.viewports_ =
      std::vector<VkViewport>(
          vk_graphics_pipeline_create_info.pViewportState->pViewports,
          vk_graphics_pipeline_create_info.pViewportState->pViewports +
              vk_graphics_pipeline_create_info.pViewportState->viewportCount);
  graphics_pipeline_data_info_.viewport_state_.scissors_ =
      std::vector<VkRect2D>(
          vk_graphics_pipeline_create_info.pViewportState->pScissors,
          vk_graphics_pipeline_create_info.pViewportState->pScissors +
              vk_graphics_pipeline_create_info.pViewportState->scissorCount);

  graphics_pipeline_data_info_
      .rasterization_state_ = PipelineRasterizationStateCreateInfo{
      vk_graphics_pipeline_create_info.pRasterizationState->flags,
      vk_graphics_pipeline_create_info.pRasterizationState->depthClampEnable !=
          0,
      vk_graphics_pipeline_create_info.pRasterizationState
              ->rasterizerDiscardEnable != 0,
      vk_graphics_pipeline_create_info.pRasterizationState->polygonMode,
      vk_graphics_pipeline_create_info.pRasterizationState->cullMode,
      vk_graphics_pipeline_create_info.pRasterizationState->frontFace,
      vk_graphics_pipeline_create_info.pRasterizationState->depthBiasEnable !=
          0,
      vk_graphics_pipeline_create_info.pRasterizationState
          ->depthBiasConstantFactor,
      vk_graphics_pipeline_create_info.pRasterizationState->depthBiasClamp,
      vk_graphics_pipeline_create_info.pRasterizationState
          ->depthBiasSlopeFactor,
      vk_graphics_pipeline_create_info.pRasterizationState->lineWidth};

  graphics_pipeline_data_info_
      .multisample_state_ = PipelineMultisampleStateCreateInfo{
      vk_graphics_pipeline_create_info.pMultisampleState->flags,
      vk_graphics_pipeline_create_info.pMultisampleState->rasterizationSamples,
      vk_graphics_pipeline_create_info.pMultisampleState->sampleShadingEnable !=
          0,
      vk_graphics_pipeline_create_info.pMultisampleState->minSampleShading,
      *vk_graphics_pipeline_create_info.pMultisampleState->pSampleMask,
      vk_graphics_pipeline_create_info.pMultisampleState
              ->alphaToCoverageEnable != 0,
      vk_graphics_pipeline_create_info.pMultisampleState->alphaToOneEnable !=
          0};

  graphics_pipeline_data_info_
      .depth_stencil_state_ = PipelineDepthStencilStateCreateInfo{
      vk_graphics_pipeline_create_info.pDepthStencilState->flags,
      vk_graphics_pipeline_create_info.pDepthStencilState->depthTestEnable != 0,
      vk_graphics_pipeline_create_info.pDepthStencilState->depthWriteEnable !=
          0,
      vk_graphics_pipeline_create_info.pDepthStencilState->depthCompareOp,
      vk_graphics_pipeline_create_info.pDepthStencilState
              ->depthBoundsTestEnable != 0,
      vk_graphics_pipeline_create_info.pDepthStencilState->stencilTestEnable !=
          0,
      vk_graphics_pipeline_create_info.pDepthStencilState->front,
      vk_graphics_pipeline_create_info.pDepthStencilState->back,
      vk_graphics_pipeline_create_info.pDepthStencilState->minDepthBounds,
      vk_graphics_pipeline_create_info.pDepthStencilState->maxDepthBounds};

  graphics_pipeline_data_info_
      .color_blend_state_ = PipelineColorBlendStateCreateInfo{
      vk_graphics_pipeline_create_info.pColorBlendState->flags,
      vk_graphics_pipeline_create_info.pColorBlendState->logicOpEnable,
      vk_graphics_pipeline_create_info.pColorBlendState->logicOp,
      std::vector<VkPipelineColorBlendAttachmentState>(
          vk_graphics_pipeline_create_info.pColorBlendState->pAttachments,
          vk_graphics_pipeline_create_info.pColorBlendState->pAttachments +
              vk_graphics_pipeline_create_info.pColorBlendState
                  ->attachmentCount),
      std::array<float, 4>{
          vk_graphics_pipeline_create_info.pColorBlendState->blendConstants[0],
          vk_graphics_pipeline_create_info.pColorBlendState->blendConstants[1],
          vk_graphics_pipeline_create_info.pColorBlendState->blendConstants[2],
          vk_graphics_pipeline_create_info.pColorBlendState->blendConstants[3],
      }};

  graphics_pipeline_data_info_.dynamic_state_ = PipelineDynamicStateCreateInfo{
      vk_graphics_pipeline_create_info.pDynamicState->flags,
      std::vector<VkDynamicState>{
          vk_graphics_pipeline_create_info.pDynamicState->pDynamicStates,
          vk_graphics_pipeline_create_info.pDynamicState->pDynamicStates +
              vk_graphics_pipeline_create_info.pDynamicState
                  ->dynamicStateCount}};

  graphics_pipeline_data_info_.layout_ =
      vk_graphics_pipeline_create_info.layout;
  graphics_pipeline_data_info_.render_pass_ =
      vk_graphics_pipeline_create_info.renderPass;
  graphics_pipeline_data_info_.subpass_ =
      vk_graphics_pipeline_create_info.subpass;

  RenderPipeline::operator=(RenderPipeline(render_engine, created_pipeline));
}

MM::RenderSystem::GraphicsPipeline::GraphicsPipeline(
    MM::RenderSystem::RenderEngine* render_engine,
    const MM::RenderSystem::GraphicsPipelineDataInfo&
        graphics_pipeline_data_info) {
#ifdef MM_CHECK_PARAMETERS
  MM_CHECK(CheckInitParametes(render_engine, graphics_pipeline_data_info),
           MM_LOG_ERROR("The input parameters are error.");
           return;)
#endif

  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{
      graphics_pipeline_data_info.vertex_input_state_
          .GetVkPipelineVertexInputStateCreateInfo()};
  VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info{
      graphics_pipeline_data_info.tessellation_state_
          .GetVkPipelineTessellationStateCreateInfo()};
  VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info{
      graphics_pipeline_data_info.viewport_state_
          .GetVkPipelineViewportStateCreateInfo()};
  VkPipelineRasterizationStateCreateInfo
      pipeline_rasterization_state_create_info{
          graphics_pipeline_data_info.rasterization_state_
              .GetVkPipelineRasterizationStateCreateInfo()};
  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{
      graphics_pipeline_data_info.multisample_state_
          .GetVkPipelineMultisampleStateCreateInfo()};
  VkPipelineDepthStencilStateCreateInfo
      pipeline_depth_stencil_state_create_info{
          graphics_pipeline_data_info.depth_stencil_state_
              .GetVkPipelineDepthStencilStateCreateInfo()};
  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info{
      graphics_pipeline_data_info.color_blend_state_
          .GetVkPipelineColorBlendStateCreateInfo()};
  VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info{
      graphics_pipeline_data_info.dynamic_state_
          .GetVkPipelineDynamicStateCreateInfo()};

  VkGraphicsPipelineCreateInfo vk_graphics_pipeline_create_info{};
  vk_graphics_pipeline_create_info.sType =
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
  vk_graphics_pipeline_create_info.pNext = nullptr;
  vk_graphics_pipeline_create_info.flags = graphics_pipeline_data_info.flags_;
  vk_graphics_pipeline_create_info.stageCount =
      graphics_pipeline_data_info.stage_.stages_.size();
  vk_graphics_pipeline_create_info.pStages =
      graphics_pipeline_data_info.stage_.stages_.data();
  vk_graphics_pipeline_create_info.pVertexInputState =
      &vertex_input_state_create_info;
  vk_graphics_pipeline_create_info.pTessellationState =
      &pipeline_tessellation_state_create_info;
  vk_graphics_pipeline_create_info.pViewportState =
      &pipeline_viewport_state_create_info;
  vk_graphics_pipeline_create_info.pRasterizationState =
      &pipeline_rasterization_state_create_info;
  vk_graphics_pipeline_create_info.pMultisampleState =
      &pipeline_multisample_state_create_info;
  vk_graphics_pipeline_create_info.pDepthStencilState =
      &pipeline_depth_stencil_state_create_info;
  vk_graphics_pipeline_create_info.pColorBlendState =
      &pipeline_color_blend_state_create_info;
  vk_graphics_pipeline_create_info.pDynamicState =
      &pipeline_dynamic_state_create_info;
  vk_graphics_pipeline_create_info.layout = graphics_pipeline_data_info.layout_;
  vk_graphics_pipeline_create_info.renderPass =
      graphics_pipeline_data_info.render_pass_;
  vk_graphics_pipeline_create_info.subpass =
      graphics_pipeline_data_info.subpass_;
  vk_graphics_pipeline_create_info.basePipelineHandle = nullptr;
  vk_graphics_pipeline_create_info.basePipelineIndex = 0;

  VkPipeline created_pipeline{nullptr};
  MM_VK_CHECK(vkCreateGraphicsPipelines(render_engine->GetDevice(),
                                        render_engine->GetPipelineCache(), 1,
                                        &vk_graphics_pipeline_create_info,
                                        nullptr, &created_pipeline),
              MM_LOG_ERROR("Failed to create VkPipeline.");
              return;)

  RenderPipeline::operator=(RenderPipeline(render_engine, created_pipeline));
  graphics_pipeline_data_info_ = graphics_pipeline_data_info;
}

MM::ExecuteResult MM::RenderSystem::GraphicsPipeline::CheckInitParametes(
    MM::RenderSystem::RenderEngine* render_engine,
    const VkGraphicsPipelineCreateInfo& vk_graphics_pipeline_create_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (vk_graphics_pipeline_create_info.pVertexInputState == nullptr ||
      vk_graphics_pipeline_create_info.pVertexInputState
              ->vertexBindingDescriptionCount < 1 ||
      vk_graphics_pipeline_create_info.pVertexInputState
              ->vertexAttributeDescriptionCount < 5 ||
      !MM::RenderSystem::Utils::VkVertexInputBindingDescriptionIsEqual(
          vk_graphics_pipeline_create_info.pVertexInputState
              ->pVertexBindingDescriptions[0],
          DefaultVertexInputStateDescription::
              vertex_input_state_bind_description_) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          vk_graphics_pipeline_create_info.pVertexInputState
              ->pVertexAttributeDescriptions[0],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[0]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          vk_graphics_pipeline_create_info.pVertexInputState
              ->pVertexAttributeDescriptions[1],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[1]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          vk_graphics_pipeline_create_info.pVertexInputState
              ->pVertexAttributeDescriptions[2],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[2]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          vk_graphics_pipeline_create_info.pVertexInputState
              ->pVertexAttributeDescriptions[3],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[3]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          vk_graphics_pipeline_create_info.pVertexInputState
              ->pVertexAttributeDescriptions[4],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[4])) {
    MM_LOG_ERROR(
        "The input parametes vk_graphics_pipeline_create_info is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (vk_graphics_pipeline_create_info.pViewportState == nullptr ||
      vk_graphics_pipeline_create_info.pViewportState->viewportCount < 1 ||
      vk_graphics_pipeline_create_info.pViewportState->scissorCount < 1 ||
      !MM::RenderSystem::Utils::VkViewportIsEqual(
          vk_graphics_pipeline_create_info.pViewportState->pViewports[0],
          DefaultViewportState::default_viewport_) ||
      !MM::RenderSystem::Utils::VkRect2DIsEqual(
          vk_graphics_pipeline_create_info.pViewportState->pScissors[0],
          DefaultViewportState::default_scissors_)) {
    MM_LOG_ERROR(
        "The input parametes vk_graphics_pipeline_create_info is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return MM::ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::GraphicsPipeline::CheckInitParametes(
    MM::RenderSystem::RenderEngine* render_engine,
    const MM::RenderSystem::GraphicsPipelineDataInfo&
        graphics_pipeline_data_info) {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (graphics_pipeline_data_info.vertex_input_state_
          .vertex_binding_description_.empty() ||
      graphics_pipeline_data_info.vertex_input_state_
              .vertex_attribute_descriptions_.size() < 5 ||
      !MM::RenderSystem::Utils::VkVertexInputBindingDescriptionIsEqual(
          graphics_pipeline_data_info.vertex_input_state_
              .vertex_binding_description_[0],
          DefaultVertexInputStateDescription::
              vertex_input_state_bind_description_) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          graphics_pipeline_data_info.vertex_input_state_
              .vertex_attribute_descriptions_[0],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[0]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          graphics_pipeline_data_info.vertex_input_state_
              .vertex_attribute_descriptions_[1],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[1]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          graphics_pipeline_data_info.vertex_input_state_
              .vertex_attribute_descriptions_[2],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[2]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          graphics_pipeline_data_info.vertex_input_state_
              .vertex_attribute_descriptions_[3],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[3]) ||
      !MM::RenderSystem::Utils::VkVertexInputAttributeDescriptionIsEqual(
          graphics_pipeline_data_info.vertex_input_state_
              .vertex_attribute_descriptions_[4],
          DefaultVertexInputStateDescription::
              vertex_input_state_attribute_descriptions_[4])) {
    MM_LOG_ERROR("The input parametes graphics_pipeline_data_info is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (graphics_pipeline_data_info.viewport_state_.viewports_.empty() ||
      graphics_pipeline_data_info.viewport_state_.scissors_.empty() ||
      !MM::RenderSystem::Utils::VkViewportIsEqual(
          graphics_pipeline_data_info.viewport_state_.viewports_[0],
          DefaultViewportState::default_viewport_) ||
      !MM::RenderSystem::Utils::VkRect2DIsEqual(
          graphics_pipeline_data_info.viewport_state_.scissors_[0],
          DefaultViewportState::default_scissors_)) {
    MM_LOG_ERROR("The input parametes graphics_pipeline_data_info is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  return MM::ExecuteResult::SUCCESS;
}

MM::RenderSystem::GraphicsPipeline::GraphicsPipeline(
    MM::RenderSystem::RenderEngine* render_engine,
    MM::RenderSystem::GraphicsPipelineDataInfo&& graphics_pipeline_data_info) {
#ifdef MM_CHECK_PARAMETERS
  MM_CHECK(CheckInitParametes(render_engine, graphics_pipeline_data_info),
           MM_LOG_ERROR("The input parameters are error.");
           return;)
#endif

  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{
      graphics_pipeline_data_info.vertex_input_state_
          .GetVkPipelineVertexInputStateCreateInfo()};
  VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info{
      graphics_pipeline_data_info.tessellation_state_
          .GetVkPipelineTessellationStateCreateInfo()};
  VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info{
      graphics_pipeline_data_info.viewport_state_
          .GetVkPipelineViewportStateCreateInfo()};
  VkPipelineRasterizationStateCreateInfo
      pipeline_rasterization_state_create_info{
          graphics_pipeline_data_info.rasterization_state_
              .GetVkPipelineRasterizationStateCreateInfo()};
  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{
      graphics_pipeline_data_info.multisample_state_
          .GetVkPipelineMultisampleStateCreateInfo()};
  VkPipelineDepthStencilStateCreateInfo
      pipeline_depth_stencil_state_create_info{
          graphics_pipeline_data_info.depth_stencil_state_
              .GetVkPipelineDepthStencilStateCreateInfo()};
  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info{
      graphics_pipeline_data_info.color_blend_state_
          .GetVkPipelineColorBlendStateCreateInfo()};
  VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info{
      graphics_pipeline_data_info.dynamic_state_
          .GetVkPipelineDynamicStateCreateInfo()};

  VkGraphicsPipelineCreateInfo vk_graphics_pipeline_create_info{};
  vk_graphics_pipeline_create_info.sType =
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
  vk_graphics_pipeline_create_info.pNext = nullptr;
  vk_graphics_pipeline_create_info.flags = graphics_pipeline_data_info.flags_;
  vk_graphics_pipeline_create_info.stageCount =
      graphics_pipeline_data_info.stage_.stages_.size();
  vk_graphics_pipeline_create_info.pStages =
      graphics_pipeline_data_info.stage_.stages_.data();
  vk_graphics_pipeline_create_info.pVertexInputState =
      &vertex_input_state_create_info;
  vk_graphics_pipeline_create_info.pTessellationState =
      &pipeline_tessellation_state_create_info;
  vk_graphics_pipeline_create_info.pViewportState =
      &pipeline_viewport_state_create_info;
  vk_graphics_pipeline_create_info.pRasterizationState =
      &pipeline_rasterization_state_create_info;
  vk_graphics_pipeline_create_info.pMultisampleState =
      &pipeline_multisample_state_create_info;
  vk_graphics_pipeline_create_info.pDepthStencilState =
      &pipeline_depth_stencil_state_create_info;
  vk_graphics_pipeline_create_info.pColorBlendState =
      &pipeline_color_blend_state_create_info;
  vk_graphics_pipeline_create_info.pDynamicState =
      &pipeline_dynamic_state_create_info;
  vk_graphics_pipeline_create_info.layout = graphics_pipeline_data_info.layout_;
  vk_graphics_pipeline_create_info.renderPass =
      graphics_pipeline_data_info.render_pass_;
  vk_graphics_pipeline_create_info.subpass =
      graphics_pipeline_data_info.subpass_;
  vk_graphics_pipeline_create_info.basePipelineHandle = nullptr;
  vk_graphics_pipeline_create_info.basePipelineIndex = 0;

  VkPipeline created_pipeline{nullptr};
  MM_VK_CHECK(vkCreateGraphicsPipelines(render_engine->GetDevice(),
                                        render_engine->GetPipelineCache(), 1,
                                        &vk_graphics_pipeline_create_info,
                                        nullptr, &created_pipeline),
              MM_LOG_ERROR("Failed to create VkPipeline.");
              return;)

  RenderPipeline::operator=(RenderPipeline(render_engine, created_pipeline));
  graphics_pipeline_data_info_ = std::move(graphics_pipeline_data_info);
}

MM::RenderSystem::GraphicsPipeline::GraphicsPipeline(
    MM::RenderSystem::GraphicsPipeline&& other) noexcept
    : RenderPipeline(std::move(other)),
      graphics_pipeline_data_info_(
          std::move(other.graphics_pipeline_data_info_)) {}

MM::RenderSystem::GraphicsPipeline&
MM::RenderSystem::GraphicsPipeline::operator=(
    MM::RenderSystem::GraphicsPipeline&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  Release();

  RenderPipeline::operator=(std::move(other));
  graphics_pipeline_data_info_ = std::move(other.graphics_pipeline_data_info_);

  return *this;
}

const MM::RenderSystem::GraphicsPipelineDataInfo&
MM::RenderSystem::GraphicsPipeline::GetGraphicsPipelineDataInfo() const {
  return graphics_pipeline_data_info_;
}

MM::RenderSystem::ComputePipeline::ComputePipeline(
    MM::RenderSystem::RenderEngine* render_engine, VkPipeline pipeline,
    const MM::RenderSystem::ComputePipelineDataInfo& compute_pipeline_data_info)
    : RenderPipeline(render_engine, pipeline),
      compute_pipeline_data_info_(compute_pipeline_data_info) {}

MM::RenderSystem::ComputePipeline::ComputePipeline(
    MM::RenderSystem::RenderEngine* render_engine,
    const VkComputePipelineCreateInfo& vk_compute_pipeline_create_info)
    : RenderPipeline(), compute_pipeline_data_info_() {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is error.");
    return;
  }

  VkPipeline created_pipeline{nullptr};
  MM_VK_CHECK(vkCreateComputePipelines(
                  render_engine->GetDevice(), GetPipelineCache(), 1,
                  &vk_compute_pipeline_create_info, nullptr, &created_pipeline),
              MM_LOG_ERROR("Failed to create VkPipeline(compute).");
              return;)

  compute_pipeline_data_info_.flags_ = vk_compute_pipeline_create_info.flags;
  compute_pipeline_data_info_.stage_ = vk_compute_pipeline_create_info.stage;
  compute_pipeline_data_info_.layout_ = vk_compute_pipeline_create_info.layout;

  RenderPipeline::operator=(
      std::move(RenderPipeline(render_engine, created_pipeline)));
}

MM::RenderSystem::ComputePipeline::ComputePipeline(
    MM::RenderSystem::RenderEngine* render_engine,
    const MM::RenderSystem::ComputePipelineDataInfo& compute_pipeline_data_info)
    : RenderPipeline(), compute_pipeline_data_info_() {
  if (render_engine == nullptr || !render_engine->IsValid()) {
    MM_LOG_ERROR("The input parameter render_engine is error.");
    return;
  }

  VkComputePipelineCreateInfo vk_compute_pipeline_create_info =
      compute_pipeline_data_info.GetVkComputePipelineCreateInfo();

  VkPipeline created_pipeline{nullptr};
  MM_VK_CHECK(vkCreateComputePipelines(
                  render_engine->GetDevice(), GetPipelineCache(), 1,
                  &vk_compute_pipeline_create_info, nullptr, &created_pipeline),
              MM_LOG_ERROR("Failed to create VkPipeline(compute).");
              return;)

  RenderPipeline::operator=(
      std::move(RenderPipeline(render_engine, created_pipeline)));
  compute_pipeline_data_info_ = compute_pipeline_data_info;
}

MM::RenderSystem::ComputePipeline::ComputePipeline(
    MM::RenderSystem::ComputePipeline&& other) noexcept
    : RenderPipeline(std::move(other)),
      compute_pipeline_data_info_(other.compute_pipeline_data_info_) {
  other.RenderPipeline::operator=(RenderPipeline{});
}

MM::RenderSystem::ComputePipeline& MM::RenderSystem::ComputePipeline::operator=(
    MM::RenderSystem::ComputePipeline&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  Release();

  RenderPipeline::operator=(std::move(other));
  compute_pipeline_data_info_ = other.compute_pipeline_data_info_;

  return *this;
}

const MM::RenderSystem::ComputePipelineDataInfo&
MM::RenderSystem::ComputePipeline::GetComputePipelineDataInfo() const {
  return compute_pipeline_data_info_;
}
