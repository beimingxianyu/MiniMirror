#pragma once

#include "runtime/core/math/definition.h"

namespace MM {
namespace Math {
glm::quat QuatFromRotationMatrix(const glm::mat3& rotation);

glm::quat QuatFromRotationMatrix(const ::glm::mat4& rotation);

glm::quat QuatFromAngleAxis(const float& angle, const glm::vec3& axis);

glm::quat QuatFromDirection(const glm::vec3& direction,
                            const glm::vec3& up_direction);

glm::quat QuatFromAxes(const glm::vec3& x_axis, const glm::vec3& y_axis,
                       const glm::vec3& z_axis);

glm::mat3 QuatToRotationMatrix3(const glm::quat& quaternion);

glm::mat4 QuatToRotationMatrix4(const glm::quat& quaternion);

inline float Deg2Rad(const float& deg) {
  return deg * MM::MathDefinition::MATH_DEG2RAD;
}

inline float Rad2Deg(const float& rad) {
  return rad * MM::MathDefinition::MATH_RAD2DEG;
}

bool NearOne(const float& value);

bool NearOne(const double& value);

bool IsNormalize(const vec2& vec);

bool IsNormalize(const vec3& vec);

bool IsNormalize(const vec4& vec);

bool IsNormalize(const dvec2& dvec);

bool IsNormalize(const dvec3& dvec);

bool IsNormalize(const dvec4& dvec);

template <int L, typename T, qualifier Q = qualifier::defaultp>
vec<L, T, Q> Normalize(const vec<L, T, Q>& vec) {
  return glm::normalize(vec);
}

template <int L, typename T, qualifier Q = qualifier::defaultp>
vec<L, T, Q> Transpose(const vec<L, T, Q>& vec) {
  return glm::transpose(vec);
}

template <int C, int R, typename T, qualifier Q = qualifier::defaultp>
mat<C, R, T, Q> Transpose(const mat<C, R, T, Q>& mat) {
  return glm::transpose(mat);
}

template <int C, int R, typename T, qualifier Q = qualifier::defaultp>
mat<C, R, T, Q> Inverse(const mat<C, R, T, Q>& mat) {
  return glm::inverse(mat);
}
}  // namespace Math
}  // namespace MM
