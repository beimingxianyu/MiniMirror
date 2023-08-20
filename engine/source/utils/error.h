#pragma once
#include <cstdint>
#include <iostream>
#include <memory>

namespace MM {
namespace Utils {
// TODO rename to ErrorCode
enum class ExecuteResult : std::uint32_t {
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
  INITIALIZATION_FAILED,
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
  CUSTOM_ERROR
};

ExecuteResult operator|(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator|=(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&(ExecuteResult l_result, ExecuteResult r_result);

ExecuteResult operator&=(ExecuteResult l_result, ExecuteResult r_result);

class ErrorTypeBase {
 public:
  ErrorTypeBase() = default;
  ~ErrorTypeBase() = default;
  ErrorTypeBase(const ErrorTypeBase& other) = default;
  ErrorTypeBase(ErrorTypeBase&& other) noexcept = default;
  ErrorTypeBase& operator=(const ErrorTypeBase& other) = default;
  ErrorTypeBase& operator=(ErrorTypeBase&& other) noexcept = default;

 public:
  virtual void Exception() = 0;

  virtual void Exception() const = 0;

  virtual bool Success() const = 0;

  virtual void Reset() = 0;
};

class ErrorNil final : public ErrorTypeBase {
 public:
  ErrorNil() = default;
  ~ErrorNil() = default;
  explicit ErrorNil(bool is_success);
  ErrorNil(const ErrorNil& other) = default;
  ErrorNil(ErrorNil&& other) noexcept;
  ErrorNil& operator=(const ErrorNil& other);
  ErrorNil& operator=(ErrorNil&& other) noexcept;

 public:
  void Exception() override;

  void Exception() const override;

  bool Success() const override;

  void Reset() override;

 private:
  bool success_{true};
};

// TODO rename to ExecuteResultBae
class ExecuteResultWrapperBase final : public ErrorTypeBase {
 public:
  using ErrorCode = ExecuteResult;

 public:
  ExecuteResultWrapperBase() = default;
  ~ExecuteResultWrapperBase() = default;
  explicit ExecuteResultWrapperBase(ErrorCode error_code);
  ExecuteResultWrapperBase(const ExecuteResultWrapperBase& other) = default;
  ExecuteResultWrapperBase(ExecuteResultWrapperBase&& other) noexcept;
  ExecuteResultWrapperBase& operator=(const ExecuteResultWrapperBase& other);
  ExecuteResultWrapperBase& operator=(
      ExecuteResultWrapperBase&& other) noexcept;

 public:
  void Exception() override;

  void Exception() const override;

  bool Success() const override;

  void Reset() override;

  ErrorCode GetErrorCode() const;

 private:
  ErrorCode error_code_{ErrorCode ::UNDEFINED_ERROR};
};

namespace ResultTrait {
struct Success {};
struct Error {};
}  // namespace ResultTrait

static ResultTrait::Success g_execute_success;
static ResultTrait::Error g_execute_error;

template <typename ResultType, typename ErrorType>
class Result {
 public:
 public:
  Result() = delete;
  ~Result() = default;
  explicit Result(const ResultType& result) : result_wrapper_(result) {}
  explicit Result(ResultType&& result) : result_wrapper_(std::move(result)) {}
  explicit Result(const ErrorType& error) : result_wrapper_(error) {}
  explicit Result(ErrorType&& error) : result_wrapper_(std::move(error)) {}
  template <typename... Args>
  explicit Result(ResultTrait::Success success, Args... args)
      : result_wrapper_(success, std::forward<Args>(args)...) {}
  template <typename... Args>
  explicit Result(ResultTrait::Error error, Args... args)
      : result_wrapper_(error, std::forward<Args>(args)...) {}
  Result(const Result& other) = default;
  Result(Result&& other) noexcept
      : result_wrapper_(std::move(other.result_wrapper_)) {}
  Result& operator=(const Result& other) {
    result_wrapper_ = other.result_wrapper_;
  }
  Result& operator=(Result&& other) noexcept {
    result_wrapper_ = other.result_wrapper_;
  }

 public:
  ErrorType& GetError() { return result_wrapper_.GetError(); }

  const ErrorType& GetError() const { return result_wrapper_.GetError(); }

  ResultType& GetResult() { return result_wrapper_.GetResult(); }

  const ResultType& GetResult() const { return result_wrapper_.GetResult(); }

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

  template <typename CallBackType>
  Result& Exception(CallBackType&& callback) {
    result_wrapper_.Exception(callback);

    return *this;
  }

  template <typename CallBackType>
  const Result& Exception(CallBackType&& callback) const {
    result_wrapper_.Exception(callback);

    return *this;
  }

  void IgnoreException() { result_wrapper_.IgnoreException(); }

  bool Success() const { return result_wrapper_.Success(); }

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
                  << "!!!!!!!!!!! Have exception not processed. !!!!!!!!!!\n"
                  << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                  << std::endl;
        abort();
      }
#endif
    };
    explicit ResultWrapper(const ResultType& result)
        : result_(result),
          error_()
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(true)
#endif
    {
    }
    explicit ResultWrapper(ResultType&& result)
        : result_(std::move(result)),
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
    explicit ResultWrapper(ResultTrait::Success, Args... args)
        : result_(std::forward<Args>(args)...),
          error_()
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
          ,
          exception_processed_(true)
#endif
    {
    }
    template <typename... Args>
    explicit ResultWrapper(ResultTrait::Error, Args... args)
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
      other.exception_processed_ = true;
    }
    ResultWrapper& operator=(const ResultWrapper& other) {
      if (std::addressof(other) == this) {
        return *this;
      }

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

      result_ = std::move(other.result_);
      error_ = std::move(other.error_);
#ifdef MM_CHECK_ALL_EXCEPTION_PROCESS
      exception_processed_ = other.exception_processed_;
      other.exception_processed_ = true;
#endif

      return *this;
    }

   public:
    explicit operator bool() const { return error_.Success(); }

   public:
    ErrorTypeIn& GetError() { return error_; }

    const ErrorTypeIn& GetError() const { return error_; }

    ResultTypeIn& GetResult() { return result_; }

    const ResultTypeIn& GetResult() const { return result_; }

    void Exception() {
      if (error_.Success()) {
        return;
      }

      error_.Exception();
    }

    void Exception() const {
      if (error_.Success()) {
        return;
      }

      error_.Exception();
    }

    void Exception(void callback(ResultType&, const ErrorType&)) {
      if (error_.Success()) {
        return;
      }

      error_.Exception();
      callback(result_, error_);
    }

    void Exception(void callback(ErrorType&)) {
      if (error_.Success()) {
        return;
      }

      error_.Exception();
      callback(error_);
    }

    void Exception(void callback(const ErrorType&)) {
      if (error_.Success()) {
        return;
      }

      error_.Exception();
      callback(error_);
    }

    void Exception(void callback(const ErrorType&)) const {
      if (error_.Success()) {
        return;
      }

      error_.Exception();
      callback(error_);
    }

    template <typename CallBackType>
    void Exception(CallBackType&& callback) {
      constexpr bool callback_signature1 =
          std::is_invocable_r_v<void, CallBackType, ResultType&,
                                const ErrorType&>;
      constexpr bool callback_signature2 =
          std::is_invocable_r_v<void, CallBackType, ErrorType&>;
      constexpr bool callback_signature3 =
          std::is_invocable_r_v<void, CallBackType, const ErrorType&>;

      static_assert(
          callback_signature1 || callback_signature2 || callback_signature3,
          "Callback signature is invalid.");

      error_.Exception();

      if constexpr (callback_signature1) {
        callback(result_, error_);
        return;
      } else if constexpr (callback_signature2) {
        callback(error_);
        return;
      } else if constexpr (callback_signature3) {
        callback(error_);
        return;
      }
    }

    template <typename CallBackType>
    void Exception(CallBackType&& callback) const {
      constexpr bool callback_signature =
          std::is_invocable_r_v<void, CallBackType, const ErrorType&>;
      static_assert(callback_signature, "Callback signature is invalid.");

      error_.Exception();
      callback(error_);
    }

    void IgnoreException() { exception_processed_ = true; }

    bool Success() const { return error_.Success(); }

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
}  // namespace Utils
}  // namespace MM
