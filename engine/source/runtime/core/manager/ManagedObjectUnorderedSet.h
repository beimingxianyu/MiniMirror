#pragma once

#include <functional>
#include <unordered_set>

#include "runtime/core/manager/ManagedObjectTableBase.h"
#include "runtime/core/manager/utils.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>>
class ManagedObjectUnorderedSet
    : public ManagedObjectTableBase<ObjectType, ObjectType, NoKeyTrait> {
 public:
  using RelationshipContainerTrait = NoKeyTrait;
  using ThisType =
      ManagedObjectUnorderedSet<ObjectType, Hash, Equal, Allocator>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType,
                                          RelationshipContainerTrait>;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = std::unordered_set<
      ObjectType, typename WrapperType::template HashWrapper<Hash>,
      typename WrapperType::template EqualWrapper<Equal>, Allocator>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value);

 public:
  ManagedObjectUnorderedSet() : data_(8) {}
  ~ManagedObjectUnorderedSet() {
    if (!data_.empty()) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  explicit ManagedObjectUnorderedSet(std::uint32_t size) : data_(size) {}
  ManagedObjectUnorderedSet(const ManagedObjectUnorderedSet& other) = delete;
  ManagedObjectUnorderedSet(ManagedObjectUnorderedSet&& other) noexcept {
    LockAll guard{other};

    BaseType::operator=(other);
    data_ = std::move(other);
  }
  ManagedObjectUnorderedSet& operator=(const ManagedObjectUnorderedSet& other) =
      delete;
  ManagedObjectUnorderedSet& operator=(
      ManagedObjectUnorderedSet&& other) noexcept {
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
    LockAll main_guard(*this), other_guard(other);

    BaseType::operator=(other);
    data_ = std::move(other.data_);

    return *this;
  }

 public:
  size_t GetSize() const override { return data_.size(); }

  bool Have(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard(ChooseMutexIn(key));
    return data_.count(key) != 0;
  }

  uint32_t Count(const ObjectType& key) const override {
    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
    return data_.count(key);
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

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(managed_object)};

    typename std::pair<typename ContainerType::iterator, bool> insert_result =
        data_.emplace(std::move(managed_object));
    HandlerType new_handler{
        BaseType::GetThisPtrPtr(),
        const_cast<ObjectType*>(insert_result.first->GetObjectPtr()),
        insert_result.first->GetUseCountPtr()};
    guard.unlock();

    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                RelationshipContainerTrait trait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(removed_object_key)};

    if (ThisType::this_ptr_ptr_ == nullptr) {
      return ExecuteResult::CUSTOM_ERROR;
    }

    auto iter = data_.find(removed_object_key);

    if (iter == data_.end()) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    if (iter->GetUseCount() == 0) {
      data_.erase(iter);
    }

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult GetObjectImp(const ObjectType& key,
                             HandlerType& handler) const override {
    if (!BaseType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    auto iter = data_.find(key);

    if (iter != data_.end()) {
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

    typename ContainerType::iterator iter = data_.find(key);
    if (iter != data_.end()) {
      return iter->GetUseCount();
    }

    return 0;
  }

 private:
  void ResizeWhenNeeded() {
    LockAll guard(*this);
    if (data_.size() >= data_.bucket_count() - 1) {
      assert(data_.bucket_count() < UINT_LEAST64_MAX / 2);
      data_.reserve(data_.bucket_count() * 2);
    }
  }

  std::shared_mutex& ChooseMutexIn(const ObjectType& key) const {
    return ChooseMutex(*this, Hash{}(key));
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
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>>
class ManagedObjectUnorderedMultiSet
    : public ManagedObjectTableBase<ObjectType, ObjectType, NoKeyTrait> {
 public:
  using RelationshipContainerTrait = NoKeyTrait;
  using ThisType =
      ManagedObjectUnorderedSet<ObjectType, Hash, Equal, Allocator>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType,
                                          RelationshipContainerTrait>;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using ContainerType = std::unordered_multiset<
      ObjectType, typename WrapperType::template HashWrapper<Hash>,
      typename WrapperType::template EqualWrapper<Equal>, Allocator>;

  friend struct LockAll<ThisType>;
  template <typename ManagedObjectTable>
  friend std::shared_mutex& ChooseMutex(
      const ManagedObjectTable& managed_object_table, std::uint64_t hash_value);

 public:
  ManagedObjectUnorderedMultiSet() : data_(8) {}
  ~ManagedObjectUnorderedMultiSet() {
    if (!data_.empty()) {
      LOG_ERROR(
          "The container is not empty, and destroying it will result in an "
          "access error.");
    }
  }
  ManagedObjectUnorderedMultiSet(std::uint32_t size) : data_(size) {}
  ManagedObjectUnorderedMultiSet(const ManagedObjectUnorderedMultiSet& other) =
      delete;
  ManagedObjectUnorderedMultiSet(
      ManagedObjectUnorderedMultiSet&& other) noexcept {
    LockAll guard(other);

    BaseType::operator=(other);
    data_ = std::move(data_);
  }
  ManagedObjectUnorderedMultiSet& operator=(
      const ManagedObjectUnorderedMultiSet& other) = delete;
  ManagedObjectUnorderedMultiSet& operator=(
      ManagedObjectUnorderedMultiSet&& other) noexcept {
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
    LockAll main_guard(*this), other_guard(other);

    BaseType::operator=(other);
    data_ = std::move(other.data_);

    return *this;
  }

 public:
  size_t GetSize() const override { return data_.size(); }

  bool Have(const ObjectType& key) const override {
    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    return data_.count(key) != 0;
  }

  uint32_t Count(const ObjectType& key) const override {
    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    return data_.count(key);
  }

  bool IsMultiContainer() const override { return true; }

  bool IsRelationshipContainer() const override { return false; }

 protected:
  ExecuteResult AddObjectImp(ObjectType&& managed_object,
                             HandlerType& handler) override {
    if (!ThisType::TestMovedWhenAddObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    ResizeWhenNeeded();

    std::unique_lock<std::shared_mutex> guard{ChooseMutexIn(managed_object)};

    typename ContainerType::iterator iter =
        data_.emplace(std::move(managed_object));
    HandlerType new_handler = HandlerType{
        BaseType::GetThisPtrPtr(),
        const_cast<ObjectType*>(iter->GetObjectPtr()), iter->GetUseCountPtr()};

    guard.unlock();
    handler = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult RemoveObjectImp(const ObjectType& removed_object_key,
                                const std::atomic_uint32_t* use_count_ptr,
                                RelationshipContainerTrait trait) override {
    std::unique_lock<std::shared_mutex> guard{
        ChooseMutexIn(removed_object_key)};
    if (ThisType::this_ptr_ptr_ == nullptr) {
      return ExecuteResult::CUSTOM_ERROR;
    }

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(removed_object_key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    for (typename ContainerType::iterator& iter = equal_range.first;
         iter != equal_range.second; ++iter) {
      if ((*iter).GetUseCountPtr() == use_count_ptr) {
        if (iter->GetUseCount() == 0) {
          data_.erase(iter);
        }

        return ExecuteResult::SUCCESS;
      }
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  ExecuteResult GetObjectImp(const ObjectType& key,
                             HandlerType& handle) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first == equal_range.second) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    HandlerType new_handler =
        HandlerType{BaseType ::GetThisPtrPtr(),
                    const_cast<ObjectType*>(equal_range.first->GetObjectPtr()),
                    equal_range.first->GetUseCountPtr()};

    guard.unlock();
    handle = std::move(new_handler);

    return ExecuteResult::SUCCESS;
  }

  ExecuteResult GetObjectImp(const ObjectType& key,
                             const std::atomic_uint32_t* use_count_ptr,
                             ThisType::HandlerType& handle) const override {
    if (!ThisType::TestMovedWhenGetObject()) {
      return ExecuteResult::OPERATION_NOT_SUPPORTED;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};
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

  void GetUseCountImp(const ObjectType& key,
                      std::vector<std::uint32_t>& use_counts) const override {
    if (!ThisType::TestMoveWhenGetUseCount()) {
      return;
    }

    std::shared_lock<std::shared_mutex> guard{ChooseMutexIn(key)};

    std::pair<typename ContainerType::iterator,
              typename ContainerType::iterator>
        equal_range = data_.equal_range(key);
    if (equal_range.first != equal_range.second) {
      return;
    }

    for (typename ContainerType::iterator iter = equal_range.first;
         iter == equal_range.second; ++iter) {
      use_counts.emplace_back(iter->GetUseCount());
    }
  }

 private:
  void ResizeWhenNeeded() {
    LockAll guard(*this);
    if (data_.size() >= data_.bucket_count() - 1) {
      assert(data_.bucket_count() < UINT_LEAST64_MAX / 2);
      data_.reserve(data_.bucket_count() * 2);
    }
  }

  std::shared_mutex& ChooseMutexIn(const ObjectType& key) const {
    return ChooseMutex(*this, Hash{}(key));
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
