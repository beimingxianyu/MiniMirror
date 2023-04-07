#pragma once

#include <memory>
#include <random>
#include <mutex>

namespace MM {
namespace Math {
class Random {
public:
  static float Uniform();

  static int Uniform(const int& min, const int& max);

  static float Uniform(const float& min, const float& max);

  static float Normal(const float& mean, const float& sigma);

  static uint64_t NewGuid(const std::size_t& hash);

  static void SetSeed(const unsigned int& seed);

private:
  static std::mt19937* rng_;
  static std::mt19937_64* guid_rng_;
  static std::uniform_real_distribution<float> urd_0_1_;
  static std::uniform_int_distribution<uint64_t> guid_64_;
  static std::mutex sync_flag_;
};
}
}  // namespace MM