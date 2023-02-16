#pragma once

#include <vcruntime_typeinfo.h>

#include <memory>
#include <string>

#include "runtime/core/reflection/database.h"
#include "runtime/core/reflection/utils.h"

namespace MM {
namespace Reflection {
class Meta;
class Type;
class Variable;

/**
 * \brief Get the original type of \ref TypeName (no reference, const, pointer,
 * etc.); \tparam TypeName You want to get the type of its original type.
 */
template <typename TypeName>
struct GetOriginalType {
  using Type = std::remove_const_t<std::remove_reference_t<
      std::remove_pointer_t<std::remove_all_extents<TypeName>>>>;
};

template <typename TypeName>
using GetOriginalTypeT = typename GetOriginalType<TypeName>::Type;

class TypeWrapperBase {
 public:
  virtual ~TypeWrapperBase() = default;

 public:
  /**
   * \brief Determine whether the original type of \ref TypeName is registered.
   * \return Returns true if the original type is registered, otherwise returns
   * false.
   */
  virtual bool IsRegistered() const = 0;

  /**
   * \brief Determine whether type is "const".
   * \return If the type is "const", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsConst() const = 0;

  /**
   * \brief Determine whether type is "reference".
   * \return If the type is "reference", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsReference() const = 0;

  /**
   * \brief Determine whether type is "array".
   * \return If the type is "array", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsArray() const = 0;

  /**
   * \brief Determine whether type is "pointer".
   * \return If the type is "pointer", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsPointer() const = 0;

  /**
   * \brief Determine whether this property is an enumeration.
   * \return If this property is an enumeration, it returns true, otherwise it
   * returns false.
   */
  virtual bool IsEnum() const = 0;

  /**
   * \brief Determine whether the type has a default constructor.
   * \return If the type contains a default constructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveDefaultConstructor() const = 0;

  /**
   * \brief Determine whether the type has a destructor.
   * \return If the type contains a destructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveDestructor() const = 0;

  /**
   * \brief Determine whether the type has a copy constructor.
   * \return If the type contains a copy constructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveCopyConstructor() const = 0;

  /**
   * \brief Determine whether the type has a move constructor.
   * \return If the type contains a move constructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveMoveConstructor() const = 0;

  /**
   * \brief Determine whether the type has a copy assign.
   * \return If the type contains a copy assign, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveCopyAssign() const = 0;

  /**
   * \brief Determine whether the type has a move assign.
   * \return If the type contains a move assign, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveMoveAssign() const = 0;

  /**
   * \brief Get the size of type.
   * \return The size of type.
   */
  virtual std::size_t GetSize() const = 0;

  /**
   * \brief Get type hash code.
   * \return The type hash code.
   */
  virtual std::size_t GetTypeHashCode() const = 0;

  /**
   * \brief Get original type hash code.
   * \return The original type hash code.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   */
  virtual std::size_t GetOriginalTypeHashCode() const = 0;

  /**
   * \brief Get type Name.
   * \return The type name.
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  virtual std::string GetTypeName() const = 0;

  /**
   * \brief Get original type Name.
   * \return The original type name.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  virtual std::string GetOriginalTypeName() const = 0;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  virtual std::weak_ptr<Meta> GetMeta() const = 0;
};

template <typename TypeName>
class TypeWrapper final : public TypeWrapperBase {
 public:
  using Type = TypeName;
  using OriginalType = GetOriginalTypeT<TypeName>;

 public:
  TypeWrapper() = default;
  ~TypeWrapper() override = default;
  TypeWrapper(const TypeWrapper& other) = default;
  TypeWrapper& operator=(const TypeWrapper& other) = default;

 public:
  /**
   * \brief Determine whether the original type of \ref TypeName is registered.
   * \return Returns true if the original type is registered, otherwise returns
   * false.
   */
  bool IsRegistered() const override;

  /**
   * \brief Determine whether type is "const".
   * \return If the type is "const", it returns true; otherwise, it returns
   * false.
   */
  bool IsConst() const override;

  /**
   * \brief Determine whether type is "reference".
   * \return If the type is "reference", it returns true; otherwise, it returns
   * false.
   */
  bool IsReference() const override;

  /**
   * \brief Determine whether type is "array".
   * \return If the type is "array", it returns true; otherwise, it returns
   * false.
   */
  bool IsArray() const override;

  /**
   * \brief Determine whether type is "pointer".
   * \return If the type is "pointer", it returns true; otherwise, it returns
   * false.
   */
  bool IsPointer() const override;

  /**
   * \brief Determine whether this property is an enumeration.
   * \return If this property is an enumeration, it returns true, otherwise it
   * returns false.
   */
  bool IsEnum() const override;

  /**
   * \brief Determine whether the type has a default constructor.
   * \return If the type contains a default constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveDefaultConstructor() const override;

  /**
   * \brief Determine whether the type has a destructor.
   * \return If the type contains a destructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveDestructor() const override;

  /**
   * \brief Determine whether the type has a copy constructor.
   * \return If the type contains a copy constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveCopyConstructor() const override;

  /**
   * \brief Determine whether the type has a move constructor.
   * \return If the type contains a move constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveMoveConstructor() const override;

  /**
   * \brief Determine whether the type has a copy assign.
   * \return If the type contains a copy assign, it returns true,
   * otherwise it returns false.
   */
  bool HaveCopyAssign() const override;

  /**
   * \brief Determine whether the type has a move assign.
   * \return If the type contains a move assign, it returns true,
   * otherwise it returns false.
   */
  bool HaveMoveAssign() const override;

  // TODO Add some instantiation.(int，float，double，std::string,vec3,
  // vec4,mat3,mat4,etc.)
  /**
   * \brief Determine whether \ref OtherType can be converted to \ref TypeName.
   * \tparam OtherType Other types that determine whether they can be converted
   * with this type. \param other Other types object that determine whether they
   * can be converted with this type. \return If \ref OtherType can be converted
   * to \ref TypeName, return true, otherwise return false.
   */
  template <typename OtherType>
  bool Convertible(OtherType&& other) const;


  /**
   * \brief Get size of \ref TypeName.
   * \return The size of \ref TypeName.
   */
  std::size_t GetSize() const override;

  /**
   * \brief Get type hash code.
   * \return The \ref TypeName hash code.
   */
  std::size_t GetTypeHashCode() const override;

  /**
   * \brief Get the original type of \ref TypeName hash code.
   * \return The original type of \ref TypeName hash code.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   */
  std::size_t GetOriginalTypeHashCode() const override;

  /**
   * \brief Get type Name.
   * \return The \ref TypeName type name.
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  std::string GetTypeName() const override;
  /**
   * \brief Get original type Name of \ref TypeName.
   * \return The original type name of \ref TypeName.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  std::string GetOriginalTypeName() const override;

  /**
   * \brief Get original type meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the unique_ptr containing nullptr
   * will be returned.
   */
  std::weak_ptr<Meta> GetMeta() const override;
};

template <typename TypeName>
Type CreateType(TypeName&& object);

template <typename TypeName>
Type CreateType();

class Type {
  template <typename VariableType, bool IsPropertyVariable_>
  friend class VariableWrapper;

 public:
  /**
   * \brief Get MM::Reflection::Type object of \ref object.
   * \tparam TypeName The type of \ref object.
   * \param object The object that you want get the  MM::Reflection::Type.
   * \return The MM::Reflection::Type of \ref object
   */
  template <typename TypeName>
  static Type CreateType(TypeName&& object) {
    Type result{};
    result.type_wrapper_ = std::make_shared<TypeWrapperBase<TypeName>>();
    return result;
  }

 public:
  Type();
  ~Type();
  Type(const Type& other);
  Type(Type&& other) noexcept;
  explicit Type(const std::shared_ptr<TypeWrapperBase>& other);
  Type& operator=(const Type& other);
  Type& operator=(Type&& other) noexcept;

 public:
  /**
   * \brief Judge whether the types are equal.
   * \param other Object to be judged.
   * \return Returns true if the two object types are the same, otherwise
   * returns false.
   * \remark Both objects must be valid to return true.
   */
  bool operator==(const Type& other) const;

  /**
   * \brief bool conversion.
   */
  operator bool() const;

  /**
   * \brief Judge whether the object is a valid object.
   * \return Returns true if the object is a valid object, otherwise returns
   * false.
   */
  bool IsValid() const;

  /**
   * \brief Judge whether the types are equal.
   * \param other Object to be judged.
   * \return Returns true if the two object types are the same, otherwise
   * returns false.
   * \remark Both objects must be valid to return true.
   */
  bool IsEqual(const Type& other) const;

  /**
   * \brief Determine whether type is "const".
   * \return If the type is "const", it returns true; otherwise, it returns
   * false.
   */
  bool IsConst() const;

  /**
   * \brief Determine whether type is "reference".
   * \return If the type is "reference", it returns true; otherwise, it returns
   * false.
   */
  bool IsReference() const;

  /**
   * \brief Determine whether type is "array".
   * \return If the type is "array", it returns true; otherwise, it returns
   * false.
   */
  bool IsArray() const;

  /**
   * \brief Determine whether type is "pointer".
   * \return If the type is "pointer", it returns true; otherwise, it returns
   * false.
   */
  bool IsPointer() const;

  /**
   * \brief Determine whether this property is an enumeration.
   * \return If this property is an enumeration, it returns true, otherwise it
   * returns false.
   */
  bool IsEnum() const;

  /**
   * \brief Determine whether the type has a default constructor.
   * \return If the type contains a default constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveDefaultConstructor() const;

  /**
   * \brief Determine whether the type has a destructor.
   * \return If the type contains a destructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveDestructor() const;

  /**
   * \brief Determine whether the type has a copy constructor.
   * \return If the type contains a copy constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveCopyConstructor() const;

  /**
   * \brief Determine whether the type has a move constructor.
   * \return If the type contains a move constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveMoveConstructor() const;

  /**
   * \brief Determine whether the type has a copy assign.
   * \return If the type contains a copy assign, it returns true,
   * otherwise it returns false.
   */
  bool HaveCopyAssign() const;

  /**
   * \brief Determine whether the type has a move assign.
   * \return If the type contains a move assign, it returns true,
   * otherwise it returns false.
   */
  bool HaveMoveAssign() const;

  /**
   * \brief Judge whether the original types are equal.
   * \param other Object to be judged.
   * \return Returns true if the two original object types are the same,
   * otherwise returns false.
   * \remark Both objects must be valid to return true.
   */
  bool OriginalTypeIsEqual(const Type& other) const;

  /**
   * \brief Swap two object.
   * \param other Objects to be exchanged.
   */
  void Swap(Type& other);

  /**
   * \brief Get the size of \ref TypeName
   * \return The size of \ref TypeName.
   */
  std::size_t GetSize() const;

  /**
   * \brief Get type hash code.
   * \return The type hash code.
   * \remark If object is not valid, it will return 0.
   */
  std::size_t GetTypeHashCode() const;

  /**
   * \brief Get original type hash code.
   * \return The original type hash code.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   * \remark If object is not valid, it will return 0.
   */
  std::size_t GetOriginalTypeHashCode() const;

  /**
   * \brief Get type Name.
   * \return The type name.
   * \remark If the type is not registered or object is not valid, the default
   * empty std::string will be returned.
   */
  std::string GetTypeName() const;

  /**
   * \brief Get original type Name.
   * \return The original type name.
   * \remark If the type is not registered or object is not valid, the default
   * empty std::string will be returned.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   */
  std::string GetOriginalTypeName() const;

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered or this object is not valid, the
   * unique_ptr containing nullptr will be returned.
   */
  std::weak_ptr<Meta> GetMate() const;

 private:
  std::shared_ptr<TypeWrapperBase> type_wrapper_ = nullptr;
};

template <typename TypeName>
bool TypeWrapper<TypeName>::IsRegistered() const {
  return g_type_database.find(GetTypeHashCode()) != g_type_database.end();
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsConst() const {
  return std::is_const<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsReference() const {
  return std::is_reference<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsArray() const {
  return std::is_array<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsPointer() const {
  return std::is_pointer<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsEnum() const {
  return std::is_enum<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveDefaultConstructor() const {
  return std::is_default_constructible<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveDestructor() const {
  return std::is_destructible<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveCopyConstructor() const {
  return std::is_copy_constructible<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveMoveConstructor() const {
  return std::is_move_constructible<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveCopyAssign() const {
  return std::is_copy_assignable<TypeName>::value;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveMoveAssign() const {
  return std::is_move_assignable<TypeName>::value;
}

template <typename TypeName>
template <typename OtherType>
bool TypeWrapper<TypeName>::Convertible(OtherType&& other) const {
  return Conversion<typename GetOriginalType<OtherType>::Type,
                    typename GetOriginalType<TypeName>::Type>::value;
}

template <typename TypeName>
std::size_t TypeWrapper<TypeName>::GetSize() const {
  return sizeof(TypeName);
}

template <typename TypeName>
std::size_t TypeWrapper<TypeName>::GetTypeHashCode() const {
  return typeid(TypeName).hash_code();
}

template <typename TypeName>
std::size_t TypeWrapper<TypeName>::GetOriginalTypeHashCode() const {
  return typeid(OriginalType).hash_code();
}

template <typename TypeName>
std::string TypeWrapper<TypeName>::GetTypeName() const {
  if (!IsRegistered()) {
    return std::string{};
  }
  std::string result{};
  if (std::is_pointer<TypeName>::value) {
    if (std::is_const<std::remove_pointer_t<TypeName>>::type) {
      result += "const ";
    }
    result += GetOriginalTypeName() + "*";
    if (std::is_const<TypeName>::value) {
      result += " const";
    }
  } else if (std::is_array<TypeName>::value) {
    if (std::is_const<TypeName>::value) {
      result += "const ";
    }
    result += GetOriginalTypeName() + "[]";
  } else {
    if (std::is_const<std::remove_reference_t<TypeName>>::value) {
      result += "const ";
    }
    if (std::is_lvalue_reference<TypeName>::value) {
      result += GetOriginalTypeName() + "&";
    }
    if (std::is_rvalue_reference<TypeName>::value) {
      result += GetOriginalTypeName() + "&&";
    }
  }
  return result;
}

template <typename TypeName>
std::string TypeWrapper<TypeName>::GetOriginalTypeName() const {
  if (!IsRegistered()) {
    return std::string{};
  }
  return g_type_database[GetOriginalTypeHashCode()];
}

template <typename TypeName>
std::weak_ptr<Meta> TypeWrapper<TypeName>::GetMeta() const {
  if (!IsRegistered()) {
    return std::make_shared<Meta>(
        g_meta_database[std::string{}]);
  }
  return std::make_shared<Meta>(g_meta_database[GetOriginalTypeName()]);
} 

template <typename TypeName>
Type CreateType(TypeName&& object) {
  return Type(std::make_shared<TypeWrapperBase>(TypeWrapper<TypeName>{}));
}

template <typename TypeName>
Type CreateType() {
  return Type(std::make_shared<TypeWrapperBase>(TypeWrapper<TypeName>{}));
}
}  // namespace Reflection
}  // namespace MM
