#pragma once

#include <functional>
#include <unordered_set>

#include "runtime/core/manager/ManagedObjectTableBase.h"

namespace MM {
namespace Manager {
template <typename ObjectType, typename Hash = std::hash<ObjectType>,
          typename Equal = std::equal_to<ObjectType>,
          typename Allocator = std::allocator<ManagedObjectWrapper<ObjectType>>>
class ManagedObjectUnorderedSet
    : public ManagedObjectTableBase<ObjectType, ObjectType, ValueTrait> {
 public:
  using RelationshipContainerTrait = ValueTrait;
  using ThisType =
      ManagedObjectUnorderedSet<ObjectType, Hash, Equal, Allocator>;
  using BaseType = ManagedObjectTableBase<ObjectType, ObjectType,
                                          RelationshipContainerTrait>;
  using WrapperType = typename BaseType::WrapperType;
  using HandlerType = typename BaseType::HandlerType;
  using Container = std::unordered_set<
      ObjectType, typename WrapperType::template HashWrapper<Hash>,
      typename WrapperType::template EqualWrapper<Equal>, Allocator>;

 public:
  ManagedObjectUnorderedSet();
};

}  // namespace Manager
}  // namespace MM
