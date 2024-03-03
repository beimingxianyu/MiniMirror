#pragma once

#include <memory>
#include <string>
#include <type_traits>

#include "runtime/core/reflection/database.h"
#include "utils/type_utils.h"

namespace MM {
namespace Reflection {
class Type;
class Variable;


class TypeWrapperBase {
 public:
  virtual ~TypeWrapperBase() = default;

 public:
  virtual bool IsVoid() const {return false;}

  /**
   * \brief Determine whether the original type of \ref TypeName is registered.
   * \return Returns true if the original type is registered, otherwise returns
   * false.
   */
  virtual bool IsRegistered() const {return false;}

  /**
   * \brief Determine whether type is "const".
   * \return If the type is "const", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsConst() const {return false;}

  /**
   * \brief Determine whether type is "reference".
   * \return If the type is "reference", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsReference() const {return false;}

  /**
   * \brief Determine whether type is "array".
   * \return If the type is "array", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsArray() const {return false;}

  /**
   * \brief Determine whether type is "pointer".
   * \return If the type is "pointer", it returns true; otherwise, it returns
   * false.
   */
  virtual bool IsPointer() const {return false;}

  /**
   * \brief Determine whether this property is an enumeration.
   * \return If this property is an enumeration, it returns true, otherwise it
   * returns false.
   */
  virtual bool IsEnum() const {return false;}

  /**
   * \brief Determine whether the type has a default constructor.
   * \return If the type contains a default constructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveDefaultConstructor() const {return false;}

  /**
   * \brief Determine whether the type has a destructor.
   * \return If the type contains a destructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveDestructor() const {return false;}

  /**
   * \brief Determine whether the type has a copy constructor.
   * \return If the type contains a copy constructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveCopyConstructor() const {return false;}

  /**
   * \brief Determine whether the type has a move constructor.
   * \return If the type contains a move constructor, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveMoveConstructor() const {return false;}

  /**
   * \brief Determine whether the type has a copy assign.
   * \return If the type contains a copy assign, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveCopyAssign() const {return false;}

  /**
   * \brief Determine whether the type has a move assign.
   * \return If the type contains a move assign, it returns true,
   * otherwise it returns false.
   */
  virtual bool HaveMoveAssign() const {return false;}

  /**
   * \brief Get the size of type.
   * \return The size of type.
   */
  virtual std::size_t GetSize() const {return 0;}

  /**
   * \brief Get type hash code.
   * \return The type hash code.
   */
  virtual std::size_t GetTypeHashCode() const {return 0;}

  /**
   * \brief Get original type hash code.
   * \return The original type hash code.
   * \remark A original type is a type without pointers, references, array, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   */
  virtual std::size_t GetOriginalTypeHashCode() const {return 0;}

  /**
   * \brief Get common type hash code.
   * \return The common type hash code.
   * \remark A original type is a type without references and
   * constants. (Example:int*: int*, int&: int, const int&: int, etc.)
   */
  virtual std::size_t GetCommonTypeHashCode() const {return 0;}

  /**
   * \brief Get type Name.
   * \return The type name.
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  virtual std::string GetTypeName() const {return std::string("");}

  /**
   * \brief Get original type Name.
   * \return The original type name.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  virtual const std::string& GetOriginalTypeName() const {static std::string empty_name{}; return empty_name;}

  /**
   * \brief Get meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the nullptr will be returned.
   */
  virtual const Meta* GetMeta() const {return nullptr;}
};

template <typename TypeName>
class TypeWrapper final : public TypeWrapperBase {
 public:
  using Type = TypeName;
  using OriginalType = Utils::GetOriginalTypeT<TypeName>;
  using CommonType = Utils::GetCommonTypeT<TypeName>;

 public:
  TypeWrapper() = default;
  ~TypeWrapper() override = default;
  TypeWrapper(const TypeWrapper& other) = default;
  TypeWrapper& operator=(const TypeWrapper& other) = default;

 public:
  bool IsVoid() const override {return false;}

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
   * \brief Get common type hash code.
   * \return The common type hash code.
   * \remark A original type is a type without references and
   * constants. (Example:int*: int*, int&: int, const int&: int, etc.)
   */
  virtual std::size_t GetCommonTypeHashCode() const override;

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
  const std::string& GetOriginalTypeName() const override;

  /**
   * \brief Get original type meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the nullptr will be returned.
   */
  const Meta* GetMeta() const override;
};

template<>
class TypeWrapper<void> final : public TypeWrapperBase {
public:
 public:
  using Type = void;
  using OriginalType = Utils::GetOriginalTypeT<void>;
  using CommonType = void;

 public:
  TypeWrapper() = default;
  ~TypeWrapper() override = default;
  TypeWrapper(const TypeWrapper& other) = default;
  TypeWrapper& operator=(const TypeWrapper& other) = default;

 public:
  bool IsVoid() const override {return true;}

  /**
   * \brief Determine whether the original type of \ref TypeName is registered.
   * \return Returns true if the original type is registered, otherwise returns
   * false.
   */
  bool IsRegistered() const override {return true;}

  /**
   * \brief Determine whether type is "const".
   * \return If the type is "const", it returns true; otherwise, it returns
   * false.
   */
  bool IsConst() const override {return false;}

  /**
   * \brief Determine whether type is "reference".
   * \return If the type is "reference", it returns true; otherwise, it returns
   * false.
   */
  bool IsReference() const override {return false;}

  /**
   * \brief Determine whether type is "array".
   * \return If the type is "array", it returns true; otherwise, it returns
   * false.
   */
  bool IsArray() const override {return false;}

  /**
   * \brief Determine whether type is "pointer".
   * \return If the type is "pointer", it returns true; otherwise, it returns
   * false.
   */
  bool IsPointer() const override {return false;}

  /**
   * \brief Determine whether this property is an enumeration.
   * \return If this property is an enumeration, it returns true, otherwise it
   * returns false.
   */
  bool IsEnum() const override {return false;}

  /**
   * \brief Determine whether the type has a default constructor.
   * \return If the type contains a default constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveDefaultConstructor() const override {return false;}

  /**
   * \brief Determine whether the type has a destructor.
   * \return If the type contains a destructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveDestructor() const override {return false;}

  /**
   * \brief Determine whether the type has a copy constructor.
   * \return If the type contains a copy constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveCopyConstructor() const override {return false;}

  /**
   * \brief Determine whether the type has a move constructor.
   * \return If the type contains a move constructor, it returns true,
   * otherwise it returns false.
   */
  bool HaveMoveConstructor() const override {return false;}

  /**
   * \brief Determine whether the type has a copy assign.
   * \return If the type contains a copy assign, it returns true,
   * otherwise it returns false.
   */
  bool HaveCopyAssign() const override {return false;}

  /**
   * \brief Determine whether the type has a move assign.
   * \return If the type contains a move assign, it returns true,
   * otherwise it returns false.
   */
  bool HaveMoveAssign() const override {return false;}

  /**
   * \brief Get size of \ref TypeName.
   * \return The size of \ref TypeName.
   */
  std::size_t GetSize() const override {return 0;}

  /**
   * \brief Get type hash code.
   * \return The \ref TypeName hash code.
   */
  std::size_t GetTypeHashCode() const override {return typeid(void).hash_code();}

  /**
   * \brief Get the original type of \ref TypeName hash code.
   * \return The original type of \ref TypeName hash code.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   */
  std::size_t GetOriginalTypeHashCode() const override {return GetTypeHashCode();}

  /**
   * \brief Get common type hash code.
   * \return The common type hash code.
   * \remark A original type is a type without references and
   * constants. (Example:int*: int*, int&: int, const int&: int, etc.)
   */
  virtual std::size_t GetCommonTypeHashCode() const override {return GetTypeHashCode();}

  /**
   * \brief Get type Name.
   * \return The \ref TypeName type name.
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  std::string GetTypeName() const override {return "void";}
  /**
   * \brief Get original type Name of \ref TypeName.
   * \return The original type name of \ref TypeName.
   * \remark A original type is a type without pointers, references, and
   * constants. (Example:int*: int, int&: int, const int&: int, etc.)
   * \remark If the type is not registered, the default empty std::string will
   * be returned.
   */
  const std::string& GetOriginalTypeName() const override { return void_type_name;}

  /**
   * \brief Get original type meta data.
   * \return Returns unique_ptr containing metadata.
   * \remark If the type is not registered, the nullptr will be returned.
   */
  const Meta* GetMeta() const override {
    if (!IsRegistered()) {
      return nullptr;
    }
    return GetMetaDatabase()[GetOriginalTypeHashCode()];
  }

private:
  static std::string void_type_name;
};

template <typename TypeName>
Type CreateType();

class Type {
  template <typename VariableType>
  friend class VariableWrapper;

 public:
  template <typename TypeName>
  static Type CreateType() {
    return Type{std::make_unique<TypeWrapper<TypeName>>()};
  }

 public:
  Type();
  ~Type();
  Type(const Type& other) = delete;
  Type(Type&& other) noexcept = default;
  explicit Type(std::unique_ptr<TypeWrapperBase>&& other) noexcept;
  Type& operator=(const Type& other) = delete;
  Type& operator=(Type&& other) noexcept = default;

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
  explicit operator bool() const;

  bool IsRegistered() const;

  bool IsVoid() const;

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
   * \brief Get common type hash code.
   * \return The common type hash code.
   * \remark A original type is a type without references and
   * constants. (Example:int*: int*, int&: int, const int&: int, etc.)
   */
  std::size_t GetCommonTypeHashCode() const;

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
   * nullptr will be returned.
   */
  const Meta* GetMate() const;

 private:
  std::unique_ptr<TypeWrapperBase> type_wrapper_ = nullptr;
};

template <typename TypeName>
bool TypeWrapper<TypeName>::IsRegistered() const {
  return GetMetaDatabase().find(GetTypeHashCode()) != GetMetaDatabase().end();
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsConst() const {
  return std::is_const_v<std::remove_reference_t<TypeName>>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsReference() const {
  return std::is_reference_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsArray() const {
  return std::is_array_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsPointer() const {
  return std::is_pointer_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::IsEnum() const {
  return std::is_enum_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveDefaultConstructor() const {
  return std::is_default_constructible_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveDestructor() const {
  return std::is_destructible_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveCopyConstructor() const {
  return std::is_copy_constructible_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveMoveConstructor() const {
  return std::is_move_constructible_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveCopyAssign() const {
  return std::is_copy_assignable_v<TypeName>;
}

template <typename TypeName>
bool TypeWrapper<TypeName>::HaveMoveAssign() const {
  return std::is_move_assignable_v<TypeName>;
}

template <typename TypeName>
template <typename OtherType>
bool TypeWrapper<TypeName>::Convertible(OtherType&&) const {
  return Utils::Conversion<typename Utils::GetOriginalType<OtherType>::Type,
                    typename Utils::GetOriginalType<TypeName>::Type>::value;
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
std::size_t TypeWrapper<TypeName>::GetCommonTypeHashCode() const {
  return typeid(CommonType).hash_code();
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
  std::string result{GetOriginalTypeName()};
  if constexpr (std::is_pointer_v<TypeName>) {
    if constexpr (std::is_const_v<std::remove_pointer_t<TypeName>>) {
      result += "const ";
    }
    result += "*";
    if constexpr (std::is_const_v<TypeName>) {
      result += " const";
    }
  } else if constexpr (std::is_array_v<TypeName>) {
    if (std::is_const_v<TypeName>) {
      result += "const ";
    }
    result += "[]";
  } else {
    if constexpr (std::is_const_v<std::remove_reference_t<TypeName>>) {
      result += "const ";
    }
    if constexpr (std::is_lvalue_reference_v<TypeName>) {
      result += "&";
    }
    if constexpr (std::is_rvalue_reference_v<TypeName>) {
      result += "&&";
    }
  }
  return result;
}

template <typename TypeName>
const std::string& TypeWrapper<TypeName>::GetOriginalTypeName() const {
  static const std::string EmptyTypeName{};
  if (!IsRegistered()) {
    return EmptyTypeName;
  }
  return GetMetaDatabase()[GetOriginalTypeHashCode()]->GetTypeName();
}

template <typename TypeName>
const Meta* TypeWrapper<TypeName>::GetMeta() const {
  if (!IsRegistered()) {
    return nullptr; 
  }
  return GetMetaDatabase()[GetOriginalTypeHashCode()];
}

template <typename TypeName>
Type CreateType() {
  return Type::CreateType<TypeName>();
}
}  // namespace Reflection
}  // namespace MM
