#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <fstream>
#include <iostream>
#include <utility>

#include "runtime/platform/cross_platform_header/cross_platform_header.h"

namespace MM {
namespace FileSystem {
class FileSystem;

class Path {
  friend FileSystem;

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

  /**
   * \brief Checks whether path refers to existing file system object.
   * \return If the file or directory exists, return true;otherwise, return
   * false.
   */
  bool Exists() const;

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
  std::string FileName() const;

  /**
   * \brief If the path is not a directory, get the file extension.
   * \return The file extension.
   */
  std::string Extension() const;

  /**
   * \brief Exchange data of two objects.
   * \param other Object used for exchange.
   */
  void Swap(const Path& other) const;

  /**
   * \brief Get the relative path relative to root_path.
   * \param root_path root root path.
   * \return Relative path relative to root_path(Obtaining a relative path
   * across drives on a Windows system will return the std::string().).
   */
  std::string RelativePath(const Path& root_path) const;

  /**
   * \brief Get the relative path relative to root_path.
   * \param root_path root root path.
   * \return Relative path relative to root_path(Obtaining a relative path
   * across drives on a Windows system will return the std::string().).
   */
  std::string RelativePath(const std::string& root_path) const;

  /**
   * \brief Get absolute path.
   * \return The absolute path.
   */
  const Path& AbsolutePath() const;

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

  friend void Swap(Path& lhs, Path& rhs) noexcept {
    using std::swap;
    swap(lhs.path_, rhs.path_);
  }

  friend void swap(Path& lhs, Path& rhs) noexcept {
    using std::swap;
    swap(lhs.path_, rhs.path_);
  }

private:
  /**
   * \brief Remove "." and ".." from the path.
   * \param original_path The original path you want to simplify.
   * \return The simplified path.
   */
  static std::string RemoveDotAndDotDot(const std::string& original_path);

private:
  std::unique_ptr<std::filesystem::path> path_;
};

class FileSystem {
  friend std::shared_ptr<FileSystem> std::make_shared();

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
  static std::shared_ptr<MM::FileSystem::FileSystem> GetInstance();

  /**
   * \brief Checks whether path refers to existing file system object.
   * \param path The path you want to check.
   * \return If the file or directory that the path refers to exists, return
   * true; otherwise, false is returned.
   */
  bool Exists(const Path& path) const;

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
   * \return The size of the directory.
   */
  std::size_t DirectorySize(const Path& dir_path) const;

  /**
   * \brief Create directory.
   * \param dir_path The directory you want to create.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool CreateDirectory(const Path& dir_path) const;

  /**
   * \brief Delete directory.
   * \param dir_path The directory path want to delete.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool DeleteDirectory(const Path& dir_path) const;

  /**
   * \brief Copy directory.
   * \param dir_path The directory you want to copy.
   * \param dest_dir The destination directory of the copy file.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool CopyDirectory(const Path& dir_path, const Path& dest_dir) const;

  /**
   * \brief Rename directory.
   * \param dir_path The directory you want to rename.
   * \param new_name The new name will be set.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool RenameDirectory(const Path& dir_path, const std::string& new_name) const;

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
   * \return All directory paths under the specified path.
   */
  std::vector<Path> GetDirectories(const Path& dir_path) const;

  /**
   * \brief Get the size of the file.
   * \param file_path The file of you want get size.
   * \return The size of the file.
   */
  std::size_t FileSize(const Path& file_path) const;

  /**
   * \brief Create file.
   * \param file_path The file you want to create.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool CreateFile(const Path& file_path) const;

  /**
   * \brief Delete file.
   * \param file_path The file you want to delete.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool DeleteFile(const Path& file_path) const;

  /**
   * \brief Copy file.
   * \param file_path The file you want to copy.
   * \param dest_dir The destination directory of the copy file.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool CopyFile(const Path& file_path, const Path& dest_dir) const;

  /**
   * \brief Rename file.
   * \param file_path The file you want to rename.
   * \param new_name The new name will be set.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool RenameFile(const Path& file_path, const std::string& new_name) const;

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
   * \return All file paths under the specified path.
   */
  std::vector<Path> GetFiles(const Path& dir_path) const;

  /**
   * \brief Create file or directory.
   * \param path The file or directory you want to create.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool Create(const Path& path) const;

  /**
   * \brief Delete file or directory.
   * \param path The file or directory you want to delete.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool Delete(const Path& path) const;

  /**
   * \brief Copy file or directory.
   * \param path The file or directory you want to copy.
   * \param dest_path The destination directory of the copy file/directory.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool Copy(const Path& path, const Path& dest_path) const;

  /**
   * \brief Rename file or directory.
   * \param path The file or directory you want to rename.
   * \param new_name The new name will be set.
   * \return Returns true if the operation is successful; otherwise, false is
   * returned.
   */
  bool Rename(const Path& path, const std::string& new_name) const;

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
   * \return The size of the directory or file.
   */
  std::size_t Size(const Path& path) const;

  /**
   * \brief Get all file paths and directory under the specified path.
   * \param path The path you want to check.
   * \return All file and directory paths under the specified path.
   */
  std::vector<Path> GetAll(const Path& path) const;

private:
  ~FileSystem();

  /**
   * \brief Destroy the instance. If it is successfully destroyed, it returns
   * true, otherwise it returns false.
   * \return If it is successfully
   * destroyed, it returns true, otherwise it returns false.
   */
  static bool Destroy();

protected:
  FileSystem() = default;
  static FileSystem* file_system_;

private:
  static std::mutex sync_flag_;
};
}
}  // namespace MM
