#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace MM {
namespace Math {
using Qualifier = glm::qualifier;

using vec1 = glm::vec1;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using dvec1 = glm::dvec1;
using dvec2 = glm::dvec2;
using dvec3 = glm::dvec3;
using dvec4 = glm::dvec4;
using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;
using ivec4 = glm::ivec4;
template<int L, typename T, Qualifier Q = Qualifier::defaultp>
using vec = glm::vec<L, T, Q>;

using mat2 = glm::mat2;
using mat3 = glm::mat3;
using mat4 = glm::mat4;
using dmat2 = glm::dmat2;
using dmat3 = glm::dmat3;
using dmat4 = glm::dmat4;
using imat2 = glm::imat2x2;
using imat3 = glm::imat3x3;
using imat4 = glm::imat4x4;
template <int C, int R, typename T, Qualifier Q = Qualifier::defaultp>
using mat = glm::mat<C, R, T, Q>;

using quat = glm::quat;
}
}