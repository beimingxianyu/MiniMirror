//
// Created by beimingxianyu on 23-7-31.
//

#include "runtime/function/render/vk_enum.h"

MM::RenderSystem::DynamicState MM::RenderSystem::operator|(
    const MM::RenderSystem::DynamicState& lhs,
    const MM::RenderSystem::DynamicState& rhs) {
  return DynamicState{static_cast<std::uint64_t>(lhs) |
                      static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::DynamicState& MM::RenderSystem::operator|=(
    MM::RenderSystem::DynamicState& lhs,
    const MM::RenderSystem::DynamicState& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::DynamicState MM::RenderSystem::operator&(
    const MM::RenderSystem::DynamicState& lhs,
    const MM::RenderSystem::DynamicState& rhs) {
  return DynamicState{static_cast<std::uint64_t>(lhs) &
                      static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::DynamicState& MM::RenderSystem::operator&=(
    MM::RenderSystem::DynamicState& lhs,
    const MM::RenderSystem::DynamicState& rhs) {
  lhs = lhs & rhs;
  return lhs;
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator|(
    const MM::RenderSystem::ShaderStage& lhs,
    const MM::RenderSystem::ShaderStage& rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) |
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage& MM::RenderSystem::operator|=(
    MM::RenderSystem::ShaderStage& lhs,
    const MM::RenderSystem::ShaderStage& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator&(
    const MM::RenderSystem::ShaderStage& lhs,
    const MM::RenderSystem::ShaderStage& rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) &
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage& MM::RenderSystem::operator&=(
    MM::RenderSystem::ShaderStage& lhs,
    const MM::RenderSystem::ShaderStage& rhs) {
  lhs = lhs & rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator|(
    const MM::RenderSystem::ShaderSlotCount& lhs,
    const MM::RenderSystem::ShaderSlotCount& rhs) {
  return ShaderSlotCount{static_cast<std::uint64_t>(lhs) |
                         static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotCount& MM::RenderSystem::operator|=(
    MM::RenderSystem::ShaderSlotCount& lhs,
    const MM::RenderSystem::ShaderSlotCount& rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator&(
    const MM::RenderSystem::ShaderSlotCount& lhs,
    const MM::RenderSystem::ShaderSlotCount& rhs) {
  return ShaderSlotCount{static_cast<std::uint64_t>(lhs) &
                         static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotCount& MM::RenderSystem::operator&=(
    MM::RenderSystem::ShaderSlotCount& lhs,
    const MM::RenderSystem::ShaderSlotCount& rhs) {
  lhs = lhs & rhs;
  return lhs;
}
