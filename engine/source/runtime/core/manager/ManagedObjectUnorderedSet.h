#pragma once

#include <cstdint>
#include <functional>
#include <unordered_set>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/utils.h"
#include "utils/hash_table.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>,
          typename IsCanMoved = CanMoved>
class ManagedObjectUnorderedSet final
    : public ManagedObjectTableBase<ObjectType, ObjectType, HashSetTrait> {
 public:
  using ContainerTrait = HashSetTrait;
  using CanMovedTrait = IsCanMoved;
  using ThisType = ManagedObjectUnorderedSet<ObjectType, Hash, Equal, Allocator,
                                             CanMovedTrait>;
  using BaseType =
      ManagedObjectTableBase<ObjectType, ObjectType, ContainerTrait>;
  using HashKeyType = ObjectType;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = Utils::HashSet<
      WrapperType, typename WrapperType::template HashWrapperObject<Hash>,
      typename WrapperType::template EqualWrapperObject<Equal>, Allocator>;
  using ContainerReturnType = const WrapperType;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value,
      std::uint64_t bucket_count);

 public:
  ManagedObjectUnorderedSet()
      : ManagedObjectTableBase<ObjectType, ObjectType, HashSetTrait>(this),
        data_(1024) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ~ManagedObjectUnorderedSet() {
    if (size_.load(std::memory_order_acquire) != 0) {
      MM_LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedSet(std::uint32_t size)
      : ManagedObjectTableBase<ObjectType, ObjectType, ListTrait>(this),
        data_(size) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ManagedObjectUnorderedSet(const ManagedObjectUnorderedSet& other) = delete;
  ManagedObjectUnorderedSet(ManagedObjectUnorderedSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CanMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");

    LockAll guard{other};

    BaseType::operator=(std::move(other));
    data_ = std::move(other);
    size_.store(other.size_.load(std::memory_order_acquire),
                std::memory_order_relaxed);
    other.size_.store(0, std::memory_order_release);
  }
  ManagedObjectUnorderedSet& operator=(const ManagedObjectUnorderedSet& other) =
      delete;
  ManagedObjectUnorderedSet& operator=(
      ManagedObjectUnorderedSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CanMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");

    if (&other == this) {
      return *this;
    }

    std::lock(data_mutex0_, data_mutex1_, data_mutex2_, data_mutex3_,
              data_mutex4_, data_mutex5_, data_mutex6_, data_mutex7_,
              data_mutex8_, data_mutex9_, data_mutex10_, data_mutex11_,
              data_mutex12_, data_mutex13_, data_mutex14_, data_mutex15_,
              other.data_mutex0_, other.data_mutex1_, other.data_mutex2_,
              other.data_mutex3_, other.data_mutex4_, other.data_mutex5_,
              other.data_mutex6_, other.data_mutex7_, other.data_mutex8_,
              other.data_mutex9_, other.data_mutex10_, other.data_mutex11_,
              other.data_mutex12_, other.data_mutex13_, other.data_mutex14_,
              other.data_mutex15_);
    if (size_.load(std::memory_order_acquire) != 0) {
      MM_LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }
    LockAll main_guard(*this, std::adopt_lock),
        other_guard(other, std::adopt_lock);

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);
    size_.store(other.size_.load(std::memory_order_acquire),
                std::memory_order_relaxed);
    other.size_.store(0, std::memory_order_release);

    return *this;
  }

 public:
  size_t GetSize() const override {
    return size_.load(std::memory_order_relaxed);
  }

  void Reserve(std::uint64_t new_size) {
    LockAll guard(*this);
    data_.ReHash(new_size);
  }

  bool Have(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard(ChooseMutexIn(key));
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Contains(key);
  }

  uint32_t GetSize(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Count(key);
  }

  bool IsMultiContainer() const override { return false; }

  bool IsRelationshipContainer() const override { return false; }

  Result<HandlerType, ErrorResult> AddObject(ObjectType&& managed_object) {
    if (!ThisType::TestMovedWhenAddObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error,
                                              ErrorCode::OBJECT_IS_INVALID);
    }

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(managed_object)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(managed_object);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(managed_object);
      }
    }

    std::pair<ContainerReturnType&, bool> insert_result =
        data_.Emplace(std::move(managed_object));

    if (!insert_result.second) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, ErrorCode::OPERATION_NOT_SUPPORTED);
    }
    Result<HandlerType, ErrorResult> result{
        st_execute_success, BaseType::GetThisPtrPtr(),
        const_cast<ObjectType*>(insert_result.first.GetObjectPtr()),
        insert_result.first.GetUseCountPtr()};

    size_.fetch_add(1, std::memory_order_acq_rel);

    return result;
  }

  MM::Result<HandlerType, ErrorResult> GetObject(const ObjectType& key) const {
    if (!BaseType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error,
                                              ErrorCode::OBJECT_IS_INVALID);
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    ContainerReturnType* iter = data_.Find(key);

    if (iter != nullptr) {
      return Result<HandlerType, ErrorResult>(
          st_execute_success, BaseType::GetThisPtrPtr(),
          const_cast<ObjectType*>(iter->GetObjectPtr()),
          iter->GetUseCountPtr());
    }

    return Result<HandlerType, ErrorResult>(
        st_execute_error,
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  uint32_t GetUseCount(const ObjectType& key) const {
    if (!ThisType::TestMovedWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    ContainerReturnType* iter = data_.Find(key);
    if (iter != nullptr) {
      return iter->GetUseCount();
    }

    return 0;
  }

 protected:
  Result<Nil, ErrorResult> RemoveObjectImp(const ObjectType& removed_object_key,
                                           HashSetTrait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(removed_object_key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(removed_object_key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(removed_object_key);
      }
    }

    if (ThisType::this_ptr_ptr_ == nullptr) {
      return Result<Nil, ErrorResult>(st_execute_error,
                                      ErrorCode::CUSTOM_ERROR);
    }

    auto iter = data_.Find(removed_object_key);

    if (iter == nullptr) {
      return Result<Nil, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    if (iter->GetUseCount() == 0) {
      data_.Erase(iter);
      size_.fetch_sub(1, std::memory_order_acq_rel);
    }

    return Result<Nil, ErrorResult>(st_execute_success);
  }

 private:
  void ResizeWhenNeeded() {
    if (data_.BucketCount() - size_.load(std::memory_order_acquire) < 128) {
      std::uint32_t new_size = data_.BucketCount() * 2;
      if (new_size < 2048) {
        new_size = 2048;
      }
      Reserve(new_size);
    }
  }

  std::shared_mutex& ChooseMutexIn(const ContainerReturnType& key) const {
    return ChooseMutex(*this, typename WrapperType::HashWrapperObject{}(key),
                       data_.BucketCount());
  }

  std::shared_mutex& ChooseMutexIn(const ObjectType& key) const {
    return ChooseMutex(*this, Hash{}(key), data_.BucketCount());
  }

 private:
  ContainerType data_{};
  std::atomic_uint64_t size_{0};

  mutable std::shared_mutex data_mutex0_{};
  mutable std::shared_mutex data_mutex1_{};
  mutable std::shared_mutex data_mutex2_{};
  mutable std::shared_mutex data_mutex3_{};
  mutable std::shared_mutex data_mutex4_{};
  mutable std::shared_mutex data_mutex5_{};
  mutable std::shared_mutex data_mutex6_{};
  mutable std::shared_mutex data_mutex7_{};
  mutable std::shared_mutex data_mutex8_{};
  mutable std::shared_mutex data_mutex9_{};
  mutable std::shared_mutex data_mutex10_{};
  mutable std::shared_mutex data_mutex11_{};
  mutable std::shared_mutex data_mutex12_{};
  mutable std::shared_mutex data_mutex13_{};
  mutable std::shared_mutex data_mutex14_{};
  mutable std::shared_mutex data_mutex15_{};
};

template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>,
          typename IsCanMoved = CanMoved>
class ManagedObjectUnorderedMultiSet
    : public ManagedObjectTableBase<ObjectType, ObjectType, HashSetTrait> {
 public:
  using RelationshipContainerTrait = HashSetTrait;
  using CanMovedTrait = IsCanMoved;
  using ThisType = ManagedObjectUnorderedMultiSet<ObjectType, Hash, Equal,
                                                  Allocator, CanMovedTrait>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType,
                                          RelationshipContainerTrait>;
  using HashKeyType = ObjectType;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = Utils::MultiHashSet<
      WrapperType, typename WrapperType::template HashWrapperObject<Hash>,
      typename WrapperType::template EqualWrapperObject<Equal>, Allocator>;
  using ContainerReturnType = const WrapperType;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value,
      std::uint64_t bucket_count);

 public:
  ManagedObjectUnorderedMultiSet()
      : ManagedObjectTableBase<ObjectType, ObjectType, HashSetTrait>(this),
        data_(1024) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ~ManagedObjectUnorderedMultiSet() {
    if (size_.load(std::memory_order_acquire) != 0) {
      MM_LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedMultiSet(std::uint32_t size)
      : ManagedObjectTableBase<ObjectType, ObjectType, ListTrait>(this),
        data_(size) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ManagedObjectUnorderedMultiSet(const ManagedObjectUnorderedMultiSet& other) =
      delete;
  ManagedObjectUnorderedMultiSet(
      ManagedObjectUnorderedMultiSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CanMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");
    LockAll guard(other);

    BaseType::operator=(std::move(other));
    data_ = std::move(data_);
    size_.store(other.size_.load(std::memory_order_acquire),
                std::memory_order_relaxed);
    other.size_.store(0, std::memory_order_release);
  }
  ManagedObjectUnorderedMultiSet& operator=(
      const ManagedObjectUnorderedMultiSet& other) = delete;
  ManagedObjectUnorderedMultiSet& operator=(
      ManagedObjectUnorderedMultiSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CanMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");
    if (&other == this) {
      return *this;
    }

    std::lock(data_mutex0_, data_mutex1_, data_mutex2_, data_mutex3_,
              data_mutex4_, data_mutex5_, data_mutex6_, data_mutex7_,
              data_mutex8_, data_mutex9_, data_mutex10_, data_mutex11_,
              data_mutex12_, data_mutex13_, data_mutex14_, data_mutex15_,
              other.data_mutex0_, other.data_mutex1_, other.data_mutex2_,
              other.data_mutex3_, other.data_mutex4_, other.data_mutex5_,
              other.data_mutex6_, other.data_mutex7_, other.data_mutex8_,
              other.data_mutex9_, other.data_mutex10_, other.data_mutex11_,
              other.data_mutex12_, other.data_mutex13_, other.data_mutex14_,
              other.data_mutex15_);
    if (size_.load(std::memory_order_acquire) != 0) {
      MM_LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }
    LockAll main_guard(*this, std::adopt_lock),
        other_guard(other, std::adopt_lock);

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);
    size_.store(other.size_.load(std::memory_order_acquire),
                std::memory_order_relaxed);
    other.size_.store(0, std::memory_order_release);

    return *this;
  }

 public:
  size_t GetSize() const override {
    return size_.load(std::memory_order_relaxed);
  }

  void Reserve(std::uint64_t new_size) {
    LockAll guard(*this);
    data_.ReHash(new_size);
  }

  bool Have(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Count(key) != 0;
  }

  uint32_t GetSize(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Count(key);
  }

  bool IsMultiContainer() const override { return true; }

  bool IsRelationshipContainer() const override { return false; }

  MM::Result<HandlerType, ErrorResult> AddObject(ObjectType&& managed_object) {
    if (!ThisType::TestMovedWhenAddObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error,
                                              ErrorCode::OBJECT_IS_INVALID);
    }

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(managed_object)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(managed_object);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(managed_object);
      }
    }

    std::pair<ContainerReturnType&, bool> iter =
        data_.Emplace(std::move(managed_object));
    if (!iter.second) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error, ErrorCode::OPERATION_NOT_SUPPORTED);
    }

    Result<HandlerType, ErrorResult> result(
        st_execute_success, BaseType::GetThisPtrPtr(),
        const_cast<ObjectType*>(iter.first.GetObjectPtr()),
        iter.first.GetUseCountPtr());

    size_.fetch_add(1, std::memory_order_acq_rel);

    return result;
  }

  MM::Result<HandlerType, ErrorResult> GetObject(const ObjectType& key, StaticTrait::GetOneObject) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error,
                                              ErrorCode::OBJECT_IS_INVALID);
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    ContainerReturnType* iter = data_.Find(key);

    return Result<HandlerType, ErrorResult>(
        st_execute_success, BaseType ::GetThisPtrPtr(),
        const_cast<ObjectType*>(iter->GetObjectPtr()), iter->GetUseCountPtr());
  }

  MM::Result<HandlerType, ErrorResult> GetObject(const ObjectType& key) const {
      return GetObject(key, st_get_one_object);
  }

  MM::Result<HandlerType, ErrorResult> GetObject(
      const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error,
                                              ErrorCode::OBJECT_IS_INVALID);
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }
    std::vector<ContainerReturnType*> equal_range = data_.equal_range(key);
    if (equal_range.empty()) {
      return Result<HandlerType, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      if (equal_range[i]->second.GetUseCountPtr() == use_count_ptr) {
        return Result<HandlerType, ErrorResult>(
            st_execute_success, BaseType::GetThisPtrPtr(),
            equal_range[i]->first,
            const_cast<ObjectType*>(equal_range[i]->second.GetObjectPtr()),
            equal_range[i]->second.GetUseCountPtr());
      }
    }

    return Result<HandlerType, ErrorResult>(
        st_execute_error,
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  MM::Result<std::vector<HandlerType>, ErrorResult> GetObject(
      const ObjectType& key, StaticTrait::GetMultiplyObject) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return Result<std::vector<HandlerType>, ErrorResult>(
          st_execute_error, ErrorCode::OBJECT_IS_INVALID);
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::vector<const ContainerReturnType*> equal_range = data_.EqualRange(key);
    if (equal_range.empty()) {
      return Result<std::vector<HandlerType>, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    std::vector<HandlerType> handlers;
    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      handlers.emplace_back(
          BaseType::GetThisPtrPtr(),
          const_cast<ObjectType*>(equal_range[i]->GetObjectPtr()),
          equal_range[i]->GetUseCountPtr());
    }

    return Result<std::vector<HandlerType>, ErrorResult>(st_execute_success,
                                                         std::move(handlers));
  }

  std::uint32_t GetUseCount(const ObjectType& key, StaticTrait::GetOneObject) const {
    if (!ThisType::TestMovedWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    ContainerReturnType* iter = data_.Find(key);

    if (iter == nullptr) {
      return 0;
    }

    return iter->GetUseCount();
  }

  std::uint32_t GetUseCount(const ObjectType& key) const {
      return GetUseCount(key, st_get_one_object);
  }

  Result<std::vector<std::uint32_t>, ErrorResult> GetUseCount(
      const ObjectType& key, StaticTrait::GetMultiplyObject) const {
    if (!ThisType::TestMovedWhenGetUseCount()) {
      return Result<std::vector<std::uint32_t>, ErrorResult>(
          st_execute_error, ErrorCode::OBJECT_IS_INVALID);
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::vector<const ContainerReturnType*> equal_range = data_.EqualRange(key);
    if (equal_range.empty()) {
      return Result<std::vector<std::uint32_t>, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    std::vector<std::uint32_t> use_counts;
    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      use_counts.emplace_back(equal_range[i]->GetUseCount());
    }

    return Result<std::vector<std::uint32_t>, ErrorResult>(
        st_execute_success, std::move(use_counts));
  }

 protected:
  Result<Nil, ErrorResult> RemoveObjectImp(
      const ObjectType& removed_object_key,
      const std::atomic_uint32_t* use_count_ptr,
      RelationshipContainerTrait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(removed_object_key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(removed_object_key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(removed_object_key);
      }
    }

    if (ThisType::this_ptr_ptr_ == nullptr) {
      return Result<Nil, ErrorResult>(st_execute_error,
                                      ErrorCode::CUSTOM_ERROR);
    }

    std::vector<ContainerReturnType*> equal_range =
        data_.EqualRange(removed_object_key);
    if (equal_range.empty()) {
      return Result<Nil, ErrorResult>(
          st_execute_error,
          ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      if (equal_range[i]->GetUseCountPtr() == use_count_ptr) {
        if (equal_range[i]->GetUseCount() == 0) {
          data_.Erase(equal_range[i]);
          size_.fetch_sub(1, std::memory_order_acq_rel);
        }

        return Result<Nil, ErrorResult>(st_execute_success);
      }
    }

    return Result<Nil, ErrorResult>(
        st_execute_error,
        ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

 private:
  void ResizeWhenNeeded() {
    if (data_.BucketCount() - size_.load(std::memory_order_acquire) < 128) {
      std::uint32_t new_size = data_.BucketCount() * 2;
      if (new_size < 2048) {
        new_size = 2048;
      }
      Reserve(new_size);
    }
  }

  std::shared_mutex& ChooseMutexIn(const ObjectType& key) const {
    return ChooseMutex(*this, Hash{}(key), data_.BucketCount());
  }

 private:
  ContainerType data_;
  std::atomic_uint64_t size_{0};

  mutable std::shared_mutex data_mutex0_{};
  mutable std::shared_mutex data_mutex1_{};
  mutable std::shared_mutex data_mutex2_{};
  mutable std::shared_mutex data_mutex3_{};
  mutable std::shared_mutex data_mutex4_{};
  mutable std::shared_mutex data_mutex5_{};
  mutable std::shared_mutex data_mutex6_{};
  mutable std::shared_mutex data_mutex7_{};
  mutable std::shared_mutex data_mutex8_{};
  mutable std::shared_mutex data_mutex9_{};
  mutable std::shared_mutex data_mutex10_{};
  mutable std::shared_mutex data_mutex11_{};
  mutable std::shared_mutex data_mutex12_{};
  mutable std::shared_mutex data_mutex13_{};
  mutable std::shared_mutex data_mutex14_{};
  mutable std::shared_mutex data_mutex15_{};
};
}  // namespace Manager
}  // namespace MM
