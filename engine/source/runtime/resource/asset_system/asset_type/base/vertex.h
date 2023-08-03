#pragma once

#include "runtime/core/math/math.h"
#include "utils/marco.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
class Vertex {
 public:
  Vertex() = default;
  ~Vertex() = default;
  Vertex(const Vertex& other) = default;
  Vertex(Vertex&& other) noexcept;
  Vertex& operator=(const Vertex& other);
  Vertex& operator=(Vertex&& other) noexcept;

  friend bool operator==(const Vertex& lhs, const Vertex& rhs);

  friend bool operator!=(const Vertex& lhs, const Vertex& rhs);

 public:
  Math::vec3& GetPosition();
  const Math::vec3& GetPosition() const;
  void SetPosition(const Math::vec3& new_position);

  Math::vec2& GetTextureCoord();
  const Math::vec2& GetTextureCoord() const;
  void SetTextureCoord(const Math::vec2& new_texture_coord);

  Math::vec3& GetNormal();
  const Math::vec3& GetNormal() const;
  void SetNormal(const Math::vec3& new_normal);

  Math::vec3& GetTangent();
  const Math::vec3& GetTangent() const;
  void SetTangent(const Math::vec3& new_tangent);

  Math::vec3& GetBiTangent();
  const Math::vec3& GetBiTangent() const;
  void SetBiTangent(const Math::vec3& new_bi_tangent);

  bool HaveTextureCoord() const;

  bool HaveNormal() const;

  bool HaveTangent() const;

  bool HaveBiTangent() const;

  void Reset();

  friend void Swap(Vertex& lhs, Vertex& rhs) noexcept;

  friend void swap(Vertex& lhs, Vertex& rhs) noexcept;

  constexpr static std::uint32_t GetOffsetOfPosition();

  constexpr static std::uint32_t GetOffsetOfNormal();

  constexpr static std::uint32_t GetOffsetOfTangent();

  constexpr static std::uint32_t GetOffsetOfBiTangent();

  constexpr static std::uint32_t GetOffsetOfTextureCoord();

 private:
  // Memory alignment for shader reading.
  Math::vec3 position_{MathDefinition::VEC3_ZERO};
  alignas(16) Math::vec2 texture_coord_{MathDefinition::VEC2_ZERO};
  alignas(16) Math::vec3 normal_{MathDefinition::VEC3_ZERO};
  alignas(16) Math::vec3 tangent_{MathDefinition::VEC3_ZERO};
  alignas(16) Math::vec3 bi_tangent_{MathDefinition::VEC3_ZERO};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
