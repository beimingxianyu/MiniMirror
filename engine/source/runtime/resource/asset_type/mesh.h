#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "runtime/resource/asset_type/base/bounding_box.h"
#include "runtime/resource/asset_type/base/asset_base.h"
#include "runtime/resource/asset_type/base/vertex.h"

namespace MM {
namespace AssetType {
class Mesh : public AssetBase{
 public:
  Mesh() = default;
  ~Mesh() override = default;
  Mesh(const std::string& asset_name, const FileSystem::Path& mesh_path,
       const uint32_t& mesh_index);
  Mesh(const std::string& asset_name,
       const uint64_t& asset_ID,
       const AABBBox& aabb_box,
       const std::shared_ptr<std::vector<uint32_t>>& indexes,
       const std::shared_ptr<std::vector<Vertex>>& vertices);
  Mesh(const std::string& asset_name, const uint64_t& asset_ID,
       const CapsuleBox& capsule_box,
       const std::shared_ptr<std::vector<uint32_t>>& indexes,
       const std::shared_ptr<std::vector<Vertex>>& vertices);
  Mesh(const Mesh& other);
  Mesh(Mesh&& other) noexcept;
  Mesh& operator=(const Mesh& other);
  Mesh& operator=(Mesh&& other) noexcept;

 public:
  uint32_t GetVerticesCount() const;

  const BoundingBox& GetBoundingBox() const;

  std::shared_ptr<const std::vector<uint32_t>> GetIndexes() const;

  std::shared_ptr<const std::vector<Vertex>> GetVertices() const;

  bool IsValid() const override;

  AssetType GetAssetType() override;

  void Release() override;

private:
  void LoadModel(const FileSystem::Path& mesh_path,
                  const uint64_t& mesh_index);

  void ProcessMesh(const aiMesh& mesh);

 private:
  std::unique_ptr<BoundingBox>
                    bounding_box_{nullptr};
  std::shared_ptr<std::vector<uint32_t>> indexes_{nullptr};
  std::shared_ptr<std::vector<Vertex>> vertices_{nullptr};
};
}  // namespace AssetType
}  // namespace MM
