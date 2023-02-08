#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

#include "json11.hpp"
#include "meta_type.h"

//std::unordered_map<std::string, int> reflection_class;
//
//std::unordered_map<std::string, int> reflection_method;
//
//std::unordered_map<std::string, int> reflection_field;

namespace MM {
    typedef std::function<MetaType* (void*)>        GetMeta;

    typedef std::function<void(void*, void*)>       SetFuncion;
    typedef std::function<void* (void*)>            GetFuncion;
    typedef std::function<const char* ()>           GetNameFuncion;
    typedef std::function<void(int, void*, void*)>  SetArrayFunc;
    typedef std::function<void* (int, void*)>       GetArrayFunc;
    typedef std::function<int(void*)>               GetSizeFunc;
    typedef std::function<bool()>                   GetBoolFunc;

    typedef std::function<void* (const json11::Json&)>                          ConstructorWithPJson;
    typedef std::function<json11::Json(void*)>                                  WriteToPJson;
    typedef std::function<std::vector<std::string>(void*)>                      GetBaseClassListFunc;

    typedef std::tuple<SetFuncion, GetFuncion, GetNameFuncion, GetNameFuncion, GetNameFuncion, GetBoolFunc, GetMeta>
        FieldFunctionTuple;
    typedef std::tuple<GetBaseClassListFunc, ConstructorWithPJson, WriteToPJson, GetNameFuncion, GetMeta>
        ClassFunctionTuple;
    typedef std::tuple<SetArrayFunc, GetArrayFunc, GetSizeFunc, GetNameFuncion, GetNameFuncion, GetMeta>
		ArrayFunctionTuple;
}
