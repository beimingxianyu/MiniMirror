#include "runtime/core/reflection/meta.h"

MM::Reflection::Meta::Meta(
    const std::string& type_name, Type&& type,
    std::unordered_map<std::string, Method>&& constructors,
    std::unordered_map<std::string, Method>&& methods,
    std::unordered_map<std::string, Property>&& properties)
    : type_name_(type_name),
      type_(std::move(type)),
      constructors_(std::move(constructors)),
      methods_(std::move(methods)),
      properties_(std::move(properties)) {}

const std::string& MM::Reflection::Meta::GetTypeName() const {
  return type_name_;
}

const MM::Reflection::Type& MM::Reflection::Meta::GetType() const {
  return type_;
}

bool MM::Reflection::Meta::HaveConstructor(
    const std::string& constructor_name) const {
  return constructors_.find(constructor_name) != constructors_.end();
}

const MM::Reflection::Method* MM::Reflection::Meta::GetConstuctor(
    const std::string& constructor_name) const {
  if (const auto& find_result = constructors_.find(constructor_name);
      find_result != constructors_.end()) {
    return &(find_result->second);
  }

  return nullptr;
}

std::vector<const MM::Reflection::Method*> MM::Reflection::Meta::GetAllConstuctor() const {
  if (constructors_.empty()) {
    return std::vector<const Method*>{};
  }

  std::vector<const Method*> result{constructors_.size(), nullptr};
  for (const std::pair<const std::string, Method>& one_method : constructors_) {
    result.push_back(&(one_method.second));
  }

  return result;
}

bool MM::Reflection::Meta::HaveMethod(const std::string& method_name) const {
  return methods_.find(method_name) != methods_.end();
}

const MM::Reflection::Method* MM::Reflection::Meta::GetMethod(
    const std::string& method_name) const {
  if (const auto& find_result = methods_.find(method_name);
      find_result != methods_.end()) {
    return &(find_result->second);
  }

  return nullptr;
}

std::vector<const MM::Reflection::Method*> MM::Reflection::Meta::GetAllMethod() const {
  if (methods_.empty()) {
    return std::vector<const Method*>{};
  }

  std::vector<const Method*> result{methods_.size(), nullptr};
  for (const std::pair<const std::string, Method>& one_method : methods_) {
    result.push_back(&(one_method.second));
  }

  return result;
}

bool MM::Reflection::Meta::HaveProperty(
    const std::string& property_name) const {
  return properties_.find(property_name) != properties_.end();
}

const MM::Reflection::Property* MM::Reflection::Meta::GetProperty(
    const std::string& property_name) const {
  if (const auto& find_result = properties_.find(property_name);
      find_result != properties_.end()) {
    return &(find_result->second);
  }

  return nullptr;
}

std::vector<const MM::Reflection::Property*> MM::Reflection::Meta::GetAllProperty() const {
  if (properties_.empty()) {
    return std::vector<const Property*>{};
  }

  std::vector<const Property*> result{properties_.size(), nullptr};
  for (const std::pair<const std::string, Property>& one_properties :
       properties_) {
    result.push_back(&(one_properties.second));
  }

  return result;
}

bool MM::Reflection::Meta::AddConstructor(Method&& constuctor) {
  if (!constuctor.IsValid()) {
    return false;
  }

  constructors_[constuctor.GetMethodName()] = std::move(constuctor);

  return true;
}

void MM::Reflection::Meta::RemoveConstructor(
    const std::string& constructor_name) {
  constructors_.erase(constructor_name);
}

bool MM::Reflection::Meta::AddMethod(Method&& method) {
  if (!method.IsValid()) {
    return false;
  }

  methods_[method.GetMethodName()] = std::move(method);

  return true;
}

void MM::Reflection::Meta::RemoveMethod(const std::string& method_name) {
  methods_.erase(method_name);
}

bool MM::Reflection::Meta::AddProperty(Property&& property) {
  if (!property.IsValid()) {
    return false;
  }

  if (HaveProperty(property.GetPropertyName())) {
    return false;
  }

  properties_.emplace(property.GetPropertyName(), std::move(property));

  return true;
}

void MM::Reflection::Meta::RemoveProperty(const std::string& property_name) {
  properties_.erase(property_name);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name, Variable& arg1) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance, arg1);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name, Variable& arg1, Variable& arg2) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance, arg1, arg2);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name, Variable& arg1, Variable& arg2,
    Variable& arg3) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance, arg1, arg2, arg3);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name, Variable& arg1, Variable& arg2,
    Variable& arg3, Variable& arg4) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance, arg1, arg2, arg3, arg4);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name, Variable& arg1, Variable& arg2,
    Variable& arg3, Variable& arg4, Variable& arg5) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance, arg1, arg2, arg3, arg4, arg5);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name, Variable& arg1, Variable& arg2,
    Variable& arg3, Variable& arg4, Variable& arg5, Variable& arg6) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance, arg1, arg2, arg3, arg4, arg5,
                             arg6);
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    const std::string& constructor_name, std::vector<Variable*>& args) const {
  const Method* constructor = GetConstuctor(constructor_name);
  if (constructor == nullptr) {
    return Variable{};
  }

  Variable empty_instance{};
  return constructor->Invoke(empty_instance, args);
}
