//
// Created by beimingxianyu on 23-7-31.
//

#include "runtime/function/render/vk_enum.h"

MM::RenderSystem::ShaderStage MM::RenderSystem::operator|(
    MM::RenderSystem::ShaderStage lhs, MM::RenderSystem::ShaderStage rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) |
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator|=(
    MM::RenderSystem::ShaderStage lhs, MM::RenderSystem::ShaderStage rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator&(
    MM::RenderSystem::ShaderStage lhs, MM::RenderSystem::ShaderStage rhs) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(lhs) &
                                  static_cast<std::uint32_t>(rhs));
}

MM::RenderSystem::ShaderStage MM::RenderSystem::operator&=(
    MM::RenderSystem::ShaderStage lhs, MM::RenderSystem::ShaderStage rhs) {
  lhs = lhs & rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator|(
    MM::RenderSystem::ShaderSlotCount lhs,
    MM::RenderSystem::ShaderSlotCount rhs) {
  return ShaderSlotCount{static_cast<std::uint64_t>(lhs) |
                         static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator|=(
    MM::RenderSystem::ShaderSlotCount lhs,
    MM::RenderSystem::ShaderSlotCount rhs) {
  lhs = lhs | rhs;
  return lhs;
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator&(
    MM::RenderSystem::ShaderSlotCount lhs,
    MM::RenderSystem::ShaderSlotCount rhs) {
  return ShaderSlotCount{static_cast<std::uint64_t>(lhs) &
                         static_cast<std::uint64_t>(rhs)};
}

MM::RenderSystem::ShaderSlotCount MM::RenderSystem::operator&=(
    MM::RenderSystem::ShaderSlotCount lhs,
    MM::RenderSystem::ShaderSlotCount rhs) {
  lhs = lhs & rhs;
  return lhs;
}
