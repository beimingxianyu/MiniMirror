#include "runtime/resource/asset_type/mesh.h"


MM::AssetType::Mesh::Mesh(const std::string& asset_name,
    const FileSystem::Path& mesh_path, const uint32_t& mesh_index) :
  AssetBase(asset_name, MM::Utils::StringHash(mesh_path.String())) {
  LoadModel(mesh_path, mesh_index);
}

MM::AssetType::Mesh::Mesh(const std::string& asset_name,
                          const uint64_t& asset_ID,
                          const AABBBox& aabb_box,
                          const std::shared_ptr<std::vector<uint32_t>>& indexes,
                          const std::shared_ptr<std::vector<Vertex>>& vertices) :
  AssetBase(asset_name, asset_ID), bounding_box_(std::make_unique<BoundingBox>(aabb_box)), indexes_(indexes), vertices_(vertices)
{}

MM::AssetType::Mesh::Mesh(
    const std::string& asset_name, const uint64_t& asset_ID,
    const CapsuleBox& capsule_box,
    const std::shared_ptr<std::vector<uint32_t>>& indexes,
    const std::shared_ptr<std::vector<Vertex>>& vertices)
  : AssetBase(asset_name, asset_ID),
    bounding_box_(std::make_unique<BoundingBox>(capsule_box)),
    indexes_(indexes),
    vertices_(vertices) {
}


MM::AssetType::Mesh::Mesh(const Mesh& other)
  : bounding_box_(std::make_unique<BoundingBox>(*other.bounding_box_)),
    indexes_(other.indexes_), vertices_(other.vertices_){}

MM::AssetType::Mesh::Mesh(Mesh&& other) noexcept {

}

MM::AssetType::Mesh& MM::AssetType::Mesh::operator=(const Mesh& other) {
  if (&other == this) {
    return *this;
  }
  bounding_box_ = std::make_unique<BoundingBox>(*other.bounding_box_);
  indexes_ = other.indexes_;
  vertices_ = other.vertices_;

  return *this;
}

MM::AssetType::Mesh& MM::AssetType::Mesh::operator=(Mesh&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  bounding_box_ = std::make_unique<BoundingBox>(*other.bounding_box_);
  indexes_ = other.indexes_;
  vertices_ = other.vertices_;

  other.Release();

  return *this;
}

bool MM::AssetType::Mesh::IsValid() const {
  return indexes_.use_count() != 0 && vertices_.use_count() != 0 && bounding_box_->IsValid();
}

MM::AssetType::AssetType MM::AssetType::Mesh::GetAssetType() {
  return AssetType::MESH;
}

uint32_t MM::AssetType::Mesh::GetVerticesCount() const {
  return vertices_->size();
}

const MM::AssetType::BoundingBox& MM::AssetType::Mesh::GetBoundingBox() const {
  return *bounding_box_;
}

std::shared_ptr<const std::vector<uint32_t>> MM::AssetType::Mesh::
GetIndexes() const {
  return indexes_;
}

std::shared_ptr<const std::vector<MM::AssetType::Vertex>> MM::AssetType::
Mesh::GetVertices() const {
  return vertices_;
}

void MM::AssetType::Mesh::Release() {
  bounding_box_.release();
  indexes_.reset();
  vertices_.reset();
}

void MM::AssetType::Mesh::LoadModel(const FileSystem::Path& mesh_path,
    const uint64_t& mesh_index) {
  if (!mesh_path.Exists()) {
    return;
  }
  Assimp::Importer mesh_importer;
  const aiScene* scene = mesh_importer.ReadFile(
      mesh_path.String().c_str(),
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode)
  {
    LOG_ERROR(std::string("Failed to create Mesh.(detail:") +
              mesh_importer.GetErrorString() + ")");
    return;
  }

  if (scene->mNumMeshes <= mesh_index) {
    LOG_ERROR("The grid index is larger than the maximum index.")
    return;
  }

  ProcessMesh(*(scene->mMeshes[mesh_index]));
}

void MM::AssetType::Mesh::ProcessMesh(const aiMesh& mesh) {
  if (!mesh.HasPositions()) {
    LOG_ERROR("There is no vertex position information in the mesh.")
    return;
  }
  vertices_->reserve(mesh.mNumVertices);

  const bool have_normal = mesh.HasNormals();
  const bool have_Tangent = mesh.HasTangentsAndBitangents();

  Vertex temp;
  for (unsigned i = 0; i < mesh.mNumVertices; ++i) {
    temp.Reset();
    temp.SetPosition({mesh.mVertices->x, mesh.mVertices->y, mesh.mVertices->z});
    // TODO Added support for vertex multi texture coordinates.
    if (mesh.mTextureCoords[0]) {
      temp.SetTextureCoord(
          Math::vec2{mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y});
    }
    if (have_normal) {
      temp.SetNormal(Math::vec3{mesh.mNormals[i].x, mesh.mNormals[i].y,
                                mesh.mNormals[i].z});
    }
    if (have_Tangent) {
      temp.SetTangent(Math::vec3{mesh.mTangents[i].x, mesh.mTangents[i].y,
                                 mesh.mTangents[i].z});
      temp.SetBiTangent(Math::vec3{mesh.mBitangents[i].x, mesh.mBitangents[i].y,
                                   mesh.mBitangents[i].z});
    }
  }

  // Generally, unsigned does not overflow.
  indexes_->reserve(mesh.mNumFaces * 3);
  for (unsigned i = 0; i < mesh.mNumFaces; ++i) {
    assert(mesh.mFaces->mNumIndices == 3);
    indexes_->emplace_back(mesh.mFaces->mIndices[0]);
    indexes_->emplace_back(mesh.mFaces->mIndices[1]);
    indexes_->emplace_back(mesh.mFaces->mIndices[2]);
  }
}
