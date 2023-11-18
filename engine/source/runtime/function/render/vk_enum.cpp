//
// Created by beimingxianyu on 23-7-31.
//

#include "runtime/function/render/vk_enum.h"

MM::RenderSystem::DynamicState MM::RenderSystem::operator|(
    const DynamicState& lhs,
    const DynamicState& rhs) {
  return DynamicState{static_cast<std::uint64_t>(lhs) |
                      static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::DynamicState& MM::RenderSystem::operator|=(
    DynamicState& lhs,
    const DynamicState& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::DynamicState MM::RenderSystem::operator&(
    const DynamicState& lhs,
    const DynamicState& rhs) {
  return DynamicState{static_cast<std::uint64_t>(lhs) &
                      static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::DynamicState& MM::RenderSystem::operator&=(
    DynamicState& lhs,
    const DynamicState& rhs) {
  lhs = lhs & rhs;
  return lhs;
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator|(
    const ShaderStage& lhs,
    const ShaderStage& rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) |
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage& MM::RenderSystem::operator|=(
    ShaderStage& lhs,
    const ShaderStage& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator&(
    const ShaderStage& lhs,
    const ShaderStage& rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) &
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage& MM::RenderSystem::operator&=(
    ShaderStage& lhs,
    const ShaderStage& rhs) {
  lhs = lhs & rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator|(
    const ShaderSlotCount& lhs,
    const ShaderSlotCount& rhs) {
  return ShaderSlotCount{static_cast<std::uint64_t>(lhs) |
                         static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotCount& MM::RenderSystem::operator|=(
    ShaderSlotCount& lhs,
    const ShaderSlotCount& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator&(
    const ShaderSlotCount& lhs,
    const ShaderSlotCount& rhs) {
  return ShaderSlotCount{static_cast<std::uint64_t>(lhs) &
                         static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotCount& MM::RenderSystem::operator&=(
    ShaderSlotCount& lhs,
    const ShaderSlotCount& rhs) {
  lhs = lhs & rhs;
  return lhs;
}
