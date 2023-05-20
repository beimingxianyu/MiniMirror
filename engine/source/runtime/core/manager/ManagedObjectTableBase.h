#pragma once

#include <array>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/platform/base/MMObject.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace Manager {
template <typename KeyType, typename ManagedType>
class ManagedObjectHandle;

template <
    typename ManagedType,
    typename IsDeriveFromManagedObjectBase = typename std::enable_if<
        std::is_base_of<ManagedObjectBase, ManagedType>::value, void>::type>
class ManagedObjectWrapper {
 public:
  ManagedObjectWrapper() = delete;
  ~ManagedObjectWrapper();
  explicit ManagedObjectWrapper(ManagedType&& managed_object);
  ManagedObjectWrapper(const ManagedObjectWrapper& other) = delete;
  ManagedObjectWrapper(ManagedObjectWrapper&& other) noexcept;
  ManagedObjectWrapper& operator=(const ManagedObjectWrapper& other) = delete;
  ManagedObjectWrapper& operator=(ManagedObjectWrapper&& other) noexcept;

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

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>::
    ManagedObjectWrapper(ManagedObjectWrapper&& other) noexcept
    : managed_object_(std::move(other.managed_object_)),
      use_count_(std::move(other.use_count_)) {}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
std::atomic_uint32_t* ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetUseCountPtr() const {
  return use_count_.get();
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
std::atomic_uint32_t* ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetUseCountPtr() {
  return use_count_.get();
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
const ManagedType* ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetObjectPtr() const {
  return managed_object_.get();
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedType* ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetObjectPtr() {
  return managed_object_.get();
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedType&
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>::GetObject() {
  return *managed_object_;
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
const ManagedType& ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetObject() const {
  return *managed_object_;
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedObjectWrapper<ManagedType,
                     IsDeriveFromManagedObjectBase>::~ManagedObjectWrapper() {}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
std::uint32_t ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetUseCount() const {
  return *use_count_;
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>::
    ManagedObjectWrapper(ManagedType&& managed_object)
    : managed_object_(std::make_unique<ManagedType>(std::move(managed_object))),
      use_count_(std::make_unique<std::atomic_uint32_t>(0)) {}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>&
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>::operator=(
    ManagedObjectWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  managed_object_ = std::move(other.managed_object_);
  use_count_ = std::move(other.use_count_);

  return *this;
}

template <typename KeyType, typename ValueType>
class ManagedObjectTableBase : virtual public MM::MMObject {
  friend class ManagedObjectHandle<KeyType, ValueType>;

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
  ManagedObjectTableBase** GetThisPtrPtr() const;

  virtual std::size_t GetSize() const;

  virtual bool Have(const KeyType& key) const;

  virtual std::uint32_t Count(const KeyType& key) const;

  virtual bool IsMultiContainer() const;

  virtual bool IsRelationshipContainer() const;

 protected:
  virtual ExecuteResult AddObjectImp(
      const KeyType& key, ValueType&& managed_object,
      ManagedObjectHandle<KeyType, ValueType>& handle);

  virtual ExecuteResult AddObjectImp(
      ValueType&& managed_object,
      ManagedObjectHandle<KeyType, ValueType>& handle);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                        ValueType& object);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                        std::atomic_uint32_t* use_count_ptr);

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key,
      ManagedObjectHandle<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key, std::atomic_uint32_t* use_count_ptr,
      ManagedObjectHandle<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key, ValueType& object,
      ManagedObjectHandle<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key,
      std::vector<ManagedObjectHandle<KeyType, ValueType>>& handles) const;

 protected:
  // If it is virtual inheritance, the update of the value will be handed over
  // to the subclass itself.
  std::unique_ptr<ManagedObjectTableBase*> this_ptr_ptr_{nullptr};
};

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObjectImp(
    const KeyType& key, ValueType& object,
    ManagedObjectHandle<KeyType, ValueType>& handle) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObjectImp(
    const KeyType& key, std::atomic_uint32_t* use_count_ptr,
    ManagedObjectHandle<KeyType, ValueType>& handle) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
ManagedObjectTableBase<KeyType, ValueType>**
ManagedObjectTableBase<KeyType, ValueType>::GetThisPtrPtr() const {
  return this_ptr_ptr_.get();
}

template <typename KeyType, typename ValueType>
ManagedObjectTableBase<KeyType, ValueType>::ManagedObjectTableBase(
    ManagedObjectTableBase&& other) noexcept
    : this_ptr_ptr_{std::move(other.this_ptr_ptr_)} {
  *this_ptr_ptr_ = this;
}

template <typename KeyType, typename ValueType>
ManagedObjectTableBase<KeyType, ValueType>::ManagedObjectTableBase(
    ManagedObjectTableBase* this_ptr)
    : this_ptr_ptr_(
          std::make_unique<ManagedObjectTableBase<KeyType, ValueType>*>(
              this_ptr)) {}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::RemoveObjectImp(
    const KeyType& removed_object_key, std::atomic_uint32_t* use_count_ptr) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
std::uint32_t ManagedObjectTableBase<KeyType, ValueType>::Count(
    const KeyType& key) const {
  return 0;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::AddObjectImp(
    ValueType&& managed_object,
    ManagedObjectHandle<KeyType, ValueType>& handle) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
bool ManagedObjectTableBase<KeyType, ValueType>::IsRelationshipContainer()
    const {
  return false;
}

template <typename KeyType, typename ValueType>
ManagedObjectTableBase<KeyType, ValueType>&
ManagedObjectTableBase<KeyType, ValueType>::operator=(
    ManagedObjectTableBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  this_ptr_ptr_ = std::move(other.this_ptr_ptr_);
  *this_ptr_ptr_ = this;

  return *this;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObjectImp(
    const KeyType& key,
    std::vector<ManagedObjectHandle<KeyType, ValueType>>& handles) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::RemoveObjectImp(
    const KeyType& removed_object_key, ValueType& object) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
bool ManagedObjectTableBase<KeyType, ValueType>::IsMultiContainer() const {
  return false;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObjectImp(
    const KeyType& key, ManagedObjectHandle<KeyType, ValueType>& handle) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
bool ManagedObjectTableBase<KeyType, ValueType>::Have(
    const KeyType& key) const {
  return false;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::RemoveObjectImp(
    const KeyType& removed_object_key) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::AddObjectImp(
    const KeyType& key, ValueType&& managed_object,
    ManagedObjectHandle<KeyType, ValueType>& handle) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
std::size_t ManagedObjectTableBase<KeyType, ValueType>::GetSize() const {
  return 0;
}

/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename KeyType, typename ManagedType>
class ManagedObjectHandle {
  friend class ManagedObjectTableBase<KeyType, ManagedType>;

 public:
  ManagedObjectHandle() = default;
  ~ManagedObjectHandle();
  ManagedObjectHandle(
      ManagedObjectTableBase<KeyType, ManagedType>** object_table,
      const KeyType* key, ManagedType* managed_object,
      std::atomic_uint32_t* use_count);
  ManagedObjectHandle(const ManagedObjectHandle& other);
  ManagedObjectHandle(ManagedObjectHandle&& other) noexcept;
  ManagedObjectHandle& operator=(const ManagedObjectHandle& other);
  ManagedObjectHandle& operator=(ManagedObjectHandle&& other) noexcept;

 public:
  ManagedType& GetObject();

  const ManagedType& GetObject() const;

  std::uint32_t GetUseCount() const;

  const KeyType& GetKey() const;

  bool IsValid() const;

 private:
  void TestAndDestruction();

 private:
  ManagedObjectTableBase<KeyType, ManagedType>** object_table_{nullptr};
  const KeyType* key_{nullptr};
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

template <typename KeyType, typename ManagedType>
bool ManagedObjectHandle<KeyType, ManagedType>::IsValid() const {
  return use_count_ != nullptr; /* && object_table != nullptr
                                 * && key != nullptr
                                 * && managed_object != nullptr*/
}

template <typename KeyType, typename ManagedType>
void ManagedObjectHandle<KeyType, ManagedType>::TestAndDestruction() {
  if (*use_count_ == 0) {
    if ((*object_table_)->IsRelationshipContainer()) {
      if ((*object_table_)->IsMultiContainer()) {
        (*object_table_)->RemoveObjectImp(*key_, use_count_);
      } else {
        (*object_table_)->RemoveObjectImp(*key_);
      }
      return;
    }

    if ((*object_table_)->IsMultiContainer()) {
      (*object_table_)->RemoveObjectImp(*managed_object_, use_count_);
    } else {
      (*object_table_)->RemoveObjectImp(*managed_object_);
    }
  }
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandle<KeyType, ManagedType>::~ManagedObjectHandle() {
  --(*use_count_);
  TestAndDestruction();
}

template <typename KeyType, typename ManagedType>
const KeyType& ManagedObjectHandle<KeyType, ManagedType>::GetKey() const {
  return *key_;
}

template <typename KeyType, typename ManagedType>
std::uint32_t ManagedObjectHandle<KeyType, ManagedType>::GetUseCount() const {
  return *use_count_;
}

template <typename KeyType, typename ManagedType>
const ManagedType& ManagedObjectHandle<KeyType, ManagedType>::GetObject()
    const {
  return *managed_object_;
}

template <typename KeyType, typename ManagedType>
ManagedType& ManagedObjectHandle<KeyType, ManagedType>::GetObject() {
  return *managed_object_;
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandle<KeyType, ManagedType>::ManagedObjectHandle(
    ManagedObjectTableBase<KeyType, ManagedType>** object_table,
    const KeyType* key, ManagedType* managed_object,
    std::atomic_uint32_t* use_count)
    : object_table_(object_table),
      key_(key),
      managed_object_(managed_object),
      use_count_(use_count) {
  ++(*use_count_);
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandle<KeyType, ManagedType>&
ManagedObjectHandle<KeyType, ManagedType>::operator=(
    ManagedObjectHandle&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  if (IsValid()) {
    --(*use_count_);
    TestAndDestruction();
  }

  use_count_ = other.use_count_;
  other.use_count_ = nullptr;

  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;

  other.object_table_ = nullptr;
  other.managed_object_ = nullptr;
  other.key_ = nullptr;

  return *this;
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandle<KeyType, ManagedType>&
ManagedObjectHandle<KeyType, ManagedType>::operator=(
    const ManagedObjectHandle& other) {
  if (&other == this) {
    return *this;
  }

  if (IsValid()) {
    --(*use_count_);
    TestAndDestruction();
  }

  ++(*(other.use_count_));

  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;
  use_count_ = other.use_count_;

  return *this;
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandle<KeyType, ManagedType>::ManagedObjectHandle(
    ManagedObjectHandle&& other) noexcept {
  use_count_ = other.use_count_;
  other.use_count_ = nullptr;

  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;

  other.object_table_ = nullptr;
  other.key_ = nullptr;
  other.managed_object_ = nullptr;
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandle<KeyType, ManagedType>::ManagedObjectHandle(
    const ManagedObjectHandle& other) {
  ++(*(other.use_count_));
  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;
  use_count_ = other.use_count_;
}

}  // namespace Manager
}  // namespace MM
