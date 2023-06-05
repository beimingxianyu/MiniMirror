#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "utils/ConcurrentHashTable.h"
#include "utils/marco.h"
#include "utils/uuid.h"

TEST(Utils, UUID) {
  std::uint64_t clock1 = 0x0123456789abcdef;
  std::uint64_t clock2 = 0xfedcba9876543210;
  std::uint64_t clock3 = 0xabcdef0123456789;
  std::uint64_t clock4 = 0x0123456789fedcba;
  std::uint64_t mac_address = 0x999999999999;

  MM::Utils::UUID uuid1(clock1, mac_address);
  MM::Utils::UUID uuid2(clock2, mac_address);
  MM::Utils::UUID uuid3(clock3, mac_address);
  MM::Utils::UUID uuid4(clock4, mac_address);

  std::string post_string;
  std::stringstream ssm;

  EXPECT_EQ(uuid1.ToString(),
            std::string("89abcdef-4567-1123-0001-999999999999"));
  EXPECT_EQ(uuid2.ToString(),
            std::string("76543210-ba98-1edc-0002-999999999999"));
  EXPECT_EQ(uuid3.ToString(),
            std::string("23456789-ef01-1bcd-0003-999999999999"));
  EXPECT_EQ(uuid4.ToString(),
            std::string("89fedcba-4567-1123-0004-999999999999"));

  // Uniqueness testing
  std::unordered_set<MM::Utils::UUID> all_uuid;
  for (std::uint32_t i = 0; i < 10000000; ++i) {
    MM::Utils::UUID new_uuid;
    EXPECT_EQ(all_uuid.count(new_uuid), 0);
    all_uuid.insert(new_uuid);
  }
}

TEST(Utils, ConcurrentHashTable_set) {
  MM::Utils::ConcurrentSet<std::string> concurrent_set, concurrent_set2;
  ASSERT_EQ(concurrent_set.Empty(), true);
  ASSERT_EQ(concurrent_set.BucketCount(), 131);
  ASSERT_EQ(concurrent_set.GetLoadFactor(), 0.75);
  concurrent_set.SetLoadFactor(0.76);
  ASSERT_EQ(concurrent_set.GetLoadFactor(), 0.76);
  ASSERT_EQ(concurrent_set.Emplace("Asset1").first, std::string("Asset1"));
  ASSERT_EQ(concurrent_set.Size(), 1);
  ASSERT_EQ(concurrent_set.Insert(std::string("Assert3")).first,
            std::string("Assert3"));
  ASSERT_EQ(concurrent_set.Size(), 2);
  std::string s1 = "Assert5";
  ASSERT_EQ(concurrent_set.Insert(s1).first, s1);
  ASSERT_EQ(concurrent_set.Insert(s1).second, false);
  ASSERT_EQ(concurrent_set.Contains("Assert2"), false);
  ASSERT_EQ(concurrent_set.Contains("Assert3"), true);
  ASSERT_EQ(concurrent_set.Contains("Assert5"), true);
  ASSERT_EQ(concurrent_set.Count("Assert2"), 0);
  ASSERT_EQ(concurrent_set.Count("Assert3"), 1);
  ASSERT_EQ(concurrent_set.Count("Assert5"), 1);
  ASSERT_EQ(concurrent_set.Find("Assert2"), nullptr);
  ASSERT_EQ(*concurrent_set.Find("Assert3"), std::string("Assert3"));
  ASSERT_EQ(*concurrent_set.Find("Assert5"), std::string("Assert5"));
  ASSERT_EQ(concurrent_set.Empty(), false);
  auto equal_vector1 = concurrent_set.EqualRange("Assert5");
  auto equal_vector2 = concurrent_set.EqualRange("Assert2");
  ASSERT_EQ(equal_vector1.size(), 1);
  ASSERT_EQ(equal_vector1[0], concurrent_set.Find("Assert5"));
  ASSERT_EQ(equal_vector2.size(), 0);
  for (int i = 0; i != 297; ++i) {
    if (concurrent_set.Size() == 100) {
      ASSERT_EQ(concurrent_set.Emplace(std::to_string(i)).second, true);
      ASSERT_EQ(concurrent_set.BucketCount(), 263);
      continue;
    }

    ASSERT_EQ(concurrent_set.Emplace(std::to_string(i)).second, true);
  }

  ASSERT_EQ(concurrent_set.Size(), 300);
  ASSERT_EQ(concurrent_set.BucketCount(), 541);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_set.Find(std::to_string(i)), std::to_string(i));
  }
  concurrent_set2 = concurrent_set;
  ASSERT_EQ(concurrent_set2.Size(), 300);
  ASSERT_EQ(concurrent_set2.BucketCount(), 541);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_set2.Find(std::to_string(i)), std::to_string(i));
  }

  MM::Utils::ConcurrentSet<std::string> concurrent_set3(concurrent_set),
      concurrent_set4(std::move(concurrent_set2));
  ASSERT_EQ(concurrent_set2.Size(), 0);
  ASSERT_EQ(concurrent_set2.BucketCount(), 131);
  ASSERT_EQ(concurrent_set3.Size(), 300);
  ASSERT_EQ(concurrent_set3.BucketCount(), 541);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_set3.Find(std::to_string(i)), std::to_string(i));
  }
  ASSERT_EQ(concurrent_set4.Size(), 300);
  ASSERT_EQ(concurrent_set4.BucketCount(), 541);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_set4.Find(std::to_string(i)), std::to_string(i));
  }
}

TEST(Utils, ConcurrentHashTable_multi_set) {
  MM::Utils::ConcurrentMultiSet<std::string> concurrent_multiset,
      concurrent_multiset2;
  ASSERT_EQ(concurrent_multiset.Empty(), true);
  ASSERT_EQ(concurrent_multiset.BucketCount(), 131);
  ASSERT_EQ(concurrent_multiset.GetLoadFactor(), 0.75);
  concurrent_multiset.SetLoadFactor(0.76);
  ASSERT_EQ(concurrent_multiset.GetLoadFactor(), 0.76);
  ASSERT_EQ(concurrent_multiset.Emplace("Asset1").first, std::string("Asset1"));
  ASSERT_EQ(concurrent_multiset.Size(), 1);
  ASSERT_EQ(concurrent_multiset.Insert(std::string("Assert3")).first,
            std::string("Assert3"));
  ASSERT_EQ(concurrent_multiset.Size(), 2);
  std::string s1 = "Assert5";
  ASSERT_EQ(concurrent_multiset.Insert(s1).first, s1);
  ASSERT_EQ(concurrent_multiset.Insert(s1).second, true);
  ASSERT_EQ(concurrent_multiset.Contains("Assert2"), false);
  ASSERT_EQ(concurrent_multiset.Contains("Assert3"), true);
  ASSERT_EQ(concurrent_multiset.Contains("Assert5"), true);
  ASSERT_EQ(concurrent_multiset.Count("Assert2"), 0);
  ASSERT_EQ(concurrent_multiset.Count("Assert3"), 1);
  ASSERT_EQ(concurrent_multiset.Count("Assert5"), 2);
  ASSERT_EQ(concurrent_multiset.Find("Assert2"), nullptr);
  ASSERT_EQ(*concurrent_multiset.Find("Assert3"), std::string("Assert3"));
  ASSERT_EQ(*concurrent_multiset.Find("Assert5"), std::string("Assert5"));
  ASSERT_EQ(concurrent_multiset.Empty(), false);
  auto equal_vector1 = concurrent_multiset.EqualRange("Assert5");
  auto equal_vector2 = concurrent_multiset.EqualRange("Assert2");
  ASSERT_EQ(equal_vector1.size(), 2);
  ASSERT_EQ(equal_vector1[0], concurrent_multiset.Find("Assert5"));
  ASSERT_EQ(equal_vector2.size(), 0);
  for (int i = 0; i != 297; ++i) {
    if (concurrent_multiset.Size() == 100) {
      ASSERT_EQ(concurrent_multiset.Emplace(std::to_string(i)).second, true);
      ASSERT_EQ(concurrent_multiset.Emplace(std::to_string(i)).second, true);
      ASSERT_EQ(concurrent_multiset.BucketCount(), 263);
      continue;
    }

    ASSERT_EQ(concurrent_multiset.Emplace(std::to_string(i)).second, true);
    ASSERT_EQ(concurrent_multiset.Emplace(std::to_string(i)).second, true);
  }

  ASSERT_EQ(concurrent_multiset.Size(), 598);
  ASSERT_EQ(concurrent_multiset.BucketCount(), 1087);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_multiset.Find(std::to_string(i)), std::to_string(i));
  }
  concurrent_multiset2 = concurrent_multiset;
  ASSERT_EQ(concurrent_multiset2.Size(), 598);
  ASSERT_EQ(concurrent_multiset2.BucketCount(), 1087);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_multiset2.Find(std::to_string(i)), std::to_string(i));
  }

  MM::Utils::ConcurrentMultiSet<std::string> concurrent_multiset3(
      concurrent_multiset),
      concurrent_multiset4(std::move(concurrent_multiset2));
  ASSERT_EQ(concurrent_multiset2.Size(), 0);
  ASSERT_EQ(concurrent_multiset2.BucketCount(), 131);
  ASSERT_EQ(concurrent_multiset3.Size(), 598);
  ASSERT_EQ(concurrent_multiset3.BucketCount(), 1087);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_multiset3.Find(std::to_string(i)), std::to_string(i));
  }
  ASSERT_EQ(concurrent_multiset4.Size(), 598);
  ASSERT_EQ(concurrent_multiset4.BucketCount(), 1087);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(*concurrent_multiset4.Find(std::to_string(i)), std::to_string(i));
  }
  concurrent_multiset3.Clear();
  concurrent_multiset4.Clear();
  ASSERT_EQ(concurrent_multiset3.Size(), 0);
  ASSERT_EQ(concurrent_multiset3.BucketCount(), 1087);
  ASSERT_EQ(concurrent_multiset4.Size(), 0);
  ASSERT_EQ(concurrent_multiset4.BucketCount(), 1087);
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(concurrent_multiset3.Find(std::to_string(i)), nullptr);
  }
  for (int i = 0; i != 297; ++i) {
    ASSERT_EQ(concurrent_multiset4.Find(std::to_string(i)), nullptr);
  }
}

TEST(Utils, Concurretn_map) {
  MM::Utils::ConcurrentMap<int, std::string> concurrent_map1, concurrent_map2;

  auto insert_result1 = concurrent_map1.Emplace(1, "insert1"),
       insert_result2 = concurrent_map1.Emplace(2, "insert2"),
       insert_result3 = concurrent_map1.Emplace(2, "Insert3");
  ASSERT_EQ(insert_result1.first.first, 1);
  ASSERT_EQ(insert_result1.first.second, std::string("insert1"));
  ASSERT_EQ(insert_result1.second, true);
  ASSERT_EQ(insert_result2.first.first, 2);
  ASSERT_EQ(insert_result2.first.second, std::string("insert2"));
  ASSERT_EQ(insert_result2.second, true);
  ASSERT_EQ(insert_result3.first.first, 2);
  ASSERT_EQ(insert_result3.first.second, std::string("insert2"));
  ASSERT_EQ(insert_result3.second, false);
  ASSERT_EQ(concurrent_map1.Contains(1), true);
  ASSERT_EQ(concurrent_map1.Contains(2), true);
  ASSERT_EQ(concurrent_map1.Contains(3), false);
  auto find_result1 = concurrent_map1.Find(1),
       find_result2 = concurrent_map1.Find(2),
       find_result3 = concurrent_map1.Find(3);
  ASSERT_EQ(find_result1->first, 1);
  ASSERT_EQ(find_result1->second, std::string("insert1"));
  ASSERT_EQ(find_result2->first, 2);
  ASSERT_EQ(find_result2->second, std::string("insert2"));
  ASSERT_EQ(find_result3, nullptr);
  ASSERT_EQ(concurrent_map1.Count(1), 1);
  ASSERT_EQ(concurrent_map1.Count(2), 1);
  ASSERT_EQ(concurrent_map1.Count(3), 0);
  auto equal_range1 = concurrent_map1.EqualRange(1),
       equal_range2 = concurrent_map1.EqualRange(2),
       equal_range3 = concurrent_map1.EqualRange(3);
  ASSERT_EQ(equal_range1.size(), 1);
  ASSERT_EQ((*equal_range1[0]).second, std::string("insert1"));
  ASSERT_EQ(equal_range2.size(), 1);
  ASSERT_EQ((*equal_range2[0]).second, std::string("insert2"));
  ASSERT_EQ(equal_range3.size(), 0);
}

struct TestClass {
  TestClass(uint64_t f_1, uint64_t f_2, uint64_t f_3)
      : f1(f_1), f2(f_2), f3(f_3) {}

  bool operator==(const TestClass& rhs) const {
    return f1 == rhs.f1 && f2 == rhs.f2 && f3 == rhs.f3;
  }
  bool operator!=(const TestClass& rhs) const { return !(rhs == *this); }

  std::uint64_t f1;
  std::uint64_t f2;
  std::uint64_t f3;
};

TEST(Utils, ConcurrentHashTable_multi_map) {
  MM::Utils::ConcurrentMultiMap<std::string, TestClass> concurrent_multi_map;

  for (std::uint64_t i = 0; i != 100; ++i) {
    for (std::uint64_t j = 0; j <= i; ++j) {
      auto insert_result =
          concurrent_multi_map.Emplace(std::to_string(i), TestClass{i, j, 1});
      ASSERT_EQ(insert_result.first.first, std::to_string(i));
      ASSERT_EQ(insert_result.first.second, TestClass(i, j, 1));
      ASSERT_EQ(insert_result.second, true);
    }
  }

  ASSERT_EQ(concurrent_multi_map.Size(), 5050);
  ASSERT_EQ(concurrent_multi_map.BucketCount(), 8731);

  for (std::uint64_t i = 0; i != 100; ++i) {
    auto equal_range = concurrent_multi_map.EqualRange(std::to_string(i));
    for (std::uint64_t j = i, k = 0; k <= i; --j, ++k) {
      ASSERT_EQ(equal_range[k]->first, std::to_string(i));
      ASSERT_EQ(equal_range[k]->second, TestClass(i, j, 1));
    }
  }

  ASSERT_EQ(concurrent_multi_map.Erase("fffffff"), 0);
  auto insert_result = concurrent_multi_map.Emplace("ffff", TestClass{1, 1, 1});
  ASSERT_EQ(concurrent_multi_map.Erase(&(insert_result.first)),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(concurrent_multi_map.Erase(nullptr),
            MM::Utils::ExecuteResult::INPUT_PARAMETERS_ARE_NOT_SUITABLE);
  ASSERT_EQ(concurrent_multi_map.Erase(
                reinterpret_cast<std::pair<const std::string, TestClass>*>(
                    0xf45664631)),
            MM::Utils::ExecuteResult::
                PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);

  for (std::uint64_t i = 0; i != 100; ++i) {
    ASSERT_EQ(concurrent_multi_map.Erase(std::to_string(i)), i + 1);
  }
  ASSERT_EQ(concurrent_multi_map.Size(), 0);
}

#define COUNT 100000
#define COUNT2 8

void InsertElement(MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data,
                   std::uint64_t start, std::uint64_t count,
                   std::uint64_t count2) {
  for (std::uint64_t i = start; i != start + count; ++i) {
    for (std::uint64_t j = 0; j != count2; ++j) {
      auto insert_result = data.Emplace(std::to_string(i), TestClass{i, j, 1});
      ASSERT_EQ(insert_result.first.first, std::to_string(i));
      ASSERT_EQ(insert_result.first.second, TestClass(i, j, 1));
      ASSERT_EQ(insert_result.second, true);
    }
  }
}

void InsertElement2(
    MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data,
    std::uint64_t start, std::uint64_t count, std::uint64_t count2,
    std::vector<std::pair<const std::string, TestClass>*>& address_vector) {
  for (std::uint64_t i = start; i != start + count; ++i) {
    for (std::uint64_t j = 0; j != count2; ++j) {
      auto insert_result = data.Emplace(std::to_string(i), TestClass{i, j, 1});
      ASSERT_EQ(insert_result.first.first, std::to_string(i));
      ASSERT_EQ(insert_result.first.second, TestClass(i, j, 1));
      ASSERT_EQ(insert_result.second, true);
    }
    auto equal_range = data.EqualRange(std::to_string(i));
    for (auto* address : equal_range) {
      address_vector.emplace_back(address);
    }
  }
}

void EraseElement(MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data1,
                  MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data2,
                  std::uint64_t start, std::uint64_t count) {
  for (std::uint64_t i = start; i != start + count; ++i) {
    if (data1.Erase(std::to_string(i)) == 0) {
      ASSERT_EQ(data2.Erase(std::to_string(i)), COUNT2);
    }
  }
}

void EraseElement2(
    MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data,
    std::vector<std::pair<const std::string, TestClass>*>& address_vector) {
  for (auto* address : address_vector) {
    auto erase_result = data.Erase(address);
    if (erase_result != MM::Utils::ExecuteResult::SUCCESS) {
      data.Erase(address);
    }
    ASSERT_EQ(erase_result, MM::Utils::ExecuteResult::SUCCESS);
  }
}

TEST(Utils, ConcurrentHashTable_multi_map2) {
  MM::Utils::ConcurrentMultiMap<std::string, TestClass> concurrent_multi_map1;
  std::vector<std::pair<const std::string, TestClass>*> address_vector;
  InsertElement2(concurrent_multi_map1, COUNT / 2, COUNT, COUNT2,
                 address_vector);
  ASSERT_EQ(concurrent_multi_map1.Size(), COUNT * COUNT2);
  for (std::uint64_t i = COUNT / 2; i != COUNT / 2 + COUNT; ++i) {
    auto equal_range = concurrent_multi_map1.EqualRange(std::to_string(i));
    ASSERT_EQ(equal_range.size(), COUNT2);
  }
  EraseElement2(concurrent_multi_map1, address_vector);
}

TEST(Utils, ConcurrentHashTable_thread) {
  MM::Utils::ConcurrentMultiMap<std::string, TestClass> concurrent_multi_map1,
      concurrent_multi_map2;
  std::vector<std::thread> threads;
  std::vector<std::pair<const std::string, TestClass>*> address_vector;

  InsertElement2(concurrent_multi_map1, COUNT / 2, COUNT, COUNT2,
                 address_vector);
  ASSERT_EQ(concurrent_multi_map1.Size(), COUNT * COUNT2);
  for (std::uint64_t i = COUNT / 2; i != COUNT / 2 + COUNT; ++i) {
    auto equal_range = concurrent_multi_map1.EqualRange(std::to_string(i));
    ASSERT_EQ(equal_range.size(), COUNT2);
  }

  for (std::uint64_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back(EraseElement2, std::ref(concurrent_multi_map1),
                           std::ref(address_vector));
    }

    threads.emplace_back(InsertElement, std::ref(concurrent_multi_map1),
                         COUNT / 8 * i, COUNT / 8, COUNT2);
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(concurrent_multi_map1.Size(), COUNT * COUNT2);
  for (std::uint64_t i = 0; i != COUNT; ++i) {
    auto equal_range = concurrent_multi_map1.EqualRange(std::to_string(i));
    for (std::uint64_t j = COUNT2, k = 0; k != COUNT2; --j, ++k) {
      ASSERT_EQ(equal_range[k]->first, std::to_string(i));
      ASSERT_EQ(equal_range[k]->second, TestClass(i, j, 1));
    }
  }

  threads.clear();
  for (std::uint64_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back([&concurrent_multi_map1, &concurrent_multi_map2]() {
        concurrent_multi_map2 = std::move(concurrent_multi_map1);
      });
    }

    threads.emplace_back(EraseElement, std::ref(concurrent_multi_map1),
                         std::ref(concurrent_multi_map2), COUNT / 8 * i,
                         COUNT / 8);
  }

  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(concurrent_multi_map1.Size(), 0);
  ASSERT_EQ(concurrent_multi_map2.Size(), 0);
}
