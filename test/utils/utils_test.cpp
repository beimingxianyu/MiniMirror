#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
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
