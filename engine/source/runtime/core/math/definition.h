#pragma once


#include <glm/ext.hpp>

namespace MM {
namespace MathDefinition {
static const float MATH_POS_INF = std::numeric_limits<float>::infinity();
static const float MATH_NEG_INF = -std::numeric_limits<float>::infinity();
static constexpr float MATH_PI = 3.14159265358979323846264338327950288f;
static constexpr float MATH_DEG2RAD = MATH_PI / 180.0f;
static constexpr float MATH_RAD2DEG = 180.0f / MATH_PI;
static constexpr float MATH_EPSILON = 1e-6f;

static constexpr float FLOAT_EPSILON = FLT_EPSILON;

static glm::vec2 VEC2_ZERO{0.0f, 0.0f};
static glm::vec2 VEC2_ONE{1.0f, 1.0f};

static glm::vec3 VEC3_ZERO{0.0f, 0.0f, 0.0f};
static glm::vec3 VEC3_ONE{1.0f, 1.0f, 1.0f};

static glm::vec4 VEC4_ZERO{0.0f, 0.0f, 0.0f, 0.0f};
static glm::vec4 VEC4_ONES{1.0f, 1.0f, 1.0f, 1.0f};

static glm::mat2 MAT2_ZERO{0.0f, 0.0f, 0.0f, 0.0f};
static glm::mat2 MAT2_IDENTITY{1.0f, 0.0f, 0.0f, 1.0f};

static glm::mat3 MAT3_ZERO{
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};
static glm::mat3 MAT3_IDENTITY{
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

static glm::mat4 MAT4_ZERO{
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};
static glm::mat4 MAT4_IDENTITY{
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

static glm::quat QUAT_ZERO{0.0f, 0.0f, 0.0f, 0.0f};
static glm::quat QUAT_IDENTITY{1.0f, 0.0f, 0.0f, 0.0f};
};  // namespace MathDefinition
}  // namespace MM