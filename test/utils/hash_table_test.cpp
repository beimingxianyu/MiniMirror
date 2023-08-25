//
// Created by beimingxianyu on 23-6-6.
//
#include "utils/hash_table.h"

#include <gtest/gtest.h>

#include <thread>

TEST(Utils, HashTable_set) {
  MM::Utils::HashSet<std::string> concurrent_set, concurrent_set2;
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

  MM::Utils::HashSet<std::string> concurrent_set3(concurrent_set),
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

TEST(Utils, HashTable_multi_set) {
  MM::Utils::MultiHashSet<std::string> concurrent_multiset,
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

  MM::Utils::MultiHashSet<std::string> concurrent_multiset3(
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

TEST(Utils, map) {
  MM::Utils::HashMap<int, std::string> concurrent_map1, concurrent_map2;

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

TEST(Utils, HashTable_multi_map) {
  MM::Utils::MultiHashMap<std::string, TestClass> concurrent_multi_map;

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
  ASSERT_EQ(concurrent_multi_map.Erase(&(insert_result.first)).Exception().Success(),
            true);
  std::pair<const std::string, TestClass>* null_ptr = nullptr;
  ASSERT_EQ(concurrent_multi_map.Erase(null_ptr).GetError(), MM::ErrorResult(MM::ErrorCode::INPUT_PARAMETERS_ARE_NOT_SUITABLE));

  for (std::uint64_t i = 0; i != 100; ++i) {
    ASSERT_EQ(concurrent_multi_map.Erase(std::to_string(i)), i + 1);
  }
  ASSERT_EQ(concurrent_multi_map.Size(), 0);
  concurrent_multi_map.Clear();
}

#define COUNT 80000
#define COUNT2 8

void InsertElement(MM::Utils::MultiHashMap<std::string, TestClass>& data,
                   std::uint64_t start, std::uint64_t count,
                   std::uint64_t count2) {
  for (std::uint64_t i = start; i != start + count; ++i) {
    for (std::uint64_t j = 0; j != count2; ++j) {
      auto insert_result = data.Emplace(std::to_string(i), TestClass{i, j, 1});
      if (insert_result.first.first != std::to_string(i)) {
        auto insert_result2 =
            data.Emplace(std::to_string(i), TestClass{i, j, 1});
      }
      ASSERT_EQ(insert_result.first.first, std::to_string(i));
      ASSERT_EQ(insert_result.first.second, TestClass(i, j, 1));
      ASSERT_EQ(insert_result.second, true);
    }
  }
}

void InsertElement2(
    MM::Utils::MultiHashMap<std::string, TestClass>& data, std::uint64_t start,
    std::uint64_t count, std::uint64_t count2,
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

void EraseElement(MM::Utils::MultiHashMap<std::string, TestClass>& data1,
                  MM::Utils::MultiHashMap<std::string, TestClass>& data2,
                  std::uint64_t start, std::uint64_t count) {
  for (std::uint64_t i = start; i != start + count; ++i) {
    if (data1.Erase(std::to_string(i)) == 0) {
      ASSERT_EQ(data2.Erase(std::to_string(i)), COUNT2);
    }
  }
}

void EraseElement2(
    MM::Utils::MultiHashMap<std::string, TestClass>& data,
    std::vector<std::pair<const std::string, TestClass>*>& address_vector) {
  for (const auto* address : address_vector) {
    auto erase_result = data.Erase(address);
//    if (erase_result != MM::Utils::ExecuteResult::SUCCESS) {
//      auto erase_result2 = data.Erase(address);
//    }
    ASSERT_EQ(erase_result.Exception().Success(), true);
  }
}

TEST(Utils, HashTable_multi_map2) {
  MM::Utils::MultiHashMap<std::string, TestClass> concurrent_multi_map1;
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
  ASSERT_EQ(concurrent_multi_map.Erase(&(insert_result.first)).Exception().Success(),
            true);

  for (std::uint64_t i = 0; i != 100; ++i) {
    ASSERT_EQ(concurrent_multi_map.Erase(std::to_string(i)), i + 1);
  }
  ASSERT_EQ(concurrent_multi_map.Size(), 0);
  concurrent_multi_map.Clear();
}

#define COUNT 80000
#define COUNT2 8

void InsertElementThread(
    MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data,
    std::uint64_t start, std::uint64_t count, std::uint64_t count2) {
  for (std::uint64_t i = start; i != start + count; ++i) {
    for (std::uint64_t j = 0; j != count2; ++j) {
      auto insert_result = data.Emplace(std::to_string(i), TestClass{i, j, 1});
      if (insert_result.first.first != std::to_string(i)) {
        auto insert_result2 =
            data.Emplace(std::to_string(i), TestClass{i, j, 1});
      }
      ASSERT_EQ(insert_result.first.first, std::to_string(i));
      ASSERT_EQ(insert_result.first.second, TestClass(i, j, 1));
      ASSERT_EQ(insert_result.second, true);
    }
  }
}

void InsertElement2Thread(
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

void EraseElementThread(
    MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data1,
    MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data2,
    std::uint64_t start, std::uint64_t count) {
  for (std::uint64_t i = start; i != start + count; ++i) {
    if (data1.Erase(std::to_string(i)) == 0) {
      ASSERT_EQ(data2.Erase(std::to_string(i)), COUNT2);
    }
  }
}

void EraseElement2Thread(
    MM::Utils::ConcurrentMultiMap<std::string, TestClass>& data,
    std::vector<std::pair<const std::string, TestClass>*>& address_vector) {
  for (auto* address : address_vector) {
    auto erase_result = data.Erase(address);
//    if (erase_result != MM::Utils::ExecuteResult::SUCCESS) {
//      auto erase_result2 = data.Erase(address);
//    }
    ASSERT_EQ(erase_result.Exception().Success(), true);
  }
}

TEST(Utils, ConcurrentHashTable_multi_map2) {
  MM::Utils::ConcurrentMultiMap<std::string, TestClass> concurrent_multi_map1;
  std::vector<std::pair<const std::string, TestClass>*> address_vector;
  InsertElement2Thread(concurrent_multi_map1, COUNT / 2, COUNT, COUNT2,
                       address_vector);
  ASSERT_EQ(concurrent_multi_map1.Size(), COUNT * COUNT2);
  for (std::uint64_t i = COUNT / 2; i != COUNT / 2 + COUNT; ++i) {
    auto equal_range = concurrent_multi_map1.EqualRange(std::to_string(i));
    ASSERT_EQ(equal_range.size(), COUNT2);
  }
  EraseElement2Thread(concurrent_multi_map1, address_vector);
}

TEST(Utils, ConcurrentHashTable_thread) {
  MM::Utils::ConcurrentMultiMap<std::string, TestClass> concurrent_multi_map1,
      concurrent_multi_map2;
  std::vector<std::thread> threads;
  std::vector<std::pair<const std::string, TestClass>*> address_vector;

  InsertElement2Thread(concurrent_multi_map1, COUNT / 2, COUNT, COUNT2,
                       address_vector);
  ASSERT_EQ(concurrent_multi_map1.Size(), COUNT * COUNT2);
  ASSERT_EQ(address_vector.size(), COUNT * COUNT2);
  for (std::uint64_t i = COUNT / 2; i != COUNT / 2 + COUNT; ++i) {
    auto equal_range = concurrent_multi_map1.EqualRange(std::to_string(i));
    ASSERT_EQ(equal_range.size(), COUNT2);
  }

  for (std::uint64_t i = 0; i != 8; ++i) {
    if (i == 4) {
      threads.emplace_back(EraseElement2Thread, std::ref(concurrent_multi_map1),
                           std::ref(address_vector));
    }

    threads.emplace_back(InsertElementThread, std::ref(concurrent_multi_map1),
                         COUNT / 8 * i, COUNT / 8, COUNT2);
  }
  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(concurrent_multi_map1.Size(), COUNT * COUNT2);
  for (std::uint64_t i = 0; i != COUNT; ++i) {
    auto equal_range = concurrent_multi_map1.EqualRange(std::to_string(i));
    for (std::uint64_t j = COUNT2 - 1, k = 0; k != COUNT2; --j, ++k) {
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

    threads.emplace_back(EraseElementThread, std::ref(concurrent_multi_map1),
                         std::ref(concurrent_multi_map2), COUNT / 8 * i,
                         COUNT / 8);
  }

  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(concurrent_multi_map1.Size(), 0);
  ASSERT_EQ(concurrent_multi_map2.Size(), 0);
}
