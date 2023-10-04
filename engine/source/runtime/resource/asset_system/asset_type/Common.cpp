//
// Created by beimingxianyu on 23-9-30.
//

#include "runtime/resource/asset_system/asset_type/Common.h"

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::Common::GetAssetType() const {
  assert(IsValid());

  return AssetType::COMMON;
}

std::string MM::AssetSystem::AssetType::Common::GetAssetTypeString() const {
  assert(IsValid());

  return MM_ASSET_TYPE_COMMON;
}

uint64_t MM::AssetSystem::AssetType::Common::GetSize() const {
  assert(IsValid());

  std::size_t size = 0;
  for (const auto& one_vector : data_) {
    size += one_vector.size();
  }

  return size;
}

MM::Result<rapidjson::Document, MM::ErrorResult>
MM::AssetSystem::AssetType::Common::GetJson() const {
  Result<Utils::Json::Document> document_result = AssetBase::GetJson();
  if (document_result.IsError()) {
    return document_result;
  }

  Utils::Json::Document& document = document_result.GetResult();
  Utils::Json::Value data_array{Utils::Json::Type::kArrayType};
  for (const auto& data : data_) {
    Utils::Json::Value one_data_value{};
    one_data_value.SetInt64(data.size());
    data_array.PushBack(one_data_value, document.GetAllocator());
  }
  document.AddMember("datas size", data_array, document.GetAllocator());

  return document_result;
}

std::vector<std::pair<const void*, std::uint64_t>>
MM::AssetSystem::AssetType::Common::GetDatas() const {
  assert(IsValid());

  std::vector<std::pair<const void*, std::uint64_t>> out_data;
  out_data.reserve(data_.size());
  for (const auto& data : data_) {
    out_data.emplace_back(data.data(), data.size());
  }

  return out_data;
}

void MM::AssetSystem::AssetType::Common::Release() {
  data_.clear();
  AssetBase::Release();
}

MM::AssetSystem::AssetType::Common::Common(
    const MM::FileSystem::Path& path, std::uint64_t offset, std::uint64_t size,
    const std::function<Result<std::vector<std::vector<char>>, ErrorResult>(
        const FileSystem::Path&, std::uint64_t, std::uint64_t)>&
        load_function) {
  Result<std::vector<std::vector<char>>, ErrorResult> load_result =
      load_function(path, offset, size)
          .Exception(MM_ERROR_DESCRIPTION(Failed to load common result.));
  if (load_result.IsError()) {
    return;
  }

  data_ = std::move(load_result.GetResult());
}

MM::AssetSystem::AssetType::Common::Common(
    MM::AssetSystem::AssetType::Common&& other) noexcept
    : AssetBase(std::move(other)), data_(std::move(other.data_)) {}

MM::AssetSystem::AssetType::Common&
MM::AssetSystem::AssetType::Common::operator=(
    MM::AssetSystem::AssetType::Common&& other) noexcept {
  if (this == std::addressof(other)) {
    return *this;
  }

  AssetBase::operator=(std::move(other));
  data_ = std::move(other.data_);

  return *this;
}

MM::Result<std::vector<std::vector<char>>, MM::ErrorResult>
MM::AssetSystem::AssetType::Common::DefaultLoadFunction(
    const MM::FileSystem::Path& path, std::uint64_t offset,
    std::uint64_t size) {
  Result<std::vector<char>> read_data =
      MM_FILE_SYSTEM->ReadFile(path, offset, size);
  if (read_data.Exception().IsError()) {
    return ResultE<>{read_data.GetError().GetErrorCode()};
  }

  return ResultS{
      std::vector<std::vector<char>>{std::move(read_data.GetResult())}};
}
