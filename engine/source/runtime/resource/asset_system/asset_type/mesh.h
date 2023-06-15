#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "runtime/resource/asset_system/asset_type/base/asset_base.h"
#include "runtime/resource/asset_system/asset_type/base/bounding_box.h"
#include "runtime/resource/asset_system/asset_type/base/vertex.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
class Mesh : public AssetBase {
 public:
  Mesh() = delete;
  ~Mesh() override = default;
  Mesh(const FileSystem::Path& mesh_path, uint32_t mesh_index);
  Mesh(const std::string& asset_name, AssetID asset_ID,
       std::unique_ptr<RectangleBox>&& aabb_box,
       std::unique_ptr<std::vector<uint32_t>>&& indexes,
       std::unique_ptr<std::vector<Vertex>>&& vertices);
  Mesh(const std::string& asset_name, AssetID asset_ID,
       std::unique_ptr<CapsuleBox>&& capsule_box,
       std::unique_ptr<std::vector<uint32_t>>&& indexes,
       std::unique_ptr<std::vector<Vertex>>&& vertices);
  Mesh(const Mesh& other) = delete;
  Mesh(Mesh&& other) noexcept;
  Mesh& operator=(const Mesh& other) = delete;
  Mesh& operator=(Mesh&& other) noexcept;

 public:
  uint32_t GetVerticesCount() const;

  const BoundingBox& GetBoundingBox() const;

  const std::vector<std::uint32_t>& GetIndexes() const;

  const std::vector<Vertex>& GetVertices() const;

  bool IsValid() const override;

  AssetType GetAssetType() const override;

  void Release() override;

 private:
  void LoadModel(const FileSystem::Path& mesh_path, const uint64_t& mesh_index);

  void ProcessMesh(const aiMesh& mesh);

 private:
  std::unique_ptr<BoundingBox> bounding_box_{nullptr};
  std::unique_ptr<std::vector<uint32_t>> indexes_{nullptr};
  std::unique_ptr<std::vector<Vertex>> vertices_{nullptr};
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
