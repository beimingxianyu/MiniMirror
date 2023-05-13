#pragma once

#include "utils/uuid.h"

namespace MM {
class MMObject {
 public:
  MMObject() = default;
  ~MMObject() = default;
  MMObject(const MMObject& other) = default;
  MMObject(MMObject&& other) noexcept = default;
  MMObject& operator=(const MMObject& other);
  MMObject& operator=(MMObject&& other) noexcept;

 public:
  friend bool operator==(const MMObject& lhs, const MMObject& rhs);
  friend bool operator!=(const MMObject& lhs, const MMObject& rhs);
  friend bool operator<(const MMObject& lhs, const MMObject& rhs);
  friend bool operator>(const MMObject& lhs, const MMObject& rhs);
  friend bool operator<=(const MMObject& lhs, const MMObject& rhs);
  friend bool operator>=(const MMObject& lhs, const MMObject& rhs);
  friend void Swap(MMObject& lhs, MMObject& rhs) noexcept;
  friend void swap(MMObject& lhs, MMObject& rhs) noexcept;

 public:
  virtual bool IsValid() const;

  MM::Utils::GUID GetGuid() const;

 private:
  MM::Utils::GUID guid_{};
};
}  // namespace MM