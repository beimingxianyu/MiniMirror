#include "runtime/core//color/color.h"

glm::vec3 MM::Color::GetColor() const { return glm::vec3{r_, g_, b_}; }

glm::vec4 MM::Color::GetColorWithAlpha() const {
  return glm::vec4{r_, g_, b_, a_};
}

void MM::Color::SetR(const float& r) { r_ = r; }

void MM::Color::SetG(const float& g) { g_ = g; }

void MM::Color::SetB(const float& b) { b_ = b; }

void MM::Color::SetA(const float& a) { a_ = a; }

float MM::Color::GetR(const float& r) { return r_; }

float MM::Color::GetG(const float& g) { return g_; }

float MM::Color::GetB(const float& b) { return b_; }

float MM::Color::GetA(const float& a) { return a_; }
