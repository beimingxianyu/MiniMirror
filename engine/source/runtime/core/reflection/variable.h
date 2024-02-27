#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include "runtime/core/reflection/database.h"
#include "runtime/core/reflection/type.h"
#include "runtime/core/reflection/utils.h"
#include "runtime/core/reflection/destructor.h"

namespace MM {
namespace Reflection {
class VariableWrapperBase;
class Property;

class Variable {
  friend class VariableWrapperBase;
  template <typename VariableType>
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
  const VariableType& GetValueCast() const;

  /**
   * \brief Get the value and perform type conversion.
   * \tparam VariableType The type of the value.
   * \return The value you want.
   */
  template <typename VariableType>
  VariableType& GetValueCast();

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
            typename TestConvertible = typename std::enable_if<
                Conversion<TargetType, VariableType>::value>::type>
  bool SetValueSafe(TargetType&& other);

 public:
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
  explicit Variable(VariableType&& other) noexcept;

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
  explicit Variable(std::unique_ptr<VariableWrapperBase>&& variable_wrapper);

 public:
  /**
   * \brief bool conversion.Call \ref IsValid.
   */
  explicit operator bool() const;

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
  const void* GetValue() const;

  /**
   * \brief Get the value of the object held by this object.
   * \return The value pointer of the object held by this object.
   * \remark It is not recommended for users and may cause runtime errors.
   * \remark If object is not valid, it will return nullptr.
   */
  void* GetValue();

  /**
   * \brief Set the value of the object held by this object.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   * \remark It is not recommended for users and may cause runtime errors.
   * \remark If object is not valid, it will do nothing and return false.
   */
  bool SetValue(void* other);

  /**
   * \brief Copy the new value to the object held by the pair, and this function will call the copy assignment function.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   * \remark It is not recommended for users and may cause runtime errors.
   * \remark If object is not valid, it will do nothing and return false.
   */
  bool CopyValue(void* other);

  /**
   * \brief Get the MM::Reflection::Type of the object held by this object.
   * \return The "MM::Reflection::Type" of the object held by this object.
   * \remark If object is not valid, it will return the default empty
   * MM::Reflection::Type{}.
   */
  const Type& GetType() const;

  /**
   * \brief Get meta data.
   * \return Returns std::weak_ptr containing metadata.
   * \remark If the type is not registered or this object is invalid, the
   * nullptr will be returned.
   */
  const Meta* GetMeta() const;

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
  void* ReleaseOwnership();

  /**
   * \brief Destroy this object.
   * \return If the destruction fails (the type of the object held by the variable is not registered and the variable is not property variable), false is returned; otherwise, true is returned.
   */
  bool Destroy();

  template<typename VariableType, typename ...Args>
  struct GetVariable {
    Variable operator()(Args... args) {
      return Variable{std::make_unique<VariableWrapperBase>(std::forward<Args>(args)...)};
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
  virtual bool IsValid() const;

  virtual bool IsVoid() const;

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
  virtual bool SetValue(void* object) = 0;

  /**
   * \brief Copy the new value to the object held by the pair, and this function will call the copy assignment function.
   * \param other A void * pointer to the new value.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   */
  virtual bool CopyValue(void* object) = 0;

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
  virtual const MM::Reflection::Type& GetType() const = 0;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  virtual const Meta* GetMeta() const = 0;

  /**
   * \brief Returns a pointer to the managed object and releases the ownership.
   * \return The pointer to the managed object.
   */
  virtual void* ReleaseOwnership() = 0;
};


template<typename VariableType_>
class VariableWrapper final : public VariableWrapperBase {
  template <typename TargetType_, typename DestructorType_>
  friend class DestructorWrapper;

  template <typename PropertyType_, typename ClassType_, bool IsStatic>
  friend class PropertyWrapper;

  friend class Variable;

 public:
  using Type = VariableType_;
  using OriginalType = typename TypeWrapper<VariableType_>::OriginalType;

 public:
  VariableWrapper() : value_(nullptr) {}

  ~VariableWrapper() override = default;

  /**
   * \brief Copy constructor.
   * \param other Objects to be copied.
   */
  VariableWrapper(const VariableWrapper& other)
    : value_(nullptr) {
    if (!other.IsValid()) {
      return;
    }

    other.value_.reset(std::make_unique(*other.value_));
  }
  /**
   * \brief Move constructor.
   * \param other Objects to be moved.
   */
  VariableWrapper(VariableWrapper&& other) noexcept
    : value_(nullptr) {
    other.value_.reset(std::make_unique(std::move(*other.value_)));
  }

  /**
   * \brief Copy assign.
   * \param other Objects to be copied.
   * \return New objects after copying.
   */
  VariableWrapper& operator=(const VariableWrapper& other) {
    if (std::addressof(other) == this) {
      return *this;
    }

    if (!IsValid() || !other.IsValid()) {
      return *this;
    }

    other.value_.reset(std::make_unique(*other.value_));

    return *this;
  }

  /**
   * \brief Move assign.
   * \tparam TestMove Determine whether move assign can be performed.
   * \param other Objects to be moved.
   * \return New objects after moving.
   */
  VariableWrapper& operator=(VariableWrapper&& other) noexcept {
    if (std::addressof(other) == this) {
      return *this;
    }

    if (!IsValid() || !other.IsValid()) {
      return *this;
    }

    other.value_.reset(std::make_unique(std::move(*other.value_)));

    return *this;
  }

  explicit VariableWrapper(std::unique_ptr<VariableType_>&& variable_ptr)
    : value_(variable_ptr.release()) {}

  explicit VariableWrapper(const VariableType_& other)
    : value_(std::make_unique<VariableType_>(other)) {}

  explicit VariableWrapper(VariableType_&& other)
    : value_(std::make_unique<VariableType_>(std::move(other))) {}

  template<typename ...Args>
  explicit VariableWrapper(Args... args)
    : value_(std::make_unique<VariableType_>(std::forward<Args>(args)...)) {}

 public:
  std::unique_ptr<VariableWrapperBase> CopyToBasePointer() override {
    if constexpr (std::is_copy_constructible_v<VariableType_>) {
      return std::make_unique<VariableWrapperBase>(
          VariableWrapper<VariableType_>(*this));
    } else {
      return nullptr;
    }
  }

  /**
   * \brief Get the MM::Reflection::Type of the object held by this object.
   * \return The "MM::Reflection::Type" of the object held by this object.
   */
  const MM::Reflection::Type& GetType() const override {
    static MM::Reflection::Type Result = CreateType<VariableType_>;
    return Result;
  }

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  const Meta* GetMeta() const override {
    return GetType().GetMate();
  }

  /**
   * \brief Get the value of the object held by this object.
   * \return The value pointer of the object held by this object.
   */
  void* GetValue() const override {
    return value_.get();
  }

  /**
   * \brief Set the value of the object held by this object.
   * \param object A void * pointer to the new value.
   * \return Returns true if the value is set successfully, otherwise returns
   * false.
   */
  bool SetValue(void* object) override {
      if (object == nullptr) {
        return false;
      }
      if constexpr (std::is_copy_constructible_v<VariableType_>) {
        value_.reset(std::make_unique<VariableType_>(*static_cast<VariableType_*>(object)));
        return true;
      }

      return false;
  }

  bool CopyValue(void* object) override {
    CopyHelp<std::is_copy_assignable_v<VariableType_>> help{};
    return help(*this, object);
  }

  /**
   * \brief Returns a pointer to the managed object and releases the ownership.
   * \return The pointer to the managed object.
   */
  void* ReleaseOwnership() override {
    return value_.release();
  }

 private:
template <bool TestCopy>
  struct CopyHelp {
    std::unique_ptr<VariableWrapperBase> operator()() { return nullptr;}
  };

  template <>
  struct CopyHelp<true> {
    std::unique_ptr<VariableWrapperBase> operator()() {
      return std::make_unique<VariableWrapperBase>(
          VariableWrapper<VariableType_>(*this));
    };
  };

  template <bool TestCopyAssignable>
  struct SetValueHelp {
    bool operator()(VariableWrapper&, void*) {
      return false;
    }
  };

  template <>
  struct SetValueHelp<true> {
    bool operator()(VariableWrapper& variable_wrapper, void* object) {
      if (object == nullptr) {
        return false;
      } 
      variable_wrapper.value_.reset(std::make_unique<VariableType_>(*static_cast<VariableType_*>(object)));
      return true;
    }
  };

 private:
  std::unique_ptr<VariableType_> value_ = nullptr;
};
}  // namespace Reflection
}  // namespace MM
