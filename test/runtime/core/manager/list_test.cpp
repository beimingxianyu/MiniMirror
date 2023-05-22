//
// Created by beimingxianyu on 23-5-21.
//

#include <gtest/gtest.h>

#include "runtime/core/manager/ManagedObjectList.h"

TEST(manager, list) {
  MM::Manager::ManagedObjectList<int> list_data;
  MM::Manager::ManagedObjectList<int>::HandlerType* handler1 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler2 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler3 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler4 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler5 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler6 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler7 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler8 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();
  MM::Manager::ManagedObjectList<int>::HandlerType* handler9 =
      new MM::Manager::ManagedObjectList<int>::HandlerType();

  EXPECT_EQ(list_data.GetSize(), 0);
  EXPECT_EQ(list_data.AddObject(1, *handler1), MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(list_data.AddObject(1, *handler1), MM::ExecuteResult::SUCCESS);
}