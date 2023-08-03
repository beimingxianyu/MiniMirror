#include "runtime/resource/asset_system/asset_type/base/vertex.h"

MM::AssetSystem::AssetType::Vertex::Vertex(Vertex&& other) noexcept
    : position_(other.position_),
      texture_coord_(other.texture_coord_),
      normal_(other.normal_),
      tangent_(other.tangent_),
      bi_tangent_(other.bi_tangent_) {
  other.Reset();
}

MM::AssetSystem::AssetType::Vertex&
MM::AssetSystem::AssetType::Vertex::operator=(const Vertex& other) {
  if (&other == this) {
    return *this;
  }
  position_ = other.position_;
  texture_coord_ = other.texture_coord_;
  normal_ = other.normal_;
  tangent_ = other.tangent_;
  bi_tangent_ = other.bi_tangent_;

  return *this;
}

MM::AssetSystem::AssetType::Vertex&
MM::AssetSystem::AssetType::Vertex::operator=(Vertex&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  position_ = other.position_;
  texture_coord_ = other.texture_coord_;
  normal_ = other.normal_;
  tangent_ = other.tangent_;
  bi_tangent_ = other.bi_tangent_;

  other.Reset();

  return *this;
}

MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetPosition() {
  return position_;
}

const MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetPosition() const {
  return position_;
}

void MM::AssetSystem::AssetType::Vertex::SetPosition(
    const Math::vec3& new_position) {
  position_ = new_position;
}

MM::Math::vec2& MM::AssetSystem::AssetType::Vertex::GetTextureCoord() {
  return texture_coord_;
}

const MM::Math::vec2& MM::AssetSystem::AssetType::Vertex::GetTextureCoord()
    const {
  return texture_coord_;
}

void MM::AssetSystem::AssetType::Vertex::SetTextureCoord(
    const Math::vec2& new_texture_coord) {
  texture_coord_ = Math::Normalize(new_texture_coord);
}

MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetNormal() {
  return normal_;
}

const MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetNormal() const {
  return normal_;
}

void MM::AssetSystem::AssetType::Vertex::SetNormal(
    const Math::vec3& new_normal) {
  normal_ = Math::Normalize(new_normal);
}

MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetTangent() {
  return tangent_;
}

const MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetTangent() const {
  return tangent_;
}

void MM::AssetSystem::AssetType::Vertex::SetTangent(
    const Math::vec3& new_tangent) {
  tangent_ = Math::Normalize(new_tangent);
}

MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetBiTangent() {
  return bi_tangent_;
}

const MM::Math::vec3& MM::AssetSystem::AssetType::Vertex::GetBiTangent() const {
  return bi_tangent_;
}

void MM::AssetSystem::AssetType::Vertex::SetBiTangent(
    const Math::vec3& new_bi_tangent) {
  bi_tangent_ = Math::Normalize(new_bi_tangent);
}

bool MM::AssetSystem::AssetType::Vertex::HaveTextureCoord() const {
  return Math::IsNormalize(texture_coord_);
}

bool MM::AssetSystem::AssetType::Vertex::HaveNormal() const {
  return Math::IsNormalize(normal_);
}

bool MM::AssetSystem::AssetType::Vertex::HaveTangent() const {
  return Math::IsNormalize(tangent_);
}

bool MM::AssetSystem::AssetType::Vertex::HaveBiTangent() const {
  return Math::IsNormalize(bi_tangent_);
}

void MM::AssetSystem::AssetType::Vertex::Reset() {
  position_ = MathDefinition::VEC3_ZERO;
  texture_coord_ = MathDefinition::VEC2_ZERO;
  normal_ = MathDefinition::VEC3_ZERO;
  tangent_ = MathDefinition::VEC3_ZERO;
  bi_tangent_ = MathDefinition::VEC3_ZERO;
}

constexpr std::uint32_t
MM::AssetSystem::AssetType::Vertex::GetOffsetOfPosition() {
  return OFFSET_OF(Vertex, position_);
}

constexpr std::uint32_t
MM::AssetSystem::AssetType::Vertex::GetOffsetOfNormal() {
  return offsetof(Vertex, normal_);
}

constexpr std::uint32_t
MM::AssetSystem::AssetType::Vertex::GetOffsetOfTangent() {
  return offsetof(Vertex, tangent_);
}

constexpr std::uint32_t
MM::AssetSystem::AssetType::Vertex::GetOffsetOfBiTangent() {
  return offsetof(Vertex, bi_tangent_);
}

constexpr std::uint32_t
MM::AssetSystem::AssetType::Vertex::GetOffsetOfTextureCoord() {
  return offsetof(Vertex, texture_coord_);
}

bool MM::AssetSystem::AssetType::operator==(const Vertex& lhs,
                                            const Vertex& rhs) {
  return lhs.position_ == rhs.position_ &&
         lhs.texture_coord_ == rhs.texture_coord_ &&
         lhs.normal_ == rhs.normal_ && lhs.tangent_ == rhs.tangent_ &&
         lhs.bi_tangent_ == rhs.bi_tangent_;
}

bool MM::AssetSystem::AssetType::operator!=(const Vertex& lhs,
                                            const Vertex& rhs) {
  return !(lhs == rhs);
}

void MM::AssetSystem::AssetType::Swap(Vertex& lhs, Vertex& rhs) noexcept {
  using std::swap;
  swap(lhs.position_, rhs.position_);
  swap(lhs.texture_coord_, rhs.texture_coord_);
  swap(lhs.normal_, rhs.normal_);
  swap(lhs.tangent_, rhs.tangent_);
  swap(lhs.bi_tangent_, rhs.bi_tangent_);
}

void MM::AssetSystem::AssetType::swap(Vertex& lhs, Vertex& rhs) noexcept {
  using std::swap;
  swap(lhs.position_, rhs.position_);
  swap(lhs.texture_coord_, rhs.texture_coord_);
  swap(lhs.normal_, rhs.normal_);
  swap(lhs.tangent_, rhs.tangent_);
  swap(lhs.bi_tangent_, rhs.bi_tangent_);
}
