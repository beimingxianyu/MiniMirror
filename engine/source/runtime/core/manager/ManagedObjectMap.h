#pragma once

#include <map>

#include "runtime/core/manager/ManagedObjectTableBase.h"

namespace MM {
namespace Manager {

template <typename KeyType, typename ValueType,
          typename Allocator = std::allocator<
              ManagedObjectWrapper<std::pair<const KeyType, ValueType>>>>
class ManagedObjectMap : public ManagedObjectTableBase<KeyType, ValueType> {
 public:
  using ThisType = ManagedObjectMap<KeyType, ValueType>;
  using BashType = ManagedObjectTableBase<KeyType, ValueType>;
  using HandlerType = ManagedObjectHandler<KeyType, ValueType>;
  using ContainerType =
      std::map<KeyType, ValueType, std::less<KeyType>, Allocator>;

 public:
  ManagedObjectMap() : data_(), data_mutex_(){};
  ~ManagedObjectMap() override {
    if (GetSize() != 0) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  };
  ManagedObjectMap(const ManagedObjectMap& other) = delete;
  ManagedObjectMap(ManagedObjectMap&& other) noexcept {
    std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

    BashType::operator=(std::move(other));
    data_ = std::move(other.data_);
  };
  ManagedObjectMap& operator=(const ManagedObjectMap& other) = delete;
  ManagedObjectMap& operator=(ManagedObjectMap&& other) noexcept {
    if (&other == this) {
      return *this;
    }

    std::lock(data_mutex_, other.data_mutex_);
    if (!data_.empty()) {
      LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }

    std::unique_lock<std::shared_mutex> main_guard{data_mutex_,
                                                   std::adopt_lock},
        other_guard{other.data_mutex_, std::adopt_lock};

    BashType::operator=(std::move(other));
    data_ = std::move(other.data_);

    return *this;
  };

 public:
  size_t GetSize() const override { return data_.size(); }

  bool Have(const KeyType& key) const override {
    std::shared_lock<std::shared_mutex> guard{data_mutex_};
    typename ContainerType::iterator iter = data_.find(key);
    return iter != data_.end();
  }

  uint32_t Count(const KeyType& key) const override {
    if (Have(key)) {
      return 1;
    }

    return 0;
  }

  bool IsMultiContainer() const override { return false; }

  bool IsRelationshipContainer() const override { return true; }

  ExecuteResult AddObject(const KeyType& key, ValueType&& managed_object,
                          HandlerType& handler) {
    return AddObjectImp(key, std::move(managed_object), handler);
  }

  ExecuteResult RemoveObject(const KeyType& removed_object_key) {
    return RemoveObjectImp(removed_object_key);
  }

  ExecuteResult GetObject(const KeyType& key, HandlerType& handle) const {
    return GetObjectImp(key, handle);
  }

  uint32_t GetUseCount(const KeyType& key) const { return GetUseCountImp(key); }

 protected:
  ExecuteResult AddObjectImp(
      const KeyType& key, ValueType&& managed_object,
      ManagedObjectHandler<KeyType, ValueType>& handler) override {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::unique_lock<std::shared_mutex> guard{data_mutex_};
    std::pair<typename ContainerType::iterator, bool> insert_result =
        data_.emplace(std::make_pair(key, std::move(managed_object)));
    HandlerType new_handler = HandlerType{
        BashType::GetThisPtrPtr(), &(insert_result.first->first),
        const_cast<ValueType*>(&(insert_result.first->second.GetObjectPtr())),
        &(insert_result.first->second.GetUseCountPtr())};
    guard.unlock();

    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key) override {
    std::unique_lock<std::shared_mutex> guard{data_mutex_};

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

  ExecuteResult GetObjectImp(
      const KeyType& key,
      ManagedObjectHandler<KeyType, ValueType>& handle) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{data_mutex_};

    typename ContainerType::iterator iter = data_.find(key);

    if (iter == data_.end()) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    HandlerType new_handler{BashType::GetThisPtrPtr(), &(iter->first),
                            const_cast<ValueType*>(iter->second.GetObjectPtr()),
                            iter->second.GetUseCountPtr()};
    guard.unlock();
    handle = std::move(new_handler);
  }

  uint32_t GetUseCountImp(const KeyType& key) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{data_mutex_};

    typename ContainerType::iterator iter = data_.find(key);

    if (iter == data_.end()) {
      return 0;
    }

    return iter->second.GetUseCount();
  }

 private:
  ContainerType data_;
  mutable std::shared_mutex data_mutex_;
};

template <typename KeyType, typename ValueType,
          typename Allocator = std::allocator<
              ManagedObjectWrapper<std::pair<const KeyType, ValueType>>>>
class ManagedObjectMultiMap
    : public ManagedObjectTableBase<KeyType, ValueType> {
 public:
  using ThisType = ManagedObjectMap<KeyType, ValueType>;
  using BashType = ManagedObjectTableBase<KeyType, ValueType>;
  using HandlerType = ManagedObjectHandler<KeyType, ValueType>;
  using ContainerType =
      std::multimap<KeyType, ValueType, std::less<KeyType>, Allocator>;

 public:
  ManagedObjectMultiMap();
  ~ManagedObjectMultiMap() override;
  ManagedObjectMultiMap(const ManagedObjectMultiMap& other) = delete;
  ManagedObjectMultiMap(ManagedObjectMultiMap&& other) noexcept;
  ManagedObjectMultiMap& operator=(const ManagedObjectMultiMap& other) = delete;
  ManagedObjectMultiMap& operator=(ManagedObjectMultiMap&& other) noexcept;

 public:
  size_t GetSize() const override { return data_.size(); }

  bool Have(const KeyType& key) const override {
    std::shared_lock<std::shared_mutex> guard{data_mutex_};

    return data_.count(key) != 0;
  }

  uint32_t Count(const KeyType& key) const override {
    std::shared_lock<std::shared_mutex> guard{data_mutex_};

    return data_.count(key);
  }

  bool IsMultiContainer() const override { return true; }

  bool IsRelationshipContainer() const override { return true; }

 protected:
  ExecuteResult AddObjectImp(
      const KeyType& key, ValueType&& managed_object,
      ManagedObjectHandler<KeyType, ValueType>& handler) override {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::unique_lock<std::shared_mutex> guard{data_mutex_};
    std::pair<typename ContainerType::iterator, bool> insert_result =
        data_.emplace(std::make_pair(key, std::move(managed_object)));
    HandlerType new_handler{
        BashType::GetThisPtrPtr(), &(insert_result.first->first),
        const_cast<ValueType*>(insert_result.first->second.GetObjectPtr()),
        insert_result.first->second.GetUseCountPtr};

    guard.unlock();
    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                std::atomic_uint32_t* use_count_ptr) override {
    std::unique_lock<std::shared_mutex> guard{data_mutex_};
    if (ThisType::this_ptr_ptr_ == nullptr) {
      return ExecuteResult::CUSTOM_ERROR;
    }

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(removed_object_key);
    if (equal_range->first == equal_range->second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::iterator iter = equal_range->first;
         iter != equal_range->second; ++iter) {
      if (iter->second.GetUseCountPtr() == use_count_ptr) {
        if (iter->second.GetUseCount() == 0) {
          data_.erase(iter);
        }

        return ExecuteResult::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObjectImp(
      const KeyType& key,
      ManagedObjectHandler<KeyType, ValueType>& handler) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{data_mutex_};
    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(key);
    if (equal_range->first == equal_range->second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    HandlerType new_handler{
        BashType::GetThisPtrPtr(), &(equal_range.first->first),
        const_cast<ValueType*>(equal_range.first->second.GetObjectPtr()),
        equal_range.first->second.GetUseCountPtr()};
    guard.unlock();

    handler = std::move(new_handler);

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObjectImp(
      const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
      ManagedObjectHandler<KeyType, ValueType>& handler) const override {}

 private:
  ContainerType data_{};
  mutable std::shared_mutex data_mutex_{};
};
}  // namespace Manager
}  // namespace MM
