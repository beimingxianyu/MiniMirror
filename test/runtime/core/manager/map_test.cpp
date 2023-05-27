//
// Created by beimingxianyu on 23-5-24.
//
#include <gtest/gtest.h>

#include "runtime/core/manager/ManagedObjectMap.h"

TEST(manager, map) {
  MM::Manager::ManagedObjectMap<int, int> map_data, map_data2;
  MM::Manager::ManagedObjectMap<int, int>::HandlerType* handler1 =
      new MM::Manager::ManagedObjectMap<int, int>::HandlerType{};
  MM::Manager::ManagedObjectMap<int, int>::HandlerType* handler2 =
      new MM::Manager::ManagedObjectMap<int, int>::HandlerType{};
  MM::Manager::ManagedObjectMap<int, int>::HandlerType* handler3 =
      new MM::Manager::ManagedObjectMap<int, int>::HandlerType{};
  MM::Manager::ManagedObjectMap<int, int>::HandlerType* handler4 =
      new MM::Manager::ManagedObjectMap<int, int>::HandlerType{};

  EXPECT_EQ(map_data.AddObject(1, 2, *handler1), MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(map_data.AddObject(1, 3, *handler2),
            MM::ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE);
  EXPECT_EQ(handler1->GetObject() == handler2->GetObject(), true);
  EXPECT_EQ(handler1->GetObject(), 2);
  EXPECT_EQ(handler2->GetObject(), 2);
  EXPECT_EQ(map_data.GetSize(), 1);
  EXPECT_EQ(map_data.AddObject(2, 3, *handler4), MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(map_data.GetObject(1, *handler3), MM::ExecuteResult::SUCCESS);
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
  EXPECT_EQ(handler1->GetObject(), 2);
  EXPECT_EQ(handler2->GetObject(), 2);

  handler1->Release();
  handler2->Release();
  handler3->Release();
  handler4->Release();

  EXPECT_EQ(map_data2.GetSize(), 0);
}

TEST(manager, map_thread) {
  MM::Manager::ManagedObjectMap<int, std::string> map_data;
}

TEST(manager, multimap) {
  MM::Manager::ManagedObjectMultiMap<int, int> multi_map_data;
}

TEST(manager, multimap_thread) {}