#include "runtime/core/math/transform.h"
#include "runtime/core/math/utils.h"

namespace MM {

Transform::Transform(const glm::vec3& position, const glm::vec3& scale,
                     const glm::quat& rotation)
    : position_(position), scale_(scale), rotation_(rotation) {}

glm::mat4 Transform::GetMatrix() const {
  glm::mat3 rot3x3{glm::QuatToRotationMatrix3(rotation_)};
  return glm::mat4{scale_.x * rot3x3[0][0],
                   scale_.y * rot3x3[0][1],
                   scale_.z * rot3x3[0][2],
                   position_.x,
                   scale_.x * rot3x3[1][0],
                   scale_.y * rot3x3[1][1],
                   scale_.z * rot3x3[1][2],
                   position_.x,
                   scale_.x * rot3x3[2][0],
                   scale_.y * rot3x3[2][1],
                   scale_.z * rot3x3[2][2],
                   position_.x,
                   0.0f,
                   0.0f,
                   0.0f,
                   1.0f};
}
}  // namespace MM
