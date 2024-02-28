#include "runtime/core/reflection/variable.h"
#include <memory>

#include "meta.h"
#include "runtime/core/reflection/type.h"

MM::Reflection::Variable::Variable() { variable_wrapper_ = nullptr; }

MM::Reflection::Variable::~Variable() { Destroy(); };

MM::Reflection::Variable::Variable(
    const Variable& other) {
  if (!other.IsValid()) {
    return;
  }

  variable_wrapper_ = other.variable_wrapper_->CopyToBasePointer();
}

MM::Reflection::Variable::Variable(
    Variable&& other) noexcept {
  if (!other.IsValid()) {
    return;
  }

  variable_wrapper_ = other.variable_wrapper_->MoveToBasePointer();
}

MM::Reflection::Variable& MM::Reflection::Variable::operator=(
    const Variable& other) {
  if (this == std::addressof(other)) {
    return *this;
  }
  if (!IsValid() || !other.IsValid()) {
    return *this;
  }
  variable_wrapper_->MoveValue(other.variable_wrapper_->GetValue());
  return *this;
}

MM::Reflection::Variable& MM::Reflection::Variable::operator=(
    Variable&& other) noexcept {
  if (this == std::addressof(other)) {
    return *this;
  }
  if (!IsValid() || !other.IsValid()) {
    return *this;
  }
  variable_wrapper_->MoveValue(other.variable_wrapper_->GetValue());
  return *this;
}

MM::Reflection::Variable::Variable(
    std::unique_ptr<VariableWrapperBase>&& variable_wrapper) : variable_wrapper_(variable_wrapper.release()){}

MM::Reflection::Variable::operator bool() const { return IsValid();}

bool MM::Reflection::Variable::IsValid() const {
  return variable_wrapper_ != nullptr && variable_wrapper_->IsValid();
}

const void* MM::Reflection::Variable::GetValue() const {
  return variable_wrapper_->GetValue();
}

void* MM::Reflection::Variable::GetValue() {
  return variable_wrapper_->GetValue();
}

bool MM::Reflection::Variable::SetValue(const void* other) {
  if (!IsValid()) {
    return false;
  }
  return variable_wrapper_->SetValue(other);
}

bool MM::Reflection::Variable::CopyValue(const void* other) {
  if (!IsValid()) {
    return false;
  }
  return variable_wrapper_->CopyValue(other);
}

bool MM::Reflection::Variable::MoveValue(void* other) {
  if (!IsValid()) {
    return false;
  }
  return variable_wrapper_->MoveValue(other);
}

const MM::Reflection::Type* MM::Reflection::Variable::GetType() const {
  static MM::Reflection::Type EmptyType{};
  if (!IsValid()) {
    return &EmptyType;
  }
  return variable_wrapper_->GetType();
}

const MM::Reflection::Meta* MM::Reflection::Variable::
GetMeta() const {
  if (!IsValid()) {
    return nullptr;
  }
  return variable_wrapper_->GetMeta();
}

MM::Reflection::Variable MM::Reflection::Variable::GetPropertyVariable(
    const std::string& property_name) {
  const Meta* metadata = GetMeta();
    if (metadata != nullptr) {
      assert(metadata->IsValid());
      const Property* target_property = metadata->GetProperty(property_name);
      if (target_property != nullptr) {
        assert(target_property->IsValid());
        return target_property->GetPropertyVariable(*this);
      }
    }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::GetPropertyVariable(
    const std::string& property_name) const {
  const Meta* metadata = GetMeta();
    if (metadata != nullptr) {
      assert(metadata->IsValid());
      const Property* target_property = metadata->GetProperty(property_name);
      if (target_property != nullptr) {
        assert(target_property->IsValid());
        return target_property->GetPropertyVariable(*this);
      }
    }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name) {
  if (IsValid()) {
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1) {
  if (IsValid()) {
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this, arg1);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, Variable& arg1, Variable& arg2) {
  if (IsValid()) {
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this, arg1, arg2);
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
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this, arg1, arg2, arg3);
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
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this, arg1, arg2, arg3, arg4);
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
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this, arg1, arg2, arg3, arg4, arg5);
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
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this, arg1, arg2, arg3, arg4, arg5, arg6);
        }
        return Variable{};
      }
  }
  return Variable{};
}

MM::Reflection::Variable MM::Reflection::Variable::Invoke(
    const std::string& method_name, std::vector<Variable>& args) {
  if (IsValid()) {
      const Meta* metadata = GetMeta();
      if (metadata != nullptr) {
        assert(metadata->IsValid());
        const Method* method{metadata->GetMethod(method_name)};
        if (method != nullptr) {
          assert(method->IsValid());
          return method->Invoke(*this, args);
        }
        return Variable{};
      }
  }
  return Variable{};
}

void* MM::Reflection::Variable::ReleaseOwnership() {
  return variable_wrapper_->ReleaseOwnership();
}

void MM::Reflection::Variable::Destroy() {
  variable_wrapper_.reset();
}
