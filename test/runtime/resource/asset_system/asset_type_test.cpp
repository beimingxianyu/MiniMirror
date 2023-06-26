//
#include <gtest/gtest.h>

#include <assimp/Exporter.hpp>
#include <cmath>
#include <cstdint>
#include <string>

#include "glm/fwd.hpp"
#include "rapidjson/document.h"
#include "runtime/platform/base/error.h"
#include "runtime/platform/file_system/file_system.h"
#include "runtime/resource/asset_system/AssetManager.h"
#include "runtime/resource/asset_system/AssetSystem.h"
#include "runtime/resource/asset_system/asset_type/Image.h"
#include "runtime/resource/asset_system/asset_type/Mesh.h"
#include "runtime/resource/asset_system/asset_type/base/asset_type_define.h"
#include "runtime/resource/asset_system/asset_type/base/bounding_box.h"
#include "utils/error.h"

TEST(asset_system, asset_base) {
  MM::FileSystem::Path path1(""),
      path2(MM::FileSystem::Path(std::string(TEST_FILE_DIR_TEST) +
                                 "/asset_system/test_picture1.jpg")),
      path3(MM::FileSystem::Path(std::string(TEST_FILE_DIR_TEST) +
                                 "/asset_system/test_picture2.png"));
  ASSERT_EQ(path1.IsExists(), false);
  ASSERT_EQ(path2.IsExists(), true);
  ASSERT_EQ(path3.IsExists(), true);

  // Will print error logs for files that do not exist.
  MM::AssetSystem::AssetType::AssetBase asset_base1(path1), asset_base2(path2),
      asset_base3(path3);

  // asset_base1
  ASSERT_EQ(asset_base1.GetAssetID(), 0);
  ASSERT_EQ(asset_base1.IsValid(), false);
  ASSERT_EQ(asset_base2.GetAssetTypeString(),
            std::string(MM_ASSET_TYPE_UNDEFINED));

  // asset_base2
  MM::FileSystem::LastWriteTime last_write_time2;
  MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path2,
                                                              last_write_time2);
  ASSERT_EQ(asset_base2.GetAssetID(),
            path2.GetHash() ^ static_cast<std::uint64_t>(
                                  last_write_time2.time_since_epoch().count()));
  ASSERT_EQ(asset_base2.IsValid(), true);
  ASSERT_EQ(asset_base2.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(asset_base2.GetAssetPath(), path2);
  ASSERT_EQ(asset_base2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::UNDEFINED);
  asset_base2.Release();
  ASSERT_EQ(asset_base2.IsValid(), false);
  ASSERT_EQ(asset_base2.GetAssetID(), 0);
  ASSERT_EQ(asset_base2.GetAssetTypeString(),
            std::string(MM_ASSET_TYPE_UNDEFINED));

  // asset_base3
  MM::FileSystem::LastWriteTime last_write_time3;
  MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path3,
                                                              last_write_time3);
  ASSERT_EQ(asset_base3.GetAssetID(),
            path3.GetHash() ^ static_cast<std::uint64_t>(
                                  last_write_time3.time_since_epoch().count()));
  ASSERT_EQ(asset_base3.IsValid(), true);
  ASSERT_EQ(asset_base3.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(asset_base3.GetAssetPath(), path3);
  ASSERT_EQ(asset_base3.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::UNDEFINED);
  asset_base3.Release();
  ASSERT_EQ(asset_base3.IsValid(), false);
  ASSERT_EQ(asset_base3.GetAssetID(), 0);
  ASSERT_EQ(asset_base2.GetAssetTypeString(),
            std::string(MM_ASSET_TYPE_UNDEFINED));
}

TEST(asset_system, image) {
  MM::FileSystem::Path path1(""),
      path2(MM::FileSystem::Path(std::string(TEST_FILE_DIR_TEST) +
                                 "/asset_system/test_picture1.jpg")),
      path3(MM::FileSystem::Path(std::string(TEST_FILE_DIR_TEST) +
                                 "/asset_system/test_picture2.png"));
  ASSERT_EQ(path1.IsExists(), false);
  ASSERT_EQ(path2.IsExists(), true);
  ASSERT_EQ(path3.IsExists(), true);

  // image1
  // Will print error logs for files that do not exist.
  MM::AssetSystem::AssetType::Image image1_1(path1, 1), image1_2(path1, 2),
      image1_3(path1, 3), image1_4(path1, 4);
  ASSERT_EQ(image1_1.GetAssetID(), 0);
  ASSERT_EQ(image1_1.IsValid(), false);
  ASSERT_EQ(image1_2.GetAssetID(), 0);
  ASSERT_EQ(image1_2.IsValid(), false);
  ASSERT_EQ(image1_3.GetAssetID(), 0);
  ASSERT_EQ(image1_3.IsValid(), false);
  ASSERT_EQ(image1_4.GetAssetID(), 0);
  ASSERT_EQ(image1_4.IsValid(), false);

  // image2
  MM::AssetSystem::AssetType::Image image2_1(path2, 1), image2_2(path2, 2),
      image2_3(path2, 3), image2_4(path2, 4);
  MM::FileSystem::LastWriteTime last_write_time2;
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                path2, last_write_time2),
            MM::Utils::ExecuteResult::SUCCESS);
  MM::AssetSystem::AssetType::AssetID asset_id2 =
      path2.GetHash() ^
      static_cast<std::uint64_t>(last_write_time2.time_since_epoch().count());
  MM::AssetSystem::AssetType::AssetID asset_id2_1 = asset_id2 + 1,
                                      asset_id2_2 = asset_id2 + 2,
                                      asset_id2_3 = asset_id2 + 3,
                                      asset_id2_4 = asset_id2 + 4;
  // image2_1
  ASSERT_EQ(image2_1.GetAssetID(), asset_id2_1);
  ASSERT_EQ(image2_1.IsValid(), true);
  ASSERT_EQ(image2_1.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(image2_1.GetAssetPath(), path2);
  ASSERT_EQ(image2_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image2_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image2_1.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::GREY);
  ASSERT_EQ(image2_1.GetImageChannels(), 1);
  ASSERT_EQ(image2_1.GetOriginalImageChannels(), 3);
  ASSERT_EQ(image2_1.GetImageWidth(), 1200);
  ASSERT_EQ(image2_1.GetImageHeight(), 1200);
  ASSERT_EQ(image2_1.GetImageSize(),
            image2_1.GetImageWidth() * image2_1.GetImageHeight() * 1);
  ASSERT_NE(image2_1.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json2_1;
  json2_1.SetObject();
  ASSERT_EQ(image2_1.GetJson(json2_1), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json2_1["name"].GetString(), image2_1.GetAssetName());
  ASSERT_EQ(json2_1["path"].GetString(), image2_1.GetAssetPath().String());
  ASSERT_EQ(json2_1["asset id"].GetUint64(), image2_1.GetAssetID());
  ASSERT_EQ(json2_1["image width"].GetUint(), image2_1.GetImageWidth());
  ASSERT_EQ(json2_1["image height"].GetUint(), image2_1.GetImageHeight());
  ASSERT_EQ(json2_1["image channels"].GetUint(), image2_1.GetImageChannels());
  ASSERT_EQ(json2_1["image size"].GetUint(), image2_1.GetImageSize());
  ASSERT_EQ(json2_1["image format"], "GREY");
  image2_1.Release();
  ASSERT_EQ(image2_1.IsValid(), false);
  ASSERT_EQ(image2_1.GetAssetID(), 0);
  // image2_2
  ASSERT_EQ(image2_2.GetAssetID(), asset_id2_2);
  ASSERT_EQ(image2_2.IsValid(), true);
  ASSERT_EQ(image2_2.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(image2_2.GetAssetPath(), path2);
  ASSERT_EQ(image2_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image2_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image2_2.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::GREY_ALPHA);
  ASSERT_EQ(image2_2.GetImageChannels(), 2);
  ASSERT_EQ(image2_2.GetImageWidth(), 1200);
  ASSERT_EQ(image2_2.GetImageHeight(), 1200);
  ASSERT_EQ(image2_2.GetImageSize(),
            image2_2.GetImageWidth() * image2_2.GetImageHeight() * 2);
  ASSERT_NE(image2_2.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json2_2;
  json2_2.SetObject();
  ASSERT_EQ(image2_2.GetJson(json2_2), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json2_2["name"].GetString(), image2_2.GetAssetName());
  ASSERT_EQ(json2_2["path"].GetString(), image2_2.GetAssetPath().String());
  ASSERT_EQ(json2_2["asset id"].GetUint64(), image2_2.GetAssetID());
  ASSERT_EQ(json2_2["image width"].GetUint(), image2_2.GetImageWidth());
  ASSERT_EQ(json2_2["image height"].GetUint(), image2_2.GetImageHeight());
  ASSERT_EQ(json2_2["image channels"].GetUint(), image2_2.GetImageChannels());
  ASSERT_EQ(json2_2["image size"].GetUint(), image2_2.GetImageSize());
  ASSERT_EQ(json2_2["image format"], "GREY_ALPHA");
  image2_2.Release();
  ASSERT_EQ(image2_2.IsValid(), false);
  ASSERT_EQ(image2_2.GetAssetID(), 0);
  // image2_3
  ASSERT_EQ(image2_3.GetAssetID(), asset_id2_3);
  ASSERT_EQ(image2_3.IsValid(), true);
  ASSERT_EQ(image2_3.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(image2_3.GetAssetPath(), path2);
  ASSERT_EQ(image2_3.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image2_3.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image2_3.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB);
  ASSERT_EQ(image2_3.GetImageChannels(), 3);
  ASSERT_EQ(image2_3.GetImageWidth(), 1200);
  ASSERT_EQ(image2_3.GetImageHeight(), 1200);
  ASSERT_EQ(image2_3.GetImageSize(),
            image2_3.GetImageWidth() * image2_3.GetImageHeight() * 3);
  ASSERT_NE(image2_3.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json2_3;
  json2_3.SetObject();
  ASSERT_EQ(image2_3.GetJson(json2_3), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json2_3["name"].GetString(), image2_3.GetAssetName());
  ASSERT_EQ(json2_3["path"].GetString(), image2_3.GetAssetPath().String());
  ASSERT_EQ(json2_3["asset id"].GetUint64(), image2_3.GetAssetID());
  ASSERT_EQ(json2_3["image width"].GetUint(), image2_3.GetImageWidth());
  ASSERT_EQ(json2_3["image height"].GetUint(), image2_3.GetImageHeight());
  ASSERT_EQ(json2_3["image channels"].GetUint(), image2_3.GetImageChannels());
  ASSERT_EQ(json2_3["image size"].GetUint(), image2_3.GetImageSize());
  ASSERT_EQ(json2_3["image format"], "RGB");
  image2_3.Release();
  ASSERT_EQ(image2_3.IsValid(), false);
  ASSERT_EQ(image2_3.GetAssetID(), 0);
  // image2_4
  ASSERT_EQ(image2_4.GetAssetID(), asset_id2_4);
  ASSERT_EQ(image2_4.IsValid(), true);
  ASSERT_EQ(image2_4.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(image2_4.GetAssetPath(), path2);
  ASSERT_EQ(image2_4.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image2_4.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image2_4.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB_ALPHA);
  ASSERT_EQ(image2_4.GetImageChannels(), 4);
  ASSERT_EQ(image2_4.GetImageWidth(), 1200);
  ASSERT_EQ(image2_4.GetImageHeight(), 1200);
  ASSERT_EQ(image2_4.GetImageSize(),
            image2_4.GetImageWidth() * image2_4.GetImageHeight() * 4);
  ASSERT_NE(image2_4.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json2_4;
  json2_4.SetObject();
  ASSERT_EQ(image2_4.GetJson(json2_4), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json2_4["name"].GetString(), image2_4.GetAssetName());
  ASSERT_EQ(json2_4["path"].GetString(), image2_4.GetAssetPath().String());
  ASSERT_EQ(json2_4["asset id"].GetUint64(), image2_4.GetAssetID());
  ASSERT_EQ(json2_4["image width"].GetUint(), image2_4.GetImageWidth());
  ASSERT_EQ(json2_4["image height"].GetUint(), image2_4.GetImageHeight());
  ASSERT_EQ(json2_4["image channels"].GetUint(), image2_4.GetImageChannels());
  ASSERT_EQ(json2_4["image size"].GetUint(), image2_4.GetImageSize());
  ASSERT_EQ(json2_4["image format"], "RGB_ALPHA");
  image2_4.Release();
  ASSERT_EQ(image2_4.IsValid(), false);
  ASSERT_EQ(image2_4.GetAssetID(), 0);

  // image3
  MM::AssetSystem::AssetType::Image image3_1(path3, 1), image3_2(path3, 2),
      image3_3(path3, 3), image3_4(path3, 4);
  MM::FileSystem::LastWriteTime last_write_time3;
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                path3, last_write_time3),
            MM::Utils::ExecuteResult::SUCCESS);
  MM::AssetSystem::AssetType::AssetID asset_id3 =
      path3.GetHash() ^
      static_cast<std::uint64_t>(last_write_time3.time_since_epoch().count());
  MM::AssetSystem::AssetType::AssetID asset_id3_1 = asset_id3 + 1,
                                      asset_id3_2 = asset_id3 + 2,
                                      asset_id3_3 = asset_id3 + 3,
                                      asset_id3_4 = asset_id3 + 4;
  // image3_1
  ASSERT_EQ(image3_1.GetAssetID(), asset_id3_1);
  ASSERT_EQ(image3_1.IsValid(), true);
  ASSERT_EQ(image3_1.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(image3_1.GetAssetPath(), path3);
  ASSERT_EQ(image3_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image3_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image3_1.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::GREY);
  ASSERT_EQ(image3_1.GetImageChannels(), 1);
  ASSERT_EQ(image3_1.GetImageWidth(), 541);
  ASSERT_EQ(image3_1.GetImageHeight(), 1200);
  ASSERT_EQ(image3_1.GetImageSize(),
            image3_1.GetImageWidth() * image3_1.GetImageHeight() * 1);
  ASSERT_NE(image3_1.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json3_1;
  json3_1.SetObject();
  ASSERT_EQ(image3_1.GetJson(json3_1), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json3_1["name"].GetString(), image3_1.GetAssetName());
  ASSERT_EQ(json3_1["path"].GetString(), image3_1.GetAssetPath().String());
  ASSERT_EQ(json3_1["asset id"].GetUint64(), image3_1.GetAssetID());
  ASSERT_EQ(json3_1["image width"].GetUint(), image3_1.GetImageWidth());
  ASSERT_EQ(json3_1["image height"].GetUint(), image3_1.GetImageHeight());
  ASSERT_EQ(json3_1["image channels"].GetUint(), image3_1.GetImageChannels());
  ASSERT_EQ(json3_1["image size"].GetUint(), image3_1.GetImageSize());
  ASSERT_EQ(json3_1["image format"], "GREY");
  image3_1.Release();
  ASSERT_EQ(image3_1.IsValid(), false);
  ASSERT_EQ(image3_1.GetAssetID(), 0);
  // image3_2
  ASSERT_EQ(image3_2.GetAssetID(), asset_id3_2);
  ASSERT_EQ(image3_2.IsValid(), true);
  ASSERT_EQ(image3_2.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(image3_2.GetAssetPath(), path3);
  ASSERT_EQ(image3_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image3_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image3_2.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::GREY_ALPHA);
  ASSERT_EQ(image3_2.GetImageChannels(), 2);
  ASSERT_EQ(image3_2.GetImageWidth(), 541);
  ASSERT_EQ(image3_2.GetImageHeight(), 1200);
  ASSERT_EQ(image3_2.GetImageSize(),
            image3_2.GetImageWidth() * image3_2.GetImageHeight() * 2);
  ASSERT_NE(image3_2.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json3_2;
  json3_2.SetObject();
  ASSERT_EQ(image3_2.GetJson(json3_2), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json3_2["name"].GetString(), image3_2.GetAssetName());
  ASSERT_EQ(json3_2["path"].GetString(), image3_2.GetAssetPath().String());
  ASSERT_EQ(json3_2["asset id"].GetUint64(), image3_2.GetAssetID());
  ASSERT_EQ(json3_2["image width"].GetUint(), image3_2.GetImageWidth());
  ASSERT_EQ(json3_2["image height"].GetUint(), image3_2.GetImageHeight());
  ASSERT_EQ(json3_2["image channels"].GetUint(), image3_2.GetImageChannels());
  ASSERT_EQ(json3_2["image size"].GetUint(), image3_2.GetImageSize());
  ASSERT_EQ(json3_2["image format"], "GREY_ALPHA");
  image3_2.Release();
  ASSERT_EQ(image3_2.IsValid(), false);
  ASSERT_EQ(image3_2.GetAssetID(), 0);
  // image3_3
  ASSERT_EQ(image3_3.GetAssetID(), asset_id3_3);
  ASSERT_EQ(image3_3.IsValid(), true);
  ASSERT_EQ(image3_3.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(image3_3.GetAssetPath(), path3);
  ASSERT_EQ(image3_3.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image3_3.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image3_3.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB);
  ASSERT_EQ(image3_3.GetImageChannels(), 3);
  ASSERT_EQ(image3_3.GetImageWidth(), 541);
  ASSERT_EQ(image3_3.GetImageHeight(), 1200);
  ASSERT_EQ(image3_3.GetImageSize(),
            image3_3.GetImageWidth() * image3_3.GetImageHeight() * 3);
  ASSERT_NE(image3_3.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json3_3;
  json3_3.SetObject();
  ASSERT_EQ(image3_3.GetJson(json3_3), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json3_3["name"].GetString(), image3_3.GetAssetName());
  ASSERT_EQ(json3_3["path"].GetString(), image3_3.GetAssetPath().String());
  ASSERT_EQ(json3_3["asset id"].GetUint64(), image3_3.GetAssetID());
  ASSERT_EQ(json3_3["image width"].GetUint(), image3_3.GetImageWidth());
  ASSERT_EQ(json3_3["image height"].GetUint(), image3_3.GetImageHeight());
  ASSERT_EQ(json3_3["image channels"].GetUint(), image3_3.GetImageChannels());
  ASSERT_EQ(json3_3["image size"].GetUint(), image3_3.GetImageSize());
  ASSERT_EQ(json3_3["image format"], "RGB");
  image3_3.Release();
  ASSERT_EQ(image3_3.IsValid(), false);
  ASSERT_EQ(image3_3.GetAssetID(), 0);
  // image3_4
  ASSERT_EQ(image3_4.GetAssetID(), asset_id3_4);
  ASSERT_EQ(image3_4.IsValid(), true);
  ASSERT_EQ(image3_4.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(image3_4.GetAssetPath(), path3);
  ASSERT_EQ(image3_4.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image3_4.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image3_4.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB_ALPHA);
  ASSERT_EQ(image3_4.GetImageChannels(), 4);
  ASSERT_EQ(image3_4.GetImageWidth(), 541);
  ASSERT_EQ(image3_4.GetImageHeight(), 1200);
  ASSERT_EQ(image3_4.GetImageSize(),
            image3_4.GetImageWidth() * image3_4.GetImageHeight() * 4);
  ASSERT_NE(image3_4.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json3_4;
  json3_4.SetObject();
  ASSERT_EQ(image3_4.GetJson(json3_4), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json3_4["name"].GetString(), image3_4.GetAssetName());
  ASSERT_EQ(json3_4["path"].GetString(), image3_4.GetAssetPath().String());
  ASSERT_EQ(json3_4["asset id"].GetUint64(), image3_4.GetAssetID());
  ASSERT_EQ(json3_4["image width"].GetUint(), image3_4.GetImageWidth());
  ASSERT_EQ(json3_4["image height"].GetUint(), image3_4.GetImageHeight());
  ASSERT_EQ(json3_4["image channels"].GetUint(), image3_4.GetImageChannels());
  ASSERT_EQ(json3_4["image size"].GetUint(), image3_4.GetImageSize());
  ASSERT_EQ(json3_4["image format"], "RGB_ALPHA");
  image3_4.Release();
  ASSERT_EQ(image3_4.IsValid(), false);
  ASSERT_EQ(image3_4.GetAssetID(), 0);
}

TEST(asset_system, mesh) {
  MM::FileSystem::Path path1(""),
      path2(MM::FileSystem::Path(std::string(TEST_FILE_DIR_TEST) +
                                 "/asset_system/monkey.obj")),
      path3(MM::FileSystem::Path(std::string(TEST_FILE_DIR_TEST) +
                                 "/asset_system/model.fbx"));
  ASSERT_EQ(path1.IsExists(), false);
  ASSERT_EQ(path2.IsExists(), true);
  ASSERT_EQ(path3.IsExists(), true);

  // Will print error logs for files that do not exist.
  // model1
  MM::AssetSystem::AssetType::Mesh mesh1_1(path1, 0), mesh1_2(path1, 1),
      mesh1_3(path1, 0,
              MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB),
      mesh1_4(
          path1, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE);
  ASSERT_EQ(mesh1_1.GetAssetID(), 0);
  ASSERT_EQ(mesh1_1.IsValid(), false);
  ASSERT_EQ(mesh1_2.GetAssetID(), 0);
  ASSERT_EQ(mesh1_2.IsValid(), false);
  ASSERT_EQ(mesh1_3.GetAssetID(), 0);
  ASSERT_EQ(mesh1_3.IsValid(), false);
  ASSERT_EQ(mesh1_4.GetAssetID(), 0);
  ASSERT_EQ(mesh1_4.IsValid(), false);

  // model2
  MM::AssetSystem::AssetType::Mesh mesh2_1(
      path2, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB),
      mesh2_2(
          path2, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE),
      mesh2_3(path2, 1,
              MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB),
      mesh2_4(
          path2, 1,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE);
  MM::FileSystem::LastWriteTime last_write_time2;
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                path2, last_write_time2),
            MM::Utils::ExecuteResult::SUCCESS);
  MM::AssetSystem::AssetType::AssetID asset_id2 =
      path2.GetHash() ^
      static_cast<std::uint64_t>(last_write_time2.time_since_epoch().count());
  MM::AssetSystem::AssetType::AssetID asset_id2_1, asset_id2_2;
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path2, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                asset_id2_1),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
          path2, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE,
          asset_id2_2),
      MM::Utils::ExecuteResult::SUCCESS);
  // mesh2_1
  ASSERT_EQ(mesh2_1.GetAssetID(), asset_id2_1);
  ASSERT_EQ(mesh2_1.IsValid(), true);
  ASSERT_EQ(mesh2_1.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(mesh2_1.GetAssetPath(), path2);
  ASSERT_EQ(mesh2_1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh2_1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh2_1.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB);
  const MM::AssetSystem::AssetType::RectangleBox& mesh2_1_box =
      dynamic_cast<const MM::AssetSystem::AssetType::RectangleBox&>(
          mesh2_1.GetBoundingBox());
  ASSERT_EQ(mesh2_1_box.IsValid(), true);
  ASSERT_FLOAT_EQ(mesh2_1_box.GetLeft(), -1.36718789);
  ASSERT_FLOAT_EQ(mesh2_1_box.GetBottom(), -0.984375);
  ASSERT_FLOAT_EQ(mesh2_1_box.GetForward(), -0.851562023);
  ASSERT_FLOAT_EQ(mesh2_1_box.GetRight(), 1.36718789);
  ASSERT_FLOAT_EQ(mesh2_1_box.GetTop(), 0.984375);
  ASSERT_FLOAT_EQ(mesh2_1_box.GetBack(), 0.851562023);
  mesh2_1.Release();
  ASSERT_EQ(mesh2_1.GetAssetID(), 0);
  ASSERT_EQ(mesh2_1.IsValid(), false);
  // mesh2_2
  ASSERT_EQ(mesh2_2.GetAssetID(), asset_id2_2);
  ASSERT_EQ(mesh2_2.IsValid(), true);
  ASSERT_EQ(mesh2_2.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(mesh2_2.GetAssetPath(), path2);
  ASSERT_EQ(mesh2_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh2_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh2_2.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE);
  const MM::AssetSystem::AssetType::CapsuleBox& mesh2_2_box =
      dynamic_cast<const MM::AssetSystem::AssetType::CapsuleBox&>(
          mesh2_2.GetBoundingBox());
  ASSERT_GE(mesh2_2_box.GetRadius(), 1.36718789);
  ASSERT_FLOAT_EQ(mesh2_2_box.GetTop(), 0.984375);
  ASSERT_FLOAT_EQ(mesh2_2_box.GetBottom(), -0.984375);
  ASSERT_FLOAT_EQ(mesh2_2_box.IsValid(), true);
  mesh2_2.Release();
  ASSERT_EQ(mesh2_2.GetAssetID(), 0);
  ASSERT_EQ(mesh2_2.IsValid(), false);
  // mesh2_3
  ASSERT_EQ(mesh2_3.GetAssetID(), 0);
  ASSERT_EQ(mesh2_3.IsValid(), false);
  // mesh2_3
  ASSERT_EQ(mesh2_3.GetAssetID(), 0);
  ASSERT_EQ(mesh2_3.IsValid(), false);

  // mesh3
  MM::AssetSystem::AssetType::Mesh mesh3_1(
      path3, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB),
      mesh3_2(
          path3, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE),
      mesh3_3(path3, 1,
              MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB),
      mesh3_4(
          path3, 1,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE);
  MM::FileSystem::LastWriteTime last_write_time3;
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                path3, last_write_time3),
            MM::Utils::ExecuteResult::SUCCESS);
  MM::AssetSystem::AssetType::AssetID asset_id3 =
      path3.GetHash() ^
      static_cast<std::uint64_t>(last_write_time3.time_since_epoch().count());
  MM::AssetSystem::AssetType::AssetID asset_id3_1, asset_id3_2;
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path3, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                asset_id3_1),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
          path3, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE,
          asset_id3_2),
      MM::Utils::ExecuteResult::SUCCESS);
  // mesh3_1
  ASSERT_EQ(mesh3_1.GetAssetID(), asset_id3_1);
  ASSERT_EQ(mesh3_1.IsValid(), true);
  ASSERT_EQ(mesh3_1.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(mesh3_1.GetAssetPath(), path3);
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
  mesh3_1.Release();
  ASSERT_EQ(mesh3_1.GetAssetID(), 0);
  ASSERT_EQ(mesh3_1.IsValid(), false);
  // mesh3_2
  ASSERT_EQ(mesh3_2.GetAssetID(), asset_id3_2);
  ASSERT_EQ(mesh3_2.IsValid(), true);
  ASSERT_EQ(mesh3_2.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(mesh3_2.GetAssetPath(), path3);
  ASSERT_EQ(mesh3_2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh3_2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh3_2.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE);
  const MM::AssetSystem::AssetType::CapsuleBox& mesh3_2_box =
      dynamic_cast<const MM::AssetSystem::AssetType::CapsuleBox&>(
          mesh3_2.GetBoundingBox());
  ASSERT_FLOAT_EQ(mesh3_2_box.GetRadius(), 4.4917974);
  ASSERT_FLOAT_EQ(mesh3_2_box.GetTop(), 2.19192767);
  ASSERT_FLOAT_EQ(mesh3_2_box.GetBottom(), -1.02342343);
  mesh3_2.Release();
  ASSERT_EQ(mesh3_2.GetAssetID(), 0);
  ASSERT_EQ(mesh3_2.IsValid(), false);
  // mesh3_3
  ASSERT_EQ(mesh3_3.GetAssetID(), 0);
  ASSERT_EQ(mesh3_3.IsValid(), false);
  // mesh3_3
  ASSERT_EQ(mesh3_3.GetAssetID(), 0);
  ASSERT_EQ(mesh3_3.IsValid(), false);
}

TEST(asset_system, combination) {
  struct ImageImageMeshMesh : public MM::AssetSystem::AssetType::Combination {
    explicit ImageImageMeshMesh(const MM::FileSystem::Path& json_path)
        : MM::AssetSystem::AssetType::Combination(json_path) {}

    MM::AssetSystem::AssetManager::AssetHandler GetHandler(
        std::uint64_t index) {
      return Get(index);
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
                                  "/asset_system/combination2.json"},
      json_path2{std::string(TEST_FILE_DIR_TEST) +
                 "/asset_system/combination1.json"};
  ImageImageMeshMesh image_image_mesh1(json_path1);
  ImageImageMeshMesh image_image_mesh2(json_path2);

  // image_image_mesh1
  ASSERT_EQ(image_image_mesh1.GetAssetID(), 0);
  ASSERT_EQ(image_image_mesh1.IsValid(), false);

  // image_image_mesh2
  MM::FileSystem::LastWriteTime last_write_time_path;
  ASSERT_EQ(MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(
                json_path2, last_write_time_path),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      image_image_mesh2.GetAssetID(),
      json_path2.GetHash() ^ last_write_time_path.time_since_epoch().count());
  ASSERT_EQ(image_image_mesh2.IsValid(), true);
  auto& image1 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *image_image_mesh2.GetHandler(0).GetObject());
  auto& image2 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *image_image_mesh2.GetHandler(1).GetObject());
  auto& mesh1 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *image_image_mesh2.GetHandler(2).GetObject());
  auto& mesh2 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *image_image_mesh2.GetHandler(3).GetObject());

  MM::FileSystem::Path path1(std::string(TEST_FILE_DIR_TEST) +
                             "/asset_system/test_picture1.jpg"),
      path2(std::string(TEST_FILE_DIR_TEST) +
            "/asset_system/test_picture2.png"),
      path3(std::string(TEST_FILE_DIR_TEST) + "/asset_system/monkey.obj"),
      path4(std::string(TEST_FILE_DIR_TEST) + "/asset_system/model.fbx");

  MM::FileSystem::LastWriteTime last_write_time1, last_write_time2,
      last_write_time3, last_write_time4;
  auto* file_system = MM::FileSystem::FileSystem::GetInstance();
  ASSERT_EQ(file_system->GetLastWriteTime(path1, last_write_time1),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(file_system->GetLastWriteTime(path2, last_write_time2),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(file_system->GetLastWriteTime(path3, last_write_time3),
            MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(file_system->GetLastWriteTime(path4, last_write_time4),
            MM::Utils::ExecuteResult::SUCCESS);
  MM::AssetSystem::AssetType::AssetID asset_id1, asset_id2, asset_id3,
      asset_id4;
  ASSERT_EQ(
      MM::AssetSystem::AssetType::Image::CalculateAssetID(path1, 3, asset_id1),
      MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      MM::AssetSystem::AssetType::Image::CalculateAssetID(path2, 4, asset_id2),
      MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
                path3, 0,
                MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB,
                asset_id3),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(
      MM::AssetSystem::AssetType::Mesh::CalculateAssetID(
          path4, 0,
          MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE,
          asset_id4),
      MM::ExecuteResult::SUCCESS);

  // image1
  ASSERT_EQ(image1.GetAssetID(), asset_id1);
  ASSERT_EQ(image1.IsValid(), true);
  ASSERT_EQ(image1.GetAssetName(), path1.GetFileName());
  ASSERT_EQ(image1.GetAssetPath(), path1);
  ASSERT_EQ(image1.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image1.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB);
  ASSERT_EQ(image1.GetImageChannels(), 3);
  ASSERT_EQ(image1.GetImageWidth(), 1200);
  ASSERT_EQ(image1.GetImageHeight(), 1200);
  ASSERT_EQ(image1.GetImageSize(),
            image1.GetImageWidth() * image1.GetImageHeight() * 3);
  ASSERT_NE(image1.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json1;
  json1.SetObject();
  ASSERT_EQ(image1.GetJson(json1), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json1["name"].GetString(), image1.GetAssetName());
  ASSERT_EQ(json1["path"].GetString(), image1.GetAssetPath().String());
  ASSERT_EQ(json1["asset id"].GetUint64(), image1.GetAssetID());
  ASSERT_EQ(json1["image width"].GetUint(), image1.GetImageWidth());
  ASSERT_EQ(json1["image height"].GetUint(), image1.GetImageHeight());
  ASSERT_EQ(json1["image channels"].GetUint(), image1.GetImageChannels());
  ASSERT_EQ(json1["image size"].GetUint(), image1.GetImageSize());
  ASSERT_EQ(json1["image format"], "RGB");

  // image2
  ASSERT_EQ(image2.GetAssetID(), asset_id2);
  ASSERT_EQ(image2.IsValid(), true);
  ASSERT_EQ(image2.GetAssetName(), path2.GetFileName());
  ASSERT_EQ(image2.GetAssetPath(), path2);
  ASSERT_EQ(image2.GetAssetType(),
            MM::AssetSystem::AssetType::AssetType::IMAGE);
  ASSERT_EQ(image2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_IMAGE));
  ASSERT_EQ(image2.GetImageFormat(),
            MM::AssetSystem::AssetType::ImageFormat::RGB_ALPHA);
  ASSERT_EQ(image2.GetImageChannels(), 4);
  ASSERT_EQ(image2.GetImageWidth(), 541);
  ASSERT_EQ(image2.GetImageHeight(), 1200);
  ASSERT_EQ(image2.GetImageSize(),
            image2.GetImageWidth() * image2.GetImageHeight() * 4);
  ASSERT_NE(image2.GetPixelsData(), nullptr);
  MM::Utils::Json::Document json2;
  json2.SetObject();
  ASSERT_EQ(image2.GetJson(json2), MM::Utils::ExecuteResult::SUCCESS);
  ASSERT_EQ(json2["name"].GetString(), image2.GetAssetName());
  ASSERT_EQ(json2["path"].GetString(), image2.GetAssetPath().String());
  ASSERT_EQ(json2["asset id"].GetUint64(), image2.GetAssetID());
  ASSERT_EQ(json2["image width"].GetUint(), image2.GetImageWidth());
  ASSERT_EQ(json2["image height"].GetUint(), image2.GetImageHeight());
  ASSERT_EQ(json2["image channels"].GetUint(), image2.GetImageChannels());
  ASSERT_EQ(json2["image size"].GetUint(), image2.GetImageSize());
  ASSERT_EQ(json2["image format"], "RGB_ALPHA");

  // mesh1
  ASSERT_EQ(mesh1.GetAssetID(), asset_id3);
  ASSERT_EQ(mesh1.IsValid(), true);
  ASSERT_EQ(mesh1.GetAssetName(), path3.GetFileName());
  ASSERT_EQ(mesh1.GetAssetPath(), path3);
  ASSERT_EQ(mesh1.GetAssetType(), MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh1.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh1.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB);
  const MM::AssetSystem::AssetType::RectangleBox& mesh1_box =
      dynamic_cast<const MM::AssetSystem::AssetType::RectangleBox&>(
          mesh1.GetBoundingBox());
  ASSERT_EQ(mesh1_box.IsValid(), true);
  ASSERT_FLOAT_EQ(mesh1_box.GetLeft(), -1.36718789);
  ASSERT_FLOAT_EQ(mesh1_box.GetBottom(), -0.984375);
  ASSERT_FLOAT_EQ(mesh1_box.GetForward(), -0.851562023);
  ASSERT_FLOAT_EQ(mesh1_box.GetRight(), 1.36718789);
  ASSERT_FLOAT_EQ(mesh1_box.GetTop(), 0.984375);
  ASSERT_FLOAT_EQ(mesh1_box.GetBack(), 0.851562023);

  // mesh2
  ASSERT_EQ(mesh2.GetAssetID(), asset_id4);
  ASSERT_EQ(mesh2.IsValid(), true);
  ASSERT_EQ(mesh2.GetAssetName(), path4.GetFileName());
  ASSERT_EQ(mesh2.GetAssetPath(), path4);
  ASSERT_EQ(mesh2.GetAssetType(), MM::AssetSystem::AssetType::AssetType::MESH);
  ASSERT_EQ(mesh2.GetAssetTypeString(), std::string(MM_ASSET_TYPE_MESH));
  ASSERT_EQ(mesh2.GetBoundingBox().GetBoundingType(),
            MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE);
  const MM::AssetSystem::AssetType::CapsuleBox& mesh2_box =
      dynamic_cast<const MM::AssetSystem::AssetType::CapsuleBox&>(
          mesh2.GetBoundingBox());
  ASSERT_FLOAT_EQ(mesh2_box.GetRadius(), 4.4917974);
  ASSERT_FLOAT_EQ(mesh2_box.GetTop(), 2.19192767);
  ASSERT_FLOAT_EQ(mesh2_box.GetBottom(), -1.02342343);

  image_image_mesh2.Release();
  ASSERT_EQ(image_image_mesh2.IsValid(), false);
  ASSERT_EQ(image_image_mesh2.GetAssetID(), 0);
}