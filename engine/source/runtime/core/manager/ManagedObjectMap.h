#pragma once

#include <map>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"

namespace MM {
namespace Manager {

template <typename KeyType, typename ValueType, typename Less = std::less<>,
          typename Allocator = std::allocator<
              std::pair<const KeyType, ManagedObjectWrapper<ValueType>>>>
class ManagedObjectMap
    : public ManagedObjectTableBase<KeyType, ValueType, MapTrait> {
 public:
  using ContainerTrait = MapTrait;
  using ThisType = ManagedObjectMap<KeyType, ValueType, Less, Allocator>;
  using BaseType = ManagedObjectTableBase<KeyType, ValueType, ContainerTrait>;
  using HandlerType = typename BaseType::HandlerType;
  using WrapperType = typename BaseType::WrapperType;
  using ContainerType =
      std::map<KeyType, WrapperType,
               typename WrapperType::template LessWrapperKey<Less>, Allocator>;

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
  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                ContainerTrait trait) override;

 private:
  ContainerType data_;
  mutable std::shared_mutex data_mutex_;
};

template <typename KeyType, typename ValueType, typename Less = std::less<>,
          typename Allocator = std::allocator<
              std::pair<const KeyType, ManagedObjectWrapper<ValueType>>>>
class ManagedObjectMultiMap
    : public ManagedObjectTableBase<KeyType, ValueType, MapTrait> {
 public:
  using ContainerTrait = MapTrait;
  using ThisType = ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>;
  using BaseType = ManagedObjectTableBase<KeyType, ValueType, ContainerTrait>;
  using HandlerType = typename BaseType::HandlerType;
  using WrapperType = typename BaseType::WrapperType;
  using ContainerType =
      std::multimap<KeyType, ManagedObjectWrapper<ValueType>,
                    typename WrapperType::template LessWrapperKey<Less>,
                    Allocator>;

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

  ExecuteResult GetObject(const KeyType& key,
                          std::vector<HandlerType>& handles) const;

  uint32_t GetUseCount(const KeyType& key) const;

  uint32_t GetUseCount(const KeyType& key,
                       const std::atomic_uint32_t* use_count_ptr) const;

  uint32_t GetUseCount(const KeyType& key, const ValueType& object) const;

  ExecuteResult GetUseCount(const KeyType& key,
                            std::vector<std::uint32_t>& use_counts) const;

 protected:
  ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                MapTrait trait) override;

 private:
  ContainerType data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMap<KeyType, ValueType, Less, Allocator>::ManagedObjectMap()
    : ManagedObjectTableBase<KeyType, ValueType, ContainerTrait>(this),
      data_(),
      data_mutex_() {}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMap<KeyType, ValueType, Less, Allocator>::~ManagedObjectMap() {
  if (GetSize() != 0) {
    LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMap<KeyType, ValueType, Less, Allocator>::ManagedObjectMap(
    ManagedObjectMap&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMap<KeyType, ValueType, Less, Allocator>&
ManagedObjectMap<KeyType, ValueType, Less, Allocator>::operator=(
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
size_t ManagedObjectMap<KeyType, ValueType, Less, Allocator>::GetSize() const {
  return data_.size();
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
bool ManagedObjectMap<KeyType, ValueType, Less, Allocator>::Have(
    const KeyType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  typename ContainerType::const_iterator iter = data_.find(key);
  return iter != data_.end();
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
uint32_t ManagedObjectMap<KeyType, ValueType, Less, Allocator>::Count(
    const KeyType& key) const {
  if (Have(key)) {
    return 1;
  }

  return 0;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
bool ManagedObjectMap<KeyType, ValueType, Less, Allocator>::IsMultiContainer()
    const {
  return false;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
bool ManagedObjectMap<KeyType, ValueType, Less,
                      Allocator>::IsRelationshipContainer() const {
  return true;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult ManagedObjectMap<KeyType, ValueType, Less, Allocator>::AddObject(
    const KeyType& key, ValueType&& managed_object, HandlerType& handler) {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::iterator, bool> insert_result =
      data_.emplace(std::make_pair(key, std::move(managed_object)));
  handler = HandlerType{
      BaseType::GetThisPtrPtr(), &(insert_result.first->first),
      const_cast<ValueType*>(insert_result.first->second.GetObjectPtr()),
      insert_result.first->second.GetUseCountPtr()};

  if (insert_result.second) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::SUCCESS;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMap<KeyType, ValueType, Less, Allocator>::RemoveObjectImp(
    const KeyType& removed_object_key, ContainerTrait) {
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult ManagedObjectMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, HandlerType& handle) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::const_iterator iter = data_.find(key);

  if (iter == data_.end()) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  handle = HandlerType{BaseType::GetThisPtrPtr(), &(iter->first),
                       const_cast<ValueType*>(iter->second.GetObjectPtr()),
                       iter->second.GetUseCountPtr()};

  return ExecuteResult::SUCCESS;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
uint32_t ManagedObjectMap<KeyType, ValueType, Less, Allocator>::GetUseCount(
    const KeyType& key) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::const_iterator iter = data_.find(key);

  if (iter == data_.end()) {
    return 0;
  }

  return iter->second.GetUseCount();
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Less,
                      Allocator>::ManagedObjectMultiMap()
    : ManagedObjectTableBase<KeyType, ValueType, ContainerTrait>(this),
      data_(),
      data_mutex_() {}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Less,
                      Allocator>::~ManagedObjectMultiMap() {
  if (GetSize() != 0) {
    LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::
    ManagedObjectMultiMap(ManagedObjectMultiMap&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard(other.data_mutex_);

  BaseType::operator=(std::move(other));
  data_ = std::move(data_);
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>&
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::operator=(
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
size_t ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetSize()
    const {
  return data_.size();
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
bool ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::Have(
    const KeyType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  return data_.count(key) != 0;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
uint32_t ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::Count(
    const KeyType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  return data_.count(key);
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
bool ManagedObjectMultiMap<KeyType, ValueType, Less,
                           Allocator>::IsMultiContainer() const {
  return true;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
bool ManagedObjectMultiMap<KeyType, ValueType, Less,
                           Allocator>::IsRelationshipContainer() const {
  return true;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::AddObject(
    const KeyType& key, ValueType&& managed_object, HandlerType& handler) {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  typename ContainerType::iterator insert_result =
      data_.emplace(std::make_pair(key, std::move(managed_object)));
  handler =
      HandlerType{BaseType::GetThisPtrPtr(), &(insert_result->first),
                  const_cast<ValueType*>(insert_result->second.GetObjectPtr()),
                  insert_result->second.GetUseCountPtr()};

  return ExecuteResult::SUCCESS;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::RemoveObjectImp(
    const KeyType& removed_object_key,
    const std::atomic_uint32_t* use_count_ptr, MapTrait) {
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, HandlerType& handler) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  handler = HandlerType{
      BaseType::GetThisPtrPtr(), &(equal_range.first->first),
      const_cast<ValueType*>(equal_range.first->second.GetObjectPtr()),
      equal_range.first->second.GetUseCountPtr()};

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
    HandlerType& handler) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
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
      handler = HandlerType{BaseType::GetThisPtrPtr(), &(iter->first),
                            const_cast<ValueType*>(iter->second.GetObjectPtr()),
                            iter->second.GetUseCountPtr()};

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, const ValueType& object, HandlerType& handler) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
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
      handler = HandlerType{BaseType::GetThisPtrPtr(), &(iter->first),
                            const_cast<ValueType*>(iter->second.GetObjectPtr()),
                            iter->second.GetUseCountPtr()};

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, std::vector<HandlerType>& handles) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
uint32_t
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetUseCount(
    const KeyType& key) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
uint32_t
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetUseCount(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
uint32_t
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetUseCount(
    const KeyType& key, const ValueType& object) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
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

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
ExecuteResult
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetUseCount(
    const KeyType& key, std::vector<std::uint32_t>& use_counts) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
    return ExecuteResult::OBJECT_IS_INVALID;
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
