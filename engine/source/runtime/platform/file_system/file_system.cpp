#include <runtime/platform/file_system/file_system.h>

std::mutex MM::FileSystem::FileSystem::sync_flag_{};
MM::FileSystem::FileSystem* MM::FileSystem::FileSystem::file_system_{nullptr};


MM::FileSystem::Path::Path(const Path& other)
    : path_(std::make_unique<std::filesystem::path>(*other.path_)) {}

MM::FileSystem::Path::Path(Path&& other) noexcept
    : path_(other.path_.release()) {}

MM::FileSystem::Path::Path(const std::string& other) {
#ifdef WIN32
  // Determine whether it is a relative path.
  if (other[0] == '.') {
    const std::string temp = std::string() + BIN_DIR + PATH_SEPARATOR + other;
    path_ = std::make_unique<std::filesystem::path>(RemoveDotAndDotDot(
        std::filesystem::path(temp).make_preferred().string()));
    return;
  }
  // Turn the drive letter of win to uppercase.
  if ((other[0] > 96 && othet[0] < 123) || (other[0] > 64 && other[0] < 91)) {
    std::string temp = other;
    temp[0] = temp[0] - 32;
    path_ = std::make_unique<std::filesystem::path>(RemoveDotAndDotDot(
        std::filesystem::path(temp).make_preferred().string()));
    return;
  }
#else
  // Determine whether it is a relative path.
  if (other[0] == '.') {
    const std::string temp = std::string() + BIN_DIR + "/" + other;
    path_ = std::make_unique<std::filesystem::path>(RemoveDotAndDotDot(
        std::filesystem::path(temp).make_preferred().string()));
    return;
  }
  if (other[0] == '/') {
    path_ = std::make_unique<std::filesystem::path>(RemoveDotAndDotDot(
        std::filesystem::path(other).make_preferred().string()));
  }
#endif
}

MM::FileSystem::Path& MM::FileSystem::Path::operator=(const Path& other) {
  if (&other == this) {
    return *this;
  }
  path_ = std::make_unique<std::filesystem::path>(*other.path_);
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
  return *path_ == *(other.path_);
}

const MM::FileSystem::Path& MM::FileSystem::Path::operator+(
    const std::string& path) const {
  *path_ += path;
  return *this;
}

MM::FileSystem::Path& MM::FileSystem::Path::operator+=(
    const std::string& path) {
  *path_ += path;
  return *this;
}

MM::FileSystem::Path::operator std::string() const { return (*path_).string(); }

MM::FileSystem::Path::operator std::filesystem::path() const { return *path_; }

std::string MM::FileSystem::Path::String() const { return (*path_).string(); }

bool MM::FileSystem::Path::IsExists() const {
  if (!path_) {
    return false;
  }
  return std::filesystem::exists(*path_);
}

bool MM::FileSystem::Path::IsDirectory() const {
  return std::filesystem::is_directory(*path_);
}

std::string MM::FileSystem::Path::FileName() const {
  if (IsDirectory()) {
    return path_->filename().string();
  }
  return std::string{};
}

std::string MM::FileSystem::Path::Extension() const {
  if (IsDirectory()) {
    return path_->extension().string();
  }
  return std::string{};
}

void MM::FileSystem::Path::Swap(const Path& other) const {
  std::swap(*path_, *other.path_);
}

std::string MM::FileSystem::Path::RelativePath(const Path& root_path) const {
  if (!root_path.IsExists()) {
    return std::string();
  }
  const std::string original_path_string = (*path_).string();
  const std::string root_path_string = (*root_path.path_).string();
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

std::string MM::FileSystem::Path::RelativePath(
    const std::string& root_path) const {
  return RelativePath(Path(root_path));
}

const MM::FileSystem::Path& MM::FileSystem::Path::AbsolutePath() const {
  return *this;
}

void MM::FileSystem::Path::ReplacePath(const Path& other_path) {
  path_ = std::make_unique<std::filesystem::path>(other_path.path_->string());
}

void MM::FileSystem::Path::ReplacePath(const std::string& other_path) {
  path_ = std::make_unique<std::filesystem::path>(other_path);
};

MM::FileSystem::FileSystem*
MM::FileSystem::FileSystem::GetInstance() {
  if (file_system_) {
  } else {
    std::lock_guard<std::mutex> guard(sync_flag_);
    if (!file_system_) {
      file_system_ = new FileSystem{};
    }
  }
  return file_system_;
}

bool MM::FileSystem::FileSystem::Exists(const Path& path) const {
  return path.IsExists();
}

bool MM::FileSystem::FileSystem::IsDirectory(const Path& path) const {
  return path.IsDirectory();
}

std::size_t MM::FileSystem::FileSystem::DirectorySize(
    const Path& dir_path) const {
  if (!dir_path.IsExists()) {
    return 0;
  }
  std::size_t size{0};
  const std::vector<Path> files = GetFiles(dir_path);
  for (const auto& file : files) {
    size += std::filesystem::file_size(*file.path_);
  }
  return size;
}

bool MM::FileSystem::FileSystem::CreateDirectory(const Path& dir_path) const {
  return std::filesystem::create_directory(*dir_path.path_);
}

bool MM::FileSystem::FileSystem::DeleteDirectory(const Path& dir_path) const {
  return std::filesystem::remove_all(*dir_path.path_);
}

bool MM::FileSystem::FileSystem::CopyDirectory(const Path& dir_path,
                                   const Path& dest_dir) const {
  if (!dir_path.IsExists()) {
    return false;
  }
  std::filesystem::copy(*dir_path.path_, *dest_dir.path_);
  return true;
}

bool MM::FileSystem::FileSystem::RenameDirectory(
    const Path& dir_path,
                                     const std::string& new_name) const {
  if (!dir_path.IsExists()) {
    return false;
  }
  std::filesystem::rename(*dir_path.path_, new_name);
  return true;
}

bool MM::FileSystem::FileSystem::DirectoryIsEmpty(const Path& dir_path) const {
  if (!dir_path.IsExists()) {
    return false;
  }
  return std::filesystem::is_empty(*dir_path.path_);
}

std::vector<MM::FileSystem::Path> MM::FileSystem::FileSystem::GetDirectories(
    const Path& dir_path) const {
  std::vector<Path> directories;
  for (const auto& directory_or_file :
       std::filesystem::recursive_directory_iterator{*dir_path.path_}) {
    if (directory_or_file.is_directory()) {
      directories.emplace_back(
          std::filesystem::path(directory_or_file).string());
    }
  }
  return directories;
}

std::size_t MM::FileSystem::FileSystem::FileSize(const Path& file_path) const {
  if (!file_path.IsExists()) {
    return 0;
  }
  return std::filesystem::file_size(*file_path.path_);
}

bool MM::FileSystem::FileSystem::CreateFile(const Path& file_path) const {
  if (file_path.IsExists()) {
    return false;
  }
  std::fstream create_file;
  create_file.open(file_path.String(), std::ios_base::app);
  if (create_file.is_open()) {
    create_file.close();
    return true;
  }
  return false;
}

bool MM::FileSystem::FileSystem::DeleteFile(const Path& file_path) const {
  return std::filesystem::remove(*file_path.path_);
}

bool MM::FileSystem::FileSystem::CopyFile(const Path& file_path,
                              const Path& dest_dir) const {
  if (!file_path.IsExists()) {
    return false;
  }
  return std::filesystem::copy_file(*file_path.path_, *dest_dir.path_);
}

bool MM::FileSystem::FileSystem::RenameFile(const Path& file_path,
                                const std::string& new_name) const {
  if (!file_path.IsExists()) {
    return false;
  }
  std::filesystem::rename(*file_path.path_, new_name);
  return true;
}

bool MM::FileSystem::FileSystem::FileIsEmpty(const Path& file_path) const {
  return std::filesystem::is_empty(*file_path.path_);
}

std::vector<MM::FileSystem::Path> MM::FileSystem::FileSystem::GetFiles(
    const Path& dir_path) const {
  std::vector<Path> files;
  for (const auto& directory_or_file :
       std::filesystem::recursive_directory_iterator{*dir_path.path_}) {
    if (directory_or_file.is_regular_file()) {
      files.emplace_back(std::filesystem::path(directory_or_file).string());
    }
  }
  return files;
}

bool MM::FileSystem::FileSystem::Create(const Path& path) const {
  if (path.IsDirectory()) {
    return CreateDirectory(path);
  }
  return CreateFile(path);
}

bool MM::FileSystem::FileSystem::Delete(const Path& path) const {
  if (path.IsDirectory()) {
    return DeleteDirectory(path);
  }
  return DeleteFile(path);
}

bool MM::FileSystem::FileSystem::Copy(const Path& path,
                                      const Path& dest_path) const {
  if (path.IsDirectory()) {
    return CopyDirectory(path, dest_path);
  }
  return CopyFile(path, dest_path);
}

bool MM::FileSystem::FileSystem::Rename(const Path& path,
                            const std::string& new_name) const {
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

std::size_t MM::FileSystem::FileSystem::Size(const Path& path) const {
  if (path.IsDirectory()) {
    return DirectorySize(path);
  }
  return FileSize(path);
}

std::vector<MM::FileSystem::Path> MM::FileSystem::FileSystem::GetAll(
    const Path& path) const {
  if (path.IsDirectory()) {
    return GetDirectories(path);
  }
  return GetFiles(path);
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

MM::FileSystem::FileSystem::~FileSystem() { file_system_ = nullptr; }
