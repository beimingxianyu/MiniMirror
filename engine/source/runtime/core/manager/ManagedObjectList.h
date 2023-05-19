#pragma once

#include <cassert>
#include <list>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Allocator = std::allocator<ObjectType>>
class ManagedObjectList
    : private ManagedObjectTableBase<ObjectType, ObjectType> {
 public:
  ManagedObjectList() = default;
  ManagedObjectList(const ManagedObjectList& other) = delete;
  ManagedObjectList(ManagedObjectList&& other) noexcept;
  ManagedObjectList& operator=(const ManagedObjectList& other) = delete;
  ManagedObjectList& operator=(ManagedObjectList&& other) noexcept;
  ~ManagedObjectList() override = default;

 public:
  size_t GetSize() const override;

  bool Have(const ObjectType& key) const override;

  uint32_t Count(const ObjectType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

 protected:
  ExecuteResult AddObjectImp(
      const ObjectType& key, ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override;

  ExecuteResult AddObject(
      ObjectType&& managed_object,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) override;

  ExecuteResult RemoveObject(const ObjectType& removed_object_key) override;

  ExecuteResult RemoveObject(const ObjectType& removed_object_key,
                             ObjectType& object) override;

  ExecuteResult RemoveObject(std::atomic_uint32_t* use_count_ptr) override;

  ExecuteResult RemoveObject(const ObjectType& removed_object_key,
                             std::uint32_t index) override;

  ExecuteResult GetObject(
      const ObjectType& key,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObject(
      const ObjectType& key, std::uint32_t index,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult RemoveObject(const ObjectType& removed_object_key,
                             std::atomic_uint32_t* use_count_ptr) override;

  ExecuteResult GetObject(
      const ObjectType& key,
      std::vector<ManagedObjectHandle<ObjectType, ObjectType>>& handles)
      const override;

 private:
  std::list<ManagedObjectWrapper<ObjectType>> data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object_key, std::atomic_uint32_t* use_count_ptr) {
  LOG_ERROR("This function should not be used.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    std::atomic_uint32_t* use_count_ptr) {
  if (use_count_ptr == nullptr) {
    return ExecuteResult::INPUT_PARAMETERS_ARE_INCORRECT;
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    if (use_count_ptr == (*iter).GetUseCountPtr()) {
      data_.erase(iter);

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
  std::unique_lock<std::shared_mutex> main_guard{data_mutex_, std::adopt_lock},
      other_guard{other.data_mutex_, std::adopt_lock};

  data_ = std::move(other.data_);

  return *this;
}

template <typename ObjectType, typename Allocator>
ManagedObjectList<ObjectType, Allocator>::ManagedObjectList(
    ManagedObjectList&& other) noexcept {
  std::lock(other.data_mutex_);
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_, std::adopt_lock};

  data_ = std::move(other.data_);
}

template <typename ObjectType, typename Allocator>
uint32_t ManagedObjectList<ObjectType, Allocator>::Count(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::uint32_t result = 0;
  for (const auto& object : data_) {
    if (object == key) {
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
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key,
    std::vector<ManagedObjectHandle<ObjectType, ObjectType>>& handles) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  for (const auto& object : data_) {
    if (object == key) {
      handles.emplace_back(this, nullptr, object.GetObjectPtr(),
                           object.GetUseCountPtr());
    }
  }

  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key, std::uint32_t index,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  std::uint32_t current_object_index = 0;

  for (const auto& object : data_) {
    if (key == object) {
      if (current_object_index == index) {
        handle = ManagedObjectHandle<ObjectType, ObjectType>{
            this, nullptr, object.GetObjectPtr(), object.GetUseCountPtr()};

        return ExecuteResult::SUCCESS;
      }
      ++current_object_index;
    }
  }

  return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  return GetObject(key, 0, handle);
}

template <typename ObjectType, typename Allocator>
bool ManagedObjectList<ObjectType, Allocator>::Have(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    if ((*iter) == key) {
      return true;
    }
  }

  return false;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object_key, std::uint32_t index) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::uint32_t matched_itrs_count;
  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    if ((*iter) == removed_object_key) {
      if (matched_itrs_count == index) {
        data_.erase(index);
        return ExecuteResult::SUCCESS;
      }
      ++matched_itrs_count;
    }
  }

  return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object_key, ObjectType& object) {
  LOG_ERROR("This function should not be used.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object_key) {
  return RemoveObject(0);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::AddObject(
    ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  for (const auto& object : data_) {
    if (managed_object != object.GetObject()) {
      handle = ManagedObjectHandle<ObjectType, ObjectType>{
          this, nullptr, object.GetObjectPtr(), object.GetUseCountPtr()};
      return ExecuteResult::SUCCESS;
    }
  }

  data_.emplace_front(std::move(managed_object));
  handle = ManagedObjectHandle<ObjectType, ObjectType>{
      this, nullptr, data_.front().GetObjectPtr(),
      data_.front().GetUseCountPtr()};
  return ExecuteResult::SUCCESS;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::AddObjectImp(
    const ObjectType& key, ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  LOG_ERROR("This function should not be used.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
size_t ManagedObjectList<ObjectType, Allocator>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
