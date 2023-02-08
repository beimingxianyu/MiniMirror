#include "runtime/core/math/random.h"

namespace MM {
inline float Random::Uniform() { return urd_0_1_(*rng_); }

inline int Random::Uniform(const int& min, const int& max) {
  std::uniform_int_distribution<int> temp(min, max);
  return temp(*rng_);
}

inline float Random::Uniform(const float& min, const float& max) {
  std::uniform_real_distribution<float> temp(min, max);
  return temp(*rng_);
}

inline float Random::Normal(const float& mean, const float& sigma) {
  std::normal_distribution<float> temp(mean, sigma);
  return temp(*rng_);
}

inline void Random::SetSeed(const unsigned& seed) { rng_->seed(seed); }
}
