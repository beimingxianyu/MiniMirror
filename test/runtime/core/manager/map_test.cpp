//
// Created by beimingxianyu on 23-5-24.
//
#include <gtest/gtest.h>

#include "runtime/core/manager/ManagedObjectMap.h"

TEST(manager, map) {
  MM::Manager::ManagedObjectMap<int, std::string> map_data, map_data2;

  auto handler1 = map_data.AddObject(1, std::string("2")).Exception();
  EXPECT_EQ(handler1.Success(), true);
  auto handler2 = map_data.AddObject(1, std::string("3")).Exception();
  EXPECT_EQ(handler2.Success(), true);
  EXPECT_EQ(
      handler1.GetResult().GetObject() == handler2.GetResult().GetObject(),
      true);
  EXPECT_EQ(handler1.GetResult().GetObject(), std::string("2"));
  EXPECT_EQ(handler2.GetResult().GetObject(), std::string("2"));
  EXPECT_EQ(map_data.GetSize(), 1);
  auto handler4 = map_data.AddObject(2, std::string("3")).Exception();
  EXPECT_EQ(handler4.Success(), true);
  auto handler3 = map_data.GetObject(1).Exception();
  EXPECT_EQ(handler3.Success(), true);
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
  EXPECT_EQ(
      handler1.GetResult().GetObject() == handler2.GetResult().GetObject(),
      true);
  EXPECT_EQ(handler1.GetResult().GetObject(), std::string("2"));
  EXPECT_EQ(handler2.GetResult().GetObject(), std::string("2"));

  handler1.GetResult().Release();
  handler2.GetResult().Release();
  handler3.GetResult().Release();
  handler4.GetResult().Release();

  EXPECT_EQ(map_data2.GetSize(), 0);
}

#define INSERT_COUNT 500000

void InsertString(
    std::vector<MM::Manager::ManagedObjectMap<
        std::uint32_t, std::string>::HandlerType>& handlers,
    MM::Manager::ManagedObjectMap<std::uint32_t, std::string>& map_data,
    std::uint32_t start) {
  for (std::uint32_t i = start; i != start + INSERT_COUNT; ++i) {
    auto handler = map_data.AddObject(i, std::to_string(i)).Exception();
    EXPECT_EQ(handler.Success(), true);
    handlers.emplace_back(std::move(handler.GetResult()));
    EXPECT_EQ(map_data.Have(i), true);
    EXPECT_EQ(map_data.GetSize(i), 1);
  }
}

TEST(manager, map_thread) {
  MM::Manager::ManagedObjectMap<std::uint32_t, std::string> map_data1,
      map_data2;
  std::vector<std::vector<
      MM::Manager::ManagedObjectMap<std::uint32_t, std::string>::HandlerType>>
      handlers_vector(9);
  std::vector<std::thread> threads;
  InsertString(handlers_vector[0], map_data1, 3 * INSERT_COUNT);
  ASSERT_EQ(map_data1.GetSize(), INSERT_COUNT);

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back(
          [&handlers_vector]() { handlers_vector[0].clear(); });
    }
    threads.emplace_back(InsertString, std::ref(handlers_vector[i + 1]),
                         std::ref(map_data1), i * INSERT_COUNT / 2);
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(map_data1.GetSize(), 9 * INSERT_COUNT / 2);

  threads.clear();

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 3) {
      map_data2 = std::move(map_data1);
    }
    threads.emplace_back([&deleted_vector = handlers_vector[i + 1]]() {
      deleted_vector.clear();
    });
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(map_data1.GetSize(), 0);
  ASSERT_EQ(map_data2.GetSize(), 0);
}

TEST(manager, multimap) {
  MM::Manager::ManagedObjectMultiMap<std::string, int> multi_map_data1,
      multi_map_data2;

  ASSERT_EQ(multi_map_data1.IsRelationshipContainer(), true);
  ASSERT_EQ(multi_map_data1.IsMultiContainer(), true);
  auto handler1 = multi_map_data1.AddObject(std::string("1"), 1).Exception();
  ASSERT_EQ(handler1.Success(), true);
  auto handler2 = multi_map_data1.AddObject(std::string("2"), 1).Exception();
  ASSERT_EQ(handler2.Success(), true);
  auto handler3 = multi_map_data1.AddObject(std::string("1"), 2).Exception();
  ASSERT_EQ(handler3.Success(), true);
  ASSERT_EQ(multi_map_data1.GetSize(), 3);
  auto handler4 = multi_map_data1.GetObject(std::string("1"), 1).Exception();
  ASSERT_EQ(handler4.Success(), true);
  auto handler5 = multi_map_data1.GetObject(std::string("1"), 2).Exception();
  ASSERT_EQ(handler5.Success(), true);
  ASSERT_EQ(multi_map_data1.GetSize(), 3);
  ASSERT_EQ(handler1.GetResult().GetObject(), 1);
  ASSERT_EQ(handler2.GetResult().GetObject(), 1);
  ASSERT_EQ(handler3.GetResult().GetObject(), 2);
  ASSERT_EQ(handler4.GetResult().GetObject(), 1);
  ASSERT_EQ(handler5.GetResult().GetObject(), 2);
  auto handlers =
      multi_map_data1.GetObject(std::string("1"), MM::st_get_multiply_object);
  ASSERT_EQ(multi_map_data1.Have(std::string("1")), true);
  ASSERT_EQ(multi_map_data1.Have(std::string("2")), true);
  ASSERT_EQ(multi_map_data1.Have(std::string("3")), false);
  ASSERT_EQ(multi_map_data1.GetSize(std::string("1")), 2);
  ASSERT_EQ(multi_map_data1.GetSize(std::string("2")), 1);
  ASSERT_EQ(multi_map_data1.GetSize(std::string("3")), 0);
  ASSERT_EQ(multi_map_data1.GetUseCount(std::string("1")), 3);
  auto handler6 = multi_map_data1.GetObject(std::string("1"), 1).Exception();
  ASSERT_EQ(handler6.Success(), true);
  ASSERT_EQ(multi_map_data1.GetUseCount(std::string("1"), 1), 4);
  ASSERT_EQ(multi_map_data1.GetUseCount(std::string("1"), 2), 3);
  ASSERT_EQ(multi_map_data1.GetUseCount(std::string("1"),
                                        handler6.GetResult().GetUseCountPtr()),
            4);
  ASSERT_EQ(multi_map_data1.GetUseCount(std::string("1"),
                                        handler5.GetResult().GetUseCountPtr()),
            3);
  auto use_counts1 =
      multi_map_data1.GetUseCount(std::string("1"), MM::st_get_multiply_object)
          .Exception();
  auto use_counts2 =
      multi_map_data1.GetUseCount(std::string("2"), MM::st_get_multiply_object)
          .Exception();
  ASSERT_EQ(use_counts1.Success(), true);
  ASSERT_EQ(use_counts2.Success(), true);
  ASSERT_EQ(use_counts1.GetResult().size(), 2);
  ASSERT_EQ(use_counts2.GetResult().size(), 1);

  multi_map_data2 = std::move(multi_map_data1);
  ASSERT_EQ(multi_map_data1.GetSize(), 0);
  ASSERT_EQ(multi_map_data2.Have(std::string("1")), true);
  ASSERT_EQ(multi_map_data2.Have(std::string("2")), true);
  ASSERT_EQ(multi_map_data2.Have(std::string("3")), false);
  ASSERT_EQ(multi_map_data2.GetSize(std::string("1")), 2);
  ASSERT_EQ(multi_map_data2.GetSize(std::string("2")), 1);
  ASSERT_EQ(multi_map_data2.GetSize(std::string("3")), 0);
  ASSERT_EQ(multi_map_data2.GetUseCount(std::string("1")), 4);

  handler6 = multi_map_data2.GetObject(std::string("1"), 1).Exception();
  ASSERT_EQ(handler6.Success(), true);
  ASSERT_EQ(handler6.GetResult().IsValid(), true);
  ASSERT_EQ(handler6.GetResult().GetUseCount(), 4);
  ASSERT_EQ(multi_map_data2.GetUseCount(std::string("1"), 1), 4);
  ASSERT_EQ(multi_map_data2.GetUseCount(std::string("1"), 2), 3);
  ASSERT_EQ(multi_map_data2.GetUseCount(std::string("1"),
                                        handler6.GetResult().GetUseCountPtr()),
            4);
  ASSERT_EQ(multi_map_data2.GetUseCount(std::string("1"),
                                        handler5.GetResult().GetUseCountPtr()),
            3);
  auto use_counts3 =
      multi_map_data2.GetUseCount(std::string("1"), MM::st_get_multiply_object)
          .Exception();
  ASSERT_EQ(use_counts3.Success(), true);
  auto use_counts4 =
      multi_map_data2.GetUseCount(std::string("2"), MM::st_get_multiply_object)
          .Exception();
  ASSERT_EQ(use_counts4.Success(), true);
  ASSERT_EQ(use_counts3.GetResult().size(), 2);
  ASSERT_EQ(use_counts4.GetResult().size(), 1);
}

void MultiInsertString(
    std::vector<MM::Manager::ManagedObjectMultiMap<
        std::string, std::uint32_t>::HandlerType>& handlers,
    MM::Manager::ManagedObjectMultiMap<std::string, std::uint32_t>& map_data,
    std::uint32_t start) {
  for (std::uint32_t i = start; i != start + INSERT_COUNT; ++i) {
    std::uint32_t temp;
    auto handler =
        map_data.AddObject(std::to_string(i), std::move(i)).Exception();
    ASSERT_EQ(handler.Success(), true);
    handlers.emplace_back(std::move(handler.GetResult()));
    EXPECT_EQ(map_data.Have(std::to_string(i)), true);
  }
}

TEST(manager, multimap_thread) {
  MM::Manager::ManagedObjectMultiMap<std::string, std::uint32_t>
      multi_map_data1, multi_map_data2;
  std::vector<std::vector<MM::Manager::ManagedObjectMultiMap<
      std::string, std::uint32_t>::HandlerType>>
      handlers_vector(9);
  std::vector<std::thread> threads;
  MultiInsertString(handlers_vector[0], multi_map_data1, 0);
  ASSERT_EQ(multi_map_data1.GetSize(), INSERT_COUNT);

  for (uint32_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back(
          [&handlers_vector]() { handlers_vector[0].clear(); });
    }

    threads.emplace_back(MultiInsertString, std::ref(handlers_vector[i + 1]),
                         std::ref(multi_map_data1), i * INSERT_COUNT / 2);
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(multi_map_data1.GetSize(), 8 * INSERT_COUNT);

  threads.clear();

  for (std::uint32_t i = 0; i != 8; ++i) {
    if (i == 3) {
      multi_map_data2 = std::move(multi_map_data1);
    }
    threads.emplace_back([&deleted_vector = handlers_vector[i + 1]]() {
      deleted_vector.clear();
    });
  }

  for (auto& th : threads) {
    th.join();
  }

  EXPECT_EQ(multi_map_data1.GetSize(), 0);
  EXPECT_EQ(multi_map_data2.GetSize(), 0);
}