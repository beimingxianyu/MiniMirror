//
// Created by lsj on 24-3-2.
//
#include "runtime/core/reflection/database.h"

std::unordered_map<std::size_t, MM::Reflection::Meta*>&
MM::Reflection::GetMetaDatabase() {
  static std::unordered_map<std::size_t, Meta*> g_meta_database;

  return g_meta_database;
}