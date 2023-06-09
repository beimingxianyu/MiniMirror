#pragma once

#include <functional>
#include <unordered_set>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/utils.h"
#include "utils/hash_table.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>,
          typename IsCanMoved = CanMoved>
class ManagedObjectUnorderedSet
    : public ManagedObjectTableBase<ObjectType, ObjectType, HashSetTrait> {
 public:
  using ContainerTrait = HashSetTrait;
  using CanMovedTrait = IsCanMoved;
  using ThisType =
      ManagedObjectUnorderedSet<ObjectType, Hash, Equal, Allocator, IsCanMoved>;
  using BaseType =
      ManagedObjectTableBase<ObjectType, ObjectType, ContainerTrait>;
  using HashKeyType = ObjectType;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = Utils::HashSet<
      WrapperType, typename WrapperType::template HashWrapper<Hash>,
      typename WrapperType::template EqualWrapper<Equal>, Allocator>;
  using ContainerReturnType = const ObjectType;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value,
      std::uint64_t bucket_count);

 public:
  ManagedObjectUnorderedSet()
      : ManagedObjectTableBase<ObjectType, ObjectType, ListTrait>(this),
        data_(131) {}
  ~ManagedObjectUnorderedSet() {
    if (!data_.Empty()) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedSet(std::uint32_t size)
      : ManagedObjectTableBase<ObjectType, ObjectType, ListTrait>(this),
        data_(size) {}
  ManagedObjectUnorderedSet(const ManagedObjectUnorderedSet& other) = delete;
  ManagedObjectUnorderedSet(ManagedObjectUnorderedSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CanMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");

    LockAll guard{other};

    BaseType::operator=(std::move(other));
    data_ = std::move(other);
  }
  ManagedObjectUnorderedSet& operator=(const ManagedObjectUnorderedSet& other) =
      delete;
  ManagedObjectUnorderedSet& operator=(
      ManagedObjectUnorderedSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CannotMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");

    if (&other == this) {
      return *this;
    }

    std::lock(data_mutex0_, data_mutex1_, data_mutex2_, data_mutex3_,
              data_mutex4_, data_mutex5_, data_mutex6_, data_mutex7_,
              data_mutex8_, data_mutex9_, data_mutex10_, data_mutex11_,
              data_mutex12_, data_mutex13_, data_mutex14_, data_mutex15_,
              other.data_mutex0_, other.data_mutex1_, other.data_mutex2_,
              other.data_mutex3_, other.data_mutex4_, other.data_mutex5_,
              other.data_mutex6_, other.data_mutex7_, other.data_mutex8_,
              other.data_mutex9_, other.data_mutex10_, other.data_mutex11_,
              other.data_mutex12_, other.data_mutex13_, other.data_mutex14_,
              other.data_mutex15_);
    if (!data_.Empty()) {
      LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }
    LockAll main_guard(*this, std::adopt_lock),
        other_guard(other, std::adopt_lock);

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);

    return *this;
  }

 public:
  size_t GetSize() const override { return data_.Size(); }

  bool Have(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard(ChooseMutexIn(key));
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Contains(key);
  }

  uint32_t Count(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Count(key);
  }

  bool IsMultiContainer() const override { return false; }

  bool IsRelationshipContainer() const override { return false; }

  ExecuteResult AddObject(ObjectType&& managed_object, HandlerType& handler) {
    return AddObjectImp(std::move(managed_object), handler);
  }

  ExecuteResult GetObject(const ObjectType& object,
                          HandlerType& handler) const {
    return GetObjectImp(object, handler);
  }

  std::uint32_t GetUseCount(const ObjectType& object) const {
    return GetUseCountImp(object);
  }

 protected:
  ExecuteResult AddObjectImp(ObjectType&& managed_object,
                             HandlerType& handler) override {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(managed_object)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(managed_object);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(managed_object);
      }
    }

    std::pair<ContainerReturnType&, bool> insert_result =
        data_.Emplace(std::move(managed_object));
    HandlerType new_handler{
        BaseType::GetThisPtrPtr(),
        const_cast<ObjectType*>(insert_result.first.GetObjectPtr()),
        insert_result.first.GetUseCountPtr()};
    guard.unlock();

    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const ObjectType* removed_object_key,
                                ContainerTrait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(*removed_object_key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(*removed_object_key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(*removed_object_key);
      }
    }

    if (ThisType::this_ptr_ptr_ == nullptr) {
      return ExecuteResult::CUSTOM_ERROR;
    }

    ContainerReturnType* iter = data_.Find(*removed_object_key);

    if (iter == NULL) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    if (iter->GetUseCount() == 0) {
      return data_.Erase(iter);
    }

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult GetObjectImp(const ObjectType& key,
                             HandlerType& handler) const override {
    if (!BaseType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    ContainerReturnType* iter = data_.find(key);

    if (iter != nullptr) {
      HandlerType new_handler =
          HandlerType{BaseType::GetThisPtrPtr(),
                      const_cast<ObjectType*>(iter->GetObjectPtr()),
                      iter->GetUseCountPtr()};
      guard.unlock();
      handler = std::move(new_handler);

      return ExecuteResult::SUCCESS;
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  uint32_t GetUseCountImp(const ObjectType& key) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    ContainerReturnType* iter = data_.Find(key);
    if (iter != nullptr) {
      return iter->GetUseCount();
    }

    return 0;
  }

 private:
  std::shared_mutex& ChooseMutexIn(const ObjectType& key) const {
    return ChooseMutex(*this, Hash{}(key), data_.BucketCount());
  }

 private:
  ContainerType data_{};

  mutable std::shared_mutex data_mutex0_{};
  mutable std::shared_mutex data_mutex1_{};
  mutable std::shared_mutex data_mutex2_{};
  mutable std::shared_mutex data_mutex3_{};
  mutable std::shared_mutex data_mutex4_{};
  mutable std::shared_mutex data_mutex5_{};
  mutable std::shared_mutex data_mutex6_{};
  mutable std::shared_mutex data_mutex7_{};
  mutable std::shared_mutex data_mutex8_{};
  mutable std::shared_mutex data_mutex9_{};
  mutable std::shared_mutex data_mutex10_{};
  mutable std::shared_mutex data_mutex11_{};
  mutable std::shared_mutex data_mutex12_{};
  mutable std::shared_mutex data_mutex13_{};
  mutable std::shared_mutex data_mutex14_{};
  mutable std::shared_mutex data_mutex15_{};
};

template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>,
          typename IsCanMoved = CanMoved>
class ManagedObjectUnorderedMultiSet
    : public ManagedObjectTableBase<ObjectType, ObjectType, ListTrait> {
 public:
  using RelationshipContainerTrait = ListTrait;
  using CanMovedTrait = IsCanMoved;
  using ThisType = ManagedObjectUnorderedSet<ObjectType, Hash, Equal, Allocator,
                                             CanMovedTrait>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType,
                                          RelationshipContainerTrait>;
  using HashKeyType = ObjectType;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = Utils::MultiHashSet<
      WrapperType, typename WrapperType::template HashWrapper<Hash>,
      typename WrapperType::template EqualWrapper<Equal>, Allocator>;
  using ContainerReturnType = const ObjectType;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value,
      std::uint64_t bucket_count);

 public:
  ManagedObjectUnorderedMultiSet()
      : ManagedObjectTableBase<ObjectType, ObjectType, ListTrait>(this),
        data_(131) {}
  ~ManagedObjectUnorderedMultiSet() {
    if (!data_.empty()) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedMultiSet(std::uint32_t size)
      : ManagedObjectTableBase<ObjectType, ObjectType, ListTrait>(this),
        data_(size) {}
  ManagedObjectUnorderedMultiSet(const ManagedObjectUnorderedMultiSet& other) =
      delete;
  ManagedObjectUnorderedMultiSet(
      ManagedObjectUnorderedMultiSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CannotMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");
    LockAll guard(other);

    BaseType::operator=(std::move(other));
    data_ = std::move(data_);
  }
  ManagedObjectUnorderedMultiSet& operator=(
      const ManagedObjectUnorderedMultiSet& other) = delete;
  ManagedObjectUnorderedMultiSet& operator=(
      ManagedObjectUnorderedMultiSet&& other) noexcept {
    static_assert(std::is_same_v<CanMovedTrait, CannotMoved>,
                  "The move operation can only be used if the template "
                  "parameter 'IsCanMoved' is marked as 'MM::Utils::TrueType'.");
    if (&other == this) {
      return *this;
    }

    std::lock(data_mutex0_, data_mutex1_, data_mutex2_, data_mutex3_,
              data_mutex4_, data_mutex5_, data_mutex6_, data_mutex7_,
              data_mutex8_, data_mutex9_, data_mutex10_, data_mutex11_,
              data_mutex12_, data_mutex13_, data_mutex14_, data_mutex15_,
              other.data_mutex0_, other.data_mutex1_, other.data_mutex2_,
              other.data_mutex3_, other.data_mutex4_, other.data_mutex5_,
              other.data_mutex6_, other.data_mutex7_, other.data_mutex8_,
              other.data_mutex9_, other.data_mutex10_, other.data_mutex11_,
              other.data_mutex12_, other.data_mutex13_, other.data_mutex14_,
              other.data_mutex15_);
    if (!data_.empty()) {
      LOG_ERROR(
          "If there is data in the original container but it is reassigned, an "
          "access error will occur.");
    }
    LockAll main_guard(*this, std::adopt_lock),
        other_guard(other, std::adopt_lock);

    BaseType::operator=(std::move(other));
    data_ = std::move(other.data_);

    return *this;
  }

 public:
  size_t GetSize() const override { return data_.Size(); }

  bool Have(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Count(key) != 0;
  }

  uint32_t Count(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    return data_.Count(key);
  }

  bool IsMultiContainer() const override { return true; }

  bool IsRelationshipContainer() const override { return false; }

  ExecuteResult AddObject(ObjectType&& managed_object, HandlerType& handler) {
    return AddObjectImp(std::move(managed_object), handler);
  }

  ExecuteResult RemoveObject(const ObjectType& removed_object_key,
                             const std::atomic_uint32_t* use_count_ptr,
                             RelationshipContainerTrait trait) {
    return RemoveObjectImp(removed_object_key, use_count_ptr, trait);
  }

  ExecuteResult GetObject(const ObjectType& key, HandlerType& handler) const {
    return GetObjectImp(key, handler);
  }

  ExecuteResult GetObject(const ObjectType& key,
                          const std::atomic_uint32_t* use_count_ptr,
                          HandlerType& handler) const {
    return GetObjectImp(key, use_count_ptr, handler);
  }

  ExecuteResult GetObject(const ObjectType& key, const ObjectType& object,
                          HandlerType& handler) const {
    return GetObjectImp(key, object, handler);
  }

  ExecuteResult GetObject(const ObjectType& key,
                          std::vector<HandlerType>& handlers) const {
    return GetObject(key, handlers);
  }

  std::uint32_t GetUseCount(const ObjectType& key) const {
    return GetUseCountImp(key);
  }

  std::uint32_t GetUseCount(const ObjectType& key,
                            const std::atomic_uint32_t* use_count) const {
    return GetUseCountImp(key, use_count);
  }

  ExecuteResult GetUseCount(const ObjectType& key,
                            std::vector<std::uint32_t>& use_counts) const {
    return GetUseCountImp(key, use_counts);
  }

 protected:
  ExecuteResult AddObjectImp(ObjectType&& managed_object,
                             HandlerType& handler) override {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(managed_object)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(managed_object);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(managed_object);
      }
    }

    std::pair<ContainerReturnType&, bool> iter =
        data_.Emplace(std::move(managed_object));
    HandlerType new_handler =
        HandlerType{BaseType::GetThisPtrPtr(),
                    const_cast<ObjectType*>(iter.first.GetObjectPtr()),
                    iter.first.GetUseCountPtr()};

    guard.unlock();
    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const ObjectType* removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                RelationshipContainerTrait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(*removed_object_key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(*removed_object_key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::unique_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(*removed_object_key);
      }
    }

    if (ThisType::this_ptr_ptr_ == nullptr) {
      return ExecuteResult::CUSTOM_ERROR;
    }

    ContainerReturnType* iter = data_.Find(*removed_object_key);

    if (iter == nullptr) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    if (iter->GetUseCount() == 0) {
      return data_.Erase(iter);
    }

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult GetObjectImp(const ObjectType& key,
                             HandlerType& handle) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    ContainerReturnType* iter = data_.Find(key);

    HandlerType new_handler = HandlerType{
        BaseType ::GetThisPtrPtr(),
        const_cast<ObjectType*>(iter->GetObjectPtr()), iter->GetUseCountPtr()};

    guard.unlock();
    handle = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult GetObjectImp(const ObjectType& key,
                             const std::atomic_uint32_t* use_count_ptr,
                             HandlerType& handle) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    bool find = false;
    HandlerType new_handler;

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      if (iter->GetUseCountPtr() == use_count_ptr) {
        new_handler = HandlerType{BaseType ::GetThisPtrPtr(),
                                  const_cast<ObjectType*>(iter->GetObjectPtr()),
                                  iter->GetUseCountPtr()};
        find = true;
        break;
      }
    }

    if (find) {
      guard.unlock();
      handle = new_handler;
      return ExecuteResult::SUCCESS;
    }

    return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
  }

  ExecuteResult GetObjectImp(
      const ObjectType& key,
      std::vector<HandlerType>& handlers) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::iterator iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      handlers.emplace_back(BaseType ::GetThisPtrPtr(),
                            const_cast<ObjectType*>(iter->GetObjectPtr()),
                            iter->GetUseCountPtr());
    }

    return ExecuteResult::SUCCESS;
  }

  std::uint32_t GetUseCountImp(const ObjectType& key) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return 0;
    }

    return equal_range.first->GetUseCount();
  }

  std::uint32_t GetUseCountImp(
      const ObjectType& key,
      const std::atomic_uint32_t* use_count_ptr) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return 0;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
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

  ExecuteResult GetUseCountImp(
      const ObjectType& key,
      std::vector<std::uint32_t>& use_counts) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    if constexpr (std::is_same_v<CanMovedTrait, CanMoved>) {
      std::shared_mutex* new_mutex = &ChooseMutexIn(key);
      while (guard.mutex() != new_mutex) {
        guard.unlock();
        guard = std::shared_lock<std::shared_mutex>(*new_mutex);
        new_mutex = &ChooseMutexIn(key);
      }
    }

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first != equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::iterator iter = equal_range.first;
         iter == equal_range.second; ++iter) {
      use_counts.emplace_back(iter->GetUseCount());
    }

    return ExecuteResult::SUCCESS;
  }

 private:
  std::shared_mutex& ChooseMutexIn(const ObjectType& key) const {
    return ChooseMutex(*this, Hash{}(key), data_.BucketCount());
  }

 private:
  ContainerType data_;

  mutable std::shared_mutex data_mutex0_{};
  mutable std::shared_mutex data_mutex1_{};
  mutable std::shared_mutex data_mutex2_{};
  mutable std::shared_mutex data_mutex3_{};
  mutable std::shared_mutex data_mutex4_{};
  mutable std::shared_mutex data_mutex5_{};
  mutable std::shared_mutex data_mutex6_{};
  mutable std::shared_mutex data_mutex7_{};
  mutable std::shared_mutex data_mutex8_{};
  mutable std::shared_mutex data_mutex9_{};
  mutable std::shared_mutex data_mutex10_{};
  mutable std::shared_mutex data_mutex11_{};
  mutable std::shared_mutex data_mutex12_{};
  mutable std::shared_mutex data_mutex13_{};
  mutable std::shared_mutex data_mutex14_{};
  mutable std::shared_mutex data_mutex15_{};
};
}  // namespace Manager
}  // namespace MM
