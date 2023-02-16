#pragma once

#include "runtime/core/reflection/type.h"
#include "runtime/core/reflection/variable.h"

namespace MM {
namespace Reflection {
class PropertyWrapperBase {
  friend class Property;

 public:
  PropertyWrapperBase() = default;
  virtual ~PropertyWrapperBase() = default;
  PropertyWrapperBase(const PropertyWrapperBase&) = default;
  PropertyWrapperBase(PropertyWrapperBase&&) = default;
  PropertyWrapperBase& operator=(const PropertyWrapperBase&) = default;
  PropertyWrapperBase& operator=(PropertyWrapperBase&&) = default;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the property, the type
   * of the class to which the property belongs, and the name of the property.
   */
  virtual std::size_t HashCode() const = 0;

  /**
   * \brief Get the MM::Reflection::Type of the class that holds the property.
   * \return The MM::Reflection::TypeWrapper of the object held by this object.
   */
  virtual MM::Reflection::Type GetType() const = 0;

  /**
   * \brief Get the MM::Reflection::Type of the class held by this property.
   * \return The MM::Reflection::TypeWrapper of the class held by this property.
   */
  virtual MM::Reflection::Type GetClassType() const = 0;

  /**
   * \brief Get meta data.
   * \return Returns the metadata of the property represented by this object.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  virtual std::weak_ptr<Meta> GetMeta() const = 0;

  /**
   * \brief Get class meta data.
   * \return Returns the metadata of the class that holds the property.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  virtual std::weak_ptr<Meta> GetClassMeta() const = 0;

 private:
  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  virtual Variable GetPropertyVariable(const Variable& class_variable) const;
};

template <typename PropertyType_, typename ClassType_, bool IsStatic>
class PropertyWrapper : public PropertyWrapperBase {};

template <typename PropertyType_, typename ClassType_>
class PropertyWrapper<PropertyType_, ClassType_, false>
    : public PropertyWrapperBase {
  friend class Property;

 public:
  using Type = PropertyType_;
  using OriginalType = typename GetOriginalType<PropertyType_>::Type;
  using ClassType = ClassType_;

 public:
  PropertyWrapper();
  ~PropertyWrapper() override;
  PropertyWrapper(const PropertyWrapper& other);
  PropertyWrapper(PropertyWrapper&& other) noexcept;
  PropertyWrapper& operator=(const PropertyWrapper& other);
  PropertyWrapper& operator=(PropertyWrapper&& other) noexcept;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the property, the type
   * of the class to which the property belongs, and the name of the property.
   */
  std::size_t HashCode() const override;

  /**
   * \brief Get the MM::Reflection::Type of the object held by this property.
   * \return The MM::Reflection::TypeWrapper of the object held by this
   * property.
   */
  MM::Reflection::Type GetType() const override;

  /**
   * \brief Get the MM::Reflection::Type of the class held by this property.
   * \return The MM::Reflection::TypeWrapper of the class held by this property.
   */
  MM::Reflection::Type GetClassType() const override;

  /**
   * \brief Get meta data.
   * \return Returns the metadata of the property represented by this object.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  std::weak_ptr<Meta> GetMeta() const override;

  /**
   * \brief Get class meta data.
   * \return Returns the metadata of the class that holds the property.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  std::weak_ptr<Meta> GetClassMeta() const override;

 private:
  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  Variable GetPropertyVariable(const Variable& class_variable) const override;

 private:
  PropertyType_ ClassType_::*address_;
};

template <typename PropertyType_, typename ClassType_>
class PropertyWrapper<PropertyType_, ClassType_, true>
    : public PropertyWrapperBase {
  friend class Property;

 public:
  using Type = PropertyType_;
  using OriginalType = typename GetOriginalType<PropertyType_>::Type;
  using ClassType = ClassType_;

 public:
  PropertyWrapper();
  ~PropertyWrapper() override;
  PropertyWrapper(const PropertyWrapper& other);
  PropertyWrapper(PropertyWrapper&& other) noexcept;
  PropertyWrapper& operator=(const PropertyWrapper& other);
  PropertyWrapper& operator=(PropertyWrapper&& other) noexcept;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the property, the type
   * of the class to which the property belongs, and the name of the property.
   */
  std::size_t HashCode() const override;

  /**
   * \brief Get the MM::Reflection::Type of the object held by this property.
   * \return The MM::Reflection::TypeWrapper of the object held by this
   * property.
   */
  MM::Reflection::Type GetType() const override;

  /**
   * \brief Get the MM::Reflection::Type of the class held by this property.
   * \return The MM::Reflection::TypeWrapper of the class held by this property.
   */
  MM::Reflection::Type GetClassType() const override;

  /**
   * \brief Get meta data.
   * \return Returns the metadata of the property represented by this object.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  std::weak_ptr<Meta> GetMeta() const override;

  /**
   * \brief Get class meta data.
   * \return Returns the metadata of the class that holds the property.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  std::weak_ptr<Meta> GetClassMeta() const override;

 private:
  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  Variable GetPropertyVariable(const Variable& class_variable) const override;

  /**
   * \brief A destructor that does nothing. Prevent the deletion of data by
   * variable reference obtained through \ref GetProperty(const Variable&
   * class_variable) from causing access errors.
   */
  struct DoNothingDestructor {
    void operator()(PropertyType_*) {}
  };

 private:
  PropertyType_* address_;
};

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, false>::PropertyWrapper() = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, false>::~PropertyWrapper() = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, false>::PropertyWrapper(
    const PropertyWrapper& other) = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, false>::PropertyWrapper(
    PropertyWrapper&& other) noexcept = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, false>&
PropertyWrapper<PropertyType, ClassType, false>::operator=(
    const PropertyWrapper& other) = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, false>&
PropertyWrapper<PropertyType, ClassType, false>::operator=(
    PropertyWrapper&& other) noexcept = default;

template <typename PropertyType_, typename ClassType_>
std::size_t PropertyWrapper<PropertyType_, ClassType_, false>::HashCode()
    const {
  return AllTypeHashCode<PropertyType_, ClassType_>() + 0;
}

template <typename PropertyType, typename ClassType>
MM::Reflection::Type PropertyWrapper<PropertyType, ClassType, false>::GetType()
    const {
  return CreateType<PropertyType>();
}

template <typename PropertyType, typename ClassType>
MM::Reflection::Type
PropertyWrapper<PropertyType, ClassType, false>::GetClassType() const {
  return CreateType<ClassType>();
}

template <typename PropertyType, typename ClassType>
std::weak_ptr<Meta> PropertyWrapper<PropertyType, ClassType, false>::GetMeta()
    const {
  return GetType().GetMate();
}

template <typename PropertyType, typename ClassType>
std::weak_ptr<Meta>
PropertyWrapper<PropertyType, ClassType, false>::GetClassMeta() const {
  return GetClassType().GetMate();
}

template <typename PropertyType, typename ClassType>
Variable PropertyWrapper<PropertyType, ClassType, false>::GetPropertyVariable(
    const Variable& class_variable) const {
  return Variable{std::make_unique<VariableWrapperBase>(
      &((*static_cast<ClassType*>(class_variable.GetValue())).address))};
}

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, true>::PropertyWrapper() = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, true>::~PropertyWrapper() = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, true>::PropertyWrapper(
    const PropertyWrapper& other) = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, true>::PropertyWrapper(
    PropertyWrapper&& other) noexcept = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, true>&
PropertyWrapper<PropertyType, ClassType, true>::operator=(
    const PropertyWrapper& other) = default;

template <typename PropertyType, typename ClassType>
PropertyWrapper<PropertyType, ClassType, true>&
PropertyWrapper<PropertyType, ClassType, true>::operator=(
    PropertyWrapper&& other) noexcept = default;

template <typename PropertyType_, typename ClassType_>
std::size_t PropertyWrapper<PropertyType_, ClassType_, true>::HashCode() const {
  return AllTypeHashCode<PropertyType_, ClassType_>() + 1;
}

template <typename PropertyType, typename ClassType>
MM::Reflection::Type PropertyWrapper<PropertyType, ClassType, true>::GetType()
    const {
  return CreateType<PropertyType>();
}

template <typename PropertyType, typename ClassType>
MM::Reflection::Type
PropertyWrapper<PropertyType, ClassType, true>::GetClassType() const {
  return CreateType<ClassType>();
}

template <typename PropertyType, typename ClassType>
std::weak_ptr<Meta> PropertyWrapper<PropertyType, ClassType, true>::GetMeta()
    const {
  return GetType().GetMate();
}

template <typename PropertyType, typename ClassType>
std::weak_ptr<Meta>
PropertyWrapper<PropertyType, ClassType, true>::GetClassMeta() const {
  return GetClassType().GetMate();
}

template <typename PropertyType, typename ClassType>
Variable PropertyWrapper<PropertyType, ClassType, true>::GetPropertyVariable(
    const Variable& class_variable) const {
  return Variable{
      std::make_unique<VariableWrapperBase>(
          &((*static_cast<ClassType*>(
                 class_variable.GetValue()))
                .address))};
}

class Property {
  friend class Meta;
  friend class Variable;

 public:
  Property();
  ~Property();
  Property(const Property& other);
  Property(Property&& other) noexcept;
  Property& operator=(const Property& other);
  Property& operator=(Property&& other) noexcept;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the property, the type
   * of the class to which the property belongs, and the name of the property.
   */
  std::size_t HashCode() const;

  /**
   * \brief Judge whether the object is a valid object.
   * \return Returns true if the object is a valid object, otherwise returns
   * false.
   */
  bool IsValid() const;

  /**
   * \brief Get the MM::Reflection::Type of the object held by this object.
   * \return The MM::Reflection::TypeWrapper of the object held by this object.
   */
  MM::Reflection::Type GetType() const;

  /**
   * \brief Get the MM::Reflection::Type of the class held by this property.
   * \return The MM::Reflection::TypeWrapper of the class held by this property.
   */
  MM::Reflection::Type GetClassType() const;

  /**
   * \brief Get the name of this property.
   * \return The name of this property.
   */
  std::string GetName() const;

  /**
   * \brief Get meta data.
   * \return Returns weak_ptr containing metadata.
   * \remark If the type is not registered or this object is invalid, the empty
   * std::weak_ptr will be returned.
   */
  std::weak_ptr<Meta> GetMate() const;

 private:
  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  Variable GetPropertyVariable(const Variable& class_variable) const;

  Property(const std::string& property_name,
           const std::shared_ptr<PropertyWrapperBase>& property_wrapper);

  static Property CreateProperty(
      const std::string& property_name,
      const std::shared_ptr<PropertyWrapperBase>& property_wrapper);

 private:
  std::string property_name_;
  std::weak_ptr<PropertyWrapperBase> property_wrapper_{};
};
}  // namespace Reflection
}  // namespace MM