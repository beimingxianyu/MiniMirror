#pragma once

#include <cassert>
#include <list>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Equal = std::equal_to<>,
          typename Allocator = std::allocator<ObjectType>>
class ManagedObjectList
    : public ManagedObjectTableBase<ObjectType, ObjectType, ListTrait> {
 public:
  using ContainerTrait = ListTrait;
  using ThisType = ManagedObjectList<ObjectType, Allocator>;
  using BaseType =
      ManagedObjectTableBase<ObjectType, ObjectType, ContainerTrait>;
  using HandlerType = typename BaseType::HandlerType;
  using WrapperType = typename BaseType::WrapperType;
  using ContainerType = std::list<ManagedObjectWrapper<ObjectType>>;

 public:
  ManagedObjectList();
  ~ManagedObjectList() override;
  ManagedObjectList(const ManagedObjectList& other) = delete;
  ManagedObjectList(ManagedObjectList&& other) noexcept;
  ManagedObjectList& operator=(const ManagedObjectList& other) = delete;
  ManagedObjectList& operator=(ManagedObjectList&& other) noexcept;

 public:
  size_t GetSize() const override;

  bool Have(const ObjectType& key) const override;

  uint32_t GetSize(const ObjectType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  ExecuteResult AddObject(ObjectType&& managed_object, HandlerType& handle);

  ExecuteResult GetObject(const ObjectType& key, HandlerType& handle) const;

  ExecuteResult GetObject(const ObjectType& key,
                          const std::atomic_uint32_t* use_count_ptr,
                          HandlerType& handle) const;

  ExecuteResult GetObject(const ObjectType& key,
                          std::vector<HandlerType>& handlers) const;

  ContainerType& GetContainer() { return data_; }

  std::uint32_t GetUseCount(const ObjectType& key) const;

  std::uint32_t GetUseCount(const ObjectType& key,
                            const std::atomic_uint32_t* use_count_ptr) const;

  ExecuteResult GetUseCount(const ObjectType& key,
                            std::vector<std::uint32_t>& use_counts) const;

 protected:
  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                ListTrait trait) override;

 private:
  std::list<ManagedObjectWrapper<ObjectType>> data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename ObjectType, typename Equal, typename Allocator>
ManagedObjectList<ObjectType, Equal, Allocator>::~ManagedObjectList() {
  if (GetSize() != 0) {
    MM_LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename ObjectType, typename Equal, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Equal, Allocator>::GetUseCount(
    const ObjectType& key, std::vector<std::uint32_t>& use_counts) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  bool have = false;
  for (const auto& object : data_) {
    if (Equal{}(object.GetObject(), key)) {
      have = true;
      use_counts.push_back(object.GetUseCount());
    }
  }

  if (!have) {
    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Equal, typename Allocator>
std::uint32_t ManagedObjectList<ObjectType, Equal, Allocator>::GetUseCount(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  for (const auto& object : data_) {
    if (Equal{}(object.GetObject(), key) &&
        use_count_ptr == object.GetUseCountPtr()) {
      return object.GetUseCount();
    }
  }

  return 0;
}

template <typename ObjectType, typename Equal, typename Allocator>
std::uint32_t ManagedObjectList<ObjectType, Equal, Allocator>::GetUseCount(
    const ObjectType& key) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  for (const auto& object : data_) {
    if (Equal{}(object.GetObject(), key)) {
      return object.GetUseCount();
    }
  }

  return 0;
}

template <typename ObjectType, typename Equal, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Equal, Allocator>::GetObject(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr,
    HandlerType& handler) const {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  bool find = false;
  HandlerType new_handler;
  for (const auto& object : data_) {
    if (Equal{}(object.GetObject(), key) &&
        object.GetUseCountPtr() == use_count_ptr) {
      new_handler = HandlerType(BaseType::GetThisPtrPtr(),
                                const_cast<ObjectType*>(object.GetObjectPtr()),
                                object.GetUseCountPtr());
      find = true;
      break;
    }
  }

  if (find) {
    guard.unlock();
    handler = new_handler;
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename ObjectType, typename Equal, typename Allocator>
ManagedObjectList<ObjectType, Equal, Allocator>::ManagedObjectList()
    : BaseType(this), data_(), data_mutex_() {}

template <typename ObjectType, typename Equal, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Equal, Allocator>::GetObject(
    const ObjectType& key, std::vector<HandlerType>& handlers) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  for (const auto& object : data_) {
    if (Equal{}(object.GetObject(), key)) {
      handlers.emplace_back(BaseType::GetThisPtrPtr(),
                            const_cast<ObjectType*>(object.GetObjectPtr()),
                            object.GetUseCountPtr());
    }
  }

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Equal, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Equal, Allocator>::GetObject(
    const ObjectType& key, HandlerType& handle) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  HandlerType new_handler;
  bool find = false;

  for (auto iter = data_.begin(); iter == data_.end(); ++iter) {
    if (Equal{}(iter->GetObject(), key)) {
      new_handler = HandlerType{BaseType::GetThisPtrPtr(),
                                const_cast<ObjectType*>(iter->GetObjectPtr()),
                                iter->GetUseCountPtr()};
      find = true;
      break;
    }
  }

  if (find) {
    guard.unlock();
    handle = std::move(new_handler);
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename ObjectType, typename Equal, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Equal, Allocator>::AddObject(
    ObjectType&& managed_object, HandlerType& handle) {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OBJECT_IS_INVALID;
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  data_.emplace_front(std::move(managed_object));
  HandlerType new_handler =
      HandlerType{BaseType::GetThisPtrPtr(), data_.front().GetObjectPtr(),
                  data_.front().GetUseCountPtr()};
  guard.unlock();
  handle = std::move(new_handler);
  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Equal, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Equal, Allocator>::RemoveObjectImp(
    const ObjectType&, const std::atomic_uint32_t* use_count_ptr, ListTrait) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  if (ThisType::this_ptr_ptr_ == nullptr) {
    return ExecuteResult::CUSTOM_ERROR;
  }

  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    if (iter->GetUseCountPtr() == use_count_ptr) {
      if (iter->GetUseCount() == 0) {
        data_.erase(iter);
      }

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
}

template <typename ObjectType, typename Equal, typename Allocator>
ManagedObjectList<ObjectType, Equal, Allocator>&
ManagedObjectList<ObjectType, Equal, Allocator>::operator=(
    ManagedObjectList&& other) noexcept {
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

template <typename ObjectType, typename Equal, typename Allocator>
ManagedObjectList<ObjectType, Equal, Allocator>::ManagedObjectList(
    ManagedObjectList&& other) noexcept
    : BaseType() {
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename ObjectType, typename Equal, typename Allocator>
uint32_t ManagedObjectList<ObjectType, Equal, Allocator>::GetSize(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::uint32_t result = 0;
  for (const auto& object : data_) {
    if (Equal{}(object.GetObject(), key)) {
      ++result;
    }
  }

  return result;
}

template <typename ObjectType, typename Equal, typename Allocator>
bool ManagedObjectList<ObjectType, Equal, Allocator>::IsRelationshipContainer()
    const {
  return false;
}

template <typename ObjectType, typename Equal, typename Allocator>
bool ManagedObjectList<ObjectType, Equal, Allocator>::IsMultiContainer() const {
  return true;
}

template <typename ObjectType, typename Equal, typename Allocator>
bool ManagedObjectList<ObjectType, Equal, Allocator>::Have(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    if (Equal{}(iter->GetObject(), key)) {
      return true;
    }
  }

  return false;
}

template <typename ObjectType, typename Equal, typename Allocator>
size_t ManagedObjectList<ObjectType, Equal, Allocator>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
