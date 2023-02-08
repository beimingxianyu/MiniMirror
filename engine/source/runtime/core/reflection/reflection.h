#pragma once

#include <unordered_map>
#include <string>

#include "runtime/core/reflection/macro.h"
#include "runtime/core/reflection/accessor.h"
#include "runtime/core/reflection/meta_type.h"


std::unordered_map<std::string, MM::ClassFunctionTuple> g_class_reflection_map;

std::unordered_map<std::string, MM::FieldFunctionTuple> g_field_reflection_map;

std::unordered_map<std::string, MM::ArrayFunctionTuple> g_array_reflection_map;


