//
// Created by beimingxianyu on 23-6-30.
//

#include "utils/ID.h"

MM::Utils::ID2::ID2(std::uint64_t sub_ID1, std::uint64_t sub_ID2)
    : sub_ID1_(sub_ID1), sub_ID2_(sub_ID2) {}

bool MM::Utils::ID2::operator==(const MM::Utils::ID2& rhs) const {
  return sub_ID1_ == rhs.sub_ID1_ && sub_ID2_ == rhs.sub_ID2_;
}

bool MM::Utils::ID2::operator!=(const MM::Utils::ID2& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::ID2::operator<(const MM::Utils::ID2& rhs) const {
  if (sub_ID1_ < rhs.sub_ID1_) return true;
  if (rhs.sub_ID1_ < sub_ID1_) return false;
  return sub_ID2_ < rhs.sub_ID2_;
}

bool MM::Utils::ID2::operator>(const MM::Utils::ID2& rhs) const {
  return rhs < *this;
}

bool MM::Utils::ID2::operator<=(const MM::Utils::ID2& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::ID2::operator>=(const MM::Utils::ID2& rhs) const {
  return !(*this < rhs);
}

void MM::Utils::ID2::Swap(MM::Utils::ID2& lhs, MM::Utils::ID2& rhs) noexcept {
  using std::swap;

  std::swap(lhs.sub_ID1_, rhs.sub_ID1_);
  std::swap(lhs.sub_ID2_, rhs.sub_ID2_);
}

void MM::Utils::ID2::swap(MM::Utils::ID2& lhs, MM::Utils::ID2& rhs) noexcept {
  using std::swap;

  std::swap(lhs.sub_ID1_, rhs.sub_ID1_);
  std::swap(lhs.sub_ID2_, rhs.sub_ID2_);
}

std::uint64_t MM::Utils::ID2::GetSubID1() const { return sub_ID1_; }

std::uint64_t MM::Utils::ID2::GetSubID2() const { return sub_ID2_; }

void MM::Utils::ID2::SetSubID1(std::uint64_t new_sub_ID1) {
  sub_ID1_ = new_sub_ID1;
}

void MM::Utils::ID2::SetSubID2(std::uint64_t new_sub_ID2) {
  sub_ID2_ = new_sub_ID2;
}

std::uint64_t MM::Utils::ID2::GetHashCode() const {
  return sub_ID1_ ^ sub_ID2_;
}

void MM::Utils::ID2::Reset() {
  sub_ID1_ = 0;
  sub_ID2_ = 0;
}

MM::Utils::ID3::ID3(std::uint64_t sub_ID1, std::uint64_t sub_ID2,
                    std::uint64_t sub_ID3)
    : sub_ID1_(sub_ID1), sub_ID2_(sub_ID2), sub_ID3_(sub_ID3) {}

bool MM::Utils::ID3::operator==(const MM::Utils::ID3& rhs) const {
  return sub_ID1_ == rhs.sub_ID1_ && sub_ID2_ == rhs.sub_ID2_ &&
         sub_ID3_ == rhs.sub_ID3_;
}

bool MM::Utils::ID3::operator!=(const MM::Utils::ID3& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::ID3::operator<(const MM::Utils::ID3& rhs) const {
  if (sub_ID1_ < rhs.sub_ID1_) return true;
  if (rhs.sub_ID1_ < sub_ID1_) return false;
  if (sub_ID2_ < rhs.sub_ID2_) return true;
  if (rhs.sub_ID2_ < sub_ID2_) return false;
  return sub_ID3_ < rhs.sub_ID3_;
}

bool MM::Utils::ID3::operator>(const MM::Utils::ID3& rhs) const {
  return rhs < *this;
}

bool MM::Utils::ID3::operator<=(const MM::Utils::ID3& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::ID3::operator>=(const MM::Utils::ID3& rhs) const {
  return !(*this < rhs);
}

std::uint64_t MM::Utils::ID3::GetSubID1() const { return sub_ID1_; }

std::uint64_t MM::Utils::ID3::GetSubID2() const { return sub_ID2_; }

std::uint64_t MM::Utils::ID3::GetSubID3() const { return sub_ID3_; }

void MM::Utils::ID3::SetSubID1(std::uint64_t new_sub_ID1) {
  sub_ID1_ = new_sub_ID1;
}

void MM::Utils::ID3::SetSubID2(std::uint64_t new_sub_ID2) {
  sub_ID2_ = new_sub_ID2;
}

void MM::Utils::ID3::SetSubID3(std::uint64_t new_sub_ID3) {
  sub_ID3_ = new_sub_ID3;
}

void MM::Utils::ID3::swap(MM::Utils::ID3& lhs, MM::Utils::ID3& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.sub_ID1_, rhs.sub_ID1_);
  swap(lhs.sub_ID2_, rhs.sub_ID2_);
  swap(lhs.sub_ID3_, rhs.sub_ID3_);
}

void MM::Utils::ID3::Swap(MM::Utils::ID3& lhs, MM::Utils::ID3& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.sub_ID1_, rhs.sub_ID1_);
  swap(lhs.sub_ID2_, rhs.sub_ID2_);
  swap(lhs.sub_ID3_, rhs.sub_ID3_);
}

std::uint64_t MM::Utils::ID3::GetHashCode() const {
  return sub_ID1_ ^ sub_ID2_ ^ sub_ID3_;
}

void MM::Utils::ID3::Reset() {
  sub_ID1_ = 0;
  sub_ID2_ = 0;
  sub_ID3_ = 0;
}

MM::Utils::ID4::ID4(std::uint64_t sub_ID1, std::uint64_t sub_ID2,
                    std::uint64_t sub_ID3, std::uint64_t sub_ID4)
    : sub_ID1_(sub_ID1),
      sub_ID2_(sub_ID2),
      sub_ID3_(sub_ID3),
      sub_ID4_(sub_ID4) {}

bool MM::Utils::ID4::operator==(const MM::Utils::ID4& rhs) const {
  return sub_ID1_ == rhs.sub_ID1_ && sub_ID2_ == rhs.sub_ID2_ &&
         sub_ID3_ == rhs.sub_ID3_ && sub_ID4_ == rhs.sub_ID4_;
}

bool MM::Utils::ID4::operator!=(const MM::Utils::ID4& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::ID4::operator<(const MM::Utils::ID4& rhs) const {
  if (sub_ID1_ < rhs.sub_ID1_) return true;
  if (rhs.sub_ID1_ < sub_ID1_) return false;
  if (sub_ID2_ < rhs.sub_ID2_) return true;
  if (rhs.sub_ID2_ < sub_ID2_) return false;
  if (sub_ID3_ < rhs.sub_ID3_) return true;
  if (rhs.sub_ID3_ < sub_ID3_) return false;
  return sub_ID4_ < rhs.sub_ID4_;
}

bool MM::Utils::ID4::operator>(const MM::Utils::ID4& rhs) const {
  return rhs < *this;
}

bool MM::Utils::ID4::operator<=(const MM::Utils::ID4& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::ID4::operator>=(const MM::Utils::ID4& rhs) const {
  return !(*this < rhs);
}
void MM::Utils::ID4::Swap(MM::Utils::ID4& lhs, MM::Utils::ID4& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.sub_ID1_, rhs.sub_ID1_);
  swap(lhs.sub_ID2_, rhs.sub_ID2_);
  swap(lhs.sub_ID3_, rhs.sub_ID3_);
  swap(lhs.sub_ID4_, rhs.sub_ID4_);
}

void MM::Utils::ID4::swap(MM::Utils::ID4& lhs, MM::Utils::ID4& rhs) noexcept {
  Swap(lhs, rhs);
}

uint64_t MM::Utils::ID4::GetSubID1() const { return sub_ID1_; }

void MM::Utils::ID4::SetSubID1(std::uint64_t new_sub_ID1) {
  sub_ID1_ = new_sub_ID1;
}

uint64_t MM::Utils::ID4::GetSubID2() const { return sub_ID2_; }

void MM::Utils::ID4::SetSubID2(std::uint64_t new_sub_ID2) {
  sub_ID2_ = new_sub_ID2;
}

uint64_t MM::Utils::ID4::GetSubID3() const { return sub_ID3_; }

void MM::Utils::ID4::SetSubID3(std::uint64_t new_sub_ID3) {
  sub_ID3_ = new_sub_ID3;
}

uint64_t MM::Utils::ID4::GetSubID4() const { return sub_ID4_; }

void MM::Utils::ID4::SetSubID4(std::uint64_t new_sub_ID4) {
  sub_ID4_ = new_sub_ID4;
}

std::uint64_t MM::Utils::ID4::GetHashCode() const {
  return sub_ID1_ ^ sub_ID2_ ^ sub_ID3_ ^ sub_ID4_;
}

void MM::Utils::ID4::Reset() {
  sub_ID1_ = 0;
  sub_ID2_ = 0;
  sub_ID3_ = 0;
  sub_ID4_ = 0;
}

MM::Utils::ID5::ID5(std::uint64_t sub_ID1, std::uint64_t sub_ID2,
                    std::uint64_t sub_ID3, std::uint64_t sub_ID4,
                    std::uint64_t sub_ID5)
    : sub_ID1_(sub_ID1),
      sub_ID2_(sub_ID2),
      sub_ID3_(sub_ID3),
      sub_ID4_(sub_ID4),
      sub_ID5_(sub_ID5) {}

bool MM::Utils::ID5::operator==(const MM::Utils::ID5& rhs) const {
  return sub_ID1_ == rhs.sub_ID1_ && sub_ID2_ == rhs.sub_ID2_ &&
         sub_ID3_ == rhs.sub_ID3_ && sub_ID4_ == rhs.sub_ID4_ &&
         sub_ID5_ == rhs.sub_ID5_;
}

bool MM::Utils::ID5::operator!=(const MM::Utils::ID5& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::ID5::operator<(const MM::Utils::ID5& rhs) const {
  if (sub_ID1_ < rhs.sub_ID1_) return true;
  if (rhs.sub_ID1_ < sub_ID1_) return false;
  if (sub_ID2_ < rhs.sub_ID2_) return true;
  if (rhs.sub_ID2_ < sub_ID2_) return false;
  if (sub_ID3_ < rhs.sub_ID3_) return true;
  if (rhs.sub_ID3_ < sub_ID3_) return false;
  if (sub_ID4_ < rhs.sub_ID4_) return true;
  if (rhs.sub_ID4_ < sub_ID4_) return false;
  return sub_ID5_ < rhs.sub_ID5_;
}

bool MM::Utils::ID5::operator>(const MM::Utils::ID5& rhs) const {
  return rhs < *this;
}

bool MM::Utils::ID5::operator<=(const MM::Utils::ID5& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::ID5::operator>=(const MM::Utils::ID5& rhs) const {
  return !(*this < rhs);
}

void MM::Utils::ID5::Swap(MM::Utils::ID5& lhs, MM::Utils::ID5& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.sub_ID1_, rhs.sub_ID1_);
  swap(lhs.sub_ID2_, rhs.sub_ID2_);
  swap(lhs.sub_ID3_, rhs.sub_ID3_);
  swap(lhs.sub_ID4_, rhs.sub_ID4_);
  swap(lhs.sub_ID5_, rhs.sub_ID5_);
}

void MM::Utils::ID5::swap(MM::Utils::ID5& lhs, MM::Utils::ID5& rhs) noexcept {
  Swap(lhs, rhs);
}

uint64_t MM::Utils::ID5::GetSubID1() const { return sub_ID1_; }

uint64_t MM::Utils::ID5::GetSubID2() const { return sub_ID2_; }

uint64_t MM::Utils::ID5::GetSubID3() const { return sub_ID3_; }

uint64_t MM::Utils::ID5::GetSubID4() const { return sub_ID4_; }

uint64_t MM::Utils::ID5::GetSubID5() const { return sub_ID5_; }

void MM::Utils::ID5::SetSubID1(std::uint64_t new_sub_ID1) {
  sub_ID1_ = new_sub_ID1;
}

void MM::Utils::ID5::SetSubID2(std::uint64_t new_sub_ID2) {
  sub_ID2_ = new_sub_ID2;
}

void MM::Utils::ID5::SetSubID3(std::uint64_t new_sub_ID3) {
  sub_ID3_ = new_sub_ID3;
}

void MM::Utils::ID5::SetSubID4(std::uint64_t new_sub_ID4) {
  sub_ID4_ = new_sub_ID4;
}

void MM::Utils::ID5::SetSubID5(std::uint64_t new_sub_ID5) {
  sub_ID5_ = new_sub_ID5;
}

std::uint64_t MM::Utils::ID5::GetHashCode() const {
  return sub_ID1_ ^ sub_ID2_ ^ sub_ID3_ ^ sub_ID3_ ^ sub_ID4_ ^ sub_ID5_;
}

void MM::Utils::ID5::Reset() {
  sub_ID1_ = 0;
  sub_ID2_ = 0;
  sub_ID3_ = 0;
  sub_ID4_ = 0;
  sub_ID5_ = 0;
}

MM::Utils::ID6::ID6(std::uint64_t sub_id1, std::uint64_t sub_id2,
                    std::uint64_t sub_id3, std::uint64_t sub_id4,
                    std::uint64_t sub_id5, std::uint64_t sub_id6)
    : sub_ID1_(sub_id1),
      sub_ID2_(sub_id2),
      sub_ID3_(sub_id3),
      sub_ID4_(sub_id4),
      sub_ID5_(sub_id5),
      sub_ID6_(sub_id6) {}

bool MM::Utils::ID6::operator==(const MM::Utils::ID6& rhs) const {
  return sub_ID1_ == rhs.sub_ID1_ && sub_ID2_ == rhs.sub_ID2_ &&
         sub_ID3_ == rhs.sub_ID3_ && sub_ID4_ == rhs.sub_ID4_ &&
         sub_ID5_ == rhs.sub_ID5_ && sub_ID6_ == rhs.sub_ID6_;
}

bool MM::Utils::ID6::operator!=(const MM::Utils::ID6& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::ID6::operator<(const MM::Utils::ID6& rhs) const {
  if (sub_ID1_ < rhs.sub_ID1_) return true;
  if (rhs.sub_ID1_ < sub_ID1_) return false;
  if (sub_ID2_ < rhs.sub_ID2_) return true;
  if (rhs.sub_ID2_ < sub_ID2_) return false;
  if (sub_ID3_ < rhs.sub_ID3_) return true;
  if (rhs.sub_ID3_ < sub_ID3_) return false;
  if (sub_ID4_ < rhs.sub_ID4_) return true;
  if (rhs.sub_ID4_ < sub_ID4_) return false;
  if (sub_ID5_ < rhs.sub_ID5_) return true;
  if (rhs.sub_ID5_ < sub_ID5_) return false;
  return sub_ID6_ < rhs.sub_ID6_;
}

bool MM::Utils::ID6::operator>(const MM::Utils::ID6& rhs) const {
  return rhs < *this;
}

bool MM::Utils::ID6::operator<=(const MM::Utils::ID6& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::ID6::operator>=(const MM::Utils::ID6& rhs) const {
  return !(*this < rhs);
}

void MM::Utils::ID6::Swap(MM::Utils::ID6& lhs, MM::Utils::ID6& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.sub_ID1_, rhs.sub_ID1_);
  swap(lhs.sub_ID2_, rhs.sub_ID2_);
  swap(lhs.sub_ID3_, rhs.sub_ID3_);
  swap(lhs.sub_ID4_, rhs.sub_ID4_);
  swap(lhs.sub_ID5_, rhs.sub_ID5_);
  swap(lhs.sub_ID6_, rhs.sub_ID6_);
}

void MM::Utils::ID6::swap(MM::Utils::ID6& lhs, MM::Utils::ID6& rhs) noexcept {
  Swap(lhs, rhs);
}

std::uint64_t MM::Utils::ID6::GetSubID1() const { return sub_ID1_; }

std::uint64_t MM::Utils::ID6::GetSubID2() const { return sub_ID2_; }

std::uint64_t MM::Utils::ID6::GetSubID3() const { return sub_ID3_; }

std::uint64_t MM::Utils::ID6::GetSubID4() const { return sub_ID4_; }

std::uint64_t MM::Utils::ID6::GetSubID5() const { return sub_ID5_; }

std::uint64_t MM::Utils::ID6::GetSubID6() const { return sub_ID6_; }

void MM::Utils::ID6::SetSubID1(std::uint64_t sub_ID1) { sub_ID1_ = sub_ID1; }

void MM::Utils::ID6::SetSubID2(std::uint64_t sub_iD2) { sub_ID2_ = sub_iD2; }

void MM::Utils::ID6::SetSubID3(std::uint64_t sub_iD3) { sub_ID3_ = sub_iD3; }

void MM::Utils::ID6::SetSubID4(std::uint64_t sub_iD4) { sub_ID4_ = sub_iD4; }

void MM::Utils::ID6::SetSubID5(std::uint64_t sub_iD5) { sub_ID5_ = sub_iD5; }

void MM::Utils::ID6::SetSubID6(std::uint64_t sub_iD6) { sub_ID6_ = sub_iD6; }

std::uint64_t MM::Utils::ID6::GetHashCode() const {
  return sub_ID1_ ^ sub_ID2_ ^ sub_ID3_ ^ sub_ID4_ ^ sub_ID5_ ^ sub_ID6_;
}

void MM::Utils::ID6::Reset() {
  sub_ID1_ = 0;
  sub_ID2_ = 0;
  sub_ID3_ = 0;
  sub_ID4_ = 0;
  sub_ID5_ = 0;
  sub_ID6_ = 0;
}

MM::Utils::ID7::ID7(std::uint64_t sub_ID1, std::uint64_t sub_ID2,
                    std::uint64_t sub_ID3, std::uint64_t sub_ID4,
                    std::uint64_t sub_ID5, std::uint64_t sub_ID6,
                    std::uint64_t sub_ID7)
    : sub_ID1_(sub_ID1),
      sub_ID2_(sub_ID2),
      sub_ID3_(sub_ID3),
      sub_ID4_(sub_ID4),
      sub_ID5_(sub_ID5),
      sub_ID6_(sub_ID6),
      sub_ID7_(sub_ID7) {}

bool MM::Utils::ID7::operator==(const MM::Utils::ID7& rhs) const {
  return sub_ID1_ == rhs.sub_ID1_ && sub_ID2_ == rhs.sub_ID2_ &&
         sub_ID3_ == rhs.sub_ID3_ && sub_ID4_ == rhs.sub_ID4_ &&
         sub_ID5_ == rhs.sub_ID5_ && sub_ID6_ == rhs.sub_ID6_ &&
         sub_ID7_ == rhs.sub_ID7_;
}

bool MM::Utils::ID7::operator!=(const MM::Utils::ID7& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::ID7::operator<(const MM::Utils::ID7& rhs) const {
  if (sub_ID1_ < rhs.sub_ID1_) return true;
  if (rhs.sub_ID1_ < sub_ID1_) return false;
  if (sub_ID2_ < rhs.sub_ID2_) return true;
  if (rhs.sub_ID2_ < sub_ID2_) return false;
  if (sub_ID3_ < rhs.sub_ID3_) return true;
  if (rhs.sub_ID3_ < sub_ID3_) return false;
  if (sub_ID4_ < rhs.sub_ID4_) return true;
  if (rhs.sub_ID4_ < sub_ID4_) return false;
  if (sub_ID5_ < rhs.sub_ID5_) return true;
  if (rhs.sub_ID5_ < sub_ID5_) return false;
  if (sub_ID6_ < rhs.sub_ID6_) return true;
  if (rhs.sub_ID6_ < sub_ID6_) return false;
  return sub_ID7_ < rhs.sub_ID7_;
}

bool MM::Utils::ID7::operator>(const MM::Utils::ID7& rhs) const {
  return rhs < *this;
}

bool MM::Utils::ID7::operator<=(const MM::Utils::ID7& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::ID7::operator>=(const MM::Utils::ID7& rhs) const {
  return !(*this < rhs);
}

void MM::Utils::ID7::Swap(MM::Utils::ID7& lhs, MM::Utils::ID7& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.sub_ID1_, rhs.sub_ID1_);
  swap(lhs.sub_ID2_, rhs.sub_ID2_);
  swap(lhs.sub_ID3_, rhs.sub_ID3_);
  swap(lhs.sub_ID4_, rhs.sub_ID4_);
  swap(lhs.sub_ID5_, rhs.sub_ID5_);
  swap(lhs.sub_ID6_, rhs.sub_ID6_);
  swap(lhs.sub_ID7_, rhs.sub_ID7_);
}

void MM::Utils::ID7::swap(MM::Utils::ID7& lhs, MM::Utils::ID7& rhs) noexcept {
  Swap(lhs, rhs);
}

std::uint64_t MM::Utils::ID7::GetSubID1() const { return sub_ID1_; }

std::uint64_t MM::Utils::ID7::GetSubID2() const { return sub_ID2_; }

std::uint64_t MM::Utils::ID7::GetSubID3() const { return sub_ID3_; }

std::uint64_t MM::Utils::ID7::GetSubID4() const { return sub_ID4_; }

std::uint64_t MM::Utils::ID7::GetSubID5() const { return sub_ID5_; }

std::uint64_t MM::Utils::ID7::GetSubID6() const { return sub_ID6_; }

std::uint64_t MM::Utils::ID7::GetSubID7() const { return sub_ID7_; }

void MM::Utils::ID7::SetSubID1(uint64_t sub_ID1) { sub_ID1_ = sub_ID1; }

void MM::Utils::ID7::SetSubID2(uint64_t sub_ID2) { sub_ID2_ = sub_ID2; }

void MM::Utils::ID7::SetSubID3(uint64_t sub_ID3) { sub_ID3_ = sub_ID3; }

void MM::Utils::ID7::SetSubID4(uint64_t sub_ID4) { sub_ID4_ = sub_ID4; }

void MM::Utils::ID7::SetSubID5(uint64_t sub_ID5) { sub_ID5_ = sub_ID5; }

void MM::Utils::ID7::SetSubID6(uint64_t sub_ID6) { sub_ID6_ = sub_ID6; }

void MM::Utils::ID7::SetSubID7(uint64_t sub_ID7) { sub_ID7_ = sub_ID7; }

std::uint64_t MM::Utils::ID7::GetHashCode() const {
  return sub_ID1_ ^ sub_ID2_ ^ sub_ID3_ ^ sub_ID4_ ^ sub_ID5_ ^ sub_ID6_ ^
         sub_ID7_;
}

void MM::Utils::ID7::Reset() {
  sub_ID1_ = 0;
  sub_ID2_ = 0;
  sub_ID3_ = 0;
  sub_ID4_ = 0;
  sub_ID5_ = 0;
  sub_ID6_ = 0;
  sub_ID7_ = 0;
}

MM::Utils::ID8::ID8(std::uint64_t sub_ID1, std::uint64_t sub_ID2,
                    std::uint64_t sub_ID3, std::uint64_t sub_ID4,
                    std::uint64_t sub_ID5, std::uint64_t sub_ID6,
                    std::uint64_t sub_ID7, std::uint64_t sub_ID8)
    : sub_ID1_(sub_ID1),
      sub_ID2_(sub_ID2),
      sub_ID3_(sub_ID3),
      sub_ID4_(sub_ID4),
      sub_ID5_(sub_ID5),
      sub_ID6_(sub_ID6),
      sub_ID7_(sub_ID7),
      sub_ID8_(sub_ID8) {}

bool MM::Utils::ID8::operator==(const MM::Utils::ID8& rhs) const {
  return sub_ID1_ == rhs.sub_ID1_ && sub_ID2_ == rhs.sub_ID2_ &&
         sub_ID3_ == rhs.sub_ID3_ && sub_ID4_ == rhs.sub_ID4_ &&
         sub_ID5_ == rhs.sub_ID5_ && sub_ID6_ == rhs.sub_ID6_ &&
         sub_ID7_ == rhs.sub_ID7_ && sub_ID8_ == rhs.sub_ID8_;
}

bool MM::Utils::ID8::operator!=(const MM::Utils::ID8& rhs) const {
  return !(rhs == *this);
}

bool MM::Utils::ID8::operator<(const MM::Utils::ID8& rhs) const {
  if (sub_ID1_ < rhs.sub_ID1_) return true;
  if (rhs.sub_ID1_ < sub_ID1_) return false;
  if (sub_ID2_ < rhs.sub_ID2_) return true;
  if (rhs.sub_ID2_ < sub_ID2_) return false;
  if (sub_ID3_ < rhs.sub_ID3_) return true;
  if (rhs.sub_ID3_ < sub_ID3_) return false;
  if (sub_ID4_ < rhs.sub_ID4_) return true;
  if (rhs.sub_ID4_ < sub_ID4_) return false;
  if (sub_ID5_ < rhs.sub_ID5_) return true;
  if (rhs.sub_ID5_ < sub_ID5_) return false;
  if (sub_ID6_ < rhs.sub_ID6_) return true;
  if (rhs.sub_ID6_ < sub_ID6_) return false;
  if (sub_ID7_ < rhs.sub_ID7_) return true;
  if (rhs.sub_ID7_ < sub_ID7_) return false;
  return sub_ID8_ < rhs.sub_ID8_;
}

bool MM::Utils::ID8::operator>(const MM::Utils::ID8& rhs) const {
  return rhs < *this;
}

bool MM::Utils::ID8::operator<=(const MM::Utils::ID8& rhs) const {
  return !(rhs < *this);
}

bool MM::Utils::ID8::operator>=(const MM::Utils::ID8& rhs) const {
  return !(*this < rhs);
}

void MM::Utils::ID8::Swap(MM::Utils::ID8& lhs, MM::Utils::ID8& rhs) noexcept {
  using std::swap;

  if (&lhs == &rhs) {
    return;
  }

  swap(lhs.sub_ID1_, rhs.sub_ID1_);
  swap(lhs.sub_ID2_, rhs.sub_ID2_);
  swap(lhs.sub_ID3_, rhs.sub_ID3_);
  swap(lhs.sub_ID4_, rhs.sub_ID4_);
  swap(lhs.sub_ID5_, rhs.sub_ID5_);
  swap(lhs.sub_ID6_, rhs.sub_ID6_);
  swap(lhs.sub_ID7_, rhs.sub_ID7_);
  swap(lhs.sub_ID8_, rhs.sub_ID8_);
}

void MM::Utils::ID8::swap(MM::Utils::ID8& lhs, MM::Utils::ID8& rhs) noexcept {
  Swap(lhs, rhs);
}

std::uint64_t MM::Utils::ID8::GetSubID1() const { return sub_ID1_; }

std::uint64_t MM::Utils::ID8::GetSubID2() const { return sub_ID2_; }

std::uint64_t MM::Utils::ID8::GetSubID3() const { return sub_ID3_; }

std::uint64_t MM::Utils::ID8::GetSubID4() const { return sub_ID4_; }

std::uint64_t MM::Utils::ID8::GetSubID5() const { return sub_ID5_; }

std::uint64_t MM::Utils::ID8::GetSubID6() const { return sub_ID6_; }

std::uint64_t MM::Utils::ID8::GetSubID7() const { return sub_ID7_; }

std::uint64_t MM::Utils::ID8::GetSubID8() const { return sub_ID8_; }

void MM::Utils::ID8::SetSubID1(std::uint64_t sub_iD1) { sub_ID1_ = sub_iD1; }

void MM::Utils::ID8::SetSubID2(std::uint64_t sub_iD2) { sub_ID2_ = sub_iD2; }

void MM::Utils::ID8::SetSubID3(std::uint64_t sub_iD3) { sub_ID3_ = sub_iD3; }

void MM::Utils::ID8::SetSubID4(std::uint64_t sub_iD4) { sub_ID4_ = sub_iD4; }

void MM::Utils::ID8::SetSubID5(std::uint64_t sub_iD5) { sub_ID5_ = sub_iD5; }

void MM::Utils::ID8::SetSubID6(std::uint64_t sub_iD6) { sub_ID6_ = sub_iD6; }

void MM::Utils::ID8::SetSubID7(std::uint64_t sub_iD7) { sub_ID7_ = sub_iD7; }

void MM::Utils::ID8::SetSubID8(std::uint64_t sub_iD8) { sub_ID8_ = sub_iD8; }

std::uint64_t MM::Utils::ID8::GetHashCode() const {
  return sub_ID1_ ^ sub_ID2_ ^ sub_ID3_ ^ sub_ID4_ ^ sub_ID5_ ^ sub_ID6_ ^
         sub_ID7_ ^ sub_ID8_;
}

void MM::Utils::ID8::Reset() {
  sub_ID1_ = 0;
  sub_ID2_ = 0;
  sub_ID3_ = 0;
  sub_ID4_ = 0;
  sub_ID5_ = 0;
  sub_ID6_ = 0;
  sub_ID7_ = 0;
  sub_ID8_ = 0;
}
