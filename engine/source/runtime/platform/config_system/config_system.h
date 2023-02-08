#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>

namespace MM {
std::string LoadOneConfigFromIni(const std::string& file_path,
                                 const std::string& key);

std::unordered_map<std::string, std::string> LoadConfigFromIni(
    const std::string& file_path);

class ConfigSystem {
 public:
  ~ConfigSystem() = delete;
  ConfigSystem(const ConfigSystem&) = delete;
  ConfigSystem(ConfigSystem&&) = delete;
  ConfigSystem& operator=(const ConfigSystem&) = delete;
  ConfigSystem& operator=(ConfigSystem&&) = delete;

 public:
  /**
   * \brief Create instance.
   * \return A ConfigSystem instance.
   */
  static ConfigSystem* GetInstance();

  /**
   * \brief Modified the value.
   * \param key Name of the setting to be modified.
   * \param data The new value of the setting to be modified.
   */
  void Set(const std::string& key, const std::string& data);
  void Set(const std::string& key, const int& data);
  void Set(const std::string& key, const long& data);
  void Set(const std::string& key, const long long& data);
  void Set(const std::string& key, const float& data);
  void Set(const std::string& key, const double& data);
  void Set(const std::string& key, const long double& data);
  void Set(const std::string& key, const unsigned& data);
  void Set(const std::string& key, const unsigned long& data);
  void Set(const std::string& key, const unsigned long long& data);

  /**
   * \brief Get the value.
   * \param key Name of the setting to be modified.
   * \param get_data A reference that returns a specific value.
   * \return If the desired setting does not exist, return false; Otherwise,
   * return true.
   */
  bool Get(const std::string& key, std::string& get_data) const;
  bool Get(const std::string& key, int& get_data) const;
  bool Get(const std::string& key, unsigned& get_data) const;
  bool Get(const std::string& key, float& get_data) const;
  bool Get(const std::string& key, long& get_data) const;
  bool Get(const std::string& key, long double& get_data) const;
  bool Get(const std::string& key, long long& get_data) const;
  bool Get(const std::string& key, unsigned long& get_data) const;
  bool Get(const std::string& key, unsigned long long& get_data) const;
  // void Set(std::string key, std::string data);

  /**
   * \brief Get the value,but its type is std::string.
   * \param key Name of the setting to be modified.
   * \return The data of setting you want to obtain.
   */
  std::string Get(const std::string& key);

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
  bool LoadOneConfigFromIni(const std::string& file_path,
                                   const std::string& key);

  /**
   * \brief Get all setting from a ini file.
   * \param file_path The path of the ini file.
   * \return Returns true if the settings were read successfully; otherwise
   * returns false.
   */
  bool LoadConfigFromIni(const std::string& file_path);

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
  const size_t Size();

protected:
  ConfigSystem() = default;

 private:
  static std::mutex sync_flag_;
  static ConfigSystem* config_system_;
  static std::unordered_map<std::string, std::string> config_data_base_;
};

std::mutex ConfigSystem::sync_flag_{std::mutex()};
ConfigSystem* ConfigSystem::config_system_{nullptr};
std::unordered_map<std::string, std::string> ConfigSystem::config_data_base_{};
}  // namespace MM
