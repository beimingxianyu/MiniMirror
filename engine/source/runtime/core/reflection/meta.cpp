#include "meta.h"

bool MM::Reflection::Meta::IsValid() const { return !type_name_.empty(); }

std::string MM::Reflection::Meta::GetTypeName() const { return type_name_; }

MM::Reflection::Type MM::Reflection::Meta::GetType() const { return type_; }

MM::Reflection::Destructor MM::Reflection::Meta::GetDestructor() const {
  if (!IsValid()) {
    return Destructor{};
  }
  return Destructor(destructor_);
}

bool MM::Reflection::Meta::HaveMethod(const std::string& method_name) const {
  if (!IsValid()) {
    return false;
  }
  return methods_.find(method_name) != methods_.end();
}

MM::Reflection::Method MM::Reflection::Meta::GetMethod(
    const std::string& method_name) const {
  if (IsValid()) {
    if (methods_.find(method_name) != methods_.end()) {
      return Method{method_name, methods_.at(method_name)};
    }
  }
  return Method{};
}

std::vector<MM::Reflection::Method> MM::Reflection::Meta::GetAllMethod() const {
  std::vector<Method> method_list;
  if (IsValid()) {
    for (const auto& method : methods_) {
      method_list.push_back(Method{method.first, method.second});
    }
    return method_list;
  }
  return method_list;
}

bool MM::Reflection::Meta::
HaveProperty(const std::string& property_name) const {
  if (!IsValid()) {
    return false;
  }
  return properties.find(property_name) != properties.end();
}

MM::Reflection::Property MM::Reflection::Meta::GetProperty(
    const std::string& property_name) const {
  if (IsValid()) {
    if (properties.find(property_name) != properties.end()) {
      return Property(property_name, properties.at(property_name));
    }
  }
  return Property{};
}

std::vector<MM::Reflection::Property> MM::Reflection::Meta::GetAllProperty()
    const {
  std::vector<Property> property_list;
  if (IsValid()) {
    for (const auto& property_ : properties) {
      property_list.push_back(Property{property_.first, property_.second});
    }
  }
  return property_list;
}

bool MM::Reflection::Meta::SetDestructor(
    const std::shared_ptr<DestructorWrapperBase>& destructor_wrapper) {
  if (IsValid()) {
    if (destructor_wrapper->GetType().GetTypeHashCode() ==
        destructor_->GetType().GetTypeHashCode()) {
      destructor_ = destructor_wrapper;
      return true;
    }
  }
  return false;
}

bool MM::Reflection::Meta::AddConstructor(
    const std::shared_ptr<ConstructorWrapperBase>& constructor_wrapper) {
  if (IsValid()) {
    if (GetType().GetTypeHashCode() ==
        constructor_wrapper->GetType().GetTypeHashCode()) {
      const std::size_t other_hash_code = constructor_wrapper->HashCode();
      for (const auto& constructor : constructors_) {
        if (other_hash_code == constructor->HashCode()) {
          return false;
        }
      }
      constructors_.push_back(constructor_wrapper);
      return true;
    }
  }
  return false;
}

bool MM::Reflection::Meta::AddMethod(
    const std::string& method_name,
    const std::shared_ptr<MethodWrapperBase>& method_wrapper) {
  if (IsValid()) {
    if (GetType().GetTypeHashCode() ==
        method_wrapper->GetClassType().GetTypeHashCode()) {
      const std::size_t other_hash_code = method_wrapper->HashCode();
      for (const auto& method : methods_) {
        if (other_hash_code == method.second->HashCode()) {
          return false;
        }
      }
      methods_[method_name] = method_wrapper;
      return true;
    }
  }
  return false;
}

bool MM::Reflection::Meta::AddProperty(
    const std::string& property_name,
    const std::shared_ptr<PropertyWrapperBase>& property_wrapper) {
  if (IsValid()) {
    if (GetType().GetTypeHashCode() ==
        property_wrapper->GetClassType().GetTypeHashCode()) {
      const std::size_t other_hash_code = property_wrapper->HashCode();
      for (const auto& property_ : properties) {
        if (other_hash_code == property_.second->HashCode()) {
          return false;
        }
      }
      properties[property_name] = property_wrapper;
      return true;
    }
  }
  return false;
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance() const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == 0) {
        Variable temp = constructor->Invoke();
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    Variable& arg1) const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == 1) {
        Variable temp = constructor->Invoke(arg1);
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(Variable& arg1,
    Variable& arg2) const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == 2) {
        Variable temp = constructor->Invoke(arg1, arg2);
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(Variable& arg1,
    Variable& arg2, Variable& arg3) const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == 3) {
        Variable temp = constructor->Invoke(arg1, arg2, arg3);
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(Variable& arg1,
    Variable& arg2, Variable& arg3, Variable& arg4) const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == 4) {
        Variable temp = constructor->Invoke(arg1, arg2, arg3, arg4);
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(Variable& arg1,
    Variable& arg2, Variable& arg3, Variable& arg4, Variable& arg5) const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == 5) {
        Variable temp = constructor->Invoke(arg1, arg2, arg3, arg4, arg5);
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(Variable& arg1,
    Variable& arg2, Variable& arg3, Variable& arg4, Variable& arg5,
    Variable& arg6) const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == 6) {
        Variable temp = constructor->Invoke(arg1, arg2, arg3, arg4, arg5, arg6);
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Meta::CreateInstance(
    std::vector<Variable>& args) const {
  if (IsValid()) {
    for (const auto& constructor : constructors_) {
      if (constructor->GetArgumentNumber() == args.size()) {
        Variable temp = constructor->Invoke(args);
        if (temp.IsValid()) {
          return temp;
        }
      }
    }
  }
  return Variable{};
}
