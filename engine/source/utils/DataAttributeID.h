#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

namespace MM {
namespace Utils {
class DataAttributeID2 {
 public:
  DataAttributeID2() = default;
  DataAttributeID2(std::uint64_t resource_attribute1,
                   std::uint64_t resource_attribute2);
  DataAttributeID2(const DataAttributeID2& other) = default;
  DataAttributeID2(DataAttributeID2&& other) noexcept = default;
  DataAttributeID2& operator=(const DataAttributeID2& other) = default;
  DataAttributeID2& operator=(DataAttributeID2&& other) noexcept = default;

 public:
  bool operator==(const DataAttributeID2& rhs) const;

  bool operator!=(const DataAttributeID2& rhs) const;

  bool operator<(const DataAttributeID2& rhs) const;

  bool operator>(const DataAttributeID2& rhs) const;

  bool operator<=(const DataAttributeID2& rhs) const;

  bool operator>=(const DataAttributeID2& rhs) const;

  friend void Swap(DataAttributeID2& lhs, DataAttributeID2& rhs) noexcept;

  friend void swap(DataAttributeID2& lhs, DataAttributeID2& rhs) noexcept;

 public:
  std::uint64_t GetResourceAttribute1() const;

  std::uint64_t GetResourceAttribute2() const;

  void SetAttribute1(std::uint64_t new_resource_attribute1);

  void SetAttribute2(std::uint64_t new_resource_attribute2);

  std::uint64_t GetHashCode() const;

 private:
  std::uint64_t resource_attribute1_{0};
  std::uint64_t resource_attribute2_{0};
};

class DataAttributeID3 {
 public:
  DataAttributeID3() = default;
  DataAttributeID3(std::uint64_t resource_attribute1,
                   std::uint64_t resource_attribute2,
                   std::uint64_t resource_attribute3);
  DataAttributeID3(const DataAttributeID3& other) = default;
  DataAttributeID3(DataAttributeID3&& other) noexcept = default;
  DataAttributeID3& operator=(const DataAttributeID3& other) = default;
  DataAttributeID3& operator=(DataAttributeID3&& other) noexcept = default;

 public:
  bool operator==(const DataAttributeID3& rhs) const;

  bool operator!=(const DataAttributeID3& rhs) const;

  bool operator<(const DataAttributeID3& rhs) const;

  bool operator>(const DataAttributeID3& rhs) const;

  bool operator<=(const DataAttributeID3& rhs) const;

  bool operator>=(const DataAttributeID3& rhs) const;

  friend void Swap(DataAttributeID3& lhs, DataAttributeID3& rhs) noexcept;

  friend void swap(DataAttributeID3& lhs, DataAttributeID3& rhs) noexcept;

 public:
  std::uint64_t GetResourceAttribute1() const;

  std::uint64_t GetResourceAttribute2() const;

  std::uint64_t GetResourceAttribute3() const;

  void SetAttribute1(std::uint64_t new_resource_attribute1);

  void SetAttribute2(std::uint64_t new_resource_attribute2);

  void SetAttribute3(std::uint64_t new_resource_attribute3);

  std::uint64_t GetHashCode() const;

 private:
  std::uint64_t resource_attribute1_{0};
  std::uint64_t resource_attribute2_{0};
  std::uint64_t resoutce_attribute3_{0};
};
}  // namespace Utils
}  // namespace MM

template <>
struct std::hash<MM::Utils::DataAttributeID2> {
  std::uint64_t operator()(const MM::Utils::DataAttributeID2& object) const {
    return object.GetHashCode();
  }
};

template <>
struct std::hash<MM::Utils::DataAttributeID3> {
  std::uint64_t operator()(const MM::Utils::DataAttributeID3& object) const {
    return object.GetHashCode();
  }
};
