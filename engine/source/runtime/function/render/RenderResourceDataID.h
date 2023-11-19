#pragma once

#include <optional>

#include "runtime/function/render/RenderDataAttributeID.h"
#include "runtime/resource/asset_system/AssetSystem.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"

namespace MM {
namespace RenderSystem {
class RenderResourceDataID {
 public:
  RenderResourceDataID() = default;
  RenderResourceDataID(AssetSystem::AssetType::AssetID asset_ID,
                       const RenderResourceDataAttributeID& resource_attribute);
  RenderResourceDataID(const RenderResourceDataID& other) = default;
  RenderResourceDataID(RenderResourceDataID&& other) noexcept = default;
  RenderResourceDataID& operator=(const RenderResourceDataID& other) = default;
  RenderResourceDataID& operator=(RenderResourceDataID&& other) noexcept =
      default;

 public:
  bool operator==(const RenderResourceDataID& rhs) const;

  bool operator!=(const RenderResourceDataID& rhs) const;

  bool operator<(const RenderResourceDataID& rhs) const;

  bool operator>(const RenderResourceDataID& rhs) const;

  bool operator<=(const RenderResourceDataID& rhs) const;

  bool operator>=(const RenderResourceDataID& rhs) const;

  friend void Swap(RenderResourceDataID& lhs,
                   RenderResourceDataID& rhs) noexcept;

  friend void swap(RenderResourceDataID& lhs,
                   RenderResourceDataID& rhs) noexcept;

 public:
  AssetSystem::AssetType::AssetID GetAssetID() const;

  RenderResourceDataAttributeID GetResourceAttributeID() const;

  void SetAssetID(AssetSystem::AssetType::AssetID asset_ID);

  void SetResourceAttributeID(
      const RenderResourceDataAttributeID& render_resource_data_attribute_ID);

  void SetResourceAttributeIDAttribute1(
      std::uint64_t render_resource_data_attribute_ID_attribute1);

  void SetResourceAttributeIDAttribute2(
      std::uint64_t render_resource_data_attribute_ID_attribute2);

  void Reset() {
    asset_ID_ = 0;
    resource_attribute_ID_.Reset();
  }

 private:
  AssetSystem::AssetType::AssetID asset_ID_{0};
  RenderResourceDataAttributeID resource_attribute_ID_{};
};
}  // namespace RenderSystem
}  // namespace MM

template <>
struct std::hash<MM::RenderSystem::RenderResourceDataID> {
  std::uint64_t operator()(
      const MM::RenderSystem::RenderResourceDataID& object) const noexcept {
    return object.GetAssetID() ^ object.GetResourceAttributeID().GetHashCode();
  }
};
