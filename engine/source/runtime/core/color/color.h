#pragma once

#include "runtime/core/math/math.h"

namespace MM {
// TODO: MM Reflection Color
class Color {
 public:
  /**
   * \brief Get color(no alpha value).
   * \return The color(no alpha value).
   */
  Math::vec3 GetColor() const;

  /**
   * \brief Get color (including alpha value).
   * \return The color (including alpha value).
   */
  glm::vec4 GetColorWithAlpha() const;

  /**
   * \brief Set the r value of rgba.
   * \param r The r value you want to set.
   */
  void SetR(const float& r);

  /**
   * \brief Set the g value of rgba.
   * \param g The g value you want to set.
   */
  void SetG(const float& g);

  /**
   * \brief Set the b value of rgba.
   * \param b The b value you want to set.
   */
  void SetB(const float& b);

  /**
   * \brief Set the a value of rgba.
   * \param a The a value you want to set.
   */
  void SetA(const float& a);

  /**
   * \brief Get the r value of rgba.
   * \param r The r value you want to get.
   */
  float GetR(const float& r);

  /**
   * \brief Get the g value of rgba.
   * \param g The g value you want to get.
   */
  float GetG(const float& g);

  /**
   * \brief Get the b value of rgba.
   * \param b The b value you want to get.
   */
  float GetB(const float& b);

  /**
   * \brief Get the a value of rgba.
   * \param a The a value you want to get.
   */
  float GetA(const float& a);

 private:
  float r_{0.0f};
  float g_{0.0f};
  float b_{0.0f};
  float a_{0.0f};
};
}  // namespace MM
