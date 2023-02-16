#pragma once

#include "runtime/core/reflection/variable.h"
#include "runtime/core/reflection/utils.h"
#include "utils/utils.h"

namespace MM {
namespace Reflection {
class ConstructorWrapperBase {
public:
  ConstructorWrapperBase() = default;
  virtual ~ConstructorWrapperBase() = default;
  ConstructorWrapperBase(const ConstructorWrapperBase& other) = default;
  ConstructorWrapperBase(ConstructorWrapperBase&& other) noexcept;
  ConstructorWrapperBase& operator=(const ConstructorWrapperBase& other) =
      default;
  ConstructorWrapperBase& operator=(ConstructorWrapperBase&& other) noexcept =
      default;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the constructed object of the constructor, the type of all parameters of the constructor and the parameter location.
   */
  virtual std::size_t HashCode() const = 0;

  /**
   * \brief Gets the \ref MM::Reflection::Type of the this constructor.
   * \return The \ref MM::Reflection::Type  of the this constructor.
   */
  virtual Type GetType() const = 0;

  /**
   * \brief Gets the \ref MM::Reflection::Meta of the this constructor.
   * \return The \ref MM::Reflection::Meta of the this constructor.
   */
  virtual std::weak_ptr<MM::Reflection::Meta> GetMeta() const = 0;

  /**
   * \brief Get the argument number.
   * \return The argument number.
   */
  virtual std::size_t GetArgumentNumber() const = 0;

  /**
   * \brief Invoke the function with 0 arguments.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke() const = 0;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& arg1) const = 0;

  /**
   * \brief Invoke the function with 2 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(Variable& arg1, Variable& arg2) const = 0;

  /**
   * \brief Invoke the function with 3 arguments.
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
  virtual Variable Invoke(Variable& arg1, Variable& arg2,
                          Variable& arg3) const = 0;

  /**
   * \brief Invoke the function with 4 arguments.
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
  virtual Variable Invoke(Variable& arg1, Variable& arg2,
                          Variable& arg3, Variable& arg4) const = 0;

  /**
   * \brief Invoke the function with 5 arguments.
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
  virtual Variable Invoke(Variable& arg1, Variable& arg2,
                          Variable& arg3, Variable& arg4,
                          Variable& arg5) const = 0;

  /**
   * \brief Invoke the function with 6 arguments.
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
  virtual Variable Invoke(Variable& arg1, Variable& arg2,
                          Variable& arg3, Variable& arg4,
                          Variable& arg5, Variable& arg6) const = 0;

  /**
   * \brief Call the function with any number of parameters.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  virtual Variable Invoke(std::vector<Variable>& args) const = 0;
};

template <typename TargetType_, typename... Args_>
class ConstructorWrapper : public ConstructorWrapperBase{
public:
  using Type = TargetType_;
  template <std::size_t index>
  struct GetArgumentType {
    using Types = std::tuple<Args_...>;
    using Type = typename std::tuple_element<index, Types>::type;
  };

public:
  ConstructorWrapper() = default;
  ConstructorWrapper(const ConstructorWrapper& other) = default;
  ConstructorWrapper(ConstructorWrapper&& other) noexcept = default;
  ConstructorWrapper& operator=(const ConstructorWrapper& other) = default;
  ConstructorWrapper& operator=(ConstructorWrapper&& other) = default;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the constructed object
   * of the constructor, the type of all parameters of the constructor and the
   * parameter location.
   */
  std::size_t HashCode() const override;

  /**
   * \brief Gets the \ref MM::Reflection::Type of the this constructor.
   * \return The \ref MM::Reflection::Type  of the this constructor.
   */
  Type GetType() const override;

  /**
   * \brief Gets the \ref MM::Reflection::Meta of the this constructor.
   * \return The \ref MM::Reflection::Meta of the this constructor.
   */
  std::weak_ptr<MM::Reflection::Meta> GetMeta() const override;

  /**
   * \brief Get the argument number.
   * \return The argument number.
   */
  std::size_t GetArgumentNumber() const override;

  /**
   * \brief Invoke the function with 0 arguments.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke() const override;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& arg1) const override;

  /**
   * \brief Invoke the function with 2 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& arg1, Variable& arg2) const override;

  /**
   * \brief Invoke the function with 3 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3) const override;

  /**
   * \brief Invoke the function with 4 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4) const override;

  /**
   * \brief Invoke the function with 5 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4,
                  Variable& arg5) const override;

  /**
   * \brief Invoke the function with 6 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4,
                  Variable& arg5, Variable& arg6) const override;

  /**
   * \brief Call the function with any number of parameters.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(std::vector<Variable>& args) const override;

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

  template<bool TestCount, std::size_t ArgsCount>
  struct InvokeImpHelp;

  template<>
  struct InvokeImpHelp<true, 0> {
    Variable operator()() {
      return Variable(std::make_unique<VariableWrapperBase>(TargetType_{}));
    }
  };

  template<>
  struct InvokeImpHelp<false, 0> {
    Variable operator()() {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, 1> {
    Variable operator()(void* arg1) {
      return Variable(std::make_unique<VariableWrapperBase>(TargetType_{
        *static_cast<std::add_pointer<GetArgumentType<0>>::type>(arg1)
      }));
    }
  };

  template <>
  struct InvokeImpHelp<false, 1> {
    Variable operator()(void* arg1) {
      return Variable{};
    }
  };

  template<> struct InvokeImpHelp<true, 2> {
    Variable operator()(void* arg1, void* arg2) {
      return Variable(std::make_unique<VariableWrapperBase>(TargetType_{
          *static_cast<std::add_pointer<GetArgumentType<0>>::type>(arg1),
          *static_cast<std::add_pointer<GetArgumentType<1>>::type>(arg2)
      }));
    }
  };

  template <>
  struct InvokeImpHelp<false, 2> {
    Variable operator()(void* arg1, void* arg2) { return Variable{}; }
  };

  template<> struct InvokeImpHelp<true, 3> {
    Variable operator()(void* arg1, void* arg2, void* arg3) {
      return Variable(std::make_unique<VariableWrapperBase>(TargetType_{
          *static_cast<std::add_pointer<GetArgumentType<0>>::type>(arg1),
          *static_cast<std::add_pointer<GetArgumentType<1>>::type>(arg2),
          *static_cast<std::add_pointer<GetArgumentType<2>>::type>(arg3)
      }));
    }
  };

  template <>
  struct InvokeImpHelp<false, 3> {
    Variable operator()(void* arg1, void* arg2, void* arg3) { return Variable{}; }
  };

  template <>
  struct InvokeImpHelp<true, 4> {
    Variable operator()(void* arg1, void* arg2, void* arg3, void* arg4) {
      return Variable(std::make_unique<VariableWrapperBase>(TargetType_{
          *static_cast<std::add_pointer<GetArgumentType<0>>::type>(arg1),
          *static_cast<std::add_pointer<GetArgumentType<1>>::type>(arg2),
          *static_cast<std::add_pointer<GetArgumentType<2>>::type>(arg3),
          *static_cast<std::add_pointer<GetArgumentType<3>>::type>(arg4)
      }));
    }
  };

  template <>
  struct InvokeImpHelp<false, 4> {
    Variable operator()(void* arg1, void* arg2, void* arg3, void* arg4) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, 5> {
    Variable operator()(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5) {
      return Variable(std::make_unique<VariableWrapperBase>(TargetType_{
          *static_cast<std::add_pointer<GetArgumentType<0>>::type>(arg1),
          *static_cast<std::add_pointer<GetArgumentType<1>>::type>(arg2),
          *static_cast<std::add_pointer<GetArgumentType<2>>::type>(arg3),
          *static_cast<std::add_pointer<GetArgumentType<3>>::type>(arg4),
          *static_cast<std::add_pointer<GetArgumentType<4>>::type>(arg5)
      }));
    }
  };

  template <>
  struct InvokeImpHelp<false, 5> {
    Variable operator()(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5) {
      return Variable{};
    }
  };

  template <>
  struct InvokeImpHelp<true, 6> {
    Variable operator()(void* arg1, void* arg2, void* arg3, void* arg4,
                        void* arg5, void* arg6) {
      return Variable(std::make_unique<VariableWrapperBase>(TargetType_{
          *static_cast<std::add_pointer<GetArgumentType<0>>::type>(arg1),
          *static_cast<std::add_pointer<GetArgumentType<1>>::type>(arg2),
          *static_cast<std::add_pointer<GetArgumentType<2>>::type>(arg3),
          *static_cast<std::add_pointer<GetArgumentType<3>>::type>(arg4),
          *static_cast<std::add_pointer<GetArgumentType<4>>::type>(arg5),
          *static_cast<std::add_pointer<GetArgumentType<5>>::type>(arg6),
      }));
    }
  };

  template <>
  struct InvokeImpHelp<false, 6> {
    Variable operator()(void* arg1, void* arg2, void* arg3, void* arg4,
                        void* arg5, void* arg6) {
      return Variable{};
    }
  };

  template <typename... Vars_>
  Variable InvokeImp(Vars_... vars) const;

  struct InvokeVariadicImpHelp {
    template <std::size_t... ArgsIdx>
    Variable operator()(const std::vector<Variable>& vars,
                        IndexSequence<ArgsIdx...>);
  };

  Variable InvokeVariadicImp(const std::vector<Variable>& vars) const;
};

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke(Variable& arg1,
  Variable& arg2, Variable& arg3, Variable& arg4) const {
  if (AllTypeSame(arg1, arg2, arg3, arg4)) {
    return InvokeImp(arg1.GetValue(), arg2.GetValue(), arg3.GetValue(),
                     arg4.GetValue());
  }
  return Variable{};
}

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke(Variable& arg1,
  Variable& arg2, Variable& arg3, Variable& arg4,
  Variable& arg5) const {
  if (AllTypeSame(arg1, arg2, arg3, arg4, arg5)) {
    return InvokeImp(arg1.GetValue(), arg2.GetValue(), arg3.GetValue(),
                     arg4.GetValue(), arg5.GetValue());
  }
  return Variable{};
}

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke(Variable& arg1,
  Variable& arg2, Variable& arg3, Variable& arg4,
  Variable& arg5, Variable& arg6) const {
  if (AllTypeSame(arg1, arg2, arg3, arg4, arg5, arg6)) {
    return InvokeImp(arg1.GetValue(), arg2.GetValue(), arg3.GetValue(),
                     arg4.GetValue(), arg5.GetValue(), arg6.GetValue());
  }
  return Variable{};
}

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke(
    std::vector<Variable>& args) const {
  if (AllTypeSameFromVector(MakeIndexSequence<sizeof...(Args_)>(), args)) {
    return InvokeVariadicImp(args);
  }
  return Variable{};
}

template <typename TargetType_, typename ... Args_>
std::size_t ConstructorWrapper<TargetType_, Args_...>::HashCode() const {
  return AllTypeHashCode<TargetType_, Args_...>();
}

template <typename TargetType_, typename ... Args_>
typename ConstructorWrapper<TargetType_, Args_...>::Type ConstructorWrapper<
TargetType_, Args_...>::GetType() const {
  return MM::Reflection::CreateType<TargetType_>();
}

template <typename TargetType_, typename ... Args_>
std::weak_ptr<MM::Reflection::Meta> ConstructorWrapper<TargetType_, Args_...>::
GetMeta() const {
  return GetType().GetMeta();
}

template <typename TargetType_, typename ... Args_>
std::size_t ConstructorWrapper<TargetType_, Args_...>::
GetArgumentNumber() const {
  return sizeof...(Args_);
}

template <typename TargetType_, typename ... Args_>
template <typename ... Vars_>
Variable ConstructorWrapper<TargetType_, Args_...>::InvokeImp(
    Vars_... vars) const {
  InvokeImpHelp<sizeof...(Args_) == sizeof...(Vars_), sizeof...(Args_)>
      help_struct{};
  return help_struct(vars...);
}

template <typename TargetType_, typename... Args_>
template <std::size_t... ArgsIdx>
Variable
ConstructorWrapper<TargetType_, Args_...>::InvokeVariadicImpHelp::operator()(
    const std::vector<Variable>& vars, IndexSequence<ArgsIdx...>) {
  return Variable{std::make_unique<TargetType_>(
      *static_cast<
          std::add_pointer<typename GetArgumentType<ArgsIdx>::Type>::type>(
          vars[ArgsIdx].GetValue()))...};
}

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::InvokeVariadicImp(
    const std::vector<Variable>& vars) const {
  if (vars.size() == sizeof...(Args_)) {
    InvokeVariadicImpHelp help_struct;
    return help_struct(vars,
                       MakeIndexSequence<sizeof...(Args_)>());
  }
  return Variable{};
}

template <typename TargetType_, typename... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke() const {
  return InvokeImp();
}

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke(
    Variable& arg1) const {
  if (AllTypeSame(arg1)) {
    return InvokeImp(arg1.GetValue());
  }
  return Variable{};
}

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke(Variable& arg1,
  Variable& arg2) const {
  if (AllTypeSame(arg1, arg2)) {
    return InvokeImp(arg1.GetValue(), arg2.GetValue());
  }
  return Variable{};
}

template <typename TargetType_, typename ... Args_>
Variable ConstructorWrapper<TargetType_, Args_...>::Invoke(Variable& arg1,
  Variable& arg2, Variable& arg3) const {
  if (AllTypeSame(arg1, arg2, arg3)) {
    return InvokeImp(arg1.GetValue(), arg2.GetValue(), arg3.GetValue());
  }
  return Variable{};
}

class Constructor {
  friend class Meta;

public:
  Constructor() = default;
 ~Constructor() = default;
 Constructor(const Constructor& other) = default;
  Constructor(Constructor&& other) noexcept = default;
 Constructor& operator=(const Constructor& other) = default;
  Constructor& operator=(Constructor&& other) = default;

public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the constructed object
   * of the constructor, the type of all parameters of the constructor and the
   * parameter location.
   */
  std::size_t HashCode() const;

  /**
   * \brief Judge whether the object is a valid object.
   * \return Returns true if the object is a valid object, otherwise returns
   * false.
   */
  bool IsValid() const;

  /**
   * \brief Gets the \ref MM::Reflection::Type of the this constructor.
   * \return The \ref MM::Reflection::Type  of the this constructor.
   */
  Type GetType() const;

  /**
   * \brief Gets the \ref MM::Reflection::Meta of the this constructor.
   * \return The \ref MM::Reflection::Meta of the this constructor.
   */
  std::weak_ptr<MM::Reflection::Meta> GetMeta() const;

  /**
   * \brief Get the argument number.
   * \return The argument number.
   */
  std::size_t GetArgumentNumber() const;

  /**
   * \brief Invoke the function with 0 arguments.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke() const;

  /**
   * \brief Invoke the function with 1 arguments.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& arg1) const;

  /**
   * \brief Invoke the function with 2 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& arg1, Variable& arg2) const;

  /**
   * \brief Invoke the function with 3 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3) const;

  /**
   * \brief Invoke the function with 4 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4) const;

  /**
   * \brief Invoke the function with 5 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4,
                  Variable& arg5) const;

  /**
   * \brief Invoke the function with 6 arguments.
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
  Variable Invoke(Variable& arg1, Variable& arg2,
                  Variable& arg3, Variable& arg4,
                  Variable& arg5, Variable& arg6) const;

  /**
   * \brief Call the function with any number of parameters.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(std::vector<Variable>& args) const;

private:
  Constructor(const std::shared_ptr<ConstructorWrapperBase>& other);

  static Constructor CreateConstructor(const std::shared_ptr<ConstructorWrapperBase>& other);

private:
  std::weak_ptr<ConstructorWrapperBase> constructor_wrapper_{};
};
}  // namespace Reflection
}  // namespace MM
