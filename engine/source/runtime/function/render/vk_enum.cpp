//
// Created by beimingxianyu on 23-7-31.
//

#include "runtime/function/render/vk_enum.h"

MM::RenderSystem::DynamicState MM::RenderSystem::operator|(
    const DynamicState& lhs, const DynamicState& rhs) {
  return DynamicState{static_cast<std::uint64_t>(lhs) |
                      static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::DynamicState& MM::RenderSystem::operator|=(
    DynamicState& lhs, const DynamicState& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::DynamicState MM::RenderSystem::operator&(
    const DynamicState& lhs, const DynamicState& rhs) {
  return DynamicState{static_cast<std::uint64_t>(lhs) &
                      static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::DynamicState& MM::RenderSystem::operator&=(
    DynamicState& lhs, const DynamicState& rhs) {
  lhs = lhs & rhs;
  return lhs;
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator|(
    const ShaderStage& lhs, const ShaderStage& rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) |
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage& MM::RenderSystem::operator|=(
    ShaderStage& lhs, const ShaderStage& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator&(
    const ShaderStage& lhs, const ShaderStage& rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) &
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage& MM::RenderSystem::operator&=(
    ShaderStage& lhs, const ShaderStage& rhs) {
  lhs = lhs & rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotDescriptor MM::RenderSystem::operator|(
    const ShaderSlotDescriptor& lhs, const ShaderSlotDescriptor& rhs) {
  return ShaderSlotDescriptor{static_cast<std::uint64_t>(lhs) |
                              static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotDescriptor& MM::RenderSystem::operator|=(
    ShaderSlotDescriptor& lhs, const ShaderSlotDescriptor& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotDescriptor MM::RenderSystem::operator&(
    const ShaderSlotDescriptor& lhs, const ShaderSlotDescriptor& rhs) {
  return ShaderSlotDescriptor{static_cast<std::uint64_t>(lhs) &
                              static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotDescriptor& MM::RenderSystem::operator&=(
    ShaderSlotDescriptor& lhs, const ShaderSlotDescriptor& rhs) {
  lhs = lhs & rhs;
  return lhs;
}
