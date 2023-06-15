#include "bounding_box.h"

MM::AssetSystem::AssetType::BoundingBox&
MM::AssetSystem::AssetType::BoundingBox::operator=(const BoundingBox& other) {
  if (&other == this) {
    return *this;
  }
  return *this;
}

MM::AssetSystem::AssetType::BoundingBox&
MM::AssetSystem::AssetType::BoundingBox::operator=(
    BoundingBox&& other) noexcept {
  if (this == &other) return *this;
  return *this;
}

MM::AssetSystem::AssetType::RectangleBox::RectangleBox(
    const Math::vec3& left_bottom_forward, const Math::vec3& right_top_back)
    : left_bottom_forward(left_bottom_forward),
      right_top_back(right_top_back) {}

MM::AssetSystem::AssetType::RectangleBox&
MM::AssetSystem::AssetType::RectangleBox::operator=(const RectangleBox& other) {
  if (&other == this) {
    return *this;
  }
  left_bottom_forward = other.left_bottom_forward;
  right_top_back = other.right_top_back;

  return *this;
}

MM::AssetSystem::AssetType::RectangleBox&
MM::AssetSystem::AssetType::RectangleBox::operator=(
    RectangleBox&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  left_bottom_forward = other.left_bottom_forward;
  right_top_back = other.right_top_back;

  left_bottom_forward = MathDefinition::VEC3_ZERO;
  right_top_back = MathDefinition::VEC3_ZERO;

  return *this;
}

float& MM::AssetSystem::AssetType::RectangleBox::GetTop() {
  return right_top_back.y;
}

const float& MM::AssetSystem::AssetType::RectangleBox::GetTop() const {
  return right_top_back.y;
}

void MM::AssetSystem::AssetType::RectangleBox::SetTop(const float& new_top) {
  right_top_back.y = new_top;
}

float& MM::AssetSystem::AssetType::RectangleBox::GetBottom() {
  return left_bottom_forward.y;
}

const float& MM::AssetSystem::AssetType::RectangleBox::GetBottom() const {
  return left_bottom_forward.y;
}

void MM::AssetSystem::AssetType::RectangleBox::SetBottom(
    const float& new_bottom) {
  left_bottom_forward.y = new_bottom;
}

float& MM::AssetSystem::AssetType::RectangleBox::GetLeft() {
  return left_bottom_forward.x;
}

const float& MM::AssetSystem::AssetType::RectangleBox::GetLeft() const {
  return left_bottom_forward.x;
}

void MM::AssetSystem::AssetType::RectangleBox::SetLeft(const float& new_left) {
  left_bottom_forward.x = new_left;
}

float& MM::AssetSystem::AssetType::RectangleBox::GetRight() {
  return right_top_back.x;
}

const float& MM::AssetSystem::AssetType::RectangleBox::GetRight() const {
  return right_top_back.x;
}

void MM::AssetSystem::AssetType::RectangleBox::SetRight(
    const float& new_right) {
  right_top_back.x = new_right;
}

float& MM::AssetSystem::AssetType::RectangleBox::GetForward() {
  return left_bottom_forward.z;
}

const float& MM::AssetSystem::AssetType::RectangleBox::GetForward() const {
  return left_bottom_forward.z;
}

void MM::AssetSystem::AssetType::RectangleBox::SetForward(
    const float& new_forward) {
  left_bottom_forward.z = new_forward;
}

float& MM::AssetSystem::AssetType::RectangleBox::GetBack() {
  return right_top_back.z;
}

const float& MM::AssetSystem::AssetType::RectangleBox::GetBack() const {
  return right_top_back.z;
}

void MM::AssetSystem::AssetType::RectangleBox::SetBack(const float& new_back) {
  right_top_back.z = new_back;
}

MM::Math::vec3&
MM::AssetSystem::AssetType::RectangleBox::GetLeftBottomForward() {
  return left_bottom_forward;
}

const MM::Math::vec3&
MM::AssetSystem::AssetType::RectangleBox::GetLeftBottomForward() const {
  return left_bottom_forward;
}

void MM::AssetSystem::AssetType::RectangleBox::SetLeftBottomForward(
    const Math::vec3& new_left_bottom_forward) {
  left_bottom_forward = new_left_bottom_forward;
}

MM::Math::vec3& MM::AssetSystem::AssetType::RectangleBox::GetRightTopBack() {
  return right_top_back;
}

const MM::Math::vec3&
MM::AssetSystem::AssetType::RectangleBox::GetRightTopBack() const {
  return right_top_back;
}

void MM::AssetSystem::AssetType::RectangleBox::SetRightTopBack(
    const Math::vec3& new_right_top_back) {
  right_top_back = new_right_top_back;
}

bool MM::AssetSystem::AssetType::RectangleBox::IsValid() const {
  if (left_bottom_forward.x > right_top_back.x) {
    return false;
  }
  if (left_bottom_forward.y > right_top_back.y) {
    return false;
  }
  if (left_bottom_forward.z > right_top_back.z) {
    return false;
  }
  return true;
}

MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType
MM::AssetSystem::AssetType::RectangleBox::GetBoundingType() const {
  return BoundingBoxType::AABB;
}

MM::AssetSystem::AssetType::CapsuleBox&
MM::AssetSystem::AssetType::CapsuleBox::operator=(const CapsuleBox& other) {
  if (&other == this) {
    return *this;
  }
  radius_ = other.radius_;
  top_ = other.top_;
  bottom_ = other.bottom_;

  return *this;
}

MM::AssetSystem::AssetType::CapsuleBox&
MM::AssetSystem::AssetType::CapsuleBox::operator=(CapsuleBox&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  radius_ = other.radius_;
  top_ = other.top_;
  bottom_ = other.bottom_;

  other.radius_ = 0;
  other.top_ = 0;
  other.bottom_ = 0;

  return *this;
}

float& MM::AssetSystem::AssetType::CapsuleBox::GetRadius() { return radius_; }

const float& MM::AssetSystem::AssetType::CapsuleBox::GetRadius() const {
  return radius_;
}

void MM::AssetSystem::AssetType::CapsuleBox::SetRadius(
    const float& new_radius) {
  radius_ = new_radius;
}

float& MM::AssetSystem::AssetType::CapsuleBox::GetTop() { return top_; }

const float& MM::AssetSystem::AssetType::CapsuleBox::GetTop() const {
  return top_;
}

void MM::AssetSystem::AssetType::CapsuleBox::SetTop(const float& new_top) {
  top_ = new_top;
}

float& MM::AssetSystem::AssetType::CapsuleBox::GetBottom() { return bottom_; }

const float& MM::AssetSystem::AssetType::CapsuleBox::GetBottom() const {
  return bottom_;
}

void MM::AssetSystem::AssetType::CapsuleBox::SetBottom(
    const float& new_bottom) {
  bottom_ = new_bottom;
}

MM::AssetSystem::AssetType::RectangleBox
MM::AssetSystem::AssetType::CapsuleBox::GetRectangleBoxFormThis() const {
  return RectangleBox{{-radius_, bottom_ - radius_, -radius_},
                      {radius_, top_ + radius_, radius_}};
}

bool MM::AssetSystem::AssetType::CapsuleBox::IsValid() const {
  return std::abs(radius_) > 1e-6;
}

MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType
MM::AssetSystem::AssetType::CapsuleBox::GetBoundingType() const {
  return BoundingBoxType::CAPSULE;
}
