#pragma once

#include "runtime/core/reflection/database.h"

#include "runtime/platform/base/cross_platform_header.h"

namespace MM {
namespace Reflection {
template <typename T, typename U>
class Conversion {
 private:
  static char Test(U);
  static int Test(...);
  static T MakeT();

 public:
  enum { value = sizeof(Test(MakeT())) == sizeof(char) };
};

template <typename Sig>
struct signature;

template <typename Ret, typename... Args>
struct signature<Ret(Args...)> {
  using Type = std::tuple<Args...>;
  bool Member = false;
  bool Const = false;
};

template <typename Ret, typename Obj, typename... Args>
struct signature<Ret (Obj::*)(Args...)> {
  using Type = std::tuple<Args...>;
  bool Member = true;
  bool Const = false;
};

template <typename Ret, typename Obj, typename... Args>
struct signature<Ret (Obj::*)(Args...) const> {
  using Type = std::tuple<Args...>;
  bool Member = true;
  bool Const = true;
};

template <typename Fun>
bool is_fun = std::is_function_v<Fun>;

template <typename Fun>
bool is_mem_fun = std::is_member_function_pointer_v<std::decay_t<Fun>>;

template <typename Fun>
bool is_functor = std::is_class_v<std::decay_t<Fun>>;

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

}
}