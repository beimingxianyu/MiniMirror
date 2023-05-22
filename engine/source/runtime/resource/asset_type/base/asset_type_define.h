//
// Created by beimingxianyu on 23-5-13.
//

#pragma once

#include "utils/uuid.h"

namespace MM {
namespace AssetType {
using AssetDataID = MM::Utils::GUID;
using AssetID = MM::Utils::GUID;

enum class ImageFormat { UNDEFINED = 0U, GREY, GREY_ALPHA, RGB, RGB_ALPHA };

enum class AssetType { UNDEFINED = 0U, IMAGE, MESH };
#define MM_ASSET_TYPE_UNDEFINED "__MM_ASSET_TYPE_UNDEFINED__"
#define MM_ASSET_TYPE_IMAGE "__MM_ASSET_TYPE_IMAGE__"
#define MM_ASSET_TYPE_MESH "__MM_ASSET_TYPE_MESH__"

}  // namespace AssetType
}  // namespace MM