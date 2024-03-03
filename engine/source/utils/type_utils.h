#pragma once

#include <cstdint>
#include <type_traits>

namespace MM {
namespace Utils {
template <bool BoolValue>
struct BoolType {
  static constexpr bool value_ = BoolValue;
};

using FalseType = BoolType<false>;
using TrueType = BoolType<true>;

template <std::uint32_t NumValue>
struct NumType {
  static constexpr std::uint32_t value_ = NumValue;
};

using NumType0 = NumType<0>;
using NumType1 = NumType<1>;
using NumType2 = NumType<2>;
using NumType3 = NumType<3>;
using NumType4 = NumType<4>;
using NumType5 = NumType<5>;
using NumType6 = NumType<6>;
using NumType7 = NumType<7>;
using NumType8 = NumType<8>;
using NumType9 = NumType<9>;
using NumType10 = NumType<10>;
using NumType11 = NumType<11>;
using NumType12 = NumType<12>;
using NumType13 = NumType<13>;
using NumType14 = NumType<14>;
using NumType15 = NumType<15>;
using NumType16 = NumType<16>;
using NumType17 = NumType<17>;
using NumType18 = NumType<18>;
using NumType19 = NumType<19>;
using NumType20 = NumType<20>;
using NumType21 = NumType<21>;
using NumType22 = NumType<22>;
using NumType23 = NumType<23>;
using NumType24 = NumType<24>;
using NumType25 = NumType<25>;
using NumType26 = NumType<26>;
using NumType27 = NumType<27>;
using NumType28 = NumType<28>;
using NumType29 = NumType<29>;
using NumType30 = NumType<30>;
using NumType31 = NumType<31>;
using NumType32 = NumType<32>;
using NumType33 = NumType<33>;
using NumType34 = NumType<34>;
using NumType35 = NumType<35>;
using NumType36 = NumType<36>;
using NumType37 = NumType<37>;
using NumType38 = NumType<38>;
using NumType39 = NumType<39>;
using NumType40 = NumType<40>;
using NumType41 = NumType<41>;
using NumType42 = NumType<42>;
using NumType43 = NumType<43>;
using NumType44 = NumType<44>;
using NumType45 = NumType<45>;
using NumType46 = NumType<46>;
using NumType47 = NumType<47>;
using NumType48 = NumType<48>;
using NumType49 = NumType<49>;
using NumType50 = NumType<50>;
using NumType51 = NumType<51>;
using NumType52 = NumType<52>;
using NumType53 = NumType<53>;
using NumType54 = NumType<54>;
using NumType55 = NumType<55>;
using NumType56 = NumType<56>;
using NumType57 = NumType<57>;
using NumType58 = NumType<58>;
using NumType59 = NumType<59>;
using NumType60 = NumType<60>;
using NumType61 = NumType<61>;
using NumType62 = NumType<62>;
using NumType63 = NumType<63>;
using NumType64 = NumType<64>;

template <typename... Args>
class LastArg;

template <typename LastArgType>
class LastArg<LastArgType> {
 public:
  using Type = LastArgType;
};

template <typename FirstArg, typename... Args>
class LastArg<FirstArg, Args...> {
 public:
  using Type = typename LastArg<Args...>::Type;
};

template <bool Condition, typename True, typename False>
struct IfThenElse;

template <typename True, typename False>
struct IfThenElse<true, True, False> {
  using Type = True;
};

template <typename True, typename False>
struct IfThenElse<false, True, False> {
  using Type = False;
};

template <bool Condition, typename True, typename False>
using IfThenElseT = typename IfThenElse<Condition, True, False>::Type;

template <bool Condition1, bool Condition2, typename T>
struct IsOr {};

template <typename T>
struct IsOr<true, true, T> {
  using Type = T;
};

template <typename T>
struct IsOr<true, false, T> {
  using Type = T;
};

template <typename T>
struct IsOr<false, true, T> {
  using Type = T;
};

template <bool Condition1, bool Condition2, typename T>
struct IsAnd {};

template <typename T>
struct IsAnd<true, true, T> {
  using Type = T;
};

template <bool Condition1, bool Condition2, typename T>
struct IsXor {};

template <typename T>
struct IsXor<true, false, T> {
  using Type = T;
};

template <typename T>
struct IsXor<false, true, T> {
  using Type = T;
};

template <bool Condition1, bool Condition2>
using IsOrT = typename IsOr<Condition1, Condition2, void>::Type;

template <bool Condition1, bool Condition2>
using IsAndT = typename IsAnd<Condition1, Condition2, void>::Type;

template <bool Condition1, bool Condition2>
using IsXorT = typename IsXor<Condition1, Condition2, void>::Type;

template <typename T1, typename T2, typename... Args>
struct IsAllSame;

template <typename T>
struct IsAllSame<T, T> {
  static constexpr bool Value = true;
};

template <typename T1, typename T2>
struct IsAllSame<T1, T2> {
  static constexpr bool Value = false;
};

template <typename T1, typename T2, typename... Args>
struct IsAllSame {
  static constexpr bool Value =
      IsAllSame<T1, T2>::Value && IsAllSame<T2, Args...>::Value;
  ;
};

template <typename T1, typename T2, typename... Args>
constexpr bool IsAllSameV = IsAllSame<T1, T2, Args...>::Value;

template <typename... BoolTypes>
bool CheckAllTrue(bool arg1, BoolTypes... args) {
  static_assert(IsAllSameV<bool, BoolTypes...>);
  return arg1 && (args && ...);
}

template <typename T, typename U>
class Conversion {
 private:
  static char Test(U);
  static int Test(...);
  static T MakeT();

 public:
  enum { value = sizeof(Test(MakeT())) == sizeof(char) };
};

template<typename Func>
struct FunctionSignature;

template<typename ReturnType_, typename... Args_>
struct FunctionSignature<ReturnType_(*)(Args_...)> {
  using ReturnType = ReturnType_;
  using Args = std::tuple<Args_...>;
  constexpr static std::uint32_t ArgsNum = sizeof...(Args_);
  constexpr static bool IsStatic = true;
  constexpr static bool IsConst = false;
  constexpr static bool IsVolatile = false;
};

template<typename ReturnType_, typename InstanceType_, typename... Args_>
struct FunctionSignature<ReturnType_(InstanceType_::*)(Args_...)> {
  using ReturnType = ReturnType_;
  using InstanceType = InstanceType_;
  using Args = std::tuple<Args_...>;
  constexpr static std::uint32_t ArgsNum = sizeof...(Args_);
  constexpr static bool IsStatic = false;
  constexpr static bool IsConst = false;
  constexpr static bool IsVolatile = false;
};

template<typename ReturnType_, typename InstanceType_, typename... Args_>
struct FunctionSignature<ReturnType_(InstanceType_::*)(Args_...) const> {
  using ReturnType = ReturnType_;
  using InstanceType = InstanceType_;
  using Args = std::tuple<Args_...>;
  constexpr static std::uint32_t ArgsNum = sizeof...(Args_);
  constexpr static bool IsStatic = false;
  constexpr static bool IsConst = true;
  constexpr static bool IsVolatile = false;
};

template<typename ReturnType_, typename InstanceType_, typename... Args_>
struct FunctionSignature<ReturnType_(InstanceType_::*)(Args_...) volatile> {
  using ReturnType = ReturnType_;
  using InstanceType = InstanceType_;
  using Args = std::tuple<Args_...>;
  constexpr static std::uint32_t ArgsNum = sizeof...(Args_);
  constexpr static bool IsStatic = false;
  constexpr static bool IsConst = false;
  constexpr static bool IsVolatile = true;
};

template<typename ReturnType_, typename InstanceType_, typename... Args_>
struct FunctionSignature<ReturnType_(InstanceType_::*)(Args_...) const volatile> {
  using ReturnType = ReturnType_;
  using InstanceType = InstanceType_;
  using Args = std::tuple<Args_...>;
  constexpr static std::uint32_t ArgsNum = sizeof...(Args_);
  constexpr static bool IsStatic = false;
  constexpr static bool IsConst = true;
  constexpr static bool IsVolatile = true;
};

template<typename Sig>
struct FunctionSignature : public FunctionSignature<decltype(&Sig::operator())> {};

template <typename T>
struct PropertyMetaData;

template <typename ClassType_, typename PropertyType_>
struct PropertyMetaData<PropertyType_ ClassType_::*> {
  using ClassType = ClassType_;
  using PropertyType = PropertyType_;
  constexpr static std::uint32_t size = sizeof(PropertyType_);
  constexpr static bool IsStatic = false;
};

template <typename PropertyType_>
struct PropertyMetaData<PropertyType_ *> {
  using ClassType = void;
  using PropertyType = PropertyType_;
  constexpr static std::uint32_t size = sizeof(PropertyType_);
  constexpr static bool IsStatic = true;
};

template <typename Fun>
bool IsFunction = std::is_function_v<Fun>;

template <typename Fun>
bool IsMemberFunction = std::is_member_function_pointer_v<std::decay_t<Fun>>;

template <typename Fun>
bool IsClass = std::is_class_v<std::decay_t<Fun>>;

template <class T, T... I>
struct IntegerSequence {
  template <T N>
  using Append = IntegerSequence<T, I..., N>;
  static std::size_t size() { return sizeof...(I); }
  using Next = Append<sizeof...(I)>;
  using Type = T;
};

template <class T, T Index, std::size_t N>
struct SequenceGenerator {
  using Type = typename SequenceGenerator<T, Index - 1, N - 1>::Type::Next;
};

template <class T, T Index>
struct SequenceGenerator<T, Index, 0ul> {
  using Type = IntegerSequence<T>;
};

template <std::size_t... I>
using IndexSequence = IntegerSequence<std::size_t, I...>;

template <class T, T N>
using MakeIntegerSequence = typename SequenceGenerator<T, N, N>::Type;

template <std::size_t N>
using MakeIndexSequence = MakeIntegerSequence<std::size_t, N>;

template <class... T>
using IndexSequenceFor = MakeIndexSequence<sizeof...(T)>;

template <typename FirstType, typename... Types>
struct AllTypeHashCode {
  static std::size_t HashCode();
};

template <typename FirstType>
struct AllTypeHashCode<FirstType> {
  static std::size_t HashCode();
};

template <typename FirstType, typename... Types>
std::size_t AllTypeHashCode<FirstType, Types...>::HashCode() {
  return typeid(FirstType).hash_code() +
         (AllTypeHashCode<Types...>::HashCode() << 1);
}

template <typename FirstType>
std::size_t AllTypeHashCode<FirstType>::HashCode() {
  return typeid(FirstType).hash_code();
}

 /**
 * \brief A destructor that does nothing. Prevent the deletion of data by
 * variable reference obtained through \ref GetProperty(const Variable&
 * class_variable) from causing access errors.
 */
template<typename T>
struct DoNothingDestructor {
  void operator()(T*) {}
};

/**
 * \brief Get the original type of \ref TypeName (no reference, const, pointer,
 * etc.); \tparam TypeName You want to get the type of its original type.
 */
template <typename TypeName>
struct GetOriginalType {
  using Type = std::remove_const_t<std::remove_reference_t<
      std::remove_pointer_t<std::remove_all_extents_t<TypeName>>>>;
};

template <typename TypeName>
using GetOriginalTypeT = typename GetOriginalType<TypeName>::Type;

template <typename T> struct GetCommonType {
  using Type = std::remove_reference_t<std::remove_const_t<T>>;
};

template <typename PointType> struct GetCommonType<const PointType *> {
  using Type = std::remove_const_t<PointType *>;
};

template <typename PointType> struct GetCommonType<const PointType *const> {
  using Type = std::remove_const_t<PointType *>;
};

template <typename T> using GetCommonTypeT = typename GetCommonType<T>::Type;

template<typename ReturnType, typename InstanceType, bool IsStatic, bool IsConst, bool IsVolatile, typename ...Args>
struct GetFunctionPointType {
  using Type = ReturnType(InstanceType::*)(Args...);
};

template<typename ReturnType, typename InstanceType, typename ...Args>
struct GetFunctionPointType<ReturnType, InstanceType, true, false, false, Args...> {
  using Type = ReturnType(*)(Args...);
};

template<typename ReturnType, typename InstanceType, typename ...Args>
struct GetFunctionPointType<ReturnType, InstanceType, false, true, false, Args...> {
  using Type = ReturnType(InstanceType::*)(Args...) const;
};

template<typename ReturnType, typename InstanceType, typename ...Args>
struct GetFunctionPointType<ReturnType, InstanceType, false, true, true, Args...> {
  using Type = ReturnType(InstanceType::*)(Args...) volatile const ;
};

template<typename ReturnType, typename InstanceType, typename ...Args>
struct GetFunctionPointType<ReturnType, InstanceType, false, false, true, Args...> {
  using Type = ReturnType(InstanceType::*)(Args...) volatile;
};

template<typename ReturnType, typename InstanceType, bool IsStatic, bool IsConst, bool IsVolatile,typename ...Args>
using GetFunctionPointTypeT =
    typename GetFunctionPointType<ReturnType, InstanceType, IsStatic, IsConst, IsVolatile, Args...>::Type;

template<typename T>
struct GetNotConstRefrence {
  using Type = T;
};

template<typename T>
struct GetNotConstRefrence<T&>
{
  using Type = T&;
};

template<typename T>
struct GetNotConstRefrence<const T&>
{
  using Type = T&;
};

template<typename T>
using GetNotConstRefrenceT = typename GetNotConstRefrence<T>::Type;
}  // namespace Utils

struct Nil {};
}  // namespace MM
