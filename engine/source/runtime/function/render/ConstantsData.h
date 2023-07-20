//
// Created by beimingxianyu on 23-7-20.
//
#pragma once

#include <cstdint>
#include <memory>

namespace MM {
namespace RenderSystem {

template <typename ConstantType>
class ConstantsData {
 public:
  ConstantsData() = default;
  ~ConstantsData() = default;
  ConstantsData(ConstantType* data_ptr, std::uint64_t offset,
                std::uint64_t size)
      : data_ptr_(data_ptr), offset_(offset), size_(size) {}
  ConstantsData(const ConstantsData& other) = default;
  ConstantsData(ConstantsData&& other) noexcept
      : data_ptr_(other.data_ptr_), offset_(other.offset_), size_(other.size_) {
    other.Reset();
  }
  ConstantsData& operator=(const ConstantsData& other) {
    if (std::addressof(other) == this) {
      return *this;
    }

    data_ptr_ = other.data_ptr_;
    offset_ = other.offset_;
    size_ = other.size_;

    return *this;
  }
  ConstantsData& operator=(ConstantsData&& other) {
    if (std::addressof(other) == this) {
      return *this;
    }

    data_ptr_ = other.data_ptr_;
    offset_ = other.offset_;
    size_ = other.size_;

    other.Reset();

    return *this;
  }

 public:
  ConstantType* GetDataPtr() { return data_ptr_; }

  const ConstantType* GetDataPtr() const { return data_ptr_; }

  uint64_t GetOffset() const { return offset_; }

  uint64_t GetSize() const { return size_; }

  void Reset() {
    data_ptr_ = nullptr;
    offset_ = 0;
    size_ = 0;
  }

  bool IsValid() const { return data_ptr_ != nullptr && size_ != 0; }

 private:
  ConstantType* data_ptr_{nullptr};
  std::uint64_t offset_{0};
  std::uint64_t size_{0};
};
}  // namespace RenderSystem
}  // namespace MM
