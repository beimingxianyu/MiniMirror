//
// Created by beimingxianyu on 23-6-9.
//
#include <gtest/gtest.h>

#include "runtime/core/manager/ManagedObjectUnorderedSet.h"

TEST(manager, unordered_set) {
  MM::Manager::ManagedObjectUnorderedSet<std::string> map_data, map_data2;
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler1 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler2 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler3 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler4 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};

  EXPECT_EQ(map_data.AddObject(std::string("2"), *handler1),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(map_data.AddObject(std::string("3"), *handler2),
            MM::ExecuteResult::OPERATION_NOT_SUPPORTED);
  ASSERT_EQ(handler2->IsValid(), false);
  *handler2 = *handler1;
  EXPECT_EQ(handler1->GetObject() == handler2->GetObject(), true);
  EXPECT_EQ(handler1->GetObject(), std::string("2"));
  EXPECT_EQ(handler2->GetObject(), std::string("2"));
  EXPECT_EQ(map_data.GetSize(), 1);
  EXPECT_EQ(map_data.AddObject(2, std::string("3"), *handler4),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(map_data.GetObject(1, *handler3), MM::ExecuteResult::SUCCESS);
  std::uint32_t temp = 2;
  EXPECT_EQ(map_data.Have(temp), true);
  EXPECT_EQ(map_data.Have(2), true);
  EXPECT_EQ(map_data.Have(1), true);
  EXPECT_EQ(map_data.Have(5), false);
  EXPECT_EQ(map_data.GetUseCount(1), 3);
  EXPECT_EQ(map_data.GetUseCount(2), 1);
  EXPECT_EQ(map_data.GetUseCount(5), 0);
  EXPECT_EQ(map_data.IsMultiContainer(), false);
  EXPECT_EQ(map_data.IsRelationshipContainer(), true);
  EXPECT_EQ(static_cast<void*>(*(map_data.GetThisPtrPtr())),
            static_cast<void*>(&map_data));
  EXPECT_EQ(map_data.IsValid(), true);

  map_data2 = std::move(map_data);

  EXPECT_EQ(map_data.IsValid(), false);
  EXPECT_EQ(map_data.GetThisPtrPtr(), nullptr);
  EXPECT_EQ(map_data2.Have(2), true);
  EXPECT_EQ(map_data2.Have(1), true);
  EXPECT_EQ(map_data2.Have(5), false);
  EXPECT_EQ(map_data2.GetUseCount(1), 3);
  EXPECT_EQ(map_data2.GetUseCount(2), 1);
  EXPECT_EQ(map_data2.GetUseCount(5), 0);
  EXPECT_EQ(map_data2.IsMultiContainer(), false);
  EXPECT_EQ(map_data2.IsRelationshipContainer(), true);
  EXPECT_EQ(handler1->GetObject() == handler2->GetObject(), true);
  EXPECT_EQ(handler1->GetObject(), std::string("2"));
  EXPECT_EQ(handler2->GetObject(), std::string("2"));

  handler1->Release();
  handler2->Release();
  handler3->Release();
  handler4->Release();

  EXPECT_EQ(map_data2.GetSize(), 0);

  delete handler1;
  delete handler2;
  delete handler3;
  delete handler4;
}

TEST(manager, unorderd_set_thread) {}

TEST(manager, unorderd_multiset) {}

TEST(manager, unorderd_multiset_thread) {}