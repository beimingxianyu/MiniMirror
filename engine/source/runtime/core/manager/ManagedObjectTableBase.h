#pragma once

#include <array>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/import_other_system.h"
#include "runtime/platform/base/MMObject.h"
#include "runtime/platform/base/error.h"
#include "utils/marco.h"
#include "utils/type_trait.h"

namespace MM {
namespace Manager {
using ListTrait = Utils::NumType0;
using SetTrait = Utils::NumType1;
using MapTrait = Utils::NumType2;
using HashSetTrait = Utils::NumType3;
using HashMapTrait = Utils::NumType4;
using CanMoved = Utils::TrueType;
using CannotMoved = Utils::FalseType;

/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
class ManagedObjectHandler;

template <typename ManagedType>
class ManagedObjectWrapper {
 public:
  ManagedObjectWrapper() = delete;
  ~ManagedObjectWrapper() = default;
  explicit ManagedObjectWrapper(ManagedType&& managed_object);
  ManagedObjectWrapper(const ManagedObjectWrapper& other) = delete;
  ManagedObjectWrapper(ManagedObjectWrapper&& other) noexcept;
  ManagedObjectWrapper& operator=(const ManagedObjectWrapper& other) = delete;
  ManagedObjectWrapper& operator=(ManagedObjectWrapper&& other) noexcept;

 public:
  friend bool operator<(const ManagedObjectWrapper& lhs,
                        const ManagedObjectWrapper& rhs) {
    return *(lhs.managed_object_) < *(rhs.managed_object_);
  }

  friend bool operator<(const ManagedObjectWrapper& lhs,
                        const ManagedType& rhs) {
    return *(lhs.managed_object_) < rhs;
  }

  friend bool operator<(const ManagedType& lhs,
                        const ManagedObjectWrapper& rhs) {
    return lhs < *(rhs.managed_object_);
  }

  friend bool operator==(const ManagedObjectWrapper<ManagedType>& lhs,
                         const ManagedObjectWrapper<ManagedType>& rhs) {
    return lhs.managed_object_ == rhs.managed_object_;
  }

  friend bool operator==(const ManagedObjectWrapper<ManagedType>& lhs,
                         const ManagedType& rhs) {
    return *(lhs.managed_object_) == rhs;
  }

  friend bool operator==(const ManagedType& lhs,
                         const ManagedObjectWrapper<ManagedType>& rhs) {
    return lhs == *(rhs.managed_object_);
  }

 public:
  template <typename Less = std::less<ManagedType>>
  struct LessWrapper {
    bool operator()(const ManagedObjectWrapper<ManagedType>& lhs,
                    const ManagedObjectWrapper<ManagedType>& rhs) const;
  };

  template <typename Equal = std::equal_to<ManagedType>>
  struct EqualWrapper {
    bool operator()(const ManagedObjectWrapper<ManagedType>& lhs,
                    const ManagedObjectWrapper<ManagedType>& rhs) const;
  };

  template <typename Hash = std::hash<ManagedType>>
  struct HashWrapper {
    std::uint64_t operator()(
        const ManagedObjectWrapper<ManagedType>& object_wrapper) const;
  };

 public:
  std::uint32_t GetUseCount() const;

  ManagedType& GetObject();

  const ManagedType& GetObject() const;

  ManagedType* GetObjectPtr();

  const ManagedType* GetObjectPtr() const;

  std::atomic_uint32_t* GetUseCountPtr();

  std::atomic_uint32_t* GetUseCountPtr() const;

 private:
  std::unique_ptr<ManagedType> managed_object_;
  std::unique_ptr<std::atomic_uint32_t> use_count_{nullptr};
};

template <typename ManagedType>
template <typename Hash>
std::uint64_t ManagedObjectWrapper<ManagedType>::HashWrapper<Hash>::operator()(
    const ManagedObjectWrapper<ManagedType>& object_wrapper) const {
  return Hash{}(*(object_wrapper.managed_object_));
}

template <typename ManagedType>
template <typename Equal>
bool ManagedObjectWrapper<ManagedType>::EqualWrapper<Equal>::operator()(
    const ManagedObjectWrapper<ManagedType>& lhs,
    const ManagedObjectWrapper<ManagedType>& rhs) const {
  return Equal{}(*(lhs.managed_object_), *(rhs.managed_object_));
}

template <typename ManagedType>
template <typename Less>
bool ManagedObjectWrapper<ManagedType>::LessWrapper<Less>::operator()(
    const ManagedObjectWrapper<ManagedType>& lhs,
    const ManagedObjectWrapper<ManagedType>& rhs) const {
  return Less{}(*(lhs.managed_object_), *(rhs.managed_object_));
}

template <typename ManagedType>
ManagedObjectWrapper<ManagedType>::ManagedObjectWrapper(
    ManagedObjectWrapper&& other) noexcept
    : managed_object_(std::move(other.managed_object_)),
      use_count_(std::move(other.use_count_)) {}

template <typename ManagedType>
std::atomic_uint32_t* ManagedObjectWrapper<ManagedType>::GetUseCountPtr()
    const {
  return use_count_.get();
}

template <typename ManagedType>
std::atomic_uint32_t* ManagedObjectWrapper<ManagedType>::GetUseCountPtr() {
  return use_count_.get();
}

template <typename ManagedType>
const ManagedType* ManagedObjectWrapper<ManagedType>::GetObjectPtr() const {
  return managed_object_.get();
}

template <typename ManagedType>
ManagedType* ManagedObjectWrapper<ManagedType>::GetObjectPtr() {
  return managed_object_.get();
}

template <typename ManagedType>
ManagedType& ManagedObjectWrapper<ManagedType>::GetObject() {
  return *managed_object_;
}

template <typename ManagedType>
const ManagedType& ManagedObjectWrapper<ManagedType>::GetObject() const {
  return *managed_object_;
}

template <typename ManagedType>
std::uint32_t ManagedObjectWrapper<ManagedType>::GetUseCount() const {
  return *use_count_;
}

template <typename ManagedType>
ManagedObjectWrapper<ManagedType>::ManagedObjectWrapper(
    ManagedType&& managed_object)
    : managed_object_(std::make_unique<ManagedType>(std::move(managed_object))),
      use_count_(std::make_unique<std::atomic_uint32_t>(0)) {}

template <typename ManagedType>
ManagedObjectWrapper<ManagedType>& ManagedObjectWrapper<ManagedType>::operator=(
    ManagedObjectWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  managed_object_ = std::move(other.managed_object_);
  use_count_ = std::move(other.use_count_);

  return *this;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
class ManagedObjectTableBase : virtual public MM::MMObject {
  friend class ManagedObjectHandler<KeyType, ValueType,
                                    RelationshipContainerTrait>;

 public:
  using HandlerType =
      ManagedObjectHandler<KeyType, ValueType, RelationshipContainerTrait>;
  using WrapperType = ManagedObjectWrapper<ValueType>;

 public:
  ManagedObjectTableBase() = default;
  explicit ManagedObjectTableBase(ManagedObjectTableBase* this_ptr);
  virtual ~ManagedObjectTableBase() = default;
  ManagedObjectTableBase(const ManagedObjectTableBase& other) = delete;
  ManagedObjectTableBase(ManagedObjectTableBase&& other) noexcept;
  ManagedObjectTableBase& operator=(const ManagedObjectTableBase& other) =
      delete;
  ManagedObjectTableBase& operator=(ManagedObjectTableBase&& other) noexcept;

 public:
  std::atomic<ManagedObjectTableBase*>* GetThisPtrPtr() const;

  virtual std::size_t GetSize() const;

  virtual bool Have(const KeyType& key) const;

  virtual std::uint32_t Count(const KeyType& key) const;

  virtual bool IsMultiContainer() const;

  virtual bool IsRelationshipContainer() const;

  bool TestMovedWhenAddObject() const;

  bool TestMovedWhenGetObject() const;

  bool TestMoveWhenGetUseCount() const;

 protected:
  virtual ExecuteResult RemoveObjectImp(
      const ValueType& removed_object_key,
      const std::atomic_uint32_t* use_count_ptr, ListTrait trait);

  virtual ExecuteResult RemoveObjectImp(const ValueType& removed_object_key,
                                        SetTrait trait);

  virtual ExecuteResult RemoveObjectImp(
      const ValueType& removed_object_key,
      const std::atomic_uint32_t* use_count_ptr, SetTrait trait);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                        MapTrait trait);

  virtual ExecuteResult RemoveObjectImp(
      const KeyType& removed_object_key,
      const std::atomic_uint32_t* use_count_ptr, MapTrait trait);

  virtual ExecuteResult RemoveObjectImp(const WrapperType* removed_object_ptr,
                                        HashSetTrait);

  virtual ExecuteResult RemoveObjectImp(
      const std::pair<const KeyType, WrapperType>* removed_object_ptr,
      HashMapTrait);

 protected:
  // If it is virtual inheritance, the update of the value will be handed over
  // to the subclass itself.
  std::unique_ptr<std::atomic<ManagedObjectTableBase*>> this_ptr_ptr_{nullptr};
};

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const ValueType& removed_object_key,
                    const std::atomic_uint32_t* use_count_ptr, SetTrait trait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const ValueType& removed_object_key, SetTrait trait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult ManagedObjectTableBase<
    KeyType, ValueType,
    RelationshipContainerTrait>::RemoveObjectImp(const WrapperType*,
                                                 HashSetTrait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const std::pair<const KeyType, WrapperType>*,
                    HashMapTrait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const KeyType& removed_object_key,
                    const std::atomic_uint32_t* use_count_ptr, MapTrait trait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const KeyType& removed_object_key, MapTrait trait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
bool ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    TestMoveWhenGetUseCount() const {
  if (!IsValid()) {
    LOG_WARN("You cannot get use count from the moved container.");
    return false;
  }

  return true;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
bool ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    TestMovedWhenGetObject() const {
  if (!IsValid()) {
    LOG_WARN("You cannot get values from the moved container.");
    return false;
  }

  return true;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
bool ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    TestMovedWhenAddObject() const {
  if (!IsValid()) {
    LOG_WARN("You cannot add values to the moved container.");
    return false;
  }

  return true;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::atomic<
    ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>*>*
ManagedObjectTableBase<KeyType, ValueType,
                       RelationshipContainerTrait>::GetThisPtrPtr() const {
  if (this_ptr_ptr_) {
    return this_ptr_ptr_.get();
  }

  return nullptr;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    ManagedObjectTableBase(ManagedObjectTableBase&& other) noexcept
    : MM::MMObject(std::move(other)) {
  this_ptr_ptr_ = std::move(other.this_ptr_ptr_);
  *this_ptr_ptr_->store(this, std::memory_order_release);
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    ManagedObjectTableBase(ManagedObjectTableBase* this_ptr)
    : MM::MMObject(),
      this_ptr_ptr_(
          std::make_unique<std::atomic<ManagedObjectTableBase<
              KeyType, ValueType, RelationshipContainerTrait>*>>(this_ptr)) {}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const ValueType&, const std::atomic_uint32_t*, ListTrait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::uint32_t
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::Count(
    const KeyType&) const {
  LOG_FATAL("This function should not be called.");

  return 0;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
bool ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    IsRelationshipContainer() const {
  LOG_FATAL("This function should not be called.");

  return false;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>&
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
operator=(ManagedObjectTableBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  MMObject::operator=(std::move(other));
  this_ptr_ptr_ = std::move(other.this_ptr_ptr_);
  this_ptr_ptr_->store(this, std::memory_order_release);

  return *this;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
bool ManagedObjectTableBase<
    KeyType, ValueType, RelationshipContainerTrait>::IsMultiContainer() const {
  LOG_FATAL("This function should not be called.");

  return false;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
bool ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    Have(const KeyType& key) const {
  LOG_FATAL("This function should not be called.");

  return false;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::size_t ManagedObjectTableBase<
    KeyType, ValueType, RelationshipContainerTrait>::GetSize() const {
  LOG_FATAL("This function should not be called.");

  return 0;
}

}  // namespace Manager
}  // namespace MM
