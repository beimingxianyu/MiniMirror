//
// Created by beimingxianyu on 23-5-13.
//

#pragma once

#include "utils/uuid.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
using AssetID = std::uint64_t;

enum class ImageFormat { UNDEFINED = 0U, GREY, GREY_ALPHA, RGB, RGB_ALPHA };

enum class AssetType { UNDEFINED = 0U, IMAGE, MESH, COMMON, COMBINATION, SHADER };
#define MM_ASSET_TYPE_UNDEFINED "__MM_ASSET_TYPE_UNDEFINED__"
#define MM_ASSET_TYPE_IMAGE "__MM_ASSET_TYPE_IMAGE__"
#define MM_ASSET_TYPE_MESH "__MM_ASSET_TYPE_MESH__"
#define MM_ASSET_TYPE_SHADER "__MM_ASSET_TYPE_SHADER__"
#define MM_ASSET_TYPE_COMMON "__MM_ASSET_TYPE_COMMON__"
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
