#include "runtime/core/reflection/type.h"

#include "meta.h"
#include "meta.h"

MM::Reflection::Type::Type() : type_wrapper_{nullptr} {}

MM::Reflection::Type::~Type() = default;

MM::Reflection::Type::Type(const Type& other) = default;

MM::Reflection::Type::Type(Type&& other) noexcept {
  std::swap(type_wrapper_, other.type_wrapper_);
  other.type_wrapper_.reset();
}

MM::Reflection::Type::Type(const std::shared_ptr<TypeWrapperBase>& other) {
  type_wrapper_ = other;
}

MM::Reflection::Type& MM::Reflection::Type::operator=(const Type& other) = default;

MM::Reflection::Type& MM::Reflection::Type::operator=(Type&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  std::swap(type_wrapper_, other.type_wrapper_);
  other.type_wrapper_.reset();
  return *this;
}

bool MM::Reflection::Type::operator==(const Type& other) const { return IsEqual(other); }

MM::Reflection::Type::operator bool() const { return type_wrapper_ != nullptr; }

bool MM::Reflection::Type::IsValid() const { return type_wrapper_ != nullptr; }

bool MM::Reflection::Type::IsEqual(const Type& other) const {
  if (IsValid() && other.IsValid()) {
    return GetTypeHashCode() == other.GetTypeHashCode();
  }
  return false;
}

bool MM::Reflection::Type::IsConst() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->IsConst();
}

bool MM::Reflection::Type::IsReference() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->IsReference();
}

bool MM::Reflection::Type::IsArray() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->IsArray();
}

bool MM::Reflection::Type::IsPointer() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->IsPointer();
}

bool MM::Reflection::Type::IsEnum() const { return type_wrapper_->IsEnum(); }

bool MM::Reflection::Type::HaveDefaultConstructor() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->HaveDefaultConstructor();
}

bool MM::Reflection::Type::HaveDestructor() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->HaveDestructor();
}

bool MM::Reflection::Type::HaveCopyConstructor() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->HaveCopyConstructor();
}

bool MM::Reflection::Type::HaveMoveConstructor() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->HaveMoveConstructor();
}

bool MM::Reflection::Type::HaveCopyAssign() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->HaveCopyAssign();
}

bool MM::Reflection::Type::HaveMoveAssign() const {
  if (!IsValid()) {
    return false;
  }
  return type_wrapper_->HaveMoveAssign();
}

auto MM::Reflection::Type::OriginalTypeIsEqual(
    const Type& other) const -> bool {
  if (IsValid() && other.IsValid()) {
    return GetOriginalTypeHashCode() == other.GetOriginalTypeHashCode();
  }
  return false;
}

void MM::Reflection::Type::Swap(MM::Reflection::Type& other) {
  if (this != &other) {
    std::swap(type_wrapper_, other.type_wrapper_);
  }
}

std::size_t MM::Reflection::Type::GetSize() const {
  if (!IsValid()) {
    return 0;
  }
  return type_wrapper_->GetSize();
}

std::size_t MM::Reflection::Type::GetTypeHashCode() const {
  if (!IsValid()) {
    return 0;
  }
  return type_wrapper_->GetTypeHashCode();
}

std::size_t MM::Reflection::Type::GetOriginalTypeHashCode() const {
  if (!IsValid()) {
    return 0;
  }
  return type_wrapper_->GetOriginalTypeHashCode();
}

std::string MM::Reflection::Type::GetTypeName() const {
  if (!IsValid()) {
    return std::string();
  }
  return type_wrapper_->GetTypeName();
}

std::string MM::Reflection::Type::GetOriginalTypeName() const {
  if (!IsValid()) {
    return std::string();
  }
  return type_wrapper_->GetOriginalTypeName();
}

std::weak_ptr<MM::Reflection::Meta> MM::Reflection::Type::GetMate() const {
  if (!IsValid()) {
    return std::weak_ptr<MM::Reflection::Meta>{};
  }
  return type_wrapper_->GetMeta();
}
