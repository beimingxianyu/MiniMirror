#include <gtest/gtest.h>

#include "runtime/core/reflection/reflection.h"

using namespace MM::Reflection;

TEST(reflection, type) {
  const Type& int_type = Type::CreateType<int>();
  EXPECT_EQ(int_type.IsValid(), true);
  EXPECT_EQ(int_type.IsRegistered(), true);
  EXPECT_EQ(int_type.GetSize(), sizeof(int));
  EXPECT_EQ(int_type.HaveDestructor(), true);
  EXPECT_EQ(int_type.IsArray(), false);
  EXPECT_EQ(int_type.IsConst(), false);
  EXPECT_EQ(int_type.IsReference(), false);
  EXPECT_EQ(int_type.IsEnum(), false);
  EXPECT_EQ(int_type.IsPointer(), false);
  EXPECT_EQ(int_type.GetTypeName(), "std::int32_t");
  const Meta* int_meta = int_type.GetMate();
  EXPECT_NE(int_meta, nullptr);
  EXPECT_EQ(int_meta->GetTypeName(), "std::int32_t");
  EXPECT_EQ(int_meta->GetAllConstuctor().size(), 0);
  EXPECT_EQ(int_meta->GetAllMethod().size(), 0);
  EXPECT_EQ(int_meta->GetAllProperty().size(), 0);

  enum class TestEnum { UNDIFINED, VALUE1, VALUE2};
  const Type& enum_type1 = Type::CreateType<TestEnum>();
  EXPECT_EQ(enum_type1.IsValid(), true);
  EXPECT_EQ(enum_type1.IsRegistered(), false);
  EXPECT_EQ(enum_type1.GetSize(), sizeof(TestEnum));
  EXPECT_EQ(enum_type1.HaveDestructor(), true);
  EXPECT_EQ(enum_type1.IsArray(), false);
  EXPECT_EQ(enum_type1.IsConst(), false);
  EXPECT_EQ(enum_type1.IsReference(), false);
  EXPECT_EQ(enum_type1.IsEnum(), true);
  EXPECT_EQ(enum_type1.IsPointer(), false);
  EXPECT_EQ(enum_type1.GetTypeName(), std::string(""));
  const Meta* enum_meta1 = enum_type1.GetMate();
  EXPECT_EQ(enum_meta1, nullptr);

  const Type& enum_type2 = Type::CreateType<const TestEnum>();
  EXPECT_EQ(enum_type2.IsValid(), true);
  EXPECT_EQ(enum_type2.IsRegistered(), false);
  EXPECT_EQ(enum_type2.GetSize(), sizeof(TestEnum));
  EXPECT_EQ(enum_type2.HaveDestructor(), true);
  EXPECT_EQ(enum_type2.IsArray(), false);
  EXPECT_EQ(enum_type2.IsConst(), true);
  EXPECT_EQ(enum_type2.IsReference(), false);
  EXPECT_EQ(enum_type2.IsEnum(), true);
  EXPECT_EQ(enum_type2.IsPointer(), false);
  EXPECT_EQ(enum_type2.GetTypeName(), std::string(""));

  const Type& enum_type3 = Type::CreateType<TestEnum[3]>();
  EXPECT_EQ(enum_type3.IsValid(), true);
  EXPECT_EQ(enum_type3.IsRegistered(), false);
  EXPECT_EQ(enum_type3.GetSize(), sizeof(TestEnum[3]));
  EXPECT_EQ(enum_type3.HaveDestructor(), true);
  EXPECT_EQ(enum_type3.IsArray(), true);
  EXPECT_EQ(enum_type3.IsConst(), false);
  EXPECT_EQ(enum_type3.IsReference(), false);
  EXPECT_EQ(enum_type3.IsEnum(), false);
  EXPECT_EQ(enum_type3.IsPointer(), false);
  EXPECT_EQ(enum_type3.GetTypeName(), std::string(""));

  const Type& enum_type4 = Type::CreateType<const TestEnum[3]>();
  EXPECT_EQ(enum_type4.IsValid(), true);
  EXPECT_EQ(enum_type4.IsRegistered(), false);
  EXPECT_EQ(enum_type4.GetSize(), sizeof(const TestEnum[3]));
  EXPECT_EQ(enum_type4.HaveDestructor(), true);
  EXPECT_EQ(enum_type4.IsArray(), true);
  EXPECT_EQ(enum_type4.IsConst(), true);
  EXPECT_EQ(enum_type4.IsReference(), false);
  EXPECT_EQ(enum_type4.IsEnum(), false);
  EXPECT_EQ(enum_type4.IsPointer(), false);
  EXPECT_EQ(enum_type4.GetTypeName(), std::string(""));
                     
  const Type& enum_type5 = Type::CreateType<TestEnum*>();
  EXPECT_EQ(enum_type5.IsValid(), true);
  EXPECT_EQ(enum_type5.IsRegistered(), false);
  EXPECT_EQ(enum_type5.GetSize(), sizeof(TestEnum*));
  EXPECT_EQ(enum_type5.HaveDestructor(), true);
  EXPECT_EQ(enum_type5.IsArray(), false);
  EXPECT_EQ(enum_type5.IsConst(), false);
  EXPECT_EQ(enum_type5.IsReference(), false);
  EXPECT_EQ(enum_type5.IsEnum(), false);
  EXPECT_EQ(enum_type5.IsPointer(), true);
  EXPECT_EQ(enum_type5.GetTypeName(), std::string(""));

  const Type& enum_type6 = Type::CreateType<const TestEnum*>();
  EXPECT_EQ(enum_type6.IsValid(), true);
  EXPECT_EQ(enum_type6.IsRegistered(), false);
  EXPECT_EQ(enum_type6.GetSize(), sizeof(TestEnum*));
  EXPECT_EQ(enum_type6.HaveDestructor(), true);
  EXPECT_EQ(enum_type6.IsArray(), false);
  EXPECT_EQ(enum_type6.IsConst(), false);
  EXPECT_EQ(enum_type6.IsReference(), false);
  EXPECT_EQ(enum_type6.IsEnum(), false);
  EXPECT_EQ(enum_type6.IsPointer(), true);
  EXPECT_EQ(enum_type6.GetTypeName(), std::string(""));

  const Type& enum_type7 = Type::CreateType<TestEnum* const>();
  EXPECT_EQ(enum_type7.IsValid(), true);
  EXPECT_EQ(enum_type7.IsRegistered(), false);
  EXPECT_EQ(enum_type7.GetSize(), sizeof(TestEnum*));
  EXPECT_EQ(enum_type7.HaveDestructor(), true);
  EXPECT_EQ(enum_type7.IsArray(), false);
  EXPECT_EQ(enum_type7.IsConst(), true);
  EXPECT_EQ(enum_type7.IsReference(), false);
  EXPECT_EQ(enum_type7.IsEnum(), false);
  EXPECT_EQ(enum_type7.IsPointer(), true);
  EXPECT_EQ(enum_type7.GetTypeName(), std::string(""));
}

class MethodTestClass {
  MM_GENERATE_REFLECTION_BODY()

public:
  char Function0 (char arg1)  {return 0.0;}

  char CFunction0 (char arg1) const {return 1.0;}

  static char SFunction0 (char arg1)  {return 2.0;}

  std::uint16_t Function1 (char arg1, const std::uint16_t& arg2)  {return 3.0;}

  std::uint16_t CFunction1 (char arg1, const std::uint16_t& arg2) const {return 4.0;}

  static std::uint16_t SFunction1 (char arg1, const std::uint16_t& arg2)  {return 5.0;}

  std::uint32_t Function2 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3)  {return 6.0;}

  std::uint32_t CFunction2 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3) const {return 7.0;}

  static std::uint32_t SFunction2 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3)  {return 8.0;}

  std::uint64_t Function3 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4)  {return 9.0;}

  std::uint64_t CFunction3 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4) const {return 10.0;}

  static std::uint64_t SFunction3 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4)  {return 11.0;}

  std::int8_t Function4 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5)  {return 12.0;}

  std::int8_t CFunction4 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5) const {return 13.0;}

  static std::int8_t SFunction4 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5)  {return 14.0;}

  std::int16_t Function5 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6)  {return 15.0;}

  std::int16_t CFunction5 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6) const {return 16.0;}

  static std::int16_t SFunction5 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6)  {return 17.0;}

  std::int32_t Function6 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7)  {return 18.0;}

  std::int32_t CFunction6 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7) const {return 19.0;}

  static std::int32_t SFunction6 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7)  {return 20.0;}

  std::int64_t Function7 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8)  {return 21.0;}

  std::int64_t CFunction7 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8) const {return 22.0;}

  static std::int64_t SFunction7 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8)  {return 23.0;}

  float Function8 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9)  {return 24.0;}

  float CFunction8 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9) const {return 25.0;}

  static float SFunction8 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9)  {return 26.0;}

  double Function9 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9, const double& arg10)  {return 27.0;}

  double CFunction9 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9, const double& arg10) const {return 28.0;}

  static double SFunction9 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9, const double& arg10)  {return 29.0;}

  std::string Function10 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9, const double& arg10, const std::string& arg11)  {return std::string("30.0");}

  std::string CFunction10 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9, const double& arg10, const std::string& arg11) const {return std::string("31.0");}

  static std::string SFunction10 (char arg1, const std::uint16_t& arg2, std::uint32_t& arg3, const std::uint64_t* const arg4, const std::int8_t* arg5, std::int16_t* const arg6, std::int32_t* arg7, std::int64_t arg8, const float arg9, const double& arg10, const std::string& arg11)  {return std::string("32.0");}

  const std::string& GetStringData() {
    return string_data;
  }

private:
  void VoidFunction() {}

private:
  std::string string_data{"data"};
};

MM_REGISTER {
  Class<MethodTestClass>("MethodTestClass")
    .Method("Function0", &MethodTestClass::Function0)
    .Method("CFunction0", &MethodTestClass::CFunction0)
    .Method("SFunction0", &MethodTestClass::SFunction0)
    .Method("Function1", &MethodTestClass::Function1)
    .Method("CFunction1", &MethodTestClass::CFunction1)
    .Method("SFunction1", &MethodTestClass::SFunction1)
    .Method("Function2", &MethodTestClass::Function2)
    .Method("CFunction2", &MethodTestClass::CFunction2)
    .Method("SFunction2", &MethodTestClass::SFunction2)
    .Method("Function3", &MethodTestClass::Function3)
    .Method("CFunction3", &MethodTestClass::CFunction3)
    .Method("SFunction3", &MethodTestClass::SFunction3)
    .Method("Function4", &MethodTestClass::Function4)
    .Method("CFunction4", &MethodTestClass::CFunction4)
    .Method("SFunction4", &MethodTestClass::SFunction4)
    .Method("Function5", &MethodTestClass::Function5)
    .Method("CFunction5", &MethodTestClass::CFunction5)
    .Method("SFunction5", &MethodTestClass::SFunction5)
    .Method("Function6", &MethodTestClass::Function6)
    .Method("CFunction6", &MethodTestClass::CFunction6)
    .Method("SFunction6", &MethodTestClass::SFunction6)
    .Method("Function7", &MethodTestClass::Function7)
    .Method("CFunction7", &MethodTestClass::CFunction7)
    .Method("SFunction7", &MethodTestClass::SFunction7)
    .Method("Function8", &MethodTestClass::Function8)
    .Method("CFunction8", &MethodTestClass::CFunction8)
    .Method("SFunction8", &MethodTestClass::SFunction8)
    .Method("Function9", &MethodTestClass::Function9)
    .Method("CFunction9", &MethodTestClass::CFunction9)
    .Method("SFunction9", &MethodTestClass::SFunction9)
    .Method("Function10", &MethodTestClass::Function10)
    .Method("CFunction10", &MethodTestClass::CFunction10)
    .Method("SFunction10", &MethodTestClass::SFunction10)
    .Method("VoidFunction", &MethodTestClass::VoidFunction)
    .Method("GetStringData", &MethodTestClass::GetStringData);
}

TEST(reflection, method) {
  Variable empty_variable{};
  Variable method_variable{Variable::EmplaceVariable<MethodTestClass>()};

  std::uint64_t static_variable3 = 3.0;
  std::int8_t static_variable4 = 4.0;
  std::int16_t static_variable5 = 5.0;
  std::int32_t static_variable6 = 6.0;
  Variable variable0{Variable::EmplaceVariable<char>(0.0)};
  Variable variable1{Variable::EmplaceVariable<std::uint16_t>(1.0)};
  Variable variable2{Variable::EmplaceVariable<std::uint32_t>(2.0)};
  Variable variable3{Variable::EmplaceVariable<const std::uint64_t* const>(&static_variable3)};
  Variable variable4{Variable::EmplaceVariable<const std::int8_t*>(&static_variable4)};
  Variable variable5{Variable::EmplaceVariable<std::int16_t* const>(&static_variable5)};
  Variable variable6{Variable::EmplaceVariable<std::int32_t*>(&static_variable6)};
  Variable variable7{Variable::EmplaceVariable<std::int64_t>(7.0)};
  Variable variable8{Variable::EmplaceVariable<float>(8.0)};
  Variable variable9{Variable::EmplaceVariable<double>(9.0)};
  Variable variable10{Variable::EmplaceVariable<std::string>("10.0")};


  const Meta* method_test_meta = GetMetaDatabase().at(typeid(MethodTestClass).hash_code());
  EXPECT_EQ(method_test_meta->HaveMethod("FunTest1"), false);
  EXPECT_EQ(method_test_meta->GetMethod("FunTest2"), nullptr);
  EXPECT_EQ(method_test_meta->HaveMethod("Function0"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction0"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction0"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function0"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction0"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction0"), nullptr);
  Variable result_variable0 = method_test_meta->GetMethod("Function0")->Invoke(method_variable, variable0);
  EXPECT_EQ(result_variable0.IsValid(), true);
  EXPECT_EQ(*static_cast<char*>(result_variable0.GetValue()), static_cast<char>(0.0));
  Variable result_variable1 = method_test_meta->GetMethod("CFunction0")->Invoke(method_variable, variable0);
  EXPECT_EQ(result_variable1.IsValid(), true);
  EXPECT_EQ(*static_cast<char*>(result_variable1.GetValue()), static_cast<char>(1.0));
  Variable result_variable2 = method_test_meta->GetMethod("SFunction0")->Invoke(empty_variable, variable0);
  EXPECT_EQ(result_variable2.IsValid(), true);
  EXPECT_EQ(*static_cast<char*>(result_variable2.GetValue()), static_cast<char>(2.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function1"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction1"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction1"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function1"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction1"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction1"), nullptr);
  Variable result_variable3 = method_test_meta->GetMethod("Function1")->Invoke(method_variable, variable0, variable1);
  EXPECT_EQ(result_variable3.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint16_t*>(result_variable3.GetValue()), static_cast<std::uint16_t>(3.0));
  Variable result_variable4 = method_test_meta->GetMethod("CFunction1")->Invoke(method_variable, variable0, variable1);
  EXPECT_EQ(result_variable4.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint16_t*>(result_variable4.GetValue()), static_cast<std::uint16_t>(4.0));
  Variable result_variable5 = method_test_meta->GetMethod("SFunction1")->Invoke(empty_variable, variable0, variable1);
  EXPECT_EQ(result_variable5.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint16_t*>(result_variable5.GetValue()), static_cast<std::uint16_t>(5.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function2"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction2"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction2"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function2"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction2"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction2"), nullptr);
  Variable result_variable6 = method_test_meta->GetMethod("Function2")->Invoke(method_variable, variable0, variable1, variable2);
  EXPECT_EQ(result_variable6.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint32_t*>(result_variable6.GetValue()), static_cast<std::uint32_t>(6.0));
  Variable result_variable7 = method_test_meta->GetMethod("CFunction2")->Invoke(method_variable, variable0, variable1, variable2);
  EXPECT_EQ(result_variable7.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint32_t*>(result_variable7.GetValue()), static_cast<std::uint32_t>(7.0));
  Variable result_variable8 = method_test_meta->GetMethod("SFunction2")->Invoke(empty_variable, variable0, variable1, variable2);
  EXPECT_EQ(result_variable8.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint32_t*>(result_variable8.GetValue()), static_cast<std::uint32_t>(8.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function3"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction3"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction3"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function3"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction3"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction3"), nullptr);
  Variable result_variable9 = method_test_meta->GetMethod("Function3")->Invoke(method_variable, variable0, variable1, variable2, variable3);
  EXPECT_EQ(result_variable9.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint64_t*>(result_variable9.GetValue()), static_cast<std::uint64_t>(9.0));
  Variable result_variable10 = method_test_meta->GetMethod("CFunction3")->Invoke(method_variable, variable0, variable1, variable2, variable3);
  EXPECT_EQ(result_variable10.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint64_t*>(result_variable10.GetValue()), static_cast<std::uint64_t>(10.0));
  Variable result_variable11 = method_test_meta->GetMethod("SFunction3")->Invoke(empty_variable, variable0, variable1, variable2, variable3);
  EXPECT_EQ(result_variable11.IsValid(), true);
  EXPECT_EQ(*static_cast<std::uint64_t*>(result_variable11.GetValue()), static_cast<std::uint64_t>(11.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function4"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction4"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction4"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function4"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction4"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction4"), nullptr);
  Variable result_variable12 = method_test_meta->GetMethod("Function4")->Invoke(method_variable, variable0, variable1, variable2, variable3, variable4);
  EXPECT_EQ(result_variable12.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int8_t*>(result_variable12.GetValue()), static_cast<std::int8_t>(12.0));
  Variable result_variable13 = method_test_meta->GetMethod("CFunction4")->Invoke(method_variable, variable0, variable1, variable2, variable3, variable4);
  EXPECT_EQ(result_variable13.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int8_t*>(result_variable13.GetValue()), static_cast<std::int8_t>(13.0));
  Variable result_variable14 = method_test_meta->GetMethod("SFunction4")->Invoke(empty_variable, variable0, variable1, variable2, variable3, variable4);
  EXPECT_EQ(result_variable14.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int8_t*>(result_variable14.GetValue()), static_cast<std::int8_t>(14.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function5"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction5"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction5"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function5"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction5"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction5"), nullptr);
  Variable result_variable15 = method_test_meta->GetMethod("Function5")->Invoke(method_variable, variable0, variable1, variable2, variable3, variable4, variable5);
  EXPECT_EQ(result_variable15.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int16_t*>(result_variable15.GetValue()), static_cast<std::int16_t>(15.0));
  Variable result_variable16 = method_test_meta->GetMethod("CFunction5")->Invoke(method_variable, variable0, variable1, variable2, variable3, variable4, variable5);
  EXPECT_EQ(result_variable16.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int16_t*>(result_variable16.GetValue()), static_cast<std::int16_t>(16.0));
  Variable result_variable17 = method_test_meta->GetMethod("SFunction5")->Invoke(empty_variable, variable0, variable1, variable2, variable3, variable4, variable5);
  EXPECT_EQ(result_variable17.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int16_t*>(result_variable17.GetValue()), static_cast<std::int16_t>(17.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function6"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction6"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction6"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function6"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction6"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction6"), nullptr);
  Variable result_variable18 = method_test_meta->GetMethod("Function6")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6});
  EXPECT_EQ(result_variable18.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int32_t*>(result_variable18.GetValue()), static_cast<std::int32_t>(18.0));
  Variable result_variable19 = method_test_meta->GetMethod("CFunction6")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6});
  EXPECT_EQ(result_variable19.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int32_t*>(result_variable19.GetValue()), static_cast<std::int32_t>(19.0));
  Variable result_variable20 = method_test_meta->GetMethod("SFunction6")->Invoke(empty_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6});
  EXPECT_EQ(result_variable20.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int32_t*>(result_variable20.GetValue()), static_cast<std::int32_t>(20.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function7"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction7"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction7"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function7"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction7"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction7"), nullptr);
  Variable result_variable21 = method_test_meta->GetMethod("Function7")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7});
  EXPECT_EQ(result_variable21.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int64_t*>(result_variable21.GetValue()), static_cast<std::int64_t>(21.0));
  Variable result_variable22 = method_test_meta->GetMethod("CFunction7")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7});
  EXPECT_EQ(result_variable22.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int64_t*>(result_variable22.GetValue()), static_cast<std::int64_t>(22.0));
  Variable result_variable23 = method_test_meta->GetMethod("SFunction7")->Invoke(empty_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7});
  EXPECT_EQ(result_variable23.IsValid(), true);
  EXPECT_EQ(*static_cast<std::int64_t*>(result_variable23.GetValue()), static_cast<std::int64_t>(23.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function8"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction8"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction8"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function8"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction8"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction8"), nullptr);
  Variable result_variable24 = method_test_meta->GetMethod("Function8")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8});
  EXPECT_EQ(result_variable24.IsValid(), true);
  EXPECT_EQ(*static_cast<float*>(result_variable24.GetValue()), static_cast<float>(24.0));
  Variable result_variable25 = method_test_meta->GetMethod("CFunction8")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8});
  EXPECT_EQ(result_variable25.IsValid(), true);
  EXPECT_EQ(*static_cast<float*>(result_variable25.GetValue()), static_cast<float>(25.0));
  Variable result_variable26 = method_test_meta->GetMethod("SFunction8")->Invoke(empty_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8});
  EXPECT_EQ(result_variable26.IsValid(), true);
  EXPECT_EQ(*static_cast<float*>(result_variable26.GetValue()), static_cast<float>(26.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function9"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction9"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction9"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function9"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction9"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction9"), nullptr);
  Variable result_variable27 = method_test_meta->GetMethod("Function9")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8, &variable9});
  EXPECT_EQ(result_variable27.IsValid(), true);
  EXPECT_EQ(*static_cast<double*>(result_variable27.GetValue()), static_cast<double>(27.0));
  Variable result_variable28 = method_test_meta->GetMethod("CFunction9")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8, &variable9});
  EXPECT_EQ(result_variable28.IsValid(), true);
  EXPECT_EQ(*static_cast<double*>(result_variable28.GetValue()), static_cast<double>(28.0));
  Variable result_variable29 = method_test_meta->GetMethod("SFunction9")->Invoke(empty_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8, &variable9});
  EXPECT_EQ(result_variable29.IsValid(), true);
  EXPECT_EQ(*static_cast<double*>(result_variable29.GetValue()), static_cast<double>(29.0));
  EXPECT_EQ(method_test_meta->HaveMethod("Function10"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("CFunction10"), true);
  EXPECT_EQ(method_test_meta->HaveMethod("SFunction10"), true);
  EXPECT_NE(method_test_meta->GetMethod("Function10"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("CFunction10"), nullptr);
  EXPECT_NE(method_test_meta->GetMethod("SFunction10"), nullptr);
  Variable result_variable30 = method_test_meta->GetMethod("Function10")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8, &variable9, &variable10});
  EXPECT_EQ(result_variable30.IsValid(), true);
  EXPECT_EQ(*static_cast<std::string*>(result_variable30.GetValue()), static_cast<std::string>("30.0"));
  Variable result_variable31 = method_test_meta->GetMethod("CFunction10")->Invoke(method_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8, &variable9, &variable10});
  EXPECT_EQ(result_variable31.IsValid(), true);
  EXPECT_EQ(*static_cast<std::string*>(result_variable31.GetValue()), static_cast<std::string>("31.0"));
  Variable result_variable32 = method_test_meta->GetMethod("SFunction10")->Invoke(empty_variable, std::vector<Variable*>{&variable0, &variable1, &variable2, &variable3, &variable4, &variable5, &variable6, &variable7, &variable8, &variable9, &variable10});
  EXPECT_EQ(result_variable32.IsValid(), true);
  EXPECT_EQ(*static_cast<std::string*>(result_variable32.GetValue()), static_cast<std::string>("32.0"));

  EXPECT_EQ(method_test_meta->HaveMethod("VoidFunction"), true);
  EXPECT_NE(method_test_meta->GetMethod("VoidFunction"), nullptr);
  Variable void_result = method_test_meta->GetMethod("VoidFunction")->Invoke(method_variable);
  EXPECT_EQ(void_result.IsVoid(), true);
  EXPECT_EQ(void_result.IsValid(), true);

  EXPECT_EQ(method_test_meta->HaveMethod("GetStringData"), true);
  EXPECT_NE(method_test_meta->GetMethod("GetStringData"), nullptr);
  Variable string_refrence = method_test_meta->GetMethod("GetStringData")->Invoke(method_variable);
  EXPECT_EQ(string_refrence.IsVoid(), false);
  EXPECT_EQ(string_refrence.IsValid(), true);
  EXPECT_EQ(string_refrence.GetValue(), &(static_cast<MethodTestClass*>(method_variable.GetValue())->GetStringData()));
}

class PropertyTestClass {
  MM_GENERATE_REFLECTION_BODY()

public:
  int property1_{10};
  float property2_{20.0};
  static std::string property3_;
};

std::string PropertyTestClass::property3_{"string"};

MM_REGISTER {
  Class<PropertyTestClass>{"PropertyTestClass"}
    .Property("property1_", &PropertyTestClass::property1_)
    .Property("property2_", &PropertyTestClass::property2_)
    .Property("property3_", &PropertyTestClass::property3_);
}

TEST(reflection, property) {
  const Meta* property_test_meta = GetMetaDatabase().at(typeid(PropertyTestClass).hash_code());
  EXPECT_NE(property_test_meta, nullptr);
  EXPECT_EQ(property_test_meta->GetAllProperty().size(), 3);
  EXPECT_EQ(property_test_meta->HaveProperty("falid_property"), false);
  EXPECT_EQ(property_test_meta->HaveProperty("property1_"), true);
  EXPECT_EQ(property_test_meta->HaveProperty("property2_"), true);
  EXPECT_EQ(property_test_meta->HaveProperty("property3_"), true);
  const Property* property1 = property_test_meta->GetProperty("property1_");
  const Property* property2 = property_test_meta->GetProperty("property2_");
  const Property* property3 = property_test_meta->GetProperty("property3_");
  EXPECT_NE(property1, nullptr);
  EXPECT_NE(property2, nullptr);
  EXPECT_NE(property3, nullptr);
  EXPECT_EQ(property1->IsValid(), true);
  EXPECT_EQ(property2->IsValid(), true);
  EXPECT_EQ(property3->IsValid(), true);
  EXPECT_EQ(property1->IsStatic(), false);
  EXPECT_EQ(property2->IsStatic(), false);
  EXPECT_EQ(property3->IsStatic(), true);
  EXPECT_EQ(property1->GetClassMeta(), property_test_meta);
  EXPECT_EQ(property2->GetClassMeta(), property_test_meta);
  EXPECT_EQ(property3->GetClassMeta(), property_test_meta);
  EXPECT_EQ(property1->GetType()->GetTypeHashCode(), typeid(int).hash_code());
  EXPECT_EQ(property2->GetType()->GetTypeHashCode(), typeid(float).hash_code());
  EXPECT_EQ(property3->GetType()->GetTypeHashCode(), typeid(std::string).hash_code());
  EXPECT_EQ(property1->GetStaticPropertyAddress(), nullptr);
  EXPECT_EQ(property2->GetStaticPropertyAddress(), nullptr);
  EXPECT_EQ(property3->GetStaticPropertyAddress(), &PropertyTestClass::property3_);
  EXPECT_EQ(property1->GetPropertyName(), std::string("property1_"));
  EXPECT_EQ(property2->GetPropertyName(), std::string("property2_"));
  EXPECT_EQ(property3->GetPropertyName(), std::string("property3_"));
  EXPECT_EQ(property1->GetPropertyOffset(), offsetof(PropertyTestClass, property1_));
  EXPECT_EQ(property2->GetPropertyOffset(), offsetof(PropertyTestClass, property2_));
  EXPECT_EQ(property3->GetPropertyOffset(), 0);
  EXPECT_EQ(property1->GetPropertySize(), sizeof(int));
  EXPECT_EQ(property2->GetPropertySize(), sizeof(float));
  EXPECT_EQ(property3->GetPropertySize(), sizeof(std::string));
}

class VariableTestClass {
  MM_GENERATE_REFLECTION_BODY()

public:
  VariableTestClass(int property1, const float& property2) : property1_(property1), property2_(property2) {}

public:
  int property1_{10};
  float property2_{20.0};
  static std::string property3_;

  int GetProperty1() const {
    return property1_;
  }

  float GetProperty2() const {
    return property2_;
  }

  const float& GetProperty2Refrence() const {
    return property2_;
  }

  void SetProperty1(const int& property1) {
    property1_ = property1;
  }

  void SetProperty2(float property2) {
    property2_ = property2;
  }
};

std::string VariableTestClass::property3_{"string"};

MM_REGISTER {
  Class<VariableTestClass>{"VariableTestClass"}
    .Constructor<int, const float&>("Init")
    .Property("property1_", &VariableTestClass::property1_)
    .Property("property2_", &VariableTestClass::property2_)
    .Property("property3_", &VariableTestClass::property3_)
    .Method("GetProperty1", &VariableTestClass::GetProperty1)
    .Method("GetProperty2", &VariableTestClass::GetProperty2)
    .Method("GetProperty2Refrence", &VariableTestClass::GetProperty2Refrence)
    .Method("SetProperty1", &VariableTestClass::SetProperty1)
    .Method("SetProperty2", &VariableTestClass::SetProperty2);
}

TEST(reflection, variable) {
  const Meta* variable_test_meta = GetMetaDatabase().at(typeid(VariableTestClass).hash_code());
  EXPECT_NE(variable_test_meta, nullptr);

  std::vector<const Method*> constructors = variable_test_meta->GetAllConstuctor();
  std::vector<const Method*> methods = variable_test_meta->GetAllMethod();
  std::vector<const Property*> properties = variable_test_meta->GetAllProperty();

  EXPECT_EQ(constructors.size(), 1);
  EXPECT_EQ(properties.size(), 3);
  EXPECT_EQ(methods.size(), 5);

  EXPECT_EQ(variable_test_meta->HaveConstructor("Init"), true);
  EXPECT_EQ(variable_test_meta->HaveConstructor("falid_constructor"), false);
  EXPECT_EQ(variable_test_meta->HaveProperty("property1_"), true);
  EXPECT_EQ(variable_test_meta->HaveProperty("property2_"), true);
  EXPECT_EQ(variable_test_meta->HaveProperty("property3_"), true);
  EXPECT_EQ(variable_test_meta->HaveProperty("falid_property"), false);
  EXPECT_EQ(variable_test_meta->HaveMethod("GetProperty1"), true);
  EXPECT_EQ(variable_test_meta->HaveMethod("GetProperty2"), true);
  EXPECT_EQ(variable_test_meta->HaveMethod("GetProperty2Refrence"), true);
  EXPECT_EQ(variable_test_meta->HaveMethod("SetProperty1"), true);
  EXPECT_EQ(variable_test_meta->HaveMethod("SetProperty2"), true);
  EXPECT_EQ(variable_test_meta->HaveMethod("SetProperty2"), true);
  EXPECT_EQ(variable_test_meta->HaveMethod("falid_method"), false);

  Variable empty_variable{};
  Variable constructor_arg1 = Variable::EmplaceVariable<int>(20), constructor_arg2 = Variable::EmplaceVariable<float>(10.0);
  Variable invalid_variable = constructors[0]->Invoke(empty_variable, empty_variable, constructor_arg2);
  EXPECT_EQ(invalid_variable.IsValid(), false);
  Variable new_variable = constructors[0]->Invoke(empty_variable, constructor_arg1, constructor_arg2);
  EXPECT_EQ(new_variable.IsValid(), true);
  VariableTestClass* variable_object = static_cast<VariableTestClass*>(new_variable.GetValue());
  EXPECT_EQ(variable_object->property1_, 20);
  EXPECT_EQ(variable_object->property2_, 10.0);

  EXPECT_EQ(variable_test_meta->GetMethod("GetProperty1")->Invoke(new_variable).GetValueCast<int>(), 20);
  EXPECT_EQ(variable_test_meta->GetMethod("GetProperty2")->Invoke(new_variable).GetValueCast<float>(), 10.0);
  EXPECT_EQ(variable_test_meta->GetMethod("GetProperty2Refrence")->Invoke(new_variable).GetValue(), &(variable_object->property2_));
  Variable new_int_variable = Variable::EmplaceVariable<int>(10), new_float_variable = Variable::EmplaceVariable<float>(20.0);
  EXPECT_EQ(variable_test_meta->GetMethod("SetProperty1")->Invoke(new_variable, new_int_variable).IsVoid(), true);
  EXPECT_EQ(variable_test_meta->GetMethod("SetProperty2")->Invoke(new_variable, new_float_variable).IsVoid(), true);
  EXPECT_EQ(variable_test_meta->GetMethod("GetProperty1")->Invoke(new_variable).GetValueCast<int>(), 10);
  EXPECT_EQ(variable_test_meta->GetMethod("GetProperty2")->Invoke(new_variable).GetValueCast<float>(), 20.0);
}

TEST(reflection, meta) {
  // default types
  EXPECT_NE(GetMetaDatabase().find(typeid(void).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(char).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::uint16_t).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::uint32_t).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::uint64_t).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::int8_t).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::int16_t).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::int32_t).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::int64_t).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(float).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(double).hash_code()), GetMetaDatabase().end());
  EXPECT_NE(GetMetaDatabase().find(typeid(std::string).hash_code()), GetMetaDatabase().end());
}
