#include "file_system.h"

#include <gtest/gtest.h>

TEST(file_system, relative_path) {
  const MM::FileSystem::Path ori_path(MM_ORIGINE_DIR);
  EXPECT_EQ(ori_path.IsDirectory(), true);
  EXPECT_EQ(ori_path.GetRelativePath(std::string(MM_ORIGINE_DIR) + "/../"),
            "./origine");
  EXPECT_EQ(ori_path.GetRelativePath(std::string(MM_ORIGINE_DIR) +
                                     "/test_dir1/test_dir2/"),
            "../../");
  EXPECT_EQ(ori_path.GetRelativePath(std::string(MM_ORIGINE_DIR) +
                                     "/test_dir1//test_dir2/"),
            "../../");
  EXPECT_EQ(ori_path.GetRelativePath(std::string(MM_ORIGINE_DIR) +
                                     "/../test_dir3/test_dir4"),
            "../../origine");
  EXPECT_EQ(
      ori_path.GetRelativePath(std::string(MM_ORIGINE_DIR) + "/../test.txt"),
      "./origine");
  EXPECT_EQ(ori_path.GetRelativePath("C:/user"), std::string());
}