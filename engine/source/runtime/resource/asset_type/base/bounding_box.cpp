#include "runtime/resource/asset_type/base/bounding_box.h"

MM::AssetType::BoundingBox& MM::AssetType::BoundingBox::operator=(
    const BoundingBox& other) {
  if (&other == this) {
    return *this;
  }
  return *this;
}

MM::AssetType::BoundingBox& MM::AssetType::BoundingBox::operator=(
    BoundingBox&& other) noexcept {
  if (this == &other)
    return *this;
  return *this;
}

MM::AssetType::AABBBox::AABBBox(const Math::vec3& left_bottom_forward,
    const Math::vec3& right_top_back)
  : left_bottom_forward(left_bottom_forward),
    right_top_back(right_top_back) {
}

MM::AssetType::AABBBox& MM::AssetType::AABBBox::operator
=(const AABBBox& other) {
  if (&other == this) {
    return *this;
  }
  left_bottom_forward = other.left_bottom_forward;
  right_top_back = other.right_top_back;

  return *this;
}

MM::AssetType::AABBBox& MM::AssetType::AABBBox::operator=(
    AABBBox&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  left_bottom_forward = other.left_bottom_forward;
  right_top_back = other.right_top_back;

  left_bottom_forward = MathDefinition::VEC3_ZERO;
  right_top_back = MathDefinition::VEC3_ZERO;

  return *this;
}

float& MM::AssetType::AABBBox::GetTop() { return right_top_back.y; }

const float& MM::AssetType::AABBBox::GetTop() const { return right_top_back.y; }

void MM::AssetType::AABBBox::SetTop(const float& new_top) {
  right_top_back.y = new_top;
}

float& MM::AssetType::AABBBox::GetBottom() { return left_bottom_forward.y; }

const float& MM::AssetType::AABBBox::GetBottom() const {
  return left_bottom_forward.y;
}

void MM::AssetType::AABBBox::SetBottom(const float& new_bottom) {
  left_bottom_forward.y = new_bottom;
}

float& MM::AssetType::AABBBox::GetLeft() { return left_bottom_forward.x; }

const float& MM::AssetType::AABBBox::GetLeft() const {
  return left_bottom_forward.x;
}

void MM::AssetType::AABBBox::SetLeft(const float& new_left) {
  left_bottom_forward.x = new_left;
}

float& MM::AssetType::AABBBox::GetRight() { return right_top_back.x; }

const float& MM::AssetType::AABBBox::GetRight() const {
  return right_top_back.x;
}

void MM::AssetType::AABBBox::SetRight(const float& new_right) {
  right_top_back.x = new_right;
}

float& MM::AssetType::AABBBox::GetForward() { return left_bottom_forward.z; }

const float& MM::AssetType::AABBBox::GetForward() const {
  return left_bottom_forward.z;
}

void MM::AssetType::AABBBox::SetForward(const float& new_forward) {
  left_bottom_forward.z = new_forward;
}

float& MM::AssetType::AABBBox::GetBack() { return right_top_back.z; }

const float& MM::AssetType::AABBBox::GetBack() const {
  return right_top_back.z;
}

void MM::AssetType::AABBBox::SetBack(const float& new_back) {
  right_top_back.z = new_back;
}

MM::Math::vec3& MM::AssetType::AABBBox::GetLeftBottomForward() {
  return left_bottom_forward;
}

const MM::Math::vec3& MM::AssetType::AABBBox::GetLeftBottomForward() const {
  return left_bottom_forward;
}

void MM::AssetType::AABBBox::SetLeftBottomForward(
    const Math::vec3& new_left_bottom_forward) {
  left_bottom_forward = new_left_bottom_forward;
}

MM::Math::vec3& MM::AssetType::AABBBox::GetRightTopBack() {
  return right_top_back;
}

const MM::Math::vec3& MM::AssetType::AABBBox::GetRightTopBack() const {
  return right_top_back;
}

void MM::AssetType::AABBBox::SetRightTopBack(
    const Math::vec3& new_right_top_back) {
  right_top_back = new_right_top_back;
}


bool MM::AssetType::AABBBox::IsValid() const {
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

MM::AssetType::BoundingBox::BoundingBoxType MM::AssetType::AABBBox::
GetBoundingType() const {
  return BoundingBoxType::AABB;
}

MM::AssetType::CapsuleBox& MM::AssetType::CapsuleBox::operator=(
    const CapsuleBox& other) {
  if (&other == this) {
    return *this;
  }
  radius_ = other.radius_;
  top_ = other.top_;
  bottom_ = other.bottom_;

  return *this;
}

MM::AssetType::CapsuleBox& MM::AssetType::CapsuleBox::operator=(
    CapsuleBox&& other) noexcept {
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

float& MM::AssetType::CapsuleBox::GetRadius() { return radius_; }

const float& MM::AssetType::CapsuleBox::GetRadius() const {
  return radius_;
}

void MM::AssetType::CapsuleBox::SetRadius(const float& new_radius) {
  radius_ = new_radius;
}

float& MM::AssetType::CapsuleBox::GetTop() { return top_; }

const float& MM::AssetType::CapsuleBox::GetTop() const { return top_; }

void MM::AssetType::CapsuleBox::SetTop(const float& new_top) { top_ = new_top; }

float& MM::AssetType::CapsuleBox::GetBottom() { return bottom_; }

const float& MM::AssetType::CapsuleBox::GetBottom() const { return bottom_; }

void MM::AssetType::CapsuleBox::SetBottom(const float& new_bottom) {
  bottom_ = new_bottom;
}

MM::AssetType::AABBBox MM::AssetType::CapsuleBox::GetAABBBoxFormThis() const {
  return AABBBox{{-radius_, bottom_ - radius_, -radius_},
                 {radius_, top_ + radius_, radius_}};
}

bool MM::AssetType::CapsuleBox::IsValid() const { return radius_ != 0; }

MM::AssetType::BoundingBox::BoundingBoxType MM::AssetType::CapsuleBox::
GetBoundingType() const {
  return BoundingBoxType::CAPSULE;
}
