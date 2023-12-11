//
// Created by beimingxianyu on 23-7-28.
//
#pragma once

#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {

class RenderPipeline {
 public:
  RenderPipeline() = default;
  virtual ~RenderPipeline();
  RenderPipeline(RenderEngine* render_engine, VkPipeline pipeline);
  RenderPipeline(const RenderPipeline& other) = delete;
  RenderPipeline(RenderPipeline&& other) noexcept;
  RenderPipeline& operator=(const RenderPipeline& other) = delete;
  RenderPipeline& operator=(RenderPipeline&& other) noexcept;

 public:
  VkPipelineCache GetPipelineCache() const;

  VkPipeline GetPipeline() const;

  VkDevice GetDevice() const;

  bool IsValid() const;

  void Release();

 private:
  RenderEngine* render_engine_{nullptr};
  VkPipeline pipeline_{nullptr};
};

class GraphicsPipeline final : public RenderPipeline {
 public:
  GraphicsPipeline() = default;
  virtual ~GraphicsPipeline() override = default;
  GraphicsPipeline(RenderEngine* render_engine, VkPipeline pipeline,
                   const GraphicsPipelineDataInfo& graphics_pipeline_data_info);
  GraphicsPipeline(RenderEngine* render_engine, VkPipeline pipeline,
                   GraphicsPipelineDataInfo&& graphics_pipeline_data_info);
  GraphicsPipeline(
      RenderEngine* render_engine,
      const VkGraphicsPipelineCreateInfo& vk_graphics_pipeline_create_info);
  GraphicsPipeline(RenderEngine* render_engine,
                   const GraphicsPipelineDataInfo& graphics_pipeline_data_info);
  GraphicsPipeline(RenderEngine* render_engine,
                   GraphicsPipelineDataInfo&& graphics_pipeline_data_info);
  GraphicsPipeline(const GraphicsPipeline& other) = delete;
  GraphicsPipeline(GraphicsPipeline&& other) noexcept;
  GraphicsPipeline& operator=(const GraphicsPipeline& other) = delete;
  GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

 public:
  const GraphicsPipelineDataInfo& GetGraphicsPipelineDataInfo() const;

 private:
  Result<Nil> CheckInitParametes(
      RenderEngine* render_engine,
      const VkGraphicsPipelineCreateInfo& vk_graphics_pipeline_create_info);

  Result<Nil> CheckInitParametes(
      RenderEngine* render_engine,
      const GraphicsPipelineDataInfo& graphics_pipeline_data_info);

 private:
  GraphicsPipelineDataInfo graphics_pipeline_data_info_{};
};

class ComputePipeline final : public RenderPipeline {
 public:
  ComputePipeline() = default;
  ~ComputePipeline() override = default;
  ComputePipeline(RenderEngine* render_engine, VkPipeline pipeline,
                  const ComputePipelineDataInfo& compute_pipeline_data_info);
  ComputePipeline(
      RenderEngine* render_engine,
      const VkComputePipelineCreateInfo& vk_compute_pipeline_create_info);
  ComputePipeline(RenderEngine* render_engine,
                  const ComputePipelineDataInfo& compute_pipeline_data_info);
  ComputePipeline(const ComputePipeline& other) = delete;
  ComputePipeline(ComputePipeline&& other) noexcept;
  ComputePipeline& operator=(const ComputePipeline& other) = delete;
  ComputePipeline& operator=(ComputePipeline&& other) noexcept;

 public:
  const ComputePipelineDataInfo& GetComputePipelineDataInfo() const;

 private:
  ComputePipelineDataInfo compute_pipeline_data_info_{};
};

}  // namespace RenderSystem
}  // namespace MM
