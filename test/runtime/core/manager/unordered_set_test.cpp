//
// Created by beimingxianyu on 23-6-9.
//
#include <gtest/gtest.h>

#include "runtime/core/manager/ManagedObjectUnorderedSet.h"

TEST(manager, unordered_set) {
  MM::Manager::ManagedObjectUnorderedSet<std::string> set_data, set_data2;
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler1 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler2 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler3 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};
  MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType* handler4 =
      new MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType{};

  EXPECT_EQ(set_data.AddObject(std::string("2"), *handler1),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(set_data.AddObject(std::string("2"), *handler2),
            MM::ExecuteResult::OPERATION_NOT_SUPPORTED);
  ASSERT_EQ(handler2->IsValid(), false);
  *handler2 = *handler1;
  EXPECT_EQ(handler1->GetObject() == handler2->GetObject(), true);
  EXPECT_EQ(handler1->GetObject(), std::string("2"));
  EXPECT_EQ(handler2->GetObject(), std::string("2"));
  EXPECT_EQ(set_data.GetSize(), 1);
  EXPECT_EQ(set_data.AddObject(std::string("3"), *handler4),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(set_data.GetObject(std::string("2"), *handler3),
            MM::ExecuteResult::SUCCESS);
  std::string temp = std::to_string(2);
  EXPECT_EQ(set_data.Have(temp), true);
  EXPECT_EQ(set_data.Have(std::to_string(2)), true);
  EXPECT_EQ(set_data.Have(std::to_string(3)), true);
  EXPECT_EQ(set_data.Have(std::to_string(5)), false);
  EXPECT_EQ(set_data.GetUseCount(std::to_string(2)), 3);
  EXPECT_EQ(set_data.GetUseCount(std::to_string(3)), 1);
  EXPECT_EQ(set_data.GetUseCount(std::to_string(5)), 0);
  EXPECT_EQ(set_data.IsMultiContainer(), false);
  EXPECT_EQ(set_data.IsRelationshipContainer(), false);
  EXPECT_EQ(static_cast<void*>(*(set_data.GetThisPtrPtr())),
            static_cast<void*>(&set_data));
  EXPECT_EQ(set_data.IsValid(), true);

  set_data2 = std::move(set_data);

  EXPECT_EQ(set_data.IsValid(), false);
  EXPECT_EQ(set_data.GetThisPtrPtr(), nullptr);
  EXPECT_EQ(set_data2.Have(std::to_string(2)), true);
  EXPECT_EQ(set_data2.Have(std::to_string(3)), true);
  EXPECT_EQ(set_data2.Have(std::to_string(5)), false);
  EXPECT_EQ(set_data2.GetUseCount(std::to_string(2)), 3);
  EXPECT_EQ(set_data2.GetUseCount(std::to_string(3)), 1);
  EXPECT_EQ(set_data2.GetUseCount(std::to_string(5)), 0);
  EXPECT_EQ(set_data2.IsMultiContainer(), false);
  EXPECT_EQ(set_data2.IsRelationshipContainer(), false);
  EXPECT_EQ(handler1->GetObject() == handler2->GetObject(), true);
  EXPECT_EQ(handler1->GetObject(), std::string("2"));
  EXPECT_EQ(handler2->GetObject(), std::string("2"));

  handler1->Release();
  handler2->Release();
  handler3->Release();
  handler4->Release();

  EXPECT_EQ(set_data2.GetSize(), 0);

  delete handler1;
  delete handler2;
  delete handler3;
  delete handler4;
}

#define INSERT_COUNT 50000

void InsertString2(
    std::vector<MM::Manager::ManagedObjectUnorderedSet<
        std::string>::HandlerType>& handlers,
    MM::Manager::ManagedObjectUnorderedSet<std::string>& set_data,
    std::uint32_t start) {
  for (std::uint32_t i = start; i != start + INSERT_COUNT; ++i) {
    handlers.emplace_back();
    set_data.AddObject(std::to_string(i), handlers.back());
    ASSERT_EQ(set_data.Have(std::to_string(i)), true);
    if (set_data.GetSize(std::to_string(i)) == 2) {
      handlers.size();
    }
    ASSERT_EQ(set_data.GetSize(std::to_string(i)), 1);
  }
}

TEST(manager, unorderd_set_thread) {
  MM::Manager::ManagedObjectUnorderedSet<std::string> set_data1, set_data2;
  std::vector<std::vector<
      MM::Manager::ManagedObjectUnorderedSet<std::string>::HandlerType>>
      handlers_vector(9);
  std::vector<std::thread> threads;
  // InsertString(handlers_vector[0], map_data1, 3 * INSERT_COUNT);
  // ASSERT_EQ(map_data1.GetSize(), INSERT_COUNT);

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back(
          [&handlers_vector]() { handlers_vector[0].clear(); });
    }
    threads.emplace_back(InsertString2, std::ref(handlers_vector[i + 1]),
                         std::ref(set_data1), i * INSERT_COUNT / 2);
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(set_data1.GetSize(), 9 * INSERT_COUNT / 2);

  threads.clear();

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 3) {
      set_data2 = std::move(set_data1);
    }
    threads.emplace_back([&deleted_vector = handlers_vector[i + 1]]() {
      deleted_vector.clear();
    });
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(set_data1.GetSize(), 0);
  ASSERT_EQ(set_data2.GetSize(), 0);
}

TEST(manager, unorderd_multiset) {
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string> multi_set_data1,
      multi_set_data2;
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType
      handler1;
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType
      handler2;
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType
      handler3;
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType
      handler4;
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType
      handler5;
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType
      handler6;
  std::vector<
      MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType>
      handlers;

  ASSERT_EQ(multi_set_data1.IsRelationshipContainer(), false);
  ASSERT_EQ(multi_set_data1.IsMultiContainer(), true);
  ASSERT_EQ(multi_set_data1.AddObject(std::string("1"), handler1),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(multi_set_data1.AddObject(std::string("2"), handler2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(multi_set_data1.AddObject(std::string("1"), handler3),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(multi_set_data1.GetSize(), 3);
  ASSERT_EQ(multi_set_data1.GetObject(std::string("1"), handler4),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(multi_set_data1.GetObject(std::string("1"), handler5),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(multi_set_data1.GetSize(), 3);
  ASSERT_EQ(handler1.GetObject(), std::to_string(1));
  ASSERT_EQ(handler2.GetObject(), std::to_string(2));
  ASSERT_EQ(handler3.GetObject(), std::to_string(1));
  ASSERT_EQ(handler4.GetObject(), std::to_string(1));
  ASSERT_EQ(handler5.GetObject(), std::to_string(1));
  multi_set_data1.GetObject(std::string("1"), handlers);
  ASSERT_EQ(handlers.size(), 2);
  ASSERT_EQ(multi_set_data1.Have(std::string("1")), true);
  ASSERT_EQ(multi_set_data1.Have(std::string("2")), true);
  ASSERT_EQ(multi_set_data1.Have(std::string("3")), false);
  ASSERT_EQ(multi_set_data1.GetSize(std::string("1")), 2);
  ASSERT_EQ(multi_set_data1.GetSize(std::string("2")), 1);
  ASSERT_EQ(multi_set_data1.GetSize(std::string("3")), 0);
  ASSERT_EQ(multi_set_data1.GetUseCount(std::string("1")), 4);
  ASSERT_EQ(multi_set_data1.GetObject(std::string("1"), handler6),
            MM::ExecuteResult::SUCCESS);
  std::vector<std::uint32_t> use_counts1, use_counts2;
  multi_set_data1.GetUseCount(std::string("1"), use_counts1);
  multi_set_data1.GetUseCount(std::string("2"), use_counts2);
  ASSERT_EQ(use_counts1.size(), 2);
  ASSERT_EQ(use_counts2.size(), 1);

  multi_set_data2 = std::move(multi_set_data1);
  ASSERT_EQ(multi_set_data1.GetSize(), 0);
  ASSERT_EQ(multi_set_data2.Have(std::string("1")), true);
  ASSERT_EQ(multi_set_data2.Have(std::string("2")), true);
  ASSERT_EQ(multi_set_data2.Have(std::string("3")), false);
  ASSERT_EQ(multi_set_data2.GetSize(std::string("1")), 2);
  ASSERT_EQ(multi_set_data2.GetSize(std::string("2")), 1);
  ASSERT_EQ(multi_set_data2.GetSize(std::string("3")), 0);
  ASSERT_EQ(multi_set_data2.GetUseCount(std::string("1")), 5);
  ASSERT_EQ(multi_set_data2.GetObject(std::string("1"), handler6),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handler6.IsValid(), true);
  ASSERT_EQ(handler6.GetUseCount(), 5);
  ASSERT_EQ(multi_set_data2.GetUseCount(std::string("1")), 5);
  std::vector<std::uint32_t> use_counts3, use_counts4;
  multi_set_data2.GetUseCount(std::string("1"), use_counts3);
  multi_set_data2.GetUseCount(std::string("2"), use_counts4);
  ASSERT_EQ(use_counts3.size(), 2);
  ASSERT_EQ(use_counts4.size(), 1);
  ASSERT_EQ(multi_set_data1.GetSize(), 0);
  ASSERT_EQ(multi_set_data2.GetSize(), 3);

  handler1.Release();
  handler2.Release();
  handler3.Release();
  handler4.Release();
  handler5.Release();
  handler6.Release();
  handlers.clear();
}

void MultiInsertString(
    std::vector<MM::Manager::ManagedObjectUnorderedMultiSet<
        std::string>::HandlerType>& handlers,
    MM::Manager::ManagedObjectUnorderedMultiSet<std::string>& map_data,
    std::uint32_t start) {
  for (std::uint32_t i = start; i != start + INSERT_COUNT; ++i) {
    std::uint32_t temp;
    handlers.emplace_back();
    map_data.AddObject(std::to_string(i), handlers.back());
    EXPECT_EQ(map_data.Have(std::to_string(i)), true);
  }
}

TEST(manager, unorderd_multiset_thread) {
  MM::Manager::ManagedObjectUnorderedMultiSet<std::string> multi_set_data1,
      multi_set_data2;
  std::vector<std::vector<
      MM::Manager::ManagedObjectUnorderedMultiSet<std::string>::HandlerType>>
      handlers_vector(9);
  std::vector<std::thread> threads;
  MultiInsertString(handlers_vector[0], multi_set_data1, 0);
  ASSERT_EQ(multi_set_data1.GetSize(), INSERT_COUNT);

  for (uint32_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back(
          [&handlers_vector]() { handlers_vector[0].clear(); });
    }

    threads.emplace_back(MultiInsertString, std::ref(handlers_vector[i + 1]),
                         std::ref(multi_set_data1), i * INSERT_COUNT / 2);
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(multi_set_data1.GetSize(), 8 * INSERT_COUNT);

  threads.clear();

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 3) {
      multi_set_data2 = std::move(multi_set_data1);
    }
    threads.emplace_back([&deleted_vector = handlers_vector[i + 1]]() {
      deleted_vector.clear();
    });
  }

  for (auto& th : threads) {
    th.join();
  }

  EXPECT_EQ(multi_set_data1.GetSize(), 0);
  EXPECT_EQ(multi_set_data2.GetSize(), 0);
}