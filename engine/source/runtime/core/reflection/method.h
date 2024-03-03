#pragma once

#include <memory>
#include <type_traits>
#include "runtime/core/reflection/type.h"
#include "runtime/core/reflection/variable.h"
#include "utils/type_utils.h"
#include "utils/utils.h"

namespace MM {
namespace Reflection {
class MethodWrapperBase {
 public:
  MethodWrapperBase() = default;
  explicit MethodWrapperBase(const std::string& method_name);
  virtual ~MethodWrapperBase() = default;
  MethodWrapperBase(const MethodWrapperBase& other) = delete;
  MethodWrapperBase(MethodWrapperBase&& other) noexcept = delete;
  MethodWrapperBase& operator=(const MethodWrapperBase& other) = delete;
  MethodWrapperBase& operator=(MethodWrapperBase&& other) noexcept = delete;

 public:
  virtual bool IsValid() const;

  /**
   * \brief Get the method name. 
   * \return The method name.
   */
  const std::string& GetMethodName() const;

  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The hash value depends on the return value type of the function,
   * the type of all parameters, the location of the parameters, the type of the
   * class to which it belongs, and whether it is a static function.
   */
  virtual std::size_t HashCode() const;

  /**
   * \brief Determine whether the function is static.
   * \return If the function is static, it returns true, otherwise it returns
   * false.
   */
  virtual bool IsStatic() const {return false;}

  virtual bool IsConst() const {return false;}

  virtual bool IsVolatile() const {return false;}

  /**
   * \brief Returns the number of arguments to the function.
   * \return The number of arguments to the function.
   */
  virtual std::uint32_t GetArgumentNumber() const {return 0;}

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \return The \ref MM::Reflection::Type of result type.
   */
  virtual const Type* GetReturnType() const {return nullptr;}

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \param argument_index The index of argument.
   * \return The \ref MM::Reflection::Type of target argument type.
   */
  virtual const Type* GetArgumentType(std::uint32_t argument_index) const {return nullptr;}

  /**
   * \brief Get the \ref MM::Reflection::Type of class type.
   * \return The \ref MM::Reflection::Type of class type.
   */
  virtual const Type* GetClassType() const {return nullptr;}

  /**
   * \brief Get the \ref MM::Reflection::Meta of result type.
   * \return The \ref MM::Reflection::Meta of result type.
   */
  virtual const Meta* GetReturnMeta() const {return nullptr;}

  /**
   * \brief Get the \ref MM::Reflection::Meta of class type.
   * \param argument_index The index of argument.
   * \return The \ref MM::Reflection::Meta of argument type.
   */
  virtual const Meta* GetArgumentMeta(std::uint32_t argument_index) const {return nullptr;}

  /**
   * \brief Get the \ref MM::Reflection::Meta of class type.
   * \return The \ref MM::Reflection::Meta of class type.
   */
  virtual const Meta* GetClassMeta() const {return nullptr;}

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
  virtual Variable Invoke(Variable& instance) const {return Variable{};}

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
  virtual Variable Invoke(Variable& instance, Variable& arg1) const {return Variable{};}

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
                          Variable& arg2) const {return Variable{};}

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
                          Variable& arg3) const {return Variable{};}

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
                          Variable& arg3, Variable& arg4) const {return Variable{};}

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
                          Variable& arg5) const {return Variable{};}

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
                          Variable& arg6) const {return Variable{};}

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
                          std::vector<Variable*>& args) const {return Variable{};}

  protected:
    static const std::string empty_string;

  private:
    std::string method_name_{};
};


template <typename ReturnType_, typename InstanceType_,  bool IsStatic_, bool IsConst_, bool IsVolatile_,  typename... Args_>
class MethodWrapper
    : public MethodWrapperBase {
 public:
  using InstanceType = InstanceType_;
  using ReturnType = ReturnType_;
  using ArgumentsType = std::tuple<Args_...>;
  using FunctionType = Utils::GetFunctionPointTypeT<ReturnType_, InstanceType_, IsStatic_, IsConst_, IsVolatile_, Args_...>;

  template <std::size_t index>
  struct StaticGetArgumentType {
    using Types = std::tuple<Args_...>;
    using Type = std::tuple_element_t<index, Types>;
  };
  template<std::size_t index>
  using StaticGetArgumentTypeT = typename StaticGetArgumentType<index>::Type;

 public:
  MethodWrapper() = default;
  ~MethodWrapper() override = default;
  MethodWrapper(const std::string& method_name,  FunctionType function_ptr) : MethodWrapperBase(method_name), function_ptr_(function_ptr) {}
  MethodWrapper(const MethodWrapper& other) = delete;
  MethodWrapper(MethodWrapper&& other) noexcept = delete;
  MethodWrapper& operator=(const MethodWrapper& other) = delete;
  MethodWrapper& operator=(MethodWrapper&& other) noexcept = delete;

 public:
  bool IsValid() const override {
    return MethodWrapperBase::IsValid() && (function_ptr_ != nullptr);
  }

  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The hash value depends on the method name, return value type of the function,
   * the type of all parameters, the location of the parameters, the type of the
   * class to which it belongs, and whether it is a static function.
   */
  std::size_t HashCode() const override {
    return ((MethodWrapperBase::HashCode() ^
             GetReturnType()->GetTypeHashCode() ^
             GetClassType()->GetTypeHashCode()) ^
            ... ^ (typeid(Args_).hash_code()));
  }

  /**
   * \brief Determine whether the function is static.
   * \return If the function is static, it returns true, otherwise it returns
   * false.
   */
  bool IsStatic() const override {
    return IsStatic_;
  }

  bool IsConst() const override {
    return IsConst_;
  }

  bool IsVolatile() const override {
    return IsVolatile_;
  }

  /**
   * \brief Returns the number of arguments to the function.
   * \return The number of arguments to the function.
   */
  std::uint32_t GetArgumentNumber() const override {
    return sizeof...(Args_);
  }

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \return The \ref MM::Reflection::Type of result type.
   */
  const Type* GetReturnType() const override {
    static Type ReturnType = CreateType<ReturnType_>();
    return &ReturnType;
  }

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \param argument_index The index of argument.
   * \return The \ref MM::Reflection::Type of target argument type.
   */
  const Type* GetArgumentType(std::uint32_t argument_index) const override {
    static std::array<Type, sizeof...(Args_)> ArgTypes{CreateType<Args_>()...};
    if (argument_index >= sizeof...(Args_)) {
      return nullptr;
    }
    return &ArgTypes[argument_index];
  }

  /**
   * \brief Get the \ref MM::Reflection::Type of class type.
   * \return The \ref MM::Reflection::Type of class type.
   */
  const Type* GetClassType() const override {
    static Type ClassType = CreateType<InstanceType_>();
    return &ClassType;
  }

  /**
   * \brief Get the \ref MM::Reflection::Meta of result type.
   * \return The \ref MM::Reflection::Meta of result type.
   */
  const Meta* GetReturnMeta() const override {
    return GetReturnType()->GetMate();
  }

  /**
   * \brief Get the \ref MM::Reflection::Meta of class type.
   * \param argument_index The index of argument.
   * \return The \ref MM::Reflection::Meta of argument type.
   */
  const Meta* GetArgumentMeta(std::uint32_t argument_index) const override {
    const Type* argument_type = GetArgumentType(argument_index);
    if (argument_type == nullptr) {
      return nullptr; 
    }

    return argument_type->GetMate();
  }

  /**
   * \brief Get the \ref MM::Reflection::Meta of class type.
   * \return The \ref MM::Reflection::Meta of class type.
   */
  const Meta* GetClassMeta() const override {
    return GetClassType()->GetMate();
  }

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
  Variable Invoke(Variable& instance) const override {
    return InvokeImp(instance);
  }

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
  Variable Invoke(Variable& instance, Variable& arg1) const override {
    return InvokeImp(instance, arg1);
  }

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
                  Variable& arg2) const override {
    return InvokeImp(instance, arg1, arg2);                  
  }

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
                  Variable& arg3) const override {
    return InvokeImp(instance, arg1, arg2, arg3);
  }

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
                  Variable& arg3, Variable& arg4) const override {
    return InvokeImp(instance, arg1, arg2, arg3, arg4);
  }

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
                  Variable& arg3, Variable& arg4,
                  Variable& arg5) const override {
    return InvokeImp(instance, arg1, arg2, arg3, arg4, arg5);
  }

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
                  Variable& arg6) const override {
    return InvokeImp(instance, arg1, arg2, arg3, arg4, arg5, arg6);
  }

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
                  std::vector<Variable*>& args) const override {
    return InvokeVectorImp(instance, args, Utils::MakeIndexSequence<sizeof...(Args_)>());
  }

 private:
  template<typename ...ArgVariableTypes>
  static constexpr bool AllTypeIsVariable(ArgVariableTypes&&...  /*args*/) {
    return ((std::is_same_v<std::remove_reference_t<ArgVariableTypes>, Variable>) && ...);
  }

  template<typename ...ArgVariableTypes>
  bool AllTypeIsArgType(ArgVariableTypes&&... args) const {
    // static_assert(AllTypeIsVariable(std::forward<ArgVariableTypes>(args)...), "All parameters must be of type MM::Reflection::Variable.");
    static_assert(((std::is_same_v<std::remove_reference_t<ArgVariableTypes>, Variable>) && ...), "All parameters must be of type MM::Reflection::Variable.");
    std::uint32_t i = 0;
    bool all_valid = ((args.IsValid()) && ...);
    if (!all_valid) {
      return all_valid;
    }
    i = 0;
    return (((args.GetType()->GetCommonTypeHashCode() == GetArgumentType(i)->GetCommonTypeHashCode()) && (++i)) && ...);
  }
  
  bool AllTypeIsArgTypeFromVector(const std::vector<Variable*>& args) const {
    if (args.size() != GetArgumentNumber()) {
      return false;
    }
    for (const auto& arg: args) {
      if (arg == nullptr || !(arg->IsValid())) {
        return false;
      }
    }
    for (std::uint32_t arg_index = 0; arg_index != GetArgumentNumber(); ++arg_index) {
      if (args[arg_index]->GetType()->GetCommonTypeHashCode() != GetArgumentType(arg_index)->GetCommonTypeHashCode()) {
        return false;
      }
    }

    return true;
  }

  template<typename ...ArgVariableTypes>
  Variable InvokeImp(Variable& instance, ArgVariableTypes&& ...args) const {
    if (!IsValid()) {
      return Variable{};
    }
    if constexpr (sizeof...(ArgVariableTypes) == sizeof...(Args_)) {
      if constexpr (!IsStatic_) {
        if (!instance.IsValid()) {
          return Variable{};
        }
        const Type* instance_type = instance.GetType();
        const Type* class_type = GetClassType();
        if ((instance_type->GetCommonTypeHashCode() != class_type->GetCommonTypeHashCode()) || (instance_type->IsConst() != class_type->IsConst())) {
          return Variable{};
        }
      }
      if (!AllTypeIsArgType(std::forward<ArgVariableTypes>(args)...)) {
        return Variable{};
      }
      if constexpr (IsStatic_) {
        if constexpr (std::is_same_v<ReturnType_, void>) {
          (*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args.GetValue()))...);
          return Variable::CreateVoidVariable();
        } else {
          return Variable::CreateVariable<ReturnType_>((*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args.GetValue()))...));
        }
      } else {
        using InstancePtrType = Utils::IfThenElseT<IsConst_, const InstanceType*, InstanceType*>;
        InstancePtrType instance_ptr = reinterpret_cast<InstancePtrType>(instance.GetValue());
        if constexpr (std::is_same_v<ReturnType_, void>) {
          ((*instance_ptr).*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args.GetValue()))...);
          return Variable::CreateVoidVariable();
        } else {
          return Variable::CreateVariable<ReturnType_>(((*instance_ptr).*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args.GetValue()))...));
        }
      }
    }

    return Variable{};
  }

  template<std::size_t ...ArgIndex>
  Variable InvokeVectorImp(Variable& instance, std::vector<Variable*>& args, Utils::IndexSequence<ArgIndex...>) const {
    if (!IsValid()) {
      return Variable{};
    }
    if constexpr (!IsStatic_) {
      if (!instance.IsValid()) {
        return Variable{};
      }
      const Type* instance_type = instance.GetType();
      const Type* class_type = GetClassType();
      if ((instance_type->GetCommonTypeHashCode() != class_type->GetCommonTypeHashCode()) || (instance_type->IsConst() != class_type->IsConst())) {
        return Variable{};
      }
    }
    if (!AllTypeIsArgTypeFromVector(args)) {
      return Variable{};
    }
    if constexpr (IsStatic_) {
      if constexpr (std::is_same_v<ReturnType_, void>) {
        (*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args[ArgIndex]->GetValue()))...);
        return Variable::CreateVoidVariable();
      } else {
        return Variable::CreateVariable<ReturnType_>((*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args[ArgIndex]->GetValue()))...));
      }
    } else {
      using InstancePtrType = Utils::IfThenElseT<IsConst_, const InstanceType*, InstanceType*>;
      InstancePtrType instance_ptr = reinterpret_cast<InstancePtrType>(instance.GetValue());
      if constexpr (std::is_same_v<ReturnType_, void>) {
        ((*instance_ptr).*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args[ArgIndex]->GetValue()))...);
        return Variable::CreateVoidVariable();
      } else {
        return Variable::CreateVariable<ReturnType_>(((*instance_ptr).*function_ptr_)((*reinterpret_cast<std::add_pointer_t<Utils::GetCommonTypeT<Args_>>>(args[ArgIndex]->GetValue()))...));
      }
    }
  }

 private:
  FunctionType function_ptr_ = nullptr;
};

class Method {
  friend class Meta;

 public:
  Method() = default;
  ~Method() = default;
  Method(const Method& other) = delete;
  Method(Method&& other) noexcept = default;
  Method& operator=(const Method& other) = delete;
  Method& operator=(Method&& other) noexcept = default;

 public:
  template<typename ReturnType, typename InstanceType, bool IsStatic, bool IsConst, bool IsVolatile, typename ...Args>
  static Method CreateMethod(
      const std::string& method_name,
      Utils::GetFunctionPointTypeT<ReturnType, InstanceType, IsStatic, IsConst, IsVolatile, Args...> function_ptr) {
    return Method{std::make_unique<MethodWrapper<ReturnType, InstanceType, IsStatic, IsConst, IsVolatile, Args...>>(method_name, function_ptr)};
  }

 public:
  /**
   * \brief Get the method name. 
   * \return The method name.
   */
  const std::string& GetMethodName() const;

  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The hash value depends on the method name, return value type of the function,
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
  const Type* GetClassType() const;

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \param argument_index The index of argument.
   * \return The \ref MM::Reflection::Type of target argument type.
   */
  const Type* GetArgumentType(std::uint32_t argument_index) const;

  /**
   * \brief Get the \ref MM::Reflection::Type of result type.
   * \return The \ref MM::Reflection::Type of result type.
   */
  const Type* GetReturnType() const;

  /**
   * \brief Get instance meta data.
   * \return Returns std::weak_ptr containing metadata.
   * \remark If the type is not registered or this object is invalid, the nullptr will be returned.
   */
  const Meta* GetClassMeta() const;

  const Meta* GetArgumentMeta(std::uint32_t argument_index) const;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered or this object is invalid, the nullptr will be returned. 
   */
  const Meta* GetReturnMeta() const;

  /**
   * \brief Invoke the function with 0 arguments.
   * \param instance instance Instance that calls this function.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
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
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
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
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
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
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
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
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
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
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
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
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
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
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, std::vector<Variable*>& args) const;

  /**
   * \brief Call the function with any number of parameters.
   * \param instance instance Instance that calls this function.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If this object is invalid ,empty \ref MM::Reflection::Variable will
   * be return. \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(Variable& instance, std::vector<Variable*>&& args) const;

 private:
  explicit Method(std::unique_ptr<MethodWrapperBase>&& method_wrapper) : method_wrapper_(std::move(method_wrapper)) {}

  private:
    const static std::string empty_string;

  private:
    std::unique_ptr<MethodWrapperBase> method_wrapper_{};
};
}  // namespace Reflection
}  // namespace MM
