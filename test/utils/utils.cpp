//
// Created by beimingxianyu on 23-6-8.
//
#include "utils/utils.h"

#include <gtest/gtest.h>

#include <atomic>
#include <thread>

TEST(Utils, SpinSharedMutex) {
  std::uint32_t count = 0;
  MM::Utils::SpinSharedMutex mutex;
  std::atomic_bool go{false};

  std::vector<std::thread> threads;

  std::uint32_t loop_count = 10000000;

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back([&count, &mutex, loop_count, &go]() {
        while (!go.load())
          ;

        MM::Utils::SpinSharedLock guard{mutex};
        for (std::uint32_t i = 0; i != loop_count; ++i) {
          ++count;
        }
      });
    }
    threads.emplace_back([&count, &mutex, loop_count, &go]() {
      while (!go.load())
        ;

      MM::Utils::SpinUniqueLock guard{mutex};
      for (std::uint32_t i = 0; i != loop_count; ++i) {
        ++count;
      }
    });
  }

  go.store(true);

  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(count, 9 * loop_count);

  count = 0;
  go.store(false);
  threads.clear();

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back([&count, &mutex, loop_count, &go]() {
        while (!go.load())
          ;

        MM::Utils::SpinUniqueLock guard{mutex};
        for (std::uint32_t i = 0; i != loop_count; ++i) {
          ++count;
        }
      });
    }
    threads.emplace_back([&count, &mutex, loop_count, &go]() {
      while (!go.load())
        ;

      MM::Utils::SpinSharedLock guard{mutex};
      for (std::uint32_t i = 0; i != loop_count; ++i) {
        ++count;
      }
    });
  }

  go.store(true);

  for (auto& th : threads) {
    th.join();
  }

  ASSERT_LE(count, 9 * loop_count);
}