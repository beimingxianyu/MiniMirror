#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

namespace MM {
namespace Utils {
class ID2 {
 public:
  ID2() = default;
  ID2(std::uint64_t sub_ID1, std::uint64_t sub_ID2);
  ID2(const ID2& other) = default;
  ID2(ID2&& other) noexcept = default;
  ID2& operator=(const ID2& other) = default;
  ID2& operator=(ID2&& other) noexcept = default;

 public:
  bool operator==(const ID2& rhs) const;

  bool operator!=(const ID2& rhs) const;

  bool operator<(const ID2& rhs) const;

  bool operator>(const ID2& rhs) const;

  bool operator<=(const ID2& rhs) const;

  bool operator>=(const ID2& rhs) const;

  friend void Swap(ID2& lhs, ID2& rhs) noexcept;

  friend void swap(ID2& lhs, ID2& rhs) noexcept;

 public:
  std::uint64_t GetSubID1() const;

  std::uint64_t GetSubID2() const;

  void SetSubID1(std::uint64_t new_sub_ID1);

  void SetSubID2(std::uint64_t new_sub_ID2);

  std::uint64_t GetHashCode() const;

  void Reset();

 private:
  std::uint64_t sub_ID1_{0};
  std::uint64_t sub_ID2_{0};
};

class ID3 {
 public:
  ID3() = default;
  ID3(std::uint64_t sub_ID1, std::uint64_t sub_ID2, std::uint64_t sub_ID3);
  ID3(const ID3& other) = default;
  ID3(ID3&& other) noexcept = default;
  ID3& operator=(const ID3& other) = default;
  ID3& operator=(ID3&& other) noexcept = default;

 public:
  bool operator==(const ID3& rhs) const;

  bool operator!=(const ID3& rhs) const;

  bool operator<(const ID3& rhs) const;

  bool operator>(const ID3& rhs) const;

  bool operator<=(const ID3& rhs) const;

  bool operator>=(const ID3& rhs) const;

  friend void Swap(ID3& lhs, ID3& rhs) noexcept;

  friend void swap(ID3& lhs, ID3& rhs) noexcept;

 public:
  std::uint64_t GetSubID1() const;

  std::uint64_t GetSubID2() const;

  std::uint64_t GetSubID3() const;

  void SetSubID1(std::uint64_t new_sub_ID1);

  void SetSubID2(std::uint64_t new_sub_ID2);

  void SetSubID3(std::uint64_t new_sub_ID3);

  std::uint64_t GetHashCode() const;

  void Reset();

 private:
  std::uint64_t sub_ID1_{0};
  std::uint64_t sub_ID2_{0};
  std::uint64_t sub_ID3_{0};
};

class ID4 {
 public:
  ID4() = default;
  ID4(std::uint64_t sub_ID1, std::uint64_t sub_ID2, std::uint64_t sub_ID3,
      std::uint64_t sub_ID4);
  ID4(const ID4& other) = default;
  ID4(ID4&& other) noexcept = default;
  ID4& operator=(const ID4& other) = default;
  ID4& operator=(ID4&& other) noexcept = default;

 public:
  bool operator==(const ID4& rhs) const;

  bool operator!=(const ID4& rhs) const;

  bool operator<(const ID4& rhs) const;

  bool operator>(const ID4& rhs) const;

  bool operator<=(const ID4& rhs) const;

  bool operator>=(const ID4& rhs) const;

  friend void Swap(ID4& lhs, ID4& rhs) noexcept;

  friend void swap(ID4& lhs, ID4& rhs) noexcept;

 public:
  std::uint64_t GetSubID1() const;

  std::uint64_t GetSubID2() const;

  std::uint64_t GetSubID3() const;

  std::uint64_t GetSubID4() const;

  void SetSubID1(std::uint64_t new_sub_ID1);

  void SetSubID2(std::uint64_t new_sub_ID2);

  void SetSubID3(std::uint64_t new_sub_ID3);

  void SetSubID4(std::uint64_t new_sub_ID4);

  std::uint64_t GetHashCode() const;

  void Reset();

 private:
  std::uint64_t sub_ID1_{0};
  std::uint64_t sub_ID2_{0};
  std::uint64_t sub_ID3_{0};
  std::uint64_t sub_ID4_{0};
};

class ID5 {
 public:
  ID5() = default;
  ID5(std::uint64_t sub_ID1, std::uint64_t sub_ID2, std::uint64_t sub_ID3,
      std::uint64_t sub_ID4, std::uint64_t sub_ID5);
  ID5(const ID5& other) = default;
  ID5(ID5&& other) noexcept = default;
  ID5& operator=(const ID5& other) = default;
  ID5& operator=(ID5&& other) noexcept = default;

 public:
  bool operator==(const ID5& rhs) const;

  bool operator!=(const ID5& rhs) const;

  bool operator<(const ID5& rhs) const;

  bool operator>(const ID5& rhs) const;

  bool operator<=(const ID5& rhs) const;

  bool operator>=(const ID5& rhs) const;

  friend void Swap(ID5& lhs, ID5& rhs) noexcept;

  friend void swap(ID5& lhs, ID5& rhs) noexcept;

 public:
  std::uint64_t GetSubID1() const;

  std::uint64_t GetSubID2() const;

  std::uint64_t GetSubID3() const;

  std::uint64_t GetSubID4() const;

  std::uint64_t GetSubID5() const;

  void SetSubID1(std::uint64_t new_sub_ID1);

  void SetSubID2(std::uint64_t new_sub_ID2);

  void SetSubID3(std::uint64_t new_sub_ID3);

  void SetSubID4(std::uint64_t new_sub_ID4);

  void SetSubID5(std::uint64_t new_sub_ID5);

  std::uint64_t GetHashCode() const;

  void Reset();

 private:
  std::uint64_t sub_ID1_{0};
  std::uint64_t sub_ID2_{0};
  std::uint64_t sub_ID3_{0};
  std::uint64_t sub_ID4_{0};
  std::uint64_t sub_ID5_{0};
};

class ID6 {
 public:
  ID6() = default;
  ID6(std::uint64_t sub_id1, std::uint64_t sub_id2, std::uint64_t sub_id3,
      std::uint64_t sub_id4, std::uint64_t sub_id5, std::uint64_t sub_id6);
  ID6(const ID6& other) = default;
  ID6(ID6&& other) noexcept = default;
  ID6& operator=(const ID6& other) = default;
  ID6& operator=(ID6&& other) noexcept = default;

 public:
  bool operator==(const ID6& rhs) const;

  bool operator!=(const ID6& rhs) const;

  bool operator<(const ID6& rhs) const;

  bool operator>(const ID6& rhs) const;

  bool operator<=(const ID6& rhs) const;

  bool operator>=(const ID6& rhs) const;

  friend void Swap(ID6& lhs, ID6& rhs) noexcept;

  friend void swap(ID6& lhs, ID6& rhs) noexcept;

 public:
  std::uint64_t GetSubID1() const;

  std::uint64_t GetSubID2() const;

  std::uint64_t GetSubID3() const;

  std::uint64_t GetSubID4() const;

  std::uint64_t GetSubID5() const;

  std::uint64_t GetSubID6() const;

  void SetSubID1(std::uint64_t sub_ID1);

  void SetSubID2(std::uint64_t sub_iD2);

  void SetSubID3(std::uint64_t sub_iD3);

  void SetSubID4(std::uint64_t sub_iD4);

  void SetSubID5(std::uint64_t sub_iD5);

  void SetSubID6(std::uint64_t sub_iD6);

  std::uint64_t GetHashCode() const;

  void Reset();

 private:
  std::uint64_t sub_ID1_{0};
  std::uint64_t sub_ID2_{0};
  std::uint64_t sub_ID3_{0};
  std::uint64_t sub_ID4_{0};
  std::uint64_t sub_ID5_{0};
  std::uint64_t sub_ID6_{0};
};

class ID7 {
 public:
  ID7() = default;
  ID7(std::uint64_t sub_ID1, std::uint64_t sub_ID2, std::uint64_t sub_ID3,
      std::uint64_t sub_ID4, std::uint64_t sub_ID5, std::uint64_t sub_ID6,
      std::uint64_t sub_ID7);
  ID7(const ID7& other) = default;
  ID7(ID7&& other) noexcept = default;
  ID7& operator=(const ID7& other) = default;
  ID7& operator=(ID7&& other) noexcept = default;

 public:
  bool operator==(const ID7& rhs) const;

  bool operator!=(const ID7& rhs) const;

  bool operator<(const ID7& rhs) const;

  bool operator>(const ID7& rhs) const;

  bool operator<=(const ID7& rhs) const;

  bool operator>=(const ID7& rhs) const;

  friend void Swap(ID7& lhs, ID7& rhs) noexcept;

  friend void swap(ID7& lhs, ID7& rhs) noexcept;

 public:
  std::uint64_t GetSubID1() const;

  std::uint64_t GetSubID2() const;

  std::uint64_t GetSubID3() const;

  std::uint64_t GetSubID4() const;

  std::uint64_t GetSubID5() const;

  std::uint64_t GetSubID6() const;

  std::uint64_t GetSubID7() const;

  void SetSubID1(std::uint64_t sub_iD1);

  void SetSubID2(std::uint64_t sub_iD2);

  void SetSubID3(std::uint64_t sub_iD3);

  void SetSubID4(std::uint64_t sub_iD4);

  void SetSubID5(std::uint64_t sub_iD5);

  void SetSubID6(std::uint64_t sub_iD6);

  void SetSubID7(std::uint64_t sub_iD7);

  std::uint64_t GetHashCode() const;

  void Reset();

 private:
  std::uint64_t sub_ID1_{0};
  std::uint64_t sub_ID2_{0};
  std::uint64_t sub_ID3_{0};
  std::uint64_t sub_ID4_{0};
  std::uint64_t sub_ID5_{0};
  std::uint64_t sub_ID6_{0};
  std::uint64_t sub_ID7_{0};
};

class ID8 {
 public:
  ID8() = default;
  ID8(std::uint64_t sub_ID1, std::uint64_t sub_ID2, std::uint64_t sub_ID3,
      std::uint64_t sub_ID4, std::uint64_t sub_ID5, std::uint64_t sub_ID6,
      std::uint64_t sub_ID7, std::uint64_t sub_ID8);
  ID8(const ID8& other) = default;
  ID8(ID8&& other) noexcept = default;
  ID8& operator=(const ID8& other) = default;
  ID8& operator=(ID8&& other) noexcept = default;

 public:
  bool operator==(const ID8& rhs) const;

  bool operator!=(const ID8& rhs) const;

  bool operator<(const ID8& rhs) const;

  bool operator>(const ID8& rhs) const;

  bool operator<=(const ID8& rhs) const;

  bool operator>=(const ID8& rhs) const;

  friend void Swap(ID8& lhs, ID8& rhs) noexcept;

  friend void swap(ID8& lhs, ID8& rhs) noexcept;

 public:
  std::uint64_t GetSubID1() const;

  std::uint64_t GetSubID2() const;

  std::uint64_t GetSubID3() const;

  std::uint64_t GetSubID4() const;

  std::uint64_t GetSubID5() const;

  std::uint64_t GetSubID6() const;

  std::uint64_t GetSubID7() const;

  std::uint64_t GetSubID8() const;

  void SetSubID1(std::uint64_t sub_iD1);

  void SetSubID2(std::uint64_t sub_iD2);

  void SetSubID3(std::uint64_t sub_iD3);

  void SetSubID4(std::uint64_t sub_iD4);

  void SetSubID5(std::uint64_t sub_iD5);

  void SetSubID6(std::uint64_t sub_iD6);

  void SetSubID7(std::uint64_t sub_iD7);

  void SetSubID8(std::uint64_t sub_iD8);

  std::uint64_t GetHashCode() const;

  void Reset();

 private:
  std::uint64_t sub_ID1_{0};
  std::uint64_t sub_ID2_{0};
  std::uint64_t sub_ID3_{0};
  std::uint64_t sub_ID4_{0};
  std::uint64_t sub_ID5_{0};
  std::uint64_t sub_ID6_{0};
  std::uint64_t sub_ID7_{0};
  std::uint64_t sub_ID8_{0};
};
}  // namespace Utils
}  // namespace MM

template <>
struct std::hash<MM::Utils::ID2> {
  std::uint64_t operator()(const MM::Utils::ID2& object) const {
    return object.GetHashCode();
  }
};

template <>
struct std::hash<MM::Utils::ID3> {
  std::uint64_t operator()(const MM::Utils::ID3& object) const {
    return object.GetHashCode();
  }
};

template <>
struct std::hash<MM::Utils::ID4> {
  std::uint64_t operator()(const MM::Utils::ID4& object) const {
    return object.GetHashCode();
  }
};

template <>
struct std::hash<MM::Utils::ID5> {
  std::uint64_t operator()(const MM::Utils::ID5& object) const {
    return object.GetHashCode();
  }
};

template <>
struct std::hash<MM::Utils::ID6> {
  std::uint64_t operator()(const MM::Utils::ID6& object) const {
    return object.GetHashCode();
  }
};

template <>
struct std::hash<MM::Utils::ID7> {
  std::uint64_t operator()(const MM::Utils::ID7& object) const {
    return object.GetHashCode();
  }
};

template <>
struct std::hash<MM::Utils::ID8> {
  std::uint64_t operator()(const MM::Utils::ID8& object) const {
    return object.GetHashCode();
  }
};
