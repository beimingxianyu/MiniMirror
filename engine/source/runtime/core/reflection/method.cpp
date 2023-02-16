#include "runtime/core/reflection/method.h"

MM::Reflection::Method::Method() : method_name_(), method_wrapper_() {}

MM::Reflection::Method::Method(const Method& other)
    : method_name_(other.method_name_),
      method_wrapper_(other.method_wrapper_) {}

MM::Reflection::Method::Method(Method&& other) noexcept {
  std::swap(method_name_, other.method_name_);
  std::swap(method_wrapper_, other.method_wrapper_);
  other.method_name_.clear();
  other.method_wrapper_.reset();
}

MM::Reflection::Method& MM::Reflection::Method::operator=(const Method& other) {
  if (this == &other) {
    return *this;
  }
  method_name_ = other.method_name_;
  method_wrapper_ = other.method_wrapper_;
  return *this;
}

MM::Reflection::Method& MM::Reflection::Method::operator=(
    Method&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  std::swap(method_name_, other.method_name_);
  std::swap(method_wrapper_, other.method_wrapper_);
  other.method_name_.clear();
  other.method_wrapper_.reset();
  return *this;
}

std::size_t MM::Reflection::Method::HashCode() const {
  return method_wrapper_.lock()->HashCode() +
         std::hash<std::string>()(method_name_);
}

bool MM::Reflection::Method::IsValid() const {
  return method_wrapper_.expired() && (!method_name_.empty());
}

bool MM::Reflection::Method::IsStatic() const {
  if (!IsValid()) {
    return false;
  }
  return method_wrapper_.lock()->IsStatic();
}

std::size_t MM::Reflection::Method::GetArgumentNumber() const {
  if (!IsValid()) {
    return 0;
  }
  return method_wrapper_.lock()->GetArgumentNumber();
}

MM::Reflection::Type MM::Reflection::Method::GetClassType() const {
  if (!IsValid()) {
    return Type{};
  }
  return method_wrapper_.lock()->GetClassType();
}

MM::Reflection::Type MM::Reflection::Method::GetReturnType() const {
  if (!IsValid()) {
    return Type{};
  }
  return method_wrapper_.lock()->GetReturnType();
}

std::weak_ptr<MM::Reflection::Meta> MM::Reflection::Method::GetClassMeta()
    const {
  if (!IsValid()) {
    return std::weak_ptr<MM::Reflection::Meta>{g_meta_database[std::string{}]};
  }
  return method_wrapper_.lock()->GetClassMeta();
}

std::weak_ptr<MM::Reflection::Meta> MM::Reflection::Method::GetReturnMeta()
    const {
  if (!IsValid()) {
    return std::weak_ptr<MM::Reflection::Meta>{g_meta_database[std::string{}]};
  }
  return method_wrapper_.lock()->GetReturnMeta();
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    MM::Reflection::Variable& instance) const {
  if (!IsValid() || method_wrapper_.lock()->GetArgumentNumber() != 0 ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(MM::Reflection::Variable& instance,
  MM::Reflection::Variable& arg1) const {
  if (!IsValid() || method_wrapper_.lock()->GetArgumentNumber() != 1 ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance, arg1);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(MM::Reflection::Variable& instance,
  MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2) const {
  if (!IsValid() || method_wrapper_.lock()->GetArgumentNumber() != 2 ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance, arg1, arg2);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(MM::Reflection::Variable& instance,
  MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2,
  MM::Reflection::Variable& arg3) const {
  if (!IsValid() || method_wrapper_.lock()->GetArgumentNumber() != 3 ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance, arg1, arg2, arg3);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(MM::Reflection::Variable& instance,
  MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2,
  MM::Reflection::Variable& arg3,
  MM::Reflection::Variable& arg4) const {
  if (!IsValid() || method_wrapper_.lock()->GetArgumentNumber() != 4 ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance, arg1, arg2, arg3, arg4);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    MM::Reflection::Variable& instance, MM::Reflection::Variable& arg1, MM::
    Reflection::Variable& arg2, MM::Reflection::Variable& arg3,
    MM::Reflection::Variable& arg4, MM::Reflection::Variable& arg5) const {
  if (!IsValid() || method_wrapper_.lock()->GetArgumentNumber() != 5 ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance, arg1, arg2, arg3, arg4, arg5);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    MM::Reflection::Variable& instance, MM::Reflection::Variable& arg1, MM::
    Reflection::Variable& arg2, MM::Reflection::Variable& arg3,
    MM::Reflection::Variable& arg4, MM::Reflection::Variable& arg5, MM::
    Reflection::Variable& arg6) const {
  if (!IsValid() || method_wrapper_.lock()->GetArgumentNumber() != 6 ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance, arg1, arg2, arg3, arg4, arg5,
                                        arg6);
}

MM::Reflection::Variable MM::Reflection::Method::Invoke(
    MM::Reflection::Variable& instance, std::vector<MM::Reflection::Variable>&
    args) const {
  if (!IsValid() ||
      method_wrapper_.lock()->GetArgumentNumber() != args.size() ||
      method_wrapper_.lock()->GetClassType().GetTypeHashCode() !=
          instance.GetType().GetTypeHashCode()) {
    return Variable{};
  }
  return method_wrapper_.lock()->Invoke(instance, args);
}

MM::Reflection::Method::Method(const std::string& method_name,
    const std::shared_ptr<MethodWrapperBase>& method_wrapper): method_name_(method_name), method_wrapper_(method_wrapper) {}

MM::Reflection::Method MM::Reflection::Method::CreateMethod(
    const std::string& method_name,
    const std::shared_ptr<MethodWrapperBase>& method_wrapper) {
  return Method(method_name, method_wrapper);
}
