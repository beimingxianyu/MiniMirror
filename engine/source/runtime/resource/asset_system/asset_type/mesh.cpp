#include "runtime/resource/asset_system/asset_type/mesh.h"

MM::AssetSystem::AssetType::Mesh::Mesh(const FileSystem::Path& mesh_path,
                                       uint32_t mesh_index)
    : AssetBase(mesh_path) {
  if (AssetBase::IsValid()) {
    LOG_ERROR(std::string("Failed to load the mesh with path ") +
              mesh_path.String() + ",because the file does not exist.");
    return;
  }

  LoadModel(mesh_path, mesh_index);
}

MM::AssetSystem::AssetType::Mesh::Mesh(
    const std::string& asset_name, MM::AssetSystem::AssetType::AssetID asset_ID,
    std::unique_ptr<RectangleBox>&& aabb_box,
    std::unique_ptr<std::vector<uint32_t>>&& indexes,
    std::unique_ptr<std::vector<Vertex>>&& vertices)
    : AssetBase(asset_name, asset_ID),
      bounding_box_(std::move(aabb_box)),
      indexes_(std::move(indexes)),
      vertices_(std::move(vertices)) {}

MM::AssetSystem::AssetType::Mesh::Mesh(
    const std::string& asset_name, MM::AssetSystem::AssetType::AssetID asset_ID,
    std::unique_ptr<CapsuleBox>&& capsule_box,
    std::unique_ptr<std::vector<uint32_t>>&& indexes,
    std::unique_ptr<std::vector<Vertex>>&& vertices)
    : AssetBase(asset_name, asset_ID),
      bounding_box_(std::move(capsule_box)),
      indexes_(std::move(indexes)),
      vertices_(std::move(vertices)) {}

MM::AssetSystem::AssetType::Mesh::Mesh(Mesh&& other) noexcept
    : AssetBase(std::move(other)),
      bounding_box_(std::move(other.bounding_box_)),
      indexes_(std::move(other.indexes_)),
      vertices_(std::move(other.vertices_)) {}

MM::AssetSystem::AssetType::Mesh& MM::AssetSystem::AssetType::Mesh::operator=(
    Mesh&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  AssetBase::operator=(std::move(other));
  bounding_box_ = std::move(other.bounding_box_);
  indexes_ = std::move(other.indexes_);
  vertices_ = std::move(other.vertices_);

  return *this;
}

bool MM::AssetSystem::AssetType::Mesh::IsValid() const {
  return AssetBase::IsValid() && bounding_box_ != nullptr &&
         indexes_ != nullptr && vertices_ != nullptr;
}

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::Mesh::GetAssetType() const {
  return AssetType::MESH;
}

uint32_t MM::AssetSystem::AssetType::Mesh::GetVerticesCount() const {
  return vertices_->size();
}

const MM::AssetSystem::AssetType::BoundingBox&
MM::AssetSystem::AssetType::Mesh::GetBoundingBox() const {
  return *bounding_box_;
}

const std::vector<std::uint32_t>& MM::AssetSystem::AssetType::Mesh::GetIndexes()
    const {
  return *indexes_;
}

const std::vector<MM::AssetSystem::AssetType::Vertex>&
MM::AssetSystem::AssetType::Mesh::GetVertices() const {
  return *vertices_;
}

void MM::AssetSystem::AssetType::Mesh::Release() {
  bounding_box_.reset();
  indexes_.reset();
  vertices_.reset();
}

void MM::AssetSystem::AssetType::Mesh::LoadModel(
    const FileSystem::Path& mesh_path, const uint64_t& mesh_index) {
  if (!mesh_path.IsExists()) {
    return;
  }
  Assimp::Importer mesh_importer;
  const aiScene* scene = mesh_importer.ReadFile(
      mesh_path.String().c_str(),
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
          aiProcess_MakeLeftHanded | aiProcess_GenNormals |
          aiProcess_OptimizeMeshes);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    LOG_ERROR(std::string("Failed to create Mesh.(detail:") +
              mesh_importer.GetErrorString() + ")");
    return;
  }

  if (scene->mNumMeshes <= mesh_index) {
    LOG_ERROR("The grid index is larger than the maximum index.");
    return;
  }

  ProcessMesh(*(scene->mMeshes[mesh_index]));
}

void MM::AssetSystem::AssetType::Mesh::ProcessMesh(const aiMesh& mesh) {
  if (!mesh.HasPositions()) {
    LOG_ERROR("There is no vertex position information in the mesh.");
    return;
  }
  vertices_->reserve(mesh.mNumVertices);

  const bool have_normal = mesh.HasNormals();
  const bool have_Tangent = mesh.HasTangentsAndBitangents();
  const bool have_texture_coord = mesh.mTextureCoords[0] != nullptr;

  Vertex temp;
  for (unsigned i = 0; i < mesh.mNumVertices; ++i) {
    temp.Reset();
    temp.SetPosition({mesh.mVertices->x, mesh.mVertices->y, mesh.mVertices->z});
    // TODO Added support for vertex multi texture coordinates.
    if (have_texture_coord) {
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
    vertices_->push_back(temp);
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
