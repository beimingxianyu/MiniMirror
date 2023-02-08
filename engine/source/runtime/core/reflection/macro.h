#pragma once

#define MM_STR_(x) #x

#define MM_STR(x) MI_STR_(x)

#define MM_CONCAT_(a, b) a##b

#define MM_CONCAT(a, b) MI_CONCAT_(a, b)

#define MM_EXPEND(...) __VA_ARGS__

#define MM_GENERATE_MEMBER(member_1, member_2, member_3, member_4, member_5, member_6, member_7, member_8, member_9, member_10, member_11, member_12, member_13, member_14, member_15, member_16,          \
						   member_17, member_18, member_19, member_20, member_21, member_22, member_23, member_24, member_25, member_26, member_27, member_28, member_29, member_30, member_31, member_32, \
						   member_33, member_34, member_35, member_36, member_37, member_38, member_39, member_40, member_41, member_42, member_43, member_44, member_45, member_46, member_47, member_48, \
    		  			   member_49, member_50, member_51, member_52, member_53, member_54, member_55, member_56, member_57, member_58, member_59, member_60, member_61, member_62, member_63, member_64) \
						   member_1 member_2 member_3 member_4 member_5 member_6 member_7 member_8 member_9 member_10 member_11 member_12 member_13 member_14 member_15 member_16 \
					       member_17 member_18 member_19 member_20 member_21 member_22 member_23 member_24 member_25 member_26 member_27 member_28 member_29 member_30 member_31 member_32 \
					       member_33 member_34 member_35 member_36 member_37 member_38 member_39 member_40 member_41 member_42 member_43 member_44 member_45 member_46 member_47 member_48 \
					       member_49 member_50 member_51 member_52 member_53 member_54 member_55 member_56 member_57 member_58 member_59 member_60 member_61 member_62 member_63 member_64

#define MM_CLASS(id, name, ...)     \
	static struct __ReflexRegister {              \
		__ReflexRegister() {               \
			reflection_class[#name] = 1;                 \
		    MM_EXPEND(MM_GENERATE_MEMBER(##__VA_ARGS__))                           \
		}                                           \
	} __reflex_var__;                                               \
};\
name##::__ReflexRegister name##::__reflex_var__{};\
struct MM_CONCAT(__ReflexRegisterIgnoreParentheses, id) {

// #define MM_CLASS_FUNCTION(class_name, function_name)

#define MM_FIELD(class_name, field_type, field_name) reflection_field[std::string(#class_name) + "::" + #field_name] = 3;

#define MM_ARRAY(class_name, array_type, array_name) reflection_array[std::string(#class_name) + "::" + #array_name] = 4;

#define MM_CLASS_GENERATE(name, ...) \
	public:\
		MM_CLASS(__COUNTER__, name, ##__VA_ARGS__)