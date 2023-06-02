#pragma once

#include "runtime/core/manager/ManagedObjectTableBase.h"

namespace MM {
namespace Manager {
/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename ManagedType>
class ManagedObjectHandler<ManagedType, ManagedType, NoKeyTrait> {
 public:
  using RelationshipContainerTrait = NoKeyTrait;
  using ThisType = ManagedObjectHandler<ManagedType, ManagedType,
                                        RelationshipContainerTrait>;
  using DataTableType = ManagedObjectTableBase<ManagedType, ManagedType,
                                               RelationshipContainerTrait>;
  friend class ManagedObjectTableBase<ManagedType, ManagedType,
                                      RelationshipContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(std::atomic<DataTableType*>* object_table,
                       ManagedType* managed_object,
                       std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        managed_object_(managed_object),
        use_count_(use_count) {
    ++(*use_count);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    ++(*use_count_);
  }
  ManagedObjectHandler(ManagedObjectHandler&& other) noexcept
      : object_table_(other.object_table_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    other.object_table_ = nullptr;
    other.managed_object_ = nullptr;
    other.use_count_ = nullptr;
  }
  ManagedObjectHandler& operator=(const ManagedObjectHandler& other) {
    if (&other == this) {
      return *this;
    }

    if (IsValid()) {
      Release();
    }

    ++(*(other.use_count_));

    object_table_ = other.object_table_;
    managed_object_ = other.managed_object_;
    use_count_ = other.use_count_;

    return *this;
  }
  ManagedObjectHandler& operator=(ManagedObjectHandler&& other) noexcept {
    if (&other == this) {
      return *this;
    }

    if (IsValid()) {
      Release();
    }

    use_count_ = other.use_count_;
    other.use_count_ = nullptr;

    object_table_ = other.object_table_;
    managed_object_ = other.managed_object_;

    other.object_table_ = nullptr;
    other.managed_object_ = nullptr;

    return *this;
  }

 public:
  ManagedType& GetObject() { return *managed_object_; }

  const ManagedType& GetObject() const { return *managed_object_; }

  ManagedType* GetObjectPtr() { return managed_object_; }

  const ManagedType* GetObjectPtr() const { return managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() const { return use_count_; }

  std::uint32_t GetUseCount() const { return *use_count_; }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1) == 1) {
      if ((*(*object_table_)).IsMultiContainer()) {
        ReleaseMulti();
      } else {
        ReleaseNoMulti();
      }
    }

    object_table_ = nullptr;
    managed_object_ = nullptr;
    use_count_ = nullptr;
  }

 private:
  void ReleaseMulti() {
    ExecuteResult result = ExecuteResult::UNDEFINED_ERROR;

    do {
      result = (*(*object_table_))
                   .RemoveObjectImp(*managed_object_, use_count_,
                                    RelationshipContainerTrait());
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

  void ReleaseNoMulti() {
    ExecuteResult result = ExecuteResult::SUCCESS;
    do {
      result =
          (*(*object_table_))
              .RemoveObjectImp(*managed_object_, RelationshipContainerTrait());
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

 private:
  std::atomic<DataTableType*>* object_table_{nullptr};
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename KeyType, typename ManagedType>
class ManagedObjectHandler<KeyType, ManagedType, NodeKeyTrait> {
 public:
  using RelationshipContainerTrait = NodeKeyTrait;
  using ThisType =
      ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>;
  using DataTableType =
      ManagedObjectTableBase<KeyType, ManagedType, RelationshipContainerTrait>;
  friend class ManagedObjectTableBase<KeyType, ManagedType,
                                      RelationshipContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(
      std::atomic<ManagedObjectTableBase<
          KeyType, ManagedType, RelationshipContainerTrait>*>* object_table,
      const KeyType* key, ManagedType* managed_object,
      std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        key_(key),
        managed_object_(managed_object),
        use_count_(use_count) {
    ++(*use_count);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        key_(other.key_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    ++(*use_count_);
  }
  ManagedObjectHandler(ManagedObjectHandler&& other) noexcept
      : object_table_(other.object_table_),
        key_(other.key_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    other.object_table_ = nullptr;
    other.key_ = nullptr;
    other.managed_object_ = nullptr;
    other.use_count_ = nullptr;
  }
  ManagedObjectHandler& operator=(const ManagedObjectHandler& other) {
    if (&other == this) {
      return *this;
    }

    if (IsValid()) {
      Release();
    }

    ++(*(other.use_count_));

    object_table_ = other.object_table_;
    key_ = other.key_;
    managed_object_ = other.managed_object_;
    use_count_ = other.use_count_;

    return *this;
  }
  ManagedObjectHandler& operator=(ManagedObjectHandler&& other) noexcept {
    if (&other == this) {
      return *this;
    }

    if (IsValid()) {
      Release();
    }

    use_count_ = other.use_count_;
    other.use_count_ = nullptr;

    object_table_ = other.object_table_;
    key_ = other.key_;
    managed_object_ = other.managed_object_;

    other.object_table_ = nullptr;
    other.key_ = nullptr;
    other.managed_object_ = nullptr;

    return *this;
  }

 public:
  ManagedType& GetObject() { return *managed_object_; }

  const ManagedType& GetObject() const { return *managed_object_; }

  ManagedType* GetObjectPtr() { return managed_object_; }

  const ManagedType* GetObjectPtr() const { return managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() { return use_count_; }

  std::uint32_t GetUseCount() const { return *use_count_; }

  const KeyType* GetKeyPtr() const { return key_; }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1) == 1) {
      if ((*(*object_table_)).IsMultiContainer()) {
        ReleaseMulti();
      } else {
        ReleaseNoMulti();
      }
    }

    object_table_ = nullptr;
    key_ = nullptr;
    managed_object_ = nullptr;
    use_count_ = nullptr;
  }

 private:
  void ReleaseMulti() {
    ExecuteResult result = ExecuteResult::UNDEFINED_ERROR;

    do {
      result =
          (*(*object_table_))
              .RemoveObjectImp(*key_, use_count_, RelationshipContainerTrait());
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

  void ReleaseNoMulti() {
    ExecuteResult result = ExecuteResult::SUCCESS;
    do {
      result = (*(*object_table_))
                   .RemoveObjectImp(*key_, RelationshipContainerTrait());
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

 private:
  std::atomic<ManagedObjectTableBase<
      KeyType, ManagedType, RelationshipContainerTrait>*>* object_table_{
      nullptr};
  const KeyType* key_{nullptr};
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename KeyType, typename ManagedType>
class ManagedObjectHandler<KeyType, ManagedType, HashKeyTrait> {
 public:
  using RelationshipContainerTrait = HashKeyTrait;
  using ThisType =
      ManagedObjectHandler<KeyType, ManagedType, RelationshipContainerTrait>;
  using DataTableType =
      ManagedObjectTableBase<KeyType, ManagedType, RelationshipContainerTrait>;
  friend class ManagedObjectTableBase<KeyType, ManagedType,
                                      RelationshipContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(
      std::atomic<ManagedObjectTableBase<
          KeyType, ManagedType, RelationshipContainerTrait>*>* object_table,
      const KeyType& key, ManagedType* managed_object,
      std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        key_(key),
        managed_object_(managed_object),
        use_count_(use_count) {
    ++(*use_count);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        key_(other.key_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    ++(*use_count_);
  }
  ManagedObjectHandler(ManagedObjectHandler&& other) noexcept
      : object_table_(other.object_table_),
        key_(other.key_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    other.object_table_ = nullptr;
    other.managed_object_ = nullptr;
    other.use_count_ = nullptr;
  }
  ManagedObjectHandler& operator=(const ManagedObjectHandler& other) {
    if (&other == this) {
      return *this;
    }

    if (IsValid()) {
      Release();
    }

    ++(*(other.use_count_));

    object_table_ = other.object_table_;
    key_ = other.key_;
    managed_object_ = other.managed_object_;
    use_count_ = other.use_count_;

    return *this;
  }
  ManagedObjectHandler& operator=(ManagedObjectHandler&& other) noexcept {
    if (&other == this) {
      return *this;
    }

    if (IsValid()) {
      Release();
    }

    use_count_ = other.use_count_;
    other.use_count_ = nullptr;

    object_table_ = other.object_table_;
    key_ = other.key_;
    managed_object_ = other.managed_object_;

    other.object_table_ = nullptr;
    other.managed_object_ = nullptr;

    return *this;
  }

 public:
  ManagedType& GetObject() { return *managed_object_; }

  const ManagedType& GetObject() const { return *managed_object_; }

  ManagedType* GetObjectPtr() { return managed_object_; }

  const ManagedType* GetObjectPtr() const { return managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() const { return use_count_; }

  std::uint32_t GetUseCount() const { return *use_count_; }

  const KeyType& GetKey() const { return key_; }

  const KeyType* GetKeyPtr() const { return &key_; }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1) == 1) {
      if ((*(*object_table_)).IsMultiContainer()) {
        ReleaseMulti();
      } else {
        ReleaseNoMulti();
      }
    }

    object_table_ = nullptr;
    managed_object_ = nullptr;
    use_count_ = nullptr;
  }

 private:
  void ReleaseMulti() {
    ExecuteResult result = ExecuteResult::UNDEFINED_ERROR;

    do {
      result =
          (*(*object_table_))
              .RemoveObjectImp(key_, use_count_, RelationshipContainerTrait());
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

  void ReleaseNoMulti() {
    ExecuteResult result = ExecuteResult::SUCCESS;
    do {
      result = (*(*object_table_))
                   .RemoveObjectImp(key_, RelationshipContainerTrait());
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

 private:
  std::atomic<ManagedObjectTableBase<
      KeyType, ManagedType, RelationshipContainerTrait>*>* object_table_{
      nullptr};
  KeyType key_;
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

}  // namespace Manager
}  // namespace MM