#include "config_system.h"

#include <gtest/gtest.h>

#include <iostream>

TEST(config_system, config_system) {
  MM::ConfigSystem::ConfigSystem* config_system(
      MM::ConfigSystem::ConfigSystem::GetInstance());
  //  EXPECT_EQ(config_system->GetConfig("config_dir") + "/init_config.ini",
  //            std::string(CONFIG_DIR) + + "/init_config.ini");

  std::string setting{};
  EXPECT_EQ(config_system->GetConfig("config_dir", setting),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(config_system->LoadConfigFromIni(setting + "/init_config.ini"),
            MM::ExecuteResult::SUCCESS);
  EXPECT_GE(config_system->GetAllConfig().size(), 18);
  EXPECT_EQ(config_system->GetConfig("engine_dir", setting),
            MM::ExecuteResult ::SUCCESS);
  EXPECT_EQ(setting, MM_ENGINE_DIR);
  EXPECT_EQ(config_system->GetConfig("config_dir", setting),
            MM::ExecuteResult ::SUCCESS);
  EXPECT_EQ(setting, MM_CONFIG_DIR);
  EXPECT_EQ(config_system->GetConfig("bin_dir", setting),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(setting, MM_BIN_DIR);
  config_system->Clear();
  EXPECT_EQ(config_system->Size(), 0);
  config_system->SetConfig("config_dir", MM_CONFIG_DIR_TEST);
  std::string temp;
  config_system->GetConfig("config_dir", temp);
  EXPECT_EQ(temp, MM_CONFIG_DIR_TEST);
  EXPECT_EQ(config_system->GetConfig("config_dir", setting),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(config_system->LoadConfigFromIni(
                MM::FileSystem::Path(setting + "/init_config_test.ini")),
            MM::ExecuteResult::SUCCESS);
  EXPECT_EQ(config_system->Size(), 4);

  EXPECT_EQ(config_system->GetConfig("field1", setting),
            MM::ExecuteResult ::SUCCESS);
  EXPECT_EQ(setting, "data1");
  EXPECT_EQ(config_system->GetConfig("field2", setting),
            MM::ExecuteResult ::SUCCESS);
  EXPECT_EQ(setting, "data2");
  EXPECT_EQ(config_system->GetConfig("field3", setting),
            MM::ExecuteResult ::SUCCESS);
  EXPECT_EQ(setting, "data3");
  config_system->Clear();
  EXPECT_EQ(config_system->Size(), 0);
}
