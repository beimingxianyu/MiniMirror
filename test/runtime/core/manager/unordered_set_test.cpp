//
// Created by beimingxianyu on 23-6-9.
//
#include <gtest/gtest.h>

#include "runtime/core/manager/ManagedObjectUnorderedSet.h"

TEST(manager, unordered_set) {
  MM::Manager::ManagedObjectUnorderedSet<std::string> set_data, set_data2;

  auto handler1 = set_data.AddObject(std::string("2")).Exception();
  EXPECT_EQ(handler1.Success(), true);
  auto handler2 = set_data.AddObject(std::string("2")).Exception();
  EXPECT_EQ(handler2.Success(), false);
  EXPECT_EQ(handler2.GetError().GetErrorCode(),
            MM::ErrorCode::OPERATION_NOT_SUPPORTED);
  ASSERT_EQ(handler2.GetResult().IsValid(), false);
  handler2.GetResult() = handler1.GetResult();
  EXPECT_EQ(
      handler1.GetResult().GetObject() == handler2.GetResult().GetObject(),
      true);
  EXPECT_EQ(handler1.GetResult().GetObject(), std::string("2"));
  EXPECT_EQ(handler2.GetResult().GetObject(), std::string("2"));
  EXPECT_EQ(set_data.GetSize(), 1);
  auto handler4 = set_data.AddObject(std::string("3")).Exception();
  EXPECT_EQ(handler4.Success(), true);
  auto handler3 = set_data.AddObject(std::string("2")).Exception();
  EXPECT_EQ(handler3.Success(), true);
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
  EXPECT_EQ(
      handler1.GetResult().GetObject() == handler2.GetResult().GetObject(),
      true);
  EXPECT_EQ(handler1.GetResult().GetObject(), std::string("2"));
  EXPECT_EQ(handler2.GetResult().GetObject(), std::string("2"));

  handler1.GetResult().Release();
  handler2.GetResult().Release();
  handler3.GetResult().Release();
  handler4.GetResult().Release();

  EXPECT_EQ(set_data2.GetSize(), 0);
}

#define INSERT_COUNT 50000

void InsertString2(
    std::vector<MM::Manager::ManagedObjectUnorderedSet<
        std::string>::HandlerType>& handlers,
    MM::Manager::ManagedObjectUnorderedSet<std::string>& set_data,
    std::uint32_t start) {
  for (std::uint32_t i = start; i != start + INSERT_COUNT; ++i) {
    auto handler = set_data.AddObject(std::to_string(i)).Exception();
    ASSERT_EQ(handler.Success(), true);
    handlers.emplace_back(std::move(handler.GetResult()));
    ASSERT_EQ(set_data.Have(std::to_string(i)), true);
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

  ASSERT_EQ(multi_set_data1.IsRelationshipContainer(), false);
  ASSERT_EQ(multi_set_data1.IsMultiContainer(), true);
  auto handler1 = multi_set_data1.AddObject(std::string("1")).Exception();
  ASSERT_EQ(handler1.Success(), true);
  auto handler2 = multi_set_data1.AddObject(std::string("2")).Exception();
  ASSERT_EQ(handler2.Success(), true);
  auto handler3 = multi_set_data1.AddObject(std::string("1")).Exception();
  ASSERT_EQ(handler3.Success(), true);
  ASSERT_EQ(multi_set_data1.GetSize(), 3);
  auto handler4 = multi_set_data1.GetObject(std::string("1")).Exception();
  ASSERT_EQ(handler4.Success(), true);
  auto handler5 = multi_set_data1.GetObject(std::string("1")).Exception();
  ASSERT_EQ(handler5.Success(), true);
  ASSERT_EQ(multi_set_data1.GetSize(), 3);
  ASSERT_EQ(handler1.GetResult().GetObject(), std::to_string(1));
  ASSERT_EQ(handler2.GetResult().GetObject(), std::to_string(2));
  ASSERT_EQ(handler3.GetResult().GetObject(), std::to_string(1));
  ASSERT_EQ(handler4.GetResult().GetObject(), std::to_string(1));
  ASSERT_EQ(handler5.GetResult().GetObject(), std::to_string(1));
  auto handlers =
      multi_set_data1.GetObject(std::string("1"), MM::st_get_multiply_object);
  ASSERT_EQ(handlers.Success(), true);
  ASSERT_EQ(handlers.GetResult().size(), 2);
  ASSERT_EQ(multi_set_data1.Have(std::string("1")), true);
  ASSERT_EQ(multi_set_data1.Have(std::string("2")), true);
  ASSERT_EQ(multi_set_data1.Have(std::string("3")), false);
  ASSERT_EQ(multi_set_data1.GetSize(std::string("1")), 2);
  ASSERT_EQ(multi_set_data1.GetSize(std::string("2")), 1);
  ASSERT_EQ(multi_set_data1.GetSize(std::string("3")), 0);
  ASSERT_EQ(multi_set_data1.GetUseCount(std::string("1")), 4);
  auto handler6 = multi_set_data1.GetObject(std::string("1")).Exception();
  ASSERT_EQ(handler6.Success(), true);
  auto use_counts1 =
      multi_set_data1.GetUseCount(std::string("1"), MM::st_get_multiply_object);
  ASSERT_EQ(use_counts1.Success(), true);
  ASSERT_EQ(use_counts1.GetResult().size(), 2);
  auto use_counts2 =
      multi_set_data1.GetUseCount(std::string("2"), MM::st_get_multiply_object);
  ASSERT_EQ(use_counts2.Success(), true);
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
  handler6 = multi_set_data2.GetObject(std::string("1")).Exception();
  ASSERT_EQ(handler6.Success(), true);
  ASSERT_EQ(handler6.GetResult().IsValid(), true);
  ASSERT_EQ(handler6.GetResult().GetUseCount(), 5);
  ASSERT_EQ(multi_set_data2.GetUseCount(std::string("1")), 5);
  auto use_counts3 =
      multi_set_data2.GetUseCount(std::string("1"), MM::st_get_multiply_object);
  auto use_counts4 =
      multi_set_data2.GetUseCount(std::string("2"), MM::st_get_multiply_object);
  ASSERT_EQ(use_counts3.size(), 2);
  ASSERT_EQ(use_counts4.size(), 1);
  ASSERT_EQ(multi_set_data1.GetSize(), 0);
  ASSERT_EQ(multi_set_data2.GetSize(), 3);

  handler1.GetResult().Release();
  handler2.GetResult().Release();
  handler3.GetResult().Release();
  handler4.GetResult().Release();
  handler5.GetResult().Release();
  handler6.GetResult().Release();
  handlers.GetResult().clear();
}

void MultiInsertString(
    std::vector<MM::Manager::ManagedObjectUnorderedMultiSet<
        std::string>::HandlerType>& handlers,
    MM::Manager::ManagedObjectUnorderedMultiSet<std::string>& map_data,
    std::uint32_t start) {
  for (std::uint32_t i = start; i != start + INSERT_COUNT; ++i) {
    std::uint32_t temp;
    auto handler = map_data.AddObject(std::to_string(i));
    ASSERT_EQ(handler.Success(), true);
    handlers.emplace_back(std::move(handler.GetResult()));
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