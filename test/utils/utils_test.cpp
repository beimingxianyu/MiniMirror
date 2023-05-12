#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <unordered_set>

#include "uuid.h"

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
