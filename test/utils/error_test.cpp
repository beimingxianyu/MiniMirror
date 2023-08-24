//
// Created by beimingxianyu on 23-8-20.
//
#include "utils/error.h"

#include <gtest/gtest.h>

struct UtilsErrorCustomStruct {
  UtilsErrorCustomStruct() = default;
  ~UtilsErrorCustomStruct() = default;
  UtilsErrorCustomStruct(const std::string& s1) : s1_(s1), s2_(), vs_() {}
  UtilsErrorCustomStruct(const std::string& s1, const std::string& s2)
      : s1_(s1), s2_(s2), vs_() {}
  UtilsErrorCustomStruct(const std::vector<std::string>& vs) : vs_(vs) {}
  UtilsErrorCustomStruct(const UtilsErrorCustomStruct& other) = default;
  UtilsErrorCustomStruct(UtilsErrorCustomStruct&& other) noexcept = default;
  UtilsErrorCustomStruct& operator=(const UtilsErrorCustomStruct& other) =
      default;
  UtilsErrorCustomStruct& operator=(UtilsErrorCustomStruct&& other) noexcept =
      default;

  bool operator==(const UtilsErrorCustomStruct& rhs) const {
    return s1_ == rhs.s1_ && s2_ == rhs.s2_ && vs_ == rhs.vs_;
  }

  bool operator!=(const UtilsErrorCustomStruct& rhs) const {
    return !(rhs == *this);
  }

  std::string s1_{"string 1"};
  std::string s2_{"string 2"};
  std::vector<std::string> vs_{"string 3", "string 4"};
};

struct UtilsErrorCustomStructNoCopy {
  UtilsErrorCustomStructNoCopy() = default;
  ~UtilsErrorCustomStructNoCopy() = default;
  UtilsErrorCustomStructNoCopy(const std::string& s1) : s1_(s1), s2_(), vs_() {}
  UtilsErrorCustomStructNoCopy(const std::string& s1, const std::string& s2)
      : s1_(s1), s2_(s2), vs_() {}
  UtilsErrorCustomStructNoCopy(const std::vector<std::string>& vs) : vs_(vs) {}
  UtilsErrorCustomStructNoCopy(const UtilsErrorCustomStructNoCopy& other) =
      delete;
  UtilsErrorCustomStructNoCopy(UtilsErrorCustomStructNoCopy&& other) noexcept =
      default;
  UtilsErrorCustomStructNoCopy& operator=(
      const UtilsErrorCustomStructNoCopy& other) = delete;
  UtilsErrorCustomStructNoCopy& operator=(
      UtilsErrorCustomStructNoCopy&& other) noexcept = default;

  bool operator==(const UtilsErrorCustomStructNoCopy& rhs) const {
    return s1_ == rhs.s1_ && s2_ == rhs.s2_ && vs_ == rhs.vs_;
  }

  bool operator!=(const UtilsErrorCustomStructNoCopy& rhs) const {
    return !(rhs == *this);
  }

  std::string s1_{"string 1"};
  std::string s2_{"string 2"};
  std::vector<std::string> vs_{"string 3", "string 4"};
};

struct UtilsErrorCustomStructNoMove {
  UtilsErrorCustomStructNoMove() = default;
  ~UtilsErrorCustomStructNoMove() = default;
  UtilsErrorCustomStructNoMove(const std::string& s1) : s1_(s1), s2_(), vs_() {}
  UtilsErrorCustomStructNoMove(const std::string& s1, const std::string& s2)
      : s1_(s1), s2_(s2), vs_() {}
  UtilsErrorCustomStructNoMove(const std::vector<std::string>& vs) : vs_(vs) {}
  UtilsErrorCustomStructNoMove(const UtilsErrorCustomStructNoMove& other) =
      default;
  UtilsErrorCustomStructNoMove(UtilsErrorCustomStructNoMove&& other) noexcept =
      delete;
  UtilsErrorCustomStructNoMove& operator=(
      const UtilsErrorCustomStructNoMove& other) = default;
  UtilsErrorCustomStructNoMove& operator=(
      UtilsErrorCustomStructNoMove&& other) noexcept = delete;

  bool operator==(const UtilsErrorCustomStructNoMove& rhs) const {
    return s1_ == rhs.s1_ && s2_ == rhs.s2_ && vs_ == rhs.vs_;
  }

  bool operator!=(const UtilsErrorCustomStructNoMove& rhs) const {
    return !(rhs == *this);
  }

  std::string s1_{"string 1"};
  std::string s2_{"string 2"};
  std::vector<std::string> vs_{"string 3", "string 4"};
};

class NewError final : public MM::Utils::ErrorTypeBase {
 public:
  NewError() = default;
  ~NewError() = default;
  NewError(std::uint32_t num) : num_(num) {}
  NewError(const NewError& other) = default;
  NewError(NewError&& other) noexcept = default;
  NewError& operator=(const NewError& other) = default;
  NewError& operator=(NewError&& other) noexcept = default;

 public:
  bool operator==(const NewError& rhs) const { return num_ == rhs.num_; }

  bool operator!=(const NewError& rhs) const { return !(rhs == *this); }

 public:
  void Exception() override {}

  void Exception() const override {}

  bool Success() const override { return num_ == 0; }

  void Reset() override { num_ = 0; }

 private:
  std::uint32_t num_{0};
};

MM::Utils::Result<UtilsErrorCustomStruct, MM::Utils::ErrorNil>
ReturnNilSuccess() {
  return MM::Utils::Result<UtilsErrorCustomStruct, MM::Utils::ErrorNil>(
      UtilsErrorCustomStruct{});
}

MM::Utils::Result<UtilsErrorCustomStruct, MM::Utils::ErrorNil>
ReturnNilFailed() {
  return MM::Utils::Result<UtilsErrorCustomStruct, MM::Utils::ErrorNil>(
      MM::Utils::ErrorNil{false});
}

MM::Utils::Result<UtilsErrorCustomStruct, MM::Utils::ErrorResult>
ReturnCodeSuccess() {
  return MM::Utils::Result<UtilsErrorCustomStruct,
                           MM::Utils::ErrorResult>(
      UtilsErrorCustomStruct{});
}

MM::Utils::Result<UtilsErrorCustomStruct, MM::Utils::ErrorResult>
ReturnCodeFailed() {
  return MM::Utils::Result<UtilsErrorCustomStruct,
                           MM::Utils::ErrorResult>(
      MM::Utils::ErrorResult{
          MM::Utils::ErrorCode::UNDEFINED_ERROR});
}

void ReturnCodeExceptionCallback(
    UtilsErrorCustomStruct& result,
    const MM::Utils::ErrorResult& error) {
  result = UtilsErrorCustomStruct("ReturnCodeExceptionCallback");
}

MM::Utils::Result<UtilsErrorCustomStructNoCopy, MM::Utils::ErrorNil>
ReturnNilSuccessNoCopy() {
  return MM::Utils::Result<UtilsErrorCustomStructNoCopy, MM::Utils::ErrorNil>(
      UtilsErrorCustomStructNoCopy{});
}

MM::Utils::Result<UtilsErrorCustomStructNoCopy, MM::Utils::ErrorNil>
ReturnNilFailedNoCopy() {
  return MM::Utils::Result<UtilsErrorCustomStructNoCopy, MM::Utils::ErrorNil>(
      MM::Utils::ErrorNil{false});
}

void ReturnNilFailedNoCopyExceptionCallback(MM::Utils::ErrorNil& error) {
  error.Reset();
}

MM::Utils::Result<UtilsErrorCustomStructNoCopy,
                  MM::Utils::ErrorResult>
ReturnCodeSuccessNoCopy() {
  return MM::Utils::Result<UtilsErrorCustomStructNoCopy,
                           MM::Utils::ErrorResult>(
      UtilsErrorCustomStructNoCopy{});
}

MM::Utils::Result<UtilsErrorCustomStructNoCopy,
                  MM::Utils::ErrorResult>
ReturnCodeFailedNoCopy() {
  return MM::Utils::Result<UtilsErrorCustomStructNoCopy,
                           MM::Utils::ErrorResult>(
      MM::Utils::ErrorResult{
          MM::Utils::ErrorCode::UNDEFINED_ERROR});
}

MM::Utils::Result<UtilsErrorCustomStructNoMove, MM::Utils::ErrorNil>
ReturnNilSuccessNoMove() {
  return MM::Utils::Result<UtilsErrorCustomStructNoMove, MM::Utils::ErrorNil>(
          MM::Utils::c_execute_success, std::string("ReturnNilSuccessNoMove"));
}

MM::Utils::Result<UtilsErrorCustomStructNoMove, MM::Utils::ErrorNil>
ReturnNilFailedNoMove() {
  return MM::Utils::Result<UtilsErrorCustomStructNoMove, MM::Utils::ErrorNil>(
      MM::Utils::ErrorNil{false});
}

MM::Utils::Result<UtilsErrorCustomStructNoMove,
                  MM::Utils::ErrorResult>
ReturnCodeSuccessNoMove() {
  return MM::Utils::Result<UtilsErrorCustomStructNoMove,
                           MM::Utils::ErrorResult>(
          MM::Utils::c_execute_success, std::string("ReturnCodeSuccessNoMove"));
}

MM::Utils::Result<UtilsErrorCustomStructNoMove,
                  MM::Utils::ErrorResult>
ReturnCodeFailedNoMove() {
  return MM::Utils::Result<UtilsErrorCustomStructNoMove,
                           MM::Utils::ErrorResult>(
      MM::Utils::ErrorResult(
          MM::Utils::ErrorCode::UNDEFINED_ERROR));
}

MM::Utils::Result<UtilsErrorCustomStruct, NewError> ReturnNewErrorSuccess() {
  return MM::Utils::Result<UtilsErrorCustomStruct, NewError>(
      UtilsErrorCustomStruct{});
}

MM::Utils::Result<UtilsErrorCustomStruct, NewError> ReturnNewErrorFailed() {
  return MM::Utils::Result<UtilsErrorCustomStruct, NewError>(NewError{1});
}

MM::Utils::Result<UtilsErrorCustomStruct, NewError>
ReturnUtilsErrorCustomErrorConstructor1(const std::string& s1) {
  return MM::Utils::Result<UtilsErrorCustomStruct, NewError>(
          MM::Utils::c_execute_success, s1);
}

MM::Utils::Result<UtilsErrorCustomStruct, NewError>
ReturnUtilsErrorCustomErrorConstructor2(const std::string& s1,
                                        const std::string& s2) {
  return MM::Utils::Result<UtilsErrorCustomStruct, NewError>(
          MM::Utils::c_execute_success, s1, s2);
}

MM::Utils::Result<UtilsErrorCustomStruct, NewError>
ReturnUtilsErrorCustomErrorConstructor3(const std::vector<std::string>& vs) {
  return MM::Utils::Result<UtilsErrorCustomStruct, NewError>(
          MM::Utils::c_execute_success, vs);
}

MM::Utils::Result<UtilsErrorCustomStruct, NewError> ReturnNewErrorConstructor(
    std::uint32_t num) {
  return MM::Utils::Result<UtilsErrorCustomStruct, NewError>(
          MM::Utils::c_execute_error, num);
}

TEST(Utils, error) {
  UtilsErrorCustomStruct default_struct{},
      struct1{"ReturnUtilsErrorCustomErrorConstructor1"},
      struct2{"ReturnUtilsErrorCustomErrorConstructor1",
              "ReturnUtilsErrorCustomErrorConstructor2"},
      struct3{
          std::vector<std::string>{"ReturnUtilsErrorCustomErrorConstructor1",
                                   "ReturnUtilsErrorCustomErrorConstructor2"}};
  UtilsErrorCustomStructNoCopy default_struct_no_copy{};
  UtilsErrorCustomStructNoMove default_struct_no_move_success{
      std::string("ReturnNilSuccessNoMove")},
      default_struct_no_move_failed{},
      default_struct_no_move_success2{std::string("ReturnCodeSuccessNoMove")};
  NewError true_new_error{0};

  MM::Utils::ErrorNil true_nil(true), false_nil(false);
  MM::Utils::ErrorResult true_code{
      MM::Utils::ErrorCode::SUCCESS},
      false_code{MM::Utils::ErrorCode::UNDEFINED_ERROR};
  std::uint32_t expection_in = 0;

  auto return_nil_success = ReturnNilSuccess(),
       return_nil_failed = ReturnNilFailed();
  EXPECT_EQ(return_nil_success.Success(), true);
  EXPECT_EQ(return_nil_success.GetResult(), default_struct);
  EXPECT_EQ(return_nil_success.GetError(), true_nil);
  return_nil_success.Exception();
  EXPECT_EQ(return_nil_failed.Success(), false);
  EXPECT_EQ(return_nil_failed.GetResult(), default_struct);
  EXPECT_EQ(return_nil_failed.GetError(), false_nil);
  return_nil_failed.Exception();

  auto return_code_success = ReturnCodeSuccess(),
       return_code_failed = ReturnCodeFailed();
  EXPECT_EQ(return_code_success.Success(), true);
  EXPECT_EQ(return_code_success.GetResult(), default_struct);
  EXPECT_EQ(return_code_success.GetError(), true_code);
  return_code_success.Exception(ReturnCodeExceptionCallback);
  EXPECT_EQ(return_code_success.GetResult(), default_struct);
  EXPECT_EQ(return_code_failed.Success(), false);
  EXPECT_EQ(return_code_failed.GetResult(), default_struct);
  EXPECT_EQ(return_code_failed.GetError(), false_code);
  return_code_failed.Exception(ReturnCodeExceptionCallback);
  EXPECT_EQ(return_code_failed.GetResult(),
            UtilsErrorCustomStruct("ReturnCodeExceptionCallback"));

  auto return_nil_success_no_copy = ReturnNilSuccessNoCopy(),
       return_nil_failed_no_copy = ReturnNilFailedNoCopy();
  EXPECT_EQ(return_nil_success_no_copy.Success(), true);
  EXPECT_EQ(return_nil_success_no_copy.GetResult(), default_struct_no_copy);
  EXPECT_EQ(return_nil_success_no_copy.GetError(), true_nil);
  return_nil_success_no_copy.Exception(ReturnNilFailedNoCopyExceptionCallback);
  EXPECT_EQ(return_nil_success_no_copy.GetError(), true_nil);
  EXPECT_EQ(return_nil_failed_no_copy.Success(), false);
  EXPECT_EQ(return_nil_failed_no_copy.GetResult(), default_struct_no_copy);
  EXPECT_EQ(return_nil_failed_no_copy.GetError(), false_nil);
  return_nil_failed_no_copy.Exception(ReturnNilFailedNoCopyExceptionCallback);
  EXPECT_EQ(return_nil_success_no_copy.GetError(), true_nil);

  auto return_code_success_no_copy = ReturnCodeSuccessNoCopy(),
       return_code_failed_no_copy = ReturnCodeFailedNoCopy();
  auto return_code_lambda =
      [&expection_in](UtilsErrorCustomStructNoCopy& result,
                      const MM::Utils::ErrorResult& error) {
        ++expection_in;
        return;
      };
  EXPECT_EQ(return_code_success_no_copy.Success(), true);
  EXPECT_EQ(return_code_success_no_copy.GetResult(), default_struct_no_copy);
  EXPECT_EQ(return_code_success_no_copy.GetError(), true_code);
  return_code_success_no_copy.Exception(return_code_lambda);
  EXPECT_EQ(expection_in, 0);
  EXPECT_EQ(return_code_failed_no_copy.Success(), false);
  EXPECT_EQ(return_code_failed_no_copy.GetResult(), default_struct_no_copy);
  EXPECT_EQ(return_code_failed_no_copy.GetError(), false_code);
  return_code_failed_no_copy.Exception(return_code_lambda);
  EXPECT_EQ(expection_in, 1);
  expection_in = 0;

  auto return_nil_success_no_move = ReturnNilSuccessNoMove(),
       return_nil_failed_no_move = ReturnNilFailedNoMove();
  auto return_nil_lambda = [&expection_in](UtilsErrorCustomStructNoMove& result,
                                           const MM::Utils::ErrorNil& error) {
    ++expection_in;
    return;
  };
  EXPECT_EQ(return_nil_success_no_move.Success(), true);
  EXPECT_EQ(return_nil_success_no_move.GetResult(),
            default_struct_no_move_success);
  EXPECT_EQ(return_nil_success_no_move.GetError(), true_nil);
  return_nil_success_no_move.Exception(return_nil_lambda);
  EXPECT_EQ(expection_in, 0);
  EXPECT_EQ(return_nil_failed_no_move.Success(), false);
  EXPECT_EQ(return_nil_failed_no_move.GetResult(),
            default_struct_no_move_failed);
  EXPECT_EQ(return_nil_failed_no_move.GetError(), false_nil);
  return_nil_failed_no_move.Exception(return_nil_lambda);
  EXPECT_EQ(expection_in, 1);

  auto return_code_success_no_move = ReturnCodeSuccessNoMove(),
       return_code_failed_no_move = ReturnCodeFailedNoMove();
  EXPECT_EQ(return_code_success_no_move.Success(), true);
  EXPECT_EQ(return_code_success_no_move.GetResult(),
            default_struct_no_move_success2);
  EXPECT_EQ(return_code_success_no_move.GetError(), true_code);
  return_code_success_no_move.Exception();
  EXPECT_EQ(return_code_failed_no_move.Success(), false);
  EXPECT_EQ(return_code_failed_no_move.GetResult(),
            default_struct_no_move_failed);
  EXPECT_EQ(return_code_failed_no_move.GetError(), false_code);
  return_code_failed_no_move.Exception();

  auto return_new_error_success = ReturnNewErrorSuccess(),
       return_new_error_failed = ReturnNewErrorFailed();
  EXPECT_EQ(return_new_error_success.Success(), true);
  EXPECT_EQ(return_new_error_success.GetResult(), default_struct);
  EXPECT_EQ(return_new_error_success.GetError(), true_new_error);
  return_new_error_success.Exception();
  EXPECT_EQ(return_new_error_failed.Success(), false);
  EXPECT_EQ(return_new_error_failed.GetResult(), default_struct);
  EXPECT_EQ(return_new_error_failed.GetError(), NewError(1));
  return_new_error_failed.Exception();

  auto return_utils_error_custom_error_constructor1 =
           ReturnUtilsErrorCustomErrorConstructor1(
               "ReturnUtilsErrorCustomErrorConstructor1"),
       return_utils_error_custom_error_constructor2 =
           ReturnUtilsErrorCustomErrorConstructor2(
               "ReturnUtilsErrorCustomErrorConstructor1",
               "ReturnUtilsErrorCustomErrorConstructor2"),
       return_utils_error_custom_error_constructor3 =
           ReturnUtilsErrorCustomErrorConstructor3(
               {"ReturnUtilsErrorCustomErrorConstructor1",
                "ReturnUtilsErrorCustomErrorConstructor2"}),
       return_new_error_constructor = ReturnNewErrorConstructor(50);
  EXPECT_EQ(return_utils_error_custom_error_constructor1.Success(), true);
  EXPECT_EQ(return_utils_error_custom_error_constructor1.GetResult(), struct1);
  EXPECT_EQ(return_utils_error_custom_error_constructor1.GetError(),
            true_new_error);
  return_utils_error_custom_error_constructor1.Exception();
  EXPECT_EQ(return_utils_error_custom_error_constructor2.Success(), true);
  EXPECT_EQ(return_utils_error_custom_error_constructor2.GetResult(), struct2);
  EXPECT_EQ(return_utils_error_custom_error_constructor2.GetError(),
            true_new_error);
  return_utils_error_custom_error_constructor2.Exception();
  EXPECT_EQ(return_utils_error_custom_error_constructor3.Success(), true);
  EXPECT_EQ(return_utils_error_custom_error_constructor3.GetResult(), struct3);
  EXPECT_EQ(return_utils_error_custom_error_constructor3.GetError(),
            true_new_error);
  return_utils_error_custom_error_constructor3.Exception();
  EXPECT_EQ(return_new_error_constructor.Success(), false);
  EXPECT_EQ(return_new_error_constructor.GetResult(), default_struct);
  EXPECT_EQ(return_new_error_constructor.GetError(), 50);
  return_new_error_constructor.Exception();
}
