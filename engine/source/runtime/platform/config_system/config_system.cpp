#include "runtime/platform/config_system/config_system.h"

std::string MM::LoadOneConfigFromIni(const std::string& file_path,
                                     const std::string& key) {
  std::fstream config_file{file_path};
  if (config_file.is_open()) {
    std::string line;
    while (std::getline(config_file, line)) {
      if (line.length() == 0) {
        continue;
      }
      if (line[0] == '[' && line[line.size() - 1] == ']') {
        continue;
      }
      if (key == line.substr(0, line.find('='))) {
        const size_t semicolon_position{
            line.substr(line.find('=') + 1, line.size() - line.find('=') - 1)
                .find(';')};
        if (semicolon_position == std::string::npos) {
          config_file.close();
          return std::string(line.substr(line.find('=') + 1,
                                         line.size() - line.find('=') - 1));
        }
        config_file.close();
        return std::string(line.substr(line.find('=') + 1, semicolon_position));
      }
    }
    config_file.close();
  }
  return std::string{};
}

std::unordered_map<std::string, std::string> MM::LoadConfigFromIni(
    const std::string& file_path) {
  std::fstream config_file{file_path};
  std::unordered_map<std::string, std::string> result;
  if (config_file.is_open()) {
    std::string line;
    while (std::getline(config_file, line)) {
      if (line.length() == 0) {
        continue;
      }
      if (line[0] == '[' && line[line.size() - 1] == ']') {
        continue;
      }
      const size_t semicolon_position{
          line.substr(line.find('=') + 1, line.size() - line.find('=') - 1)
              .find(';')};
      if (semicolon_position == std::string::npos) {
        result.emplace(
            line.substr(0, line.find('=')),
            line.substr(line.find('=') + 1, line.size() - line.find('=') - 1));
      } else {
        result.emplace(line.substr(0, line.find('=')),
                       line.substr(line.find('=') + 1, semicolon_position));
      }
    }
    config_file.close();
    return result;
  }
  return result;
}

MM::ConfigSystem* MM::ConfigSystem::GetInstance() {
  if (config_system_) {
  } else {
    std::lock_guard<std::mutex> guard(sync_flag_);
    if (!config_system_) {
      config_system_ = new ConfigSystem();
      config_data_base_["config_dir"] = CONFIG_DIR;
      config_system_->LoadConfigFromIni(config_data_base_["config_dir"] + "init_config.ini");
    }
  }
  return config_system_;
}

std::string MM::ConfigSystem::Get(const std::string& key) {
  if (Have(key)) {
    return config_data_base_[key];
  }
  return std::string{};
}

inline bool MM::ConfigSystem::Have(const std::string& key) const{
  return config_data_base_.find(key) != config_data_base_.end();
}

bool MM::ConfigSystem::LoadOneConfigFromIni(const std::string& file_path,
                                            const std::string& key) {
  std::fstream config_file{file_path};
  if (config_file.is_open()) {
    std::string line;
    while (std::getline(config_file, line)) {
      if (line.length() == 0) {
        continue;
      }
      if (line[0] == '[' && line[line.size() - 1] == ']') {
        continue;
      }
      if (key == line.substr(0, line.find('='))) {
        const size_t semicolon_position{
            line.substr(line.find('=') + 1, line.size() - line.find('=') - 1)
                .find(';')};
        if (semicolon_position == std::string::npos) {
          config_data_base_[key] = std::string(line.substr(
              line.find('=') + 1, line.size() - line.find('=') - 1));
        }
        config_data_base_[key] =
            std::string(line.substr(line.find('=') + 1, semicolon_position));
        config_file.close();
        return true;
      }
    }
    config_file.close();
    return false;
  }
  return false;
}

bool MM::ConfigSystem::LoadConfigFromIni(const std::string& file_path) {
  std::fstream config_file{file_path};
  if (config_file.is_open()) {
    std::string line;
    while (std::getline(config_file, line)) {
      if (line.length() == 0) {
        continue;
      }
      if (line[0] == '[' && line[line.size() - 1] == ']') {
        continue;
      }
      const size_t semicolon_position{
          line.substr(line.find('=') + 1, line.size() - line.find('=') - 1)
              .find(';')};
      if (semicolon_position == std::string::npos) {
        config_data_base_.emplace(
            line.substr(0, line.find('=')),
            line.substr(line.find('=') + 1, line.size() - line.find('=') - 1));
      } else {
        config_data_base_.emplace(line.substr(0, line.find('=')),
                       line.substr(line.find('=') + 1, semicolon_position));
      }
    }
    config_file.close();
    return true;
  }
  return false;
}

void MM::ConfigSystem::Clear() { config_data_base_.clear(); }

const std::unordered_map<std::string, std::string>& MM::ConfigSystem::GetAllConfig() const{
  return config_data_base_;
}

const size_t MM::ConfigSystem::Size() { return config_data_base_.size(); }

void MM::ConfigSystem::Set(const std::string& key,
                           const std::string& data) {
  config_data_base_[key] = data;
}

void MM::ConfigSystem::Set(const std::string& key, const int& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key, const long& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key, const long long& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key, const float& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key, const double& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key, const long double& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key,
                                  const unsigned int& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key,
                                  const unsigned long& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::Set(const std::string& key,
                                  const unsigned long long& data) {
  config_data_base_[key] = std::to_string(data);
}

bool MM::ConfigSystem::Get(const std::string& key, std::string& get_data) const {
  if (Have(key)) {
    get_data = config_data_base_[key];
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key, int& get_data) const{
  if (Have(key)) {
    get_data = std::stoi(config_data_base_[key]);
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key, unsigned int& get_data) const{
  if (Have(key)) {
    get_data = static_cast<unsigned int>(std::stoi(config_data_base_[key]));
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key, float& get_data) const{
  if (Have((key))) {
    get_data = std::stof(config_data_base_[key]);
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key, long& get_data) const{
  if (Have((key))) {
    get_data = std::stol(config_data_base_[key]);
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key, long double& get_data) const{
  if (Have((key))) {
    get_data = std::stold(config_data_base_[key]);
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key, long long& get_data) const{
  if (Have((key))) {
    get_data = std::stoll(config_data_base_[key]);
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key, unsigned long& get_data) const{
  if (Have((key))) {
    get_data = std::stoul(config_data_base_[key]);
    return true;
  }
  return false;
}

bool MM::ConfigSystem::Get(const std::string& key,
                                  unsigned long long& get_data) const{
  if (Have((key))) {
    get_data = std::stoull(config_data_base_[key]);
    return true;
  }
  return false;
}
