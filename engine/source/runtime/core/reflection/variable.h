#pragma once

#include "variable.h"
#include "variable.h"
#include "runtime/core/reflection/type.h"
#include "runtime/core/reflection/utils.h"
#include "runtime/core/reflection/destructor.h"

namespace MM {
namespace Reflection {
class VariableWrapperBase;
class Property;

class Variable {
  friend class VariableWrapperBase;
  template <typename VariableType, bool IsPropertyVariable_>
  friend class VariableWrapper;
  template <typename PropertyType, typename ClassType, bool IsStatic>
  friend class PropertyWrapper;
  template <typename TargetType_, typename DestructorType_>
  friend class DestructorWrapper;

 public:
  /**
   * \brief Get the value and perform type conversion.
   * \tparam VariableType The type of the value.
   * \return The value you want.
   */
  template <typename VariableType>
  VariableType GetValueFunction() const;

  /**
   * \brief Set new value.
   * \tparam TargetType The type of new value.
   * \tparam VariableType The type of value to be changed.
   * \tparam TestConvertible Determine whether "A" can be converted to "B".
   * \param other New value.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   */
  template <typename TargetType, typename VariableType,
            typename TestConvertible = std::enable_if<
                Conversion<TargetType, VariableType>::value>::type>
  bool SetValueFunction(TargetType other);

 public:
  /**
   * \brief Determine whether this variable is an property of another \ref
   * MM::Reflection::Variable. \return  If this variable is an property of
   * another \ref MM::Reflection::Variable, return true, otherwise return false.
   * \remark Property variable has no ownership of the value held.
   */
  bool IsPropertyVariable() const;

  /**
   * \brief Default initialization. The initialized object is invalid.
   */
  Variable();

  /**
   * \brief Use the registered destructor to destruct. If the destructor is not registered actively, the destructor of each class itself will be used.
   */
  ~Variable(); 

  /**
   * \brief Generate a \ref MM::Reflection::Variable from an rvalue.
   * \tparam VariableType VariableType The type of rvalue.
   * \param other rvalue.
   * \remark The new \ref MM::Reflection::Variable holds a value to
   * \ref other.
   * \remark \ref Variable must have move constructor.If \ref VariableType not have move constructor, use "A" to construct the \ref MM::Reflection::Variable.
   */
  template <typename VariableType>
  Variable(VariableType&& other) noexcept;

  /**
   * \brief Copy constructor.
   * \param other Other objects will be copied to this object.
   * \remark If the value held by this object has no copy constructor, this
   * function will do nothing. \remark If \ref other is an invalid object, this
   * object will also change to an invalid object.
   */
  Variable(const Variable& other);

  /**
   * \brief Copy constructor.
   * \param other Other objects will be copied to this object.
   * \remark If the value held by this object has no copy constructor, this
   * function will do nothing. \remark If \ref other is an invalid object, this
   * object will also change to an invalid object.
   */
  Variable(Variable& other);

  /**
   * \brief Move constructor.
   * \param other Other objects will be moved to this object.
   * \remark If the value held by this object has no move constructor, this
   * function will do nothing.
   * \remark If \ref other is an invalid object, this
   * object will also change to an invalid object.
   */
  Variable(Variable&& other) noexcept;

  /**
   * \brief Copy Assign.
   * \param other Other objects will be copied to this object.
   * \remark If the value held by this object has no copy assign, this
   * function will do nothing.
   * \remark If \ref other is an invalid object, this
   * object will also change to an invalid object.
   */
  Variable& operator=(const Variable& other);

  /**
   * \brief Move Assign.
   * \param other Other objects will be moved to this object.
   * \remark If the value held by this object has no move assign, this
   * function will do nothing.
   * \remark If \ref other is an invalid object, this object will also
   * change to an invalid object.
   */
  Variable& operator=(Variable&& other) noexcept;

  /**
   * \brief Construct an object from an std::unique_ptr<VariableWrapperBase>.
   * \param variable_wrapper std::unique_ptr<VariableWrapperBase> containing variable data.
   */
  Variable(std::unique_ptr<VariableWrapperBase>&& variable_wrapper);

 public:
  /**
   * \brief bool conversion.Call \ref IsValid.
   */
  operator bool() const;

  /**
   * \brief void* conversion.Call \ref GetValue.
   */
  operator void*() const;

  /**
   * \brief Judge whether the object is a valid object.
   * \return Returns true if the object is a valid object, otherwise returns
   * false.
   */
  bool IsValid() const;

  /**
   * \brief Get the value of the object held by this object.
   * \return The value pointer of the object held by this object.
   * \remark It is not recommended for users and may cause runtime errors.
   * \remark If object is not valid, it will return nullptr.
   */
  void* GetValue() const;

  /**
   * \brief Set the value of the object held by this object.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   * \remark It is not recommended for users and may cause runtime errors.
   * \remark If object is not valid, it will do nothing and return false.
   */
  bool SetValue(void* other);

  /**
   * \brief Get the MM::Reflection::Type of the object held by this object.
   * \return The "MM::Reflection::Type" of the object held by this object.
   * \remark If object is not valid, it will return the default empty
   * MM::Reflection::Type{}.
   */
  Type GetType() const;

  /**
   * \brief Get meta data.
   * \return Returns std::weak_ptr containing metadata.
   * \remark If the type is not registered or this object is invalid, the
   * empty std::weak_ptr will be returned.
   */
  std::weak_ptr<Meta> GetMeta() const;

  /**
   * \brief Gets the properties of the object held by this object.
   * \param property_name The name of property.
   * \return A MM::Reflection::Variable that holds the specific property.
   */
  Variable GetPropertyVariable(const std::string& property_name) const;

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name);

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name, Variable& arg1);

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name, Variable& arg1, Variable& arg2);

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name, Variable& arg1, Variable& arg2, Variable& arg3);

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name, Variable& arg1, Variable& arg2, Variable& arg3, Variable& arg4);

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name, Variable& arg1, Variable& arg2, Variable& arg3, Variable& arg4, Variable& arg5);

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \param arg6 6th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name, Variable& arg1, Variable& arg2, Variable& arg3, Variable& arg4, Variable& arg5, Variable& arg6);

  /**
   * \brief Invoke the function with 0 arguments.
   * \param method_name The name of a method that you want call.
   * \param args Arguments.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the function held by this object or the target method not exist, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable Invoke(const std::string& method_name, std::vector<Variable>& args);

  /**
   * \brief Returns a pointer to the managed object and releases the ownership.
   * \return The pointer to the managed object.
   */
  void* Release();

  /**
   * \brief Destroy this object.
   * \return If the destruction fails (the type of the object held by the variable is not registered and the variable is not property variable), false is returned; otherwise, true is returned.
   */
  bool Destroy();

  template<typename VariableType, typename ...Args>
  struct GetVariable {
    Variable operator()(Args... args) {
      return Variable{std::make_shared<VariableWrapperBase>(
          VariableWrapper<VariableType, false>(std::forward<Args>(args)...))};
    }
  };

 private:
  std::unique_ptr<VariableWrapperBase> variable_wrapper_ = nullptr;
};

class VariableWrapperBase {
  friend class Variable;

 public:
  VariableWrapperBase() = default;
  virtual ~VariableWrapperBase() = default;
  VariableWrapperBase(const VariableWrapperBase&) = default;
  VariableWrapperBase(VariableWrapperBase&&) = default;
  VariableWrapperBase& operator=(const VariableWrapperBase&) = default;
  VariableWrapperBase& operator=(VariableWrapperBase&&) = default;

 public:
  /**
   * \brief Get a base pointer of copy.
   * \return The base pointer of copy.
   */
  virtual std::unique_ptr<VariableWrapperBase> CopyToBasePointer() = 0;

  /**
   * \brief Get the value of the object held by this object.
   * \return The value pointer of the object held by this object.
   */
  virtual void* GetValue() const = 0;

  /**
   * \brief Set the value of the object held by this object.
   * \param other A void * pointer to the new value.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   */
  virtual bool SetValue(void* other) = 0;

  /**
   * \brief Determine whether this variable is an property of another \ref MM::Reflection::Variable.
   * \return  If this variable is an property of another \ref MM::Reflection::Variable, return true, otherwise return false.
   * \remark Property variable has no ownership of the value held.
   */
  virtual bool IsPropertyVariable() const = 0;

  /**
   * \brief Gets the properties of the object held by this object.
   * \param property_name The name of property.
   * \return A MM::Reflection::Variable that holds the specific property.
   */
  virtual Variable GetPropertyVariable(const std::string& property_name) const = 0;

  /**
   * \brief Get the MM::Reflection::Type of the object held by this object.
   * \return The MM::Reflection::TypeWrapper of the object held by this object.
   */
  virtual MM::Reflection::Type GetType() const = 0;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  virtual std::weak_ptr<Meta> GetMeta() const = 0;

  /**
   * \brief Returns a pointer to the managed object and releases the ownership.
   * \return The pointer to the managed object.
   */
  virtual void* Release() = 0;
};

template <typename VariableType_, bool IsPropertyVariable_ = false>
class VariableWrapper : public VariableWrapperBase {};

template<typename VariableType_>
class VariableWrapper<VariableType_, false> final : public VariableWrapperBase {
  template <typename TargetType_, typename DestructorType_>
  friend class DestructorWrapper;

  template <typename PropertyType_, typename ClassType_, bool IsStatic>
  friend class PropertyWrapper;

  friend class Variable;

 public:
  using Type = VariableType_;
  using OriginalType = typename TypeWrapper<VariableType_>::OriginalType;

 public:
  VariableWrapper();

  ~VariableWrapper() override;

  /**
   * \brief Copy constructor.
   * \tparam TestCopyCT Determine whether copy construction can be performed.
   * \param other Objects to be copied.
   */
  template <typename TestCopyCT = std::enable_if<
                std::is_copy_constructible<VariableType_>::value, void>::type>
  VariableWrapper(const VariableWrapper& other);
  /**
   * \brief Move constructor.
   * \tparam TestMoveCT Determine whether move construction can be performed.
   * \param other Objects to be moved.
   */
  template <typename TestMoveCT = std::enable_if<
                std::is_move_constructible<VariableType_>::value, void>::type>
  VariableWrapper(VariableWrapper&& other) noexcept;

  /**
   * \brief Copy assign.
   * \tparam TestCopy Determine whether copy assign can be performed.
   * \param other Objects to be copied.
   * \return New objects after copying.
   */
  template <typename TestCopy = std::enable_if<
                std::is_copy_assignable<VariableType_>::value, void>::type>
  VariableWrapper& operator=(const VariableWrapper& other);

  /**
   * \brief Move assign.
   * \tparam TestMove Determine whether move assign can be performed.
   * \param other Objects to be moved.
   * \return New objects after moving.
   */
  template <typename TestMove = std::enable_if<
                std::is_move_assignable<VariableType_>::value>::type>
  VariableWrapper& operator=(VariableWrapper&& other) noexcept;

  VariableWrapper(std::unique_ptr<VariableType_>& variable_ptr);

  template<typename TestCopy = std::enable_if<std::is_copy_constructible<VariableType_>::value, void>::type>
  VariableWrapper(const VariableType_& other);

  template<typename TestMove = std::enable_if<std::is_move_constructible<VariableType_>::value, void>::type>
  VariableWrapper(VariableType_&& other);

  template<typename ...Args>
  explicit VariableWrapper(Args... args);

 public:
  /**
   * \brief Determine whether this variable is an property of another \ref
   * MM::Reflection::Variable. \return  If this variable is an property of
   * another \ref MM::Reflection::Variable, return true, otherwise return false.
   * \remark Property variable has no ownership of the value held.
   */
   bool IsPropertyVariable() const override;

  std::unique_ptr<VariableWrapperBase> CopyToBasePointer() override;

  /**
   * \brief Get the MM::Reflection::Type of the object held by this object.
   * \return The "MM::Reflection::Type" of the object held by this object.
   */
  MM::Reflection::Type GetType() const override;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  std::weak_ptr<Meta> GetMeta() const override;

  /**
   * \brief Get the value of the object held by this object.
   * \return The value pointer of the object held by this object.
   */
  void* GetValue() const override;

  /**
   * \brief Set the value of the object held by this object.
   * \param other A void * pointer to the new value.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   */
  bool SetValue(void* other) override;

  /**
   * \brief Returns a pointer to the managed object and releases the ownership.
   * \return The pointer to the managed object.
   */
  void* Release() override;

 private:
  template <bool TestCopy>
  struct CopyHelp {
    std::unique_ptr<VariableWrapperBase> operator()() { return nullptr;}
  };

  template <>
  struct CopyHelp<true> {
    std::unique_ptr<VariableWrapperBase> operator()() {
      return std::make_unique<VariableWrapperBase>(
          VariableWrapper<VariableType_, false>(*this));
    };
  };

  template <>
  struct CopyHelp<false> {
    std::unique_ptr<VariableWrapperBase> operator()() { return nullptr;
    };
  };

  template <bool TestCopy>
  struct SetValueHelp {
    static bool SetValue(void* object);
  };

  template <>
  struct SetValueHelp<true> {
    static bool SetValue(VariableWrapper& variable_wrapper, void* object) {
      *variable_wrapper.value_ = *static_cast<VariableType_*>(object);
      return true;
    }
  };

  template <>
  struct SetValueHelp<false> {
    static bool SetValue(VariableWrapper& variable_wrapper, void* object) {
      *variable_wrapper.value_ = *static_cast<VariableType_*>(object);
      return false;
    }
  };

 private:
  std::unique_ptr<VariableType_> value_ = nullptr;
};

template <typename VariableType_>
class VariableWrapper<VariableType_, true> final : public VariableWrapperBase {
  template <typename TargetType_, typename DestructorType_>
  friend class DestructorWrapper;

  template <typename PropertyType_, typename ClassType_, bool IsStatic>
  friend class PropertyWrapper;

  friend class Variable;

 public:
  using Type = VariableType_;
  using OriginalType = typename TypeWrapper<VariableType_>::OriginalType;

 public:
  VariableWrapper();

  ~VariableWrapper() override; 

  /**
   * \brief Copy constructor.
   * \tparam TestCopyCT Determine whether copy construction can be performed.
   * \param other Objects to be copied.
   */
  template <typename TestCopyCT = std::enable_if<
                std::is_copy_constructible<VariableType_>::value, void>::type>
  VariableWrapper(const VariableWrapper& other);
  /**
   * \brief Move constructor.
   * \tparam TestMoveCT Determine whether move construction can be performed.
   * \param other Objects to be moved.
   */
  template <typename TestMoveCT = std::enable_if<
                std::is_move_constructible<VariableType_>::value, void>::type>
  VariableWrapper(VariableWrapper&& other) noexcept;

  /**
   * \brief Copy assign.
   * \tparam TestCopy Determine whether copy assign can be performed.
   * \param other Objects to be copied.
   * \return New objects after copying.
   */
  template <typename TestCopy = std::enable_if<
                std::is_copy_assignable<VariableType_>::value, void>::type>
  VariableWrapper& operator=(const VariableWrapper& other);

  /**
   * \brief Move assign.
   * \tparam TestMove Determine whether move assign can be performed.
   * \param other Objects to be moved.
   * \return New objects after moving.
   */
  template <typename TestMove = std::enable_if<
                std::is_move_assignable<VariableType_>::value>::type>
  VariableWrapper& operator=(VariableWrapper&& other) noexcept;

  VariableWrapper(std::unique_ptr<VariableType_>& variable_ptr);

  template <typename TestCopy = std::enable_if<
                std::is_copy_constructible<VariableType_>::value, void>::type>
  VariableWrapper(const VariableType_& other);

  template <typename TestMove = std::enable_if<
                std::is_move_constructible<VariableType_>::value, void>::type>
  VariableWrapper(VariableType_&& other);

  template <typename... Args>
  explicit VariableWrapper(Args... args);

 public:
  /**
   * \brief Determine whether this variable is an property of another \ref
   * MM::Reflection::Variable. \return  If this variable is an property of
   * another \ref MM::Reflection::Variable, return true, otherwise return false.
   * \remark Property variable has no ownership of the value held.
   */
  bool IsPropertyVariable() const override;

  std::unique_ptr<VariableWrapperBase> CopyToBasePointer() override;

  /**
   * \brief Get the MM::Reflection::Type of the object held by this object.
   * \return The "MM::Reflection::Type" of the object held by this object.
   */
  MM::Reflection::Type GetType() const override;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  std::weak_ptr<Meta> GetMeta() const override;

  /**
   * \brief Get the value of the object held by this object.
   * \return The value pointer of the object held by this object.
   */
  void* GetValue() const override;

  /**
   * \brief Set the value of the object held by this object.
   * \param other A void * pointer to the new value.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   */
  bool SetValue(void* other) override;

    /**
   * \brief Returns a pointer to the managed object and releases the ownership.
   * \return The pointer to the managed object.
   */
  void* Release() override;

 private:
  template <bool TestCopy>
  struct CopyHelp {
    std::unique_ptr<VariableWrapperBase> operator()() { return nullptr; }
  };

  template <>
  struct CopyHelp<true> {
    std::unique_ptr<VariableWrapperBase> operator()() {
      return std::make_unique<VariableWrapperBase>(
          VariableWrapper<VariableType_, false>(*this));
    };
  };

  template <>
  struct CopyHelp<false> {
    std::unique_ptr<VariableWrapperBase> operator()() { return nullptr;
    };
  };

  template<bool TestCopy>
  struct SetValueHelp {
    static bool SetValue(void* object);
  };

  template<>
  struct SetValueHelp<true> {
    static bool SetValue(VariableWrapper& variable_wrapper, void* object) {
      *variable_wrapper.value_ = *static_cast<VariableType_*>(object);
      return true;
    }
  };

  template <>
  struct SetValueHelp<false> {
    static bool SetValue(VariableWrapper& variable_wrapper, void* object) {
      *variable_wrapper.value_ = *static_cast<VariableType_*>(object);
      return false;
    }
  };

 private:
  std::unique_ptr<VariableType_> value_ = nullptr;
};

template <typename VariableType_>
::MM::Reflection::VariableWrapper<VariableType_, false>::VariableWrapper()
    : value_(nullptr) {}

template <typename VariableType_>
::MM::Reflection::VariableWrapper<VariableType_, false>::VariableWrapper::~VariableWrapper() =
    default;

template <typename VariableType_>
template <typename TestCopyCT>
VariableWrapper<VariableType_, false>::VariableWrapper(
    const VariableWrapper& other) :  value_(*other) {}

template <typename VariableType_>
template <typename TestMoveCT>
VariableWrapper<VariableType_, false>::VariableWrapper(
    VariableWrapper&& other) noexcept {
  std::swap(value_, other.value_);
  other.value_.reset();
}

template <typename VariableType_>
template <typename TestCopy>
VariableWrapper<VariableType_, false>& VariableWrapper<VariableType_, false>::
operator=(const VariableWrapper& other) {
  if (this == &other) {
    return *this;
  }
  *value_ = *(other.value_);
  return *this;
}

template <typename VariableType_>
template <typename TestMove>
VariableWrapper<VariableType_, false>& VariableWrapper<VariableType_, false>::
operator=(VariableWrapper&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  std::swap(value_, other.value_);
  other.value_.reset();
  return *this;
}

template <typename VariableType_>
VariableWrapper<VariableType_, false>::VariableWrapper(
    std::unique_ptr<VariableType_>& variable_ptr) : value_(variable_ptr.release()){}

template <typename VariableType_>
template <typename TestCopy>
VariableWrapper<VariableType_, false>::VariableWrapper(
    const VariableType_& other)
  : value_(
      std::make_unique<VariableType_>(std::forward<VariableType_>(other))) {
}

template <typename VariableType_>
template <typename TestMove>
VariableWrapper<VariableType_, false>::VariableWrapper(VariableType_&& other)
  : value_(
      std::make_unique<VariableType_>(std::forward<VariableType_>(other))) {
}

template <typename VariableType_>
template <typename ... Args>
VariableWrapper<VariableType_, false>::VariableWrapper(Args ... args)
  : value_(std::make_unique<VariableType_>(std::forward<Args>(args)...)) {
}

template <typename VariableType_>
bool VariableWrapper<VariableType_, false>::IsPropertyVariable() const {
  return false;
}

template <typename VariableType_>
std::unique_ptr<VariableWrapperBase> VariableWrapper<VariableType_, false>::
CopyToBasePointer() {
  CopyHelp<std::is_copy_constructible<VariableType_>::value> help_struct{};
  return help_struct();
}

template <typename VariableType_>
MM::Reflection::Type VariableWrapper<VariableType_, false>::GetType() const {
  return CreateType<VariableType_>();
}

template <typename VariableType_>
std::weak_ptr<Meta> VariableWrapper<VariableType_, false>::GetMeta() const {
  return GetType().GetMate();
}

template <typename VariableType_>
void* VariableWrapper<VariableType_, false>::GetValue() const {
  return value_.get();
}

template <typename VariableType_>
bool VariableWrapper<VariableType_, false>::SetValue(void* other) {
  return SetValueHelp<std::is_copy_assignable<VariableType_>::value>::SetValue(
      *this, other);
}

template <typename VariableType_>
void* VariableWrapper<VariableType_, false>::Release() {
  return value_.release();
}

template <typename VariableType_>
VariableWrapper<VariableType_, true>::VariableWrapper() : value_(nullptr) {}

template <typename VariableType_>
VariableWrapper<VariableType_, true>::~VariableWrapper() = default;

template <typename VariableType_>
template <typename TestCopyCT>
VariableWrapper<VariableType_, true>::VariableWrapper(
    const VariableWrapper& other) : value_(std::make_unique<VariableType_>(*(other.value_))) {}

template <typename VariableType_>
template <typename TestMoveCT>
VariableWrapper<VariableType_, true>::VariableWrapper(
    VariableWrapper&& other) noexcept {
  std::swap(value_, other.value_);
  other.value_.reset();
}

template <typename VariableType_>
template <typename TestCopy>
VariableWrapper<VariableType_, true>& VariableWrapper<VariableType_, true>::
operator=(const VariableWrapper& other) {
  if (this == &other) {
    return *this;
  }
  value_ = *other.value_;
  return *this;
}

template <typename VariableType_>
template <typename TestMove>
VariableWrapper<VariableType_, true>& VariableWrapper<VariableType_, true>::
operator=(VariableWrapper&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  std::swap(value_, other.value_);
  other.value_.reset();
  return *this;
}

template <typename VariableType_>
VariableWrapper<VariableType_, true>::VariableWrapper(
    std::unique_ptr<VariableType_>& variable_ptr) : value_(variable_ptr.release()){}

template <typename VariableType_>
template <typename TestCopy>
VariableWrapper<VariableType_, true>::VariableWrapper(
    const VariableType_& other) : value_(std::make_unique<VariableType_>(std::forward<VariableType_>(other))){}

template <typename VariableType_>
template <typename TestMove>
VariableWrapper<VariableType_, true>::VariableWrapper(VariableType_&& other)
  : value_(
      std::make_unique<VariableType_>(std::forward<VariableType_>(other))) {
}

template <typename VariableType_>
template <typename ... Args>
VariableWrapper<VariableType_, true>::VariableWrapper(Args... args) : value_(std::make_unique<VariableType_>(std::forward<Args>(args)...)){}

template <typename VariableType_>
bool VariableWrapper<VariableType_, true>::IsPropertyVariable() const {
  return true;
}

template <typename VariableType_>
std::unique_ptr<VariableWrapperBase> VariableWrapper<VariableType_, true>::
CopyToBasePointer() {
  CopyHelp<std::is_copy_constructible<VariableType_>::value> help_struct{};
  return help_struct();
}

template <typename VariableType_>
MM::Reflection::Type VariableWrapper<VariableType_, true>::GetType() const {
  return CreateType<VariableType_>();
}

template <typename VariableType_>
std::weak_ptr<Meta> VariableWrapper<VariableType_, true>::GetMeta() const {
  return GetType().GetMate();
}

template <typename VariableType_>
void* VariableWrapper<VariableType_, true>::GetValue() const {
  return value_.get();
}

template <typename VariableType_>
bool VariableWrapper<VariableType_, true>::SetValue(void* other) {
  return SetValueHelp<std::is_copy_assignable<VariableType_>::value>::SetValue(
      *this, other);
}

template <typename VariableType_>
void* VariableWrapper<VariableType_, true>::Release() {
  return value_.release();
}


template <typename VariableType_>
VariableType_ Variable::GetValueFunction() const {
  return static_cast<VariableType_>(*(GetValue()));
}

template <typename TargetType, typename VariableType_, typename TestConvertible>
bool Variable::SetValueFunction(TargetType other) {
  auto temp = static_cast<VariableType_>(other);
  return SetValue(&temp);
}

//template <typename VariableType_, bool IsPropertyVariable>
//Variable::Variable(VariableType_& other)
//  : variable_wrapper_(
//      std::make_shared<VariableWrapperBase>(
//          VariableWrapper<VariableType_&>(std::forward<VariableType_>(other)))) {
//}

template <typename VariableType_>
Variable::Variable(VariableType_&& other) noexcept
  : variable_wrapper_(
      std::make_shared<VariableWrapperBase>(
          VariableWrapper<std::remove_reference<VariableType_>::type>(std::forward<VariableType_>(other)))) {}

}  // namespace Reflection
}  // namespace MM
