#include "utils/error.h"

MM::Utils::ErrorCode MM::Utils::operator|(ErrorCode l_result,
                                              ErrorCode r_result) {
  return l_result |= r_result;
}

MM::Utils::ErrorCode MM::Utils::operator|=(ErrorCode l_result,
                                               ErrorCode r_result) {
  l_result = ErrorCode{static_cast<std::uint32_t>(l_result) |
                           static_cast<std::uint32_t>(r_result)};
  return l_result;
}

MM::Utils::ErrorCode MM::Utils::operator&(ErrorCode l_result,
                                              ErrorCode r_result) {
  return l_result &= r_result;
}

MM::Utils::ErrorCode MM::Utils::operator&=(ErrorCode l_result,
                                               ErrorCode r_result) {
  l_result = ErrorCode{static_cast<std::uint32_t>(l_result) &
                           static_cast<std::uint32_t>(r_result)};
  return l_result;
}

void MM::Utils::ErrorTypeBase::Exception() {}

void MM::Utils::ErrorTypeBase::Exception() const {}

MM::Utils::ErrorNil::ErrorNil(bool is_success) : success_(is_success) {}

MM::Utils::ErrorNil::ErrorNil(MM::Utils::ErrorNil&& other) noexcept
    : success_(other.success_) {
  other.Reset();
}

MM::Utils::ErrorNil& MM::Utils::ErrorNil::operator=(
    const MM::Utils::ErrorNil& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  success_ = other.success_;

  return *this;
}

MM::Utils::ErrorNil& MM::Utils::ErrorNil::operator=(
    MM::Utils::ErrorNil&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  success_ = other.success_;
  other.Reset();

  return *this;
}

bool MM::Utils::ErrorNil::operator==(const MM::Utils::ErrorNil& rhs) const {
  return success_ == rhs.success_;
}

bool MM::Utils::ErrorNil::operator!=(const MM::Utils::ErrorNil& rhs) const {
  return !(rhs == *this);
}

MM::Utils::ErrorNil::operator bool() const { return Success(); }

void MM::Utils::ErrorNil::Exception() {}

void MM::Utils::ErrorNil::Exception() const {}

bool MM::Utils::ErrorNil::Success() const { return success_; }

void MM::Utils::ErrorNil::Reset() { success_ = true; }

bool MM::Utils::ErrorResult::operator==(
    const MM::Utils::ErrorResult& rhs) const {
  return error_code_ == rhs.error_code_;
}

bool MM::Utils::ErrorResult::operator!=(
    const MM::Utils::ErrorResult& rhs) const {
  return !(rhs == *this);
}

MM::Utils::ErrorResult::operator bool() const { return Success(); }

MM::Utils::ErrorResult::ErrorResult(
    MM::Utils::ErrorCode error_code)
    : error_code_(error_code) {}

MM::Utils::ErrorResult::ErrorResult(
    MM::Utils::ErrorResult&& other) noexcept
    : error_code_(other.error_code_) {
  other.Reset();
}

MM::Utils::ErrorResult&
MM::Utils::ErrorResult::operator=(
    const MM::Utils::ErrorResult& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  error_code_ = other.error_code_;

  return *this;
}

MM::Utils::ErrorResult&
MM::Utils::ErrorResult::operator=(
    MM::Utils::ErrorResult&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  error_code_ = other.error_code_;
  other.Reset();

  return *this;
}

MM::Utils::ErrorCode
MM::Utils::ErrorResult::GetErrorCode() const {
  return error_code_;
}

void MM::Utils::ErrorResult::Exception() {}

void MM::Utils::ErrorResult::Exception() const {}

bool MM::Utils::ErrorResult::Success() const {
  return error_code_ == ErrorCode::SUCCESS;
}

void MM::Utils::ErrorResult::Reset() {
  error_code_ = ErrorCode ::SUCCESS;
}

