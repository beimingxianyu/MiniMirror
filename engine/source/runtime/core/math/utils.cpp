#include "runtime/core/math/utils.h"

namespace glm {
glm::quat QuatFromRotationMatrix(const glm::mat3& rotation) {
  glm::quat quaternion;
  const float trace = rotation[0][0] + rotation[1][1] + rotation[2][2];
  float root;

  if (trace > 0.0f) {
    // |w| > 1/2, may as well choose w > 1/2
    root = std::sqrt(trace + 1.0f);  // 2w
    quaternion.w = 0.5f * root;
    root = 0.5f / root;  // 1/(4w)
    quaternion.x = (rotation[2][1] - rotation[1][2]) * root;
    quaternion.y = (rotation[0][2] - rotation[2][0]) * root;
    quaternion.z = (rotation[1][0] - rotation[0][1]) * root;
  } else {
    // |w| <= 1/2
    constexpr size_t s_iNext[3] = {1, 2, 0};
    size_t i = 0;
    if (rotation[1][1] > rotation[0][0]) i = 1;
    if (rotation[2][2] > rotation[i][i]) i = 2;
    const size_t j = s_iNext[i];
    const size_t k = s_iNext[j];

    root = std::sqrt(rotation[i][i] - rotation[j][j] - rotation[k][k] + 1.0f);
    float* apkQuat[3] = {&quaternion.x, &quaternion.y, &quaternion.z};
    *apkQuat[i] = 0.5f * root;
    root = 0.5f / root;
    quaternion.w = (rotation[k][j] - rotation[j][k]) * root;
    *apkQuat[j] = (rotation[j][i] + rotation[i][j]) * root;
    *apkQuat[k] = (rotation[k][i] + rotation[i][k]) * root;
  }
  return quaternion;
}

glm::quat QuatFromRotationMatrix(const glm::mat4& rotation) {
  glm::quat quaternion;
  const float trace = rotation[0][0] + rotation[1][1] + rotation[2][2];
  float root;

  if (trace > 0.0f) {
    // |w| > 1/2, may as well choose w > 1/2
    root = std::sqrt(trace + 1.0f);  // 2w
    quaternion.w = 0.5f * root;
    root = 0.5f / root;  // 1/(4w)
    quaternion.x = (rotation[2][1] - rotation[1][2]) * root;
    quaternion.y = (rotation[0][2] - rotation[2][0]) * root;
    quaternion.z = (rotation[1][0] - rotation[0][1]) * root;
  } else {
    // |w| <= 1/2
    constexpr size_t s_iNext[3] = {1, 2, 0};
    size_t i = 0;
    if (rotation[1][1] > rotation[0][0]) i = 1;
    if (rotation[2][2] > rotation[i][i]) i = 2;
    const size_t j = s_iNext[i];
    const size_t k = s_iNext[j];

    root = std::sqrt(rotation[i][i] - rotation[j][j] - rotation[k][k] + 1.0f);
    float* apkQuat[3] = {&quaternion.x, &quaternion.y, &quaternion.z};
    *apkQuat[i] = 0.5f * root;
    root = 0.5f / root;
    quaternion.w = (rotation[k][j] - rotation[j][k]) * root;
    *apkQuat[j] = (rotation[j][i] + rotation[i][j]) * root;
    *apkQuat[k] = (rotation[k][i] + rotation[i][k]) * root;
  }
  return quaternion;
}

glm::quat QuatFromAngleAxis(const float& angle, const glm::vec3& axis) {
  const float half_angle(0.5 * angle);
  const float sin_v = glm::sin(half_angle);
  return glm::quat{glm::cos(half_angle), sin_v * axis.x, sin_v * axis.y,
                   sin_v * axis.z};
}

glm::quat QuatFromDirection(const glm::vec3& direction, const glm::vec3& up_direction) {
  glm::vec3 forward_direction = direction;
  forward_direction.z = 0.0f;
  forward_direction = glm::normalize(forward_direction);

  const glm::vec3 left_direction = glm::cross(up_direction, forward_direction);

  return glm::normalize(QuatFromAxes(left_direction, -forward_direction, up_direction));
}

glm::quat QuatFromAxes(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis) {
  glm::mat3 rot;

  rot[0][0] = x_axis.x;
  rot[1][0] = x_axis.y;
  rot[2][0] = x_axis.z;

  rot[0][1] = y_axis.x;
  rot[1][1] = y_axis.y;
  rot[2][1] = y_axis.z;

  rot[0][2] = z_axis.x;
  rot[1][2] = z_axis.y;
  rot[2][2] = z_axis.z;

  return QuatFromRotationMatrix(rot);
}

glm::mat3 QuatToRotationMatrix3(const glm::quat& quaternion) {
  float fTx = quaternion.x + quaternion.x;  // 2x
  float fTy = quaternion.y + quaternion.y;  // 2y
  float fTz = quaternion.z + quaternion.z;  // 2z
  float fTwx = fTx * quaternion.w;          // 2xw
  float fTwy = fTy * quaternion.w;          // 2yw
  float fTwz = fTz * quaternion.w;          // 2z2
  float fTxx = fTx * quaternion.x;          // 2x^2
  float fTxy = fTy * quaternion.x;          // 2xy
  float fTxz = fTz * quaternion.x;          // 2xz
  float fTyy = fTy * quaternion.y;          // 2y^2
  float fTyz = fTz * quaternion.y;          // 2yz
  float fTzz = fTz * quaternion.z;          // 2z^2

  return glm::mat3{1.0f - (fTyy + fTzz), fTxy - fTwz, fTxz + fTwy,
                   fTxy + fTwz, 1.0f - (fTxx + fTzz),fTyz - fTwx,
                   fTxz - fTwy, fTyz + fTwx, 1.0f - (fTxx + fTyy)
         };
  }

glm::mat4 QuatToRotationMatrix4(const glm::quat& quaternion) {
  float fTx = quaternion.x + quaternion.x;  // 2x
  float fTy = quaternion.y + quaternion.y;  // 2y
  float fTz = quaternion.z + quaternion.z;  // 2z
  float fTwx = fTx * quaternion.w;          // 2xw
  float fTwy = fTy * quaternion.w;          // 2yw
  float fTwz = fTz * quaternion.w;          // 2z2
  float fTxx = fTx * quaternion.x;          // 2x^2
  float fTxy = fTy * quaternion.x;          // 2xy
  float fTxz = fTz * quaternion.x;          // 2xz
  float fTyy = fTy * quaternion.y;          // 2y^2
  float fTyz = fTz * quaternion.y;          // 2yz
  float fTzz = fTz * quaternion.z;          // 2z^2

  return glm::mat4{1.0f - (fTyy + fTzz), fTxy - fTwz, fTxz + fTwy, 0.0f,
                   fTxy + fTwz,1.0f - (fTxx + fTzz), fTyz - fTwx, 0.0f,
                   fTxz - fTwy, fTyz + fTwx, 1.0f - (fTxx + fTyy), 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f
         };
}
}
