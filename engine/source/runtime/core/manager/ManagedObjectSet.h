#pragma once

#include <cassert>
#include <set>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType>
struct WrapperLess {
  bool operator()(const ManagedObjectWrapper<ObjectType>& lhs,
                  const ManagedObjectWrapper<ObjectType>& rhs) const {
    return std::less{}(lhs.GetObject(), rhs.GetObject());
  }
};

template <typename ObjectType, typename Allocator = std::allocator<ObjectType>>
class ManagedObjectSet : public ManagedObjectTableBase<ObjectType, ObjectType> {
 public:
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType>;
  using HandleType = ManagedObjectHandle<ObjectType, ObjectType>;
  using ContainerType = std::set<ManagedObjectWrapper<ObjectType>,
                                 WrapperLess<ObjectType>, Allocator>;

 public:
  ManagedObjectSet();
  ~ManagedObjectSet() = default;
  ManagedObjectSet(const ManagedObjectSet& other) = delete;
  ManagedObjectSet(ManagedObjectSet&& other) noexcept;
  ManagedObjectSet& operator=(const ManagedObjectSet& other) = delete;
  ManagedObjectSet& operator=(ManagedObjectSet&& other) noexcept;

 public:
  size_t GetSize() const override;

  bool Have(const ObjectType& key) const override;

  uint32_t Count(const ObjectType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  ExecuteResult AddObject(ObjectType&& managed_object,
                          ManagedObjectHandle<ObjectType, ObjectType>& handle);

  ExecuteResult RemoveObject(const ObjectType& removed_object);

  ExecuteResult GetObject(
      const ObjectType& object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const;

 protected:
  ExecuteResult AddObjectImp(
      const ObjectType& key, ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override;

  ExecuteResult AddObjectImp(
      ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                ObjectType& object) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                std::atomic_uint32_t* use_count_ptr) override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key, ObjectType& object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      std::vector<ManagedObjectHandle<ObjectType, ObjectType>>& handles)
      const override;

 private:
  ContainerType data_;
  mutable std::shared_mutex data_mutex_;
};

template <typename ObjectType, typename Allocator = std::allocator<ObjectType>>
class ManagedObjectMultiSet
    : public ManagedObjectTableBase<ObjectType, ObjectType> {
 public:
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType>;
  using HandleType = ManagedObjectHandle<ObjectType, ObjectType>;
  using ContainerType = std::multiset<ManagedObjectWrapper<ObjectType>,
                                      WrapperLess<ObjectType>, Allocator>;

 public:
  ManagedObjectMultiSet();
  ~ManagedObjectMultiSet() override = default;
  ManagedObjectMultiSet(const ManagedObjectMultiSet& other) = delete;
  ManagedObjectMultiSet(ManagedObjectMultiSet&& other) noexcept;
  ManagedObjectMultiSet& operator=(const ManagedObjectMultiSet& other) = delete;
  ManagedObjectMultiSet& operator=(ManagedObjectMultiSet&& other) noexcept;

 public:
  size_t GetSize() const override { return data_.size(); }

  bool Have(const ObjectType& key) const override {
    return data_.count(key) > 0;
  }

  uint32_t Count(const ObjectType& key) const override {
    return data_.count(key);
  }

  bool IsMultiContainer() const override { return true; }

  bool IsRelationshipContainer() const override { return false; }

  ExecuteResult AddObject(ObjectType&& managed_object,
                          ManagedObjectHandle<ObjectType, ObjectType>& handle);

  ExecuteResult RemoveObject(const ObjectType& removed_object,
                             std::atomic_uint32_t* use_count_ptr);

  ExecuteResult GetObject(
      const ObjectType& key,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const;

  ExecuteResult GetObject(
      const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const;

  ExecuteResult GetObject(const ObjectType& key,
                          std::vector<HandleType>& handles) const;

 protected:
  ExecuteResult AddObjectImp(
      const ObjectType& key, ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override;

  ExecuteResult AddObjectImp(
      ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                ObjectType& object) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                std::atomic_uint32_t* use_count_ptr) override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key, ObjectType& object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(const ObjectType& key,
                             std::vector<HandleType>& handles) const override;

 private:
  ContainerType data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename ObjectType, typename Allocator>
ManagedObjectSet<ObjectType, Allocator>::ManagedObjectSet()
    : ManagedObjectTableBase<ObjectType, ObjectType>(this),
      data_(),
      data_mutex_() {}

template <typename ObjectType, typename Allocator>
ManagedObjectMultiSet<ObjectType, Allocator>::ManagedObjectMultiSet()
    : ManagedObjectTableBase<ObjectType, ObjectType>(this),
      data_(),
      data_mutex_() {}

template <typename ObjectType, typename Allocator>
ManagedObjectMultiSet<ObjectType, Allocator>::ManagedObjectMultiSet(
    ManagedObjectMultiSet&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename ObjectType, typename Allocator>
ManagedObjectMultiSet<ObjectType, Allocator>&
ManagedObjectMultiSet<ObjectType, Allocator>::operator=(
    ManagedObjectMultiSet&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  std::lock(data_mutex_, other.data_mutex_);
  std::unique_lock<std::shared_mutex> main_guard{data_mutex_, std::adopt_lock},
      other_guard{other.data_mutex_, std::adopt_lock};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);

  return *this;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::AddObject(
    ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  return AddObjectImp(std::move(managed_object), handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object, std::atomic_uint32_t* use_count_ptr) {
  return RemoveObjectImp(removed_object, use_count_ptr);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObject(
    const ObjectType& key,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(key, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObject(
    const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(key, use_count_ptr, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObject(
    const ObjectType& key, std::vector<HandleType>& handles) const {
  return GetObjectImp(key, handles);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::AddObjectImp(
    const ObjectType& key, ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::AddObjectImp(
    ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::iterator iter =
      data_.emplace(std::move(managed_object));

  handle = HandleType{BaseType::GetThisPtrPtr(), nullptr, iter->GetObjectPtr(),
                      iter->GetUseCountPtr()};

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, ObjectType& object) {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, std::atomic_uint32_t* use_count_ptr) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  ManagedObjectTableBase<ObjectType, ObjectType>* current_ptr =
      *(ManagedObjectTableBase<ObjectType, ObjectType>::this_ptr_ptr_);

  if (current_ptr != this) {
    current_ptr->RemoveObjectImp(removed_object_key, use_count_ptr);
  }

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(removed_object_key);

  for (const typename ContainerType::iterator& iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if ((*iter).GetUseCountPtr() == use_count_ptr) {
      if (iter->GetUseCount() == 0) {
        data_.erase(iter);
      }

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::iterator iter = data_.find(key);
  if (iter == data_.end()) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  handle = HandleType{BaseType ::GetThisPtrPtr(), nullptr, iter->GetObjectPtr(),
                      iter->GetUseCountPtr()};

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if (iter->GetUseCountPtr() == use_count_ptr) {
      handle = HandleType{BaseType ::GetThisPtrPtr(), nullptr,
                          iter->GetObjectPtr(), iter->GetUseCountPtr()};

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, ObjectType& object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  LOG_ERROR("This function should not be used.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, std::vector<HandleType>& handles) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    handles.emplace_back(BaseType ::GetThisPtrPtr(), nullptr,
                         iter->GetObjectPtr(), iter->GetUseCountPtr());
  }

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, ObjectType& object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ManagedObjectSet<ObjectType, Allocator>&
ManagedObjectSet<ObjectType, Allocator>::operator=(
    ManagedObjectSet&& other) noexcept {
  if (other == this) {
    return *this;
  }

  std::lock(data_mutex_, other.data_mutex_);
  std::unique_lock<std::shared_mutex> main_guard{data_mutex_, std::adopt_lock},
      other_guard{other.data_mutex_, std::adopt_lock};

  ManagedObjectTableBase<ObjectType, ObjectType>::operator=(std::move(other));
  data_ = std::move(other.data_);

  return *this;
}

template <typename ObjectType, typename Allocator>
ManagedObjectSet<ObjectType, Allocator>::ManagedObjectSet(
    ManagedObjectSet&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

  ManagedObjectTableBase<ObjectType, ObjectType>::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::AddObject(
    ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  return AddObjectImp(std::move(managed_object), handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object) {
  return RemoveObjectImp(removed_object);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObject(
    const ObjectType& object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(object, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::AddObjectImp(
    const ObjectType& key, ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::AddObjectImp(
    ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename std::set<ManagedObjectWrapper<ObjectType>,
                              WrapperLess<ObjectType>, Allocator>::iterator,
            bool>
      insert_result = data_.emplace(std::move(managed_object));

  handle = ManagedObjectHandle<ObjectType, ObjectType>{
      ManagedObjectTableBase<ObjectType, ObjectType>::GetThisPtrPtr(), nullptr,
      insert_result.first->GetObjectPtr(),
      insert_result.first->GetUseCountPtr()};

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  ManagedObjectTableBase<ObjectType, ObjectType>* current_ptr =
      *(ManagedObjectTableBase<ObjectType, ObjectType>::this_ptr_ptr_);

  if (current_ptr != this) {
    current_ptr->RemoveObjectImp(removed_object_key);
  }

  auto iter = data_.find(removed_object_key);

  if (iter == data_.end()) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  if (iter->GetUseCount() == 0) {
    data_.erase(iter);
  }

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, ObjectType& object) {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, std::atomic_uint32_t* use_count_ptr) {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  auto iter = data_.find(key);

  if (iter != data_.end()) {
    handle = ManagedObjectHandle<ObjectType, ObjectType>{
        ManagedObjectTableBase<ObjectType, ObjectType>::GetThisPtrPtr(),
        nullptr, iter->GetObjectPtr(), iter->GetUseCountPtr()};

    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key,
    std::vector<ManagedObjectHandle<ObjectType, ObjectType>>& handles) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
bool ManagedObjectSet<ObjectType, Allocator>::Have(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  return data_.find(key);
}

template <typename ObjectType, typename Allocator>
bool ManagedObjectSet<ObjectType, Allocator>::IsRelationshipContainer() const {
  return false;
}

template <typename ObjectType, typename Allocator>
bool ManagedObjectSet<ObjectType, Allocator>::IsMultiContainer() const {
  return false;
}

template <typename ObjectType, typename Allocator>
uint32_t ManagedObjectSet<ObjectType, Allocator>::Count(
    const ObjectType& key) const {
  if (Have(key)) {
    return 1;
  }

  return 0;
}

template <typename ObjectType, typename Allocator>
size_t ManagedObjectSet<ObjectType, Allocator>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
