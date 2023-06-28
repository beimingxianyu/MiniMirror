#pragma once

#include <functional>
#include <optional>

namespace MM {
namespace RenderSystem {
class RenderResourceDataAttributeID {
 public:
  RenderResourceDataAttributeID() = default;
  ~RenderResourceDataAttributeID() = default;
  RenderResourceDataAttributeID(std::uint64_t resource_attribute1,
                                std::uint64_t resource_attribute2);
  RenderResourceDataAttributeID(const RenderResourceDataAttributeID& other) =
      default;
  RenderResourceDataAttributeID(
      RenderResourceDataAttributeID&& other) noexcept = default;
  RenderResourceDataAttributeID& operator=(
      const RenderResourceDataAttributeID& other) = default;
  RenderResourceDataAttributeID& operator=(
      RenderResourceDataAttributeID&& other) noexcept = default;

 public:
  bool operator==(const RenderResourceDataAttributeID& rhs) const;

  bool operator!=(const RenderResourceDataAttributeID& rhs) const;

  bool operator<(const RenderResourceDataAttributeID& rhs) const;

  bool operator>(const RenderResourceDataAttributeID& rhs) const;

  bool operator<=(const RenderResourceDataAttributeID& rhs) const;

  bool operator>=(const RenderResourceDataAttributeID& rhs) const;

  friend void Swap(RenderResourceDataAttributeID& lhs,
                   RenderResourceDataAttributeID& rhs) noexcept;

  friend void swap(RenderResourceDataAttributeID& lhs,
                   RenderResourceDataAttributeID& rhs) noexcept;

 public:
  std::uint64_t GetResourceAttribute1() const;

  std::uint64_t GetResourceAttribute2() const;

  void SetResourceAttribute1(std::uint64_t new_resource_attribute1);

  void SetResourceAttribute2(std::uint64_t new_resource_attribute2);

 private:
  std::uint64_t resource_attribute1_{0};
  std::uint64_t resource_attribute2_{0};
};
// clang-format off
/** resource_attribute1_
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - | - - - - - - | - - - | - -  | - - |
 * |           pNext                 |              flags                    |IType|      format     |   mipLevels | arrayLayers |samples|tiling|SMod |
 * |           16bit                 |              19bits                   |2bits|      8bits      |     6bits   |    6bits    | 3bits |2bits |2bits|
 * |  The last 16 bits of pointer.   |              Bitmask                  |value|      value      |     value   |    value    | value |value |value|
 * | - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - | - - | - - - - - - - - | - - - - - - | - - - - - - | - - - | - -  | - - |
 *
 * resource_attribute2_
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - | - - - - - |
 * |        ext.width          |         ext.height        |          ext.depth        |                 usage                   | initLayout|
 * |         13bits            |           13bits          |           13bits          |                 20bits                  |    5bits  |
 * |         value             |           value           |           value           |                 Bitmask                 |    value  |
 * | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - | - - - - - |
**/
// clang-format on
using RenderImageDataAttribute = RenderResourceDataAttributeID;

// clang-format off
/** resource_attribute1_
 * | - - - - - - - - - - - - - - - - | - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - |
 * |           pNext                 |      flags      |                     usage                         |               empty           |
 * |           16bit                 |      8bits      |                     25bits                        |               empty           |
 * |  The last 16 bits of pointer.   |      Bitmask    |                     Bitmask                       |               empty           |
 * | - - - - - - - - - - - - - - - - | - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - |
 *
 * resource_attribute2_
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
 * |                                                         size                                                                    |
 * |                                                         64bits                                                                  |
 * |                                                         value                                                                   |
 * | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - |
**/
// clang-format on
using RenderBufferDataAttribute = RenderResourceDataAttributeID;
}  // namespace RenderSystem
}  // namespace MM

template <>
struct std::hash<MM::RenderSystem::RenderResourceDataAttributeID> {
  std::uint64_t operator()(
      const MM::RenderSystem::RenderResourceDataAttributeID& object) {
    return object.GetResourceAttribute1() ^ object.GetResourceAttribute2();
  }
};
