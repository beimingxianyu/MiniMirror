#include "runtime/core/math/random.h"

namespace MM {
namespace Math {
float Random::Uniform() { return urd_0_1_(*rng_); }

int Random::Uniform(const int& min, const int& max) {
  std::uniform_int_distribution<int> temp(min, max);
  return temp(*rng_);
}

float Random::Uniform(const float& min, const float& max) {
  std::uniform_real_distribution<float> temp(min, max);
  return temp(*rng_);
}

float Random::Normal(const float& mean, const float& sigma) {
  std::normal_distribution<float> temp(mean, sigma);
  return temp(*rng_);
}

uint64_t Random::NewGuid(const std::size_t& hash) {
  std::lock_guard<std::mutex> guard{sync_flag_};
  guid_rng_->seed(hash);
  return guid_64_(*guid_rng_);
}

void Random::SetSeed(const unsigned& seed) { rng_->seed(seed); }

std::mt19937* Random::rng_{new std::mt19937()};
std::mt19937_64* Random::guid_rng_{new std::mt19937_64()};
std::uniform_int_distribution<uint64_t> Random::guid_64_{0, UINT64_MAX};
std::uniform_real_distribution<float> Random::urd_0_1_{0, 1};
std::mutex Random::sync_flag_{};
}
}
