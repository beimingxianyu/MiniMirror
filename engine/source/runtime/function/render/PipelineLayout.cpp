//
// Created by beimingxianyu on 23-7-31.
//

#include "runtime/function/render/PipelineLayout.h"

#include <cassert>

#include "runtime/function/render/vk_engine.h"
#include "runtime/function/render/vk_enum.h"

MM::RenderSystem::PipelineLayout::~PipelineLayout() { Release(); }

MM::RenderSystem::PipelineLayout::PipelineLayout(
    RenderEngine *render_engine, ShaderSlotDescriptor shader_slot_count)
    : render_engine_(render_engine),
      shader_slot_count_(shader_slot_count),
      pipeline_layout_(nullptr) {
#ifdef MM_CHECK_PARAMETERS
  if (auto if_result = CheckInitParameters();
      if_result.Exception(MM_ERROR_DESCRIPTION2("Input parameters are error."))
          .IsError()) {
    render_engine_ = nullptr;
    shader_slot_count_ = ShaderSlotDescriptor::UNDEFINED;
    return;
  }
#endif

  if (IsDefaultPipelineLayout()) {
    switch (shader_slot_count_) {
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_0_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout0();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_1_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout1();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_2_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout2();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_3_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout3();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_4_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout4();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_5_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout5();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_6_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout6();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_7_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout7();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_8_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout8();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_9_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout9();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_10_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout10();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_11_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout11();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_12_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout12();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_13_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout13();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_14_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout14();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_15_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout15();
        return;
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_16_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout16();
        return;
      default:
        assert(false);
    }
  }

  if (auto if_result = InitPipelineLayoutWhenNotUseDefault();
      if_result
          .Exception(MM_ERROR_DESCRIPTION2(
              "Failed to initialization VkPipelineLayout."))
          .IsError()) {
    render_engine_ = nullptr;
    shader_slot_count_ = ShaderSlotDescriptor::UNDEFINED;
  }
}

MM::RenderSystem::PipelineLayout::PipelineLayout(
    PipelineLayout &&other) noexcept
    : render_engine_(other.render_engine_),
      shader_slot_count_(other.shader_slot_count_),
      pipeline_layout_(other.pipeline_layout_) {
  other.render_engine_ = nullptr;
  other.shader_slot_count_ = ShaderSlotDescriptor::UNDEFINED;
  other.pipeline_layout_ = nullptr;
}

MM::RenderSystem::PipelineLayout &MM::RenderSystem::PipelineLayout::operator=(
    PipelineLayout &&other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  Release();

  render_engine_ = other.render_engine_;
  shader_slot_count_ = other.shader_slot_count_;
  pipeline_layout_ = other.pipeline_layout_;

  other.render_engine_ = nullptr;
  other.shader_slot_count_ = ShaderSlotDescriptor::UNDEFINED;
  other.pipeline_layout_ = nullptr;

  return *this;
}

VkDevice MM::RenderSystem::PipelineLayout::GetDevice() const {
  assert(IsValid());
  return render_engine_->GetDevice();
}

MM::RenderSystem::PipelineLayoutType
MM::RenderSystem::PipelineLayout::GetPipelineLayoutType() const {
  assert(IsValid());
  if (IsAllPipelineLayout()) {
    return PipelineLayoutType::ALL;
  }
  if (IsDefaultPipelineLayout()) {
    return PipelineLayoutType::DEFAULT;
  }
  if (IsGraphicsPipelineLayout()) {
    return PipelineLayoutType::GRAPHICS;
  }
  if (IsComputePipelineLayout()) {
    return PipelineLayoutType::COMPUTE;
  }
  if (IsRayPipelineLayout()) {
    return PipelineLayoutType::RAY;
  }
  if (IsSubpassPipelineLayout()) {
    return PipelineLayoutType::SUBPASS;
  }

  assert(false);
}

std::uint8_t MM::RenderSystem::PipelineLayout::GetShaderSlotCount() const {
  assert(IsValid());
  return shader_slot_count_;
}

bool MM::RenderSystem::PipelineLayout::IsGraphicsPipelineLayout() const {
  assert(IsValid());
  return pipeline_layout_type_ == PipelineLayoutType::GRAPHICS;
}

bool MM::RenderSystem::PipelineLayout::IsComputePipelineLayout() const {
  assert(IsValid());
  return pipeline_layout_type_ == PipelineLayoutType::COMPUTE;
}

bool MM::RenderSystem::PipelineLayout::IsRayPipelineLayout() const {
  assert(IsValid());
  return pipeline_layout_type_ == PipelineLayoutType::RAY;
}

bool MM::RenderSystem::PipelineLayout::IsSubpassPipelineLayout() const {
  assert(IsValid());
  return pipeline_layout_type_ == PipelineLayoutType::SUBPASS;
}

bool MM::RenderSystem::PipelineLayout::IsDefaultPipelineLayout() const {
  assert(IsValid());
  return pipeline_layout_type_ == PipelineLayoutType::DEFAULT;
}

bool MM::RenderSystem::PipelineLayout::IsAllPipelineLayout() const {
  assert(IsValid());
  if (IsDefaultPipelineLayout()) {
    return true;
  }

  return pipeline_layout_type_ == PipelineLayoutType::ALL;
}

std::uint8_t MM::RenderSystem::PipelineLayout::VertexShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}
std::uint8_t
MM::RenderSystem::PipelineLayout::TessellationControlShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 5) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t
MM::RenderSystem::PipelineLayout::TessellationEvaluationShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 10) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::GeometryShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 15) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::FragmentShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 20) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::AllGraphicsShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  switch (shader_slot_count_) {
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_0_SLOT_BIT:
      return 0;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT:
      return 1;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT:
      return 2;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT:
      return 3;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT:
      return 4;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT:
      return 5;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT:
      return 6;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT:
      return 7;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT:
      return 8;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT:
      return 9;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT:
      return 10;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT:
      return 11;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT:
      return 12;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT:
      return 13;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT:
      return 14;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT:
      return 15;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT:
      return 16;
    default:
      return 0;
  }
}

std::uint8_t MM::RenderSystem::PipelineLayout::ComputeShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsComputePipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::RayGenShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsRayPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}
std::uint8_t MM::RenderSystem::PipelineLayout::AnyHitShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsRayPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 5) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::ClosestShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsRayPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 10) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::MissShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsRayPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 15) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::IntersectionShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsRayPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 20) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::CallableShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsRayPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 25) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::TaskShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 25) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::MeshShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsGraphicsPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 30) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::SubpassShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsRayPipelineLayout()) {
    return 0;
  }

  const std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::AllShaderSlotCount() const {
  if (!IsAllPipelineLayout()) {
    return 0;
  }

  switch (shader_slot_count_) {
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_0_SLOT_BIT:
      return 0;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_1_SLOT_BIT:
      return 1;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_2_SLOT_BIT:
      return 2;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_3_SLOT_BIT:
      return 3;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_4_SLOT_BIT:
      return 4;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_5_SLOT_BIT:
      return 5;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_6_SLOT_BIT:
      return 6;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_7_SLOT_BIT:
      return 7;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_8_SLOT_BIT:
      return 8;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_9_SLOT_BIT:
      return 9;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_10_SLOT_BIT:
      return 10;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_11_SLOT_BIT:
      return 11;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_12_SLOT_BIT:
      return 12;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_13_SLOT_BIT:
      return 13;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_14_SLOT_BIT:
      return 14;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_15_SLOT_BIT:
      return 15;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_16_SLOT_BIT:
      return 16;
    default:
      assert(false);
  }
}

bool MM::RenderSystem::PipelineLayout::IsValid() const {
  return render_engine_ != nullptr &&
         shader_slot_count_ != ShaderSlotDescriptor::UNDEFINED &&
         pipeline_layout_ != nullptr;
}

void MM::RenderSystem::PipelineLayout::Release() {
  if (IsValid() && !IsDefaultPipelineLayout()) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout_,
                            nullptr);
    render_engine_ == nullptr;
    shader_slot_count_ = ShaderSlotDescriptor::UNDEFINED;
    pipeline_layout_ = nullptr;
  }
}

MM::Result<MM::Nil> MM::RenderSystem::PipelineLayout::CheckInitParameters() {
  if (render_engine_ == nullptr || !render_engine_->IsValid()) {
    MM_LOG_ERROR("The input parameters render_engine is error.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }

  if (pipeline_layout_type_ == PipelineLayoutType::UNDEFINED) {
    MM_LOG_ERROR("The input parameters pipeline_layout_type is error.");
    return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
  }

  if (pipeline_layout_type_ == PipelineLayoutType::GRAPHICS) {
    const std::uint64_t use_info =
        static_cast<std::uint64_t>(shader_slot_count_) & 0x7FFFFFFFF;
    if (use_info == 0) {
      MM_LOG_ERROR("The input parameters shader_slot_count is error.");
      return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
    }
    return ResultS<Nil>{};
  }

  if (pipeline_layout_type_ == PipelineLayoutType::COMPUTE) {
    const std::uint64_t use_info =
        static_cast<std::uint64_t>(shader_slot_count_) & 0x1F;
    if (use_info == 0) {
      MM_LOG_ERROR("The input parameters shader_slot_count is error.");
      return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
    }
    return ResultS<Nil>{};
  }

  if (pipeline_layout_type_ == PipelineLayoutType::RAY) {
    const std::uint64_t use_info =
        static_cast<std::uint64_t>(shader_slot_count_) & 0x3FFFFFFF;
    if (use_info == 0) {
      MM_LOG_ERROR("The input parameters shader_slot_count is error.");
      return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
    }
    return ResultS<Nil>{};
  }

  if (pipeline_layout_type_ == PipelineLayoutType::ALL ||
      pipeline_layout_type_ == PipelineLayoutType::DEFAULT) {
    switch (shader_slot_count_) {
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_0_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_1_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_2_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_3_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_4_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_5_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_6_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_7_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_8_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_9_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_10_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_11_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_12_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_13_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_14_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_15_SLOT_BIT:
      case ShaderSlotDescriptor::SHADER_STAGE_ALL_STAGE_16_SLOT_BIT:
        return ResultS<Nil>{};

      default:
        return ResultE<>{ErrorCode::INITIALIZATION_FAILED};
    }
  }

  MM_LOG_ERROR("Don't support subpass pipeline.");
  return ResultE<>{ErrorCode::OPERATION_NOT_SUPPORTED};
}

MM::Result<MM::Nil>
MM::RenderSystem::PipelineLayout::InitPipelineLayoutWhenNotUseDefault() {
  switch (pipeline_layout_type_) {
    case PipelineLayoutType::GRAPHICS:
      return InitGraphicsPipelineLayout();
    case PipelineLayoutType::COMPUTE:
      return InitComputePipelineLayout();
    case PipelineLayoutType::RAY:
      return InitRayPipelineLayout();
    case PipelineLayoutType::SUBPASS:
      return InitSubpassPipelineLayout();
    case PipelineLayoutType::ALL:
      return InitAllPipelineLayout();
    default:
      assert(false);
  }
}

MM::Result<MM::Nil>
MM::RenderSystem::PipelineLayout::InitGraphicsPipelineLayout() {
  std::array<VkDescriptorSetLayout, 2> descriptor_sets{
      render_engine_->GetDescriptorManager().GetGlobalDescriptorSetLayout(),
      render_engine_->GetDescriptorManager().GetMaterialDescriptorSetLayout()};
  std::array<VkPushConstantRange, 7> ranger{};
  std::uint32_t range_count = 0;
  bool is_all_graphics_stage = false;
  VkPipelineLayoutCreateInfo pipeline_layout_create_info{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      nullptr,
      0,
      descriptor_sets.size(),
      descriptor_sets.data(),
      range_count,
      ranger.data()};

  switch (shader_slot_count_) {
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData1)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData2)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData3)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData4)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData5)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData6)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData7)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData8)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData9)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData10)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData11)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData12)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData13)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData14)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData15)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData16)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    default:
      break;
  }

  pipeline_layout_create_info.pushConstantRangeCount = range_count;

  if (is_all_graphics_stage) {
    if (auto if_result = ConvertVkResultToMMResult(vkCreatePipelineLayout(
            render_engine_->GetDevice(), &pipeline_layout_create_info, nullptr,
            &pipeline_layout_));
        if_result
            .Exception(
                MM_ERROR_DESCRIPTION2("Filed to create VkPipelineLayout."))
            .IsError()) {
      return ResultE{if_result.GetError()};
    }
  }

  std::uint8_t slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_VERTEX_BIT, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 5) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 10) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 15) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_GEOMETRY_BIT, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 20) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 25) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_TASK_BIT_NV, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 30) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_MESH_BIT_NV, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  pipeline_layout_create_info.pushConstantRangeCount = range_count;
  if (auto if_result = ConvertVkResultToMMResult(vkCreatePipelineLayout(
          render_engine_->GetDevice(), &pipeline_layout_create_info, nullptr,
          &pipeline_layout_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkPipelineLayout."))
          .IsError()) {
    return ResultE{if_result.GetError()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::PipelineLayout::InitComputePipelineLayout() {
  std::array<VkDescriptorSetLayout, 2> descriptor_sets{
      render_engine_->GetDescriptorManager().GetGlobalDescriptorSetLayout(),
      render_engine_->GetDescriptorManager().GetMaterialDescriptorSetLayout()};
  VkPushConstantRange ranger{};
  VkPipelineLayoutCreateInfo pipeline_layout_create_info{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      nullptr,
      0,
      descriptor_sets.size(),
      descriptor_sets.data(),
      0,
      nullptr};

  const std::uint8_t slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger = VkPushConstantRange{VK_SHADER_STAGE_COMPUTE_BIT, 0,
                                 static_cast<uint32_t>((slot_count - 1) * 8)};
    pipeline_layout_create_info.pushConstantRangeCount = 1;
    pipeline_layout_create_info.pPushConstantRanges = &ranger;
  }

  if (auto if_result = ConvertVkResultToMMResult(vkCreatePipelineLayout(
          render_engine_->GetDevice(), &pipeline_layout_create_info, nullptr,
          &pipeline_layout_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkPipelineLayout."))
          .IsError()) {
    return ResultE{if_result.GetError()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil> MM::RenderSystem::PipelineLayout::InitRayPipelineLayout() {
  std::array<VkDescriptorSetLayout, 2> descriptor_sets{
      render_engine_->GetDescriptorManager().GetGlobalDescriptorSetLayout(),
      render_engine_->GetDescriptorManager().GetMaterialDescriptorSetLayout()};
  std::array<VkPushConstantRange, 6> ranger{};
  std::uint32_t range_count = 0;
  VkPipelineLayoutCreateInfo pipeline_layout_create_info{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      nullptr,
      0,
      descriptor_sets.size(),
      descriptor_sets.data(),
      range_count,
      ranger.data()};

  pipeline_layout_create_info.pushConstantRangeCount = range_count;

  std::uint8_t slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_RAYGEN_BIT_KHR, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 5) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 10) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 15) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_MISS_BIT_KHR, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 20) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_INTERSECTION_BIT_KHR, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }
  slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 25) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger[range_count] =
        VkPushConstantRange{VK_SHADER_STAGE_CALLABLE_BIT_KHR, 0,
                            static_cast<uint32_t>((slot_count - 1) * 8)};
    ++range_count;
  }

  pipeline_layout_create_info.pushConstantRangeCount = range_count;
  if (auto if_result = ConvertVkResultToMMResult(vkCreatePipelineLayout(
          render_engine_->GetDevice(), &pipeline_layout_create_info, nullptr,
          &pipeline_layout_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkPipelineLayout."))
          .IsError()) {
    return ResultE{if_result.GetError()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil>
MM::RenderSystem::PipelineLayout::InitSubpassPipelineLayout() {
  std::array<VkDescriptorSetLayout, 2> descriptor_sets{
      render_engine_->GetDescriptorManager().GetGlobalDescriptorSetLayout(),
      render_engine_->GetDescriptorManager().GetMaterialDescriptorSetLayout()};
  VkPushConstantRange ranger{};
  VkPipelineLayoutCreateInfo pipeline_layout_create_info{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      nullptr,
      0,
      descriptor_sets.size(),
      descriptor_sets.data(),
      0,
      nullptr};

  const std::uint8_t slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI, 0,
                                 static_cast<uint32_t>((slot_count - 1) * 8)};
    pipeline_layout_create_info.pushConstantRangeCount = 1;
    pipeline_layout_create_info.pPushConstantRanges = &ranger;
  }

  if (auto if_result = ConvertVkResultToMMResult(vkCreatePipelineLayout(
          render_engine_->GetDevice(), &pipeline_layout_create_info, nullptr,
          &pipeline_layout_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkPipelineLayout."))
          .IsError()) {
    return ResultE{if_result.GetError()};
  }

  return ResultS<Nil>{};
}

MM::Result<MM::Nil> MM::RenderSystem::PipelineLayout::InitAllPipelineLayout() {
  std::array<VkDescriptorSetLayout, 2> descriptor_sets{
      render_engine_->GetDescriptorManager().GetGlobalDescriptorSetLayout(),
      render_engine_->GetDescriptorManager().GetMaterialDescriptorSetLayout()};
  VkPushConstantRange ranger{};
  bool is_have_push_constant = false;
  VkPipelineLayoutCreateInfo pipeline_layout_create_info{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      nullptr,
      0,
      descriptor_sets.size(),
      descriptor_sets.data(),
      0,
      nullptr};

  switch (shader_slot_count_) {
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData1)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData2)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData3)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData4)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData5)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData6)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData7)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData8)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData9)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData10)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData11)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData12)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData13)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData14)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData15)};
      is_have_push_constant = true;
      break;
    case ShaderSlotDescriptor::SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData16)};
      is_have_push_constant = true;
      break;
    default:
      break;
  }

  if (is_have_push_constant) {
    pipeline_layout_create_info.pushConstantRangeCount = 1;
    pipeline_layout_create_info.pPushConstantRanges = &ranger;
  }

  if (auto if_result = ConvertVkResultToMMResult(vkCreatePipelineLayout(
          render_engine_->GetDevice(), &pipeline_layout_create_info, nullptr,
          &pipeline_layout_));
      if_result
          .Exception(MM_ERROR_DESCRIPTION2("Filed to create VkPipelineLayout."))
          .IsError()) {
    return ResultE{if_result.GetError()};
  }

  return ResultS<Nil>{};
}
