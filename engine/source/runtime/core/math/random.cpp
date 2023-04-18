#include "runtime/core/math/random.h"

namespace MM {
namespace Math {
std::mt19937* Random::rng32_{new std::mt19937()};
std::mt19937_64* Random::uid64_rng_{new std::mt19937_64()};
std::uniform_int_distribution<std::uint32_t> Random::uid_32_{0, UINT32_MAX};
std::uniform_int_distribution<std::uint64_t> Random::uid_64_{0, UINT64_MAX};
std::uniform_real_distribution<float> Random::urd_0_1_{0, 1};
std::mutex Random::sync_flag_{};

float Random::Uniform() { return urd_0_1_(*rng32_); }

int Random::Uniform(const int& min, const int& max) {
  std::uniform_int_distribution<int> temp(min, max);
  return temp(*rng32_);
}

float Random::Uniform(const float& min, const float& max) {
  std::uniform_real_distribution<float> temp(min, max);
  return temp(*rng32_);
}

float Random::Normal(const float& mean, const float& sigma) {
  std::normal_distribution<float> temp(mean, sigma);
  return temp(*rng32_);
}

std::uint32_t Random::GetRandomUint32() { return uid_32_(rng32_); }

std::uint64_t Random::GetRandomUint64() { return uid_64_(uid64_rng_); }

std::uint64_t Random::NewGuid(const std::size_t& hash) {
  std::lock_guard<std::mutex> guard{sync_flag_};
  uid64_rng_->seed(hash);
  return uid_64_(*uid64_rng_);
}

void Random::SetSeed(const unsigned& seed) { rng32_->seed(seed); }
}
}
