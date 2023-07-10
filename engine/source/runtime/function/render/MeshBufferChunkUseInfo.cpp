//
// Created by beimingxianyu on 23-7-9.
//

#include "runtime/function/render/MeshBufferChunkUseInfo.h"

MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::
    MeshBufferChunkUseInfoLowLevel(std::uint64_t chunk_count,
                                   std::uint64_t unit_chunk_size)
    : chunk_count_(chunk_count),
      free_chunk_count_(chunk_count),
      unit_chunk_size_(unit_chunk_size),
      chunk_use_info_set_(new bool[64]) {
  for (std::uint64_t i = 0; i != chunk_count_; ++i) {
    chunk_use_info_set_[i] = false;
  }
}

MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::
    MeshBufferChunkUseInfoLowLevel(
        MM::RenderSystem::MeshBufferChunkUseInfoLowLevel&& other) noexcept
    : chunk_count_(other.chunk_count_),
      free_chunk_count_(other.free_chunk_count_),
      unit_chunk_size_(other.unit_chunk_size_),
      chunk_use_info_set_(std::move(other.chunk_use_info_set_)) {
  Release();
}

MM::RenderSystem::MeshBufferChunkUseInfoLowLevel&
MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::operator=(
    MM::RenderSystem::MeshBufferChunkUseInfoLowLevel&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  chunk_count_ = other.chunk_count_;
  free_chunk_count_ = other.free_chunk_count_;
  unit_chunk_size_ = other.unit_chunk_size_;
  chunk_use_info_set_ = std::move(other.chunk_use_info_set_);

  other.Release();

  return *this;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::GetUsedCount()
    const {
  return chunk_count_ - free_chunk_count_;
}

bool MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::Get(
    std::uint64_t index) {
  assert(index < chunk_count_);
  return chunk_use_info_set_[index];
}

bool MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::Any() const {
  return free_chunk_count_ != chunk_count_;
}

bool MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::None() const {
  return !Any();
}

bool MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::All() const {
  return free_chunk_count_ == 0;
}

void MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::MarkUse(
    std::uint64_t index) {
  assert(index < chunk_count_);
  assert(chunk_use_info_set_[index] != true);

  chunk_use_info_set_[index] = true;
  ++free_chunk_count_;
}

void MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::MarkUse(
    std::uint64_t offset_index, std::uint64_t size) {
  assert(offset_index + size < chunk_count_);

  for (std::uint64_t i = 0; i != size; ++i) {
    assert(chunk_use_info_set_[offset_index + i] != true);
    chunk_use_info_set_[offset_index + i] = true;
  }
  free_chunk_count_ += size;
}

void MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::MarkUnused(
    std::uint64_t index) {
  assert(index < chunk_count_);
  assert(chunk_use_info_set_[index]);

  chunk_use_info_set_[index] = false;
  --free_chunk_count_;
}

void MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::MarkUnused(
    std::uint64_t offset_index, std::uint64_t size) {
  assert(offset_index + size < chunk_count_);

  for (std::uint64_t i = 0; i != size; ++i) {
    assert(chunk_use_info_set_[offset_index + i]);
    chunk_use_info_set_[offset_index + i] = false;
  }
  free_chunk_count_ -= size;
}

std::pair<std::uint64_t, std::uint64_t>
MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::FindFreeChunk(
    std::uint64_t require_size, std::uint64_t start_index) {
  assert(start_index < chunk_count_);

  if (free_chunk_count_ * unit_chunk_size_ < require_size &&
      (chunk_count_ - start_index) * unit_chunk_size_ < require_size) {
    return std::make_pair(0, 0);
  }

  std::uint64_t free_size = 0;
  std::uint64_t offset_index = 0;
  std::uint64_t free_chunk_size = 0;
  for (std::uint64_t i = start_index; i < chunk_count_; ++i) {
    if (chunk_use_info_set_[i]) {
      offset_index = i;
      for (; chunk_use_info_set_[i] && i < chunk_count_; ++i) {
        free_size += unit_chunk_size_;
        ++free_chunk_size;
        if (free_size >= require_size) {
          return std::make_pair(offset_index, free_chunk_size);
        }
      }

      free_size = 0;
      free_chunk_size = 0;
    }
  }

  return std::make_pair(0, 0);
}

std::pair<std::uint64_t, std::uint64_t>
MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::FindFreeChunk(
    std::uint64_t require_size) {
  return FindFreeChunk(require_size, 0);
}

std::pair<std::uint64_t, std::uint64_t>
MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::FindFreeChunkBack(
    std::uint64_t require_size, std::uint64_t end_index) {
  assert(end_index <= chunk_count_);

  if (free_chunk_count_ * unit_chunk_size_ < require_size &&
      end_index * unit_chunk_size_ < require_size) {
    return std::make_pair(0, 0);
  }

  std::uint64_t free_size = 0;
  std::uint64_t offset_index = 0;
  std::uint64_t free_chunk_size = 0;
  for (std::uint64_t i = end_index; i != UINT64_MAX; --i) {
    if (chunk_use_info_set_[i]) {
      offset_index = i;
      for (; chunk_use_info_set_[i] && i != UINT64_MAX; --i) {
        free_size += unit_chunk_size_;
        ++free_chunk_size;
        if (free_size >= require_size) {
          return std::make_pair(offset_index, free_chunk_size);
        }
      }

      if (i == UINT64_MAX) {
        return std::make_pair(0, 0);
      }

      free_size = 0;
      free_chunk_size = 0;
    }
  }

  return std::make_pair(0, 0);
}

std::pair<std::uint64_t, std::uint64_t>
MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::FindFreeChunkBack(
    std::uint64_t require_size) {
  return FindFreeChunkBack(require_size, chunk_count_);
}

void MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::Release() {
  chunk_count_ = 0;
  free_chunk_count_ = 0;
  unit_chunk_size_ = 0;
  chunk_use_info_set_.reset();
}

bool MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::IsValid() const {
  return chunk_count_ != 0 && unit_chunk_size_ != 0 &&
         chunk_use_info_set_ != nullptr;
}

std::uint64_t
MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::GetUnitChunkSize() const {
  return unit_chunk_size_;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::GetChunkCount()
    const {
  return chunk_count_;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoLowLevel::GetFreeCount()
    const {
  return free_chunk_count_;
}

MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::
    MeshBufferChunkUseInfoHighLevel(std::uint64_t high_level_chunk_count,
                                    std::uint64_t low_level_chunk_count,
                                    std::uint64_t unit_chunk_size)
    : low_level_chunk_use_info_count_(high_level_chunk_count),
      free_low_level_chunk_use_info_count_(high_level_chunk_count),
      low_level_chunk_use_info_set_(
          new MeshBufferChunkUseInfoLowLevel[high_level_chunk_count]),
      low_level_chunk_count_(low_level_chunk_count),
      high_level_free_chunk_count_(low_level_chunk_count *
                                   high_level_chunk_count) {
  for (std::uint64_t i = 0; i != high_level_chunk_count; ++i) {
    low_level_chunk_use_info_set_[i] =
        MeshBufferChunkUseInfoLowLevel{low_level_chunk_count, unit_chunk_size};
  }
}

MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::
    MeshBufferChunkUseInfoHighLevel(
        MM::RenderSystem::MeshBufferChunkUseInfoHighLevel&& other) noexcept
    : low_level_chunk_use_info_count_(other.low_level_chunk_use_info_count_),
      free_low_level_chunk_use_info_count_(
          other.free_low_level_chunk_use_info_count_),
      low_level_chunk_use_info_set_(
          std::move(other.low_level_chunk_use_info_set_)),
      low_level_chunk_count_(other.low_level_chunk_count_),
      high_level_free_chunk_count_(other.high_level_free_chunk_count_) {
  other.Release();
}

MM::RenderSystem::MeshBufferChunkUseInfoHighLevel&
MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::operator=(
    MM::RenderSystem::MeshBufferChunkUseInfoHighLevel&& other) noexcept {
  if (std::addressof(other) == this) {
    return *this;
  }

  low_level_chunk_use_info_count_ = other.low_level_chunk_use_info_count_;
  free_low_level_chunk_use_info_count_ =
      other.free_low_level_chunk_use_info_count_;
  low_level_chunk_use_info_set_ =
      std::move(other.low_level_chunk_use_info_set_);
  low_level_chunk_count_ = other.low_level_chunk_count_;
  high_level_free_chunk_count_ = other.high_level_free_chunk_count_;

  other.Release();

  return *this;
}

std::uint64_t
MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::GetLowLevelChunkInfoCount()
    const {
  return low_level_chunk_use_info_count_;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::
    GetFreeLowLevelChunkInfoCount() const {
  return free_low_level_chunk_use_info_count_;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::
    GetUsedLowLevelChunkInfoCount() const {
  return GetLowLevelChunkInfoCount() - GetFreeLowLevelChunkInfoCount();
}

std::uint64_t
MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::GetLowLevelChunkCount()
    const {
  return low_level_chunk_count_;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::GetChunkCount()
    const {
  return low_level_chunk_use_info_count_ * GetLowLevelChunkCount();
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::GetUsedCount()
    const {
  assert(GetChunkCount() > high_level_free_chunk_count_);

  return GetChunkCount() - high_level_free_chunk_count_;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::GetUsedCount(
    std::uint64_t low_level_chunk_use_info_index) const {
  assert(low_level_chunk_use_info_index < low_level_chunk_use_info_count_);

  return low_level_chunk_use_info_set_[low_level_chunk_use_info_index]
      .GetUsedCount();
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::GetFreeCount()
    const {
  return high_level_free_chunk_count_;
}

std::uint64_t MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::GetFreeCount(
    std::uint64_t low_level_chunk_use_info_index) const {
  assert(low_level_chunk_use_info_index < low_level_chunk_use_info_count_);

  return low_level_chunk_use_info_set_[low_level_chunk_use_info_index]
      .GetFreeCount();
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::Get(
    std::uint64_t high_level_index, std::uint64_t low_level_index) {
  assert(high_level_index < low_level_chunk_use_info_count_);

  return low_level_chunk_use_info_set_[high_level_index].Get(low_level_index);
}

MM::RenderSystem::MeshBufferChunkUseInfoLowLevel&
MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::Get(
    std::uint64_t low_level_chunk_use_info_index) {
  return low_level_chunk_use_info_set_[low_level_chunk_use_info_index];
}

const MM::RenderSystem::MeshBufferChunkUseInfoLowLevel&
MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::Get(
    std::uint64_t low_level_chunk_use_info_index) const {
  return low_level_chunk_use_info_set_[low_level_chunk_use_info_index];
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::Any() const {
  return GetUsedCount() != 0;
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::Any(
    std::uint64_t low_level_chunk_use_info_index) const {
  assert(low_level_chunk_use_info_index < low_level_chunk_use_info_count_);

  return low_level_chunk_use_info_set_[low_level_chunk_use_info_index].Any();
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::None() const {
  return !Any();
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::None(
    std::uint64_t low_level_chunk_use_info_index) const {
  assert(low_level_chunk_use_info_index < low_level_chunk_use_info_count_);

  return low_level_chunk_use_info_set_[low_level_chunk_use_info_index].None();
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::All() const {
  if (IsValid()) {
    return GetUsedCount() == GetChunkCount();
  }

  return true;
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::All(
    std::uint64_t low_level_chunk_use_info_index) const {
  assert(low_level_chunk_use_info_index < low_level_chunk_use_info_count_);

  return low_level_chunk_use_info_set_[low_level_chunk_use_info_index].All();
}

void MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::MarkUse(
    std::uint64_t high_level_index, std::uint64_t low_level_index) {
  assert(high_level_index < low_level_chunk_use_info_count_);

  low_level_chunk_use_info_set_[high_level_index].MarkUse(low_level_index);
  --high_level_free_chunk_count_;
  if (low_level_chunk_use_info_set_[high_level_index].GetUsedCount() == 1) {
    --free_low_level_chunk_use_info_count_;
  }
}

void MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::MarkUse(
    std::uint64_t high_level_offset, std::uint64_t low_level_offset_index,
    std::uint64_t size) {
  assert(high_level_offset < low_level_chunk_use_info_count_);
  assert((low_level_chunk_use_info_count_ - high_level_offset) *
                 GetLowLevelChunkInfoCount() -
             low_level_offset_index >=
         size);

  while (size != 0) {
    std::uint64_t current_low_level_chunk_use_info_free_count =
        GetLowLevelChunkCount() - low_level_offset_index;
    if (current_low_level_chunk_use_info_free_count >= size) {
      low_level_chunk_use_info_set_[high_level_offset].MarkUse(
          low_level_offset_index, size);
      if (low_level_chunk_use_info_set_[high_level_offset].GetUsedCount() ==
          size) {
        --free_low_level_chunk_use_info_count_;
      }
      high_level_free_chunk_count_ -= size;
      size = 0;
    } else {
      low_level_chunk_use_info_set_[high_level_offset].MarkUse(
          low_level_offset_index, current_low_level_chunk_use_info_free_count);
      if (low_level_chunk_use_info_set_[high_level_offset].GetUsedCount() ==
          current_low_level_chunk_use_info_free_count) {
        --free_low_level_chunk_use_info_count_;
      }
      high_level_free_chunk_count_ -=
          current_low_level_chunk_use_info_free_count;
      size -= current_low_level_chunk_use_info_free_count;
      ++high_level_offset;
      low_level_offset_index = 0;
    }
  }
}

void MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::MarkUnused(
    std::uint64_t high_level_index, std::uint64_t low_level_index) {
  assert(high_level_index < low_level_chunk_use_info_count_);

  low_level_chunk_use_info_set_[high_level_index].MarkUnused(low_level_index);
  ++high_level_free_chunk_count_;
  if (low_level_chunk_use_info_set_[high_level_index].GetUsedCount() == 0) {
    ++free_low_level_chunk_use_info_count_;
  }
}

void MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::MarkUnused(
    std::uint64_t high_level_offset, std::uint64_t low_level_offset_index,
    std::uint64_t size) {
  assert(high_level_offset < low_level_chunk_use_info_count_);
  assert((low_level_chunk_use_info_count_ - high_level_offset) *
                 GetLowLevelChunkInfoCount() -
             low_level_offset_index >=
         size);

  while (size != 0) {
    std::uint64_t current_low_level_chunk_use_info_free_count =
        GetLowLevelChunkCount() - low_level_offset_index;
    if (current_low_level_chunk_use_info_free_count >= size) {
      low_level_chunk_use_info_set_[high_level_offset].MarkUnused(
          low_level_offset_index, size);
      if (low_level_chunk_use_info_set_[high_level_offset].GetUsedCount() ==
          0) {
        ++free_low_level_chunk_use_info_count_;
      }
      high_level_free_chunk_count_ += size;
      size = 0;
    } else {
      low_level_chunk_use_info_set_[high_level_offset].MarkUnused(
          low_level_offset_index, current_low_level_chunk_use_info_free_count);
      if (low_level_chunk_use_info_set_[high_level_offset].GetUsedCount() ==
          0) {
        ++free_low_level_chunk_use_info_count_;
      }
      high_level_free_chunk_count_ +=
          current_low_level_chunk_use_info_free_count;
      size -= current_low_level_chunk_use_info_free_count;
      ++high_level_offset;
      low_level_offset_index = 0;
    }
  }
}

void MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::Release() {
  low_level_chunk_use_info_count_ = 0;
  free_low_level_chunk_use_info_count_ = 0;
  low_level_chunk_use_info_set_.reset();
  low_level_chunk_count_ = 0;
  high_level_free_chunk_count_ = 0;
}

bool MM::RenderSystem::MeshBufferChunkUseInfoHighLevel::IsValid() const {
  return low_level_chunk_use_info_count_ != 0 &&
         free_low_level_chunk_use_info_count_ != 0 &&
         low_level_chunk_use_info_set_ != nullptr &&
         low_level_chunk_count_ != 0 && high_level_free_chunk_count_ != 0;
}
