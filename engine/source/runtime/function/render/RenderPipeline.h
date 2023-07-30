//
// Created by beimingxianyu on 23-7-28.
//

#include "runtime/function/render/vk_type_define.h"

namespace MM {
namespace RenderSystem {

class RenderPipeline {
 public:
  RenderPipeline() = default;
  ~RenderPipeline() = default;
  explicit RenderPipeline(VkPipeline pipeline);
  RenderPipeline(const RenderPipeline& other) = delete;
  RenderPipeline(RenderPipeline&& other) noexcept;
  RenderPipeline& operator=(const RenderPipeline& other) = delete;
  RenderPipeline& operator=(RenderPipeline&& other) noexcept;

 public:
  VkPipelineCache GetPipelineCache() const;

  VkPipeline GetPipeline() const;

 private:
  static ExecuteResult CreatePipelineCache();

 private:
  VkPipeline pipeline_{nullptr};

  static VkPipelineCache pipeline_cache_;
  static std::mutex sync_flag_;
};

class GraphicsPipeline : public RenderPipeline {
 public:
  GraphicsPipeline() = default;
  ~GraphicsPipeline() = default;
  GraphicsPipeline(const GraphicsPipeline& other) = delete;
  GraphicsPipeline(GraphicsPipeline&& other) noexcept = default;
  GraphicsPipeline& operator=(const GraphicsPipeline& other) = delete;
  GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept = default;

 private:
  GraphicsPipelineDataInfo graphics_pipeline_data_info_{};
};

class ComputePipeline : public RenderPipeline {
 public:
  ComputePipeline() = default;
  ~ComputePipeline() = default;
  ComputePipeline(RenderEngine* render_engine,
                  const ComputePipelineDataInfo& compute_pipeline_data_info);
  ComputePipeline(
      RenderEngine* render_engine,
      const VkComputePipelineCreateInfo& vk_compute_pipeline_create_info);
  ComputePipeline(const ComputePipeline& other) = delete;
  ComputePipeline(ComputePipeline&& other) noexcept = default;
  ComputePipeline& operator=(const ComputePipeline& other) = delete;
  ComputePipeline& operator=(ComputePipeline&& other) noexcept = default;

 private:
  ComputePipelineDataInfo compute_pipeline_data_info_{};
};

}  // namespace RenderSystem
}  // namespace MM
