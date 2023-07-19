#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "runtime/platform/base/error.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/resource/asset_system/asset_type/base/asset_base.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "runtime/resource/asset_system/asset_type/base/bounding_box.h"
#include "runtime/resource/asset_system/asset_type/base/vertex.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
class Mesh : public AssetBase {
 public:
  Mesh() = default;
  ~Mesh() override = default;
  Mesh(const FileSystem::Path& mesh_path, std::uint32_t mesh_index);
  Mesh(const FileSystem::Path& mesh_path, std::uint32_t mesh_index,
       BoundingBox::BoundingBoxType bounding_box_type);
  Mesh(const FileSystem::Path& asset_path, AssetID asset_ID,
       std::unique_ptr<RectangleBox>&& aabb_box,
       std::vector<uint32_t>&& indexes, std::vector<Vertex>&& vertices);
  Mesh(const FileSystem::Path& asset_path, AssetID asset_ID,
       std::unique_ptr<CapsuleBox>&& capsule_box,
       std::vector<uint32_t>&& indexes, std::vector<Vertex>&& vertices);
  Mesh(const Mesh& other) = delete;
  Mesh(Mesh&& other) noexcept;
  Mesh& operator=(const Mesh& other) = delete;
  Mesh& operator=(Mesh&& other) noexcept;

 public:
  std::uint32_t GetVerticesCount() const;

  std::uint32_t GetIndexesCount() const;

  const BoundingBox& GetBoundingBox() const;

  const std::vector<std::uint32_t>& GetIndexes() const;

  const std::vector<Vertex>& GetVertices() const;

  std::uint64_t GetSize() const override;

  std::vector<std::pair<void*, std::uint64_t>> GetDatas() override;

  std::vector<std::pair<const void*, std::uint64_t>> GetDatas() const override;

  bool IsValid() const override;

  std::string GetAssetTypeString() const override;

  AssetType GetAssetType() const override;

  ExecuteResult GetJson(Utils::Json::Document& document) const override;

  static ExecuteResult CalculateAssetID(
      const FileSystem::Path& path, std::uint32_t index,
      AssetSystem::AssetType::BoundingBox::BoundingBoxType bounding_box_type,
      AssetID& asset_ID);

  void Release() override;

 private:
  void LoadModel(const FileSystem::Path& mesh_path, const uint64_t& mesh_index);

  void ProcessMesh(const aiMesh& mesh);

 private:
  std::unique_ptr<BoundingBox> bounding_box_{nullptr};
  std::vector<uint32_t> indexes_{};
  std::vector<Vertex> vertices_{};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
