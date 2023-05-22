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

namespace MM {
namespace Manager {
template <typename KeyType, typename ManagedType>
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

template <typename KeyType, typename ValueType>
class ManagedObjectTableBase : virtual public MM::MMObject {
  friend class ManagedObjectHandler<KeyType, ValueType>;

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

 protected:
  virtual ExecuteResult AddObjectImp(
      const KeyType& key, ValueType&& managed_object,
      ManagedObjectHandler<KeyType, ValueType>& handle);

  virtual ExecuteResult AddObjectImp(
      ValueType&& managed_object,
      ManagedObjectHandler<KeyType, ValueType>& handle);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                        ValueType& object);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                        std::atomic_uint32_t* use_count_ptr);

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key,
      ManagedObjectHandler<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
      ManagedObjectHandler<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key, const ValueType& object,
      ManagedObjectHandler<KeyType, ValueType>& handle) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key,
      std::vector<ManagedObjectHandler<KeyType, ValueType>>& handles) const;

  virtual std::uint32_t GetUseCountImp(const KeyType& key) const;

  virtual std::uint32_t GetUseCountImp(
      const KeyType& key, const std::atomic_uint32_t* use_count_ptr) const;

  virtual std::uint32_t GetUseCountImp(const KeyType& key,
                                       const ValueType& object) const;

  virtual void GetUseCountImp(const KeyType& key,
                              std::vector<std::uint32_t>& use_counts) const;

 protected:
  // If it is virtual inheritance, the update of the value will be handed over
  // to the subclass itself.
  std::unique_ptr<std::atomic<ManagedObjectTableBase*>> this_ptr_ptr_{nullptr};
};

template <typename KeyType, typename ValueType>
void ManagedObjectTableBase<KeyType, ValueType>::GetUseCountImp(
    const KeyType& key, std::vector<std::uint32_t>& use_counts) const {}

template <typename KeyType, typename ValueType>
std::uint32_t ManagedObjectTableBase<KeyType, ValueType>::GetUseCountImp(
    const KeyType& key, const ValueType& object) const {
  return 0;
}

template <typename KeyType, typename ValueType>
std::uint32_t ManagedObjectTableBase<KeyType, ValueType>::GetUseCountImp(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr) const {
  return 0;
}

template <typename KeyType, typename ValueType>
std::uint32_t ManagedObjectTableBase<KeyType, ValueType>::GetUseCountImp(
    const KeyType& key) const {
  return 0;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObjectImp(
    const KeyType& key, const ValueType& object,
    ManagedObjectHandler<KeyType, ValueType>& handle) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
MM::ExecuteResult ManagedObjectTableBase<KeyType, ValueType>::GetObjectImp(
    const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
    ManagedObjectHandler<KeyType, ValueType>& handle) const {
  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType>
std::atomic<ManagedObjectTableBase<KeyType, ValueType>*>*
ManagedObjectTableBase<KeyType, ValueType>::GetThisPtrPtr() const {
  return this_ptr_ptr_.get();
}

template <typename KeyType, typename ValueType>
ManagedObjectTableBase<KeyType, ValueType>::ManagedObjectTableBase(
    ManagedObjectTableBase&& other) noexcept {
  this_ptr_ptr_ = std::move(other.this_ptr_ptr_);
  *this_ptr_ptr_ = this;
}

template <typename KeyType, typename ValueType>
ManagedObjectTableBase<KeyType, ValueType>::ManagedObjectTableBase(
    ManagedObjectTableBase* this_ptr)
    : this_ptr_ptr_(std::make_unique<
                    std::atomic<ManagedObjectTableBase<KeyType, ValueType>*>>(
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
    ManagedObjectHandler<KeyType, ValueType>& handle) {
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
    std::vector<ManagedObjectHandler<KeyType, ValueType>>& handles) const {
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
    const KeyType& key,
    ManagedObjectHandler<KeyType, ValueType>& handle) const {
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
    ManagedObjectHandler<KeyType, ValueType>& handle) {
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
class ManagedObjectHandler {
  friend class ManagedObjectTableBase<KeyType, ManagedType>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler();
  ManagedObjectHandler(
      std::atomic<ManagedObjectTableBase<KeyType, ManagedType>*>* object_table,
      KeyType* key, ManagedType* managed_object,
      std::atomic_uint32_t* use_count);
  ManagedObjectHandler(const ManagedObjectHandler& other);
  ManagedObjectHandler(ManagedObjectHandler&& other) noexcept;
  ManagedObjectHandler& operator=(const ManagedObjectHandler& other);
  ManagedObjectHandler& operator=(ManagedObjectHandler&& other) noexcept;

 public:
  ManagedType& GetObject();

  const ManagedType& GetObject() const;

  ManagedType* GetObjectPtr();

  const ManagedType* GetObjectPtr() const;

  std::atomic_uint32_t* GetUseCountPtr();

  const std::atomic_uint32_t* GetUseCountPtr() const;

  std::uint32_t GetUseCount() const;

  const KeyType* GetKeyPtr() const;

  bool IsValid() const;

  void Release();

 private:
  void TestAndDestruction();

 private:
  std::atomic<ManagedObjectTableBase<KeyType, ManagedType>*>* object_table_{
      nullptr};
  KeyType* key_{nullptr};
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

template <typename KeyType, typename ManagedType>
void ManagedObjectHandler<KeyType, ManagedType>::Release() {
  if (!IsValid()) {
    return;
  }

  TestAndDestruction();
  object_table_ = nullptr;
  key_ = nullptr;
  managed_object_ = nullptr;
  use_count_ = nullptr;
}

template <typename KeyType, typename ManagedType>
const std::atomic_uint32_t*
ManagedObjectHandler<KeyType, ManagedType>::GetUseCountPtr() const {
  return use_count_;
}

template <typename KeyType, typename ManagedType>
std::atomic_uint32_t*
ManagedObjectHandler<KeyType, ManagedType>::GetUseCountPtr() {
  return use_count_;
}

template <typename KeyType, typename ManagedType>
const ManagedType* ManagedObjectHandler<KeyType, ManagedType>::GetObjectPtr()
    const {
  return managed_object_;
}

template <typename KeyType, typename ManagedType>
ManagedType* ManagedObjectHandler<KeyType, ManagedType>::GetObjectPtr() {
  return managed_object_;
}

template <typename KeyType, typename ManagedType>
const KeyType* ManagedObjectHandler<KeyType, ManagedType>::GetKeyPtr() const {
  return key_;
}

template <typename KeyType, typename ManagedType>
bool ManagedObjectHandler<KeyType, ManagedType>::IsValid() const {
  return use_count_ != nullptr; /* && object_table != nullptr
                                 * && key != nullptr
                                 * && managed_object != nullptr*/
}

template <typename KeyType, typename ManagedType>
void ManagedObjectHandler<KeyType, ManagedType>::TestAndDestruction() {
  if (use_count_->fetch_sub(1) == 1) {
    ExecuteResult result = ExecuteResult::UNDEFINED_ERROR;
    if ((*(*object_table_)).IsRelationshipContainer()) {
      if ((*(*object_table_)).IsMultiContainer()) {
        do {
          result = (*(*object_table_)).RemoveObjectImp(*key_, use_count_);
          if (result == ExecuteResult::SUCCESS) {
            break;
          }
          if (result == ExecuteResult::CUSTOM_ERROR) {
            continue;
          }
          LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
        } while (true);
      } else {
        do {
          result = (*(*object_table_)).RemoveObjectImp(*key_);
          if (result == ExecuteResult::SUCCESS) {
            break;
          }
          if (result == ExecuteResult::CUSTOM_ERROR) {
            continue;
          }
          LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
        } while (true);
      }
      return;
    }

    if ((*(*object_table_)).IsMultiContainer()) {
      do {
        result =
            (*(*object_table_)).RemoveObjectImp(*managed_object_, use_count_);
        if (result == ExecuteResult::SUCCESS) {
          break;
        }
        if (result == ExecuteResult::CUSTOM_ERROR) {
          continue;
        }
        LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
      } while (true);
    } else {
      do {
        result = (*(*object_table_)).RemoveObjectImp(*managed_object_);
        if (result == ExecuteResult::SUCCESS) {
          break;
        }
        if (result == ExecuteResult::CUSTOM_ERROR) {
          continue;
        }
        LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
      } while (true);
    }
  }
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandler<KeyType, ManagedType>::~ManagedObjectHandler() {
  if (use_count_ != nullptr) {
    TestAndDestruction();
  }
}

template <typename KeyType, typename ManagedType>
std::uint32_t ManagedObjectHandler<KeyType, ManagedType>::GetUseCount() const {
  assert(IsValid());
  return *use_count_;
}

template <typename KeyType, typename ManagedType>
const ManagedType& ManagedObjectHandler<KeyType, ManagedType>::GetObject()
    const {
  assert(IsValid());
  return *managed_object_;
}

template <typename KeyType, typename ManagedType>
ManagedType& ManagedObjectHandler<KeyType, ManagedType>::GetObject() {
  assert(IsValid());
  return *managed_object_;
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandler<KeyType, ManagedType>::ManagedObjectHandler(
    std::atomic<ManagedObjectTableBase<KeyType, ManagedType>*>* object_table,
    KeyType* key, ManagedType* managed_object, std::atomic_uint32_t* use_count)
    : object_table_(object_table),
      key_(key),
      managed_object_(managed_object),
      use_count_(use_count) {
  ++(*use_count_);
}

template <typename KeyType, typename ManagedType>
ManagedObjectHandler<KeyType, ManagedType>&
ManagedObjectHandler<KeyType, ManagedType>::operator=(
    ManagedObjectHandler&& other) noexcept {
  if (&other == this) {
    return *this;
  }

  if (IsValid()) {
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
ManagedObjectHandler<KeyType, ManagedType>&
ManagedObjectHandler<KeyType, ManagedType>::operator=(
    const ManagedObjectHandler& other) {
  if (&other == this) {
    return *this;
  }

  if (IsValid()) {
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
ManagedObjectHandler<KeyType, ManagedType>::ManagedObjectHandler(
    ManagedObjectHandler&& other) noexcept {
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
ManagedObjectHandler<KeyType, ManagedType>::ManagedObjectHandler(
    const ManagedObjectHandler& other) {
  ++(*(other.use_count_));
  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;
  use_count_ = other.use_count_;
}

}  // namespace Manager
}  // namespace MM
