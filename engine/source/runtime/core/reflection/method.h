#pragma once

#include "runtime/core/reflection/type.h"
#include "runtime/core/reflection/variable.h"
#include "utils/utils.h"

namespace MM {
namespace Reflection {
class MethodWrapperBase {
 public:
  MethodWrapperBase() = default;
  virtual ~MethodWrapperBase() = default;
  MethodWrapperBase(const MethodWrapperBase& other) = default;
  MethodWrapperBase(MethodWrapperBase&& other) noexcept = default;
  MethodWrapperBase& operator=(const MethodWrapperBase& other) = default;
  MethodWrapperBase& operator=(MethodWrapperBase&& other) noexcept = default;

public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The hash value depends on the return value type of the function, the type of all parameters, the location of the parameters, the type of the class to which it belongs, and whether it is a static function.
   */
  virtual std::size_t HashCode() const = 0;

  /**
   * \brief Determine whether the function is static.
   * \return If the function is static, it returns true, otherwise it returns
   * false.
   */
  virtual bool IsStatic() const = 0;

  /**
   * \brief Returns the number of arguments to the function.
   * \return The number of arguments to the function.
   */
  virtual std::size_t GetArgumentNumber() const = 0;

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \return The \ref MM::Reflection::Type of result type.
   */
  virtual Type GetReturnType() const = 0;

  /**
   * \brief Get the \ref MM::Reflection::Type of class type.
   * \return The \ref MM::Reflection::Type of class type.
   */
  virtual Type GetClassType() const = 0;

  /**
   * \brief Get the \ref MM::Reflection::Meta of result type.
   * \return The \ref MM::Reflection::Meta of result type.
   */
  virtual std::shared_ptr<Meta> GetReturnMeta() const = 0;

  /**
   * \brief Get the \ref MM::Reflection::Meta of class type.
   * \return The \ref MM::Reflection::Meta of class type.
   */
  virtual std::shared_ptr<Meta> GetClassMeta() const = 0;

  /**
   * \brief Invoke the function with 0 arguments.
   * \param instance Instance that calls this function.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance) const = 0;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance, Variable& arg1) const = 0;

  /**
   * \brief Invoke the function with 2 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance, Variable& arg1,
                          Variable& arg2) const = 0;

  /**
   * \brief Invoke the function with 3 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \param arg3 3rd argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                          Variable& arg3) const = 0;

  /**
   * \brief Invoke the function with 4 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                          Variable& arg3, Variable& arg4) const = 0;

  /**
   * \brief Invoke the function with 5 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                          Variable& arg3, Variable& arg4,
                          Variable& arg5) const = 0;

  /**
   * \brief Invoke the function with 6 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \param arg6 6th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                          Variable& arg3, Variable& arg4, Variable& arg5,
                          Variable& arg6) const = 0;

  /**
   * \brief Call the function with any number of parameters.
   * \param instance instance Instance that calls this function.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& instance,
                          std::vector<Variable>& args) const = 0;
};

template <typename InstanceType_, typename ResultType_, bool IsStatic_,
          typename... Args_>
class MethodWrapper : public MethodWrapperBase {};

template <typename InstanceType_, typename ResultType_, typename... Args_>
class MethodWrapper<InstanceType_, ResultType_, false, Args_...>
    : public MethodWrapperBase {
 public:
  using InstanceType = InstanceType_;
  using ResultType = ResultType_;
  using ArgumentsType = std::tuple<Args_...>;
  using FunctionType = ResultType_ (InstanceType_::*)(Args_...);

  template <std::size_t index>
  struct GetArgumentType {
    using Types = std::tuple<Args_...>;
    using Type = typename std::tuple_element<index, Types>::type;
  };

public:
  MethodWrapper();
  ~MethodWrapper() override;
  MethodWrapper(const MethodWrapper& other);
  MethodWrapper(MethodWrapper&& other) noexcept;
  MethodWrapper& operator=(const MethodWrapper& other);
  MethodWrapper& operator=(MethodWrapper&& other) noexcept;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The hash value depends on the return value type of the function,
   * the type of all parameters, the location of the parameters, the type of the
   * class to which it belongs, and whether it is a static function.
   */
   std::size_t HashCode() const override;

  /**
   * \brief Determine whether the function is static.
   * \return If the function is static, it returns true, otherwise it returns
   * false.
   */
  bool IsStatic() const override;

  /**
   * \brief Returns the number of arguments to the function.
   * \return The number of arguments to the function.
   */
  std::size_t GetArgumentNumber() const override;

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \return The \ref MM::Reflection::Type of result type.
   */
  Type GetReturnType() const override;

  /**
   * \brief Get the \ref MM::Reflection::Type of class type.
   * \return The \ref MM::Reflection::Type of class type.
   */
  Type GetClassType() const override;

  /**
   * \brief Get the \ref MM::Reflection::Meta of result type.
   * \return The \ref MM::Reflection::Meta of result type.
   */
  std::shared_ptr<Meta> GetReturnMeta() const override;

  /**
   * \brief Get the \ref MM::Reflection::Meta of class type.
   * \return The \ref MM::Reflection::Meta of class type.
   */
  std::shared_ptr<Meta> GetClassMeta() const override;

  /**
   * \brief Invoke the function with 0 arguments.
   * \param instance Instance that calls this function.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1) const override;

  /**
   * \brief Invoke the function with 2 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1,
                  Variable& arg2) const override;

  /**
   * \brief Invoke the function with 3 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \param arg3 3rd argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3) const override;

  /**
   * \brief Invoke the function with 4 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4) const override;

  /**
   * \brief Invoke the function with 5 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4, Variable& arg5) const override;

  /**
   * \brief Invoke the function with 6 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \param arg6 6th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4, Variable& arg5,
                  Variable& arg6) const override;

  /**
   * \brief Call the function with any number of parameters.
   * \param instance instance Instance that calls this function.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance,
                  std::vector<Variable>& args) const override;

 private:
  bool AllTypeSame(const Variable& var1) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
        (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
            .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3, const Variable& var4) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode(),
        var4.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<3>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3, const Variable& var4,
                   const Variable& var5) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode(),
        var4.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<3>::Type>())
                .GetTypeHashCode(),
        var5.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<4>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3, const Variable& var4,
                   const Variable& var5, const Variable& var6) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode(),
        var4.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<3>::Type>())
                .GetTypeHashCode(),
        var5.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<4>::Type>())
                .GetTypeHashCode(),
        var6.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<5>::Type>())
                .GetTypeHashCode());
  }

  template <std::size_t... ArgIdx>
  bool AllTypeSameFromVector(IndexSequence<ArgIdx...>,
                             const std::vector<Variable>& vars) {
    return MM::Utils::CheckAllTrue(
        (vars[ArgIdx].GetType().GetTypeHashCode() ==
         (MM::Reflection::CreateType<typename GetArgumentType<ArgIdx>::Type>())
             .GetTypeHashCode())...);
  }

  bool AllTypeSame(const std::vector<Variable>& vars) {
    return AllTypeSameFromVector(MakeIndexSequence<sizeof...(Args_)>(), vars);
  }

  template <bool TestCount, bool HaveReturn, std::size_t ArgsCount>
  struct InvokeImpHelp;

  template <>
  struct InvokeImpHelp<true, true, 0> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object) {
      return Variable{(method_wrapper.function_ptr_)()};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 0> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object) {
      (method_wrapper.function_ptr_)();

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 0> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 1> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 1> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 1> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 2> {
    Variable operator()(MethodWrapper<InstanceType_, ResultType_, false,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 2> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 2> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 3> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 3> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 3> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 4> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 4> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 4> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 5> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 5> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 5> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 6> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5, void* arg6) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<5>::Type>::type>(arg6))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 6> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5, void* arg6) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<5>::Type>::type>(arg6));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 6> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5, void* arg6) {
      return Variable{};
    }
  };

  template <typename... Vars_>
  Variable InvokeImp(void* instance, Vars_... vars) const;

  template <bool HaveReturn>
  struct InvokeVariadicImpHelp {};

  template <>
  struct InvokeVariadicImpHelp<true> {
    template <std::size_t... ArgsIdx>
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, const std::vector<Variable>& vars,
                        IndexSequence<ArgsIdx...>) {
      return Variable{(method_wrapper.function_ptr_)(
          (*static_cast<
              std::add_pointer<typename GetArgumentType<ArgsIdx>::Type>::type>(
              vars[ArgsIdx].GetValue()))...)};
    }
  };

  template <>
  struct InvokeVariadicImpHelp<false> {
    template <std::size_t... ArgsIdx>
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, const std::vector<Variable>& vars,
                        IndexSequence<ArgsIdx...>) {
      (method_wrapper.function_ptr_)(
          (*static_cast<
              std::add_pointer<typename GetArgumentType<ArgsIdx>::Type>::type>(
              vars[ArgsIdx].GetValue()))...);
      return Variable{};
    }
  };
  Variable InvokeVariadicImp(void* object, const std::vector<Variable>& vars) const;

private:
  ResultType_ (InstanceType_::*function_ptr_)(Args_...) = nullptr;
};

template <typename InstanceType_, typename ResultType_, typename... Args_>
class MethodWrapper<InstanceType_, ResultType_, true, Args_...>
    : public MethodWrapperBase {
 public:
  using InstanceType = InstanceType_;
  using ResultType = ResultType_;
  using ArgumentsType = std::tuple<Args_...>;
  using FunctionType = ResultType_ (*)(Args_...);

  template <std::size_t index>
  struct GetArgumentType {
    using Types = std::tuple<Args_...>;

    struct ElementType {
      using Type = typename std::tuple_element<index, Types>::type;
    };
  };

public:
  MethodWrapper();
  ~MethodWrapper() override;
  MethodWrapper(const MethodWrapper& other);
  MethodWrapper(MethodWrapper&& other) noexcept;
  MethodWrapper& operator=(const MethodWrapper& other);
  MethodWrapper& operator=(MethodWrapper&& other) noexcept;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The hash value depends on the return value type of the function,
   * the type of all parameters, the location of the parameters, the type of the
   * class to which it belongs, and whether it is a static function.
   */
   std::size_t HashCode() const override;

  /**
   * \brief Determine whether the function is static.
   * \return If the function is static, it returns true, otherwise it returns
   * false.
   */
  bool IsStatic() const override;

  /**
   * \brief Returns the number of arguments to the function.
   * \return The number of arguments to the function.
   */
  std::size_t GetArgumentNumber() const override;

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \return The \ref MM::Reflection::Type of result type.
   */
  Type GetReturnType() const override;

  /**
   * \brief Get the \ref MM::Reflection::Type of class type.
   * \return The \ref MM::Reflection::Type of class type.
   */
  Type GetClassType() const override;

  /**
   * \brief Get the \ref MM::Reflection::Meta of result type.
   * \return The \ref MM::Reflection::Meta of result type.
   */
  std::shared_ptr<Meta> GetReturnMeta() const override;

  /**
   * \brief Get the \ref MM::Reflection::Meta of class type.
   * \return The \ref MM::Reflection::Meta of class type.
   */
  std::shared_ptr<Meta> GetClassMeta() const override;

  /**
   * \brief Invoke the function with 0 arguments.
   * \param instance Instance that calls this function.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1,
                  Variable& arg2) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \param arg3 3rd argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4, Variable& arg5) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \param arg6 6th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4, Variable& arg5,
                  Variable& arg6) const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance,
                  std::vector<Variable>& args) const override;

 private:
  bool AllTypeSame(const Variable& var1) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
        (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
            .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3, const Variable& var4) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode(),
        var4.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<3>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3, const Variable& var4,
                   const Variable& var5) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode(),
        var4.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<3>::Type>())
                .GetTypeHashCode(),
        var5.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<4>::Type>())
                .GetTypeHashCode());
  }

  bool AllTypeSame(const Variable& var1, const Variable& var2,
                   const Variable& var3, const Variable& var4,
                   const Variable& var5, const Variable& var6) {
    return MM::Utils::CheckAllTrue(
        var1.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<0>::Type>())
                .GetTypeHashCode(),
        var2.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<1>::Type>())
                .GetTypeHashCode(),
        var3.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<2>::Type>())
                .GetTypeHashCode(),
        var4.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<3>::Type>())
                .GetTypeHashCode(),
        var5.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<4>::Type>())
                .GetTypeHashCode(),
        var6.GetType().GetTypeHashCode() ==
            (MM::Reflection::CreateType<typename GetArgumentType<5>::Type>())
                .GetTypeHashCode());
  }

  template <std::size_t... ArgIdx>
  bool AllTypeSameFromVector(IndexSequence<ArgIdx...>,
                             const std::vector<Variable>& vars) {
    return MM::Utils::CheckAllTrue(
        (vars[ArgIdx].GetType().GetTypeHashCode() ==
         (MM::Reflection::CreateType<typename GetArgumentType<ArgIdx>::Type>())
             .GetTypeHashCode())...);
  }

  bool AllTypeSame(const std::vector<Variable>& vars) {
    return AllTypeSameFromVector(MakeIndexSequence<sizeof...(Args_)>(), vars);
  }

  template <bool TestCount, bool HaveReturn, std::size_t ArgsCount>
  struct InvokeImpHelp;

  template <>
  struct InvokeImpHelp<true, true, 0> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object) {
  return Variable{(method_wrapper.function_ptr_)()};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 0> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object) {
  (method_wrapper.function_ptr_)();

  return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 0> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object) {
  return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 1> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1) {
  return Variable{(method_wrapper.function_ptr_)(
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<0>::Type>::type>(
          arg1))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 1> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1) {
  (method_wrapper.function_ptr_)(
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<0>::Type>::type>(
          arg1));

  return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 1> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1) {
  return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 2> {
    Variable operator()(MethodWrapper<InstanceType_, ResultType_, false,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2) {
  return Variable{(method_wrapper.function_ptr_)(
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<0>::Type>::type>(
          arg1),
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<1>::Type>::type>(
          arg2))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 2> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2) {
  (method_wrapper.function_ptr_)(
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<0>::Type>::type>(
          arg1),
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<1>::Type>::type>(
          arg2));

  return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 2> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2) {
  return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 3> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                            Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3) {
  return Variable{(method_wrapper.function_ptr_)(
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<0>::Type>::type>(
          arg1),
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<1>::Type>::type>(
          arg2),
      *static_cast<
          typename std::add_pointer<typename GetArgumentType<2>::Type>::type>(
          arg3))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 3> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 3> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 4> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 4> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 4> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 5> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 5> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 5> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, true, 6> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5, void* arg6) {
      return Variable{(method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<5>::Type>::type>(arg6))};
    }
  };

  template <>
  struct InvokeImpHelp<true, false, 6> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5, void* arg6) {
      (method_wrapper.function_ptr_)(
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<0>::Type>::type>(arg1),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<1>::Type>::type>(arg2),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<2>::Type>::type>(arg3),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<3>::Type>::type>(arg4),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<4>::Type>::type>(arg5),
          *static_cast<typename std::add_pointer<
              typename GetArgumentType<5>::Type>::type>(arg6));

      return Variable{};
    }
  };

  template <bool HaveReturn>
  struct InvokeImpHelp<false, HaveReturn, 6> {
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, void* arg1, void* arg2, void* arg3,
                        void* arg4, void* arg5, void* arg6) {
      return Variable{};
    }
  };

  template <typename... Vars_>
  Variable InvokeImp(void* instance, Vars_... vars) const;

  template <bool HaveReturn>
  struct InvokeVariadicImpHelp {};

  template <>
  struct InvokeVariadicImpHelp<true> {
    template <std::size_t... ArgsIdx>
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, const std::vector<Variable>& vars,
                        IndexSequence<ArgsIdx...>) {
      return Variable{(method_wrapper.function_ptr_)(
          (*static_cast<
              std::add_pointer<typename GetArgumentType<ArgsIdx>::Type>::type>(
              vars[ArgsIdx].GetValue()))...)};
    }
  };

  template <>
  struct InvokeVariadicImpHelp<false> {
    template <std::size_t... ArgsIdx>
    Variable operator()(const MethodWrapper<InstanceType_, ResultType_, true,
                                      Args_...>& method_wrapper,
                        void* object, const std::vector<Variable>& vars,
                        IndexSequence<ArgsIdx...>) {
      (method_wrapper.function_ptr_)(
          (*static_cast<
              std::add_pointer<typename GetArgumentType<ArgsIdx>::Type>::type>(
              vars[ArgsIdx].GetValue()))...);
      return Variable{};
    }
  };

  Variable InvokeVariadicImp(void* object, const std::vector<Variable>& vars) const;

private:
  ResultType_ (*function_ptr_)(Args_...) = nullptr;
};

template <typename InstanceType_, typename ResultType_, typename ... Args_>
std::size_t MethodWrapper<InstanceType_, ResultType_, true, Args_...>::
HashCode() const {
  return AllTypeHashCode<InstanceType_, ResultType_, Args_...>() + 1;
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
MethodWrapper<InstanceType_, ResultType_, false, Args_...>::MethodWrapper() : function_ptr_(nullptr){}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
MethodWrapper<InstanceType_, ResultType_, false, Args_...>::~MethodWrapper() =
    default;

template <typename InstanceType_, typename ResultType_, typename ... Args_>
MethodWrapper<InstanceType_, ResultType_, false, Args_...>::MethodWrapper(
    const MethodWrapper& other) : function_ptr_(other.function_ptr_){}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
MethodWrapper<InstanceType_, ResultType_, false, Args_...>::MethodWrapper(
    MethodWrapper&& other) noexcept {
  std::swap(function_ptr_, other.function_ptr_);
  other.function_ptr_ = nullptr;
  return *this;
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
MethodWrapper<InstanceType_, ResultType_, false, Args_...>& MethodWrapper<
InstanceType_, ResultType_, false, Args_...>::operator=(
    const MethodWrapper& other) {
  if (this == &other) {
    return *this;
  }
  function_ptr_ = other.function_ptr_;
  return *this;
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
MethodWrapper<InstanceType_, ResultType_, false, Args_...>& MethodWrapper<
InstanceType_, ResultType_, false, Args_...>::operator=(
    MethodWrapper&& other) noexcept {
  if (this == &other) {
    return false;
  }
  std::swap(function_ptr_, other.function_ptr_);
  other.function_ptr_ = nullptr;
  return *this;
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
std::size_t MethodWrapper<InstanceType_, ResultType_, false, Args_...>::
HashCode() const {
  return AllTypeHashCode<InstanceType_, ResultType_, Args_...>() + 0;
}

template <typename InstanceType_, typename ResultType_, typename... Args>
bool MethodWrapper<InstanceType_, ResultType_, false, Args...>::IsStatic()
    const {
  return false;
}

template <typename InstanceType_, typename ResultType_, typename... Args>
std::size_t MethodWrapper<InstanceType_, ResultType_, false,
                          Args...>::GetArgumentNumber() const {
  return sizeof...(Args);
}

template <typename InstanceType_, typename ResultType_, typename... Args>
Type MethodWrapper<InstanceType_, ResultType_, false, Args...>::GetReturnType()
    const {
  return MM::Reflection::CreateType<ResultType_>();
}

template <typename InstanceType_, typename ResultType_, typename... Args>
Type MethodWrapper<InstanceType_, ResultType_, false, Args...>::GetClassType()
    const {
  return MM::Reflection::CreateType<InstanceType_>();
}

template <typename InstanceType_, typename ResultType_, typename... Args>
std::shared_ptr<Meta> MethodWrapper<InstanceType_, ResultType_, false,
                                    Args...>::GetReturnMeta() const {
  return GetReturnType().GetMate();
}

template <typename InstanceType_, typename ResultType_, typename... Args>
std::shared_ptr<Meta> MethodWrapper<InstanceType_, ResultType_, false,
                                    Args...>::GetClassMeta() const {
  return GetClassType().GetMate();
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance) const {
  return InvokeImp(instance.GetValue());
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance, Variable& arg1) const {
  if (AllTypeSame(arg1)) {
    return InvokeImp(instance.GetValue(), arg1.GetValue());
  }
  return Variable{};
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance, Variable& arg1, Variable& arg2) const {
  if (AllTypeSame(arg1, arg2)) {
    return InvokeImp(instance.GetValue(), arg1.GetValue(), arg2.GetValue());
  }
  return Variable{};
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance, Variable& arg1, Variable& arg2, Variable& arg3) const {
  if (AllTypeSame(arg1, arg2, arg3)) {
    return InvokeImp(instance.GetValue(), arg2.GetValue(), arg3.GetValue());
  }
  return Variable{};
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance, Variable& arg1, Variable& arg2, Variable& arg3,
    Variable& arg4) const {
  if (AllTypeSame(arg1, arg2, arg3, arg4)) {
    return InvokeImp(instance.GetValue(), arg2.GetValue(), arg3.GetValue(),
                     arg4.GetValue());
  }
  return Variable{};
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance, Variable& arg1, Variable& arg2, Variable& arg3,
    Variable& arg4, Variable& arg5) const {
  if (AllTypeSame(arg1, arg2, arg3, arg4, arg5)) {
    return InvokeImp(instance.GetValue(), arg1.GetValue(), arg2.GetValue(), arg3.GetValue(), arg4.GetValue(), arg5.GetValue());
  }
  return Variable{};
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance, Variable& arg1, Variable& arg2, Variable& arg3,
    Variable& arg4, Variable& arg5, Variable& arg6) const {
  if (AllTypeSame(arg1, arg2, arg3, arg4, arg5, arg6)) {
    return InvokeImp(instance.GetValue(), arg1.GetValue(), arg2.GetValue(),
                     arg3.GetValue(), arg4.GetValue(), arg5.GetValue(),
                     arg6.GetValue());
  }
  return Variable{};
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, false, Args_...>::Invoke(
    Variable& instance, std::vector<Variable>& args) const {
  if (AllTypeSameFromVector(MakeIndexSequence<sizeof...(Args_)>(), args)) {
    return InvokeVariadicImp(instance.GetValue(), args);
  }
  return Variable{};
}

template <typename InstanceType_, typename ResultType_, typename ... Args_>
Variable MethodWrapper<InstanceType_, ResultType_, true, Args_...>::
InvokeVariadicImp(void* object, const std::vector<Variable>& vars) const {
  if (vars.size() == sizeof...(Args_)) {
    InvokeVariadicImpHelp<std::is_same<void, ResultType_>::value> help_struct;
    return help_struct(*this, object, vars,
                       MakeIndexSequence<sizeof...(Args_)>());
  }
  return Variable{};
}

class Method {
  friend class Meta;

public:
  Method();
  Method(const Method& other);
  Method(Method&& other) noexcept;
  Method& operator=(const Method& other);
  Method& operator=(Method&& other) noexcept;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The hash value depends on the return value type of the function,
   * the type of all parameters, the location of the parameters, the type of the
   * class to which it belongs, and whether it is a static function.
   */
  std::size_t HashCode() const;

  /**
   * \brief Judge whether the object is a valid object.
   * \return Returns true if the object is a valid object, otherwise returns
   * false.
   */
  bool IsValid() const;
 
  /**
   * \brief Determine whether the function is static.
   * \return If the function is static, it returns true, otherwise it returns
   * false.
   * \remark Returns false if the object is invalid.
   */
  bool IsStatic() const;

  /**
   * \brief Returns the number of arguments to the function.
   * \return The number of arguments to the function.
   * \remark Return 0 if the object is invalid.
   */
  std::size_t GetArgumentNumber() const;

  /**
   * \brief Get the \ref MM::Reflection::Type of class type.
   * \return The \ref MM::Reflection::Type of class type.
   */
  Type GetClassType() const;

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \return The \ref MM::Reflection::Type of result type.
   */
  Type GetReturnType() const;

  /**
   * \brief Get instance meta data.
   * \return Returns std::weak_ptr containing metadata.
   * \remark If the type is not registered or this object is invalid, the empty
   * std::weak_ptr will be returned.
   */
  std::weak_ptr<Meta> GetClassMeta() const;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered or this object is invalid, the empty
   * std::weak_ptr will be returned.
   */
  std::weak_ptr<Meta> GetReturnMeta() const;

  /**
   * \brief Invoke the function with 0 arguments.
   * \param instance instance Instance that calls this function.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance) const;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1) const;

  /**
   * \brief Invoke the function with 2 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2) const;

  /**
   * \brief Invoke the function with 3 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3) const;

  /**
   * \brief Invoke the function with 4 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4) const;

  /**
   * \brief Invoke the function with 5 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4, Variable& arg5) const;

  /**
   * \brief Invoke the function with 6 arguments.
   * \param instance instance Instance that calls this function.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \param arg6 6th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4, Variable& arg5,
                  Variable& arg6) const;

  /**
   * \brief Call the function with any number of parameters.
   * \param instance instance Instance that calls this function.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will be return.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, std::vector<Variable>& args) const;

private:
  Method(const std::string& method_name, const std::shared_ptr<MethodWrapperBase>& method_wrapper);

  static Method CreateMethod(const std::string& method_name, const std::shared_ptr<MethodWrapperBase> 
                             & method_wrapper);

private:
  std::string method_name_;
  std::weak_ptr<MethodWrapperBase> method_wrapper_{};
};
}  // namespace Reflection
}  // namespace MM
