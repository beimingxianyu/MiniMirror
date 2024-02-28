#pragma once

#include <string>
#include <vector>

#include "runtime/core/reflection/database.h"
#include "runtime/core/reflection/type.h"
#include "runtime/core/reflection/method.h"
#include "runtime/core/reflection/property.h"
#include "runtime/core/reflection/constructor.h"

namespace MM {
namespace Reflection {
class Meta {
public:
  Meta() = delete;
   Meta(const Meta& other) = delete;
  Meta(Meta&& other) noexcept = delete;
  Meta& operator=(const Meta& other) = delete;
  Meta& operator=(Meta&& other) = delete;

public:
  /**
   * \brief Judge whether the object is a valid object.
   * \return If the object is invalid(default meta), false is returned; otherwise, true is returned.
   */
  bool IsValid() const;

  /**
   * \brief Get type name of this meta.
   * \return The type name of this meta.
   */
  std::string GetTypeName() const;

  /**
   * \brief Get the \ref MM::Reflection::Type of this object.
   * \return The \ref MM::Reflection::Type of this object.
   */
  Type GetType() const;

  /**
   * \brief Gets the \ref MM::Reflection::Destructor of the type that this metadata refers to.
   * \return The destructor of the type that this metadata refers to.
   */
  Destructor GetDestructor() const;

  /**
   * \brief Determine whether the method specified by \ref method_name exists.
   * \param method_name The method name;
   * \return If the specified method exists, return true; otherwise, return false.
   */
  bool HaveMethod(const std::string& method_name) const;

  /**
   * \brief Get \ref MM::Reflection::Method with a name.
   * \param method_name The name of method.
   * \return The \ref MM::Reflection::Method.
   */
  const Method* GetMethod(const std::string& method_name) const;

  /**
   * \brief Get all \ref MM::Reflection::Method of this meta.
   * \return All \ref MM::Reflection::Method of this meta.
   */
  std::vector<Method> GetAllMethod() const;

  /**
   * \brief Determine whether the property specified by \ref property_name exists.
   * \param property_name The property name;
   * \return If the specified property exists, return true; otherwise, return
   * false.
   */
  bool HaveProperty(const std::string& property_name) const;

  /**
   * \brief Get \ref MM::Reflection::Property with a name.
   * \param property_name The name of property name.
   * \return The \ref MM::Reflection::Property.
   */
  const Property* GetProperty(const std::string& property_name) const;

  /**
   * \brief Get all \ref MM::Reflection::Property of this meta.
   * \return All \ref MM::Reflection::Property of this meta.
   */
  std::vector<Property> GetAllProperty() const;

  /**
   * \brief Set destructor.
   * \param destructor_wrapper The new destructor to be set.
   * \return Returns true if the operation is successful, otherwise returns false.
   */
  bool SetDestructor(
      const std::shared_ptr<DestructorWrapperBase>& destructor_wrapper);

  /**
   * \brief Add new constructor.
   * \param constructor_wrapper The constructor wrapper.
   * \return Returns true if the operation is successful, otherwise returns false.
   * \remark If a new entry is added to register, the registration will not be repeated.
   */
  bool AddConstructor(
      const std::shared_ptr<ConstructorWrapperBase>& constructor_wrapper);

  /**
   * \brief Register method to metadata.
   * \param method_name The name of the method.
   * \param method_wrapper The method wrapper.
   * \return Returns true if the operation is successful, otherwise returns false.
   * \remark If a new entry is added to register, the registration will not be repeated.
   */
  bool AddMethod(const std::string& method_name, const std::shared_ptr<MethodWrapperBase>& method_wrapper);

  /**
   * \brief Register property to metadata.
   * \param property_name The name of property.
   * \param property_wrapper The property wrapper.
   * \return Returns true if the operation is successful, otherwise returns false.
   * \remark If a new entry is added to register, the registration will not be repeated.
   */
  bool AddProperty(const std::string& property_name,
      const std::shared_ptr<PropertyWrapperBase>& property_wrapper);

  /**
   * \brief Create instance with 0 arguments.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance() const;

  /**
   * \brief Create instance with 1 arguments.
   * \param arg1 1st argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance(Variable& arg1) const;

  /**
   * \brief Create instance with 2 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance(Variable& arg1, Variable& arg2) const;

  /**
   * \brief Create instance with 3 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument
   * \param arg3 3rd argument
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance(Variable& arg1, Variable& arg2, Variable& arg3) const;

  /**
   * \brief Create with 4 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance(Variable& arg1, Variable& arg2, Variable& arg3, Variable& arg4) const;

  /**
   * \brief Create instance with 5 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance(Variable& arg1, Variable& arg2, Variable& arg3,
                          Variable& arg4, Variable& arg5) const;

  /**
   * \brief Create instance with 6 arguments.
   * \param arg1 1st argument.
   * \param arg2 2ed argument.
   * \param arg3 3rd argument.
   * \param arg4 4th argument.
   * \param arg5 5th argument.
   * \param arg6 6th argument.
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance(Variable& arg1, Variable& arg2, Variable& arg3,
                          Variable& arg4, Variable& arg5, Variable& arg6) const;

  /**
   * \brief Create instance with any number of parameters.
   * \param args The list of arguments,
   * \return \ref MM::Reflection::Variable containing the return value of this
   * function.
   * \remark If the number or type of incoming argument is different
   * from the argument required by the constructor held by this meta, the
   * function held by this object will not be called and return an empty \ref
   * MM::Reflection::Variable.
   */
  Variable CreateInstance(std::vector<Variable>& args) const;

private:
  Meta(const std::string& type_name, const Type& type,
       const std::shared_ptr<DestructorWrapperBase>& destructor = nullptr)
    : type_name_(type_name),
      type_(type),
      destructor_(destructor) {
  }

private:
  /**
  * \brief The type name of this Meta.
  */
  std::string type_name_;

  /**
   * \brief The \ref MM::Reflection::Type of this object.
   */
  Type type_;

  /**
   * \brief Constructor list.
   */
  std::vector<std::shared_ptr<ConstructorWrapperBase>> constructors_;

  /**
   * \brief Destructor.
   */
  std::shared_ptr<DestructorWrapperBase> destructor_;

  /**
   * \brief Method list.
   */
  std::unordered_map<std::string, std::shared_ptr<MethodWrapperBase>> methods_;

  /**
   * \brief Property list.
   */
  std::unordered_map<std::string, std::shared_ptr<PropertyWrapperBase>> properties;
};

// Delay definition to prevent circular reference of header file.(start)
template <typename TargetType_, typename DestructorType_>
bool DestructorWrapper<TargetType_, DestructorType_>::Destroy(
    Variable& object) const {
  if (GetType().GetTypeHashCode() == object.GetType().GetTypeHashCode()) {
    if (object.IsPropertyVariable()) {
      object.Release();
      return true;
    }
    std::unique_ptr<TargetType_, DestructorType_> new_unique{
            object.Release()};
    return true;
  }
  return false;
}

template <typename TargetType_>
bool DestructorWrapper<TargetType_, DefaultDestructor>::Destroy(
    Variable& object) const {
  if (GetType().GetTypeHashCode() == object.GetType().GetTypeHashCode()) {
    if (object.IsPropertyVariable()) {
      object.Release();
      return true;
    }
    std::unique_ptr<TargetType_> new_unique{object.Release()};
    return true;
  }
  return false;
}

// Delay definition to prevent circular reference of header file.(end)
}
}
