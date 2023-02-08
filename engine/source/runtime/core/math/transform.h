#pragma once

#include <glm/ext.hpp>

namespace MM {
// TODO: MM Reflection Transform
class Transform {
 public:
  glm::vec3 position_{0.0f, 0.0f, 0.0f};
  glm::vec3 scale_{1.0f, 1.0f, 1.0f};
  glm::quat rotation_{1.0f, 0.0f, 0.0f, 0.0f};

  Transform() = default;
  Transform(const glm::vec3& position, const glm::vec3& scale,
            const glm::quat& rotation);
  glm::mat4 GetMatrix() const;
  
};
}  // namespace MM
