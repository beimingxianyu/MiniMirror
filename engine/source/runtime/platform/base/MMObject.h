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
  bool operator==(const MMObject& rhs) const;
  bool operator!=(const MMObject& rhs) const;
  bool operator<(const MMObject& rhs) const;
  bool operator>(const MMObject& rhs) const;
  bool operator<=(const MMObject& rhs) const;
  bool operator>=(const MMObject& rhs) const;
  static void Swap(MMObject& lhs, MMObject& rhs) noexcept;
  static void swap(MMObject& lhs, MMObject& rhs) noexcept;

 public:
  virtual bool IsValid() const;

  MM::Utils::GUID GetGuid() const;

 protected:
  virtual void Reset();

 private:
  MM::Utils::GUID guid_{};
};
}  // namespace MM