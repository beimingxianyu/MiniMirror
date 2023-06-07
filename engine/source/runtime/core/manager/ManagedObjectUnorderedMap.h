#pragma once

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/utils.h"
#include "utils/ConcurrentHashTable.h"

namespace MM {
namespace Manager {
template <
    typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>,
    typename Equal = std::equal_to<KeyType>,
    typename Allocator =
        std::allocator<std::pair<KeyType, ManagedObjectWrapper<ValueType>>>,
    typename IsCanMoved = Utils::TrueType>
class ManagedObjectUnorderedMap
    : public ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait> {
 public:
  using RelationshipContainerTrait = HashKeyTrait;
  using MovedTrait = IsCanMoved;
  using ThisType =
      ManagedObjectUnorderedMap<KeyType, ValueType, Hash, Equal, Allocator>;
  using BaseType =
      ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType =
      Utils::ConcurrentMultiMap<KeyType, WrapperType, Hash, Equal, Allocator>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value);

 public:
  ManagedObjectUnorderedMap()
      : ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait>(this),
        data_() {}
  ~ManagedObjectUnorderedMap() {
    if (!data_.Eempty()) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedMap(std::uint32_t size)
      : ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait>(this),
        data_(size) {}
  ManagedObjectUnorderedMap(const ManagedObjectUnorderedMap& other) = delete;
  ManagedObjectUnorderedMap(ManagedObjectUnorderedMap&& other) noexcept {
    static_assert(
        std::is_same_v<MovedTrait, Utils::TrueType>,
        "Only by marking 'IsCanMoved' as' TrueType 'can the object be moved.");
    Utils::SpinUniqueLock guard{move_mutex_};

    BaseType::operator=(std::move(other));
    data_ = std::move(data_);
  }
  ManagedObjectUnorderedMap& operator=(const ManagedObjectUnorderedMap& other) =
      delete;
  ManagedObjectUnorderedMap& operator=(
      ManagedObjectUnorderedMap&& other) noexcept {
    static_assert(
        std::is_same_v<MovedTrait, Utils::TrueType>,
        "Only by marking 'IsCanMoved' as' TrueType 'can the object be moved.");

    if (&other == this) {
      return *this;
    }
    Utils::SpinUniqueLock guard{move_mutex_};

    if (!data_.Empty()) {
      LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);

    return *this;
  }

 public:
  size_t GetSize() const override { return data_.Size(); }

  bool Have(const KeyType& key) const override {
    if constexpr (IsCanMovedValue()) {
      Utils::SpinSharedLock guard{move_mutex_};

      return data_.Contains(key);
    }

    return data_.Contains(key);
  }

  uint32_t Count(const KeyType& key) const override {
    if constexpr (IsCanMovedValue()) {
      Utils::SpinSharedLock guard{move_mutex_};

      return data_.Contains(key);
    }

    return data_.count(key);
  }

  bool IsMultiContainer() const override { return false; }

  bool IsRelationshipContainer() const override { return true; }

  ExecuteResult AddObject(const KeyType& key, ValueType&& managed_object,
                          HandlerType& handler) {
    return AddObjectImp(key, std::move(managed_object), handler);
  }

  ExecuteResult GetObject(const KeyType& key, HandlerType& handle) const {
    return GetObjectImp(key, handle);
  }

  uint32_t GetUseCount(const KeyType& key) const { return GetUseCountImp(key); }

 protected:
  ExecuteResult AddObjectImp(const KeyType& key, ValueType&& managed_object,
                             HandlerType& handler) override {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    // The repeated insertion operation is not supported.
    if (data_.count(key)) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::pair<typename ContainerType::iterator, bool> insert_result =
        data_.emplace(key, std::move(managed_object));
    HandlerType new_handler{
        BaseType::GetThisPtrPtr(), insert_result.first->first,
        const_cast<ValueType*>(insert_result.first->second.GetObjectPtr()),
        insert_result.first->second.GetUseCountPtr()};

    guard.unlock();

    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                RelationshipContainerTrait trait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(removed_object_key)};

    if (ThisType::this_ptr_ptr_ == nullptr) {
      return ExecuteResult::CUSTOM_ERROR;
    }

    auto iter = data_.find(removed_object_key);

    if (iter == data_.end()) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    if (iter->second.GetUseCount() == 0) {
      data_.erase(iter);
    }

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetObjectImp(const KeyType& key,
                             HandlerType& handle) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    typename ContainerType::const_iterator iter = data_.find(key);

    if (iter == data_.end()) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    HandlerType new_handler{BaseType::GetThisPtrPtr(), iter->first,
                            const_cast<ValueType*>(iter->second.GetObjectPtr()),
                            iter->second.GetUseCountPtr()};
    guard.unlock();
    handle = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  uint32_t GetUseCountImp(const KeyType& key) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    typename ContainerType::const_iterator iter = data_.find(key);

    if (iter == data_.end()) {
      return 0;
    }

    return iter->second.GetUseCount();
  }

 private:
  constexpr bool IsCanMovedValue() const {
    return std::is_same_v<MovedTrait, Utils::TrueType>;
  }

 private:
  ContainerType data_{};

  mutable Utils::SpinSharedMutex move_mutex_;
};

template <
    typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>,
    typename Equal = std::equal_to<KeyType>,
    typename Allocator =
        std::allocator<std::pair<KeyType, ManagedObjectWrapper<ValueType>>>,
    typename IsCanMoved = Utils::TrueType>
class ManagedObjectUnorderedMultiMap
    : public ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait> {
 public:
  using RelationshipContainerTrait = HashKeyTrait;
  using MovedTrait = IsCanMoved;
  using ThisType = ManagedObjectUnorderedMultiMap<KeyType, ValueType, Hash,
                                                  Equal, Allocator>;
  using BaseType =
      ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType =
      Utils::ConcurrentMultiMap<KeyType, WrapperType, Hash, Equal, Allocator>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value);

 public:
  ManagedObjectUnorderedMultiMap()
      : ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait>(this),
        data_(8) {}
  ~ManagedObjectUnorderedMultiMap() {
    if (!data_.empty()) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedMultiMap(std::uint32_t size)
      : ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait>(this),
        data_(size) {}
  ManagedObjectUnorderedMultiMap(const ManagedObjectUnorderedMultiMap& other) =
      delete;
  ManagedObjectUnorderedMultiMap(
      ManagedObjectUnorderedMultiMap&& other) noexcept {
    static_assert(
        std::is_same_v<MovedTrait, Utils::TrueType>,
        "Only by marking 'IsCanMoved' as' TrueType 'can the object be moved.");
    Utils::SpinUniqueLock guard{move_mutex_};

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);
  }
  ManagedObjectUnorderedMultiMap& operator=(
      const ManagedObjectUnorderedMultiMap& other) = delete;
  ManagedObjectUnorderedMultiMap& operator=(
      ManagedObjectUnorderedMultiMap&& other) noexcept {
    static_assert(
        std::is_same_v<MovedTrait, Utils::TrueType>,
        "Only by marking 'IsCanMoved' as' TrueType 'can the object be moved.");
    if (&other == this) {
      return *this;
    }

    Utils::SpinUniqueLock guard{move_mutex_};

    if (!data_.empty()) {
      LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);

    return *this;
  }

 public:
  size_t GetSize() const override { return data_.size(); }

  bool Have(const KeyType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    return data_.count(key) != 0;
  }

  uint32_t Count(const KeyType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    return data_.count(key);
  }

  bool IsMultiContainer() const override { return true; }

  bool IsRelationshipContainer() const override { return true; }

  ExecuteResult AddObject(const KeyType& key, ValueType&& managed_object,
                          HandlerType& handler) {
    return AddObjectImp(key, std::move(managed_object), handler);
  }

  ExecuteResult RemoveObject(const KeyType& removed_object_key,
                             const std::atomic_uint32_t* use_count_ptr,
                             RelationshipContainerTrait trait) {
    return RemoveObjectImp(removed_object_key, use_count_ptr, trait);
  }

  ExecuteResult GetObject(const KeyType& key, HandlerType& handler) const {
    return GetObjectImp(key, handler);
  }

  ExecuteResult GetObject(const KeyType& key,
                          const std::atomic_uint32_t* use_count_ptr,
                          HandlerType& handler) const {
    return GetObjectImp(key, use_count_ptr, handler);
  }

  ExecuteResult GetObject(const KeyType& key, const ValueType& object,
                          HandlerType& handler) const {
    return GetObjectImp(key, object, handler);
  }

  ExecuteResult GetObject(const KeyType& key,
                          std::vector<HandlerType>& handlers) const {
    return GetObjectImp(key, handlers);
  }

  std::uint32_t GetUseCount(const KeyType& key) const {
    return GetUseCountImp(key);
  }

  std::uint32_t GetUseCount(const KeyType& key,
                            const std::atomic_uint32_t* use_count) const {
    return GetUseCountImp(key, use_count);
  }

  std::uint32_t GetUseCount(const KeyType& key, const ValueType& object) const {
    return GetUseCountImp(key, object);
  }

  ExecuteResult GetUseCount(const KeyType& key,
                            std::vector<std::uint32_t>& use_counts) const {
    return GetUseCountImp(key, use_counts);
  }

 protected:
  ExecuteResult AddObjectImp(const KeyType& key, ValueType&& managed_object,
                             HandlerType& handler) override {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    typename ContainerType::iterator insert_result =
        data_.emplace(std::make_pair(key, std::move(managed_object)));
    HandlerType new_handler{
        BaseType::GetThisPtrPtr(), insert_result->first,
        const_cast<ValueType*>(insert_result->second.GetObjectPtr()),
        insert_result->second.GetUseCountPtr()};

    guard.unlock();
    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                RelationshipContainerTrait trait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(removed_object_key)};
    if (ThisType::this_ptr_ptr_ == nullptr) {
      return ExecuteResult::CUSTOM_ERROR;
    }

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(removed_object_key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      if (iter->second.GetUseCountPtr() == use_count_ptr) {
        if (iter->second.GetUseCount() == 0) {
          data_.erase(iter);
        }

        return ExecuteResult::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObjectImp(const KeyType& key,
                             HandlerType& handler) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    HandlerType new_handler{
        BaseType::GetThisPtrPtr(), equal_range.first->first,
        const_cast<ValueType*>(equal_range.first->second.GetObjectPtr()),
        equal_range.first->second.GetUseCountPtr()};
    guard.unlock();

    handler = std::move(new_handler);

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObjectImp(const KeyType& key,
                             const std::atomic_uint32_t* use_count_ptr,
                             HandlerType& handler) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::const_iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      if (iter->second.GetUseCountPtr() == use_count_ptr) {
        HandlerType new_handler{
            BaseType::GetThisPtrPtr(), iter->first,
            const_cast<ValueType*>(iter->second.GetObjectPtr()),
            iter->second.GetUseCountPtr()};

        guard.unlock();
        handler = std::move(new_handler);

        return ExecuteResult::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObjectImp(const KeyType& key, const ValueType& object,
                             HandlerType& handler) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::const_iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      if (iter->second.GetObject() == object) {
        HandlerType new_handler{
            BaseType::GetThisPtrPtr(), iter->first,
            const_cast<ValueType*>(iter->second.GetObjectPtr()),
            iter->second.GetUseCountPtr()};

        guard.unlock();
        handler = std::move(new_handler);

        return ExecuteResult::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObjectImp(
      const KeyType& key, std::vector<HandlerType>& handlers) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::const_iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      handlers.emplace_back(BaseType::GetThisPtrPtr(), iter->first,
                            const_cast<ValueType*>(iter->second.GetObjectPtr()),
                            iter->second.GetUseCountPtr());
    }

    return ExecuteResult::SUCCESS;
  }

  std::uint32_t GetUseCountImp(const KeyType& key) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return 0;
    }

    return equal_range.first->second.GetUseCount();
  }

  std::uint32_t GetUseCountImp(
      const KeyType& key,
      const std::atomic_uint32_t* use_count_ptr) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return 0;
    }

    for (typename ContainerType::const_iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      if (iter->second.GetUseCountPtr() == use_count_ptr) {
        return iter->second.GetUseCount();
      }
    }

    return 0;
  }

  std::uint32_t GetUseCountImp(const KeyType& key,
                               const ValueType& object) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return 0;
    }

    for (typename ContainerType::const_iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      if (iter->second.GetObject() == object) {
        return iter->second.GetUseCount();
      }
    }

    return 0;
  }

  ExecuteResult GetUseCountImp(
      const KeyType& key,
      std::vector<std::uint32_t>& use_counts) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    std::pair<typename ContainerType::const_iterator,
              typename ContainerType::const_iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::const_iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      use_counts.emplace_back(iter->second.GetUseCount());
    }

    return ExecuteResult::SUCCESS;
  }

 private:
  void ResizeWhenNeeded() {
    if (data_.bucket_count() - data_.size() < 128) {
      std::uint32_t new_size = data_.bucket_count() * 2;
      if (new_size < 2048) {
        new_size = 2048;
      }
      LockAll guard(*this);
      data_.reserve(new_size);
    }
  }

  std::shared_mutex& ChooseMutexIn(const KeyType& key) const {
    return ChooseMutex(*this, Hash{}(key));
  }

 private:
  ContainerType data_;

  mutable Utils::SpinSharedMutex move_mutex_;
};
}  // namespace Manager
}  // namespace MM
