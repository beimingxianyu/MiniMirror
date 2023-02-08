#include <iostream>

#include <gtest/gtest.h>

#include "config_system.h"

TEST(config_system, main) {
  MM::ConfigSystem* config_system(MM::ConfigSystem::GetInstance());
  EXPECT_EQ(config_system->Get("config_dir") + "/init_config.ini",
            "D:/vs/MiniMirror/MiniMirror/build/Debug/config/init_config.ini");
  EXPECT_EQ(config_system->LoadConfigFromIni(config_system->Get("config_dir") +
                                             "/init_config.ini"),
            true);
  EXPECT_EQ(config_system->GetAllConfig().size(), 3);
  EXPECT_EQ(config_system->Get("engine_dir"),
            "D:/vs/MiniMirror/MiniMirror/build/Debug");
  EXPECT_EQ(config_system->Get("config_dir"),
            "D:/vs/MiniMirror/MiniMirror/build/Debug/config");
  EXPECT_EQ(config_system->Get("bin_dir"),
            "D:/vs/MiniMirror/MiniMirror/build/Debug/bin");
  config_system->Clear();
  EXPECT_EQ(config_system->Size(), 0);
  config_system->Set("config_dir", CONFIG_DIR);
  std::string temp;
  config_system->Get("config_dir", temp);
  EXPECT_EQ(temp, CONFIG_DIR);
  EXPECT_EQ(
      config_system->LoadConfigFromIni(config_system->Get("config_dir") + "/init_config.ini"), true);
  EXPECT_EQ(config_system->Size(), 4);
  EXPECT_EQ(config_system->Get("field1"), "data1");
  EXPECT_EQ(config_system->Get("field2"), "data2");
  EXPECT_EQ(config_system->Get("field3"), "data3");
  config_system->Clear();
  EXPECT_EQ(config_system->Size(), 0);
}
