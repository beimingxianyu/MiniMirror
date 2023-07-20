#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>

#include "runtime/platform/file_system/file_system.h"
#include "utils/marco.h"

namespace MM {
namespace ConfigSystem {
std::string LoadOneConfigFromIni(const MM::FileSystem::Path& file_path,
                                 const std::string& key);

std::unordered_map<std::string, std::string> LoadConfigFromIni(
    const MM::FileSystem::Path& file_path);

class ConfigSystem {
  friend std::shared_ptr<ConfigSystem>;

 public:
  ConfigSystem(const ConfigSystem&) = delete;
  ConfigSystem(ConfigSystem&&) = delete;
  ConfigSystem& operator=(const ConfigSystem&) = delete;
  ConfigSystem& operator=(ConfigSystem&&) = delete;

 public:
  /**
   * \brief Create instance.
   * \return A ConfigSystem instance.
   */
  static MM::ConfigSystem::ConfigSystem* GetInstance();

  /**
   * \brief Modified the value.
   * \param key Name of the setting to be modified.
   * \param data The new value of the setting to be modified.
   */
  void SetConfig(const std::string& key, const std::string& data);
  void SetConfig(const std::string& key, const int& data);
  void SetConfig(const std::string& key, const long& data);
  void SetConfig(const std::string& key, const long long& data);
  void SetConfig(const std::string& key, const float& data);
  void SetConfig(const std::string& key, const double& data);
  void SetConfig(const std::string& key, const long double& data);
  void SetConfig(const std::string& key, const unsigned& data);
  void SetConfig(const std::string& key, const unsigned long& data);
  void SetConfig(const std::string& key, const unsigned long long& data);

  // TYPE_CONVERSION_FAILED.
  /**
   * \brief Get the value.
   * \param key Name of the setting to be modified.
   * \param get_data A reference that returns a specific value.
   * \return If the desired setting does not exist, return false; Otherwise,
   * return true.
   */
  ExecuteResult GetConfig(const std::string& key, std::string& get_data) const;
  ExecuteResult GetConfig(const std::string& key, std::int32_t& get_data) const;
  ExecuteResult GetConfig(const std::string& key,
                          std::uint32_t& get_data) const;
  ExecuteResult GetConfig(const std::string& key, std::int64_t& get_data) const;
  ExecuteResult GetConfig(const std::string& key,
                          std::uint64_t& get_data) const;
  ExecuteResult GetConfig(const std::string& key, float& get_data) const;
  ExecuteResult GetConfig(const std::string& key, double& get_data) const;
  ExecuteResult GetConfig(const std::string& key, long double& get_data) const;
  // void SetConfig(std::string key, std::string data);

  /**
   * \brief Judge whether a setting item exists.
   * \param key The name of the setting you want to know if it exists.
   * \return If the setting exists, return true;otherwise, false is returned.
   */
  inline bool Have(const std::string& key) const;

  /**
   * \brief Get one setting from a ini file.
   * \param file_path The path of the ini file.
   * \param key The name of the setting you want to get.
   * \return Returns true if the settings were read successfully; otherwise
   * returns false.
   */
  bool LoadOneConfigFromIni(const MM::FileSystem::Path& file_path,
                            const std::string& key);

  /**
   * \brief Get all setting from a ini file.
   * \param file_path The path of the ini file.
   * \return Returns true if the settings were read successfully; otherwise
   * returns false.
   */
  ExecuteResult LoadConfigFromIni(const MM::FileSystem::Path& file_path);

  /**
   * \brief Clear all settings.
   */
  void Clear();

  /**
   * \brief Get constant references of all settings.
   * \return Constant reference of all settings.
   */
  const std::unordered_map<std::string, std::string>& GetAllConfig() const;

  /**
   * \brief Return the count of all setting.
   * \return The count of all setting.
   */
  std::size_t Size();

  /**
   * \brief Destroy the instance. If it is successfully destroyed, it returns
   * true, otherwise it returns false.
   * \remark Only when no other module uses
   * this system can it be destroyed successfully.
   * \return If it is successfully
   * destroyed, it returns true, otherwise it returns false.
   */

 protected:
  ConfigSystem() = default;
  ~ConfigSystem();
  static ConfigSystem* config_system_;

 private:
  /**
   * \brief Destroy the instance. If it is successfully destroyed, it returns
   * true, otherwise it returns false.
   * \return If it is successfully
   * destroyed, it returns true, otherwise it returns false.
   */
  ExecuteResult Destroy();

  static void CheckInit();

  static bool CheckAllNeedConfigLoaded();

 private:
  static std::mutex sync_flag_;
  static std::unordered_map<std::string, std::string> config_data_base_;
};
}  // namespace ConfigSystem
}  // namespace MM
