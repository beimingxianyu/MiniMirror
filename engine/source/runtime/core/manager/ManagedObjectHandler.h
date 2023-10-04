#pragma once

#include "runtime/core/log/exception_description.h"
#include "runtime/core/manager/ManagedObjectTableBase.h"

namespace MM {
namespace Manager {
/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename ManagedType>
class ManagedObjectHandler<ManagedType, ManagedType, ListTrait> {
 public:
  using ContainerTrait = ListTrait;
  using ThisType =
      ManagedObjectHandler<ManagedType, ManagedType, ContainerTrait>;
  using DataTableType =
      ManagedObjectTableBase<ManagedType, ManagedType, ContainerTrait>;
  friend class ManagedObjectTableBase<ManagedType, ManagedType, ContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(std::atomic<DataTableType*>* object_table,
                       ManagedType* managed_object,
                       std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        managed_object_(managed_object),
        use_count_(use_count) {
    use_count_->fetch_add(1, std::memory_order_acq_rel);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    if (IsValid()) {
      use_count_->fetch_add(1, std::memory_order_acq_rel);
    }
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

    if (other.IsValid()) {
      other.use_count_->fetch_add(1, std::memory_order_acq_rel);
    }

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

    std::swap(use_count_, other.use_count_);
    std::swap(object_table_, other.object_table_);
    std::swap(managed_object_, other.managed_object_);

    return *this;
  }

 public:
  ManagedType& GetObject() { return *managed_object_; }

  const ManagedType& GetObject() const { return *managed_object_; }

  ManagedType* GetObjectPtr() { return managed_object_; }

  const ManagedType* GetObjectPtr() const { return managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() const { return use_count_; }

  std::uint32_t GetUseCount() const {
    return use_count_->load(std::memory_order_relaxed);
  }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
      do {
        Result<Nil, ErrorResult> result =
            object_table_->load(std::memory_order_acquire)
                ->RemoveObjectImp(*managed_object_, use_count_,
                                  ContainerTrait());
        if (result.IsSuccess()) {
          result.IgnoreException();
          break;
        }
        if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
          result.IgnoreException();
          continue;
        }

        result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));

        break;
      } while (true);
    }

    object_table_ = nullptr;
    managed_object_ = nullptr;
    use_count_ = nullptr;
  }

  template <typename DriveType>
  ManagedObjectHandler<DriveType, DriveType, ListTrait> DynamicCast() {
    static_assert(std::is_base_of_v<ManagedType, DriveType>);

    return ManagedObjectHandler<DriveType, DriveType, ListTrait>{
        object_table_, dynamic_cast<DriveType*>(managed_object_), use_count_};
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
template <typename ManagedType>
class ManagedObjectHandler<ManagedType, ManagedType, SetTrait> {
 public:
  using ContainerTrait = SetTrait;
  using ThisType =
      ManagedObjectHandler<ManagedType, ManagedType, ContainerTrait>;
  using DataTableType =
      ManagedObjectTableBase<ManagedType, ManagedType, ContainerTrait>;
  friend class ManagedObjectTableBase<ManagedType, ManagedType, ContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(std::atomic<DataTableType*>* object_table,
                       const ManagedType* managed_object,
                       std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        managed_object_(managed_object),
        use_count_(use_count) {
    assert(object_table_ != nullptr && managed_object_ != nullptr &&
           use_count_ != nullptr);
    use_count_->fetch_add(1, std::memory_order_acq_rel);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    if (IsValid()) {
      use_count_->fetch_add(1, std::memory_order_acq_rel);
    }
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

    if (other.IsValid()) {
      other.use_count_->fetch_add(1, std::memory_order_acq_rel);
    }

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

    std::swap(use_count_, other.use_count_);
    std::swap(object_table_, other.object_table_);
    std::swap(managed_object_, other.managed_object_);

    return *this;
  }

 public:
  const ManagedType& GetObject() const { return *managed_object_; }

  const ManagedType* GetObjectPtr() const { return managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() const { return use_count_; }

  std::uint32_t GetUseCount() const {
    return use_count_->load(std::memory_order_relaxed);
  }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
      if (object_table_->load(std::memory_order_acquire)->IsMultiContainer()) {
        ReleaseMulti();
      } else {
        ReleaseNoMulti();
      }
    }

    object_table_ = nullptr;
    managed_object_ = nullptr;
    use_count_ = nullptr;
  }

  template <typename DriveType>
  ManagedObjectHandler<DriveType, DriveType, ContainerTrait> DynamicCast() {
    static_assert(std::is_base_of_v<ManagedType, DriveType>);

    return ManagedObjectHandler<DriveType, DriveType, ListTrait>{
        object_table_, dynamic_cast<DriveType*>(managed_object_), use_count_};
  }

 private:
  void ReleaseMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*managed_object_, use_count_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }
      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object));
      return;
    } while (true);
  }

  void ReleaseNoMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*managed_object_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }

      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));
      return;
    } while (true);
  }

 private:
  std::atomic<DataTableType*>* object_table_{nullptr};
  const ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename ManagedType>
class ManagedObjectHandler<ManagedType, ManagedType, HashSetTrait> {
 public:
  using ContainerTrait = HashSetTrait;
  using ThisType =
      ManagedObjectHandler<ManagedType, ManagedType, ContainerTrait>;
  using DataTableType =
      ManagedObjectTableBase<ManagedType, ManagedType, ContainerTrait>;
  friend class ManagedObjectTableBase<ManagedType, ManagedType, ContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(std::atomic<DataTableType*>* object_table,
                       const ManagedType* managed_object,
                       std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        managed_object_(managed_object),
        use_count_(use_count) {
    assert(object_table_ != nullptr && managed_object_ != nullptr &&
           use_count_ != nullptr);
    use_count_->fetch_add(1, std::memory_order_acq_rel);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    if (IsValid()) {
      use_count_->fetch_add(1, std::memory_order_acq_rel);
    }
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

    if (other.IsValid()) {
      other.use_count_->fetch_add(1, std::memory_order_acq_rel);
    }

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

    std::swap(use_count_, other.use_count_);
    std::swap(object_table_, other.object_table_);
    std::swap(managed_object_, other.managed_object_);

    return *this;
  }

 public:
  const ManagedType& GetObject() const { return *managed_object_; }

  const ManagedType* GetObjectPtr() const { return *managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() const { return use_count_; }

  std::uint32_t GetUseCount() const {
    return use_count_->load(std::memory_order_relaxed);
  }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
      if (object_table_->load(std::memory_order_acquire)->IsMultiContainer()) {
        ReleaseMulti();
      } else {
        ReleaseNoMulti();
      }
    }

    object_table_ = nullptr;
    managed_object_ = nullptr;
    use_count_ = nullptr;
  }

  template <typename DriveType>
  ManagedObjectHandler<DriveType, DriveType, ContainerTrait> DynamicCast() {
    static_assert(std::is_base_of_v<ManagedType, DriveType>);

    return ManagedObjectHandler<DriveType, DriveType, ListTrait>{
        object_table_, dynamic_cast<DriveType*>(managed_object_), use_count_};
  }

 private:
  void ReleaseMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*managed_object_, use_count_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }

      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));
      return;
    } while (true);
  }

  void ReleaseNoMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*managed_object_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }

      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));
      return;
    } while (true);
  }

 private:
  std::atomic<DataTableType*>* object_table_{nullptr};
  const ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

/**
 * \remark Using this type in reference mode with multiple threads can cause
 * errors. It is recommended to use this type through replication in a
 * multi-threaded environment.
 */
template <typename KeyType, typename ManagedType>
class ManagedObjectHandler<KeyType, ManagedType, MapTrait> {
 public:
  using ContainerTrait = MapTrait;
  using ThisType = ManagedObjectHandler<KeyType, ManagedType, ContainerTrait>;
  using DataTableType =
      ManagedObjectTableBase<KeyType, ManagedType, ContainerTrait>;
  friend class ManagedObjectTableBase<KeyType, ManagedType, ContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(
      std::atomic<ManagedObjectTableBase<KeyType, ManagedType,
                                         ContainerTrait>*>* object_table,
      const KeyType* key, ManagedType* managed_object,
      std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        key_(key),
        managed_object_(managed_object),
        use_count_(use_count) {
    assert(object_table_ != nullptr && key_ != nullptr &&
           managed_object_ != nullptr && use_count_ != nullptr);
    use_count_->fetch_add(1, std::memory_order_acq_rel);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        key_(other.key_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    if (IsValid()) {
      use_count_->fetch_add(1, std::memory_order_acq_rel);
    }
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

    if (other.IsValid()) {
      other.use_count_->fetch_add(1, std::memory_order_acq_rel);
    }

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

    std::swap(use_count_, other.use_count_);
    std::swap(object_table_, other.object_table_);
    std::swap(key_, other.key_);
    std::swap(managed_object_, other.managed_object_);

    return *this;
  }

 public:
  ManagedType& GetObject() { return *managed_object_; }

  const ManagedType& GetObject() const { return *managed_object_; }

  ManagedType* GetObjectPtr() { return managed_object_; }

  const ManagedType* GetObjectPtr() const { return managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() { return use_count_; }

  std::uint32_t GetUseCount() const {
    return use_count_->load(std::memory_order_relaxed);
  }

  const KeyType* GetKeyPtr() const { return key_; }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
      if (object_table_->load(std::memory_order_acquire)->IsMultiContainer()) {
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

  template <typename DriveType>
  ManagedObjectHandler<DriveType, DriveType, ContainerTrait> DynamicCast() {
    static_assert(std::is_base_of_v<ManagedType, DriveType>);

    return ManagedObjectHandler<DriveType, DriveType, ListTrait>{
        object_table_, key_, dynamic_cast<DriveType*>(managed_object_),
        use_count_};
  }

 private:
  void ReleaseMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*key_, use_count_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }

      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));
      return;
    } while (true);
  }

  void ReleaseNoMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*key_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }

      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));

      return;
    } while (true);
  }

 private:
  std::atomic<DataTableType*>* object_table_{nullptr};
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
class ManagedObjectHandler<KeyType, ManagedType, HashMapTrait> {
 public:
  using ContainerTrait = HashMapTrait;
  using ThisType = ManagedObjectHandler<KeyType, ManagedType, ContainerTrait>;
  using DataTableType =
      ManagedObjectTableBase<KeyType, ManagedType, ContainerTrait>;
  friend class ManagedObjectTableBase<KeyType, ManagedType, ContainerTrait>;

 public:
  ManagedObjectHandler() = default;
  ~ManagedObjectHandler() { Release(); }
  ManagedObjectHandler(std::atomic<DataTableType*>* object_table,
                       const KeyType* key, ManagedType* managed_object,
                       std::atomic_uint32_t* use_count)
      : object_table_(object_table),
        key_(key),
        managed_object_(managed_object),
        use_count_(use_count) {
    assert(object_table != nullptr && key != nullptr &&
           managed_object_ != nullptr && use_count_ != nullptr);
    use_count_->fetch_add(1, std::memory_order_acq_rel);
  }
  ManagedObjectHandler(const ManagedObjectHandler& other)
      : object_table_(other.object_table_),
        key_(other.key_),
        managed_object_(other.managed_object_),
        use_count_(other.use_count_) {
    if (IsValid()) {
      use_count_->fetch_add(1, std::memory_order_acq_rel);
    }
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

    if (other.IsValid()) {
      other.use_count_->fetch_add(1, std::memory_order_acq_rel);
    }

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

    std::swap(use_count_, other.use_count_);
    std::swap(key_, other.key_);
    std::swap(object_table_, other.object_table_);
    std::swap(managed_object_, other.managed_object_);

    return *this;
  }

 public:
  ManagedType& GetObject() { return *managed_object_; }

  const ManagedType& GetObject() const { return *managed_object_; }

  ManagedType* GetObjectPtr() { return managed_object_; }

  const ManagedType* GetObjectPtr() const { return managed_object_; }

  const std::atomic_uint32_t* GetUseCountPtr() const { return use_count_; }

  std::uint32_t GetUseCount() const {
    return use_count_->load(std::memory_order_relaxed);
  }

  const KeyType& GetKey() const { return *key_; }

  const KeyType* GetKeyPtr() const { return key_; }

  bool IsValid() const { return managed_object_ != nullptr; }

  void Release() {
    if (!IsValid()) {
      return;
    }

    if (use_count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
      if (object_table_->load(std::memory_order_acquire)->IsMultiContainer()) {
        ReleaseMulti();
      } else {
        ReleaseNoMulti();
      }
    }

    object_table_ = nullptr;
    managed_object_ = nullptr;
    use_count_ = nullptr;
  }

  template <typename DriveType>
  ManagedObjectHandler<DriveType, DriveType, ContainerTrait> DynamicCast() {
    static_assert(std::is_base_of_v<ManagedType, DriveType>);

    return ManagedObjectHandler<DriveType, DriveType, ListTrait>{
        object_table_, key_, dynamic_cast<DriveType*>(managed_object_),
        use_count_};
  }

 private:
  void ReleaseMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*key_, use_count_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }

      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));

      return;
    } while (true);
  }

  void ReleaseNoMulti() {
    do {
      Result<Nil, ErrorResult> result =
          object_table_->load(std::memory_order_acquire)
              ->RemoveObjectImp(*key_, ContainerTrait());
      if (result.IsSuccess()) {
        result.IgnoreException();
        return;
      }
      if (result.GetError().GetErrorCode() == ErrorCode::CUSTOM_ERROR) {
        result.IgnoreException();
        continue;
      }

      result.Exception(MM_ERROR_DESCRIPTION(Failed to remove object.));

      return;
    } while (true);
  }

 private:
  std::atomic<DataTableType*>* object_table_{nullptr};
  const KeyType* key_{nullptr};
  ManagedType* managed_object_{nullptr};
  std::atomic_uint32_t* use_count_{nullptr};
};

}  // namespace Manager
}  // namespace MM