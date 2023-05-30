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

struct KeyTrait {};
struct ValueTrait {};

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
  virtual ExecuteResult AddObjectImp(const KeyType& key,
                                     ValueType&& managed_object,
                                     HandlerType& handler);

  virtual ExecuteResult AddObjectImp(ValueType&& managed_object,
                                     HandlerType& handler);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                        KeyTrait trait);

  virtual ExecuteResult RemoveObjectImp(const KeyType& removed_object_key,
                                        std::atomic_uint32_t* use_count_ptr,
                                        KeyTrait trait);

  virtual ExecuteResult RemoveObjectImp(const ValueType& removed_object_key,
                                        ValueTrait trait);

  virtual ExecuteResult RemoveObjectImp(const ValueType& removed_object_key,
                                        std::atomic_uint32_t* use_count_ptr,
                                        ValueTrait trait);

  virtual MM::ExecuteResult GetObjectImp(const KeyType& key,
                                         HandlerType& handler) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
      HandlerType& handler) const;

  virtual MM::ExecuteResult GetObjectImp(const KeyType& key,
                                         const ValueType& object,
                                         HandlerType& handler) const;

  virtual MM::ExecuteResult GetObjectImp(
      const KeyType& key, std::vector<HandlerType>& handlers) const;

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

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const KeyType& removed_object_key,
                    std::atomic_uint32_t* use_count_ptr, KeyTrait trait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const KeyType& removed_object_key, KeyTrait trait) {
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
void ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    GetUseCountImp(const KeyType& key,
                   std::vector<std::uint32_t>& use_counts) const {
  LOG_FATAL("This function should not be called.");
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::uint32_t ManagedObjectTableBase<
    KeyType, ValueType,
    RelationshipContainerTrait>::GetUseCountImp(const KeyType& key,
                                                const ValueType& object) const {
  LOG_FATAL("This function should not be called.");

  return 0;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::uint32_t
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    GetUseCountImp(const KeyType& key,
                   const std::atomic_uint32_t* use_count_ptr) const {
  LOG_FATAL("This function should not be called.");

  return 0;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::uint32_t ManagedObjectTableBase<
    KeyType, ValueType,
    RelationshipContainerTrait>::GetUseCountImp(const KeyType& key) const {
  LOG_FATAL("This function should not be called.");

  return 0;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
MM::ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    GetObjectImp(
        const KeyType& key, const ValueType& object,
        ManagedObjectHandler<KeyType, ValueType, RelationshipContainerTrait>&
            handler) const {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
MM::ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    GetObjectImp(
        const KeyType& key, const std::atomic_uint32_t* use_count_ptr,
        ManagedObjectHandler<KeyType, ValueType, RelationshipContainerTrait>&
            handler) const {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
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
  *this_ptr_ptr_ = this;
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
    RemoveObjectImp(const ValueType& removed_object_key,
                    std::atomic_uint32_t* use_count_ptr, ValueTrait trait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::uint32_t
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::Count(
    const KeyType& key) const {
  LOG_FATAL("This function should not be called.");

  return 0;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    AddObjectImp(ValueType&& managed_object,
                 ManagedObjectHandler<KeyType, ValueType,
                                      RelationshipContainerTrait>& handler) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
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
  *this_ptr_ptr_ = this;

  return *this;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
MM::ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    GetObjectImp(const KeyType& key,
                 std::vector<ManagedObjectHandler<KeyType, ValueType,
                                                  RelationshipContainerTrait>>&
                     handlers) const {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
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
MM::ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    GetObjectImp(
        const KeyType& key,
        ManagedObjectHandler<KeyType, ValueType, RelationshipContainerTrait>&
            handler) const {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
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
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    RemoveObjectImp(const ValueType& removed_object_key, ValueTrait trait) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
ExecuteResult
ManagedObjectTableBase<KeyType, ValueType, RelationshipContainerTrait>::
    AddObjectImp(const KeyType& key, ValueType&& managed_object,
                 ManagedObjectHandler<KeyType, ValueType,
                                      RelationshipContainerTrait>& handler) {
  LOG_FATAL("This function should not be called.");

  return ExecuteResult::UNDEFINED_ERROR;
}

template <typename KeyType, typename ValueType,
          typename RelationshipContainerTrait>
std::size_t ManagedObjectTableBase<
    KeyType, ValueType, RelationshipContainerTrait>::GetSize() const {
  LOG_FATAL("This function should not be called.");

  return 0;
}

/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
class ManagedObjectHandler {
  friend class ManagedObjectTableBase<KeyType, ManagedType,
                                      RelationshipContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler();
  ManagedObjectHandler(
      std::atomic<ManagedObjectTableBase<
          KeyType, ManagedType, RelationshipContainerTrait>*>* object_table,
      const KeyType* key, ManagedType* managed_object,
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
  std::atomic<ManagedObjectTableBase<
      KeyType, ManagedType, RelationshipContainerTrait>*>* object_table_{
      nullptr};
  const KeyType* key_{nullptr};
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
void ManagedObjectHandler<KeyType, ManagedType,
                          RelationshipContainerTrait>::Release() {
  if (!IsValid()) {
    return;
  }

  TestAndDestruction();
  object_table_ = nullptr;
  key_ = nullptr;
  managed_object_ = nullptr;
  use_count_ = nullptr;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
const std::atomic_uint32_t* ManagedObjectHandler<
    KeyType, ManagedType, RelationshipContainerTrait>::GetUseCountPtr() const {
  return use_count_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
std::atomic_uint32_t* ManagedObjectHandler<
    KeyType, ManagedType, RelationshipContainerTrait>::GetUseCountPtr() {
  return use_count_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
const ManagedType* ManagedObjectHandler<
    KeyType, ManagedType, RelationshipContainerTrait>::GetObjectPtr() const {
  return managed_object_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedType* ManagedObjectHandler<KeyType, ManagedType,
                                  RelationshipContainerTrait>::GetObjectPtr() {
  return managed_object_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
const KeyType* ManagedObjectHandler<
    KeyType, ManagedType, RelationshipContainerTrait>::GetKeyPtr() const {
  return key_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
bool ManagedObjectHandler<KeyType, ManagedType,
                          RelationshipContainerTrait>::IsValid() const {
  return use_count_ != nullptr; /* && object_table != nullptr
                                 * && key != nullptr
                                 * && managed_object != nullptr*/
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
void ManagedObjectHandler<KeyType, ManagedType,
                          RelationshipContainerTrait>::TestAndDestruction() {
  if (use_count_->fetch_sub(1) == 1) {
    ExecuteResult result = ExecuteResult::UNDEFINED_ERROR;
    if ((*(*object_table_)).IsRelationshipContainer()) {
      if ((*(*object_table_)).IsMultiContainer()) {
        do {
          result = (*(*object_table_))
                       .RemoveObjectImp(*key_, use_count_, KeyTrait());
          if (result == ExecuteResult::SUCCESS) {
            return;
          }
          if (result == ExecuteResult::CUSTOM_ERROR) {
            continue;
          }
          LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
          return;
        } while (true);
      } else {
        do {
          result = (*(*object_table_)).RemoveObjectImp(*key_, KeyTrait());
          if (result == ExecuteResult::SUCCESS) {
            return;
          }
          if (result == ExecuteResult::CUSTOM_ERROR) {
            continue;
          }
          LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
          return;
        } while (true);
      }
    }

    if ((*(*object_table_)).IsMultiContainer()) {
      do {
        result =
            (*(*object_table_))
                .RemoveObjectImp(*managed_object_, use_count_, ValueTrait());
        if (result == ExecuteResult::SUCCESS) {
          return;
        }
        if (result == ExecuteResult::CUSTOM_ERROR) {
          continue;
        }
        LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
        return;
      } while (true);
    } else {
      do {
        result =
            (*(*object_table_)).RemoveObjectImp(*managed_object_, ValueTrait());
        if (result == ExecuteResult::SUCCESS) {
          return;
        }
        if (result == ExecuteResult::CUSTOM_ERROR) {
          continue;
        }
        LOG_SYSTEM->CheckResult(result, CODE_LOCATION);
        return;
      } while (true);
    }
  }
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedObjectHandler<KeyType, ManagedType,
                     RelationshipContainerTrait>::~ManagedObjectHandler() {
  if (use_count_ != nullptr) {
    TestAndDestruction();
  }
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
std::uint32_t ManagedObjectHandler<
    KeyType, ManagedType, RelationshipContainerTrait>::GetUseCount() const {
  assert(IsValid());
  return *use_count_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
const ManagedType& ManagedObjectHandler<
    KeyType, ManagedType, RelationshipContainerTrait>::GetObject() const {
  assert(IsValid());
  return *managed_object_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedType& ManagedObjectHandler<KeyType, ManagedType,
                                  RelationshipContainerTrait>::GetObject() {
  assert(IsValid());
  return *managed_object_;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>::
    ManagedObjectHandler(
        std::atomic<ManagedObjectTableBase<
            KeyType, ManagedType, RelationshipContainerTrait>*>* object_table,
        const KeyType* key, ManagedType* managed_object,
        std::atomic_uint32_t* use_count)
    : object_table_(object_table),
      key_(key),
      managed_object_(managed_object),
      use_count_(use_count) {
  ++(*use_count_);
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>&
ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>::
operator=(ManagedObjectHandler&& other) noexcept {
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

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>&
ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>::
operator=(const ManagedObjectHandler& other) {
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

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>::
    ManagedObjectHandler(ManagedObjectHandler&& other) noexcept {
  use_count_ = other.use_count_;
  other.use_count_ = nullptr;

  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;

  other.object_table_ = nullptr;
  other.key_ = nullptr;
  other.managed_object_ = nullptr;
}

template <typename KeyType, typename ManagedType,
          typename RelationshipContainerTrait>
ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>::
    ManagedObjectHandler(const ManagedObjectHandler& other) {
  ++(*(other.use_count_));
  object_table_ = other.object_table_;
  key_ = other.key_;
  managed_object_ = other.managed_object_;
  use_count_ = other.use_count_;
}

}  // namespace Manager
}  // namespace MM
