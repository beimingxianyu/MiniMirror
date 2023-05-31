#pragma once
#include <cstdint>

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

template <typename... BoolArgs>
static bool CheckAllTrue(bool arg1, BoolArgs... args) {
  return arg1 & CheckAllTrue(args...);
}

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
}  // namespace Utils
}  // namespace MM
