#pragma once

#include <memory>

#include "runtime/platform/base/MMObject.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace Manager {

using ManagedObjectID = MM::Utils::GUID;

class ManagedObjectBase : public MMObject {
 public:
  ManagedObjectBase();
  virtual ~ManagedObjectBase() = default;
  explicit ManagedObjectBase(const std::string& object_name);
  ManagedObjectBase(const ManagedObjectBase& other) = default;
  ManagedObjectBase(ManagedObjectBase&& other) noexcept = default;
  ManagedObjectBase& operator=(const ManagedObjectBase& other);
  ManagedObjectBase& operator=(ManagedObjectBase&& other) noexcept;

 public:
  const std::string& GetObjectName() const;

  ManagedObjectID GetObjectID() const;

  bool LowLevelEqual(const ManagedObjectBase& other) const;

  friend void Swap(ManagedObjectBase& lhs, ManagedObjectBase& rhs) noexcept;

  friend void swap(ManagedObjectBase& lhs, ManagedObjectBase& rhs) noexcept;

  //  virtual ExecuteResult GetLightCopy(
  //      const std::string& new_object_name,
  //      std::unique_ptr<ManagedObjectBase>& output_new_light_copy_object)
  //      const;
  //
  //  virtual ExecuteResult GetDeepCopy(
  //      const std::string& new_object_name,
  //      std::unique_ptr<ManagedObjectBase>& output_new_deep_copy_object)
  //      const;

 protected:
  void Reset() override;

 private:
  std::string object_name_{};
};

}  // namespace Manager
}  // namespace MM
