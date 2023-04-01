#pragma once

#include <stb_image.h>

#include <memory>
#include <string>

#include "runtime/resource/asset_type/base/import_other_system.h"
#include "utils/utils.h"

namespace MM {
namespace AssetType {

enum class AssetType { UNDEFINED = 0U, IMAGE, MESH };

enum class ImageFormat { UNDEFINED = 0U, GREY, GREY_ALPHA, RGB, RGB_ALPHA };

class AssetBase {
 public:
  AssetBase() = default;
  virtual ~AssetBase() = default;
  AssetBase(const std::string& asset_name, const uint64_t& asset_ID);
  AssetBase(const AssetBase& other) = default;
  AssetBase(AssetBase&& other) noexcept;
  AssetBase& operator=(const AssetBase& other);
  AssetBase& operator=(AssetBase&& other) noexcept;

  friend bool operator==(const AssetBase& lhs, const AssetBase& rhs) {
    return lhs.asset_ID_ == rhs.asset_ID_;
  }

  friend bool operator!=(const AssetBase& lhs, const AssetBase& rhs) {
    return !(lhs == rhs);
  }

public:
  const std::string& GetAssetName() const;
  AssetBase& SetAssetName(const std::string& new_asset_name);

  const uint32_t& GetAssetID() const;

  virtual bool IsValid() const = 0;

  virtual AssetType GetAssetType() = 0;

  virtual void Release() = 0;

  friend void Swap(AssetBase& lhs, AssetBase& rhs) noexcept;

  friend void swap(AssetBase& lhs, AssetBase& rhs) noexcept;

private:
  std::string asset_name_{};
  uint64_t asset_ID_{0};
};
}
}
