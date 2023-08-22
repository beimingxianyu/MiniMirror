#pragma once

#include <cstdint>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/utils.h"
#include "utils/hash_table.h"

namespace MM {
namespace Manager {
template <
    typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>,
    typename Equal = std::equal_to<KeyType>,
    typename Allocator =
        std::allocator<std::pair<KeyType, ManagedObjectWrapper<ValueType>>>,
    typename IsCanMoved = CanMoved>
class ManagedObjectUnorderedMap
    : public ManagedObjectTableBase<KeyType, ValueType, HashMapTrait> {
 public:
  using ContainerTrait = HashMapTrait;
  using CanMovedTrait = IsCanMoved;
  using ThisType = ManagedObjectUnorderedMap<KeyType, ValueType, Hash, Equal,
                                             Allocator, CanMovedTrait>;
  using BaseType = ManagedObjectTableBase<KeyType, ValueType, ContainerTrait>;
  using HashKeyType = KeyType;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = Utils::HashMap<
      KeyType, WrapperType, typename WrapperType::template HashWrapperKey<Hash>,
      typename WrapperType::template EqualWrapperKey<Equal>, Allocator>;
  using ContainerReturnType = std::pair<const KeyType, WrapperType>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value,
      std::uint64_t bucket_count);

 public:
  ManagedObjectUnorderedMap()
      : ManagedObjectTableBase<KeyType, ValueType, HashMapTrait>(this),
        data_(1024) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ~ManagedObjectUnorderedMap() {
    if (size_.load(std::memory_order_acquire) != 0) {
      MM_LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedMap(std::uint64_t size)
      : ManagedObjectTableBase<KeyType, ValueType, HashMapTrait>(this),
        data_(size) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ManagedObjectUnorderedMap(const ManagedObjectUnorderedMap& other) = delete;
  ManagedObjectUnorderedMap(ManagedObjectUnorderedMap&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CanMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");
    LockAll guard(other);

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);
    size_.store(other.size_.load(std::memory_order_acquire),
                std::memory_order_release);
    other.size_.store(0, std::memory_order_release);
  }
  ManagedObjectUnorderedMap& operator=(const ManagedObjectUnorderedMap& other) =
      delete;
  ManagedObjectUnorderedMap& operator=(
      ManagedObjectUnorderedMap&& other) noexcept {
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
                std::memory_order_release);
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

  bool Have(const KeyType& key) const override {
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

  uint32_t GetSize(const KeyType& key) const override {
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

  bool IsRelationshipContainer() const override { return true; }

  ExecuteResult AddObject(const KeyType& key, ValueType&& managed_object,
                          HandlerType& handler) {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OBJECT_IS_INVALID;
    }

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::pair<ContainerReturnType&, bool> insert_result =
        data_.Emplace(key, std::move(managed_object));

    if (!insert_result.second) {
      return ExecuteResult ::OPERATION_NOT_SUPPORTED;
    }

    handler = HandlerType{
        BaseType::GetThisPtrPtr(), &(insert_result.first.first),
        const_cast<ValueType*>(insert_result.first.second.GetObjectPtr()),
        insert_result.first.second.GetUseCountPtr()};

    size_.fetch_add(1, std::memory_order_acq_rel);
    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetObject(const KeyType& key, HandlerType& handler) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OBJECT_IS_INVALID;
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

    const ContainerReturnType* iter = data_.Find(key);

    if (iter == nullptr) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    handler = HandlerType{BaseType::GetThisPtrPtr(), &(iter->first),
                          const_cast<ValueType*>(iter->second.GetObjectPtr()),
                          iter->second.GetUseCountPtr()};

    return ExecuteResult::SUCCESS;
  }

  uint32_t GetUseCount(const KeyType& key) const {
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

    const ContainerReturnType* iter = data_.Find(key);

    if (iter == nullptr) {
      return 0;
    }

    return iter->second.GetUseCount();
  }

 protected:
  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                HashMapTrait) override {
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
      return ExecuteResult::CUSTOM_ERROR;
    }

    auto iter = data_.Find(removed_object_key);

    if (iter == nullptr) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    if (iter->second.GetUseCount() == 0) {
      data_.Erase(iter);

      size_.fetch_sub(1, std::memory_order_acq_rel);
    }

    return ExecuteResult ::SUCCESS;
  }

 private:
  void ResizeWhenNeeded() {
    if (data_.BucketCount() - size_.load(std::memory_order_acquire) < 128) {
      std::uint64_t new_size = data_.BucketCount() * 2;
      if (new_size < 2048) {
        new_size = 2048;
      }
      Reserve(new_size);
    }
  }

  std::shared_mutex& ChooseMutexIn(const KeyType& key) const {
    // return data_mutex0_;
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

template <
    typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>,
    typename Equal = std::equal_to<KeyType>,
    typename Allocator =
        std::allocator<std::pair<KeyType, ManagedObjectWrapper<ValueType>>>,
    typename IsCanMoved = CanMoved>
class ManagedObjectUnorderedMultiMap
    : public ManagedObjectTableBase<KeyType, ValueType, HashMapTrait> {
 public:
  using ContainerTrait = HashMapTrait;
  using CanMovedTrait = IsCanMoved;
  using ThisType =
      ManagedObjectUnorderedMultiMap<KeyType, ValueType, Hash, Equal, Allocator,
                                     CanMovedTrait>;
  using BaseType = ManagedObjectTableBase<KeyType, ValueType, ContainerTrait>;
  using HashKeyType = KeyType;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = Utils::MultiHashMap<
      KeyType, WrapperType, typename WrapperType::template HashWrapperKey<Hash>,
      typename WrapperType::template EqualWrapperKey<Equal>, Allocator>;
  using ContainerReturnType = std::pair<const KeyType, WrapperType>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value,
      std::uint64_t bucket_count);

 public:
  ManagedObjectUnorderedMultiMap()
      : ManagedObjectTableBase<KeyType, ValueType, HashMapTrait>(this),
        data_(1024) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ~ManagedObjectUnorderedMultiMap() {
    if (size_.load(std::memory_order_acquire) != 0) {
      MM_LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedMultiMap(std::uint64_t size)
      : ManagedObjectTableBase<KeyType, ValueType, HashMapTrait>(this),
        data_(size) {
    // Prevent automatic rehash.
    data_.SetLoadFactor(50.0f);
  }
  ManagedObjectUnorderedMultiMap(const ManagedObjectUnorderedMultiMap& other) =
      delete;
  ManagedObjectUnorderedMultiMap(
      ManagedObjectUnorderedMultiMap&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CanMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");
    LockAll guard{other};

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);
    size_.store(other.size_.load(std::memory_order_acquire),
                std::memory_order_release);
    other.size_.store(0, std::memory_order_release);
  }
  ManagedObjectUnorderedMultiMap& operator=(
      const ManagedObjectUnorderedMultiMap& other) = delete;
  ManagedObjectUnorderedMultiMap& operator=(
      ManagedObjectUnorderedMultiMap&& other) noexcept {
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
                std::memory_order_release);
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

  bool Have(const KeyType& key) const override {
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

  uint32_t GetSize(const KeyType& key) const override {
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

  bool IsRelationshipContainer() const override { return true; }

  ExecuteResult AddObject(const KeyType& key, ValueType&& managed_object,
                          HandlerType& handler) {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OBJECT_IS_INVALID;
    }

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::pair<ContainerReturnType&, bool> insert_result =
        data_.Emplace(std::make_pair(key, std::move(managed_object)));

    if (!insert_result.second) {
      return ExecuteResult ::OPERATION_NOT_SUPPORTED;
    }

    handler = HandlerType{
        BaseType::GetThisPtrPtr(), &(insert_result.first.first),
        const_cast<ValueType*>(insert_result.first.second.GetObjectPtr()),
        insert_result.first.second.GetUseCountPtr()};
    size_.fetch_add(1, std::memory_order_acq_rel);
    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetObject(const KeyType& key, HandlerType& handler) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
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

    const ContainerReturnType* iter = data_.Find(key);

    if (iter == nullptr) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    handler = HandlerType{BaseType::GetThisPtrPtr(), &(iter->first),
                          const_cast<ValueType*>(iter->second.GetObjectPtr()),
                          iter->second.GetUseCountPtr()};

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObject(const KeyType& key,
                          const std::atomic_uint32_t* use_count_ptr,
                          HandlerType& handler) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OBJECT_IS_INVALID;
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
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      if (equal_range[i]->second.GetUseCountPtr() == use_count_ptr) {
        handler = HandlerType{
            BaseType::GetThisPtrPtr(), &(equal_range[i]->first),
            const_cast<ValueType*>(equal_range[i]->second.GetObjectPtr()),
            equal_range[i]->second.GetUseCountPtr()};

        return ExecuteResult::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObject(const KeyType& key, const ValueType& object,
                          HandlerType& handler) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OBJECT_IS_INVALID;
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
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      if (equal_range[i]->second.GetObject() == object) {
        handler = HandlerType{
            BaseType::GetThisPtrPtr(), &(equal_range[i]->first),
            const_cast<ValueType*>(equal_range[i]->second.GetObjectPtr()),
            equal_range[i]->second.GetUseCountPtr()};

        return ExecuteResult::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObject(const KeyType& key,
                          std::vector<HandlerType>& handlers) const {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OBJECT_IS_INVALID;
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
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      handlers.emplace_back(
          BaseType::GetThisPtrPtr(), &(equal_range[i]->first),
          const_cast<ValueType*>(equal_range[i]->second.GetObjectPtr()),
          equal_range[i]->second.GetUseCountPtr());
    }

    return ExecuteResult::SUCCESS;
  }

  std::uint32_t GetUseCount(const KeyType& key) const {
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

    const ContainerReturnType* iter = data_.Find(key);
    if (iter == nullptr) {
      return 0;
    }

    return iter->second.GetUseCount();
  }

  std::uint32_t GetUseCount(const KeyType& key, const ValueType& object) const {
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
    std::vector<const ContainerReturnType*> equal_range = data_.EqualRange(key);
    if (equal_range.empty()) {
      return 0;
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      if (equal_range[i]->second.GetObject() == object) {
        return equal_range[i]->second.GetUseCount();
      }
    }

    return 0;
  }

  ExecuteResult GetUseCount(const KeyType& key,
                            std::vector<std::uint32_t>& use_counts) const {
    if (!ThisType::TestMovedWhenGetUseCount()) {
      return ExecuteResult::OBJECT_IS_INVALID;
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
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      use_counts.emplace_back(equal_range[i]->second.GetUseCount());
    }

    return ExecuteResult::SUCCESS;
  }

 protected:
  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                HashMapTrait) override {
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
      return ExecuteResult::CUSTOM_ERROR;
    }

    std::vector<ContainerReturnType*> equal_range =
        data_.EqualRange(removed_object_key);
    if (equal_range.empty()) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (std::uint32_t i = 0; i != equal_range.size(); ++i) {
      if (equal_range[i]->second.GetUseCountPtr() == use_count_ptr) {
        if (equal_range[i]->second.GetUseCount() == 0) {
          data_.Erase(equal_range[i]);
          size_.fetch_sub(1, std::memory_order_acq_rel);
        }

        return ExecuteResult ::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

 private:
  void ResizeWhenNeeded() {
    if (data_.BucketCount() - size_.load(std::memory_order_acquire) < 128) {
      std::uint64_t new_size = data_.BucketCount() * 2;
      if (new_size < 2048) {
        new_size = 2048;
      }
      Reserve(new_size);
    }
  }

  std::shared_mutex& ChooseMutexIn(const KeyType& key) const {
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