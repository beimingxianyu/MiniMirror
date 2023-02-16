#include "runtime/core/reflection/destructor.h"

bool MM::Reflection::Destructor::IsValid() const {
  return destructor_wrapper_.expired();
}

MM::Reflection::Type MM::Reflection::Destructor::GetType() const { if (!IsValid()) {
    return Type{};
  }
  return destructor_wrapper_.lock()->GetType();
}

std::weak_ptr<MM::Reflection::Meta> MM::Reflection::Destructor::
GetMeta() const {
  if (!IsValid()) {
    return std::weak_ptr<MM::Reflection::Meta>{g_meta_database[std::string{}]};
  }
  return destructor_wrapper_.lock()->GetMeta();
}

bool MM::Reflection::Destructor::Destroy(Variable& object) const { if (!IsValid()) {
    return false;
  }
  return destructor_wrapper_.lock()->Destroy(object);
}

MM::Reflection::Destructor MM::Reflection::Destructor::CreateDestructor(
    const std::shared_ptr<DestructorWrapperBase>& other) {
  return Destructor(other);
}
