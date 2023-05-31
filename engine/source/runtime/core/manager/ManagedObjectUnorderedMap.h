#pragma once

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/utils.h"

namespace MM {
namespace Manager {
template <
    typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>,
    typename Equal = std::equal_to<KeyType>,
    typename Allocator =
        std::allocator<std::pair<KeyType, ManagedObjectWrapper<ValueType>>>>
class ManagedObjectUnorderedMap
    : ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait> {
 public:
  using RelationshipContainerTrait = NoKeyTrait;
  using ThisType =
      ManagedObjectUnorderedMap<KeyType, ValueType, Hash, Equal, Allocator>;
  using BaseType =
      ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = std::unordered_map<
      KeyType, ValueType, typename WrapperType::template HashWrapper<Hash>,
      typename WrapperType::template EqualWrapper<Equal>, Allocator>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value);

 public:
  ManagedObjectUnorderedMap() : data_(8) {}
  ~ManagedObjectUnorderedMap() {
    if (!data_.empty()) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  ManagedObjectUnorderedMap(std::uint32_t size) : data_(size) {}
  ManagedObjectUnorderedMap(const ManagedObjectUnorderedMap& other) = delete;
  ManagedObjectUnorderedMap(ManagedObjectUnorderedMap&& other) noexcept {
    LockAll guard(other);

    BaseType::operator=(other);
    data_ = std::move(data_);
  }
  ManagedObjectUnorderedMap& operator=(const ManagedObjectUnorderedMap& other) =
      delete;
  ManagedObjectUnorderedMap& operator=(
      ManagedObjectUnorderedMap&& other) noexcept {
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
    if (!data_.empty()) {
      LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }
    LockAll main_guard(*this), other_guard(other);

    BaseType::operator=(other);
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
    std::pair<typename ContainerType::iterator, bool> insert_result =
        data_.emplace(std::make_pair(key, std::move(managed_object)));
    HandlerType new_handler = HandlerType{
        BaseType::GetThisPtrPtr(), insert_result.first->first,
        const_cast<ValueType*>(insert_result.first->second.GetObjectPtr()),
        insert_result.first->second.GetUseCountPtr()};
    guard.unlock();

    handler = std::move(new_handler);

    if (insert_result.second) {
      return ExecuteResult::SUCCESS;
    }

    return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
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
  void ResizeWhenNeeded() {
    LockAll guard(*this);
    if (data_.size() >= data_.bucket_count() - 1) {
      assert(data_.bucket_count() < UINT_LEAST64_MAX / 2);
      data_.reserve(data_.bucket_count() * 2);
    }
  }

  std::shared_mutex& ChooseMutexIn(const KeyType& key) const {
    return ChooseMutex(*this, Hash{}(key));
  }

 private:
  ContainerType data_{};

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
        std::allocator<std::pair<KeyType, ManagedObjectWrapper<ValueType>>>>
class ManagedObjectUnorderedMultiMap
    : ManagedObjectTableBase<KeyType, ValueType, HashKeyTrait> {
 public:
  using RelationshipContainerTrait = NoKeyTrait;
  using ThisType = ManagedObjectUnorderedMultiMap<KeyType, ValueType, Hash,
                                                  Equal, Allocator>;
  using BaseType =
      ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = std::unordered_multimap<
      KeyType, ValueType, typename WrapperType::template HashWrapper<Hash>,
      typename WrapperType::template EqualWrapper<Equal>, Allocator>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value);

 public:
  ManagedObjectUnorderedMultiMap();
  ~ManagedObjectUnorderedMultiMap();
  ManagedObjectUnorderedMultiMap(std::uint32_t size);
  ManagedObjectUnorderedMultiMap(const ManagedObjectUnorderedMultiMap& other) =
      delete;
  ManagedObjectUnorderedMultiMap(
      ManagedObjectUnorderedMultiMap&& other) noexcept;
  ManagedObjectUnorderedMultiMap& operator=(
      const ManagedObjectUnorderedMultiMap& other) = delete;
  ManagedObjectUnorderedMultiMap& operator=(
      ManagedObjectUnorderedMultiMap&& other) noexcept;

 public:
 private:
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
                                NodeKeyTrait trait) override;

  ExecuteResult GetObjectImp(const KeyType& key,
                             HandlerType& handler) const override;

  ExecuteResult GetObjectImp(const KeyType& key,
                             const std::atomic_uint32_t* use_count_ptr,
                             HandlerType& handler) const override;

  ExecuteResult GetObjectImp(const KeyType& key, const ValueType& object,
                             HandlerType& handler) const override;

  ExecuteResult GetObjectImp(const KeyType& key,
                             std::vector<HandlerType>& handles) const override;

  uint32_t GetUseCountImp(const KeyType& key) const override;

  uint32_t GetUseCountImp(
      const KeyType& key,
      const std::atomic_uint32_t* use_count_ptr) const override;

  uint32_t GetUseCountImp(const KeyType& key,
                          const ValueType& object) const override;

  void GetUseCountImp(const KeyType& key,
                      std::vector<uint32_t>& use_counts) const override;

 private:
  void ResizeWhenNeeded() {
    LockAll guard(*this);
    if (data_.size() >= data_.bucket_count() - 1) {
      assert(data_.bucket_count() < UINT_LEAST64_MAX / 2);
      data_.reserve(data_.bucket_count() * 2);
    }
  }

  std::shared_mutex& ChooseMutexIn(const KeyType& key) const {
    return ChooseMutex(*this, Hash{}(key));
  }

 private:
  ContainerType data_;

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
