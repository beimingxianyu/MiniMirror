#pragma once

#include <cassert>
#include <set>

#include "runtime/core/manager/ManagedObjectHandler.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/pre_header.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Less = std::less<>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>>
class ManagedObjectSet final
    : public ManagedObjectTableBase<ObjectType, ObjectType, SetTrait> {
 public:
  using ContainerTrait = SetTrait;
  using ThisType = ManagedObjectSet<ObjectType, Less, Allocator>;
  using BaseType =
      ManagedObjectTableBase<ObjectType, ObjectType, ContainerTrait>;
  using HandlerType = typename BaseType::HandlerType;
  using WrapperType = typename BaseType::WrapperType;
  using ContainerType =
      std::set<WrapperType,
               typename WrapperType::template LessWrapperObject<Less>,
               Allocator>;

 public:
  ManagedObjectSet();
  ~ManagedObjectSet() override;
  ManagedObjectSet(const ManagedObjectSet& other) = delete;
  ManagedObjectSet(ManagedObjectSet&& other) noexcept;
  ManagedObjectSet& operator=(const ManagedObjectSet& other) = delete;
  ManagedObjectSet& operator=(ManagedObjectSet&& other) noexcept;

 public:
  size_t GetSize() const override;

  bool Have(const ObjectType& key) const override;

  uint32_t GetSize(const ObjectType& key) const override;

  bool IsMultiContainer() const override;

  bool IsRelationshipContainer() const override;

  Result<HandlerType, ErrorResult> AddObject(ObjectType&& managed_object);

  Result<HandlerType, ErrorResult> GetObject(const ObjectType& key) const;

  std::uint32_t GetUseCount(const ObjectType& key) const;

 protected:
  Result<Nil, ErrorResult> RemoveObjectImp(const ObjectType& removed_object_key,
                                ContainerTrait trait) override;

 private:
  ContainerType data_;
  mutable std::shared_mutex data_mutex_;
};

template <typename ObjectType, typename Less = std::less<>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>>
class ManagedObjectMultiSet
    : public ManagedObjectTableBase<ObjectType, ObjectType, ListTrait> {
 public:
  using ContainerTrait = ListTrait;
  using ThisType = ManagedObjectMultiSet<ObjectType, Less, Allocator>;
  using BaseType =
      ManagedObjectTableBase<ObjectType, ObjectType, ContainerTrait>;
  using HandlerType = typename BaseType::HandlerType;
  using WrapperType = typename BaseType::WrapperType;
  using ContainerType =
      std::multiset<WrapperType,
                    typename WrapperType::template LessWrapperObject<Less>,
                    Allocator>;

 public:
  ManagedObjectMultiSet();
  ~ManagedObjectMultiSet() override;
  ManagedObjectMultiSet(const ManagedObjectMultiSet& other) = delete;
  ManagedObjectMultiSet(ManagedObjectMultiSet&& other) noexcept;
  ManagedObjectMultiSet& operator=(const ManagedObjectMultiSet& other) = delete;
  ManagedObjectMultiSet& operator=(ManagedObjectMultiSet&& other) noexcept;

 public:
  size_t GetSize() const override { return data_.size(); }

  bool Have(const ObjectType& key) const override {
    return data_.count(key) > 0;
  }

  uint32_t GetSize(const ObjectType& key) const override {
    return data_.count(key);
  }

  bool IsMultiContainer() const override { return true; }

  bool IsRelationshipContainer() const override { return false; }

  Result<HandlerType, ErrorResult> AddObject(ObjectType&& managed_object);

  Result<HandlerType, ErrorResult> GetObject(const ObjectType& key) const;

  Result<HandlerType, ErrorResult> GetObject(const ObjectType& key,
                          const std::atomic_uint32_t* use_count_ptr) const;

  Result<std::vector<HandlerType>, ErrorResult> GetObject(const ObjectType& key, StaticTrait::GetMultiplyObject) const;

  std::uint32_t GetUseCount(const ObjectType& key) const;

  std::uint32_t GetUseCount(const ObjectType& key,
                            const std::atomic_uint32_t* use_count_ptr) const;

  Result<std::vector<std::uint32_t>, ErrorResult> GetUseCount(const ObjectType& key, StaticTrait::GetMultiplyObject) const;

 protected:
  Result<Nil, ErrorResult> RemoveObjectImp(const ObjectType& removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                ListTrait trait) override;

 private:
  ContainerType data_{};
  mutable std::shared_mutex data_mutex_{};
};

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectMultiSet<ObjectType, Less, Allocator>::~ManagedObjectMultiSet() {
  if (GetSize() != 0) {
    MM_LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectSet<ObjectType, Less, Allocator>::~ManagedObjectSet() {
  if (GetSize() != 0) {
    MM_LOG_ERROR(
        "The container is not empty, and destroying it will result in an "
        "access error.");
  }
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<std::vector<std::uint32_t>, ErrorResult> ManagedObjectMultiSet<ObjectType, Less, Allocator>::GetUseCount(
    const ObjectType& key, StaticTrait::GetMultiplyObject) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
     return Result<std::vector<std::uint32_t>, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first != equal_range.second) {
      return Result<std::vector<std::uint32_t>, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  std::vector<std::uint32_t> use_counts;
  for (typename ContainerType::iterator iter = equal_range.first;
       iter == equal_range.second; ++iter) {
    use_counts.emplace_back(iter->GetUseCount());
  }

  return Result<std::vector<std::uint32_t>, ErrorResult>(st_execute_success, std::move(use_counts));
}

template <typename ObjectType, typename Less, typename Allocator>
std::uint32_t ManagedObjectMultiSet<ObjectType, Less, Allocator>::GetUseCount(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
    return 0;
  }

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

template <typename ObjectType, typename Less, typename Allocator>
std::uint32_t ManagedObjectMultiSet<ObjectType, Less, Allocator>::GetUseCount(
    const ObjectType& key) const {
  if (!ThisType::TestMovedWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
    return 0;
  }

  return equal_range.first->GetUseCount();
}

template <typename ObjectType, typename Less, typename Allocator>
std::uint32_t ManagedObjectSet<ObjectType, Less, Allocator>::GetUseCount(
    const ObjectType& key) const {
  if (!BaseType::TestMovedWhenGetUseCount()) {
    return 0;
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::iterator iter = data_.find(key);
  if (iter != data_.end()) {
    return iter->GetUseCount();
  }

  return 0;
}

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectSet<ObjectType, Less, Allocator>::ManagedObjectSet()
    : ManagedObjectTableBase<ObjectType, ObjectType, ContainerTrait>(this),
      data_(),
      data_mutex_() {}

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectMultiSet<ObjectType, Less, Allocator>::ManagedObjectMultiSet()
    : ManagedObjectTableBase<ObjectType, ObjectType, ContainerTrait>(this),
      data_(),
      data_mutex_() {}

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectMultiSet<ObjectType, Less, Allocator>::ManagedObjectMultiSet(
    ManagedObjectMultiSet&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectMultiSet<ObjectType, Less, Allocator>&
ManagedObjectMultiSet<ObjectType, Less, Allocator>::operator=(
    ManagedObjectMultiSet&& other) noexcept {
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

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<typename ManagedObjectMultiSet<ObjectType, Less, Allocator>::HandlerType, ErrorResult> ManagedObjectMultiSet<ObjectType, Less, Allocator>::AddObject(
    ObjectType&& managed_object) {
  if (!ThisType::TestMovedWhenAddObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};

  typename ContainerType::iterator iter =
      data_.emplace(std::move(managed_object));

  return Result<HandlerType, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(), const_cast<ObjectType*>(iter->GetObjectPtr()),
                                          iter->GetUseCountPtr());
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<Nil, ErrorResult>
ManagedObjectMultiSet<ObjectType, Less, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key,
    const std::atomic_uint32_t* use_count_ptr, ListTrait) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  if (ThisType::this_ptr_ptr_ == nullptr) {
    return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(removed_object_key);
  if (equal_range.first == equal_range.second) {
      return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  for (typename ContainerType::iterator& iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if ((*iter).GetUseCountPtr() == use_count_ptr) {
      if (iter->GetUseCount() == 0) {
        data_.erase(iter);
      }

      return Result<Nil, ErrorResult>(st_execute_success);
    }
  }

    return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<typename ManagedObjectMultiSet<ObjectType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectMultiSet<ObjectType, Less, Allocator>::GetObject(
    const ObjectType& key) const {
  if (!ThisType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

return Result<HandlerType, ErrorResult>(st_execute_success, BaseType ::GetThisPtrPtr(),
                                        const_cast<ObjectType*>(equal_range.first->GetObjectPtr()),
                                        equal_range.first->GetUseCountPtr());
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<typename ManagedObjectMultiSet<ObjectType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectMultiSet<ObjectType, Less, Allocator>::GetObject(
    const ObjectType& key, const std::atomic_uint32_t* use_count_ptr) const {
  if (!ThisType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  bool find = false;
  HandlerType new_handler;

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    if (iter->GetUseCountPtr() == use_count_ptr) {
        return Result<HandlerType, ErrorResult>(st_execute_success, BaseType ::GetThisPtrPtr(),
                                                const_cast<ObjectType*>(iter->GetObjectPtr()),
                                                iter->GetUseCountPtr());
    }
  }


    return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE);
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<std::vector<typename ManagedObjectMultiSet<ObjectType, Less, Allocator>::HandlerType>, ErrorResult>
ManagedObjectMultiSet<ObjectType, Less, Allocator>::GetObject(
    const ObjectType& key, StaticTrait::GetMultiplyObject) const {
  if (!ThisType::TestMovedWhenGetObject()) {
      return Result<std::vector<HandlerType>, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  std::pair<typename ContainerType::iterator, typename ContainerType::iterator>
      equal_range = data_.equal_range(key);
  if (equal_range.first == equal_range.second) {
      return Result<std::vector<HandlerType>, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  std::vector<HandlerType> handlers;
  for (typename ContainerType::iterator iter = equal_range.first;
       iter != equal_range.second; ++iter) {
    handlers.emplace_back(BaseType ::GetThisPtrPtr(),
                          const_cast<ObjectType*>(iter->GetObjectPtr()),
                          iter->GetUseCountPtr());
  }

    return Result<std::vector<HandlerType>, ErrorResult>(st_execute_success, std::move(handlers));
}

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectSet<ObjectType, Less, Allocator>&
ManagedObjectSet<ObjectType, Less, Allocator>::operator=(
    ManagedObjectSet&& other) noexcept {
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

template <typename ObjectType, typename Less, typename Allocator>
ManagedObjectSet<ObjectType, Less, Allocator>::ManagedObjectSet(
    ManagedObjectSet&& other) noexcept {
  std::unique_lock<std::shared_mutex> guard{other.data_mutex_};

  BaseType::operator=(std::move(other));
  data_ = std::move(other.data_);
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<typename ManagedObjectSet<ObjectType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectSet<ObjectType, Less, Allocator>::AddObject(
    ObjectType&& managed_object) {
  if (!BaseType::TestMovedWhenAddObject()) {
    return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::unique_lock<std::shared_mutex> guard{data_mutex_};
  std::pair<typename ContainerType::iterator, bool> insert_result =
      data_.emplace(std::move(managed_object));

  return Result<HandlerType, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(),
                                        const_cast<ObjectType*>(insert_result.first->GetObjectPtr()),
                                        insert_result.first->GetUseCountPtr());
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<Nil, ErrorResult>
ManagedObjectSet<ObjectType, Less, Allocator>::RemoveObjectImp(
    const ObjectType& removed_object_key, ContainerTrait) {
  std::unique_lock<std::shared_mutex> guard{data_mutex_};

  if (ThisType::this_ptr_ptr_ == nullptr) {
     return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::CUSTOM_ERROR);
  }

  auto iter = data_.find(removed_object_key);

  if (iter == data_.end()) {
     return Result<Nil, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  }

  if (iter->GetUseCount() == 0) {
    data_.erase(iter);
  }

 return Result<Nil, ErrorResult>(st_execute_success);
}

template <typename ObjectType, typename Less, typename Allocator>
MM::Result<typename ManagedObjectSet<ObjectType, Less, Allocator>::HandlerType, ErrorResult>
ManagedObjectSet<ObjectType, Less, Allocator>::GetObject(
    const ObjectType& key) const {
  if (!BaseType::TestMovedWhenGetObject()) {
      return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::OBJECT_IS_INVALID);
  }

  std::shared_lock<std::shared_mutex> guard{data_mutex_};

  auto iter = data_.find(key);

  if (iter != data_.end()) {
   return Result<HandlerType, ErrorResult>(st_execute_success, BaseType::GetThisPtrPtr(),
                                           const_cast<ObjectType*>(iter->GetObjectPtr()), iter->GetUseCountPtr());
  }

    return Result<HandlerType, ErrorResult>(st_execute_error, ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
}

template <typename ObjectType, typename Less, typename Allocator>
bool ManagedObjectSet<ObjectType, Less, Allocator>::Have(
    const ObjectType& key) const {
  std::shared_lock<std::shared_mutex> guard{data_mutex_};
  return data_.find(key) != data_.end();
}

template <typename ObjectType, typename Less, typename Allocator>
bool ManagedObjectSet<ObjectType, Less, Allocator>::IsRelationshipContainer()
    const {
  return false;
}

template <typename ObjectType, typename Less, typename Allocator>
bool ManagedObjectSet<ObjectType, Less, Allocator>::IsMultiContainer() const {
  return false;
}

template <typename ObjectType, typename Less, typename Allocator>
uint32_t ManagedObjectSet<ObjectType, Less, Allocator>::GetSize(
    const ObjectType& key) const {
  if (Have(key)) {
    return 1;
  }

  return 0;
}

template <typename ObjectType, typename Less, typename Allocator>
size_t ManagedObjectSet<ObjectType, Less, Allocator>::GetSize() const {
  return data_.size();
}
}  // namespace Manager
}  // namespace MM
