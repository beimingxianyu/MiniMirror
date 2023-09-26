#include "runtime/resource/asset_system/asset_type/Mesh.h"

#include <cstdint>

#include "base/asset_base.h"
#include "base/bounding_box.h"
#include "runtime/platform/base/error.h"

MM::AssetSystem::AssetType::Mesh::Mesh(const FileSystem::Path& mesh_path,
                                       uint32_t mesh_index)
    : AssetBase(mesh_path),
      bounding_box_(
          std::make_unique<MM::AssetSystem::AssetType::RectangleBox>()) {
  if (!AssetBase::IsValid()) {
    MM_LOG_ERROR(std::string("Failed to load the mesh with path ") +
                 mesh_path.StringView().data() +
                 ",because the file does not exist.");
    return;
  }

  LoadModel(mesh_path, mesh_index);

  if (!(AssetBase::IsValid() && bounding_box_ != nullptr && !indexes_.empty() &&
        !vertices_.empty())) {
    AssetBase::Release();
    return;
  }

  SetAssetID(GetAssetID() + mesh_index +
             (static_cast<std::uint64_t>(0x1) << 16));
}

MM::AssetSystem::AssetType::Mesh::Mesh(const FileSystem::Path& asset_path,
                                       AssetID asset_ID,
                                       std::unique_ptr<RectangleBox>&& aabb_box,
                                       std::vector<uint32_t>&& indexes,
                                       std::vector<Vertex>&& vertices)
    : AssetBase(asset_path, asset_ID),
      bounding_box_(std::move(aabb_box)),
      indexes_(std::move(indexes)),
      vertices_(std::move(vertices)) {}

MM::AssetSystem::AssetType::Mesh::Mesh(
    const FileSystem::Path& asset_path, AssetID asset_ID,
    std::unique_ptr<CapsuleBox>&& capsule_box, std::vector<uint32_t>&& indexes,
    std::vector<Vertex>&& vertices)
    : AssetBase(asset_path, asset_ID),
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
         !indexes_.empty() && !vertices_.empty();
}

MM::AssetSystem::AssetType::AssetType
MM::AssetSystem::AssetType::Mesh::GetAssetType() const {
  return AssetType::MESH;
}

uint32_t MM::AssetSystem::AssetType::Mesh::GetVerticesCount() const {
  return vertices_.size();
}

const MM::AssetSystem::AssetType::BoundingBox&
MM::AssetSystem::AssetType::Mesh::GetBoundingBox() const {
  assert(bounding_box_ != nullptr);
  return *bounding_box_;
}

const std::vector<std::uint32_t>& MM::AssetSystem::AssetType::Mesh::GetIndexes()
    const {
  return indexes_;
}

const std::vector<MM::AssetSystem::AssetType::Vertex>&
MM::AssetSystem::AssetType::Mesh::GetVertices() const {
  return vertices_;
}

void MM::AssetSystem::AssetType::Mesh::Release() {
  bounding_box_.reset();
  indexes_.clear();
  vertices_.clear();
  AssetBase::Release();
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
    MM_LOG_ERROR(std::string("Failed to create Mesh.(detail:") +
                 mesh_importer.GetErrorString() + ")");
    return;
  }

  if (scene->mNumMeshes <= mesh_index) {
    MM_LOG_ERROR("The grid index is larger than the maximum index.");
    return;
  }

  ProcessMesh(*(scene->mMeshes[mesh_index]));
}

void MM::AssetSystem::AssetType::Mesh::ProcessMesh(const aiMesh& mesh) {
  if (!mesh.HasPositions()) {
    MM_LOG_ERROR("There is no vertex position information in the mesh.");
    return;
  }
  std::vector<Vertex> vertices;
  vertices.reserve(mesh.mNumVertices);

  const bool have_normal = mesh.HasNormals();
  const bool have_Tangent = mesh.HasTangentsAndBitangents();
  const bool have_texture_coord = mesh.mTextureCoords[0] != nullptr;

  for (unsigned i = 0; i < mesh.mNumVertices; ++i) {
    Vertex temp;
    temp.SetPosition(
        {mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z});
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
    vertices.push_back(std::move(temp));
  }
  vertices_ = std::move(vertices);

  // Generally, unsigned does not overflow.
  std::vector<std::uint32_t> indexes;
  indexes.reserve(mesh.mNumFaces * 3);
  for (unsigned i = 0; i < mesh.mNumFaces; ++i) {
    assert(mesh.mFaces->mNumIndices == 3);
    indexes.emplace_back(mesh.mFaces[i].mIndices[0]);
    indexes.emplace_back(mesh.mFaces[i].mIndices[1]);
    indexes.emplace_back(mesh.mFaces[i].mIndices[2]);
  }
  indexes.shrink_to_fit();
  indexes_ = std::move(indexes);

  bounding_box_->UpdateBoundingBox(*this);
}

std::string MM::AssetSystem::AssetType::Mesh::GetAssetTypeString() const {
  return std::string(MM_ASSET_TYPE_MESH);
}

MM::Result<MM::Utils::Json::Document, MM::ErrorResult>
MM::AssetSystem::AssetType::Mesh::GetJson() const {
  if (!IsValid()) {
    return ResultE<ErrorResult>{ErrorCode::OBJECT_IS_INVALID};
  }

  Utils::Json::Document document{};
  document.SetObject();
  auto& allocator = document.GetAllocator();

  document.AddMember("name",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetName().c_str()),
                     allocator);
  document.AddMember("path",
                     Utils::Json::GenericStringRef<Utils::Json::UTF8<>::Ch>(
                         GetAssetPath().StringView().data()),
                     allocator);
  document.AddMember("asset id", GetAssetID(), allocator);
  document.AddMember("number of indexes", indexes_.size(), allocator);
  document.AddMember("number of vertices", vertices_.size(), allocator);
  Utils::Json::Value bounding_box = bounding_box_->GetJson(allocator);
  document.AddMember("bounding box", bounding_box, allocator);

  return ResultS{std::move(document)};
}

MM::AssetSystem::AssetType::Mesh::Mesh(
    const MM::FileSystem::Path& mesh_path, std::uint32_t mesh_index,
    MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType bounding_box_type)
    : AssetBase(mesh_path) {
  if (!AssetBase::IsValid()) {
    MM_LOG_ERROR(std::string("Failed to load the mesh with path ") +
                 mesh_path.String() + ",because the file does not exist.");
    return;
  }

  if (bounding_box_type == BoundingBox::BoundingBoxType::AABB) {
    bounding_box_ = std::make_unique<RectangleBox>();
  } else if (bounding_box_type == BoundingBox::BoundingBoxType::CAPSULE) {
    bounding_box_ = std::make_unique<CapsuleBox>();
  }

  LoadModel(mesh_path, mesh_index);

  if (!Mesh::IsValid()) {
    AssetBase::Release();
    return;
  }

  std::uint64_t bounding_type_offset = 0;
  switch (GetBoundingBox().GetBoundingType()) {
    case BoundingBox::BoundingBoxType::AABB:
      bounding_type_offset = static_cast<std::uint64_t>(0x1) << 16;
      break;
    case BoundingBox::BoundingBoxType::CAPSULE:
      bounding_type_offset = static_cast<std::uint64_t>(0x1) << 32;
      break;
  }
  SetAssetID(GetAssetID() + mesh_index + bounding_type_offset);
}

MM::Result<MM::AssetSystem::AssetType::AssetID, MM::ErrorResult>
MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
    const MM::FileSystem::Path& path, std::uint32_t index,
    MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType
        bounding_box_type) {
  Result<FileSystem::LastWriteTime, ErrorResult> last_write_time =
      MM_FILE_SYSTEM->GetLastWriteTime(path);
  last_write_time.Exception(
      MM_ERROR_DESCRIPTION(Failed to get last write time.));
  if (last_write_time.IsError()) {
    return ResultE<ErrorResult>{ErrorCode::FILE_OPERATION_ERROR};
  }

  std::uint64_t bounding_type_offset = 0;
  switch (bounding_box_type) {
    case BoundingBox::BoundingBoxType::AABB:
      bounding_type_offset = static_cast<std::uint64_t>(0x1) << 16;
      break;
    case BoundingBox::BoundingBoxType::CAPSULE:
      bounding_type_offset = static_cast<std::uint64_t>(0x1) << 32;
      break;
  }
  AssetID asset_ID = (path.GetHash() ^
                      last_write_time.GetResult().time_since_epoch().count()) +
                     (index + bounding_type_offset);
  return ResultS{asset_ID};
}

std::vector<std::pair<void*, std::uint64_t>>
MM::AssetSystem::AssetType::Mesh::GetDatas() {
  return std::vector<std::pair<void*, std::uint64_t>>{
      std::pair<void*, std::uint64_t>(indexes_.data(),
                                      sizeof(std::uint32_t) * indexes_.size()),
      std::pair<void*, std::uint64_t>(vertices_.data(),
                                      sizeof(Vertex) * vertices_.size())};
}

std::vector<std::pair<const void*, std::uint64_t>>
MM::AssetSystem::AssetType::Mesh::GetDatas() const {
  return std::vector<std::pair<const void*, std::uint64_t>>{
      std::pair<const void*, std::uint64_t>(
          indexes_.data(), sizeof(std::uint32_t) * indexes_.size()),
      std::pair<const void*, std::uint64_t>(vertices_.data(),
                                            sizeof(Vertex) * vertices_.size())};
}

std::uint64_t MM::AssetSystem::AssetType::Mesh::GetSize() const {
  return indexes_.size() * sizeof(std::uint32_t) +
         vertices_.size() * sizeof(Vertex);
}

std::uint32_t MM::AssetSystem::AssetType::Mesh::GetIndexesCount() const {
  return indexes_.size();
}