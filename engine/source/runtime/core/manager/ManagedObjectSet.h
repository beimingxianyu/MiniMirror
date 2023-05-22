#pragma once

#include <cassert>
#include <set>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>>
class ManagedObjectSet : public ManagedObjectTableBase<ObjectType, ObjectType> {
 public:
  using ThisType = ManagedObjectSet<ObjectType, Allocator>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType>;
  using HandlerType = ManagedObjectHandler<ObjectType, ObjectType>;
  using ContainerType =
      std::set<ManagedObjectWrapper<ObjectType>, std::less<>, Allocator>;

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
                          ManagedObjectHandler<ObjectType, ObjectType>& handle);

  ExecuteResult GetObject(
      const ObjectType& object,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) const;

  std::uint32_t GetUseCount(const ObjectType& object) const;

 protected:
  ExecuteResult AddObjectImp(
      const ObjectType& key, ObjectType&& managed_object,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) override;

  ExecuteResult AddObjectImp(
      ObjectType&& managed_object,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                ObjectType& object) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                std::atomic_uint32_t* use_count_ptr) override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(const ObjectType& key,
                             const std::atomic_uint32_t* use_count_ptr,
                             ThisType::HandlerType& handle) const override;

  ExecuteResult GetObjectImp(const ObjectType& key, const ObjectType& object,
                             ThisType::HandlerType& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      std::vector<ManagedObjectHandler<ObjectType, ObjectType>>& handles)
      const override;

  std::uint32_t GetUseCountImp(const ObjectType& key) const override;

  std::uint32_t GetUseCountImp(
      const ObjectType& key,
      const std::atomic_uint32_t* use_count_ptr) const override;

  std::uint32_t GetUseCountImp(const ObjectType& key,
                               const ObjectType& object) const override;

  void GetUseCountImp(const ObjectType& key,
                      std::vector<std::uint32_t>& use_counts) const override;

 private:
  ContainerType data_;
  mutable std::shared_mutex data_mutex_;
};

template <typename ObjectType,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>>
class ManagedObjectMultiSet
    : public ManagedObjectTableBase<ObjectType, ObjectType> {
 public:
  using ThisType = ManagedObjectMultiSet<ObjectType, Allocator>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType>;
  using HandlerTyper = ManagedObjectHandler<ObjectType, ObjectType>;
  using ContainerType =
      std::multiset<ManagedObjectWrapper<ObjectType>, std::less<>, Allocator>;

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
                          ManagedObjectHandler<ObjectType, ObjectType>& handle);

  ExecuteResult GetObject(
      const ObjectType& key,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) const;

  ExecuteResult GetObject(
      const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) const;

  ExecuteResult GetObject(const ObjectType& key,
                          std::vector<HandlerTyper>& handles) const;

  std::uint32_t GetUseCount(const ObjectType& key) const;

  std::uint32_t GetUseCount(const ObjectType& key,
                            std::atomic_uint32_t* use_count_ptr) const;

  void GetUseCount(const ObjectType& key,
                   std::vector<std::uint32_t>& use_counts) const;

 protected:
  ExecuteResult AddObjectImp(
      const ObjectType& key, ObjectType&& managed_object,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) override;

  ExecuteResult AddObjectImp(
      ObjectType&& managed_object,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                ObjectType& object) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                std::atomic_uint32_t* use_count_ptr) override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      ManagedObjectHandler<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(const ObjectType& key,
                             const std::atomic_uint32_t* use_count_ptr,
                             ThisType::HandlerTyper& handle) const override;

  ExecuteResult GetObjectImp(const ObjectType& key, const ObjectType& object,
                             ThisType::HandlerTyper& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      std::vector<HandlerTyper>& handlers) const override;

  std::uint32_t GetUseCountImp(const ObjectType& key) const override;

  std::uint32_t GetUseCountImp(
      const ObjectType& key,
      const std::atomic_uint32_t* use_count_ptr) const override;

  std::uint32_t GetUseCountImp(const ObjectType& key,
                               const ObjectType& object) const override;

  void GetUseCountImp(const ObjectType& key,
                      std::vector<std::uint32_t>& use_counts) const override;

 private:
  ContainerType data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename ObjectType, typename Allocator>
void ManagedObjectMultiSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, std::vector<std::uint32_t>& use_counts) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first != equal_range.second) {
    return;
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter == equal_range.second; ++iter) {
    use_counts.emplace_back(iter->GetUseCount());
  }
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectMultiSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, const ObjectType& object) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return 0;
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectMultiSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return 0;
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if (iter->GetUseCountPtr() == use_count_ptr) {
      return iter->GetUseCount();
    }
  }

  return 0;
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectMultiSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return 0;
  }

  return equal_range.first->GetUseCount();
}

template <typename ObjectType, typename Allocator>
void ManagedObjectMultiSet<ObjectType, Allocator>::GetUseCount(
    const ObjectType& key, std::vector<std::uint32_t>& use_counts) const {
  GetUseCountImp(key, use_counts);
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectMultiSet<ObjectType, Allocator>::GetUseCount(
    const ObjectType& key, std::atomic_uint32_t* use_count_ptr) const {
  return GetUseCountImp(key, use_count_ptr);
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectMultiSet<ObjectType, Allocator>::GetUseCount(
    const ObjectType& key) const {
  return GetUseCountImp(key);
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectSet<ObjectType, Allocator>::GetUseCount(
    const ObjectType& object) const {
  return GetUseCountImp(object);
}

template <typename ObjectType, typename Allocator>
void ManagedObjectSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, std::vector<std::uint32_t>& use_counts) const {
  LOG_ERROR("This function should not be called.");
  assert(false);
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, const ObjectType& object) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return 0;
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return 0;
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectSet<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::iterator iter = data_.find(key);
  if (iter != data_.end()) {
    return iter->GetUseCount();
  }

  return 0;
}

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
  if (!data_.empty()) {
    LOG_FATAL(
        "If there is data in the original container but it is reassigned, an "
        "access error will occur.");
  }

  std::unique_lock<std::shared_mutex> main_guard{data_mutex_, std::adopt_lock},
      other_guard{other.data_mutex_, std::adopt_lock};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);

  return *this;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::AddObject(
    ObjectType&& managed_object,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) {
  return AddObjectImp(std::move(managed_object), handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObject(
    const ObjectType& key,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(key, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObject(
    const ObjectType& key, std::atomic_uint32_t* use_count_ptr,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(key, use_count_ptr, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObject(
    const ObjectType& key, std::vector<HandlerTyper>& handles) const {
  return GetObjectImp(key, handles);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::AddObjectImp(
    const ObjectType& key, ObjectType&& managed_object,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::AddObjectImp(
    ObjectType&& managed_object,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::iterator iter =
      data_.emplace(std::move(managed_object));

  guard.unlock();
  handle = HandlerTyper{BaseType::GetThisPtrPtr(), nullptr,
                        const_cast<ObjectType*>(iter->GetObjectPtr()),
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
  if (ThisType::this_ptr_ptr_ == nullptr) {
    return ExecuteResult::CUSTOM_ERROR;
  }

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(removed_object_key);

  for (typename ContainerType::iterator& iter = equal_range.first;
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
    ManagedObjectHandler<ObjectType, ObjectType>& handle) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  guard.unlock();
  handle =
      HandlerTyper{BaseType ::GetThisPtrPtr(), nullptr,
                   const_cast<ObjectType*>(equal_range.first->GetObjectPtr()),
                   equal_range.first->GetUseCountPtr()};

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr,
    ThisType::HandlerTyper& handle) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  bool find = false;
  HandlerTyper new_handler;

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if (iter->GetUseCountPtr() == use_count_ptr) {
      new_handler = HandlerTyper{BaseType ::GetThisPtrPtr(), nullptr,
                                 const_cast<ObjectType*>(iter->GetObjectPtr()),
                                 iter->GetUseCountPtr()};
      find = true;
      break;
    }
  }

  if (find) {
    guard.unlock();
    handle = new_handler;
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, const ObjectType& object,
    ThisType::HandlerTyper& handle) const {
  LOG_ERROR("This function should not be used.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectMultiSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, std::vector<HandlerTyper>& handlers) const {
  for (const auto& handler : handlers) {
    if (handler.IsValid()) {
      handler.Release();
    }
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    handlers.emplace_back(BaseType ::GetThisPtrPtr(), nullptr,
                          const_cast<ObjectType*>(iter->GetObjectPtr()),
                          iter->GetUseCountPtr());
  }

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, const ObjectType& object,
    ThisType::HandlerType& handle) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr,
    ThisType::HandlerType& handle) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ManagedObjectSet<ObjectType, Allocator>&
ManagedObjectSet<ObjectType, Allocator>::operator=(
    ManagedObjectSet&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  std::lock(data_mutex_, other.data_mutex_);
  if (!data_.empty()) {
    LOG_FATAL(
        "If there is data in the original container but it is reassigned, an "
        "access error will occur.");
  }

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
    ManagedObjectHandler<ObjectType, ObjectType>& handle) {
  return AddObjectImp(std::move(managed_object), handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObject(
    const ObjectType& object,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(object, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::AddObjectImp(
    const ObjectType& key, ObjectType&& managed_object,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::AddObjectImp(
    ObjectType&& managed_object,
    ManagedObjectHandler<ObjectType, ObjectType>& handle) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::iterator, bool> insert_result =
      data_.emplace(std::move(managed_object));
  guard.unlock();

  handle = ManagedObjectHandler<ObjectType, ObjectType>{
      ManagedObjectTableBase<ObjectType, ObjectType>::GetThisPtrPtr(), nullptr,
      const_cast<ObjectType*>(insert_result.first->GetObjectPtr()),
      insert_result.first->GetUseCountPtr()};

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};

  if (ThisType::this_ptr_ptr_ == nullptr) {
    return ExecuteResult::CUSTOM_ERROR;
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
    ManagedObjectHandler<ObjectType, ObjectType>& handle) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  HandlerType handler;

  auto iter = data_.find(key);

  if (iter != data_.end()) {
    guard.unlock();
    handle = ManagedObjectHandler<ObjectType, ObjectType>{
        ManagedObjectTableBase<ObjectType, ObjectType>::GetThisPtrPtr(),
        nullptr, const_cast<ObjectType*>(iter->GetObjectPtr()),
        iter->GetUseCountPtr()};

    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectSet<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key,
    std::vector<ManagedObjectHandler<ObjectType, ObjectType>>& handles) const {
  LOG_ERROR("This function should not be called.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
bool ManagedObjectSet<ObjectType, Allocator>::Have(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  return data_.find(key) != data_.end();
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