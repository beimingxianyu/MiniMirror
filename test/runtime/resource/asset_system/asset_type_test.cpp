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
      path2(MM::FileSystem::Path(std::string(MM_TEST_FILE_DIR_TEST) +
                                 "/asset_system/test_picture1.jpg")),
      path3(MM::FileSystem::Path(std::string(MM_TEST_FILE_DIR_TEST) +
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
  MM::Result<MM::FileSystem::LastWriteTime> last_write_time2 =
  MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path2).Exception();
  ASSERT_EQ(asset_base2.GetAssetID(),
            path2.GetHash() ^ static_cast<std::uint64_t>(
                                  last_write_time2.GetResult().time_since_epoch().count()));
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
  MM::Result<MM::FileSystem::LastWriteTime> last_write_time3 =
  MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path3).Exception();
  ASSERT_EQ(asset_base3.GetAssetID(),
            path3.GetHash() ^ static_cast<std::uint64_t>(
                                  last_write_time3.GetResult().time_since_epoch().count()));
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
      path2(MM::FileSystem::Path(std::string(MM_TEST_FILE_DIR_TEST) +
                                 "/asset_system/test_picture1.jpg")),
      path3(MM::FileSystem::Path(std::string(MM_TEST_FILE_DIR_TEST) +
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
  MM::Result<MM::FileSystem::LastWriteTime> last_write_time2 = MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path2).Exception();
  ASSERT_EQ(last_write_time2.IsSuccess(), true);
  MM::AssetSystem::AssetType::AssetID asset_id2 =
      path2.GetHash() ^
      static_cast<std::uint64_t>(last_write_time2.GetResult().time_since_epoch().count());
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
  MM::Result<MM::Utils::Json::Document> json2_1{image2_1.GetJson().Exception().Move()};
  ASSERT_EQ(json2_1.IsSuccess(), true);
  ASSERT_EQ(json2_1.GetResult()["name"].GetString(), image2_1.GetAssetName());
  ASSERT_EQ(json2_1.GetResult()["path"].GetString(), image2_1.GetAssetPath().String());
  ASSERT_EQ(json2_1.GetResult()["asset id"].GetUint64(), image2_1.GetAssetID());
  ASSERT_EQ(json2_1.GetResult()["image width"].GetUint(), image2_1.GetImageWidth());
  ASSERT_EQ(json2_1.GetResult()["image height"].GetUint(), image2_1.GetImageHeight());
  ASSERT_EQ(json2_1.GetResult()["image channels"].GetUint(), image2_1.GetImageChannels());
  ASSERT_EQ(json2_1.GetResult()["image size"].GetUint(), image2_1.GetImageSize());
  ASSERT_EQ(json2_1.GetResult()["image format"], "GREY");
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
  MM::Result<MM::Utils::Json::Document> json2_2 = image2_2.GetJson().Exception().Move();
  ASSERT_EQ(json2_2.IsSuccess(), true);
  ASSERT_EQ(json2_2.GetResult()["name"].GetString(), image2_2.GetAssetName());
  ASSERT_EQ(json2_2.GetResult()["path"].GetString(), image2_2.GetAssetPath().String());
  ASSERT_EQ(json2_2.GetResult()["asset id"].GetUint64(), image2_2.GetAssetID());
  ASSERT_EQ(json2_2.GetResult()["image width"].GetUint(), image2_2.GetImageWidth());
  ASSERT_EQ(json2_2.GetResult()["image height"].GetUint(), image2_2.GetImageHeight());
  ASSERT_EQ(json2_2.GetResult()["image channels"].GetUint(), image2_2.GetImageChannels());
  ASSERT_EQ(json2_2.GetResult()["image size"].GetUint(), image2_2.GetImageSize());
  ASSERT_EQ(json2_2.GetResult()["image format"], "GREY_ALPHA");
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
  MM::Result<MM::Utils::Json::Document> json2_3 = image2_3.GetJson().Exception().Move();
  ASSERT_EQ(json2_3.IsSuccess(), true);
  ASSERT_EQ(json2_3.GetResult()["name"].GetString(), image2_3.GetAssetName());
  ASSERT_EQ(json2_3.GetResult()["path"].GetString(), image2_3.GetAssetPath().String());
  ASSERT_EQ(json2_3.GetResult()["asset id"].GetUint64(), image2_3.GetAssetID());
  ASSERT_EQ(json2_3.GetResult()["image width"].GetUint(), image2_3.GetImageWidth());
  ASSERT_EQ(json2_3.GetResult()["image height"].GetUint(), image2_3.GetImageHeight());
  ASSERT_EQ(json2_3.GetResult()["image channels"].GetUint(), image2_3.GetImageChannels());
  ASSERT_EQ(json2_3.GetResult()["image size"].GetUint(), image2_3.GetImageSize());
  ASSERT_EQ(json2_3.GetResult()["image format"], "RGB");
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
  MM::Result<MM::Utils::Json::Document> json2_4 = image2_4.GetJson().Exception().Move();
  ASSERT_EQ(json2_4.IsSuccess(), true);
  ASSERT_EQ(json2_4.GetResult()["name"].GetString(), image2_4.GetAssetName());
  ASSERT_EQ(json2_4.GetResult()["path"].GetString(), image2_4.GetAssetPath().String());
  ASSERT_EQ(json2_4.GetResult()["asset id"].GetUint64(), image2_4.GetAssetID());
  ASSERT_EQ(json2_4.GetResult()["image width"].GetUint(), image2_4.GetImageWidth());
  ASSERT_EQ(json2_4.GetResult()["image height"].GetUint(), image2_4.GetImageHeight());
  ASSERT_EQ(json2_4.GetResult()["image channels"].GetUint(), image2_4.GetImageChannels());
  ASSERT_EQ(json2_4.GetResult()["image size"].GetUint(), image2_4.GetImageSize());
  ASSERT_EQ(json2_4.GetResult()["image format"], "RGB_ALPHA");
  image2_4.Release();
  ASSERT_EQ(image2_4.IsValid(), false);
  ASSERT_EQ(image2_4.GetAssetID(), 0);

  // image3
  MM::AssetSystem::AssetType::Image image3_1(path3, 1), image3_2(path3, 2),
      image3_3(path3, 3), image3_4(path3, 4);
  MM::Result<MM::FileSystem::LastWriteTime> last_write_time3 = MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path3).Exception().Move();
  ASSERT_EQ(last_write_time3.IsSuccess(), true);
  MM::AssetSystem::AssetType::AssetID asset_id3 =
      path3.GetHash() ^
      static_cast<std::uint64_t>(last_write_time3.GetResult().time_since_epoch().count());
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
  MM::Result<MM::Utils::Json::Document> json3_1 = image3_1.GetJson().Exception().Move();
  ASSERT_EQ(json3_1.IsSuccess(), true);
  ASSERT_EQ(json3_1.GetResult()["name"].GetString(), image3_1.GetAssetName());
  ASSERT_EQ(json3_1.GetResult()["path"].GetString(), image3_1.GetAssetPath().String());
  ASSERT_EQ(json3_1.GetResult()["asset id"].GetUint64(), image3_1.GetAssetID());
  ASSERT_EQ(json3_1.GetResult()["image width"].GetUint(), image3_1.GetImageWidth());
  ASSERT_EQ(json3_1.GetResult()["image height"].GetUint(), image3_1.GetImageHeight());
  ASSERT_EQ(json3_1.GetResult()["image channels"].GetUint(), image3_1.GetImageChannels());
  ASSERT_EQ(json3_1.GetResult()["image size"].GetUint(), image3_1.GetImageSize());
  ASSERT_EQ(json3_1.GetResult()["image format"], "GREY");
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
  MM::Result<MM::Utils::Json::Document> json3_2 = image3_2.GetJson().Exception().Move();
  ASSERT_EQ(json3_2.IsSuccess(), true);
  ASSERT_EQ(json3_2.GetResult()["name"].GetString(), image3_2.GetAssetName());
  ASSERT_EQ(json3_2.GetResult()["path"].GetString(), image3_2.GetAssetPath().String());
  ASSERT_EQ(json3_2.GetResult()["asset id"].GetUint64(), image3_2.GetAssetID());
  ASSERT_EQ(json3_2.GetResult()["image width"].GetUint(), image3_2.GetImageWidth());
  ASSERT_EQ(json3_2.GetResult()["image height"].GetUint(), image3_2.GetImageHeight());
  ASSERT_EQ(json3_2.GetResult()["image channels"].GetUint(), image3_2.GetImageChannels());
  ASSERT_EQ(json3_2.GetResult()["image size"].GetUint(), image3_2.GetImageSize());
  ASSERT_EQ(json3_2.GetResult()["image format"], "GREY_ALPHA");
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
  MM::Result<MM::Utils::Json::Document> json3_3 = image3_3.GetJson().Exception().Move();
  ASSERT_EQ(json3_3.IsSuccess(), true);
  ASSERT_EQ(json3_3.GetResult()["name"].GetString(), image3_3.GetAssetName());
  ASSERT_EQ(json3_3.GetResult()["path"].GetString(), image3_3.GetAssetPath().String());
  ASSERT_EQ(json3_3.GetResult()["asset id"].GetUint64(), image3_3.GetAssetID());
  ASSERT_EQ(json3_3.GetResult()["image width"].GetUint(), image3_3.GetImageWidth());
  ASSERT_EQ(json3_3.GetResult()["image height"].GetUint(), image3_3.GetImageHeight());
  ASSERT_EQ(json3_3.GetResult()["image channels"].GetUint(), image3_3.GetImageChannels());
  ASSERT_EQ(json3_3.GetResult()["image size"].GetUint(), image3_3.GetImageSize());
  ASSERT_EQ(json3_3.GetResult()["image format"], "RGB");
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
  MM::Result<MM::Utils::Json::Document> json3_4 = image3_4.GetJson().Exception().Move();
  ASSERT_EQ(json3_4.IsSuccess(), true);
  ASSERT_EQ(json3_4.GetResult()["name"].GetString(), image3_4.GetAssetName());
  ASSERT_EQ(json3_4.GetResult()["path"].GetString(), image3_4.GetAssetPath().String());
  ASSERT_EQ(json3_4.GetResult()["asset id"].GetUint64(), image3_4.GetAssetID());
  ASSERT_EQ(json3_4.GetResult()["image width"].GetUint(), image3_4.GetImageWidth());
  ASSERT_EQ(json3_4.GetResult()["image height"].GetUint(), image3_4.GetImageHeight());
  ASSERT_EQ(json3_4.GetResult()["image channels"].GetUint(), image3_4.GetImageChannels());
  ASSERT_EQ(json3_4.GetResult()["image size"].GetUint(), image3_4.GetImageSize());
  ASSERT_EQ(json3_4.GetResult()["image format"], "RGB_ALPHA");
  image3_4.Release();
  ASSERT_EQ(image3_4.IsValid(), false);
  ASSERT_EQ(image3_4.GetAssetID(), 0);
}

TEST(asset_system, mesh) {
  MM::FileSystem::Path path1(""),
      path2(MM::FileSystem::Path(std::string(MM_TEST_FILE_DIR_TEST) +
                                 "/asset_system/monkey.obj")),
      path3(MM::FileSystem::Path(std::string(MM_TEST_FILE_DIR_TEST) +
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
  MM::Result<MM::FileSystem::LastWriteTime> last_write_time2 = MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path2).Exception().Move();
  ASSERT_EQ(last_write_time2.IsSuccess(), true);
  MM::AssetSystem::AssetType::AssetID asset_id2 =
      path2.GetHash() ^
      static_cast<std::uint64_t>(last_write_time2.GetResult().time_since_epoch().count());
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id2_1 = MM::AssetSystem::AssetType::Mesh::CalculateAssetID(path2, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB).Exception().Move();
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id2_2 = MM::AssetSystem::AssetType::Mesh::CalculateAssetID(path2, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE).Exception().Move();
  ASSERT_EQ(asset_id2_1.IsSuccess(), true);
  ASSERT_EQ(asset_id2_2.IsSuccess(), true);

  // mesh2_1
  ASSERT_EQ(mesh2_1.GetAssetID(), asset_id2_1.GetResult());
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
  ASSERT_EQ(mesh2_2.GetAssetID(), asset_id2_2.GetResult());
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
  MM::Result<MM::FileSystem::LastWriteTime> last_write_time3 = MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(path3).Exception().Move();
  ASSERT_EQ(last_write_time3.IsSuccess(), true);
  MM::AssetSystem::AssetType::AssetID asset_id3 =
      path3.GetHash() ^
      static_cast<std::uint64_t>(last_write_time3.GetResult().time_since_epoch().count());
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id3_1 = MM::AssetSystem::AssetType::Mesh::CalculateAssetID(path3, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB).Exception().Move();
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id3_2 = MM::AssetSystem::AssetType::Mesh::CalculateAssetID(path3, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE).Exception().Move();
  ASSERT_EQ(asset_id3_1.IsSuccess(), true);
  ASSERT_EQ(asset_id3_2.IsSuccess(), true);
  // mesh3_1
  ASSERT_EQ(mesh3_1.GetAssetID(), asset_id3_1.GetResult());
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
  ASSERT_EQ(mesh3_2.GetAssetID(), asset_id3_2.GetResult());
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

  MM::FileSystem::Path json_path1{std::string(MM_TEST_FILE_DIR_TEST) +
                                  "/asset_system/combination2.json"},
      json_path2{std::string(MM_TEST_FILE_DIR_TEST) +
                 "/asset_system/combination1.json"};
  ImageImageMeshMesh image_image_mesh1(json_path1);
  ImageImageMeshMesh image_image_mesh2(json_path2);

  // image_image_mesh1
  ASSERT_EQ(image_image_mesh1.GetAssetID(), 0);
  ASSERT_EQ(image_image_mesh1.IsValid(), false);

  // image_image_mesh2
  MM::Result<MM::FileSystem::LastWriteTime> last_write_time_path = MM::FileSystem::FileSystem::GetInstance()->GetLastWriteTime(json_path2).Exception().Move();
  ASSERT_EQ(last_write_time_path.IsSuccess(), true);
  ASSERT_EQ(
      image_image_mesh2.GetAssetID(),
      json_path2.GetHash() ^ last_write_time_path.GetResult().time_since_epoch().count());
  ASSERT_EQ(image_image_mesh2.IsValid(), true);
  auto& image1 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *image_image_mesh2.GetHandler(0).GetObject());
  auto& image2 = dynamic_cast<MM::AssetSystem::AssetType::Image&>(
      *image_image_mesh2.GetHandler(1).GetObject());
  auto& mesh1 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *image_image_mesh2.GetHandler(2).GetObject());
  auto& mesh2 = dynamic_cast<MM::AssetSystem::AssetType::Mesh&>(
      *image_image_mesh2.GetHandler(3).GetObject());

  MM::FileSystem::Path path1(std::string(MM_TEST_FILE_DIR_TEST) +
                             "/asset_system/test_picture1.jpg"),
      path2(std::string(MM_TEST_FILE_DIR_TEST) +
            "/asset_system/test_picture2.png"),
      path3(std::string(MM_TEST_FILE_DIR_TEST) + "/asset_system/monkey.obj"),
      path4(std::string(MM_TEST_FILE_DIR_TEST) + "/asset_system/model.fbx");

  auto* file_system = MM::FileSystem::FileSystem::GetInstance();
  ASSERT_EQ(file_system->GetLastWriteTime(path1).Exception().IsSuccess(),
            true);
  ASSERT_EQ(file_system->GetLastWriteTime(path2).Exception().IsSuccess(),
            true);
  ASSERT_EQ(file_system->GetLastWriteTime(path3).Exception().IsSuccess(),
            true);
  ASSERT_EQ(file_system->GetLastWriteTime(path4).Exception().IsSuccess(),
            true);
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id1 = MM::AssetSystem::AssetType::Image::CalculateAssetID(path1, 3).Exception().Move();
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id2 = MM::AssetSystem::AssetType::Image::CalculateAssetID(path2, 4).Exception().Move();
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id3 = MM::AssetSystem::AssetType::Mesh::CalculateAssetID(path3, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::AABB).Exception().Move();
  MM::Result<MM::AssetSystem::AssetType::AssetID> asset_id4 = MM::AssetSystem::AssetType::Mesh::CalculateAssetID(path4, 0, MM::AssetSystem::AssetType::BoundingBox::BoundingBoxType::CAPSULE).Exception().Move();
  ASSERT_EQ(asset_id1.IsSuccess(), true);
  ASSERT_EQ(asset_id2.IsSuccess(), true);
  ASSERT_EQ(asset_id3.IsSuccess(), true);
  ASSERT_EQ(asset_id4.IsSuccess(), true);

  // image1
  ASSERT_EQ(image1.GetAssetID(), asset_id1.GetResult());
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
  MM::Result<MM::Utils::Json::Document> json1 = image1.GetJson().Exception().Move();
  ASSERT_EQ(json1.IsSuccess(), true);
  ASSERT_EQ(json1.GetResult()["name"].GetString(), image1.GetAssetName());
  ASSERT_EQ(json1.GetResult()["path"].GetString(), image1.GetAssetPath().String());
  ASSERT_EQ(json1.GetResult()["asset id"].GetUint64(), image1.GetAssetID());
  ASSERT_EQ(json1.GetResult()["image width"].GetUint(), image1.GetImageWidth());
  ASSERT_EQ(json1.GetResult()["image height"].GetUint(), image1.GetImageHeight());
  ASSERT_EQ(json1.GetResult()["image channels"].GetUint(), image1.GetImageChannels());
  ASSERT_EQ(json1.GetResult()["image size"].GetUint(), image1.GetImageSize());
  ASSERT_EQ(json1.GetResult()["image format"], "RGB");

  // image2
  ASSERT_EQ(image2.GetAssetID(), asset_id2.GetResult());
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
  MM::Result<MM::Utils::Json::Document> json2 = image2.GetJson().Exception().Move();
  ASSERT_EQ(json2.IsSuccess(), true);
  ASSERT_EQ(json2.GetResult()["name"].GetString(), image2.GetAssetName());
  ASSERT_EQ(json2.GetResult()["path"].GetString(), image2.GetAssetPath().String());
  ASSERT_EQ(json2.GetResult()["asset id"].GetUint64(), image2.GetAssetID());
  ASSERT_EQ(json2.GetResult()["image width"].GetUint(), image2.GetImageWidth());
  ASSERT_EQ(json2.GetResult()["image height"].GetUint(), image2.GetImageHeight());
  ASSERT_EQ(json2.GetResult()["image channels"].GetUint(), image2.GetImageChannels());
  ASSERT_EQ(json2.GetResult()["image size"].GetUint(), image2.GetImageSize());
  ASSERT_EQ(json2.GetResult()["image format"], "RGB_ALPHA");

  // mesh1
  ASSERT_EQ(mesh1.GetAssetID(), asset_id3.GetResult());
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
  ASSERT_EQ(mesh2.GetAssetID(), asset_id4.GetResult());
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