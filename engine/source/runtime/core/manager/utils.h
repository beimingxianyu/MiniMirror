#pragma once

#include <mutex>
#include <shared_mutex>

#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {

template <typename ManagedObjectTable>
struct LockAll {
  explicit LockAll(const ManagedObjectTable& object)
      : guard0_(object.data_mutex0_, std::defer_lock),
        guard1_(object.data_mutex1_, std::defer_lock),
        guard2_(object.data_mutex2_, std::defer_lock),
        guard3_(object.data_mutex3_, std::defer_lock),
        guard4_(object.data_mutex4_, std::defer_lock),
        guard5_(object.data_mutex5_, std::defer_lock),
        guard6_(object.data_mutex6_, std::defer_lock),
        guard7_(object.data_mutex7_, std::defer_lock),
        guard8_(object.data_mutex8_, std::defer_lock),
        guard9_(object.data_mutex9_, std::defer_lock),
        guard10_(object.data_mutex10_, std::defer_lock),
        guard11_(object.data_mutex11_, std::defer_lock),
        guard12_(object.data_mutex12_, std::defer_lock),
        guard13_(object.data_mutex13_, std::defer_lock),
        guard14_(object.data_mutex14_, std::defer_lock),
        guard15_(object.data_mutex15_, std::defer_lock) {
    std::lock(guard0_, guard1_, guard2_, guard3_, guard4_, guard5_, guard6_,
              guard7_, guard8_, guard9_, guard10_, guard11_, guard12_, guard13_,
              guard14_, guard15_);
  }

  LockAll(const ManagedObjectTable& object, std::adopt_lock_t adopt)
      : guard0_(object.data_mutex0_, std::adopt_lock),
        guard1_(object.data_mutex1_, std::adopt_lock),
        guard2_(object.data_mutex2_, std::adopt_lock),
        guard3_(object.data_mutex3_, std::adopt_lock),
        guard4_(object.data_mutex4_, std::adopt_lock),
        guard5_(object.data_mutex5_, std::adopt_lock),
        guard6_(object.data_mutex6_, std::adopt_lock),
        guard7_(object.data_mutex7_, std::adopt_lock),
        guard8_(object.data_mutex8_, std::adopt_lock),
        guard9_(object.data_mutex9_, std::adopt_lock),
        guard10_(object.data_mutex10_, std::adopt_lock),
        guard11_(object.data_mutex11_, std::adopt_lock),
        guard12_(object.data_mutex12_, std::adopt_lock),
        guard13_(object.data_mutex13_, std::adopt_lock),
        guard14_(object.data_mutex14_, std::adopt_lock),
        guard15_(object.data_mutex15_, std::adopt_lock) {}

  LockAll(const ManagedObjectTable& object, std::defer_lock_t defer)
      : guard0_(object.data_mutex0_, std::defer_lock),
        guard1_(object.data_mutex1_, std::defer_lock),
        guard2_(object.data_mutex2_, std::defer_lock),
        guard3_(object.data_mutex3_, std::defer_lock),
        guard4_(object.data_mutex4_, std::defer_lock),
        guard5_(object.data_mutex5_, std::defer_lock),
        guard6_(object.data_mutex6_, std::defer_lock),
        guard7_(object.data_mutex7_, std::defer_lock),
        guard8_(object.data_mutex8_, std::defer_lock),
        guard9_(object.data_mutex9_, std::defer_lock),
        guard10_(object.data_mutex10_, std::defer_lock),
        guard11_(object.data_mutex11_, std::defer_lock),
        guard12_(object.data_mutex12_, std::defer_lock),
        guard13_(object.data_mutex13_, std::defer_lock),
        guard14_(object.data_mutex14_, std::defer_lock),
        guard15_(object.data_mutex15_, std::defer_lock) {}

  ~LockAll() = default;
  LockAll(const LockAll& other) = delete;
  LockAll(LockAll&& other) = delete;
  LockAll& operator=(const LockAll& other) = delete;
  LockAll& operator=(LockAll& other) = delete;

  void Unlock() {
    guard0_.unlock();
    guard1_.unlock();
    guard2_.unlock();
    guard3_.unlock();
    guard4_.unlock();
    guard5_.unlock();
    guard6_.unlock();
    guard7_.unlock();
    guard8_.unlock();
    guard9_.unlock();
    guard10_.unlock();
    guard11_.unlock();
    guard12_.unlock();
    guard13_.unlock();
    guard14_.unlock();
    guard15_.unlock();
  }

  std::unique_lock<std::shared_mutex> guard0_{};
  std::unique_lock<std::shared_mutex> guard1_{};
  std::unique_lock<std::shared_mutex> guard2_{};
  std::unique_lock<std::shared_mutex> guard3_{};
  std::unique_lock<std::shared_mutex> guard4_{};
  std::unique_lock<std::shared_mutex> guard5_{};
  std::unique_lock<std::shared_mutex> guard6_{};
  std::unique_lock<std::shared_mutex> guard7_{};
  std::unique_lock<std::shared_mutex> guard8_{};
  std::unique_lock<std::shared_mutex> guard9_{};
  std::unique_lock<std::shared_mutex> guard10_{};
  std::unique_lock<std::shared_mutex> guard11_{};
  std::unique_lock<std::shared_mutex> guard12_{};
  std::unique_lock<std::shared_mutex> guard13_{};
  std::unique_lock<std::shared_mutex> guard14_{};
  std::unique_lock<std::shared_mutex> guard15_{};
};

template <typename ManagedObjectTable>
std::shared_mutex& ChooseMutex(const ManagedObjectTable& managed_object_table,
                               std::uint64_t hash_value) {
  std::uint64_t index = hash_value & 0xF;  // hash_value % 16
  switch (index) {
    case 0:
      return managed_object_table.data_mutex0_;
    case 1:
      return managed_object_table.data_mutex1_;
    case 2:
      return managed_object_table.data_mutex2_;
    case 3:
      return managed_object_table.data_mutex3_;
    case 4:
      return managed_object_table.data_mutex4_;
    case 5:
      return managed_object_table.data_mutex5_;
    case 6:
      return managed_object_table.data_mutex6_;
    case 7:
      return managed_object_table.data_mutex7_;
    case 8:
      return managed_object_table.data_mutex8_;
    case 9:
      return managed_object_table.data_mutex9_;
    case 10:
      return managed_object_table.data_mutex10_;
    case 11:
      return managed_object_table.data_mutex11_;
    case 12:
      return managed_object_table.data_mutex12_;
    case 13:
      return managed_object_table.data_mutex13_;
    case 14:
      return managed_object_table.data_mutex14_;
    case 15:
      return managed_object_table.data_mutex15_;
    default:
      LOG_FATAL("Error branch.");
      return managed_object_table.data_mutex0_;
  }
}
}  // namespace Manager
}  // namespace MM