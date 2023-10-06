//
// Created by beimingxianyu on 23-7-31.
//
#include "runtime/function/render/DescriptorManager.h"

namespace MM {
namespace RenderSystem {

class PipelineLayout {
 public:
  PipelineLayout() = default;
  ~PipelineLayout();
  PipelineLayout(RenderEngine* render_engine,
                 PipelineLayoutType pipeline_layout_type,
                 ShaderSlotCount shader_slot_count);
  PipelineLayout(const PipelineLayout& other) = delete;
  PipelineLayout(PipelineLayout&& other) noexcept;
  PipelineLayout& operator=(const PipelineLayout& other) = delete;
  PipelineLayout& operator=(PipelineLayout&& other) noexcept;

 public:
  VkDevice GetDevice() const;

  PipelineLayoutType GetPipelineLayoutType() const;

  ShaderSlotCount GetShaderSlotCount() const;

  bool IsGraphicsPipelineLayout() const;

  bool IsComputePipelineLayout() const;

  bool IsRayPipelineLayout() const;

  bool IsSubpassPipelineLayout() const;

  bool IsDefaultPipelineLayout() const;

  bool IsAllPipelineLayout() const;

  std::uint8_t VertexShaderSlotCount();

  std::uint8_t TessellationControlShaderSlotCount();

  std::uint8_t TessellationEvaluationShaderSlotCount();

  std::uint8_t GeometryShaderSlotCount();

  std::uint8_t FragmentShaderSlotCount();

  std::uint8_t AllGraphicsShaderSlotCount();

  std::uint8_t ComputeShaderSlotCount();

  std::uint8_t RayGenShaderSlotCount();

  std::uint8_t AnyHitShaderSlotCount();

  std::uint8_t ClosestShaderSlotCount();

  std::uint8_t MissShaderSlotCount();

  std::uint8_t IntersectionShaderSlotCount();

  std::uint8_t CallableShaderSlotCount();

  std::uint8_t TaskShaderSlotCount();

  std::uint8_t MeshShaderSlotCount();

  std::uint8_t SubpassShaderSlotCount();

  std::uint8_t AllShaderSlotCount();

  bool IsValid() const;

  void Release();

 private:
  ExecuteResult CheckInitParameters();

  ExecuteResult InitGraphicsPipelineLayout();

  ExecuteResult InitComputePipelineLayout();

  ExecuteResult InitRayPipelineLayout();

  ExecuteResult InitSubpassPipelineLayout();

  ExecuteResult InitAllPipelineLayout();

  ExecuteResult InitPipelineLayoutWhenNotUseDefault();

 private:
  RenderEngine* render_engine_{nullptr};
  PipelineLayoutType pipeline_layout_type_{PipelineLayoutType::UNDEFINED};
  ShaderSlotCount shader_slot_count_{ShaderSlotCount::UNDEFINED};

  VkPipelineLayout pipeline_layout_{nullptr};
};

}  // namespace RenderSystem
}  // namespace MM
