#pragma once


#include <string>


#include "runtime/core/reflection/accessor.h"

namespace MM {
class FieldAccessor;
class ArrayAccessor;

struct FieldList {
  std::vector<FieldAccessor> field_accessors;
  std::vector<ArrayAccessor> array_accessors;
};

class MetaType {
 public:
  MetaType();
  ~MetaType();
  static MetaType NewMetaTypeFromName(const std::string&);

  std::string GetTypeName();
  FieldList GetFieldList();
  std::vector<MetaType> GetBaseTypeMeta();
  bool GetFieldByName(const std::string& name, FieldAccessor& in);
  bool GetFieldByName(const std::string& name, ArrayAccessor& in);


  
 private:
  std::string type_name_;
};

}  // namespace MM