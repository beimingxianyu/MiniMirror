#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <cstddef>

#include "runtime/platform/cross_platform_header/cross_platform_header.h"


namespace MM{
namespace Reflection {
class Meta;
/**
 * \brief The database of all registered metadata.
 * \remark The std::pair.first is the typeid(T).hash_cond(), the
 * std::pair.second is the type name.
 */
extern std::unordered_map<std::size_t, std::string> g_type_database;

/**
 * \brief The Database of all registered metadata.
 */
extern std::unordered_map<std::string, std::shared_ptr<Meta>> g_meta_database;
}
}