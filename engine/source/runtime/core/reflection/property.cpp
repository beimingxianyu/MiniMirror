#include "runtime/core/reflection/property.h"

MM::Reflection::Property::Property() : property_wrapper_{} {}

MM::Reflection::Property::~Property() = default;

MM::Reflection::Property::Property(const Property& other) : property_wrapper_(other.property_wrapper_){}

MM::Reflection::Property::Property(Property&& other) noexcept {
  std::swap(property_wrapper_, other.property_wrapper_);
  other.property_wrapper_.reset();
}

MM::Reflection::Property& MM::Reflection::Property::operator=(
    const Property& other) {
  if (this == &other) {
    return *this;
  }
  property_wrapper_ = other.property_wrapper_;
  return *this;
}

MM::Reflection::Property& MM::Reflection::Property::operator=(
    Property&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  std::swap(property_wrapper_, other.property_wrapper_);
  other.property_wrapper_.reset();
  return *this;
}

std::size_t MM::Reflection::Property::HashCode() const {
  return property_wrapper_.lock()->HashCode() +
         std::hash<std::string>()(property_name_);
}

bool MM::Reflection::Property::IsValid() const {
  return (property_wrapper_.expired()) && (!property_name_.empty());
}

MM::Reflection::Type MM::Reflection::Property::GetType() const {
  if (!IsValid()) {
    return Type{};
  }
  return property_wrapper_.lock()->GetType();
}

MM::Reflection::Type MM::Reflection::Property::GetClassType() const {
  if (!IsValid()) {
    return Type{};
  }
  return property_wrapper_.lock()->GetClassType();
}

std::string MM::Reflection::Property::GetName() const {
  if (!IsValid()) {
    return std::string{};
  }
  return property_name_;
}

std::weak_ptr<MM::Reflection::Meta> MM::Reflection::Property::
GetMate() const {
  if (IsValid()) {
    return std::weak_ptr<MM::Reflection::Meta>{g_meta_database[std::string{}]};
  }
  return property_wrapper_.lock()->GetMeta();
}

MM::Reflection::Variable MM::Reflection::Property::GetPropertyVariable(
    const Variable& class_variable) const {
  if (class_variable.GetType().GetTypeHashCode() ==
      property_wrapper_.lock()->GetClassType().GetOriginalTypeHashCode()) {
    return property_wrapper_.lock()->GetPropertyVariable(class_variable);
  }
  return Variable{};
}

MM::Reflection::Property::Property(const std::string& property_name,
                                   const std::shared_ptr<PropertyWrapperBase>& property_wrapper) : property_name_(property_name), property_wrapper_(property_wrapper){}

MM::Reflection::Property MM::Reflection::Property::CreateProperty(
    const std::string& property_name,
    const std::shared_ptr<PropertyWrapperBase>& property_wrapper) {
  return Property{property_name, property_wrapper};
}


