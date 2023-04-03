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

MM::AssetType::RectangleBox::RectangleBox(const Math::vec3& left_bottom_forward,
    const Math::vec3& right_top_back)
  : left_bottom_forward(left_bottom_forward),
    right_top_back(right_top_back) {
}

MM::AssetType::RectangleBox& MM::AssetType::RectangleBox::operator
=(const RectangleBox& other) {
  if (&other == this) {
    return *this;
  }
  left_bottom_forward = other.left_bottom_forward;
  right_top_back = other.right_top_back;

  return *this;
}

MM::AssetType::RectangleBox& MM::AssetType::RectangleBox::operator=(
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

float& MM::AssetType::RectangleBox::GetTop() { return right_top_back.y; }

const float& MM::AssetType::RectangleBox::GetTop() const { return right_top_back.y; }

void MM::AssetType::RectangleBox::SetTop(const float& new_top) {
  right_top_back.y = new_top;
}

float& MM::AssetType::RectangleBox::GetBottom() { return left_bottom_forward.y; }

const float& MM::AssetType::RectangleBox::GetBottom() const {
  return left_bottom_forward.y;
}

void MM::AssetType::RectangleBox::SetBottom(const float& new_bottom) {
  left_bottom_forward.y = new_bottom;
}

float& MM::AssetType::RectangleBox::GetLeft() { return left_bottom_forward.x; }

const float& MM::AssetType::RectangleBox::GetLeft() const {
  return left_bottom_forward.x;
}

void MM::AssetType::RectangleBox::SetLeft(const float& new_left) {
  left_bottom_forward.x = new_left;
}

float& MM::AssetType::RectangleBox::GetRight() { return right_top_back.x; }

const float& MM::AssetType::RectangleBox::GetRight() const {
  return right_top_back.x;
}

void MM::AssetType::RectangleBox::SetRight(const float& new_right) {
  right_top_back.x = new_right;
}

float& MM::AssetType::RectangleBox::GetForward() { return left_bottom_forward.z; }

const float& MM::AssetType::RectangleBox::GetForward() const {
  return left_bottom_forward.z;
}

void MM::AssetType::RectangleBox::SetForward(const float& new_forward) {
  left_bottom_forward.z = new_forward;
}

float& MM::AssetType::RectangleBox::GetBack() { return right_top_back.z; }

const float& MM::AssetType::RectangleBox::GetBack() const {
  return right_top_back.z;
}

void MM::AssetType::RectangleBox::SetBack(const float& new_back) {
  right_top_back.z = new_back;
}

MM::Math::vec3& MM::AssetType::RectangleBox::GetLeftBottomForward() {
  return left_bottom_forward;
}

const MM::Math::vec3& MM::AssetType::RectangleBox::GetLeftBottomForward() const {
  return left_bottom_forward;
}

void MM::AssetType::RectangleBox::SetLeftBottomForward(
    const Math::vec3& new_left_bottom_forward) {
  left_bottom_forward = new_left_bottom_forward;
}

MM::Math::vec3& MM::AssetType::RectangleBox::GetRightTopBack() {
  return right_top_back;
}

const MM::Math::vec3& MM::AssetType::RectangleBox::GetRightTopBack() const {
  return right_top_back;
}

void MM::AssetType::RectangleBox::SetRightTopBack(
    const Math::vec3& new_right_top_back) {
  right_top_back = new_right_top_back;
}


bool MM::AssetType::RectangleBox::IsValid() const {
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

MM::AssetType::BoundingBox::BoundingBoxType MM::AssetType::RectangleBox::
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

MM::AssetType::RectangleBox MM::AssetType::CapsuleBox::GetRectangleBoxFormThis() const {
  return RectangleBox{{-radius_, bottom_ - radius_, -radius_},
                 {radius_, top_ + radius_, radius_}};
}

bool MM::AssetType::CapsuleBox::IsValid() const { return radius_ != 0; }

MM::AssetType::BoundingBox::BoundingBoxType MM::AssetType::CapsuleBox::
GetBoundingType() const {
  return BoundingBoxType::CAPSULE;
}
