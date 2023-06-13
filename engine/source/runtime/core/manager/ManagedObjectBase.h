#pragma once

#include <memory>

#include "runtime/platform/base/MMObject.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace Manager {

using ManagedObjectID = MM::Utils::GUID;

class ManagedObjectBase : virtual public MMObject {
 public:
  ManagedObjectBase() = delete;
  virtual ~ManagedObjectBase() = default;
  explicit ManagedObjectBase(const std::string& object_name);
  ManagedObjectBase(const ManagedObjectBase& other) = default;
  ManagedObjectBase(ManagedObjectBase&& other) noexcept = default;
  ManagedObjectBase& operator=(const ManagedObjectBase& other);
  ManagedObjectBase& operator=(ManagedObjectBase&& other) noexcept;

 public:
  const std::string& GetObjectName() const;

  ManagedObjectID GetObjectID() const;

  virtual std::size_t GetObjectSize() const;

  //  virtual ExecuteResult GetLightCopy(
  //      const std::string& new_object_name,
  //      std::unique_ptr<ManagedObjectBase>& output_new_light_copy_object)
  //      const;
  //
  //  virtual ExecuteResult GetDeepCopy(
  //      const std::string& new_object_name,
  //      std::unique_ptr<ManagedObjectBase>& output_new_deep_copy_object)
  //      const;

 private:
  std::string object_name_{};
};

}  // namespace Manager
}  // namespace MM
