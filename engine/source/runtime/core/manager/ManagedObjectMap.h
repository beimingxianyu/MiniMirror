#pragma once

#include <map>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"

namespace MM {
namespace Manager {

template <typename KeyType, typename ValueType,
          typename Allocator = std::allocator<
              std::pair<const KeyType, ManagedObjectWrapper<ValueType>>>>
class ManagedObjectMap
    : public ManagedObjectTableBase<KeyType, ValueType, NodeKeyTrait> {
 public:
  using RelationshipContainerTrait = NodeKeyTrait;
  using ThisType = ManagedObjectMap<KeyType, ValueType, Allocator>;
  using BaseType =
      ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>;
  using HandlerType = typename BaseType::HandlerType;
  using WrapperType = typename BaseType::WrapperType;
  using ContainerType = std::map<KeyType, ManagedObjectWrapper<ValueType>,
                                 std::less<KeyType>, Allocator>;

 public:
  ManagedObjectMap();
  ~ManagedObjectMap() override;
  ManagedObjectMap(const ManagedObjectMap& other) = delete;
  ManagedObjectMap(ManagedObjectMap&& other) noexcept;
  ManagedObjectMap& operator=(const ManagedObjectMap& other) = delete;
  ManagedObjectMap& operator=(ManagedObjectMap&& other) noexcept;

 public:
  size_t GetSize() const override;

  bool Have(const KeyType& key) const override;

  uint32_t Count(const KeyType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  ExecuteResult AddObject(const KeyType& key, ValueType&& managed_object,
                          HandlerType& handler);

  ExecuteResult GetObject(const KeyType& key, HandlerType& handle) const;

  uint32_t GetUseCount(const KeyType& key) const;

 protected:
  ExecuteResult AddObjectImp(const KeyType& key, ValueType&& managed_object,
                             HandlerType& handler) override;

  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                RelationshipContainerTrait trait) override;

  ExecuteResult GetObjectImp(const KeyType& key,
                             HandlerType& handle) const override;

  uint32_t GetUseCountImp(const KeyType& key) const override;

 private:
  ContainerType data_;
  mutable std::shared_mutex data_mutex_;
};

template <typename KeyType, typename ValueType,
          typename Allocator = std::allocator<
              std::pair<const KeyType, ManagedObjectWrapper<ValueType>>>>
class ManagedObjectMultiMap
    : public ManagedObjectTableBase<KeyType, ValueType, NodeKeyTrait> {
 public:
  using RelationshipContainerTrait = NodeKeyTrait;
  using ThisType = ManagedObjectMultiMap<KeyType, ValueType, Allocator>;
  using BaseType =
      ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>;
  using HandlerType = typename BaseType::HandlerType;
  using WrapperType = typename BaseType::WrapperType;
  using ContainerType = std::multimap<KeyType, ManagedObjectWrapper<ValueType>,
                                      std::less<KeyType>, Allocator>;

 public:
  ManagedObjectMultiMap();
  ~ManagedObjectMultiMap() override;
  ManagedObjectMultiMap(const ManagedObjectMultiMap& other) = delete;
  ManagedObjectMultiMap(ManagedObjectMultiMap&& other) noexcept;
  ManagedObjectMultiMap& operator=(const ManagedObjectMultiMap& other) = delete;
  ManagedObjectMultiMap& operator=(ManagedObjectMultiMap&& other) noexcept;

 public:
  size_t GetSize() const override;

  bool Have(const KeyType& key) const override;

  uint32_t Count(const KeyType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  ExecuteResult AddObject(const KeyType& key, ValueType&& managed_object,
                          HandlerType& handler);

  ExecuteResult GetObject(const KeyType& key, HandlerType& handler) const;

  ExecuteResult GetObject(const KeyType& key,
                          const std::atomic_uint32_t* use_count_ptr,
                          HandlerType& handler) const;

  ExecuteResult GetObject(const KeyType& key, const ValueType& object,
                          HandlerType& handler) const;

  void GetObject(const KeyType& key, std::vector<HandlerType>& handlers) const;

  std::uint32_t GetUseCount(const KeyType& key) const;

  std::uint32_t GetUseCount(const KeyType& key,
                            const std::atomic_uint32_t* use_count_ptr) const;

  std::uint32_t GetUseCount(const KeyType& key, const ValueType& object) const;

  void GetUseCount(const KeyType& key,
                   std::vector<std::uint32_t>& use_counts) const;

 protected:
  ExecuteResult AddObjectImp(const KeyType& key, ValueType&& managed_object,
                             HandlerType& handler) override;

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

  ExecuteResult GetUseCountImp(
      const KeyType& key,
      std::vector<std::uint32_t>& use_counts) const override;

 private:
  ContainerType data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMap<KeyType, ValueType, Allocator>::ManagedObjectMap()
    : ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>(
          this),
      data_(),
      data_mutex_() {}

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMap<KeyType, ValueType, Allocator>::~ManagedObjectMap() {
  if (GetSize() != 0) {
    LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMap<KeyType, ValueType, Allocator>::ManagedObjectMap(
    ManagedObjectMap&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMap<KeyType, ValueType, Allocator>&
ManagedObjectMap<KeyType, ValueType, Allocator>::operator=(
    ManagedObjectMap&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  std::lock(data_mutex_, other.data_mutex_);
  if (!data_.empty()) {
    LOG_ERROR(
        "If there is data in the original container but it is reassigned, an "
        "access error will occur.");
  }

  std::unique_lock<std::shared_mutex> main_guard{data_mutex_, std::adopt_lock},
      other_guard{other.data_mutex_, std::adopt_lock};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);

  return *this;
}

template <typename KeyType, typename ValueType, typename Allocator>
size_t ManagedObjectMap<KeyType, ValueType, Allocator>::GetSize() const {
  return data_.size();
}

template <typename KeyType, typename ValueType, typename Allocator>
bool ManagedObjectMap<KeyType, ValueType, Allocator>::Have(
    const KeyType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  typename ContainerType::const_iterator iter = data_.find(key);
  return iter != data_.end();
}

template <typename KeyType, typename ValueType, typename Allocator>
uint32_t ManagedObjectMap<KeyType, ValueType, Allocator>::Count(
    const KeyType& key) const {
  if (Have(key)) {
    return 1;
  }

  return 0;
}

template <typename KeyType, typename ValueType, typename Allocator>
bool ManagedObjectMap<KeyType, ValueType, Allocator>::IsMultiContainer() const {
  return false;
}

template <typename KeyType, typename ValueType, typename Allocator>
bool ManagedObjectMap<KeyType, ValueType, Allocator>::IsRelationshipContainer()
    const {
  return true;
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMap<KeyType, ValueType, Allocator>::AddObject(
    const KeyType& key, ValueType&& managed_object,
    ManagedObjectMap::HandlerType& handler) {
  return AddObjectImp(key, std::move(managed_object), handler);
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMap<KeyType, ValueType, Allocator>::GetObject(
    const KeyType& key, ManagedObjectMap::HandlerType& handle) const {
  return GetObjectImp(key, handle);
}

template <typename KeyType, typename ValueType, typename Allocator>
uint32_t ManagedObjectMap<KeyType, ValueType, Allocator>::GetUseCount(
    const KeyType& key) const {
  return GetUseCountImp(key);
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMap<KeyType, ValueType, Allocator>::AddObjectImp(
    const KeyType& key, ValueType&& managed_object, HandlerType& handler) {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::iterator, bool> insert_result =
      data_.emplace(std::make_pair(key, std::move(managed_object)));
  HandlerType new_handler = HandlerType{
      BaseType::GetThisPtrPtr(), &(insert_result.first->first),
      const_cast<ValueType*>(insert_result.first->second.GetObjectPtr()),
      insert_result.first->second.GetUseCountPtr()};
  guard.unlock();

  handler = std::move(new_handler);

  if (insert_result.second) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMap<KeyType, ValueType, Allocator>::RemoveObjectImp(
    const KeyType& removed_object_key, RelationshipContainerTrait trait) {
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

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMap<KeyType, ValueType, Allocator>::GetObjectImp(
    const KeyType& key, HandlerType& handle) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::const_iterator iter = data_.find(key);

  if (iter == data_.end()) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  HandlerType new_handler{BaseType::GetThisPtrPtr(), &(iter->first),
                          const_cast<ValueType*>(iter->second.GetObjectPtr()),
                          iter->second.GetUseCountPtr()};
  guard.unlock();
  handle = std::move(new_handler);

  return ExecuteResult::SUCCESS;
}

template <typename KeyType, typename ValueType, typename Allocator>
uint32_t ManagedObjectMap<KeyType, ValueType, Allocator>::GetUseCountImp(
    const KeyType& key) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::const_iterator iter = data_.find(key);

  if (iter == data_.end()) {
    return 0;
  }

  return iter->second.GetUseCount();
}

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::ManagedObjectMultiMap()
    : ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>(
          this),
      data_(),
      data_mutex_() {}

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::~ManagedObjectMultiMap() {
  if (GetSize() != 0) {
    LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::ManagedObjectMultiMap(
    ManagedObjectMultiMap&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard(other.data_mutex_);

  BaseType::operator=(std::move(other));
  data_ = std::move(data_);
}

template <typename KeyType, typename ValueType, typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Allocator>&
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::operator=(
    ManagedObjectMultiMap&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  std::lock(data_mutex_, other.data_mutex_);

  if (!data_.empty()) {
    LOG_ERROR(
        "If there is data in the original container but it is reassigned, an "
        "access error will occur.");
  }
  std::unique_lock<std::shared_mutex> main_guard(data_mutex_, std::adopt_lock),
      other_guard(other.data_mutex_, std::adopt_lock);

  BaseType ::operator=(std::move(other));
  data_ = std::move(other.data_);

  return *this;
}

template <typename KeyType, typename ValueType, typename Allocator>
size_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetSize() const {
  return data_.size();
}

template <typename KeyType, typename ValueType, typename Allocator>
bool ManagedObjectMultiMap<KeyType, ValueType, Allocator>::Have(
    const KeyType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  return data_.count(key) != 0;
}

template <typename KeyType, typename ValueType, typename Allocator>
uint32_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::Count(
    const KeyType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  return data_.count(key);
}

template <typename KeyType, typename ValueType, typename Allocator>
bool ManagedObjectMultiMap<KeyType, ValueType, Allocator>::IsMultiContainer()
    const {
  return true;
}

template <typename KeyType, typename ValueType, typename Allocator>
bool ManagedObjectMultiMap<KeyType, ValueType,
                           Allocator>::IsRelationshipContainer() const {
  return true;
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMultiMap<KeyType, ValueType, Allocator>::AddObject(
    const KeyType& key, ValueType&& managed_object,
    ManagedObjectMultiMap::HandlerType& handler) {
  return AddObjectImp(key, std::move(managed_object), handler);
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObject(
    const KeyType& key, ManagedObjectMultiMap::HandlerType& handler) const {
  return GetObjectImp(key, handler);
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObject(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
    ManagedObjectMultiMap::HandlerType& handler) const {
  return GetObjectImp(key, use_count_ptr, handler);
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObject(
    const KeyType& key, const ValueType& object,
    ManagedObjectMultiMap::HandlerType& handler) const {
  return GetObjectImp(key, object, handler);
}

template <typename KeyType, typename ValueType, typename Allocator>
void ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObject(
    const KeyType& key, std::vector<HandlerType>& handlers) const {
  GetObjectImp(key, handlers);
}

template <typename KeyType, typename ValueType, typename Allocator>
std::uint32_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCount(
    const KeyType& key) const {
  return GetUseCountImp(key);
}

template <typename KeyType, typename ValueType, typename Allocator>
std::uint32_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCount(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr) const {
  return GetUseCountImp(key, use_count_ptr);
}

template <typename KeyType, typename ValueType, typename Allocator>
std::uint32_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCount(
    const KeyType& key, const ValueType& object) const {
  return GetUseCountImp(key, object);
}

template <typename KeyType, typename ValueType, typename Allocator>
void ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCount(
    const KeyType& key, std::vector<std::uint32_t>& use_counts) const {
  GetUseCountImp(key, use_counts);
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::AddObjectImp(
    const KeyType& key, ValueType&& managed_object, HandlerType& handler) {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  typename ContainerType::iterator insert_result =
      data_.emplace(std::make_pair(key, std::move(managed_object)));
  HandlerType new_handler{
      BaseType::GetThisPtrPtr(), &(insert_result->first),
      const_cast<ValueType*>(insert_result->second.GetObjectPtr()),
      insert_result->second.GetUseCountPtr()};

  guard.unlock();
  handler = std::move(new_handler);

  return ExecuteResult::SUCCESS;
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::RemoveObjectImp(
    const KeyType& removed_object_key,
    const std::atomic_uint32_t* use_count_ptr, NodeKeyTrait trait) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  if (ThisType::this_ptr_ptr_ == nullptr) {
    return ExecuteResult::CUSTOM_ERROR;
  }

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
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

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObjectImp(
    const KeyType& key, HandlerType& handler) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  HandlerType new_handler{
      BaseType::GetThisPtrPtr(), &(equal_range.first->first),
      const_cast<ValueType*>(equal_range.first->second.GetObjectPtr()),
      equal_range.first->second.GetUseCountPtr()};
  guard.unlock();

  handler = std::move(new_handler);

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObjectImp(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
    HandlerType& handler) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
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
          BaseType::GetThisPtrPtr(), &(iter->first),
          const_cast<ValueType*>(iter->second.GetObjectPtr()),
          iter->second.GetUseCountPtr()};

      guard.unlock();
      handler = std::move(new_handler);

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObjectImp(
    const KeyType& key, const ValueType& object, HandlerType& handler) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
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
          BaseType::GetThisPtrPtr(), &(iter->first),
          const_cast<ValueType*>(iter->second.GetObjectPtr()),
          iter->second.GetUseCountPtr()};

      guard.unlock();
      handler = std::move(new_handler);

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetObjectImp(
    const KeyType& key, std::vector<HandlerType>& handles) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  for (typename ContainerType::const_iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    handles.emplace_back(BaseType::GetThisPtrPtr(), &(iter->first),
                         const_cast<ValueType*>(iter->second.GetObjectPtr()),
                         iter->second.GetUseCountPtr());
  }

  return ExecuteResult::SUCCESS;
}

template <typename KeyType, typename ValueType, typename Allocator>
uint32_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCountImp(
    const KeyType& key) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return 0;
  }

  return equal_range.first->second.GetUseCount();
}

template <typename KeyType, typename ValueType, typename Allocator>
uint32_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCountImp(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
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

template <typename KeyType, typename ValueType, typename Allocator>
uint32_t ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCountImp(
    const KeyType& key, const ValueType& object) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
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

template <typename KeyType, typename ValueType, typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Allocator>::GetUseCountImp(
    const KeyType& key, std::vector<std::uint32_t>& use_counts) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
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
}  // namespace Manager
}  // namespace MM
