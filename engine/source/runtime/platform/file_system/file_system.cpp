#include <runtime/platform/file_system/file_system.h>

std::mutex MM::FileSystem::FileSystem::sync_flag_{};
MM::FileSystem::FileSystem* MM::FileSystem::FileSystem::file_system_{nullptr};

MM::FileSystem::Path::Path(const Path& other) : path_(other.path_) {}

MM::FileSystem::Path::Path(Path&& other) noexcept
    : path_(std::move(other.path_)) {}

MM::FileSystem::Path::Path(const std::string& other) {
  std::size_t first_not_empty = other.find_first_not_of(' ');
  if (first_not_empty == std::string::npos) {
    return;
  }

  std::string new_path;
  if (first_not_empty != 0) {
    new_path = other.substr(first_not_empty);
  } else {
    new_path = other;
  }
#ifdef WIN32
  // Determine whether it is a relative path.
  if (new_path[0] == '.') {
    const std::string temp =
        std::string() + BIN_DIR + PATH_SEPARATOR + new_path;
    path_ = std::make_unique<std::filesystem::path>(RemoveDotAndDotDot(
        std::filesystem::path(temp).make_preferred().string()));
    return;
  }
  // Turn the drive letter of win to uppercase.
  if ((new_path[0] > 96 && new_path[0] < 123) ||
      (new_path[0] > 64 && new_path[0] < 91)) {
    std::string temp = other;
    temp[0] = temp[0] - 32;
    path_ = std::make_unique<std::filesystem::path>(RemoveDotAndDotDot(
        std::filesystem::path(temp).make_preferred().string()));
    return;
  }
#else
  // Determine whether it is a relative path.
  if (new_path[0] == '.') {
    const std::string temp = std::string() + BIN_DIR + "/" + new_path;
    path_ = RemoveDotAndDotDot(
        std::filesystem::path(temp).make_preferred().string());
    return;
  }
  if (new_path[0] == '/') {
    path_ = RemoveDotAndDotDot(
        std::filesystem::path(new_path).make_preferred().string());
  }
#endif
}

MM::FileSystem::Path& MM::FileSystem::Path::operator=(const Path& other) {
  if (&other == this) {
    return *this;
  }

  path_ = other.path_;
  return *this;
}

MM::FileSystem::Path& MM::FileSystem::Path::operator=(Path&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  path_ = std::move(other.path_);
  return *this;
}

bool MM::FileSystem::Path::operator==(const Path& other) const {
  return path_ == other.path_;
}

const MM::FileSystem::Path MM::FileSystem::Path::operator+(
    const std::string& path) const {
  return Path(path_.string() + path);
}

MM::FileSystem::Path& MM::FileSystem::Path::operator+=(
    const std::string& path) {
  path_ += path;
  return *this;
}

MM::FileSystem::Path::operator std::string() const { return (path_).string(); }

MM::FileSystem::Path::operator std::filesystem::path() const { return path_; }

std::string MM::FileSystem::Path::String() const { return (path_).string(); }

bool MM::FileSystem::Path::IsExists() const {
  return std::filesystem::exists(path_);
}

bool MM::FileSystem::Path::IsDirectory() const {
  return std::filesystem::is_directory(path_);
}

std::string MM::FileSystem::Path::GetFileName() const {
  if (IsDirectory()) {
    return path_.filename().string();
  }
  return std::string{};
}

std::string MM::FileSystem::Path::GetExtension() const {
  if (IsDirectory()) {
    return path_.extension().string();
  }
  return std::string{};
}

void MM::FileSystem::Path::Swap(Path& other) { std::swap(path_, other.path_); }

std::string MM::FileSystem::Path::GetRelativePath(const Path& root_path) const {
  if (!root_path.IsExists()) {
    return std::string();
  }
  const std::string original_path_string = (path_).string();
  const std::string root_path_string = (root_path.path_).string();
#ifdef WIN32
  // Obtaining a relative path across drives on a Windows system will return
  // std::string().
  if (original_path_string[0] != root_path_string[0]) {
    return std::string();
  }
#endif
  std::list<std::string> original_path_parts;
  std::list<std::string> root_path_parts;
  for (std::size_t index = 0, last = 0;; ++index) {
    if (index == original_path_string.size()) {
      if (index == last) {
        break;
      }
      original_path_parts.emplace_back(
          original_path_string.substr(last, index - last));
      break;
    }

    if (original_path_string[index] == PATH_SEPARATOR) {
      original_path_parts.emplace_back(
          original_path_string.substr(last, index - last));
      last = index + 1;
    }
  }
  for (std::size_t index = 0, last = 0;; ++index) {
    if (index == root_path_string.size()) {
      if (index == last) {
        break;
      }
      root_path_parts.emplace_back(root_path_string.substr(last, index - last));
      break;
    }
    if (root_path_string[index] == PATH_SEPARATOR) {
      root_path_parts.emplace_back(root_path_string.substr(last, index - last));
      last = index + 1;
    }
  }
  if (!root_path.IsDirectory()) {
    root_path_parts.pop_back();
  }

  std::string result;
  if (original_path_parts.size() >= root_path_parts.size()) {
    for (auto original_itr = original_path_parts.begin(),
              root_itr = root_path_parts.begin();
         ; ++original_itr, ++root_itr) {
      if (root_itr == root_path_parts.end()) {
        result += ".";
        if (original_itr == original_path_parts.end()) {
          result += PATH_SEPARATOR;
          return result;
        }
        for (; original_itr != original_path_parts.end(); ++original_itr) {
          result += PATH_SEPARATOR + *original_itr;
        }
        return result;
      }
      if (*root_itr != *original_itr) {
        for (; root_itr != root_path_parts.end(); ++root_itr) {
          result += std::string("..") + PATH_SEPARATOR;
        }
        bool flag = false;
        for (; original_itr != original_path_parts.end(); ++original_itr) {
          if (flag) {
            result += std::string() + PATH_SEPARATOR + *original_itr;
          } else {
            flag = true;
            result += *original_itr;
          }
        }
        return result;
      }
    }
  }
  // original_path_parts.size() < root_path_parts.size()
  for (auto original_itr = original_path_parts.begin(),
            root_itr = root_path_parts.begin();
       ; ++original_itr, ++root_itr) {
    if (original_itr == original_path_parts.end()) {
      for (; root_itr != root_path_parts.end(); ++root_itr) {
        result += std::string("..") + PATH_SEPARATOR;
      }
      return result;
    }
    if (*original_itr != *root_itr) {
      for (; root_itr != root_path_parts.end(); ++root_itr) {
        result += std::string("..") + PATH_SEPARATOR;
      }
      bool flag = false;
      for (; original_itr != original_path_parts.end(); ++original_itr) {
        if (flag) {
          result += std::string() + PATH_SEPARATOR + *original_itr;
        } else {
          flag = true;
          result += *original_itr;
        }
      }
      return result;
    }
  }
}

std::string MM::FileSystem::Path::GetRelativePath(
    const std::string& root_path) const {
  return GetRelativePath(Path(root_path));
}

const MM::FileSystem::Path& MM::FileSystem::Path::GetAbsolutePath() const {
  return *this;
}

void MM::FileSystem::Path::ReplacePath(const Path& other_path) {
  path_ = other_path.path_;
}

void MM::FileSystem::Path::ReplacePath(const std::string& other_path) {
  path_ = other_path;
}

MM::FileSystem::FileSystem* MM::FileSystem::FileSystem::GetInstance() {
  if (file_system_) {
  } else {
    std::lock_guard<std::mutex> guard(sync_flag_);
    if (!file_system_) {
      file_system_ = new FileSystem{};
    }
  }
  return file_system_;
}

bool MM::FileSystem::FileSystem::IsExists(const Path& path) const {
  return path.IsExists();
}

bool MM::FileSystem::FileSystem::IsDirectory(const Path& path) const {
  return path.IsDirectory();
}

MM::ExecuteResult MM::FileSystem::FileSystem::DirectorySize(
    const Path& dir_path, std::size_t& directory_sie) const {
  if (!dir_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  std::vector<Path> files;
  ExecuteResult get_files_result = GetFiles(dir_path, files);
  if (get_files_result != ExecuteResult::SUCCESS) {
    return get_files_result;
  }

  for (const auto& file : files) {
    directory_sie += std::filesystem::file_size(file.path_);
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::FileSystem::FileSystem::CreateDirectory(
    const Path& dir_path) const {
  std::error_code error_code;
  if (std::filesystem::create_directory(dir_path.path_, error_code)) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

MM::ExecuteResult MM::FileSystem::FileSystem::DeleteDirectory(
    const Path& dir_path) const {
  if (!dir_path.IsExists()) {
    return ExecuteResult::SUCCESS;
  }

  std::error_code error_code;
  if (std::filesystem::remove_all(dir_path.path_, error_code)) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

MM::ExecuteResult MM::FileSystem::FileSystem::CopyDirectory(
    const Path& dir_path, const Path& dest_dir) const {
  if (!dir_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  std::error_code error_code;
  std::filesystem::copy(dir_path.path_, dest_dir.path_, error_code);

  if (error_code.value() == 0) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

MM::ExecuteResult MM::FileSystem::FileSystem::RenameDirectory(
    const Path& dir_path, const std::string& new_name) const {
  if (!dir_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  if (dir_path.String() == new_name) {
    return ExecuteResult::SUCCESS;
  }

  std::error_code error_code;
  std::filesystem::rename(dir_path.path_, new_name, error_code);

  if (error_code.value() == 0) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

bool MM::FileSystem::FileSystem::DirectoryIsEmpty(const Path& dir_path) const {
  if (!dir_path.IsExists()) {
    return false;
  }
  return std::filesystem::is_empty(dir_path.path_);
}

MM::ExecuteResult MM::FileSystem::FileSystem::GetDirectories(
    const Path& dir_path, std::vector<Path>& directories) const {
  if (!dir_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }
  if (!dir_path.IsDirectory()) {
    return ExecuteResult::FILE_OPERATION_ERROR;
  }

  for (const auto& directory_or_file :
       std::filesystem::recursive_directory_iterator{dir_path.path_}) {
    if (directory_or_file.is_directory()) {
      directories.emplace_back(
          std::filesystem::path(directory_or_file).string());
    }
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::FileSystem::FileSystem::FileSize(
    const Path& file_path, std::size_t& file_size) const {
  if (!file_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  std::error_code error_code;
  file_size = std::filesystem::file_size(file_path.path_, error_code);

  if (error_code.value() == 0) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

MM::ExecuteResult MM::FileSystem::FileSystem::CreateFile(
    const Path& file_path) const {
  if (file_path.IsExists()) {
    return ExecuteResult::FILE_OPERATION_ERROR;
  }
  std::fstream create_file;
  create_file.open(file_path.String(), std::ios_base::app);
  if (create_file.is_open()) {
    create_file.close();
    return ExecuteResult::SUCCESS;
  }
  return ExecuteResult::FILE_OPERATION_ERROR;
}

MM::ExecuteResult MM::FileSystem::FileSystem::DeleteFile(
    const Path& file_path) const {
  if (!file_path.IsExists()) {
    return ExecuteResult::SUCCESS;
  }

  std::error_code error_code;
  if (std::filesystem::remove(file_path.path_, error_code)) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

MM::ExecuteResult MM::FileSystem::FileSystem::CopyFile(
    const Path& file_path, const Path& dest_dir) const {
  if (!file_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  std::error_code error_code;
  if (std::filesystem::copy_file(file_path.path_, dest_dir.path_, error_code)) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

MM::ExecuteResult MM::FileSystem::FileSystem::RenameFile(
    const Path& file_path, const std::string& new_name) const {
  if (!file_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  if (file_path.String() == new_name) {
    return ExecuteResult::SUCCESS;
  }

  std::error_code error_code;
  std::filesystem::rename(file_path.path_, new_name, error_code);

  if (error_code.value() == 0) {
    return ExecuteResult::SUCCESS;
  }

  return ExecuteResult::FILE_OPERATION_ERROR;
}

bool MM::FileSystem::FileSystem::FileIsEmpty(const Path& file_path) const {
  return std::filesystem::is_empty(file_path.path_);
}

MM::ExecuteResult MM::FileSystem::FileSystem::GetFiles(
    const Path& dir_path, std::vector<Path>& files) const {
  if (!dir_path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  for (const auto& directory_or_file :
       std::filesystem::recursive_directory_iterator{dir_path.path_}) {
    if (directory_or_file.is_regular_file()) {
      files.emplace_back(std::filesystem::path(directory_or_file).string());
    }
  }

  return ExecuteResult::SUCCESS;
}

MM::ExecuteResult MM::FileSystem::FileSystem::Create(const Path& path) const {
  if (path.IsDirectory()) {
    return CreateDirectory(path);
  }
  return CreateFile(path);
}

MM::ExecuteResult MM::FileSystem::FileSystem::Delete(const Path& path) const {
  if (path.IsDirectory()) {
    return DeleteDirectory(path);
  }
  return DeleteFile(path);
}

MM::ExecuteResult MM::FileSystem::FileSystem::Copy(
    const Path& path, const Path& dest_path) const {
  if (path.IsDirectory()) {
    return CopyDirectory(path, dest_path);
  }
  return CopyFile(path, dest_path);
}

MM::ExecuteResult MM::FileSystem::FileSystem::Rename(
    const Path& path, const std::string& new_name) const {
  if (path.IsDirectory()) {
    return RenameDirectory(path, new_name);
  }
  return RenameFile(path, new_name);
}

bool MM::FileSystem::FileSystem::IsEmpty(const Path& path) const {
  if (path.IsDirectory()) {
    return DirectoryIsEmpty(path);
  }
  return FileIsEmpty(path);
}

MM::ExecuteResult MM::FileSystem::FileSystem::Size(const Path& path,
                                                   std::size_t& size) const {
  if (path.IsDirectory()) {
    return DirectorySize(path, size);
  }
  return FileSize(path, size);
}

MM::ExecuteResult MM::FileSystem::FileSystem::GetAll(
    const Path& path, std::vector<Path>& sub_paths) const {
  GetDirectories(path, sub_paths);
  return GetFiles(path, sub_paths);
}

bool MM::FileSystem::FileSystem::Destroy() {
  std::lock_guard<std::mutex> guard{sync_flag_};
  if (file_system_) {
    delete file_system_;
    file_system_ = nullptr;

    return true;
  }
  return false;
}

std::string MM::FileSystem::Path::RemoveDotAndDotDot(
    const std::string& original_path) {
  std::list<std::string> path_parts;
  for (std::size_t index = 0, last = 0;; ++index) {
    if (index == original_path.size()) {
      std::string sub_string{original_path.substr(last, index - last)};
      if (sub_string.empty()) {
        break;
      }
      if (sub_string == std::string("..")) {
        path_parts.pop_back();
        break;
      }
      if (sub_string != std::string(".")) {
        path_parts.push_back(original_path.substr(last, index - last));
        break;
      }
    }
    if (original_path[index] == PATH_SEPARATOR) {
      std::string sub_string{original_path.substr(last, index - last)};
      if (sub_string == std::string("..")) {
        path_parts.pop_back();
        last = index + 1;
        continue;
      }
      if (sub_string == std::string(".")) {
        last = index + 1;
        continue;
      }
      path_parts.push_back(original_path.substr(last, index - last));
      last = index + 1;
    }
  }
  std::string result;
  std::size_t index = 1;
  for (const auto& part : path_parts) {
    result += part;
    if (index != path_parts.size()) {
      result += PATH_SEPARATOR;
    }
    ++index;
  }
  return result;
}

void MM::FileSystem::Swap(MM::FileSystem::Path& lhs,
                          MM::FileSystem::Path& rhs) noexcept {
  using std::swap;
  swap(lhs.path_, rhs.path_);
}

void MM::FileSystem::swap(MM::FileSystem::Path& lhs,
                          MM::FileSystem::Path& rhs) noexcept {
  using std::swap;
  swap(lhs.path_, rhs.path_);
}

std::uint64_t MM::FileSystem::Path::GetHash() const {
  std::hash<std::string_view> hash;
  return hash(StringView());
}

std::string_view MM::FileSystem::Path::StringView() const {
  return std::string_view(path_.c_str());
}

const char* MM::FileSystem::Path::CStr() const { return path_.c_str(); }

MM::FileSystem::FileSystem::~FileSystem() { file_system_ = nullptr; }

MM::ExecuteResult MM::FileSystem::FileSystem::GetLastWriteTime(
    const MM::FileSystem::Path& path,
    MM::FileSystem::LastWriteTime& last_write_time) const {
  if (!path.IsExists()) {
    return ExecuteResult::FILE_IS_NOT_EXIST;
  }

  last_write_time = std::filesystem::last_write_time(path.path_);

  return ExecuteResult::SUCCESS;
}
