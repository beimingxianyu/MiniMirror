#pragma once

#include <cassert>
#include <list>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Allocator = std::allocator<ObjectType>>
class ManagedObjectList
    : public ManagedObjectTableBase<ObjectType, ObjectType, NoKeyTrait> {
 public:
  using RelationshipContainerTrait = NoKeyTrait;
  using ThisType = ManagedObjectList<ObjectType, Allocator>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType,
                                          RelationshipContainerTrait>;
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

  uint32_t Count(const ObjectType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  ExecuteResult AddObject(ObjectType&& managed_object, HandlerType& handle);

  ExecuteResult GetObject(const ObjectType& key, HandlerType& handle) const;

  ExecuteResult GetObject(const ObjectType& key,
                          const std::atomic_uint32_t* use_count_ptr,
                          HandlerType& handle) const;

  ExecuteResult GetObject(const ObjectType& key,
                          std::vector<HandlerType>& handles) const;

  uint32_t GetUseCount(const ObjectType& key) const;

  uint32_t GetUseCount(const ObjectType& key,
                       const std::atomic_uint32_t* use_count_ptr) const;

  void GetUseCount(const ObjectType& key,
                   std::vector<uint32_t>& use_counts) const;

  ContainerType& GetContainer() { return data_; }

 protected:
  ExecuteResult AddObjectImp(ObjectType&& managed_object,
                             HandlerType& handle) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                NoKeyTrait trait) override;

  ExecuteResult GetObjectImp(const ObjectType& key,
                             HandlerType& handle) const override;

  ExecuteResult GetObjectImp(const ObjectType& key,
                             const std::atomic_uint32_t* use_count_ptr,
                             ThisType::HandlerType& handler) const override;

  ExecuteResult GetObjectImp(const ObjectType& key,
                             std::vector<HandlerType>& handlers) const override;

  std::uint32_t GetUseCountImp(const ObjectType& key) const override;

  std::uint32_t GetUseCountImp(
      const ObjectType& key,
      const std::atomic_uint32_t* use_count_ptr) const override;

  void GetUseCountImp(const ObjectType& key,
                      std::vector<uint32_t>& use_counts) const override;

 private:
  std::list<ManagedObjectWrapper<ObjectType>> data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename ObjectType, typename Allocator>
ManagedObjectList<ObjectType, Allocator>::~ManagedObjectList() {
  if (GetSize() != 0) {
    LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename ObjectType, typename Allocator>
void ManagedObjectList<ObjectType, Allocator>::GetUseCount(
    const ObjectType& key, std::vector<uint32_t>& use_counts) const {
  GetUseCountImp(key, use_counts);
}

template <typename ObjectType, typename Allocator>
uint32_t ManagedObjectList<ObjectType, Allocator>::GetUseCount(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
  return GetUseCountImp(key, use_count_ptr);
}

template <typename ObjectType, typename Allocator>
uint32_t ManagedObjectList<ObjectType, Allocator>::GetUseCount(
    const ObjectType& key) const {
  return GetUseCountImp(key);
}

template <typename ObjectType, typename Allocator>
void ManagedObjectList<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, std::vector<uint32_t>& use_counts) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  for (const auto& object : data_) {
    if (key == object.GetObject()) {
      use_counts.push_back(object.GetUseCount());
    }
  }
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectList<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  for (const auto& object : data_) {
    if (key == object.GetObject() && use_count_ptr == object.GetUseCountPtr()) {
      return object.GetUseCount();
    }
  }

  return 0;
}

template <typename ObjectType, typename Allocator>
std::uint32_t ManagedObjectList<ObjectType, Allocator>::GetUseCountImp(
    const ObjectType& key) const {
  if (!ThisType::TestMoveWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  for (const auto& object : data_) {
    if (key == object.GetObject()) {
      return object.GetUseCount();
    }
  }

  return 0;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr,
    HandlerType& handle) const {
  return GetObjectImp(key, use_count_ptr, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr,
    ThisType::HandlerType& handler) const {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  bool find = false;
  HandlerType new_handler;
  for (const auto& object : data_) {
    if (object.GetObject() == key && object.GetUseCountPtr() == use_count_ptr) {
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

template <typename ObjectType, typename Allocator>
ManagedObjectList<ObjectType, Allocator>::ManagedObjectList()
    : BaseType(this), data_(), data_mutex_() {}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key, std::vector<HandlerType>& handles) const {
  return GetObjectImp(key, handles);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key, HandlerType& handle) const {
  return GetObjectImp(key, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::AddObject(
    ObjectType&& managed_object, HandlerType& handle) {
  return AddObjectImp(std::move(managed_object), handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key,
    const std::atomic_uint32_t* use_count_ptr, NoKeyTrait trait) {
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

template <typename ObjectType, typename Allocator>
ManagedObjectList<ObjectType, Allocator>&
ManagedObjectList<ObjectType, Allocator>::operator=(
    ManagedObjectList&& other) noexcept {
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

template <typename ObjectType, typename Allocator>
ManagedObjectList<ObjectType, Allocator>::ManagedObjectList(
    ManagedObjectList&& other) noexcept
    : BaseType() {
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename ObjectType, typename Allocator>
uint32_t ManagedObjectList<ObjectType, Allocator>::Count(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::uint32_t result = 0;
  for (const auto& object : data_) {
    if (object.GetObject() == key) {
      ++result;
    }
  }

  return result;
}

template <typename ObjectType, typename Allocator>
bool ManagedObjectList<ObjectType, Allocator>::IsRelationshipContainer() const {
  return false;
}

template <typename ObjectType, typename Allocator>
bool ManagedObjectList<ObjectType, Allocator>::IsMultiContainer() const {
  return true;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, std::vector<HandlerType>& handlers) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  for (const auto& object : data_) {
    if (object.GetObject() == key) {
      handlers.emplace_back(BaseType::GetThisPtrPtr(),
                            const_cast<ObjectType*>(object.GetObjectPtr()),
                            object.GetUseCountPtr());
    }
  }

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key, HandlerType& handle) const {
  if (!ThisType::TestMovedWhenGetObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  HandlerType new_handler;
  bool find = false;

  for (auto iter = data_.begin(); iter == data_.end(); ++iter) {
    if (iter->GetObject() == key) {
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

template <typename ObjectType, typename Allocator>
bool ManagedObjectList<ObjectType, Allocator>::Have(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    if (iter->GetObject() == key) {
      return true;
    }
  }

  return false;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::AddObjectImp(
    ObjectType&& managed_object, HandlerType& handle) {
  if (!ThisType::TestMovedWhenAddObject()) {
    return ExecuteResult::OPERATION_NOT_SUPPORTED;
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

template <typename ObjectType, typename Allocator>
size_t ManagedObjectList<ObjectType, Allocator>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
