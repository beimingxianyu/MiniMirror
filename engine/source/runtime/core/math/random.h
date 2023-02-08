#pragma once

#include <memory>
#include <random>

namespace MM {
class Random {
 public:
  Random() {
    if (!rng_) {
      rng_ = new std::mt19937();
      urd_0_1_ = std::uniform_real_distribution<float>(0, 1);
      rng_->seed(time(0));
    }
  }

  float Uniform();

  int Uniform(const int& min, const int& max);

  float Uniform(const float& min, const float& max);

  float Normal(const float& mean, const float& sigma);

  void SetSeed(const unsigned int& seed);

 private:
  static std::mt19937* rng_;
  static std::uniform_real_distribution<float> urd_0_1_;
};
}  // namespace MM