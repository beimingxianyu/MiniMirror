#pragma once

#include <memory>
#include "runtime/core/reflection/property.h"
#include "runtime/core/reflection/type.h"
#include "runtime/core/reflection/variable.h"

namespace MM {
namespace Reflection {
class PropertyWrapperBase {
  friend class Property;

 public:
  PropertyWrapperBase(const std::string& property_name, std::uint64_t offset, std::uint64_t size) : property_name_(property_name), offset_(offset), size_(size) {}
  virtual ~PropertyWrapperBase() = default;
  PropertyWrapperBase(const PropertyWrapperBase& other) = delete;
  PropertyWrapperBase(PropertyWrapperBase&& other) = delete; 
  PropertyWrapperBase& operator=(const PropertyWrapperBase& other) = delete;
  PropertyWrapperBase& operator=(PropertyWrapperBase&&) = delete;

 public:
  const std::string& GetPropertyName() const {return property_name_;}

  std::uint64_t GetPropertyOffset() const {return offset_;}

  std::uint64_t GetPropertySize() const {return size_;}

  virtual bool IsValid() const {
    return size_ != 0;
  }

  virtual bool IsStaticProperty() const = 0;

  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the property, the type
   * of the class to which the property belongs, and the name of the property.
   */
  virtual std::size_t HashCode() const = 0;

  /**
   * \brief Get the MM::Reflection::Type of the class that holds the property.
   * \return The MM::Reflection::Type of the object held by this object.
   */
  virtual const MM::Reflection::Type* GetType() const = 0;

  /**
   * \brief Get the MM::Reflection::Type of the class held by this property.
   * \return The MM::Reflection::TypeWrapper of the class held by this property.
   */
  virtual const MM::Reflection::Type* GetClassType() const = 0;

  /**
   * \brief Get meta data.
   * \return Returns the metadata of the property represented by this object.
   * \remark If the type is not registered, the nullptr will be returned.
   */
  virtual const Meta* GetMeta() const = 0;

  /**
   * \brief Get class meta data.
   * \return Returns the metadata of the class that holds the property.
   * \remark If the type is not registered, the containing nullptr will be returned.
   */
  virtual const Meta* GetClassMeta() const = 0;

 private:
  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  virtual Variable GetPropertyVariable(Variable& class_variable) = 0;

  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  virtual Variable GetPropertyVariable(const Variable& class_variable) const = 0;

  private:
    std::string property_name_{};
    std::uint64_t offset_{0};
    std::uint64_t size_{0};
};


template <typename PropertyType_, typename ClassType_>
class CommonPropertyWrapper
    : public PropertyWrapperBase {
  friend class Property;

 public:
  using Type = PropertyType_;
  using OriginalType = typename GetOriginalType<PropertyType_>::Type;
  using ClassType = ClassType_;

 public:
  CommonPropertyWrapper(const std::string& property_name, std::uint64_t offset, std::uint64_t size) : PropertyWrapperBase(property_name, offset, size) {}
  ~CommonPropertyWrapper() override = default;
  CommonPropertyWrapper(const CommonPropertyWrapper& other) = delete;
  CommonPropertyWrapper(CommonPropertyWrapper&& other) noexcept = delete;
  CommonPropertyWrapper& operator=(const CommonPropertyWrapper& other) = delete;
  CommonPropertyWrapper& operator=(CommonPropertyWrapper&& other) noexcept = delete;

 public:
  /**
   * \brief Get the hashcode of this class.
   * \return The hashcode of this class.
   * \remark The value of hashcode depends on the type of the property, the type
   * of the class to which the property belongs, and the name of the property.
   */
  std::uint64_t HashCode() const override {
    return AllTypeHashCode<PropertyType_, ClassType_>() + std::hash<std::string>(GetPropertyName());
  }

  /**
   * \brief Get the MM::Reflection::Type of the object held by this property.
   * \return The MM::Reflection::TypeWrapper of the object held by this
   * property.
   */
  const MM::Reflection::Type* GetType() const override {
    static MM::Reflection::Type Type = CreateType<PropertyType_>();
    return &Type;
  }

  /**
   * \brief Get the MM::Reflection::Type of the class held by this property.
   * \return The MM::Reflection::TypeWrapper of the class held by this property.
   */
  const MM::Reflection::Type* GetClassType() const override {
    static MM::Reflection::Type Type = CreateType<ClassType_>();
    return &Type;
  }

  /**
   * \brief Get meta data.
   * \return Returns the metadata of the property represented by this object.
   * \remark If the type is not registered, the nullptr will be returned.
   */
  const Meta* GetMeta() const override {
    const MM::Reflection::Type* PropertyType = GetType();
    assert(PropertyType != nullptr);
    return PropertyType->GetMate();
  }

  /**
   * \brief Get class meta data.
   * \return Returns the metadata of the class that holds the property.
   * \remark If the type is not registered, the nullptr will be returned.
   */
  const Meta* GetClassMeta() const override {
    const MM::Reflection::Type* ClassType = GetType();
    assert(ClassType != nullptr);
    return ClassType->GetMate();
  }

};

template <typename PropertyType_, typename ClassType_>
class PropertyWrapper
    : public CommonPropertyWrapper<PropertyType_, ClassType_> {
  public:
    bool IsStaticProperty() const {
      return false;
    }

 private:
  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  Variable GetPropertyVariable(Variable& class_variable) override {
    PropertyType_* property_ptr =reinterpret_cast<PropertyType_*>(static_cast<char*>(class_variable.GetValue()) + GetPropertyOffset());
    return Variable{std::make_unique<PropertyVariable<PropertyType_>>(*property_ptr)};
  }

  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  Variable GetPropertyVariable(const Variable& class_variable) const override {
    const PropertyType_* property_ptr =reinterpret_cast<const PropertyType_*>(static_cast<const char*>(class_variable.GetValue()) + GetPropertyOffset());
    return Variable{std::make_unique<PropertyVariable<const PropertyType_>>(*property_ptr)};
  }
};

template <typename PropertyType_, typename ClassType_>
class StaticPropertyWrapper
    : public CommonPropertyWrapper<PropertyType_, ClassType_> {
  public:
    using StaticPropertyType = PropertyType_ ClassType_::*;

  public:
   StaticPropertyWrapper(const std::string& property_name,
                         StaticPropertyType address, std::uint64_t size)
       : PropertyWrapperBase(property_name, 0, size),
         static_property_address_(address) {}
  virtual ~StaticPropertyWrapper() = default; 

  public:
    bool IsValid() const override {
      return PropertyWrapperBase::IsValid() && static_property_address_ != nullptr;
    }

    bool IsStaticProperty() const {
      return true;
    }

 private:
  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  Variable GetPropertyVariable(Variable&) override {
    return Variable{std::make_unique<PropertyVariable<PropertyType_>>(*static_property_address_)};
  }

  /**
   * \brief Gets the variable that the attribute refers to.
   * \param class_variable The class variable of this property.
   * \return A MM::Reflection::Variable that holds the this property contained
   * in \ref class_variable.
   */
  Variable GetPropertyVariable(const Variable&) const override {
    return Variable{std::make_unique<PropertyVariable<const PropertyType_>>(*static_property_address_)};
  }

  private:
    StaticPropertyType static_property_address_{nullptr};
};


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
};
}  // namespace Reflection
}  // namespace MM