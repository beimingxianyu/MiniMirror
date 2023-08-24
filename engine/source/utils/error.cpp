#include "utils//error.h"

MM::Utils::ExecuteResult MM::Utils::operator|(ExecuteResult l_result,
                                              ExecuteResult r_result) {
  return l_result |= r_result;
}

MM::Utils::ExecuteResult MM::Utils::operator|=(ExecuteResult l_result,
                                               ExecuteResult r_result) {
  l_result = ExecuteResult{static_cast<std::uint32_t>(l_result) |
                           static_cast<std::uint32_t>(r_result)};
  return l_result;
}

MM::Utils::ExecuteResult MM::Utils::operator&(ExecuteResult l_result,
                                              ExecuteResult r_result) {
  return l_result &= r_result;
}

MM::Utils::ExecuteResult MM::Utils::operator&=(ExecuteResult l_result,
                                               ExecuteResult r_result) {
  l_result = ExecuteResult{static_cast<std::uint32_t>(l_result) &
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

bool MM::Utils::ExecuteResultWrapperBase::operator==(
    const MM::Utils::ExecuteResultWrapperBase& rhs) const {
  return error_code_ == rhs.error_code_;
}

bool MM::Utils::ExecuteResultWrapperBase::operator!=(
    const MM::Utils::ExecuteResultWrapperBase& rhs) const {
  return !(rhs == *this);
}

MM::Utils::ExecuteResultWrapperBase::operator bool() const { return Success(); }

MM::Utils::ExecuteResultWrapperBase::ExecuteResultWrapperBase(
    MM::Utils::ExecuteResultWrapperBase::ErrorCode error_code)
    : error_code_(error_code) {}

MM::Utils::ExecuteResultWrapperBase::ExecuteResultWrapperBase(
    MM::Utils::ExecuteResultWrapperBase&& other) noexcept
    : error_code_(other.error_code_) {
  other.Reset();
}

MM::Utils::ExecuteResultWrapperBase&
MM::Utils::ExecuteResultWrapperBase::operator=(
    const MM::Utils::ExecuteResultWrapperBase& other) {
  if (std::addressof(other) == this) {
    return *this;
  }

  error_code_ = other.error_code_;

  return *this;
}

MM::Utils::ExecuteResultWrapperBase&
MM::Utils::ExecuteResultWrapperBase::operator=(
    MM::Utils::ExecuteResultWrapperBase&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  error_code_ = other.error_code_;
  other.Reset();

  return *this;
}

MM::Utils::ExecuteResultWrapperBase::ErrorCode
MM::Utils::ExecuteResultWrapperBase::GetErrorCode() const {
  return error_code_;
}

void MM::Utils::ExecuteResultWrapperBase::Exception() {}

void MM::Utils::ExecuteResultWrapperBase::Exception() const {}

bool MM::Utils::ExecuteResultWrapperBase::Success() const {
  return error_code_ == ErrorCode::SUCCESS;
}

void MM::Utils::ExecuteResultWrapperBase::Reset() {
  error_code_ = ErrorCode ::SUCCESS;
}

