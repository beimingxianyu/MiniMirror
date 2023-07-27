#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "runtime/platform/base/cross_platform_header.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace FileSystem {
using LastWriteTime = std::filesystem::file_time_type;

class FileSystem;

const std::string g_bin_dir = std::filesystem::current_path().string();

class Path {
  friend FileSystem;
  friend std::hash<Path>;

 public:
  Path() = delete;
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

  friend void Swap(Path& lhs, Path& rhs) noexcept;

  friend void swap(Path& lhs, Path& rhs) noexcept;

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
   * \param directory_size The size of the directory.
   * \return Return error code.
   */
  ExecuteResult DirectorySize(const Path& dir_path,
                              std::size_t& directory_size) const;

  /**
   * \brief Create directory.
   * \param dir_path The directory you want to create.
   * \return Return error code.
   */
  ExecuteResult CreateDirectory(const Path& dir_path) const;

  /**
   * \brief Delete directory.
   * \param dir_path The directory path want to delete.
   * \return Return error code.
   */
  ExecuteResult DeleteDirectory(const Path& dir_path) const;

  /**
   * \brief Copy directory.
   * \param dir_path The directory you want to copy.
   * \param dest_dir The destination directory of the copy file.
   * \return Return error code.
   */
  ExecuteResult CopyDirectory(const Path& dir_path, const Path& dest_dir) const;

  /**
   * \brief Rename directory.
   * \param dir_path The directory you want to rename.
   * \param new_name The new name will be set.
   * \return Return error code.
   */
  ExecuteResult RenameDirectory(const Path& dir_path,
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
   * \param directories The result.
   * \return Return error code.
   */
  ExecuteResult GetDirectories(const Path& dir_path,
                               std::vector<Path>& directories) const;

  /**
   * \brief Get the size of the file.
   * \param file_path The file of you want get size.
   * \param file_size The result.
   * \return The result code.
   */
  ExecuteResult FileSize(const Path& file_path, std::size_t& file_size) const;

  /**
   * \brief Create file.
   * \param file_path The file you want to create.
   * \return Return result code.
   */
  ExecuteResult CreateFile(const Path& file_path) const;

  /**
   * \brief Delete file.
   * \param file_path The file you want to delete.
   * \return Return result code.
   */
  ExecuteResult DeleteFile(const Path& file_path) const;

  /**
   * \brief Copy file.
   * \param file_path The file you want to copy.
   * \param dest_dir The destination directory of the copy file.
   * \return Return result code.
   */
  ExecuteResult CopyFile(const Path& file_path, const Path& dest_dir) const;

  /**
   * \brief Rename file.
   * \param file_path The file you want to rename.
   * \param new_name The new name will be set.
   * \return Return result code.
   */
  ExecuteResult RenameFile(const Path& file_path,
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
   * \param files All file paths under the specified path.
   * \return Return error code.
   */
  ExecuteResult GetFiles(const Path& dir_path, std::vector<Path>& files) const;

  /**
   * \brief Create file or directory.
   * \param path The file or directory you want to create.
   * \return Returns error code.
   */
  ExecuteResult Create(const Path& path) const;

  /**
   * \brief Delete file or directory.
   * \param path The file or directory you want to delete.
   * \return Returns error code.
   */
  ExecuteResult Delete(const Path& path) const;

  /**
   * \brief Copy file or directory.
   * \param path The file or directory you want to copy.
   * \param dest_path The destination directory of the copy file/directory.
   * \return Returns error code.
   */
  ExecuteResult Copy(const Path& path, const Path& dest_path) const;

  /**
   * \brief Rename file or directory.
   * \param path The file or directory you want to rename.
   * \param new_name The new name will be set.
   * \return Returns error code.
   */
  ExecuteResult Rename(const Path& path, const std::string& new_name) const;

  /**
   * \brief Rename file or directory.
   * \param path The file or directory you want to rename.
   * \param new_name The new name will be set.
   * \return Returns error code.
   */
  ExecuteResult Rename(const Path& path, const Path& new_path) const;

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
   * \param size The size of the directory or file.
   * \return Return error code.
   */
  ExecuteResult Size(const Path& path, std::size_t& size) const;

  /**
   * \brief Get all file paths and directory under the specified path.
   * \param path The path you want to check.
   * \param all_sub_path All file and directory paths under the specified path.
   * \return Return error code.
   */
  ExecuteResult GetAll(const Path& path, std::vector<Path>& all_sub_path) const;

  /**
   * \brief Get file/directory last write time.
   * \param path The path you want to check.
   * \return The time of the file/directory last write time.
   */
  ExecuteResult GetLastWriteTime(const Path& path,
                                 LastWriteTime& last_write_time) const;

  ExecuteResult ReadFile(const MM::FileSystem::Path& path,
                         std::vector<char>& output_data) const;

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