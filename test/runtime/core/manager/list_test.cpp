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

  EXPECT_EQ(list_data.IsRelationshipContainer(), false);
  EXPECT_EQ(list_data.IsMultiContainer(), true);
  EXPECT_EQ(list_data.GetSize(), 0);
  EXPECT_EQ(list_data.AddObject(1, *handler1), MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(list_data.AddObject(1, *handler1), MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(list_data.GetSize(), 1);
  EXPECT_EQ(list_data.GetUseCount(1, handler1->GetUseCountPtr()), 1);
  EXPECT_EQ(handler1->GetUseCount(), 1);
  EXPECT_EQ(list_data.AddObject(1, *handler2), MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(list_data.GetSize(), 2);
  *handler3 = *handler1;
  std::vector<std::uint32_t> use_count;
  list_data.GetUseCount(1, use_count);
  EXPECT_EQ(use_count.size(), 2);
  EXPECT_EQ(list_data.GetSize(), 2);
  std::vector<MM::Manager::ManagedObjectList<int>::HandlerType> handlers;
  for (int i = 0; i != 100; ++i) {
    MM::Manager::ManagedObjectList<int>::HandlerType new_handler;
    int temp = i;
    EXPECT_EQ(list_data.AddObject(std::move(temp), new_handler),
              MM::ExecuteResult::SUCCESS);
    handlers.emplace_back(std::move(new_handler));
  }
  EXPECT_EQ(list_data.GetSize(), 102);
  handlers.clear();
  EXPECT_EQ(list_data.GetSize(), 2);
  delete handler1;
  delete handler2;
  delete handler3;
}

#define COUNT_SIZE 1000
#define STEP_SIZE 250

void InsertObject2(
    MM::Manager::ManagedObjectList<int>& list_manager,
    std::vector<MM::Manager::ManagedObjectList<int>::HandlerType>& handlers) {
  for (int i = 0; i < COUNT_SIZE; ++i) {
    MM::Manager::ManagedObjectList<int>::HandlerType handler;
    int temp = i;
    EXPECT_EQ(list_manager.AddObject(std::move(temp), handler),
              MM::ExecuteResult::SUCCESS);
    handlers.emplace_back(std::move(handler));
  }
}

TEST(manager, list_thread) {
  MM::Manager::ManagedObjectList<int> list_manager, list_manager2;
  int size = 20;
  std::vector<std::vector<MM::Manager::ManagedObjectList<int>::HandlerType>>
      handlers_vector{static_cast<std::uint32_t>(size)};
  std::vector<std::thread> threads;

  for (int i = 0; i != size; ++i) {
    std::thread t(InsertObject2, std::ref(list_manager),
                  std::ref(handlers_vector[i]));
    threads.emplace_back(std::move(t));
  }

  for (int i = 0; i != size; ++i) {
    threads[i].join();
  }

  EXPECT_EQ(list_manager.GetSize(), size * COUNT_SIZE);

  threads.clear();
  for (int i = 0; i != size; ++i) {
    std::thread t(
        [&handlers_vector, &list_manager, &list_manager2, size](int i) {
          if (i == size / 2) {
            list_manager2 = std::move(list_manager);
          }
          for (const auto& handler : handlers_vector[i]) {
            EXPECT_EQ(handler.IsValid(), true);
          }
          for (auto& handler : handlers_vector[i]) {
            handler.Release();
          }
          for (const auto& handler : handlers_vector[i]) {
            EXPECT_EQ(handler.IsValid(), false);
          }
        },
        i);
    threads.emplace_back(std::move(t));
  }

  for (int i = 0; i != size; ++i) {
    threads[i].join();
  }

  EXPECT_EQ(list_manager2.GetSize(), 0);
  EXPECT_EQ(list_manager.GetSize(), 0);

  for (auto& handler : list_manager2.GetContainer()) {
    std::cout << "value:" << handler.GetObject() << '\t'
              << "use_count:" << handler.GetUseCount() << std::endl;
  }
}