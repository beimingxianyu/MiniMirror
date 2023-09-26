#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <locale>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "runtime/platform/base/cross_platform_header.h"
#include "utils/error.h"
#include "utils/marco.h"
#include "utils/type_utils.h"

namespace MM {
namespace FileSystem {
using LastWriteTime = std::filesystem::file_time_type;

class FileSystem;
class Path;

std::string __GetCurrentPath__();

const std::string g_bin_dir = __GetCurrentPath__();
class Path {
  friend FileSystem;
  friend std::hash<Path>;

 public:
  Path() = default;
  ~Path() = default;
  Path(const Path& other);
  Path(Path&& other) noexcept;
  Path(const std::string& other);
  Path& operator=(const Path& other);
  Path& operator=(Path&& other) noexcept;
  bool operator==(const Path& other) const;
  const Path operator+(const std::string& path) const;
  Path& operator+=(const std::string& path);
  explicit operator std::string() const;
  explicit operator std::filesystem::path() const;

 public:
  /**
   * \brief Get source path data.
   * \return The source path data.
   */
  std::string String() const;

  std::string_view StringView() const;

  const char* CStr() const;

  /**
   * \brief Checks whether path refers to existing file system object.
   * \return If the file or directory exists, return true;otherwise, return
   * false.
   */
  bool IsExists() const;

  /**
   * \brief Check if the path is a directory.
   * \return If the path is a directory, return true; Otherwise, false is
   * returned.
   */
  bool IsDirectory() const;

  /**
   * \brief If the path is not a directory, get the file name.
   * \return The file name.
   */
  std::string GetFileName() const;

  /**
   * \brief If the path is not a directory, get the file name.
   * \return The file name.
   */
  std::string_view GetFileNameView() const;

  /**
   * \brief If the path is not a directory, get the file extension.
   * \return The file extension.
   */
  std::string GetExtension() const;

  /**
   * \brief If the path is not a directory, get the file extension.
   * \return The file extension.
   */
  std::string_view GetExtensionView() const;

  /**
   * \brief Exchange data of two objects.
   * \param other Object used for exchange.
   */
  void Swap(Path& other);

  /**
   * \brief Get the relative path relative to root_path.
   * \param root_path root root path.
   * \return Relative path relative to root_path(Obtaining a relative path
   * across drives on a Windows system will return the std::string().).
   */
  std::string GetRelativePath(const Path& root_path) const;

  /**
   * \brief Get the relative path relative to root_path.
   * \param root_path root root path.
   * \return Relative path relative to root_path(Obtaining a relative path
   * across drives on a Windows system will return the std::string().).
   */
  std::string GetRelativePath(const std::string& root_path) const;

  /**
   * \brief Get absolute path.
   * \return The absolute path.
   */
  const Path& GetAbsolutePath() const;

  /**
   * \brief Replace the original path.
   * \param other_path The path to be replaced.
   */
  void ReplacePath(const Path& other_path);

  /**
   * \brief Replace the original path.
   * \param other_path The path to be replaced.
   */
  void ReplacePath(const std::string& other_path);

  Path GetParentDirPath() const;

  /**
   * \brief Get hash value of path..
   */
  std::uint64_t GetHash() const;

  static void Swap(Path& lhs, Path& rhs) noexcept;

  static void swap(Path& lhs, Path& rhs) noexcept;

 private:
  /**
   * \brief Remove "." and ".." from the path.
   * \param original_path The original path you want to simplify.
   * \return The simplified path.
   */
  static std::string RemoveDotAndDotDot(const std::string& original_path);

 private:
  std::filesystem::path path_;
};

class FileSystem {
 public:
  FileSystem(const FileSystem&) = delete;
  FileSystem(const FileSystem&&) = delete;
  FileSystem& operator=(const FileSystem&) = delete;
  FileSystem& operator=(const FileSystem&&) = delete;

 public:
  /**
   * \brief Create Instance.
   * \return A FileSystem pointer.
   */
  static MM::FileSystem::FileSystem* GetInstance();

  /**
   * \brief Checks whether path refers to existing file system object.
   * \param path The path you want to check.
   * \return If the file or directory that the path refers to exists, return
   * true; otherwise, false is returned.
   */
  bool IsExists(const Path& path) const;

  /**
   * \brief Check if the path is a directory.
   * \param path The path you want to check.
   * \return If the path is a directory, return true; Otherwise, false is
   * returned.
   */
  bool IsDirectory(const Path& path) const;

  /**
   * \brief Get the size of the directory.
   * \param dir_path The directory of you want get size.
   * \return Return directory size or error.
   */
  Result<std::size_t, ErrorResult> DirectorySize(const Path& dir_path) const;

  /**
   * \brief Create directory.
   * \param dir_path The directory you want to create.
   * \return Return error code.
   */
  Result<Nil, ErrorResult> CreateDirectory(const Path& dir_path) const;

  /**
   * \brief Delete directory.
   * \param dir_path The directory path want to delete.
   * \return Return error code.
   */
  Result<Nil, ErrorResult> DeleteDirectory(const Path& dir_path) const;

  /**
   * \brief Copy directory.
   * \param dir_path The directory you want to copy.
   * \param dest_dir The destination directory of the copy file.
   * \return Return error code.
   */
  Result<Nil, ErrorResult> CopyDirectory(const Path& dir_path,
                                         const Path& dest_dir) const;

  /**
   * \brief Rename directory.
   * \param dir_path The directory you want to rename.
   * \param new_name The new name will be set.
   * \return Return error code.
   */
  Result<Nil, ErrorResult> RenameDirectory(const Path& dir_path,
                                           const std::string& new_name) const;

  /**
   * \brief Check whether the specified directory is empty.
   * \param dir_path The directory you want to check.
   * \return Returns false if the operation is fail or the directory does not
   * exist; otherwise, true is returned.
   */
  bool DirectoryIsEmpty(const Path& dir_path) const;

  /**
   * \brief Get all directory paths under the specified path.
   * \param dir_path The path you want to check.
   * \return Return directories or error.
   */
  Result<std::vector<Path>, ErrorResult> GetDirectories(
      const Path& dir_path) const;

  /**
   * \brief Get the size of the file.
   * \param file_path The file of you want get size.
   * \return The file size or error.
   */
  Result<std::size_t, ErrorResult> FileSize(const Path& file_path) const;

  /**
   * \brief Create file.
   * \param file_path The file you want to create.
   * \return Return result code.
   */
  Result<Nil, ErrorResult> CreateFile(const Path& file_path) const;

  /**
   * \brief Delete file.
   * \param file_path The file you want to delete.
   * \return Return result code.
   */
  Result<Nil, ErrorResult> DeleteFile(const Path& file_path) const;

  /**
   * \brief Copy file.
   * \param file_path The file you want to copy.
   * \param dest_dir The destination directory of the copy file.
   * \return Return result code.
   */
  Result<Nil, ErrorResult> CopyFile(const Path& file_path,
                                    const Path& dest_dir) const;

  /**
   * \brief Rename file.
   * \param file_path The file you want to rename.
   * \param new_name The new name will be set.
   * \return Return result code.
   */
  Result<Nil, ErrorResult> RenameFile(const Path& file_path,
                                      const std::string& new_name) const;

  /**
   * \brief Check whether the specified file is empty.
   * \param file_path The path you want to check.
   * \return Returns false if the operation is fail or the directory does not
   * exist; otherwise, true is returned.
   */
  bool FileIsEmpty(const Path& file_path) const;

  /**
   * \brief Get all file paths under the specified path.
   * \param dir_path The path you want to check.
   * \return Return files or error.
   */
  Result<std::vector<Path>, ErrorResult> GetFiles(const Path& dir_path) const;

  /**
   * \brief Create file or directory.
   * \param path The file or directory you want to create.
   * \return Returns error code.
   */
  Result<Nil, ErrorResult> Create(const Path& path) const;

  /**
   * \brief Delete file or directory.
   * \param path The file or directory you want to delete.
   * \return Returns error code.
   */
  Result<Nil, ErrorResult> Delete(const Path& path) const;

  /**
   * \brief Copy file or directory.
   * \param path The file or directory you want to copy.
   * \param dest_path The destination directory of the copy file/directory.
   * \return Returns error code.
   */
  Result<Nil, ErrorResult> Copy(const Path& path, const Path& dest_path) const;

  /**
   * \brief Rename file or directory.
   * \param path The file or directory you want to rename.
   * \param new_name The new name will be set.
   * \return Returns error code.
   */
  Result<Nil, ErrorResult> Rename(const Path& path,
                                  const std::string& new_name) const;

  /**
   * \brief Rename file or directory.
   * \param path The file or directory you want to rename.
   * \param new_name The new name will be set.
   * \return Returns error code.
   */
  Result<Nil, ErrorResult> Rename(const Path& path, const Path& new_path) const;

  /**
   * \brief Check whether the specified file or directory is empty.
   * \param path The path you want to check.
   * \return Returns false if the operation is fail or the directory does not
   * exist; otherwise, true is returned.
   */
  bool IsEmpty(const Path& path) const;

  /**
   * \brief Get the size of the directory or file.
   * \param path The directory or file of you want get size.
   * \return Return size or error.
   */
  Result<std::size_t, ErrorResult> Size(const Path& path) const;

  /**
   * \brief Get all file paths and directory under the specified path.
   * \param path The path you want to check.
   * \return Return all file or error.
   */
  Result<std::vector<MM::FileSystem::Path>, ErrorResult> GetAll(
      const Path& path) const;

  /**
   * \brief Get file/directory last write time.
   * \return The time of the file/directory last write time or error.
   */
  Result<LastWriteTime, ErrorResult> GetLastWriteTime(const Path& path) const;

  Result<std::vector<char>, ErrorResult> ReadFile(
      const MM::FileSystem::Path& path) const;

  const Path& GetAssetDir() const;

  const Path& GetAssetDirStd() const;

  const Path& GetAssetDirUser() const;

  const Path& GetAssetDirCache() const;

 private:
  /**
   * \brief Destroy the instance. If it is successfully destroyed, it returns
   * true, otherwise it returns false.
   * \return If it is successfully
   * destroyed, it returns true, otherwise it returns false.
   */
  static bool Destroy();

 protected:
  FileSystem() = default;
  ~FileSystem();
  static FileSystem* file_system_;

 private:
  static std::mutex sync_flag_;
};

#define MM_FILE_SYSTEM MM_file_system

#define MM_IMPORT_FILE_SYSTEM                               \
  inline const MM::FileSystem::FileSystem* MM_FILE_SYSTEM { \
    MM::FileSystem::FileSystem::GetInstance()               \
  }
}  // namespace FileSystem
}  // namespace MM

namespace std {
template <>
struct hash<MM::FileSystem::Path> {
  size_t operator()(const MM::FileSystem::Path& path) const {
    return path.GetHash();
  }
};
}  // namespace std