//
// Created by beimingxianyu on 23-7-31.
//

#include "runtime/function/render/PipelineLayout.h"

#include "runtime/function/render/vk_engine.h"

MM::RenderSystem::PipelineLayout::~PipelineLayout() { Release(); }

MM::RenderSystem::PipelineLayout::PipelineLayout(
    MM::RenderSystem::RenderEngine *render_engine,
    MM::RenderSystem::PipelineLayoutType pipeline_layout_type,
    MM::RenderSystem::ShaderSlotCount shader_slot_count)
    : render_engine_(render_engine),
      pipeline_layout_type_(pipeline_layout_type),
      shader_slot_count_(shader_slot_count),
      pipeline_layout_(nullptr) {
#ifdef CHECK_PARAMETERS
  MM_CHECK(CheckInitParameters(), MM_LOG_ERROR("Input parameters are error.");
           render_engine_ = nullptr;
           pipeline_layout_type = PipelineLayoutType::UNDEFINE;
           shader_slot_count = ShaderSlotCount::UNDEFINE; return;)
#endif

  if (IsDefaultPipelineLayout()) {
    switch (shader_slot_count_) {
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_0_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout0();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_1_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout1();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_2_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout2();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_3_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout3();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_4_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout4();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_5_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout5();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_6_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout6();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_7_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout7();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_8_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout8();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_9_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout9();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_10_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout10();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_11_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout11();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_12_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout12();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_13_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout13();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_14_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout14();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_15_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout15();
        return;
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_16_SLOT_BIT:
        pipeline_layout_ =
            render_engine_->GetDescriptorManager().GetDefaultPipelineLayout16();
        return;
      default:
        assert(false);
    }
  }

  MM_CHECK(InitPipelineLayoutWhenNotUseDefault(),
           MM_LOG_ERROR("Failed to initialization VkPipelineLayout.");
           render_engine_ = nullptr;
           pipeline_layout_type = PipelineLayoutType::UNDEFINE;
           shader_slot_count = ShaderSlotCount::UNDEFINE;)
}

MM::RenderSystem::PipelineLayout::PipelineLayout(
    MM::RenderSystem::PipelineLayout &&other) noexcept
    : render_engine_(other.render_engine_),
      pipeline_layout_type_(other.pipeline_layout_type_),
      shader_slot_count_(other.shader_slot_count_),
      pipeline_layout_(other.pipeline_layout_) {
  other.render_engine_ = nullptr;
  other.pipeline_layout_type_ = PipelineLayoutType::UNDEFINE;
  other.shader_slot_count_ = ShaderSlotCount::UNDEFINE;
  other.pipeline_layout_ = nullptr;
}

MM::RenderSystem::PipelineLayout &MM::RenderSystem::PipelineLayout::operator=(
    MM::RenderSystem::PipelineLayout &&other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  render_engine_ = other.render_engine_;
  pipeline_layout_type_ = other.pipeline_layout_type_;
  shader_slot_count_ = other.shader_slot_count_;
  pipeline_layout_ = other.pipeline_layout_;

  other.render_engine_ = nullptr;
  other.pipeline_layout_type_ = PipelineLayoutType::UNDEFINE;
  other.shader_slot_count_ = ShaderSlotCount::UNDEFINE;
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
  return pipeline_layout_type_;
}

MM::RenderSystem::ShaderSlotCount
MM::RenderSystem::PipelineLayout::GetShaderSlotCount() const {
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  ShaderSlotCount temp = ShaderSlotCount{static_cast<std::uint64_t>(
      (static_cast<std::uint64_t>(shader_slot_count_)) &
      static_cast<std::uint64_t>(0x1FFFFFF))};
  if (temp == ShaderSlotCount::UNDEFINE) {
    return 0;
  }

  switch (temp) {
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_0_SLOT_BIT:
      return 0;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT:
      return 1;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT:
      return 2;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT:
      return 3;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT:
      return 4;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT:
      return 5;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT:
      return 6;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT:
      return 7;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT:
      return 8;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT:
      return 9;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT:
      return 10;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT:
      return 11;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT:
      return 12;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT:
      return 13;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT:
      return 14;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT:
      return 15;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT:
      return 16;
    default:
      assert(false);
  }
}

std::uint8_t MM::RenderSystem::PipelineLayout::ComputeShaderSlotCount() {
  if (!IsAllPipelineLayout() && !IsComputePipelineLayout()) {
    return 0;
  }

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
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

  std::uint8_t temp = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (temp == 0) {
    return 0;
  }

  return temp - 1;
}

std::uint8_t MM::RenderSystem::PipelineLayout::AllShaderSlotCount() {
  if (!IsAllPipelineLayout()) {
    return 0;
  }

  switch (shader_slot_count_) {
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_0_SLOT_BIT:
      return 0;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_1_SLOT_BIT:
      return 1;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_2_SLOT_BIT:
      return 2;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_3_SLOT_BIT:
      return 3;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_4_SLOT_BIT:
      return 4;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_5_SLOT_BIT:
      return 5;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_6_SLOT_BIT:
      return 6;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_7_SLOT_BIT:
      return 7;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_8_SLOT_BIT:
      return 8;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_9_SLOT_BIT:
      return 9;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_10_SLOT_BIT:
      return 10;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_11_SLOT_BIT:
      return 11;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_12_SLOT_BIT:
      return 12;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_13_SLOT_BIT:
      return 13;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_14_SLOT_BIT:
      return 14;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_15_SLOT_BIT:
      return 15;
    case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_16_SLOT_BIT:
      return 16;
    default:
      assert(false);
  }
}

bool MM::RenderSystem::PipelineLayout::IsValid() const {
  return render_engine_ != nullptr &&
         pipeline_layout_type_ != PipelineLayoutType::UNDEFINE &&
         shader_slot_count_ != ShaderSlotCount::UNDEFINE &&
         pipeline_layout_ != nullptr;
}

void MM::RenderSystem::PipelineLayout::Release() {
  if (IsValid() && !IsDefaultPipelineLayout()) {
    vkDestroyPipelineLayout(render_engine_->GetDevice(), pipeline_layout_,
                            nullptr);
  }
}

MM::ExecuteResult MM::RenderSystem::PipelineLayout::CheckInitParameters() {
  if (render_engine_ == nullptr || !render_engine_->IsValid()) {
    MM_LOG_ERROR("The input parameters render_engine is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (pipeline_layout_type_ == PipelineLayoutType::UNDEFINE) {
    MM_LOG_ERROR("The input parameters pipeline_layout_type is error.");
    return ExecuteResult ::INITIALIZATION_FAILED;
  }

  if (pipeline_layout_type_ == PipelineLayoutType::GRAPHICS) {
    std::uint64_t use_info =
        static_cast<std::uint64_t>(shader_slot_count_) & 0x7FFFFFFFF;
    if (use_info == 0) {
      MM_LOG_ERROR("The input parameters shader_slot_count is error.");
      return ExecuteResult ::INITIALIZATION_FAILED;
    }
    return ExecuteResult ::SUCCESS;
  }

  if (pipeline_layout_type_ == PipelineLayoutType::COMPUTE) {
    std::uint64_t use_info =
        static_cast<std::uint64_t>(shader_slot_count_) & 0x1F;
    if (use_info == 0) {
      MM_LOG_ERROR("The input parameters shader_slot_count is error.");
      return ExecuteResult ::INITIALIZATION_FAILED;
    }
    return ExecuteResult ::SUCCESS;
  }

  if (pipeline_layout_type_ == PipelineLayoutType::RAY) {
    std::uint64_t use_info =
        static_cast<std::uint64_t>(shader_slot_count_) & 0x3FFFFFFF;
    if (use_info == 0) {
      MM_LOG_ERROR("The input parameters shader_slot_count is error.");
      return ExecuteResult ::INITIALIZATION_FAILED;
    }
    return ExecuteResult ::SUCCESS;
  }

  if (pipeline_layout_type_ == PipelineLayoutType::ALL ||
      pipeline_layout_type_ == PipelineLayoutType::DEFAULT) {
    switch (shader_slot_count_) {
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_0_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_1_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_2_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_3_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_4_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_5_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_6_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_7_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_8_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_9_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_10_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_11_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_12_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_13_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_14_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_15_SLOT_BIT:
      case ShaderSlotCount::SHADER_STAGE_ALL_STAGE_16_SLOT_BIT:
        return ExecuteResult ::SUCCESS;

      default:
        return ExecuteResult ::INITIALIZATION_FAILED;
    }
  }
}

MM::ExecuteResult
MM::RenderSystem::PipelineLayout::InitPipelineLayoutWhenNotUseDefault() {
  VkShaderStageFlagBits a;
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

MM::ExecuteResult
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
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData1)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData2)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData3)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData4)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData5)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData6)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData7)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData8)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData9)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData10)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData11)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData12)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData13)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData14)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT:
      ranger[0] = VkPushConstantRange{VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                      sizeof(PushData15)};
      range_count = 1;
      is_all_graphics_stage = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT:
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
    MM_VK_CHECK(
        vkCreatePipelineLayout(render_engine_->GetDevice(),
                               &pipeline_layout_create_info, nullptr,
                               &pipeline_layout_),
        MM_LOG_ERROR("Filed to create VkPipelineLayout.");
        return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE))
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
  MM_VK_CHECK(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &pipeline_layout_create_info, nullptr,
                             &pipeline_layout_),
      MM_LOG_ERROR("Filed to create VkPipelineLayout.");
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE))

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
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

  std::uint8_t slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger = VkPushConstantRange{VK_SHADER_STAGE_COMPUTE_BIT, 0,
                                 static_cast<uint32_t>((slot_count - 1) * 8)};
    pipeline_layout_create_info.pushConstantRangeCount = 1;
    pipeline_layout_create_info.pPushConstantRanges = &ranger;
  }

  MM_VK_CHECK(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &pipeline_layout_create_info, nullptr,
                             &pipeline_layout_),
      MM_LOG_ERROR("Filed to create VkPipelineLayout.");
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE))

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::PipelineLayout::InitRayPipelineLayout() {
  std::array<VkDescriptorSetLayout, 2> descriptor_sets{
      render_engine_->GetDescriptorManager().GetGlobalDescriptorSetLayout(),
      render_engine_->GetDescriptorManager().GetMaterialDescriptorSetLayout()};
  std::array<VkPushConstantRange, 6> ranger{};
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
  MM_VK_CHECK(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &pipeline_layout_create_info, nullptr,
                             &pipeline_layout_),
      MM_LOG_ERROR("Filed to create VkPipelineLayout.");
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE))

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult
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

  std::uint8_t slot_count = static_cast<std::uint8_t>(
      ((static_cast<std::uint64_t>(shader_slot_count_) >> 0) &
       static_cast<std::uint64_t>(0b11111)));
  if (slot_count != 0) {
    ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI, 0,
                                 static_cast<uint32_t>((slot_count - 1) * 8)};
    pipeline_layout_create_info.pushConstantRangeCount = 1;
    pipeline_layout_create_info.pPushConstantRanges = &ranger;
  }

  MM_VK_CHECK(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &pipeline_layout_create_info, nullptr,
                             &pipeline_layout_),
      MM_LOG_ERROR("Filed to create VkPipelineLayout.");
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE))

  return ExecuteResult ::SUCCESS;
}

MM::ExecuteResult MM::RenderSystem::PipelineLayout::InitAllPipelineLayout() {
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
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_1_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData1)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_2_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData2)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_3_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData3)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_4_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData4)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_5_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData5)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_6_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData6)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_7_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData7)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_8_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData8)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_9_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData9)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_10_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData10)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_11_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData11)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_12_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData12)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_13_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData13)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_14_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData14)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_15_SLOT_BIT:
      ranger = VkPushConstantRange{VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
                                   0, sizeof(PushData15)};
      is_have_push_constant = true;
      break;
    case ShaderSlotCount::SHADER_STAGE_ALL_GRAPHICS_16_SLOT_BIT:
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

  MM_VK_CHECK(
      vkCreatePipelineLayout(render_engine_->GetDevice(),
                             &pipeline_layout_create_info, nullptr,
                             &pipeline_layout_),
      MM_LOG_ERROR("Filed to create VkPipelineLayout.");
      return MM::RenderSystem::Utils::VkResultToMMResult(MM_VK_RESULT_CODE))

  return ExecuteResult ::SUCCESS;
}
