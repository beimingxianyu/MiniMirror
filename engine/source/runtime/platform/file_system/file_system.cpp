#include <runtime/platform/file_system/file_system.h>

std::mutex MM::FileSystem::FileSystem::sync_flag_{};
MM::FileSystem::FileSystem* MM::FileSystem::FileSystem::file_system_{nullptr};

std::string MM::FileSystem::__GetCurrentPath__() {
#ifdef WIN32
  const std::string win_format_path = std::filesystem::current_path().string();
  std::string new_string{};
  new_string.reserve(win_format_path.size());
  for (std::size_t index = 0; index < win_format_path.size(); ++index) {
    if (win_format_path[index] == '\\') {
      new_string.append(1, '/');
      continue;
    }
    new_string.append(1, win_format_path[index]);
  }
  new_string.shrink_to_fit();
  return new_string;
#else
  return std::filesystem::current_path().string();
#endif
}

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
  // Determine whether it is a relative path.
  if (new_path[0] == '.') {
    const std::string temp = g_bin_dir + "/" + new_path;
    path_ = RemoveDotAndDotDot(temp);
    return;
  }
#ifdef WIN32
  // Turn the drive letter of win to uppercase.
  if (new_path[0] > 96 && new_path[0] < 123) {
    std::string temp = other;
    temp[0] = temp[0] - 32;
    path_ = RemoveDotAndDotDot(temp);
    return;
  }
  path_ = RemoveDotAndDotDot(other);
#else
  if (new_path[0] == '/') {
    path_ = RemoveDotAndDotDot(new_path);
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
  std::error_code error_code;
  return std::filesystem::exists(path_);
}

bool MM::FileSystem::Path::IsDirectory() const {
  return std::filesystem::is_directory(path_);
}

std::string MM::FileSystem::Path::GetFileName() const {
  if (!IsDirectory()) {
    return path_.filename().string();
  }
  return std::string{};
}

std::string_view MM::FileSystem::Path::GetFileNameView() const {
  if (!IsDirectory()) {
    auto string_view = StringView();
    auto string_size = string_view.size();
    auto dot_postion = string_view.rfind('/');
    if (dot_postion == std::string_view::npos) {
      return std::string_view{};
    }
    return std::string_view(CStr() + dot_postion, string_size - dot_postion);
  }
  return std::string_view{};
}

std::string MM::FileSystem::Path::GetExtension() const {
  if (!IsDirectory()) {
    return path_.extension().string();
  }
  return std::string{};
}

std::string_view MM::FileSystem::Path::GetExtensionView() const {
  if (!IsDirectory()) {
    auto string_view = StringView();
    auto string_size = string_view.size();
    auto dot_postion = string_view.rfind('.');
    if (dot_postion == std::string_view::npos) {
      return std::string_view{};
    }
    return std::string_view(CStr() + dot_postion, string_size - dot_postion);
  }
  return std::string_view{};
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

    if (original_path_string[index] == '/') {
      std::size_t len = index - last;
      if (len != 0) {
        original_path_parts.emplace_back(
            original_path_string.substr(last, len));
      }
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
    if (root_path_string[index] == '/') {
      std::size_t len = index - last;
      if (len != 0) {
        root_path_parts.emplace_back(root_path_string.substr(last, len));
      }
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
          result += '/';
          return result;
        }
        for (; original_itr != original_path_parts.end(); ++original_itr) {
          result += '/' + *original_itr;
        }
        return result;
      }
      if (*root_itr != *original_itr) {
        for (; root_itr != root_path_parts.end(); ++root_itr) {
          result += std::string("..") + '/';
        }
        bool flag = false;
        for (; original_itr != original_path_parts.end(); ++original_itr) {
          if (flag) {
            result += std::string() + '/' + *original_itr;
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
        result += std::string("..") + '/';
      }
      return result;
    }
    if (*original_itr != *root_itr) {
      for (; root_itr != root_path_parts.end(); ++root_itr) {
        result += std::string("..") + '/';
      }
      bool flag = false;
      for (; original_itr != original_path_parts.end(); ++original_itr) {
        if (flag) {
          result += std::string() + '/' + *original_itr;
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

MM::Result<std::size_t, MM::ErrorResult>
MM::FileSystem::FileSystem::DirectorySize(const Path& dir_path) const {
  Result<std::vector<Path>, ErrorResult> get_files_result =
      GetFiles(dir_path).Exception();
  if (!get_files_result.IsSuccess()) {
    return Result<std::size_t, MM::ErrorResult>(
        st_execute_error, get_files_result.GetError().GetErrorCode());
  }

  std::error_code error_code;
  std::size_t directory_sie = 0;
  for (const auto& file : get_files_result.GetResult()) {
    directory_sie += std::filesystem::file_size(file.path_, error_code);

    if (error_code) {
      if (error_code.value() == 2) {
        return Result<std::size_t, ErrorResult>{st_execute_error,
                                                ErrorCode::FILE_IS_NOT_EXIST};
      }

      return Result<std::size_t, ErrorResult>{st_execute_error,
                                              ErrorCode::FILE_OPERATION_ERROR};
    }
  }

  return Result<std::size_t, ErrorResult>{st_execute_success, directory_sie};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::FileSystem::FileSystem::CreateDirectory(const Path& dir_path) const {
  std::error_code error_code;
  if (std::filesystem::create_directory(dir_path.path_, error_code)) {
    return Result<Nil, ErrorResult>{st_execute_success};
  }

  if (error_code.value() == 2) {
    return Result<Nil, ErrorResult>{st_execute_error,
                                    ErrorCode::FILE_IS_NOT_EXIST};
  }

  return Result<Nil, ErrorResult>{st_execute_error,
                                  ErrorCode::FILE_OPERATION_ERROR};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::FileSystem::FileSystem::DeleteDirectory(const Path& dir_path) const {
  std::error_code error_code;
  if (std::filesystem::remove_all(dir_path.path_, error_code)) {
    return Result<Nil, ErrorResult>{st_execute_success};
  }

  if (error_code.value() == 2) {
    return Result<Nil, ErrorResult>{st_execute_error,
                                    ErrorCode::FILE_IS_NOT_EXIST};
  }

  return Result<Nil, ErrorResult>{st_execute_error,
                                  ErrorCode::FILE_OPERATION_ERROR};
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::CopyDirectory(
    const Path& dir_path, const Path& dest_dir) const {
  std::error_code error_code;
  std::filesystem::copy(dir_path.path_, dest_dir.path_, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<Nil, ErrorResult>{st_execute_error,
                                      ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<Nil, ErrorResult>{st_execute_error,
                                    ErrorCode::FILE_OPERATION_ERROR};
  }

  return Result<Nil, ErrorResult>{st_execute_success};
}

MM::Result<MM::Nil, MM::ErrorResult>
MM::FileSystem::FileSystem::RenameDirectory(const Path& dir_path,
                                            const std::string& new_name) const {
  if (dir_path.StringView().compare(new_name.c_str()) == 0) {
    return Result<Nil, ErrorResult>{st_execute_success};
  }

  std::error_code error_code;
  std::filesystem::rename(dir_path.path_, new_name, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<Nil, ErrorResult>{st_execute_error,
                                      ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<Nil, ErrorResult>{st_execute_error,
                                    ErrorCode::FILE_OPERATION_ERROR};
  }

  return Result<Nil, ErrorResult>{st_execute_success};
}

bool MM::FileSystem::FileSystem::DirectoryIsEmpty(const Path& dir_path) const {
  return std::filesystem::is_empty(dir_path.path_);
}

MM::Result<std::vector<MM::FileSystem::Path>, MM::ErrorResult>
MM::FileSystem::FileSystem::GetDirectories(const Path& dir_path) const {
  std::error_code error_code;

  auto directory_or_files =
      std::filesystem::recursive_directory_iterator(dir_path.path_, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<std::vector<Path>, ErrorResult>{
          st_execute_error, ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<std::vector<Path>, ErrorResult>{
        st_execute_error, ErrorCode::FILE_OPERATION_ERROR};
  }

  std::vector<Path> directories{};
  for (const auto& directory_or_file : directory_or_files) {
    if (directory_or_file.is_directory()) {
      directories.emplace_back(
          std::filesystem::path(directory_or_file).string());
    }
  }

  return Result<std::vector<Path>, ErrorResult>{st_execute_success,
                                                std::move(directories)};
}

MM::Result<std::size_t, MM::ErrorResult> MM::FileSystem::FileSystem::FileSize(
    const Path& file_path) const {
  std::error_code error_code;
  std::size_t file_size =
      std::filesystem::file_size(file_path.path_, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<std::size_t, ErrorResult>{st_execute_error,
                                              ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<std::size_t, ErrorResult>{st_execute_error,
                                            ErrorCode::FILE_OPERATION_ERROR};
  }

  return Result<std::size_t, ErrorResult>{st_execute_success, file_size};
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::CreateFile(
    const Path& file_path) const {
  if (file_path.IsExists()) {
    return Result<Nil, ErrorResult>{st_execute_error,
                                    ErrorCode::FILE_OPERATION_ERROR};
  }

  std::fstream create_file;
  create_file.open(file_path.String(), std::ios_base::app);
  if (create_file.is_open()) {
    create_file.close();
    return Result<Nil, ErrorResult>{st_execute_success};
  }
  return Result<Nil, ErrorResult>{st_execute_error,
                                  ErrorCode::FILE_OPERATION_ERROR};
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::DeleteFile(
    const Path& file_path) const {
  std::error_code error_code;
  if (std::filesystem::remove(file_path.path_, error_code)) {
    return Result<Nil, ErrorResult>{st_execute_success};
  }

  return Result<Nil, ErrorResult>{st_execute_error,
                                  ErrorCode::FILE_OPERATION_ERROR};
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::CopyFile(
    const Path& file_path, const Path& dest_dir) const {
  std::error_code error_code;
  if (std::filesystem::copy_file(file_path.path_, dest_dir.path_, error_code)) {
    return Result<Nil, ErrorResult>{st_execute_success};
  }

  if (error_code.value() == 2) {
    return Result<Nil, ErrorResult>{st_execute_error,
                                    ErrorCode::FILE_IS_NOT_EXIST};
  }

  return Result<Nil, ErrorResult>{st_execute_error,
                                  ErrorCode::FILE_OPERATION_ERROR};
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::RenameFile(
    const Path& file_path, const std::string& new_name) const {
  if (file_path.String() == new_name) {
    return Result<Nil, ErrorResult>{st_execute_success};
  }

  std::error_code error_code;
  std::filesystem::rename(file_path.path_, new_name, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<Nil, ErrorResult>{st_execute_error,
                                      ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<Nil, ErrorResult>{st_execute_error,
                                    ErrorCode::FILE_OPERATION_ERROR};
  }

  return Result<Nil, ErrorResult>{st_execute_success};
}

bool MM::FileSystem::FileSystem::FileIsEmpty(const Path& file_path) const {
  return std::filesystem::is_empty(file_path.path_);
}

MM::Result<std::vector<MM::FileSystem::Path>, MM::ErrorResult>
MM::FileSystem::FileSystem::GetFiles(const Path& dir_path) const {
  std::error_code error_code;

  auto directory_or_files =
      std::filesystem::recursive_directory_iterator(dir_path.path_, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<std::vector<Path>, ErrorResult>{
          st_execute_error, ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<std::vector<Path>, ErrorResult>{
        st_execute_error, ErrorCode::FILE_OPERATION_ERROR};
  }

  std::vector<Path> files{};
  for (const auto& directory_or_file : directory_or_files) {
    if (directory_or_file.is_regular_file()) {
      files.emplace_back(std::filesystem::path(directory_or_file).string());
    }
  }

  return Result<std::vector<Path>, ErrorResult>{st_execute_success,
                                                std::move(files)};
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::Create(
    const Path& path) const {
  if (path.IsDirectory()) {
    return CreateDirectory(path);
  }
  return CreateFile(path);
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::Delete(
    const Path& path) const {
  if (path.IsDirectory()) {
    return DeleteDirectory(path);
  }
  return DeleteFile(path);
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::Copy(
    const Path& path, const Path& dest_path) const {
  if (path.IsDirectory()) {
    return CopyDirectory(path, dest_path);
  }
  return CopyFile(path, dest_path);
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::Rename(
    const Path& path, const std::string& new_name) const {
  if (path.IsDirectory()) {
    return RenameDirectory(path, new_name);
  }
  return RenameFile(path, new_name);
}

MM::Result<MM::Nil, MM::ErrorResult> MM::FileSystem::FileSystem::Rename(
    const MM::FileSystem::Path& path,
    const MM::FileSystem::Path& new_path) const {
  if (path.IsDirectory()) {
    return RenameDirectory(path, new_path.String());
  }
  return RenameFile(path, new_path.String());
}

bool MM::FileSystem::FileSystem::IsEmpty(const Path& path) const {
  if (path.IsDirectory()) {
    return DirectoryIsEmpty(path);
  }
  return FileIsEmpty(path);
}

MM::Result<std::size_t, MM::ErrorResult> MM::FileSystem::FileSystem::Size(
    const Path& path) const {
  if (path.IsDirectory()) {
    return DirectorySize(path);
  }
  return FileSize(path);
}

MM::Result<std::vector<MM::FileSystem::Path>, MM::ErrorResult>
MM::FileSystem::FileSystem::GetAll(const Path& path) const {
  std::error_code error_code;

  auto directory_or_files =
      std::filesystem::recursive_directory_iterator(path.path_, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<std::vector<Path>, ErrorResult>{
          st_execute_error, ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<std::vector<Path>, ErrorResult>{
        st_execute_error, ErrorCode::FILE_OPERATION_ERROR};
  }

  std::vector<Path> result{};
  for (const auto& directory_or_file : directory_or_files) {
    if (directory_or_file.is_regular_file() ||
        directory_or_file.is_directory()) {
      result.emplace_back(std::filesystem::path(directory_or_file).string());
    }
  }

  return Result<std::vector<Path>, ErrorResult>{st_execute_success,
                                                std::move(result)};
}

MM::Result<std::vector<char>, MM::ErrorResult>
MM::FileSystem::FileSystem::ReadFile(const MM::FileSystem::Path& path, std::size_t offset, std::size_t read_size) const {
  std::ifstream file(path.CStr(), std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    return Result<std::vector<char>, ErrorResult>{
        st_execute_error, ErrorCode::FILE_OPERATION_ERROR};
  }

  file.seekg(0, std::fstream::end);
  std::size_t file_size = static_cast<std::size_t>(file.tellg());
  std::size_t need_size = 0;
  if (read_size == UINT64_MAX) {
    if (offset >= file_size) {
      return ResultE<>{ErrorCode::FILE_OPERATION_ERROR};
    }
    need_size = file_size - offset;
  } else {
    if (offset + read_size >= file_size) {
      return ResultE<>{ErrorCode::FILE_OPERATION_ERROR};
    }
    need_size = read_size;
  }
  file.seekg(offset);
  std::vector<char> output_data(need_size);
  file.read(output_data.data(), need_size);

  file.close();

  return Result<std::vector<char>, ErrorResult>{st_execute_success,
                                                std::move(output_data)};
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
    if (original_path[index] == '/') {
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
      result += '/';
    }
    ++index;
  }
  return result;
}

void MM::FileSystem::Path::Swap(MM::FileSystem::Path& lhs,
                                MM::FileSystem::Path& rhs) noexcept {
  using std::swap;
  swap(lhs.path_, rhs.path_);
}

void MM::FileSystem::Path::swap(MM::FileSystem::Path& lhs,
                                MM::FileSystem::Path& rhs) noexcept {
  using std::swap;
  swap(lhs.path_, rhs.path_);
}

std::uint64_t MM::FileSystem::Path::GetHash() const {
  std::hash<std::string_view> hash;
  return hash(StringView());
}

std::string_view MM::FileSystem::Path::StringView() const {
  return std::string_view(CStr());
}

const char* MM::FileSystem::Path::CStr() const {
  return reinterpret_cast<const char*>(path_.c_str());
}

MM::FileSystem::Path MM::FileSystem::Path::GetParentDirPath() const {
  if (!IsExists()) {
    return Path{""};
  }
  if (IsDirectory()) {
    return *this;
  }

  return Path{path_.parent_path().string()};
}

MM::FileSystem::FileSystem::~FileSystem() { file_system_ = nullptr; }

MM::Result<MM::FileSystem::LastWriteTime, MM::ErrorResult>
MM::FileSystem::FileSystem::GetLastWriteTime(
    const MM::FileSystem::Path& path) const {
  std::error_code error_code;
  LastWriteTime last_write_time =
      std::filesystem::last_write_time(path.path_, error_code);

  if (error_code) {
    if (error_code.value() == 2) {
      return Result<LastWriteTime, ErrorResult>{st_execute_error,
                                                ErrorCode::FILE_IS_NOT_EXIST};
    }

    return Result<LastWriteTime, ErrorResult>{st_execute_error,
                                              ErrorCode::FILE_OPERATION_ERROR};
  }

  return Result<LastWriteTime, ErrorResult>{st_execute_success,
                                            std::move(last_write_time)};
}

const MM::FileSystem::Path& MM::FileSystem::FileSystem::GetAssetDir() const {
  static Path asset_path(MM_STR(MM_RELATIVE_ASSET_DIR));

  return asset_path;
}

const MM::FileSystem::Path& MM::FileSystem::FileSystem::GetAssetDirStd() const {
  static Path asset_std_path(MM_STR(MM_RELATIVE_ASSET_DIR_STD));

  return asset_std_path;
}

const MM::FileSystem::Path& MM::FileSystem::FileSystem::GetAssetDirUser()
    const {
  static Path asset_user_path(MM_STR(MM_RELATIVE_ASSET_DIR_USER));

  return asset_user_path;
}

const MM::FileSystem::Path& MM::FileSystem::FileSystem::GetAssetDirCache()
    const {
  static Path asset_cache_path(MM_STR(MM_RELATIVE_ASSET_DIR_CACHE));

  return asset_cache_path;
}
