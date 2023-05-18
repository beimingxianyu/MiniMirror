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
  template <typename KeyType, typename ValueType>
  friend class ManagedObjectTableBase;

 public:
  ManagedObjectWrapper() = delete;
  ~ManagedObjectWrapper();
  explicit ManagedObjectWrapper(ManagedType&& managed_object);
  ManagedObjectWrapper(const ManagedObjectWrapper& other) = delete;
  ManagedObjectWrapper(ManagedObjectWrapper&& other) noexcept = default;
  ManagedObjectWrapper& operator=(const ManagedObjectWrapper& other) = delete;
  ManagedObjectWrapper& operator=(ManagedObjectWrapper&& other) noexcept;

 public:
  std::uint32_t GetUseCount() const;

  ManagedType& GetObject() const;

 private:
  ManagedType managed_object_;
  std::atomic_uint32_t* use_count_{nullptr};
};

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedType& ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetObject() const {
  return managed_object_;
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedObjectWrapper<ManagedType,
                     IsDeriveFromManagedObjectBase>::~ManagedObjectWrapper() {
  delete use_count_;
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
std::uint32_t ManagedObjectWrapper<
    ManagedType, IsDeriveFromManagedObjectBase>::GetUseCount() const {
  return *use_count_;
}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>::
    ManagedObjectWrapper(ManagedType&& managed_object)
    : managed_object_(std::move(managed_object)),
      use_count_(new std::atomic_uint32_t(0)) {}

template <typename ManagedType, typename IsDeriveFromManagedObjectBase>
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>&
ManagedObjectWrapper<ManagedType, IsDeriveFromManagedObjectBase>::operator=(
    ManagedObjectWrapper&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  managed_object_ = std::move(other.managed_object_);
  use_count_ = other.use_count_;

  other.use_count_ = nullptr;

  return *this;
}

template <typename KeyType, typename ValueType>
class ManagedObjectTableBase : virtual public MM::MMObject {
 public:
  ManagedObjectTableBase() = default;
  virtual ~ManagedObjectTableBase() = default;
  ManagedObjectTableBase(const ManagedObjectTableBase& other) = delete;
  ManagedObjectTableBase(ManagedObjectTableBase&& other) noexcept = default;
  ManagedObjectTableBase& operator=(const ManagedObjectTableBase& other) =
      delete;
  ManagedObjectTableBase& operator=(ManagedObjectTableBase&& other) noexcept;

 public:
  virtual std::size_t GetSize() const;

  virtual ExecuteResult AddObject(
      const KeyType& key, ValueType&& managed_object,
      ManagedObjectHandle<KeyType, ValueType>& handle);

  virtual ExecuteResult AddObject(
      ValueType&& managed_object,
      ManagedObjectHandle<KeyType, ValueType>& handle);

  virtual ExecuteResult RemoveObject(const KeyType& removed_object_key);

  virtual ExecuteResult RemoveObject(const KeyType& removed_object_key,
                                     ValueType& object);

  virtual ExecuteResult RemoveObject(const KeyType& removed_object_key,
                                     std::uint32_t index);

  virtual bool Have(const KeyType& key) const;

  virtual MM::ExecuteResult GetObject(
      const KeyType& ket,
      ManagedObjectHandle<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObject(
      const KeyType& removed_object_key, std::uint32_t index,
      ManagedObjectHandle<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObject(
      const KeyType& removed_object_key,
      std::vector<ManagedObjectHandle<KeyType, ValueType>>& handles) const;

  virtual bool IsMutliContainer() const;

  virtual bool IsRelationshipContainer() const;
};

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::AddObject(
    ValueType&& managed_object,
    ManagedObjectHandle<KeyType, ValueType>& handle) {
  return ExecuteResult::SUCCESS;
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

  return *this;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObject(
    const KeyType& removed_object_key,
    std::vector<ManagedObjectHandle<KeyType, ValueType>>& handles) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObject(
    const KeyType& removed_object_key, std::uint32_t index,
    ManagedObjectHandle<KeyType, ValueType>& handle) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::RemoveObject(
    const KeyType& removed_object_key, std::uint32_t index) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::RemoveObject(
    const KeyType& removed_object_key, ValueType& object) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
bool ManagedObjectTableBase<KeyType, ValueType>::IsMutliContainer() const {
  return false;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObject(
    const KeyType& ket, ManagedObjectHandle<KeyType, ValueType>& handle) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
bool ManagedObjectTableBase<KeyType, ValueType>::Have(
    const KeyType& key) const {
  return false;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::RemoveObject(
    const KeyType& removed_object_key) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::AddObject(
    const KeyType& key, ValueType&& managed_object,
    ManagedObjectHandle<KeyType, ValueType>& handle) {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
std::size_t ManagedObjectTableBase<KeyType, ValueType>::GetSize() const {
  return 0;
}

template <typename KeyType, typename ManagedType>
class ManagedObjectHandle {
  friend class ManagedObjectTableBase<KeyType, ManagedType>;

 public:
  ManagedObjectHandle() = default;
  ~ManagedObjectHandle();
  ManagedObjectHandle(const ManagedObjectHandle& other);
  ManagedObjectHandle(ManagedObjectHandle&& other) noexcept;
  ManagedObjectHandle& operator=(const ManagedObjectHandle& other);
  ManagedObjectHandle& operator=(ManagedObjectHandle&& other) noexcept;

 public:
  ManagedType& GetObject();

  const ManagedType& GetObject() const;

  std::uint32_t GetUseCount() const;

  const KeyType& GetKey() const;

 private:
  ManagedObjectHandle(
      ManagedObjectTableBase<KeyType, ManagedType>* object_table,
      const KeyType* key, ManagedType* managed_object,
      std::atomic_uint32_t* use_count);

 private:
  ManagedObjectTableBase<KeyType, ManagedType>* object_table_{nullptr};
  const KeyType* key_{nullptr};
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

template <typename KeyType, typename ManagedType>
ManagedObjectHandle<KeyType, ManagedType>::~ManagedObjectHandle() {
  --(*use_count_);
  if (object_table_->IsMutliContainer()) {
    object_table_->RemoveObject(*key_, *managed_object_);
  } else if (object_table_->IsRelationshipContainer()) {
    object_table_->RemoveObject(*managed_object_);
  } else {
    object_table_->RemoveObject(*key_);
  }
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
    ManagedObjectTableBase<KeyType, ManagedType>* object_table,
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

  use_count_ = other.use_count_;
  other.use_count_ = 0;
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

  ++(*other.use_count_);

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
  ++(use_count_);
  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;
  use_count_ = other.use_count_;
}

}  // namespace Manager
}  // namespace MM
