#pragma once

#include <cassert>
#include <list>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/import_other_system.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Allocator = std::allocator<ObjectType>>
class ManagedObjectList
    : public ManagedObjectTableBase<ObjectType, ObjectType> {
  friend class ManagedObjectHandle<ObjectType, ObjectType>;

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

  ExecuteResult AddObject(ObjectType&& managed_object,
                          ManagedObjectHandle<ObjectType, ObjectType>& handle);

  ExecuteResult RemoveObject(const ObjectType& removed_object);

  ExecuteResult RemoveObject(const ObjectType& removed_object,
                             std::atomic_uint32_t* use_count_ptr);

  ExecuteResult RemoveObject(const ObjectType& removed_object,
                             std::uint32_t index);

  ExecuteResult GetObject(
      const ObjectType& key,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const;

  ExecuteResult GetObject(
      const ObjectType& key, std::uint32_t index,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const;

  ExecuteResult GetObject(
      const ObjectType& key,
      std::vector<ManagedObjectHandle<ObjectType, ObjectType>>& handles) const;

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
                                std::uint32_t index) override;

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                std::atomic_uint32_t* use_count_ptr) override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key, std::uint32_t index,
      ManagedObjectHandle<ObjectType, ObjectType>& handle) const override;

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      std::vector<ManagedObjectHandle<ObjectType, ObjectType>>& handles)
      const override;

 private:
  std::list<ManagedObjectWrapper<ObjectType>> data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key,
    std::vector<ManagedObjectHandle<ObjectType, ObjectType>>& handles) const {
  return GetObjectImp(key, handles);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key, std::uint32_t index,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(key, index, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObject(
    const ObjectType& key,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(key, handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object, std::uint32_t index) {
  return RemoveObjectImp(removed_object, index);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object, std::atomic_uint32_t* use_count_ptr) {
  return RemoveObjectImp(removed_object, use_count_ptr);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObject(
    const ObjectType& removed_object) {
  return RemoveObjectImp(removed_object);
}
template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::AddObject(
    ObjectType&& managed_object,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) {
  return AddObjectImp(std::move(managed_object), handle);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, std::atomic_uint32_t* use_count_ptr) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  for (auto& iter = data_.begin(); iter != data_.end(); ++iter) {
    if (iter->GetUseCountPtr() == use_count_ptr) {
      if (iter->GetUseCount() == 0) {
        data_.erase(iter);
      }

      return ExecuteResult::SUCCESS;
    }
  }

  return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
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
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObjectImp(
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
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObjectImp(
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
ExecuteResult ManagedObjectList<ObjectType, Allocator>::GetObjectImp(
    const ObjectType& key,
    ManagedObjectHandle<ObjectType, ObjectType>& handle) const {
  return GetObjectImp(key, 0, handle);
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
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, std::uint32_t index) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::uint32_t matched_itrs_count;
  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    if ((*iter) == removed_object_key) {
      if (matched_itrs_count == index) {
        if (iter->GetUseCount() == 0) {
          data_.erase(index);
        }

        return ExecuteResult::SUCCESS;
      }
      ++matched_itrs_count;
    }
  }

  return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, ObjectType& object) {
  LOG_ERROR("This function should not be used.");
  assert(false);

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key) {
  return RemoveObjectImp(removed_object_key, 0);
}

template <typename ObjectType, typename Allocator>
ExecuteResult ManagedObjectList<ObjectType, Allocator>::AddObjectImp(
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
