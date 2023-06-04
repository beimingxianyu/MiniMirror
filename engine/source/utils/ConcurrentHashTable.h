#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <mutex>
#include <shared_mutex>

#include "utils.h"
#include "utils/error.h"
#include "utils/type_trait.h"

namespace MM {
namespace Utils {
template <typename RelationshipTrait, typename MultiTrait, typename KeyType,
          typename ObjectType, typename ReturnType,
          typename Hash = std::hash<KeyType>,
          typename Equal = std::equal_to<KeyType>,
          typename Allocator = std::allocator<ObjectType>>
class HashTable {
 public:
  using ThisType = HashTable<RelationshipTrait, MultiTrait, KeyType, ObjectType,
                             ReturnType, Hash, Equal, Allocator>;
  using IsMap = TrueType;
  using IsSet = FalseType;
  using IsMulti = TrueType;
  using NotMulti = FalseType;
  using SharedLockType = TrueType;
  using UniqueLockType = FalseType;

 private:
  using MutexType = std::shared_mutex;

  struct Node;

 public:
  HashTable() = default;
  virtual ~HashTable() {
    Clear();
    delete[] data_;
  }
  explicit HashTable(std::uint64_t size)
      : data_(new Node[size]{}),
        load_factor_(0.75),
        size_(0),
        bucket_count_(size) {}
  HashTable(const HashTable& other)
      : data_(nullptr), load_factor_(0.75), size_(0), bucket_count_(0) {
    other.LockAll();
    std::shared_lock<std::shared_mutex> guard0{other.data_mutex0_,
                                               std::adopt_lock},
        guard1{other.data_mutex1_, std::adopt_lock},
        guard2{other.data_mutex2_, std::adopt_lock},
        guard3{other.data_mutex3_, std::adopt_lock},
        guard4{other.data_mutex4_, std::adopt_lock},
        guard5{other.data_mutex5_, std::adopt_lock},
        guard6{other.data_mutex6_, std::adopt_lock},
        guard7{other.data_mutex7_, std::adopt_lock},
        guard8{other.data_mutex8_, std::adopt_lock},
        guard9{other.data_mutex9_, std::adopt_lock},
        guard10{other.data_mutex10_, std::adopt_lock},
        guard11{other.data_mutex11_, std::adopt_lock},
        guard12{other.data_mutex12_, std::adopt_lock},
        guard13{other.data_mutex13_, std::adopt_lock},
        guard14{other.data_mutex14_, std::adopt_lock},
        guard15{other.data_mutex15_, std::adopt_lock};

    data_ = new Node[other.bucket_count_]{};
    load_factor_ = other.load_factor_;
    size_ = other.size_;
    bucket_count_ = other.bucket_count_;

    for (std::uint64_t i = 0; i != bucket_count_; ++i) {
      if (other.data_[i].object_) {
        Node* other_first_node = &other.data_[i];
        Node* first_node = &data_[i];
        data_[i].object_ =
            std::make_unique<ObjectType>(*(other_first_node->object_));
        while (other_first_node->next_node_) {
          Node* next_node =
              new Node{std::make_unique<ObjectType>(
                           *(other_first_node->next_node_->object_)),
                       nullptr};
          first_node->next_node_ = next_node;
          first_node = first_node->next_node_;

          other_first_node = other_first_node->next_node_;
        }
      }
    }
  }

  HashTable(HashTable&& other) noexcept
      : data_(nullptr), load_factor_(0), size_(0), bucket_count_(0) {
    LockAllGuard guard{other};

    data_ = other.data_;
    load_factor_ = other.load_factor_;
    size_ = other.size_;
    bucket_count_ = other.bucket_count_;

    other.data_ = new Node[131]{};
    other.load_factor_ = 0.75;
    other.size_ = 0;
    other.bucket_count_ = 131;
  }
  HashTable& operator=(const HashTable& other) {
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
    LockAllGuard main_guard{*this, std::adopt_lock};
    std::shared_lock<std::shared_mutex> guard0{other.data_mutex0_,
                                               std::adopt_lock},
        guard1{other.data_mutex1_, std::adopt_lock},
        guard2{other.data_mutex2_, std::adopt_lock},
        guard3{other.data_mutex3_, std::adopt_lock},
        guard4{other.data_mutex4_, std::adopt_lock},
        guard5{other.data_mutex5_, std::adopt_lock},
        guard6{other.data_mutex6_, std::adopt_lock},
        guard7{other.data_mutex7_, std::adopt_lock},
        guard8{other.data_mutex8_, std::adopt_lock},
        guard9{other.data_mutex9_, std::adopt_lock},
        guard10{other.data_mutex10_, std::adopt_lock},
        guard11{other.data_mutex11_, std::adopt_lock},
        guard12{other.data_mutex12_, std::adopt_lock},
        guard13{other.data_mutex13_, std::adopt_lock},
        guard14{other.data_mutex14_, std::adopt_lock},
        guard15{other.data_mutex15_, std::adopt_lock};

    if (!Empty()) {
      Clear();
    }

    delete[] data_;

    data_ = new Node[other.bucket_count_]{};
    load_factor_ = other.load_factor_;
    size_ = other.size_;
    bucket_count_ = other.bucket_count_;

    for (std::uint64_t i = 0; i != bucket_count_; ++i) {
      if (other.data_[i].object_) {
        Node* other_first_node = &other.data_[i];
        Node* first_node = &data_[i];
        data_[i].object_ =
            std::make_unique<ObjectType>(*(other_first_node->object_));
        while (other_first_node->next_node_) {
          Node* next_node =
              new Node{std::make_unique<ObjectType>(
                           *(other_first_node->next_node_->object_)),
                       nullptr};
          first_node->next_node_ = next_node;
          first_node = first_node->next_node_;

          other_first_node = other_first_node->next_node_;
        }
      }
    }

    return *this;
  }
  HashTable& operator=(HashTable&& other) noexcept {
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
    LockAllGuard main_guard{*this, std::adopt_lock},
        other_guard{other, std::adopt_lock};

    if (!Empty()) {
      Clear();
    }

    delete[] data_;

    data_ = other.data_;
    load_factor_ = other.load_factor_;
    size_ = other.size_;
    bucket_count_ = other.bucket_count_;

    other.data_ = new Node[131]{};
    other.load_factor_ = 0.75;
    other.size_ = 0;
    other.bucket_count_ = 131;

    return *this;
  }

 public:
  bool Empty() const { return size_ == 0; }

  std::uint64_t Size() const { return size_; }

  std::uint64_t BucketCount() const { return bucket_count_; }

  void Clear() {
    LockAllGuard guard{*this};
    for (std::uint64_t i = 0; i != bucket_count_; ++i) {
      if (data_[i].object_ != nullptr) {
        DeleteOneList(&data_[i]);
      }
    }

    size_ = 0;
  }

  std::pair<ReturnType&, bool> Insert(const ObjectType& object) {
    return Insert(object, MultiTrait{});
  }

  std::pair<ReturnType&, bool> Insert(ObjectType&& other) {
    return Insert(std::move(other), MultiTrait{});
  }

  template <typename... Args>
  std::pair<ReturnType&, bool> Emplace(Args&&... args) {
    return Emplace(MultiTrait{}, std::forward<Args>(args)...);
  }

  ExecuteResult Erase(const ObjectType* object_ptr) {
    if (object_ptr == nullptr) {
      return ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE;
    }

    std::uint64_t hash_code = GetObjectHash(*object_ptr);
    LockGuard guard{*this, hash_code, UniqueLockType{}};

    std::uint64_t data_offset = hash_code % bucket_count_;
    if (data_[data_offset].object_ == nullptr) {
      return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
    }

    Node* first_node = &data_[data_offset];
    if (first_node->object_ == object_ptr) {
      data_[data_offset] = std::move(*(first_node->next_node_));
      --size_;

      return ExecuteResult::SUCCESS;
    }

    while (first_node->next_node_ != nullptr &&
           first_node->next_node_->object_ != object_ptr) {
      first_node = first_node->next_node_;
    }

    if (first_node->next_node_->object_ == object_ptr) {
      Node* old_next_node = first_node->next_node_;
      first_node->next_node_ = first_node->next_node_->next_node_;
      delete old_next_node;
      --size_;

      return ExecuteResult::SUCCESS;
    }

    return ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT;
  }

  std::uint32_t Erase(const KeyType& key) {
    std::uint64_t hash_code = Hash{}(key);
    LockGuard guard{*this, hash_code, UniqueLockType{}};

    std::uint64_t data_offset = hash_code % bucket_count_;
    if (data_[data_offset].object_ == nullptr) {
      return 0;
    }

    std::uint32_t count = 0;
    while (KeyEqual2(data_[data_offset], key)) {
      if (data_[data_offset].next_node_) {
        data_[data_offset] = std::move(*(data_[data_offset].next_node_));
        ++count;
      } else {
        data_[data_offset].object_.reset();
        ++count;
        return count;
      }
    }

    Node* node_ptr = &data_[data_offset];
    while (node_ptr->next_node_) {
      if (KeyEqual2(*(node_ptr->next_node_), key)) {
        Node* old_nex_node = node_ptr->next_node_;
        node_ptr->next_node_ = node_ptr->next_node_->next_node_;
        delete old_nex_node;

        node_ptr = node_ptr->next_node_;
        continue;
      }
      node_ptr = node_ptr->next_node_;
    }

    size_ -= count;

    return count;
  }

  std::uint32_t Count(const KeyType& key) const {
    std::uint64_t hash_code = Hash{}(key);
    LockGuard guard(*this, hash_code, SharedLockType{});

    const Node* first_node = &data_[hash_code % bucket_count_];
    if (first_node->object_ == nullptr) {
      return 0;
    }

    std::uint32_t count = 1;
    while (first_node->next_node_) {
      if (KeyEqual2(*(first_node->object_), key)) {
        ++count;
      }
      first_node = first_node->next_node_;
    }

    return count;
  }

  ReturnType* Find(const KeyType& key) {
    std::uint64_t hash_code = Hash{}(key);
    LockGuard guard(*this, hash_code, SharedLockType{});

    Node* first_node = &data_[hash_code % bucket_count_];

    if (first_node->object_) {
      while (first_node) {
        if (KeyEqual2(*(first_node->object_), key)) {
          return first_node->object_.get();
        }
        first_node = first_node->next_node_;
      }

      return nullptr;
    }

    return nullptr;
  }

  const ReturnType* Find(const KeyType& key) const {
    std::uint64_t hash_code = Hash{}(key);
    LockGuard guard(*this, hash_code, SharedLockType{});

    const Node* first_node = &data_[hash_code % bucket_count_];

    if (first_node->object_) {
      while (first_node) {
        if (KeyEqual2(*(first_node->object_), key)) {
          return first_node->object_.get();
        }
        first_node = first_node->next_node_;
      }

      return nullptr;
    }

    return nullptr;
  }

  bool Contains(const KeyType& key) const { return Find(key) != nullptr; }

  std::vector<ReturnType*> EqualRange(const KeyType& key) {
    std::uint64_t hash_code = Hash{}(key);
    LockGuard guard{*this, hash_code, SharedLockType{}};

    Node* first_node = &data_[hash_code % bucket_count_];

    if (first_node->object_ == nullptr) {
      return std::vector<ReturnType*>{};
    }

    std::vector<ReturnType*> result;
    while (first_node) {
      if (KeyEqual2(*(first_node->object_), key)) {
        result.emplace_back(first_node->object_.get());
      }

      first_node = first_node->next_node_;
    }

    return result;
  }

  std::vector<const ReturnType*> EqualRange(const KeyType& key) const {
    std::uint64_t hash_code = Hash{}(key);
    LockGuard guard{*this, hash_code, SharedLockType{}};

    const Node* first_node = data_[hash_code % bucket_count_];

    if (first_node == nullptr) {
      return std::vector<ReturnType>{};
    }

    std::vector<const ReturnType*> result;
    while (first_node) {
      if (KeyEqual2(*(first_node->object_), key)) {
        result.emplace_back(first_node->object_.get());
      }

      first_node = first_node->next_node_;
    }

    return result;
  }

  void ReHash(std::uint64_t new_bucket_size) {
    if (new_bucket_size > bucket_count_) {
      LockAllGuard guard{*this};

      new_bucket_size = MinPrime(new_bucket_size);

      Node* new_data = new Node[new_bucket_size]{};

      for (std::uint64_t i = 0; i != bucket_count_; ++i) {
        if (data_[i].object_) {
          std::uint64_t insert_pos =
              GetObjectHash(*(data_[i].object_)) % new_bucket_size;

          bool insert_first = false;
          if (new_data[insert_pos].object_ == nullptr) {
            new_data[insert_pos].object_ =
                std::make_unique<ObjectType>(std::move(*(data_[i].object_)));
            insert_first = true;
          }

          if (!insert_first) {
            Node* new_data_first_node = &new_data[insert_pos];
            while (new_data_first_node->next_node_) {
              new_data_first_node = new_data_first_node->next_node_;
            }
            new_data_first_node->next_node_ = new Node{
                std::make_unique<ObjectType>(std::move(*(data_[i].object_))),
                nullptr};
          }

          Node* old_data_first_node = data_[i].next_node_;
          while (old_data_first_node) {
            insert_pos = GetObjectHash(*(old_data_first_node->object_)) %
                         new_bucket_size;

            if (new_data[insert_pos].object_ == nullptr) {
              new_data[insert_pos].object_ = std::make_unique<ObjectType>(
                  std::move(*(old_data_first_node->object_)));
              old_data_first_node = old_data_first_node->next_node_;
              continue;
            }

            Node* new_data_first_node = &new_data[insert_pos];
            while (new_data_first_node->next_node_) {
              new_data_first_node = new_data_first_node->next_node_;
            }
            new_data_first_node->next_node_ =
                new Node{std::make_unique<ObjectType>(
                             std::move(*(old_data_first_node->object_))),
                         nullptr};
            old_data_first_node = old_data_first_node->next_node_;
          }
        }
      }

      bucket_count_ = new_bucket_size;
      delete[] data_;
      data_ = new_data;
    }
  }

  double GetLoadFactor() const { return load_factor_; }

  void SetLoadFactor(double new_load_factor) {
    LockAllGuard guard(*this);
    load_factor_ = new_load_factor;
  }

  Node* Data() { return data_; }

  Node* Data() const { return data_; }

 private:
  std::pair<ReturnType&, bool> Insert(const ObjectType& other, NotMulti) {
    RehashWhenNeed();

    std::uint64_t hash_code = GetObjectHash(other);
    LockGuard guard(*this, hash_code, UniqueLockType{});

    std::uint64_t data_offset = hash_code % bucket_count_;
    if (data_[data_offset].object_ == nullptr) {
      data_[data_offset].object_ = std::make_unique<ObjectType>(other);
      ++size_;
      return {*(data_[data_offset].object_), true};
    }

    Node* first_node = &data_[data_offset];
    while (first_node) {
      if (KeyEqual(*(first_node->object_), other)) {
        return {*(first_node->object_), false};
      }

      first_node = first_node->next_node_;
    }

    Node* old_first_node = new Node{std::move(data_[data_offset])};
    data_[data_offset] =
        Node{std::make_unique<ObjectType>(other), old_first_node};
    ++size_;
    return {*(data_[data_offset].object_), true};
  }

  std::pair<ReturnType&, bool> Insert(const ObjectType& other, IsMulti) {
    RehashWhenNeed();

    std::uint64_t hash_code = GetObjectHash(other);
    LockGuard guard(*this, hash_code, UniqueLockType{});

    std::uint64_t data_offset = hash_code % bucket_count_;
    if (data_[data_offset].object_ == nullptr) {
      data_[data_offset].object_ = std::make_unique<ObjectType>(other);
      ++size_;
      return {*(data_[data_offset].object_), true};
    }

    Node* old_first_node = new Node{std::move(data_[data_offset])};
    data_[data_offset] =
        Node{std::make_unique<ObjectType>(other), old_first_node};
    ++size_;
    return {*(data_[data_offset].object_), true};
  }

  std::pair<ReturnType&, bool> Insert(ObjectType&& other, NotMulti) {
    RehashWhenNeed();

    std::uint64_t hash_code = GetObjectHash(other);
    LockGuard guard(*this, hash_code, UniqueLockType{});

    std::uint64_t data_offset = hash_code % bucket_count_;
    if (data_[data_offset].object_ == nullptr) {
      data_[data_offset].object_ =
          std::make_unique<ObjectType>(std::move(other));
      ++size_;
      return {*(data_[data_offset].object_), true};
    }

    Node* first_node = &data_[data_offset];
    while (first_node) {
      if (KeyEqual(*(first_node->object_), other)) {
        return {*(first_node->object_), false};
      }

      first_node = first_node->next_node_;
    }

    Node* old_first_node = new Node{std::move(data_[data_offset])};
    data_[data_offset] =
        Node{std::make_unique<ObjectType>(std::move(other)), old_first_node};
    ++size_;
    return {*(data_[data_offset].object_), true};
  }

  std::pair<ReturnType&, bool> Insert(ObjectType&& other, IsMulti) {
    RehashWhenNeed();

    std::uint64_t hash_code = GetObjectHash(other);
    LockGuard guard(*this, hash_code, UniqueLockType{});

    std::uint64_t data_offset = hash_code % bucket_count_;
    if (data_[data_offset].object_ == nullptr) {
      data_[data_offset].object_ =
          std::make_unique<ObjectType>(std::move(other));
      ++size_;
      return {*(data_[data_offset].object_), true};
    }

    Node* old_first_node = new Node{std::move(data_[data_offset])};
    data_[data_offset] =
        Node{std::make_unique<ObjectType>(std::move(other)), old_first_node};
    ++size_;

    return {*(data_[data_offset].object_), true};
  }

  template <typename... Args>
  std::pair<ReturnType&, bool> Emplace(NotMulti, Args&&... args) {
    return Insert(ObjectType{std::forward<Args>(args)...}, NotMulti{});
  }

  template <typename... Args>
  std::pair<ReturnType&, bool> Emplace(IsMulti, Args&&... args) {
    return Insert(ObjectType{std::forward<Args>(args)...}, IsMulti{});
  }

  void DeleteOneListNext(Node* node, std::uint32_t& count) {
    if (node->next_node_) {
      DeleteOneListNext(node->next_node_);
    }

    delete node;
    ++count;
  }

  void DeleteOneList(Node* node, std::uint32_t& count) {
    if (node->next_node_) {
      DeleteOneList(node->next_node_, count);
    }

    node->object_.reset();
    node->next_node_ = nullptr;
  }

  void DeleteOneListNext(Node* node) {
    if (node->next_node_) {
      DeleteOneListNext(node->next_node_);
    }

    delete node;
  }

  void DeleteOneList(Node* node) {
    assert(node != nullptr);
    if (node->next_node_) {
      DeleteOneListNext(node->next_node_);
    }

    node->object_.reset();
    node->next_node_ = nullptr;
  }

  std::uint64_t GetObjectHash(const ObjectType& object, IsMap) {
    return Hash{}(object.first());
  }

  std::uint64_t GetObjectHash(const ObjectType& object, IsSet) {
    return Hash{}(object);
  }

  std::uint64_t GetObjectHash(const ObjectType& object) {
    return GetObjectHash(object, RelationshipTrait());
  }

  bool KeyEqual(const ObjectType& lhs, const ObjectType& rhs, IsMap) const {
    return Equal{}(lhs.first, rhs.first);
  }

  bool KeyEqual(const ObjectType& lhs, const ObjectType& rhs, IsSet) const {
    return Equal{}(lhs, rhs);
  }

  bool KeyEqual(const ObjectType& lhs, const ObjectType& rhs) const {
    return KeyEqual(lhs, rhs, RelationshipTrait{});
  }

  bool KeyEqual2(const ObjectType& lhs, const KeyType& rhs, IsMap) const {
    return Equal{}(lhs.first, rhs);
  }

  bool KeyEqual2(const ObjectType& lhs, const KeyType& rhs, IsSet) const {
    return Equal{}(lhs, rhs);
  }

  bool KeyEqual2(const ObjectType& lhs, const KeyType& rhs) const {
    return KeyEqual2(lhs, rhs, RelationshipTrait{});
  }

  void RehashWhenNeed() {
    if (size_ > std::floor(bucket_count_ * load_factor_)) {
      ReHash(2 * bucket_count_);
    }
  }

  void LockAll() const {
    std::lock(data_mutex0_, data_mutex1_, data_mutex2_, data_mutex3_,
              data_mutex4_, data_mutex5_, data_mutex6_, data_mutex7_,
              data_mutex8_, data_mutex9_, data_mutex10_, data_mutex11_,
              data_mutex12_, data_mutex13_, data_mutex14_, data_mutex15_);
  }

  void UnlockAll() const {
    data_mutex0_.unlock();
    data_mutex1_.unlock();
    data_mutex2_.unlock();
    data_mutex3_.unlock();
    data_mutex4_.unlock();
    data_mutex5_.unlock();
    data_mutex6_.unlock();
    data_mutex7_.unlock();
    data_mutex8_.unlock();
    data_mutex9_.unlock();
    data_mutex10_.unlock();
    data_mutex11_.unlock();
    data_mutex12_.unlock();
    data_mutex13_.unlock();
    data_mutex14_.unlock();
    data_mutex15_.unlock();
  }

 private:
  struct LockGuard {
    LockGuard(const ThisType& hash_table, const KeyType& key, TrueType)
        : is_lock_(true),
          is_shared_(true),
          parent_(hash_table),
          shared_guard_(ChooseMutex(key)) {}
    LockGuard(const ThisType& hash_table, const KeyType& key, FalseType)
        : is_lock_(true),
          is_shared_(false),
          parent_(hash_table),
          unique_guard_(ChooseMutex(key)) {}
    LockGuard(const ThisType& hash_table, const KeyType& key, TrueType,
              std::adopt_lock_t)
        : is_lock_(true),
          is_shared_(true),
          parent_(hash_table),
          shared_guard_(ChooseMutex(key), std::adopt_lock) {}
    LockGuard(const ThisType& hash_table, std::uint64_t hash_code, FalseType,
              std::adopt_lock_t)
        : is_lock_(true),
          is_shared_(false),
          parent_(hash_table),
          unique_guard_(ChooseMutex(hash_code), std::adopt_lock) {}
    LockGuard(const ThisType& hash_table, std::uint64_t hash_code, TrueType)
        : is_lock_(true),
          is_shared_(true),
          parent_(hash_table),
          shared_guard_(ChooseMutex(hash_code)) {}
    LockGuard(const ThisType& hash_table, std::uint64_t hash_code, FalseType)
        : is_lock_(true),
          is_shared_(false),
          parent_(hash_table),
          unique_guard_(ChooseMutex(hash_code)) {}
    LockGuard(const ThisType& hash_table, std::uint64_t hash_code, TrueType,
              std::adopt_lock_t)
        : is_lock_(true),
          is_shared_(true),
          parent_(hash_table),
          shared_guard_(ChooseMutex(hash_code), std::adopt_lock) {}
    LockGuard(const ThisType& hash_table, const KeyType& key, FalseType,
              std::adopt_lock_t)
        : is_lock_(true),
          is_shared_(false),
          parent_(hash_table),
          unique_guard_(ChooseMutex(key), std::adopt_lock) {}

    ~LockGuard() = default;
    LockGuard(const LockGuard& other) = delete;
    LockGuard(LockGuard&& other) = delete;
    LockGuard& operator=(const LockGuard& other) = delete;
    LockGuard& operator=(LockGuard&& other) = delete;

    void Lock() {
      if (!is_lock_) {
        is_lock_ = true;
        if (is_shared_) {
          shared_guard_.lock();
        } else {
          unique_guard_.lock();
        }
      }
    }

    MutexType& ChooseMutex(std::uint64_t hash_code) {
      switch (hash_code & 0xF) {
        case 0:
          return parent_.data_mutex0_;
        case 1:
          return parent_.data_mutex1_;
        case 2:
          return parent_.data_mutex2_;
        case 3:
          return parent_.data_mutex3_;
        case 4:
          return parent_.data_mutex4_;
        case 5:
          return parent_.data_mutex5_;
        case 6:
          return parent_.data_mutex6_;
        case 7:
          return parent_.data_mutex7_;
        case 8:
          return parent_.data_mutex8_;
        case 9:
          return parent_.data_mutex9_;
        case 10:
          return parent_.data_mutex10_;
        case 11:
          return parent_.data_mutex11_;
        case 12:
          return parent_.data_mutex12_;
        case 13:
          return parent_.data_mutex13_;
        case 14:
          return parent_.data_mutex14_;
        case 15:
          return parent_.data_mutex15_;
        default:
          assert(false);
          return parent_.data_mutex0_;
      }
    }

    MutexType& ChooseMutex(const KeyType& key) {
      return ChooseMutex(Hash{}(key));
    }

    void Unlock() {
      if (is_lock_) {
        is_lock_ = false;
        if (is_shared_) {
          shared_guard_.unlock();
        } else {
          unique_guard_.unlock();
        }
      }
    }

    bool is_lock_{false};
    bool is_shared_{false};
    const ThisType& parent_;
    std::unique_lock<MutexType> unique_guard_;
    std::shared_lock<MutexType> shared_guard_;
  };

  // unique_lock
  struct LockAllGuard {
    explicit LockAllGuard(const ThisType& hash_table)
        : parent_(hash_table), is_lock_(false) {
      LockAll();
    }

    LockAllGuard(const ThisType& hash_table, std::adopt_lock_t)
        : parent_(hash_table), is_lock_(true) {}

    ~LockAllGuard() {
      if (is_lock_) {
        parent_.UnlockAll();
      }
    }

    LockAllGuard(const LockGuard& other) = delete;
    LockAllGuard(LockGuard&& other) = delete;
    LockAllGuard& operator=(const LockGuard& other) = delete;
    LockAllGuard& operator=(LockGuard&& other) = delete;

    void LockAll() {
      if (!is_lock_) {
        parent_.LockAll();
        is_lock_ = true;
      }
    }

    void UnlockAll() {
      if (is_lock_) {
        parent_.UnlockAll();
        is_lock_ = false;
      }
    }

    bool is_lock_{false};
    const ThisType& parent_;
  };

  struct Node {
    Node() = default;
    Node(std::unique_ptr<ObjectType>&& object, Node* next_node)
        : object_(std::move(object)), next_node_(next_node) {}
    Node(const Node& other) = delete;
    Node(Node&& other) noexcept
        : object_(other.object_.release()), next_node_(other.next_node_) {
      other.next_node_ = nullptr;
    }
    Node& operator=(const Node& other) = delete;
    Node& operator=(Node&& other) noexcept {
      if (&other == this) {
        return *this;
      }

      object_.reset(other.object_.release());
      next_node_ = other.next_node_;

      other.next_node_ = nullptr;

      return *this;
    }
    ~Node() = default;

    std::unique_ptr<ObjectType> object_{nullptr};
    Node* next_node_{nullptr};
  };

 private:
  Node* data_{new Node[131]{}};

  double load_factor_{0.75f};
  std::uint64_t size_{0};
  std::uint64_t bucket_count_{131};

  mutable MutexType data_mutex0_{};
  mutable MutexType data_mutex1_{};
  mutable MutexType data_mutex2_{};
  mutable MutexType data_mutex3_{};
  mutable MutexType data_mutex4_{};
  mutable MutexType data_mutex5_{};
  mutable MutexType data_mutex6_{};
  mutable MutexType data_mutex7_{};
  mutable MutexType data_mutex8_{};
  mutable MutexType data_mutex9_{};
  mutable MutexType data_mutex10_{};
  mutable MutexType data_mutex11_{};
  mutable MutexType data_mutex12_{};
  mutable MutexType data_mutex13_{};
  mutable MutexType data_mutex14_{};
  mutable MutexType data_mutex15_{};
};

template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ObjectType>>
using ConcurrentSet = HashTable<FalseType, FalseType, ObjectType, ObjectType,
                                const ObjectType, Hash, Equal, Allocator>;

template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ObjectType>>
using ConcurrentMultiSet =
    HashTable<FalseType, TrueType, ObjectType, ObjectType, const ObjectType,
              Hash, Equal, Allocator>;

template <typename KeyObject, typename ValueObject,
          typename Hash = std::hash<KeyObject>,
          typename Equal = std::equal_to<KeyObject>,
          typename Allocator =
              std::allocator<std::pair<const KeyObject, ValueObject>>>
using ConcurrentMap =
    HashTable<TrueType, FalseType, KeyObject,
              std::pair<const KeyObject, ValueObject>,
              std::pair<const KeyObject, ValueObject>, Hash, Equal, Allocator>;

template <typename KeyObject, typename ValueObject,
          typename Hash = std::hash<KeyObject>,
          typename Equal = std::equal_to<KeyObject>,
          typename Allocator =
              std::allocator<std::pair<const KeyObject, ValueObject>>>
using ConcurrentMultiMap =
    HashTable<TrueType, TrueType, KeyObject,
              std::pair<const KeyObject, ValueObject>,
              std::pair<const KeyObject, ValueObject>, Hash, Equal, Allocator>;
// template <typename ObjectType, typename Hash = std::hash<ObjectType>,
//           typename Equal = std::equal_to<ObjectType>,
//           typename Allocator = std::allocator<ObjectType>>
// class ConcurrentSet final
//     : public HashTable<FalseType, FalseType, ObjectType, ObjectType, Hash,
//                        Equal, Allocator> {
//  public:
//   using BaseType = HashTable<FalseType, FalseType, ObjectType, ObjectType,
//   Hash,
//                              Equal, Allocator>;
//
//  public:
//   ConcurrentSet() : BaseType() {}
//   ~ConcurrentSet() = default;
//   explicit ConcurrentSet(std::uint64_t size) : BaseType(size){};
//   ConcurrentSet(const ConcurrentSet& other) = default;
//   ConcurrentSet(ConcurrentSet&& other) noexcept = default;
//   ConcurrentSet& operator=(const ConcurrentSet& other) {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(other);
//
//     return *this;
//   }
//   ConcurrentSet& operator=(ConcurrentSet&& other) noexcept {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(std::move(other));
//
//     return *this;
//   }
//
//  public:
//   std::pair<const ObjectType&, bool> Insert(const ObjectType& object) {
//     return BaseType::Insert(object);
//   }
//
//   std::pair<const ObjectType&, bool> Insert(ObjectType&& object) {
//     return BaseType::Insert(std::move(object));
//   }
//
//   template <typename... Args>
//   std::pair<const ObjectType&, bool> Emplace(Args... args) {
//     return BaseType::Emplace(std::forward<Args>(args)...);
//   }
//
//   const ObjectType* Find(const ObjectType& key) { return BaseType::Find(key);
//   }
//
//   std::vector<const ObjectType*> EqualRange(const ObjectType& key) {
//     return BaseType::EqualRange(key);
//   }
// };
//
// template <typename ObjectType, typename Hash = std::hash<ObjectType>,
//           typename Equal = std::equal_to<ObjectType>,
//           typename Allocator = std::allocator<ObjectType>>
// class ConcurrentMultiSet final
//     : public HashTable<FalseType, TrueType, ObjectType, ObjectType, Hash,
//     Equal,
//                        Allocator> {
//  public:
//   using BaseType = HashTable<FalseType, TrueType, ObjectType, ObjectType,
//   Hash,
//                              Equal, Allocator>;
//
//  public:
//   ConcurrentMultiSet() : BaseType() {}
//   ~ConcurrentMultiSet() = default;
//   explicit ConcurrentMultiSet(std::uint64_t size) : BaseType(size){};
//   ConcurrentMultiSet(const ConcurrentMultiSet& other) = default;
//   ConcurrentMultiSet(ConcurrentMultiSet&& other) noexcept = default;
//   ConcurrentMultiSet& operator=(const ConcurrentMultiSet& other) {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(other);
//
//     return *this;
//   }
//   ConcurrentMultiSet& operator=(ConcurrentMultiSet&& other) noexcept {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(std::move(other));
//
//     return *this;
//   }
//
//  public:
//   std::pair<const ObjectType&, bool> Insert(const ObjectType& object) {
//     return BaseType::Insert(object);
//   }
//
//   std::pair<const ObjectType&, bool> Insert(ObjectType&& object) {
//     return BaseType::Insert(std::move(object));
//   }
//
//   template <typename... Args>
//   std::pair<const ObjectType&, bool> Emplace(Args... args) {
//     return BaseType::Emplace(std::forward<Args>(args)...);
//   }
//
//   const ObjectType* Find(const ObjectType& key) { return BaseType::Find(key);
//   }
//
//   std::vector<const ObjectType*> EqualRange(const ObjectType& key) {
//     return BaseType::EqualRange(key);
//   }
// };
//
// template <typename KeyType, typename ValueType,
//           typename Hash = std::hash<KeyType>,
//           typename Equal = std::equal_to<KeyType>,
//           typename Allocator = std::allocator<std::pair<KeyType, ValueType>>>
// class ConcurrentMap final
//     : public HashTable<TrueType, FalseType, KeyType,
//                        std::pair<const KeyType, ValueType>, Hash, Equal,
//                        Allocator> {
//  public:
//   using BaseType = HashTable<TrueType, FalseType, KeyType, ValueType, Hash,
//                              Equal, Allocator>;
//   using ObjectType = std::pair<const KeyType, ValueType>;
//
//  public:
//   ConcurrentMap() : BaseType() {}
//   ~ConcurrentMap() = default;
//   explicit ConcurrentMap(std::uint64_t size) : BaseType(size){};
//   ConcurrentMap(const ConcurrentMap& other) = default;
//   ConcurrentMap(ConcurrentMap&& other) noexcept = default;
//   ConcurrentMap& operator=(const ConcurrentMap& other) {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(other);
//
//     return *this;
//   }
//   ConcurrentMap& operator=(ConcurrentMap&& other) noexcept {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(std::move(other));
//
//     return *this;
//   }
// };
//
// template <typename KeyType, typename ValueType,
//           typename Hash = std::hash<KeyType>,
//           typename Equal = std::equal_to<KeyType>,
//           typename Allocator = std::allocator<std::pair<KeyType, ValueType>>>
// class ConcurrentMultiMap final
//     : public HashTable<TrueType, TrueType, KeyType,
//                        std::pair<const KeyType, ValueType>, Hash, Equal,
//                        Allocator> {
//  public:
//   using BaseType =
//       HashTable<TrueType, TrueType, KeyType, ValueType, Hash, Equal,
//       Allocator>;
//
//  public:
//   ConcurrentMultiMap() : BaseType() {}
//   ~ConcurrentMultiMap() = default;
//   explicit ConcurrentMultiMap(std::uint64_t size) : BaseType(size){};
//   ConcurrentMultiMap(const ConcurrentMultiMap& other) = default;
//   ConcurrentMultiMap(ConcurrentMultiMap&& other) noexcept = default;
//   ConcurrentMultiMap& operator=(const ConcurrentMultiMap& other) {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(other);
//
//     return *this;
//   }
//   ConcurrentMultiMap& operator=(ConcurrentMultiMap&& other) noexcept {
//     if (&other == this) {
//       return *this;
//     }
//
//     BaseType::operator=(std::move(other));
//
//     return *this;
//   }
// };

}  // namespace Utils
}  // namespace MM
