#include "runtime/platform/config_system/config_system.h"

std::mutex MM::ConfigSystem::ConfigSystem::sync_flag_{std::mutex()};
MM::ConfigSystem::ConfigSystem* MM::ConfigSystem::ConfigSystem::config_system_{
    nullptr};
std::unordered_map<std::string, std::string>
    MM::ConfigSystem::ConfigSystem::config_data_base_{};

std::string MM::ConfigSystem::LoadOneConfigFromIni(
    const MM::FileSystem::Path& file_path, const std::string& key) {
  std::fstream config_file{file_path.String()};
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

std::unordered_map<std::string, std::string>
MM::ConfigSystem::LoadConfigFromIni(const MM::FileSystem::Path& file_path) {
  std::fstream config_file{file_path.String()};
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

MM::ConfigSystem::ConfigSystem::~ConfigSystem() { config_system_ = nullptr; }

MM::ConfigSystem::ConfigSystem* MM::ConfigSystem::ConfigSystem::GetInstance() {
  if (config_system_) {
  } else {
    std::lock_guard<std::mutex> guard(sync_flag_);
    if (!config_system_) {
      config_system_ = new ConfigSystem{};
      config_data_base_["config_dir"] = CONFIG_DIR;
      std::cout << CONFIG_DIR << std::endl;
      ExecuteResult load_result =
          config_system_->LoadConfigFromIni(FileSystem::Path(
              config_data_base_["config_dir"] + "/init_config.ini"));
    }
    CheckInit();
  }
  return config_system_;
}

std::string MM::ConfigSystem::ConfigSystem::GetConfig(const std::string& key) {
  if (Have(key)) {
    return config_data_base_[key];
  }
  return std::string{};
}

bool MM::ConfigSystem::ConfigSystem::Have(const std::string& key) const {
  return config_data_base_.find(key) != config_data_base_.end();
}

bool MM::ConfigSystem::ConfigSystem::LoadOneConfigFromIni(
    const MM::FileSystem::Path& file_path, const std::string& key) {
  std::fstream config_file{file_path.String()};
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

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::LoadConfigFromIni(
    const MM::FileSystem::Path& file_path) {
  std::fstream config_file{file_path.String()};
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
        config_data_base_.emplace(
            line.substr(0, line.find('=')),
            line.substr(line.find('=') + 1, semicolon_position));
      }
    }
    config_file.close();
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::FILE_IS_NOT_EXIST;
}

void MM::ConfigSystem::ConfigSystem::Clear() { config_data_base_.clear(); }

const std::unordered_map<std::string, std::string>&
MM::ConfigSystem::ConfigSystem::GetAllConfig() const {
  return config_data_base_;
}

std::size_t MM::ConfigSystem::ConfigSystem::Size() {
  return config_data_base_.size();
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::Destroy() {
  std::lock_guard<std::mutex> guard{sync_flag_};
  if (config_system_) {
    delete config_system_;
    config_data_base_.clear();
    config_system_ = nullptr;

    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::DESTROY_FAILED;
}

void MM::ConfigSystem::ConfigSystem::CheckInit() {
  if (!CheckAllNeedConfigLoaded()) {
    throw std::runtime_error(
        "The required settings are not loaded. Please reset init config file.");
  }
  if (!CheckInitVertexAndIndexBuffer()) {
    throw std::runtime_error(
        "The setting for vertex_buffer_size and index_buffer_size are "
        "incorrect. Please reset init config file.");
  }
}

bool MM::ConfigSystem::ConfigSystem::CheckAllNeedConfigLoaded() {
  if (config_data_base_.size() < 18) {
    return false;
  }
  if (config_data_base_.find("version_major") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("version_minor") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("version_patch") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("logic_thread_share") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("graph_thread_share") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("manager_size") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("engine_dir") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("config_dir") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("bin_dir") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("asset_dir") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("asset_dir_std") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("asset_dir_user") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("window_extent_width") ==
      config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("window_extent_height") ==
      config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("multi_sample_count") == config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("init_vertex_buffer_size") ==
      config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("max_vertex_buffer_size") ==
      config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("init_index_buffer_size") ==
      config_data_base_.end()) {
    return false;
  }
  if (config_data_base_.find("max_index_buffer_size") ==
      config_data_base_.end()) {
    return false;
  }
  return true;
}

bool MM::ConfigSystem::ConfigSystem::CheckInitVertexAndIndexBuffer() {
  const std::string init_vertex_buffer_size =
      config_data_base_.find("init_vertex_buffer_size")->second;
  const std::string max_vertex_buffer_size =
      config_data_base_.find("max_vertex_buffer_size")->second;
  if (std::stoul(init_vertex_buffer_size) >
      std::stoul(max_vertex_buffer_size)) {
    return false;
  }

  const std::string init_index_buffer_size =
      config_data_base_.find("init_index_buffer_size")->second;
  const std::string max_index_buffer_size =
      config_data_base_.find("max_index_buffer_size")->second;
  if (std::stoul(init_index_buffer_size) > std::stoul(max_index_buffer_size)) {
    return false;
  }

  return true;
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const std::string& data) {
  config_data_base_[key] = data;
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const int& data) {
  ConfigSystem::config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const long& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const long long& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const float& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const double& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const long double& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const unsigned int& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const unsigned long& data) {
  config_data_base_[key] = std::to_string(data);
}

void MM::ConfigSystem::ConfigSystem::SetConfig(const std::string& key,
                                               const unsigned long long& data) {
  config_data_base_[key] = std::to_string(data);
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, std::string& get_data) const {
  if (Have(key)) {
    get_data = config_data_base_[key];
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, int& get_data) const {
  if (Have(key)) {
    get_data = std::stoi(config_data_base_[key]);
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, unsigned int& get_data) const {
  if (Have(key)) {
    get_data = static_cast<unsigned int>(std::stoi(config_data_base_[key]));
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, float& get_data) const {
  if (Have((key))) {
    get_data = std::stof(config_data_base_[key]);
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, long& get_data) const {
  if (Have((key))) {
    get_data = std::stol(config_data_base_[key]);
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, long double& get_data) const {
  if (Have((key))) {
    get_data = std::stold(config_data_base_[key]);
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, long long& get_data) const {
  if (Have((key))) {
    get_data = std::stoll(config_data_base_[key]);
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, unsigned long& get_data) const {
  if (Have((key))) {
    get_data = std::stoul(config_data_base_[key]);
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}

MM::ExecuteResult MM::ConfigSystem::ConfigSystem::GetConfig(
    const std::string& key, unsigned long long& get_data) const {
  if (Have((key))) {
    get_data = std::stoull(config_data_base_[key]);
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::NO_SUCH_CONFIG;
}
