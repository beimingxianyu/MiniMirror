#include "file_system.h"
#include <gtest/gtest.h>

TEST(file_system, relative_path) {
  const MM::Path ori_path(ORIGINE_DIR);
#ifdef WIN32
  EXPECT_EQ(ori_path.IsDirectory(), true);
  EXPECT_EQ(ori_path.RelativePath(std::string(ORIGINE_DIR) + "/../"),
            ".\\origine");
  EXPECT_EQ(ori_path.RelativePath(std::string(ORIGINE_DIR) + "/test_dir1/test_dir2/"),
            "..\\..\\");
  EXPECT_EQ(
      ori_path.RelativePath(std::string(ORIGINE_DIR) + "/../test_dir3/test_dir4"),
      "..\\..\\origine");
  EXPECT_EQ(ori_path.RelativePath(std::string(ORIGINE_DIR) + "/../test.txt"), ".\\origine");
  EXPECT_EQ(ori_path.RelativePath("C:/user"), std::string());
#else
  EXPECT_EQ(ori_path.IsDirectory(), true);
  EXPECT_EQ(ori_path.RelativePath(std::string(ORIGINE_DIR) + "/../"),
            "./origine");
  EXPECT_EQ(
      ori_path.RelativePath(std::string(ORIGINE_DIR) + "/test_dir1/test_dir2/"),
      "../../");
  EXPECT_EQ(ori_path.RelativePath(std::string(ORIGINE_DIR) +
                                  "/../test_dir3/test_dir4"),
            "../../origine");
  EXPECT_EQ(ori_path.RelativePath(std::string(ORIGINE_DIR) + "/../test.txt"),
            "./origine");
  EXPECT_EQ(ori_path.RelativePath("C:/user"), std::string());
#endif
}