#include "runtime/core/reflection/variable.h"

#include "meta.h"

bool MM::Reflection::Variable::IsPropertyVariable() const {
  return variable_wrapper_->IsPropertyVariable();
}

MM::Reflection::Variable::Variable() { variable_wrapper_ = nullptr; }

MM::Reflection::Variable::~Variable() { Destroy(); };

MM::Reflection::Variable::Variable(
    const Variable& other) {
  // Judge whether the held value can be copied and constructed.
  if (other.GetType().HaveCopyConstructor()) {
    // Object is valid before its function can be called.
    if (other.IsValid()) {
      variable_wrapper_ = other.variable_wrapper_->CopyToBasePointer();
    } else {
      variable_wrapper_ = nullptr;
    }
  }
}

MM::Reflection::Variable::Variable(Variable& other) {
  // Judge whether the held value can be copied and constructed.
  if (other.GetType().HaveCopyConstructor()) {
    // Object is valid before its function can be called.
    if (other.IsValid()) {
      variable_wrapper_ = other.variable_wrapper_->CopyToBasePointer();
    } else {
      variable_wrapper_ = nullptr;
    }
  }
}

MM::Reflection::Variable::Variable(
    Variable&& other) noexcept {
  if (other.GetType().HaveMoveConstructor()) {
    std::swap(variable_wrapper_, other.variable_wrapper_);
    other.variable_wrapper_.reset();
  }
}

MM::Reflection::Variable& MM::Reflection::Variable::operator=(
    const Variable& other) {
  if (this == &other) {
    return *this;
  }
  if (other.GetType().HaveCopyAssign()) {
    if (other.IsValid()) {
      variable_wrapper_ = other.variable_wrapper_->CopyToBasePointer();
    } else {
      variable_wrapper_ = nullptr;
    }
  }
  return *this;
}

MM::Reflection::Variable& MM::Reflection::Variable::operator=(
    Variable&& other) noexcept {
  if (other.GetType().HaveMoveAssign()) {
    std::swap(variable_wrapper_, other.variable_wrapper_);
    other.variable_wrapper_.reset();
  }
  return *this;
}

MM::Reflection::Variable::Variable(
    std::unique_ptr<VariableWrapperBase>&& variable_wrapper) : variable_wrapper_(variable_wrapper.release()){}

MM::Reflection::Variable::operator bool() const { return IsValid();}

MM::Reflection::Variable::operator void*() const { return GetValue(); }

bool MM::Reflection::Variable::IsValid() const {
  return variable_wrapper_ != nullptr;
}

void* MM::Reflection::Variable::GetValue() const {
  if (!IsValid()) {
    return nullptr;
  }
  return variable_wrapper_->GetValue();
}

bool MM::Reflection::Variable::SetValue(void* other) {
  if (!IsValid() || (other == nullptr)) {
    return false;
  }
  return variable_wrapper_->SetValue(other);
}

MM::Reflection::Type MM::Reflection::Variable::GetType() const {
  if (!IsValid()) {
    return MM::Reflection::Type{};
  }
  return variable_wrapper_->GetType();
}

std::weak_ptr<MM::Reflection::Meta> MM::Reflection::Variable::
GetMeta() const {
  if (!IsValid()) {
    return std::weak_ptr<MM::Reflection::Meta>{g_meta_database[std::string{}]};
  }
  return variable_wrapper_->GetMeta();
}

MM::Reflection::Variable MM::Reflection::Variable::GetPropertyVariable(
    const std::string& property_name) const {
  const auto metadata = GetMeta().lock();
    if (metadata->IsValid()) {
      const Property target_property = metadata->GetProperty(property_name);
      if (target_property.IsValid()) {
        return target_property.GetPropertyVariable(*this);
      }
    }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this, arg1);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1, Variable& arg2) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this, arg1, arg2);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1, Variable& arg2,
    Variable& arg3) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this, arg1, arg2, arg3);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1, Variable& arg2,
    Variable& arg3, Variable& arg4) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this, arg1, arg2, arg3, arg4);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1, Variable& arg2,
    Variable& arg3, Variable& arg4, Variable& arg5) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this, arg1, arg2, arg3, arg4, arg5);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1, Variable& arg2,
    Variable& arg3, Variable& arg4, Variable& arg5, Variable& arg6) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this, arg1, arg2, arg3, arg4, arg5, arg6);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, std::vector<Variable>& args) {
  if (IsValid()) {
      const auto metadata = GetMeta().lock();
      if (metadata->IsValid()) {
        const Method method{metadata->GetMethod(method_name)};
        if (method.IsValid()) {
          return method.Invoke(*this, args);
        }
        return Variable{};
      }
  }
  return Variable{};
}

void* MM::Reflection::Variable::Release() {
  return variable_wrapper_.release();
}

bool MM::Reflection::Variable::Destroy() {
  const auto metadata = GetMeta().lock();
    if (IsPropertyVariable()) {
      variable_wrapper_.release();
      return true;
    }
    if (metadata->IsValid()) {
      const Destructor destructor = metadata->GetDestructor();
        destructor.Destroy(*this);
        variable_wrapper_.reset();
        return true;
    }
  return false;
}
