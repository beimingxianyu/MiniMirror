#include "runtime/core/reflection/constructor.h"

std::size_t MM::Reflection::Constructor::HashCode() const {
  return constructor_wrapper_.lock()->HashCode();
}

bool MM::Reflection::Constructor::IsValid() const {
  return constructor_wrapper_.expired();
}

MM::Reflection::Type MM::Reflection::Constructor::GetType() const {
  if (!IsValid()) {
    return Type{};
  }
  return constructor_wrapper_.lock()->GetType();
}

std::weak_ptr<MM::Reflection::Meta> MM::Reflection::Constructor::
GetMeta() const {
  if (!IsValid()) {
    return std::weak_ptr<MM::Reflection::Meta>{g_meta_database[std::string{}]};
  }
  return GetType().GetMate();
}

std::size_t MM::Reflection::Constructor::GetArgumentNumber() const { if (!IsValid()) {
    return 0;
  }
  return constructor_wrapper_.lock()->GetArgumentNumber();
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke() const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke();
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke(
    MM::Reflection::Variable& arg1) const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke(arg1);
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke(MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2) const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke(arg1, arg2);
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke(MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2, MM::Reflection::Variable& arg3) const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke(arg1, arg2, arg3);
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke(MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2, MM::Reflection::Variable& arg3, MM::Reflection
  ::Variable& arg4) const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke(arg1, arg2, arg3, arg4);
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke(MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2, MM::Reflection::Variable& arg3, MM::Reflection
  ::Variable& arg4, MM::Reflection::Variable& arg5) const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke(arg1, arg2, arg3, arg4, arg5);
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke(MM::Reflection::Variable& arg1,
  MM::Reflection::Variable& arg2, MM::Reflection::Variable& arg3, MM::Reflection
  ::Variable& arg4, MM::Reflection::Variable& arg5,
  MM::Reflection::Variable& arg6) const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke(arg1, arg2, arg3, arg4, arg5, arg6);
}

MM::Reflection::Variable MM::Reflection::Constructor::Invoke(
    std::vector<MM::Reflection::Variable>& args) const {
  if (!IsValid()) {
    return Variable{};
  }
  return constructor_wrapper_.lock()->Invoke(args);
}

MM::Reflection::Constructor::Constructor(
    const std::shared_ptr<ConstructorWrapperBase>& other): constructor_wrapper_(other) {}

MM::Reflection::Constructor MM::Reflection::Constructor::CreateConstructor(
    const std::shared_ptr<ConstructorWrapperBase>& other) {
  return Constructor(other);
}
