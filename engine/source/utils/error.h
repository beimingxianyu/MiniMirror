#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>

#include "utils/marco.h"

namespace MM {

namespace StaticTrait {
struct Success {};
struct Error {};
}  // namespace StaticTrait

static StaticTrait::Success st_execute_success;
static StaticTrait::Error st_execute_error;

enum class ErrorCode : std::uint32_t {
  SUCCESS = 0,
  UNDEFINED_ERROR,
  OUT_OF_HOST_MEMORY,
  OUT_OF_DEVICE_MEMORY,
  OBJECT_IS_INVALID,
  NO_SUCH_CONFIG,
  TYPE_CONVERSION_FAILED,
  LOAD_CONFIG_FROM_FILE_FAILED,
  FILE_IS_NOT_EXIST,
  CREATE_OBJECT_FAILED,
  INITIALIZATION_FAILED = CREATE_OBJECT_FAILED,
  DESTROY_FAILED,
  // Such as child "class A", parent "class B", B have a member std::set<A>
  // A_set. The instance of class B b_instance and the instance of class A
  // a_instance, b.A_set not contain a_instance and call A_set.at(a_instance,)
  // will return this error.
  PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT,
  RENDER_COMMAND_RECORD_OR_SUBMIT_FAILED,
  INPUT_PARAMETERS_ARE_INCORRECT,
  TIMEOUT,
  OPERATION_NOT_SUPPORTED,
  INPUT_PARAMETERS_ARE_NOT_SUITABLE,
  // rename/delete/create/remove/etc.
  FILE_OPERATION_ERROR,
  SYNCHRONIZE_FAILED,
  NO_AVAILABLE_ELEMENT,
  NULL_OBJECT_ERROR,
  CUSTOM_ERROR
};

ErrorCode operator|(ErrorCode l_result, ErrorCode r_result);

ErrorCode operator|=(ErrorCode l_result, ErrorCode r_result);

ErrorCode operator&(ErrorCode l_result, ErrorCode r_result);

ErrorCode operator&=(ErrorCode l_result, ErrorCode r_result);

class ErrorTypeBase {
 public:
  ErrorTypeBase() = default;
  virtual ~ErrorTypeBase() = default;
  ErrorTypeBase(const ErrorTypeBase& other) = default;
  ErrorTypeBase(ErrorTypeBase&& other) noexcept = default;
  ErrorTypeBase& operator=(const ErrorTypeBase& other) = default;
  ErrorTypeBase& operator=(ErrorTypeBase&& other) noexcept = default;

 public:
  virtual void Exception();

  virtual void Exception() const;

  virtual bool IsSuccess() const = 0;

  virtual void Reset() = 0;
};

class ErrorNil final : public ErrorTypeBase {
 public:
  ErrorNil() = default;
  ~ErrorNil() override = default;
  explicit ErrorNil(bool is_success);
  ErrorNil(const ErrorNil& other) = default;
  ErrorNil(ErrorNil&& other) noexcept;
  ErrorNil& operator=(const ErrorNil& other);
  ErrorNil& operator=(ErrorNil&& other) noexcept;

 public:
  bool operator==(const ErrorNil& rhs) const;

  bool operator!=(const ErrorNil& rhs) const;

  explicit operator bool() const;

 public:
  void Exception() override;

  void Exception() const override;

  bool IsSuccess() const override;

  void Reset() override;

 private:
  bool success_{true};
};

class ErrorResult final : public ErrorTypeBase {
 public:
  ErrorResult() = default;
  ~ErrorResult() override = default;
  explicit ErrorResult(ErrorCode error_code);
  ErrorResult(const ErrorResult& other) = default;
  ErrorResult(ErrorResult&& other) noexcept;
  ErrorResult& operator=(const ErrorResult& other);
  ErrorResult& operator=(ErrorResult&& other) noexcept;

 public:
  bool operator==(const ErrorResult& rhs) const;

  bool operator!=(const ErrorResult& rhs) const;

  explicit operator bool() const;

 public:
  void Exception() override;

  void Exception() const override;

  bool IsSuccess() const override;

  void Reset() override;

  ErrorCode GetErrorCode() const;

 private:
  ErrorCode error_code_{ErrorCode ::SUCCESS};
};

// Success result
template <typename ResultTypeArg>
class ResultS {
 public:
  using ResultType = ResultTypeArg;

  template <typename ResultTypeArgSame, typename ErrorTypeArg>
  friend class Result;

 public:
  ResultS() = default;
  ~ResultS() = default;
  explicit ResultS(const ResultTypeArg& result) : result_(result) {}
  explicit ResultS(ResultTypeArg&& result) : result_(std::move(result)) {}
  template <typename... Args>
  explicit ResultS(Args... args) : result_(std::forward<Args>(args)...) {}
  ResultS(const ResultS& other) : result_(other.result_) {}
  ResultS(ResultS&& other) noexcept : result_(std::move(other.result_)) {}
  ResultS& operator=(const ResultS& other) {
    if (std::addressof(other) == this) {
      return *this;
    }

    result_ = other.result_;

    return *this;
  }
  ResultS& operator=(ResultS&& other) noexcept {
    if (std::addressof(other) == this) {
      return *this;
    }

    result_ = std::move(other.result_);

    return *this;
  }

 public:
  ResultType& GetResult() { return result_; }

  const ResultType& GetResult() const { return result_; }

 private:
  ResultType&& Move() { return std::move(result_); }

 private:
  ResultType result_{};
};

// Error result
template <typename ErrorTypeArg = ErrorResult>
class ResultE {
 public:
  using ErrorType = ErrorTypeArg;

  template <typename ResultTypeArg, typename ErrorTypeArgSame>
  friend class Result;

 public:
  ResultE() = default;
  ~ResultE() = default;
  explicit ResultE(const ErrorType& error) : error_(error) {}
  explicit ResultE(ErrorType&& error) : error_(std::move(error)) {}
  template <typename... Args>
  explicit ResultE(Args... args) : error_(std::forward<Args>(args)...) {}
  ResultE(const ResultE& other) : error_(other.error_) {}
  ResultE(ResultE&& other) noexcept : error_(std::move(other.error_)) {}
  ResultE& operator=(const ResultE& other) {
    if (std::addressof(other) == this) {
      return *this;
    }

    error_ = other.error_;

    return *this;
  }
  ResultE& operator=(ResultE&& other) noexcept {
    if (std::addressof(other) == this) {
      return *this;
    }

    error_ = std::move(other.error_);

    return *this;
  }

 public:
  ErrorType& GetError() { return error_; }

  const ErrorType& GetError() const { return error_; }

 private:
  ErrorType&& Move() { return std::move(error_); }

 private:
  ErrorType error_{};
};

template <typename ResultTypeArg, typename ErrorTypeArg = ErrorResult>
class Result {
 public:
  using ResultType = ResultTypeArg;
  using ErrorType = ErrorTypeArg;

 public:
  Result() = delete;
  ~Result() = default;
  template <typename... Args>
  explicit Result(Args&&... args)
      : result_wrapper_(std::forward<Args>(args)...) {}
  explicit Result(const ErrorType& error) : result_wrapper_(error) {}
  explicit Result(ErrorType&& error) : result_wrapper_(std::move(error)) {}
  template <typename... Args>
  explicit Result(StaticTrait::Success, Args... args)
      : result_wrapper_(st_execute_success, std::forward<Args>(args)...) {}
  template <typename... Args>
  explicit Result(StaticTrait::Error, Args... args)
      : result_wrapper_(st_execute_error, std::forward<Args>(args)...) {}
  Result(ResultS<ResultType>&& result)
      : result_wrapper_(st_execute_success, result.Move()) {}
  Result(ResultE<ErrorType>&& error)
      : result_wrapper_(st_execute_error, error.Move()) {}
  Result(const Result& other) : result_wrapper_(other.result_wrapper_) {}
  Result(Result&& other) noexcept
      : result_wrapper_(std::move(other.result_wrapper_)) {}
  Result& operator=(const Result& other) {
    result_wrapper_ = other.result_wrapper_;

    return *this;
  }
  Result& operator=(Result&& other) noexcept {
    result_wrapper_ = other.result_wrapper_;

    return *this;
  }

 public:
  ErrorType& GetError() {
    assert(IsError());
    return result_wrapper_.GetError();
  }

  const ErrorType& GetError() const {
    assert(IsError());
    return result_wrapper_.GetError();
  }

  ResultType& GetResult() {
    assert(IsSuccess());
    return result_wrapper_.GetResult();
  }

  const ResultType& GetResult() const {
    assert(IsSuccess());
    return result_wrapper_.GetResult();
  }

  Result&& Move() { return std::move(*this); }

  Result& Exception() {
    result_wrapper_.Exception();

    return *this;
  }

  const Result& Exception() const {
    result_wrapper_.Exception();

    return *this;
  }

  Result& Exception(void callback(ResultType&, const ErrorType&)) {
    result_wrapper_.Exception(callback);

    return *this;
  }

  Result& Exception(void callback(ErrorType&)) {
    result_wrapper_.Exception(callback);

    return *this;
  }

  Result& Exception(void callback(const ErrorType&)) {
    result_wrapper_.Exception(callback);

    return *this;
  }

  const Result& Exception(void callback(const ErrorType&)) const {
    result_wrapper_.Exception(callback);

    return *this;
  }

  template <typename CallbackType>
  Result& Exception(CallbackType&& callback) {
    result_wrapper_.Exception(callback);

    return *this;
  }

  template <typename CallbackType>
  const Result& Exception(CallbackType&& callback) const {
    result_wrapper_.Exception(callback);

    return *this;
  }

  Result& IgnoreException() {
    result_wrapper_.IgnoreException();
    return *this;
  }

  bool IsSuccess() const { return result_wrapper_.Success(); }

  bool IsError() const { return !result_wrapper_.Success(); }

  void IsSuccess(bool& result) const { result = result_wrapper_.Success(); }

  void IsError(bool& result) const { result = !result_wrapper_.Success(); }

 private:
  template <typename ResultTypeIn, typename ErrorTypeIn,
            typename ErrorTypeInIsBaseOnErrorTypeBase = std::enable_if_t<
                std::is_base_of_v<ErrorTypeBase, ErrorTypeIn>, void>>
  class ResultWrapper {
   public:
    ResultWrapper() = delete;
    ~ResultWrapper() {
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      if (!exception_processed_) {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                  << "!!!!!!!!!!! Have exception not processed !!!!!!!!!!!\n"
                  << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                  << std::endl;
        abort();
      }
#endif
    };
    template <typename... Args>
    explicit ResultWrapper(Args&&... args)
        : result_(std::forward<Args>(args)...),
          error_()
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(true)
#endif
    {
    }
    explicit ResultWrapper(const ErrorType& error)
        : result_(),
          error_(error)
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(false)
#endif
    {
    }
    explicit ResultWrapper(ErrorType&& error)
        : result_(),
          error_(std::move(error))
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(false)
#endif
    {
    }
    template <typename... Args>
    explicit ResultWrapper(StaticTrait::Success, Args... args)
        : result_(std::forward<Args>(args)...),
          error_()
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(true)
#endif
    {
    }
    template <typename... Args>
    explicit ResultWrapper(StaticTrait::Error, Args... args)
        : result_(),
          error_(std::forward<Args>(args)...)
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(false)
#endif
    {
    }
    ResultWrapper(const ResultWrapper& other) = default;
    ResultWrapper(ResultWrapper&& other) noexcept
        : result_(std::move(other.result_)),
          error_(std::move(other.error_))
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(other.exception_processed_)
#endif
    {
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      other.exception_processed_ = true;
#endif
    }
    ResultWrapper& operator=(const ResultWrapper& other) {
      if (std::addressof(other) == this) {
        return *this;
      }

#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      if (!exception_processed_) {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                  << "!!!!!!!!!!! Have exception not processed !!!!!!!!!!!\n"
                  << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                  << std::endl;
        abort();
      }
#endif

      result_ = other.result_;
      error_ = other.error_;
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      exception_processed_ = other.exception_processed_;
#endif

      return *this;
    }
    ResultWrapper& operator=(ResultWrapper&& other) noexcept {
      if (std::addressof(other) == this) {
        return *this;
      }

#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      if (!exception_processed_) {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                  << "!!!!!!!!!!! Have exception not processed !!!!!!!!!!!\n"
                  << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                  << std::endl;
        abort();
      }
#endif

      result_ = std::move(other.result_);
      error_ = std::move(other.error_);
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      exception_processed_ = other.exception_processed_;
      other.exception_processed_ = true;
#endif

      return *this;
    }

   public:
    explicit operator bool() const { return error_.IsSuccess(); }

   public:
    ErrorTypeIn& GetError() { return error_; }

    const ErrorTypeIn& GetError() const { return error_; }

    ResultTypeIn& GetResult() { return result_; }

    const ResultTypeIn& GetResult() const { return result_; }

    void IgnoreException() {
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      exception_processed_ = true;
#endif
    }

    void Exception() {
      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();

      IgnoreException();
    }

    void Exception() const {
      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();

      IgnoreException();
    }

    void Exception(void callback()) const {
      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();
      callback();

      IgnoreException();
    }

    void Exception(void callback(ResultType&, const ErrorType&)) {
      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();
      callback(result_, error_);

      IgnoreException();
    }

    void Exception(void callback(ErrorType&)) {
      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();
      callback(error_);

      IgnoreException();
    }

    void Exception(void callback(const ErrorType&)) {
      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();
      callback(error_);

      IgnoreException();
    }

    void Exception(void callback(const ErrorType&)) const {
      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();
      callback(error_);

      IgnoreException();
    }

    template <typename CallbackType>
    void Exception(CallbackType&& callback) {
      constexpr bool callback_signature1 =
          std::is_invocable_v<CallbackType, ResultType&, const ErrorType&>;
      constexpr bool callback_signature2 =
          std::is_invocable_v<CallbackType, ErrorType&>;
      constexpr bool callback_signature3 =
          std::is_invocable_v<CallbackType, const ErrorType&>;
      constexpr bool callback_signature4 =
          std::is_invocable_v<CallbackType, ErrorType>;
      constexpr bool callback_signature5 =
          std::is_invocable_v<CallbackType, ResultType&, ErrorType>;
      constexpr bool callback_signature6 =
          std::is_invocable_v<CallbackType, ResultType&>;
      constexpr bool callback_signature7 = std::is_invocable_v<CallbackType>;

      static_assert(callback_signature1 || callback_signature2 ||
                        callback_signature3 || callback_signature4 ||
                        callback_signature5 || callback_signature6 ||
                        callback_signature7,
                    "Callback signature is invalid.");

      if (likely(error_.IsSuccess())) {
        return;
      }

      error_.Exception();

      if constexpr (callback_signature1 || callback_signature5) {
        callback(result_, error_);

        IgnoreException();
        return;
      } else if constexpr (callback_signature2) {
        callback(error_);

        IgnoreException();
        return;
      } else if constexpr (callback_signature3 || callback_signature4) {
        callback(error_);

        IgnoreException();
        return;
      } else if constexpr (callback_signature6) {
        callback(result_);

        IgnoreException();
        return;
      } else if constexpr (callback_signature7) {
        callback();

        IgnoreException();
        return;
      }
    }

    template <typename CallBackType>
    void Exception(CallBackType&& callback) const {
      constexpr bool callback_signature1 =
          std::is_invocable_v<CallBackType, const ErrorType&>;
      constexpr bool callback_signature2 =
          std::is_invocable_v<CallBackType, ErrorType>;
      constexpr bool callback_signature3 = std::is_invocable_v<CallBackType>;
      static_assert(
          callback_signature1 || callback_signature2 || callback_signature3,
          "Callback signature is invalid.");

      if constexpr (callback_signature1 || callback_signature2) {
        error_.Exception();
        callback(error_);
      } else {
        callback();
      }

      IgnoreException();
    }

    bool Success() const { return error_.IsSuccess(); }

   private:
    ResultType result_{};
    ErrorType error_{};

#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
    bool exception_processed_{false};
#endif
  };

 private:
  ResultWrapper<ResultType, ErrorType> result_wrapper_{};
};
}  // namespace MM
