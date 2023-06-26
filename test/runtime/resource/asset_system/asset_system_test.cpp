//
// Created by beimingxianyu on 23-6-22.
//

#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "runtime/platform/base/error.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/resource/asset_system/AssetManager.h"
#include "runtime/resource/asset_system/AssetSystem.h"
#include "runtime/resource/asset_system/asset_type/Combination.h"
#include "runtime/resource/asset_system/asset_type/Image.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "runtime/resource/asset_system/asset_type/base/bounding_box.h"
#include "utils/error.h"

TEST(asset_system, asset_manager) {
  struct ImageImageMeshMesh : public MM::AssetSystem::AssetType::Combination {
    explicit ImageImageMeshMesh(const MM::FileSystem::Path& json_path)
        : MM::AssetSystem::AssetType::Combination(json_path) {}

    MM::AssetSystem::AssetManager::AssetHandler GetHandler(
        std::uint64_t index) {
      return Get(index);
    }

    std::vector<MM::AssetSystem::AssetManager::AssetHandler>&
    GetAssetHandlers2() {
      return GetAssetHandlers();
    }

    const std::vector<MM::AssetSystem::AssetManager::AssetHandler>&
    GetAssetHandlers2() const {
      return GetAssetHandlers();
    }

    bool IsValid() const {
      bool result = true;
      if (GetAssetHandlers().empty()) {
        return false;
      }
      for (const auto& h : GetAssetHandlers()) {
        if (!h.IsValid()) {
          result = false;
        }
      }
      return result;
    }
  };

  struct ImageImageMesh : public MM::AssetSystem::AssetType::Combination {
    explicit ImageImageMesh(const MM::FileSystem::Path& json_path)
        : MM::AssetSystem::AssetType::Combination(json_path) {}

    MM::AssetSystem::AssetManager::AssetHandler GetHandler(
        std::uint64_t index) {
      return Get(index);
    }

    std::vector<MM::AssetSystem::AssetManager::AssetHandler>&
    GetAssetHandlers2() {
      return GetAssetHandlers();
    }

    const std::vector<MM::AssetSystem::AssetManager::AssetHandler>&
    GetAssetHandlers2() const {
      return GetAssetHandlers();
    }

    bool IsValid() const {
      bool result = true;
      if (GetAssetHandlers().empty()) {
        return false;
      }
      for (const auto& h : GetAssetHandlers()) {
        if (!h.IsValid()) {
          result = false;
        }
      }
      return result;
    }
  };

  MM::FileSystem::Path json_path1{std::string(TEST_FILE_DIR_TEST) +
                                  "/asset_system/combination1.json"},
      json_path2{std::string(TEST_FILE_DIR_TEST) +
                 "/asset_system/combination2.json"},
      json_path3{std::string(TEST_FILE_DIR_TEST) +
                 "/asset_system/combination3.json"},
      json_path4{std::string(TEST_FILE_DIR_TEST) +
                 "/asset_system/combination4.json"};
  ImageImageMeshMesh asset1(json_path1), asset2(json_path2);
  ImageImageMesh asset3(json_path3), asset4(json_path4);

  MM::FileSystem::LastWriteTime last_write_time1, last_write_time2,
      last_write_time3, last_write_time4;

  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                json_path1, last_write_time1),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                json_path2, last_write_time2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                json_path3, last_write_time3),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                json_path4, last_write_time4),
            MM::ExecuteResult::SUCCESS);

  MM::AssetSystem::AssetType::AssetID
      asset_id1 =
          json_path1.GetHash() ^ last_write_time1.time_since_epoch().count(),
      asset_id2 = 0,
      asset_id3 =
          json_path3.GetHash() ^ last_write_time3.time_since_epoch().count(),
      asset_id4 =
          json_path4.GetHash() ^ last_write_time4.time_since_epoch().count();

  ASSERT_EQ(asset1.IsValid(), true);
  ASSERT_EQ(asset2.IsValid(), false);
  ASSERT_EQ(asset3.IsValid(), true);
  ASSERT_EQ(asset4.IsValid(), true);
  ASSERT_EQ(asset1.GetAssetID(), asset_id1);
  ASSERT_EQ(asset2.GetAssetID(), asset_id2);
  ASSERT_EQ(asset3.GetAssetID(), asset_id3);
  ASSERT_EQ(asset4.GetAssetID(), asset_id4);

  // ----------------------------------------------------------------
  // asset1
  auto& image1_1 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *asset1.GetHandler(0).GetObject());
  auto& image1_2 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *asset1.GetHandler(1).GetObject());
  auto& mesh1_1 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *asset1.GetHandler(2).GetObject());
  auto& mesh1_2 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *asset1.GetHandler(3).GetObject());

  MM::FileSystem::Path path1_1(std::string(TEST_FILE_DIR_TEST) +
                               "/asset_system/test_picture1.jpg"),
      path1_2(std::string(TEST_FILE_DIR_TEST) +
              "/asset_system/test_picture2.png"),
      path1_3(std::string(TEST_FILE_DIR_TEST) + "/asset_system/monkey.obj"),
      path1_4(std::string(TEST_FILE_DIR_TEST) + "/asset_system/model.fbx");
  MM::AssetSystem::AssetType::AssetID asset_id1_1, asset_id1_2, asset_id1_3,
      asset_id1_4;
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(path1_1, 3,
                                                                asset_id1_1),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(path1_2, 4,
                                                                asset_id1_2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path1_3, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                asset_id1_3),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
          path1_4, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE,
          asset_id1_4),
      MM::ExecuteResult::SUCCESS);

  // image1_1
  ASSERT_EQ(image1_1.GetAssetID(), asset_id1_1);
  ASSERT_EQ(image1_1.IsValid(), true);
  ASSERT_EQ(image1_1.GetAssetName(), path1_1.GetFileName());
  ASSERT_EQ(image1_1.GetAssetPath(), path1_1);
  ASSERT_EQ(image1_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image1_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image1_1.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB);
  ASSERT_EQ(image1_1.GetImageChannels(), 3);
  ASSERT_EQ(image1_1.GetImageWidth(), 1200);
  ASSERT_EQ(image1_1.GetImageHeight(), 1200);
  ASSERT_EQ(image1_1.GetImageSize(),
            image1_1.GetImageWidth() * image1_1.GetImageHeight() * 3);
  ASSERT_NE(image1_1.GetPixelsData(), nullptr);

  // image1_2
  ASSERT_EQ(image1_2.GetAssetID(), asset_id1_2);
  ASSERT_EQ(image1_2.IsValid(), true);
  ASSERT_EQ(image1_2.GetAssetName(), path1_2.GetFileName());
  ASSERT_EQ(image1_2.GetAssetPath(), path1_2);
  ASSERT_EQ(image1_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image1_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image1_2.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB_ALPHA);
  ASSERT_EQ(image1_2.GetImageChannels(), 4);
  ASSERT_EQ(image1_2.GetImageWidth(), 541);
  ASSERT_EQ(image1_2.GetImageHeight(), 1200);
  ASSERT_EQ(image1_2.GetImageSize(),
            image1_2.GetImageWidth() * image1_2.GetImageHeight() * 4);
  ASSERT_NE(image1_2.GetPixelsData(), nullptr);

  // mesh1_1
  ASSERT_EQ(mesh1_1.GetAssetID(), asset_id1_3);
  ASSERT_EQ(mesh1_1.IsValid(), true);
  ASSERT_EQ(mesh1_1.GetAssetName(), path1_3.GetFileName());
  ASSERT_EQ(mesh1_1.GetAssetPath(), path1_3);
  ASSERT_EQ(mesh1_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh1_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh1_1.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB);
  const MM::AssetSystem::AssetType::RectangleBox& mesh1_1_box =
      dynamic_cast<const MM::AssetSystem::AssetType::RectangleBox&>(
          mesh1_1.GetBoundingBox());
  ASSERT_EQ(mesh1_1_box.IsValid(), true);
  ASSERT_FLOAT_EQ(mesh1_1_box.GetLeft(), -1.36718789);
  ASSERT_FLOAT_EQ(mesh1_1_box.GetBottom(), -0.984375);
  ASSERT_FLOAT_EQ(mesh1_1_box.GetForward(), -0.851562023);
  ASSERT_FLOAT_EQ(mesh1_1_box.GetRight(), 1.36718789);
  ASSERT_FLOAT_EQ(mesh1_1_box.GetTop(), 0.984375);
  ASSERT_FLOAT_EQ(mesh1_1_box.GetBack(), 0.851562023);

  // mesh1_2
  ASSERT_EQ(mesh1_2.GetAssetID(), asset_id1_4);
  ASSERT_EQ(mesh1_2.IsValid(), true);
  ASSERT_EQ(mesh1_2.GetAssetName(), path1_4.GetFileName());
  ASSERT_EQ(mesh1_2.GetAssetPath(), path1_4);
  ASSERT_EQ(mesh1_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh1_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh1_2.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE);
  const MM::AssetSystem::AssetType::CapsuleBox& mesh1_2_box =
      dynamic_cast<const MM::AssetSystem::AssetType::CapsuleBox&>(
          mesh1_2.GetBoundingBox());
  ASSERT_FLOAT_EQ(mesh1_2_box.GetRadius(), 4.4917974);
  ASSERT_FLOAT_EQ(mesh1_2_box.GetTop(), 2.19192767);
  ASSERT_FLOAT_EQ(mesh1_2_box.GetBottom(), -1.02342343);
  // ----------------------------------------------------------------

  // ----------------------------------------------------------------
  // asset3
  auto& image3_1 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *asset3.GetHandler(0).GetObject());
  auto& image3_2 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *asset3.GetHandler(1).GetObject());
  auto& mesh3_1 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *asset3.GetHandler(2).GetObject());

  MM::FileSystem::Path path3_1(std::string(TEST_FILE_DIR_TEST) +
                               "/asset_system/test_picture1.jpg"),
      path3_2(std::string(TEST_FILE_DIR_TEST) +
              "/asset_system/test_picture2.png"),
      path3_3(std::string(TEST_FILE_DIR_TEST) + "/asset_system/model.fbx");
  MM::AssetSystem::AssetType::AssetID asset_id3_1, asset_id3_2, asset_id3_3;
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(path3_1, 2,
                                                                asset_id3_1),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(path3_2, 4,
                                                                asset_id3_2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path3_3, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                asset_id3_3),
            MM::ExecuteResult::SUCCESS);

  // image3_1
  ASSERT_EQ(image3_1.GetAssetID(), asset_id3_1);
  ASSERT_EQ(image3_1.IsValid(), true);
  ASSERT_EQ(image3_1.GetAssetName(), path3_1.GetFileName());
  ASSERT_EQ(image3_1.GetAssetPath(), path3_1);
  ASSERT_EQ(image3_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image3_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image3_1.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::GREY_ALPHA);
  ASSERT_EQ(image3_1.GetImageChannels(), 2);
  ASSERT_EQ(image3_1.GetImageWidth(), 1200);
  ASSERT_EQ(image3_1.GetImageHeight(), 1200);
  ASSERT_EQ(image3_1.GetImageSize(),
            image3_1.GetImageWidth() * image3_1.GetImageHeight() * 2);
  ASSERT_NE(image3_1.GetPixelsData(), nullptr);

  // image3_2
  ASSERT_EQ(image3_2.GetAssetID(), asset_id3_2);
  ASSERT_EQ(image3_2.IsValid(), true);
  ASSERT_EQ(image3_2.GetAssetName(), path3_2.GetFileName());
  ASSERT_EQ(image3_2.GetAssetPath(), path3_2);
  ASSERT_EQ(image3_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image3_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image3_2.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB_ALPHA);
  ASSERT_EQ(image3_2.GetImageChannels(), 4);
  ASSERT_EQ(image3_2.GetImageWidth(), 541);
  ASSERT_EQ(image3_2.GetImageHeight(), 1200);
  ASSERT_EQ(image3_2.GetImageSize(),
            image3_2.GetImageWidth() * image3_2.GetImageHeight() * 4);
  ASSERT_NE(image3_2.GetPixelsData(), nullptr);

  // mesh3_1
  ASSERT_EQ(mesh3_1.GetAssetID(), asset_id3_3);
  ASSERT_EQ(mesh3_1.IsValid(), true);
  ASSERT_EQ(mesh3_1.GetAssetName(), path3_3.GetFileName());
  ASSERT_EQ(mesh3_1.GetAssetPath(), path3_3);
  ASSERT_EQ(mesh3_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh3_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh3_1.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB);
  const MM::AssetSystem::AssetType::RectangleBox& mesh3_1_box =
      dynamic_cast<const MM::AssetSystem::AssetType::RectangleBox&>(
          mesh3_1.GetBoundingBox());
  ASSERT_FLOAT_EQ(mesh3_1_box.GetLeft(), -1.02342343);
  ASSERT_FLOAT_EQ(mesh3_1_box.GetBottom(), -1.02342343);
  ASSERT_FLOAT_EQ(mesh3_1_box.GetForward(), -4.18217993);
  ASSERT_FLOAT_EQ(mesh3_1_box.GetRight(), 4.33429909);
  ASSERT_FLOAT_EQ(mesh3_1_box.GetTop(), 2.19192767);
  ASSERT_FLOAT_EQ(mesh3_1_box.GetBack(), 1.25619268);
  // ----------------------------------------------------------------

  // ----------------------------------------------------------------
  // asset4
  auto& image4_1 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *asset4.GetHandler(0).GetObject());
  auto& image4_2 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *asset4.GetHandler(1).GetObject());
  auto& mesh4_1 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *asset4.GetHandler(2).GetObject());

  MM::FileSystem::Path path4_1(std::string(TEST_FILE_DIR_TEST) +
                               "/asset_system/test_picture1.jpg"),
      path4_2(std::string(TEST_FILE_DIR_TEST) +
              "/asset_system/test_picture1.jpg"),
      path4_3(std::string(TEST_FILE_DIR_TEST) + "/asset_system/model.fbx");
  MM::AssetSystem::AssetType::AssetID asset_id4_1, asset_id4_2, asset_id4_3;
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(path4_1, 3,
                                                                asset_id4_1),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(path4_2, 3,
                                                                asset_id4_2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path4_3, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                asset_id4_3),
            MM::ExecuteResult::SUCCESS);

  // image4_1
  ASSERT_EQ(image4_1.GetAssetID(), asset_id4_1);
  ASSERT_EQ(image4_1.IsValid(), true);
  ASSERT_EQ(image4_1.GetAssetName(), path4_1.GetFileName());
  ASSERT_EQ(image4_1.GetAssetPath(), path4_1);
  ASSERT_EQ(image4_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image4_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image4_1.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB);
  ASSERT_EQ(image4_1.GetImageChannels(), 3);
  ASSERT_EQ(image4_1.GetImageWidth(), 1200);
  ASSERT_EQ(image4_1.GetImageHeight(), 1200);
  ASSERT_EQ(image4_1.GetImageSize(),
            image4_1.GetImageWidth() * image4_1.GetImageHeight() * 3);
  ASSERT_NE(image4_1.GetPixelsData(), nullptr);

  // image4_2
  ASSERT_EQ(image4_2.GetAssetID(), asset_id4_2);
  ASSERT_EQ(image4_2.IsValid(), true);
  ASSERT_EQ(image4_2.GetAssetName(), path4_2.GetFileName());
  ASSERT_EQ(image4_2.GetAssetPath(), path4_2);
  ASSERT_EQ(image4_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image4_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image4_2.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB);
  ASSERT_EQ(image4_2.GetImageChannels(), 3);
  ASSERT_EQ(image4_2.GetImageWidth(), 1200);
  ASSERT_EQ(image4_2.GetImageHeight(), 1200);
  ASSERT_EQ(image4_2.GetImageSize(),
            image4_2.GetImageWidth() * image4_2.GetImageHeight() * 3);
  ASSERT_NE(image4_2.GetPixelsData(), nullptr);

  // mesh4_1
  ASSERT_EQ(mesh4_1.GetAssetID(), asset_id4_3);
  ASSERT_EQ(mesh4_1.IsValid(), true);
  ASSERT_EQ(mesh4_1.GetAssetName(), path4_3.GetFileName());
  ASSERT_EQ(mesh4_1.GetAssetPath(), path4_3);
  ASSERT_EQ(mesh4_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh4_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh4_1.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB);
  const MM::AssetSystem::AssetType::RectangleBox& mesh4_1_box =
      dynamic_cast<const MM::AssetSystem::AssetType::RectangleBox&>(
          mesh4_1.GetBoundingBox());
  ASSERT_FLOAT_EQ(mesh4_1_box.GetLeft(), -1.02342343);
  ASSERT_FLOAT_EQ(mesh4_1_box.GetBottom(), -1.02342343);
  ASSERT_FLOAT_EQ(mesh4_1_box.GetForward(), -4.18217993);
  ASSERT_FLOAT_EQ(mesh4_1_box.GetRight(), 4.33429909);
  ASSERT_FLOAT_EQ(mesh4_1_box.GetTop(), 2.19192767);
  ASSERT_FLOAT_EQ(mesh4_1_box.GetBack(), 1.25619268);
  // ----------------------------------------------------------------

  MM::AssetSystem::AssetManager* asset_manager =
      MM::AssetSystem::AssetManager::GetInstance();
  std::vector<MM::AssetSystem::AssetManager::AssetHandler>
      image1_asset_handler_vector, image2_asset_handler_vector,
      mesh1_asset_handler_vector, mesh2_asset_handler_vector;
  ASSERT_EQ(asset_manager->GetSize(), 6);
  ASSERT_EQ(asset_manager->GetAssetByAssetName("test_picture1.jpg",
                                               image1_asset_handler_vector),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(asset_manager->GetAssetByAssetName("test_picture2.png",
                                               image2_asset_handler_vector),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(asset_manager->GetAssetByAssetName("monkey.obj",
                                               mesh1_asset_handler_vector),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(asset_manager->GetAssetByAssetName("model.fbx",
                                               mesh2_asset_handler_vector),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(image1_asset_handler_vector.size(), 2);
  ASSERT_EQ(image2_asset_handler_vector.size(), 1);
  ASSERT_EQ(mesh1_asset_handler_vector.size(), 1);
  ASSERT_EQ(mesh2_asset_handler_vector.size(), 2);
  image1_asset_handler_vector.clear();
  image2_asset_handler_vector.clear();
  mesh1_asset_handler_vector.clear();
  mesh2_asset_handler_vector.clear();
  //  auto& image1_asset_handler_vector0 =
  //      dynamic_cast<MM::AssetSystem::AssetType::Image&>(
  //          *image1_asset_handler_vector[0].GetObject());
  //  auto& image1_asset_handler_vector1 =
  //      dynamic_cast<MM::AssetSystem::AssetType::Image&>(
  //          *image1_asset_handler_vector[1].GetObject());
  //  auto& image1_2_channel = image1_asset_handler_vector0.GetImageChannels()
  //  == 2
  //                               ? image1_asset_handler_vector0
  //                               : image1_asset_handler_vector1;
  //  auto& image1_3_channel = &image1_2_channel ==
  //  &image1_asset_handler_vector0
  //                               ? image1_asset_handler_vector1
  //                               : image1_asset_handler_vector0;
  MM::AssetSystem::AssetType::AssetID image1_2_channel_asset_id,
      image1_3_channel_asset_id, image2_4_channel_asset_id, mesh1_AABB_asset_id,
      mesh2_AABB_asset_id, mesh2_CAPSULE_asset_id;
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(
                path1_1, 2, image1_2_channel_asset_id),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(
                path1_1, 3, image1_3_channel_asset_id),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Image::CalculateAssetID(
                path1_2, 4, image2_4_channel_asset_id),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path1_3, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                mesh1_AABB_asset_id),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path1_4, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                mesh2_AABB_asset_id),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
          path1_4, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE,
          mesh2_CAPSULE_asset_id),
      MM::Utils::ExecuteResult::SUCCESS);

  MM::AssetSystem::AssetManager::HandlerType handler1, handler2, handler3,
      handler4, handler5, handler6;
  ASSERT_EQ(
      asset_manager->GetAssetByAssetID(image1_2_channel_asset_id, handler1),
      MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      asset_manager->GetAssetByAssetID(image1_3_channel_asset_id, handler2),
      MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      asset_manager->GetAssetByAssetID(image2_4_channel_asset_id, handler3),
      MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(asset_manager->GetAssetByAssetID(mesh1_AABB_asset_id, handler4),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(asset_manager->GetAssetByAssetID(mesh2_AABB_asset_id, handler5),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(asset_manager->GetAssetByAssetID(mesh2_CAPSULE_asset_id, handler6),
            MM::Utils::ExecuteResult::SUCCESS);

  ASSERT_EQ(handler1.GetUseCount(), 2);
  ASSERT_EQ(handler2.GetUseCount(), 4);
  ASSERT_EQ(handler3.GetUseCount(), 3);
  ASSERT_EQ(handler4.GetUseCount(), 2);
  ASSERT_EQ(handler5.GetUseCount(), 3);
  ASSERT_EQ(handler6.GetUseCount(), 2);
}