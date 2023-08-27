#pragma once

#include <map>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"

namespace MM {
namespace Manager {
template <typename KeyType, typename ValueType, typename Less = std::less<>,
          typename Allocator = std::allocator<
              std::pair<const KeyType, ManagedObjectWrapper<ValueType>>>>
class ManagedObjectMap final
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

  uint32_t GetSize(const KeyType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  Result<HandlerType, ErrorResult> AddObject(const KeyType& key, ValueType&& managed_object);

  Result<HandlerType, ErrorResult> GetObject(const KeyType& key) const;

  uint32_t GetUseCount(const KeyType& key) const;

 protected:
  Result<Nil, ErrorResult> RemoveObjectImp(const KeyType& removed_object_key,
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

  uint32_t GetSize(const KeyType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  Result<HandlerType, ErrorResult> AddObject(const KeyType& key, ValueType&& managed_object);

  Result<HandlerType, ErrorResult> GetObject(const KeyType& key) const;

  Result<HandlerType, ErrorResult> GetObject(const KeyType& key,
                          const std::atomic_uint32_t* use_count_ptr) const;

  Result<HandlerType, ErrorResult> GetObject(const KeyType& key, const ValueType& object) const;

  Result<std::vector<HandlerType>, ErrorResult> GetObject(const KeyType& key, StaticTrait::GetMultiplyObject) const;

  uint32_t GetUseCount(const KeyType& key) const;

  uint32_t GetUseCount(const KeyType& key,
                       const std::atomic_uint32_t* use_count_ptr) const;

  uint32_t GetUseCount(const KeyType& key, const ValueType& object) const;

  Result<std::vector<std::uint32_t>, ErrorResult> GetUseCount(const KeyType& key, StaticTrait::GetMultiplyObject) const;

 protected:
  Result<Nil, ErrorResult> RemoveObjectImp(const KeyType& removed_object_key,
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
    MM_LOG_ERROR(
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
    MM_LOG_ERROR(
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
uint32_t ManagedObjectMap<KeyType, ValueType, Less, Allocator>::GetSize(
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
MM::Result<typename ManagedObjectMap<KeyType, ValueType, Less, Allocator>::HandlerType, ErrorResult> ManagedObjectMap<KeyType, ValueType, Less, Allocator>::AddObject(
    const KeyType& key, ValueType&& managed_object) {
  if (!ThisType::TestMovedWhenAddObject()) {
     return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::iterator, bool> insert_result =
      data_.emplace(std::make_pair(key, std::move(managed_object)));

    return Result<HandlerType, ErrorResult>(st_execute_success,
                                            BaseType::GetThisPtrPtr(), &(insert_result.first->first),
                                            const_cast<ValueType*>(insert_result.first->second.GetObjectPtr()),
                                            insert_result.first->second.GetUseCountPtr());
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
MM::Result<Nil, ErrorResult>
ManagedObjectMap<KeyType, ValueType, Less, Allocator>::RemoveObjectImp(
    const KeyType& removed_object_key, ContainerTrait) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};

  if (ThisType::this_ptr_ptr_ == nullptr) {
     return MM::Result<Nil, ErrorResult>(st_execute_error, ErrorCode::CUSTOM_ERROR);
  }

  auto iter = data_.find(removed_object_key);

  if (iter == data_.end()) {
      return MM::Result<Nil, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  if (iter->second.GetUseCount() == 0) {
    data_.erase(iter);
  }

  return MM::Result<Nil, ErrorResult>(st_execute_success);
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
MM::Result<typename ManagedObjectMap<KeyType, ValueType, Less, Allocator>::HandlerType, ErrorResult> ManagedObjectMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::const_iterator iter = data_.find(key);

  if (iter == data_.end()) {
     return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

return Result<Nil, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(), &(iter->first),
                                const_cast<ValueType*>(iter->second.GetObjectPtr()),
                                iter->second.GetUseCountPtr());
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
    MM_LOG_ERROR(
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
    MM_LOG_ERROR(
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
uint32_t ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetSize(
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
MM::Result<typename ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::AddObject(
    const KeyType& key, ValueType&& managed_object) {
  if (!ThisType::TestMovedWhenAddObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  typename ContainerType::iterator insert_result =
      data_.emplace(std::make_pair(key, std::move(managed_object)));

  return Result<HandlerType, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(), &(insert_result->first),
                                          const_cast<ValueType*>(insert_result->second.GetObjectPtr()),
                                          insert_result->second.GetUseCountPtr());
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
Result<Nil, ErrorResult>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::RemoveObjectImp(
    const KeyType& removed_object_key,
    const std::atomic_uint32_t* use_count_ptr, MapTrait) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  if (ThisType::this_ptr_ptr_ == nullptr) {
      return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::CUSTOM_ERROR);
  }

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(removed_object_key);
  if (equal_range.first == equal_range.second) {
     return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if (iter->second.GetUseCountPtr() == use_count_ptr) {
      if (iter->second.GetUseCount() == 0) {
        data_.erase(iter);
      }

      return Result<Nil, ErrorResult>(st_execute_success);
    }
  }

  return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
MM::Result<typename ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key) const {
  if (!ThisType::TestMovedWhenGetObject()) {
     return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  return Result<HandlerType, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(), &(equal_range.first->first),
                                          const_cast<ValueType*>(equal_range.first->second.GetObjectPtr()),
                                          equal_range.first->second.GetUseCountPtr());
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
MM::Result<typename ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr) const {
  if (!ThisType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  for (typename ContainerType::const_iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if (iter->second.GetUseCountPtr() == use_count_ptr) {
      return Result<HandlerType, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(), &(iter->first),
                                              const_cast<ValueType*>(iter->second.GetObjectPtr()),
                                              iter->second.GetUseCountPtr());
    }
  }


    return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
MM::Result<typename ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, const ValueType& object) const {
  if (!ThisType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  for (typename ContainerType::const_iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if (iter->second.GetObject() == object) {
        return Result<HandlerType, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(), &(iter->first),
                                                const_cast<ValueType*>(iter->second.GetObjectPtr()),
                                                iter->second.GetUseCountPtr());
    }
  }

  return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
}

template <typename KeyType, typename ValueType, typename Less,
          typename Allocator>
MM::Result<std::vector<typename ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::HandlerType>, ErrorResult>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetObject(
    const KeyType& key, StaticTrait::GetMultiplyObject) const {
  if (!ThisType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  std::vector<HandlerType> handles;
  for (typename ContainerType::const_iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    handles.emplace_back(BaseType::GetThisPtrPtr(), &(iter->first),
                         const_cast<ValueType*>(iter->second.GetObjectPtr()),
                         iter->second.GetUseCountPtr());
  }

  return Result<std::vector<HandlerType>, ErrorResult>(st_execute_success, std::move(handles));
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
 MM::Result<std::vector<std::uint32_t>, ErrorResult>
ManagedObjectMultiMap<KeyType, ValueType, Less, Allocator>::GetUseCount(
    const KeyType& key, StaticTrait::GetMultiplyObject) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::const_iterator,
            typename ContainerType::const_iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  std::vector<HandlerType> use_counts;
  for (typename ContainerType::const_iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    use_counts.emplace_back(iter->second.GetUseCount());
  }

  return Result<HandlerType, ErrorResult>(st_execute_success, std::move(use_counts));
}
}  // namespace Manager
}  // namespace MM
