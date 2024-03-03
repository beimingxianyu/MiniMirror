#include "runtime/core/reflection/method.h"

const std::string MM::Reflection::MethodWrapperBase::empty_string{};

const std::string MM::Reflection::Method::empty_string{};

MM::Reflection::MethodWrapperBase::MethodWrapperBase(
    const std::string& method_name)
    : method_name_(method_name) {}
const std::string& MM::Reflection::MethodWrapperBase::GetMethodName() const {
  return method_name_;
}

std::size_t MM::Reflection::MethodWrapperBase::HashCode() const {
  return std::hash<std::string>{}(method_name_);
}

bool MM::Reflection::MethodWrapperBase::IsValid() const {
  return !method_name_.empty();
}

const std::string& MM::Reflection::Method::GetMethodName() const {
  if (!IsValid()) {
    return empty_string;
  }

  return method_wrapper_->GetMethodName();
}

std::size_t MM::Reflection::Method::HashCode() const {
  if (!IsValid()) {
    return 0;
  }

  return method_wrapper_->HashCode();
}

bool MM::Reflection::Method::IsValid() const {
  return method_wrapper_ != nullptr && method_wrapper_->IsValid();
}

bool MM::Reflection::Method::IsStatic() const {
  if (!IsValid()) {
    return false;
  }

  return method_wrapper_->IsStatic();
}

std::size_t MM::Reflection::Method::GetArgumentNumber() const {
  if (!IsValid()) {
    return 0;
  }

  return method_wrapper_->GetArgumentNumber();
}

const MM::Reflection::Type* MM::Reflection::Method::GetClassType() const {
  if (!IsValid()) {
    return nullptr;
  }

  return method_wrapper_->GetClassType();
}

const MM::Reflection::Type* MM::Reflection::Method::GetArgumentType(
    std::uint32_t argument_index) const {
  if (!IsValid()) {
    return nullptr;
  }

  return method_wrapper_->GetArgumentType(argument_index);
}

const MM::Reflection::Type* MM::Reflection::Method::GetReturnType() const {
  if (!IsValid()) {
    return nullptr;
  }

  return method_wrapper_->GetReturnType();
}

const MM::Reflection::Meta* MM::Reflection::Method::GetClassMeta() const {
  const Type* class_type = method_wrapper_->GetClassType();
  if (class_type == nullptr) {
    return nullptr;
  }

  return class_type->GetMate();
}

const MM::Reflection::Meta* MM::Reflection::Method::GetArgumentMeta(
    std::uint32_t argument_index) const {
  const Type* argument_type = method_wrapper_->GetArgumentType(argument_index);
  if (argument_type == nullptr) {
    return nullptr;
  }

  return argument_type->GetMate();
}

const MM::Reflection::Meta* MM::Reflection::Method::GetReturnMeta() const {
  const Type* return_type = method_wrapper_->GetReturnType();
  if (return_type == nullptr) {
    return nullptr;
  }

  return return_type->GetMate();
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    Variable& instance) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(Variable& instance,
                                                        Variable& arg1) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, arg1);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(Variable& instance,
                                                        Variable& arg1,
                                                        Variable& arg2) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, arg1, arg2);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(Variable& instance,
                                                        Variable& arg1,
                                                        Variable& arg2,
                                                        Variable& arg3) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, arg1, arg2, arg3);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(Variable& instance,
                                                        Variable& arg1,
                                                        Variable& arg2,
                                                        Variable& arg3,
                                                        Variable& arg4) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, arg1, arg2, arg3, arg4);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    Variable& instance, Variable& arg1, Variable& arg2, Variable& arg3,
    Variable& arg4, Variable& arg5) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, arg1, arg2, arg3, arg4, arg5);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    Variable& instance, Variable& arg1, Variable& arg2, Variable& arg3,
    Variable& arg4, Variable& arg5, Variable& arg6) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, arg1, arg2, arg3, arg4, arg5, arg6);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    Variable& instance, std::vector<Variable*>& args) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, args);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    Variable& instance, std::vector<Variable*>&& args) const {
  if (!IsValid()) {
    return Variable{};
  }

  return method_wrapper_->Invoke(instance, args);
}
