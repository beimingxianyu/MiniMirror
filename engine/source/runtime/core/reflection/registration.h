#pragma once

#include "runtime/core/reflection/meta.h"
#include "utils/marco.h"

// TODO Add a default meta.

// TODO Add some mete(int, double, float,etc.)

namespace MM {
namespace Reflection {
#define MM_REGISTER_IMP(count)                                \
  static void Utils::MM_CAT(MMAutoRegisterFunction, count)(); \
  struct Utils::MM_CAT(MMAutoRegisterStruct, count) {         \
    Utils::MM_CAT(MMAutoRegisterStruct, count)() {            \
      Utils::MM_CAT(MMAutoRegisterFunction, count)();         \
    }                                                         \
  } static const Utils::MM_CAT(MMAutoRegisterStruct, count)   \
      Utils::MM_CAT(MM_auto_register_variable, count);        \
  static void Utils::MM_CAT(MMAutoRegisterFunction, count)()

#define MM_REGISTER MM_REGISTER_IMP(__COUNTER__)

template <typename ClassType_>
class Class {
public:
  Class() = delete;
  Class(const Class& other) = delete;
  Class(Class&& other) noexcept = delete;
  Class& operator=(const Class& other) = delete;
  Class& operator=(Class& other) noexcept = delete;

public:
  Class(const std::string& type_name);

  template<typename F_>
  Class& Method(const std::string& method_name, F_ method_ptr);

  template<typename Property_>
  Class& Property(const std::string& property_name, Property_ ClassType_::* property_ptr);

  template<typename Destructor_>
  Class& Destuctor();

  template<typename ...Args>
  Class& Constructor();
};

}  // namespace Reflection
}  // namespace MM
