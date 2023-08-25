#include "utils/error.h"

MM::ErrorCode MM::operator|(ErrorCode l_result,
                            ErrorCode r_result) {
    return l_result |= r_result;
}

MM::ErrorCode MM::operator|=(ErrorCode l_result,
                             ErrorCode r_result) {
    l_result = ErrorCode{static_cast<std::uint32_t>(l_result) |
                         static_cast<std::uint32_t>(r_result)};
    return l_result;
}

MM::ErrorCode MM::operator&(ErrorCode l_result,
                            ErrorCode r_result) {
    return l_result &= r_result;
}

MM::ErrorCode MM::operator&=(ErrorCode l_result,
                             ErrorCode r_result) {
    l_result = ErrorCode{static_cast<std::uint32_t>(l_result) &
                         static_cast<std::uint32_t>(r_result)};
    return l_result;
}

void MM::ErrorTypeBase::Exception() {}

void MM::ErrorTypeBase::Exception() const {}

MM::ErrorNil::ErrorNil(bool is_success) : success_(is_success) {}

MM::ErrorNil::ErrorNil(MM::ErrorNil &&other) noexcept
        : success_(other.success_) {
    other.Reset();
}

MM::ErrorNil &MM::ErrorNil::operator=(
        const MM::ErrorNil &other) {
    if (std::addressof(other) == this) {
        return *this;
    }

    success_ = other.success_;

    return *this;
}

MM::ErrorNil &MM::ErrorNil::operator=(
        MM::ErrorNil &&other) noexcept {
    if (std::addressof(other) == this) {
        return *this;
    }

    success_ = other.success_;
    other.Reset();

    return *this;
}

bool MM::ErrorNil::operator==(const MM::ErrorNil &rhs) const {
    return success_ == rhs.success_;
}

bool MM::ErrorNil::operator!=(const MM::ErrorNil &rhs) const {
    return !(rhs == *this);
}

MM::ErrorNil::operator bool() const { return Success(); }

void MM::ErrorNil::Exception() {}

void MM::ErrorNil::Exception() const {}

bool MM::ErrorNil::Success() const { return success_; }

void MM::ErrorNil::Reset() { success_ = true; }

bool MM::ErrorResult::operator==(
        const MM::ErrorResult &rhs) const {
    return error_code_ == rhs.error_code_;
}

bool MM::ErrorResult::operator!=(
        const MM::ErrorResult &rhs) const {
    return !(rhs == *this);
}

MM::ErrorResult::operator bool() const { return Success(); }

MM::ErrorResult::ErrorResult(
        MM::ErrorCode error_code)
        : error_code_(error_code) {}

MM::ErrorResult::ErrorResult(
        MM::ErrorResult &&other) noexcept
        : error_code_(other.error_code_) {
    other.Reset();
}

MM::ErrorResult &
MM::ErrorResult::operator=(
        const MM::ErrorResult &other) {
    if (std::addressof(other) == this) {
        return *this;
    }

    error_code_ = other.error_code_;

    return *this;
}

MM::ErrorResult &
MM::ErrorResult::operator=(
        MM::ErrorResult &&other) noexcept {
    if (std::addressof(other) == this) {
        return *this;
    }

    error_code_ = other.error_code_;
    other.Reset();

    return *this;
}

MM::ErrorCode
MM::ErrorResult::GetErrorCode() const {
    return error_code_;
}

void MM::ErrorResult::Exception() {}

void MM::ErrorResult::Exception() const {}

bool MM::ErrorResult::Success() const {
    return error_code_ == ErrorCode::SUCCESS;
}

void MM::ErrorResult::Reset() {
    error_code_ = ErrorCode::SUCCESS;
}