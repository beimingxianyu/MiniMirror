#pragma once

#include <cassert>
#include <cstdint>
#include <memory>

namespace MM {
namespace RenderSystem {
std::uint32_t g_expansion_factor1 = 64;
std::uint32_t g_expansion_factor2 = 32;

std::uint64_t g_vertex_chunk_unit_size1 = 8192;
std::uint64_t g_index_chunk_unit_size1 = 2048;
std::uint64_t g_vertex_chunk_unit_size2 =
    g_vertex_chunk_unit_size1 * g_expansion_factor1;
std::uint64_t g_index_chunk_unit_size2 =
    g_index_chunk_unit_size1 * g_expansion_factor1;
std::uint64_t g_vertex_chunk_unit_size3 =
    g_vertex_chunk_unit_size2 * g_expansion_factor2;
std::uint64_t g_index_chunk_unit_size3 =
    g_index_chunk_unit_size2 * g_expansion_factor2;

class MeshBufferChunkUseInfoLowLevel {
 public:
  MeshBufferChunkUseInfoLowLevel() = default;
  ~MeshBufferChunkUseInfoLowLevel() = default;
  MeshBufferChunkUseInfoLowLevel(std::uint64_t chunk_count,
                                 std::uint64_t unit_chunk_size);
  MeshBufferChunkUseInfoLowLevel(const MeshBufferChunkUseInfoLowLevel& other) =
      delete;
  MeshBufferChunkUseInfoLowLevel(
      MeshBufferChunkUseInfoLowLevel&& other) noexcept;
  MeshBufferChunkUseInfoLowLevel& operator=(
      const MeshBufferChunkUseInfoLowLevel& other) = delete;
  MeshBufferChunkUseInfoLowLevel& operator=(
      MeshBufferChunkUseInfoLowLevel&& other) noexcept;

 public:
  std::uint64_t GetUnitChunkSize() const;

  std::uint64_t GetChunkCount() const;

  std::uint64_t GetUsedCount() const;

  std::uint64_t GetFreeCount() const;

  bool Get(std::uint64_t index);

  bool Any() const;

  bool None() const;

  bool All() const;

  void MarkUse(std::uint64_t index);

  void MarkUse(std::uint64_t offset_index, std::uint64_t size);

  void MarkUnused(std::uint64_t index);

  void MarkUnused(std::uint64_t offset_index, std::uint64_t size);

  std::uint64_t GetForwardFreeChunkCount(std::uint64_t start_index) const;

  std::uint64_t GetForwardFreeSize(std::uint64_t start_index) const;

  std::uint64_t GetForwardFreeChunkCount() const;

  std::uint64_t GetForwardFreeSize() const;

  std::uint64_t GetReverseFreeChunkCount(std::uint64_t end_index) const;

  std::uint64_t GetReverseFreeSize(std::uint64_t end_index) const;

  std::uint64_t GetReverseFreeChunkCount() const;

  std::uint64_t GetReverseFreeSize() const;

  /**
   * Find free chunks of sufficient size.
   * \param require_size The required size, in bytes.
   * \param start_index The index of the starting chunk for the search.
   * \return Return a pair which first number is offset of free chunk, second
   * member is number of free chunk.When suitable chunks cannot be found, both
   * members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunk(
      std::uint64_t require_size, std::uint64_t start_index) const;

  /**
   * Find free chunks of sufficient size.
   * \param require_size The required size, in bytes.
   * \return Return a pair which first number is offset of free chunk, second
   * member is number of free chunk.When suitable chunks cannot be found, both
   * members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunk(
      std::uint64_t require_size) const;

  /**
   * Find free chunks of sufficient size.Reverse search.
   * \param require_size The required size, in bytes.
   * \param end_index The index of the ending chunk for the search.
   * \return Return a pair which first number is offset of free chunk, second
   * member is number of free chunk.When suitable chunks cannot be found, both
   * members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunkReverse(
      std::uint64_t require_size, std::uint64_t end_index) const;

  /**
   * Find free chunks of sufficient size.Reverse search.
   * \param require_size The required size, in bytes.
   * \return Return a pair which first number is offset of free chunk, second
   * member is number of free chunk.When suitable chunks cannot be found, both
   * members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunkReverse(
      std::uint64_t require_size) const;

  void Release();

  bool IsValid() const;

 private:
  std::uint64_t chunk_count_{0};
  std::uint64_t free_chunk_count_{0};
  std::uint64_t unit_chunk_size_{0};
  std::unique_ptr<bool[]> chunk_use_info_set_{nullptr};
};

class MeshBufferChunkUseInfoHighLevel {
 public:
  MeshBufferChunkUseInfoHighLevel() = default;
  ~MeshBufferChunkUseInfoHighLevel() = default;
  MeshBufferChunkUseInfoHighLevel(std::uint64_t high_level_chunk_count,
                                  std::uint64_t low_level_chunk_count,
                                  std::uint64_t unit_chunk_size);
  MeshBufferChunkUseInfoHighLevel(
      const MeshBufferChunkUseInfoHighLevel& other) = delete;
  MeshBufferChunkUseInfoHighLevel(
      MeshBufferChunkUseInfoHighLevel&& other) noexcept;
  MeshBufferChunkUseInfoHighLevel& operator=(
      const MeshBufferChunkUseInfoHighLevel& other) = delete;
  MeshBufferChunkUseInfoHighLevel& operator=(
      MeshBufferChunkUseInfoHighLevel&& other) noexcept;

 public:
  std::uint64_t GetLowLevelChunkInfoCount() const;

  std::uint64_t GetFreeLowLevelChunkInfoCount() const;

  std::uint64_t GetUsedLowLevelChunkInfoCount() const;

  std::uint64_t GetLowLevelChunkCount() const;

  std::uint64_t GetChunkCount() const;

  std::uint64_t GetUsedCount() const;

  std::uint64_t GetUsedCount(
      std::uint64_t low_level_chunk_use_info_index) const;

  std::uint64_t GetFreeCount() const;

  std::uint64_t GetFreeCount(
      std::uint64_t low_level_chunk_use_info_index) const;

  bool Get(std::uint64_t high_level_index, std::uint64_t low_level_index);

  MeshBufferChunkUseInfoLowLevel& Get(
      std::uint64_t low_level_chunk_use_info_index);

  const MeshBufferChunkUseInfoLowLevel& Get(
      std::uint64_t low_level_chunk_use_info_index) const;

  bool Any() const;

  bool Any(std::uint64_t low_level_chunk_use_info_index) const;

  bool None() const;

  bool None(std::uint64_t low_level_chunk_use_info_index) const;

  bool All() const;

  bool All(std::uint64_t low_level_chunk_use_info_index) const;

  void MarkUse(std::uint64_t high_level_index, std::uint64_t low_level_index);

  void MarkUse(std::uint64_t high_level_offset,
               std::uint64_t low_level_offset_index, std::uint64_t size);

  void MarkUnused(std::uint64_t high_level_index,
                  std::uint64_t low_level_index);

  void MarkUnused(std::uint64_t high_level_offset,
                  std::uint64_t low_level_offset_index, std::uint64_t size);

  /**
   * Find free chunks of sufficient size.
   * \param require_size The required size, in bytes.
   * \param high_level_start_index The index of the starting chunk use info for
   * the search.
   * \param low_level_start_index The index of the starting chunk
   * for the search.
   * \return Return a pair which first number is offset of free
   * chunk, second member is number of free chunk.When suitable chunks cannot be
   * found, both members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunk(
      std::uint64_t require_size, std::uint64_t high_level_start_index,
      std::uint64_t low_level_start_index) {
    assert(high_level_start_index <= low_level_chunk_use_info_count_);
  }

  /**
   * Find free chunks of sufficient size.
   * \param require_size The required size, in bytes.
   * \return Return a pair which first number is offset of free chunk, second
   * member is number of free chunk.When suitable chunks cannot be found, both
   * members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunk(
      std::uint64_t require_size);

  /**
   * Find free chunks of sufficient size.Reverse search.
   * \param require_size The required size, in bytes.
   * \param high_level_end_index The index of the ending chunk use info for the
   * search. \param low_level_end_index The index of the ending chunk for the
   * search. \return Return a pair which first number is offset of free chunk,
   * second member is number of free chunk.When suitable chunks cannot be found,
   * both members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunkReverse(
      std::uint64_t require_size, std::uint64_t high_level_end_index,
      std::uint64_t low_level_end_index);

  /**
   * Find free chunks of sufficient size.Reverse search.
   * \param require_size The required size, in bytes.
   * \return Return a pair which first number is offset of free chunk, second
   * member is number of free chunk.When suitable chunks cannot be found, both
   * members are 0.
   */
  std::pair<std::uint64_t, std::uint64_t> FindFreeChunkReverse(
      std::uint64_t require_size);

  void Release();

  bool IsValid() const;

 private:
  std::uint64_t low_level_chunk_use_info_count_{0};
  std::uint64_t free_low_level_chunk_use_info_count_{0};
  std::unique_ptr<MeshBufferChunkUseInfoLowLevel[]>
      low_level_chunk_use_info_set_{nullptr};
  std::uint64_t low_level_chunk_count_{0};
  std::uint64_t high_level_free_chunk_count_{0};
};
}  // namespace RenderSystem
}  // namespace MM
