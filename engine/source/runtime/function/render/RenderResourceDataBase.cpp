//
// Created by beimingxianyu on 23-6-28.
//

#include "runtime/function/render/RenderResourceDataBase.h"

MM::RenderSystem::RenderResourceDataBase::RenderResourceDataBase(
    const std::string& resource_name,
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID)
    : ManagedObjectBase(resource_name),
      render_resource_data_ID_(render_resource_data_ID),
      used_for_write_(false),
      is_managed_(false),
      is_asset_resource_(false) {}

MM::RenderSystem::RenderResourceDataBase::RenderResourceDataBase(
    MM::RenderSystem::RenderResourceDataBase&& other) noexcept
    : ManagedObjectBase(std::move(other)),
      render_resource_data_ID_(std::move(other.render_resource_data_ID_)),
      used_for_write_(other.used_for_write_),
      is_managed_(other.is_managed_),
      is_asset_resource_(other.is_asset_resource_) {
  other.used_for_write_ = false;
  other.is_managed_ = false;
  other.is_asset_resource_ = false;
}

MM::RenderSystem::RenderResourceDataBase&
MM::RenderSystem::RenderResourceDataBase::operator=(
    RenderResourceDataBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  ManagedObjectBase::operator=(std::move(other));
  render_resource_data_ID_ = std::move(other.render_resource_data_ID_);
  used_for_write_ = other.used_for_write_;
  is_managed_ = other.is_managed_;
  is_asset_resource_ = other.is_asset_resource_;

  other.used_for_write_ = false;
  other.is_managed_ = false;
  other.is_asset_resource_ = false;

  return *this;
}

const std::string& MM::RenderSystem::RenderResourceDataBase::GetResourceName()
    const {
  return GetObjectName();
}

MM::RenderSystem::ResourceType
MM::RenderSystem::RenderResourceDataBase::GetResourceType() const {
  return ResourceType::UNDEFINED;
}

void MM::RenderSystem::RenderResourceDataBase::Release() {
  ManagedObjectBase::Reset();
  render_resource_data_ID_.Reset();
  used_for_write_ = false;
  is_managed_ = false;
  is_asset_resource_ = false;
}

bool MM::RenderSystem::RenderResourceDataBase::IsArray() const { return false; }

bool MM::RenderSystem::RenderResourceDataBase::CanWrite() const {
  return false;
}

VkDeviceSize MM::RenderSystem::RenderResourceDataBase::GetSize() const {
  return 0;
}

const MM::RenderSystem::RenderResourceDataID&
MM::RenderSystem::RenderResourceDataBase::GetRenderResourceDataID() const {
  return render_resource_data_ID_;
}

const MM::RenderSystem::RenderResourceDataID&
MM::RenderSystem::RenderResourceDataBase::GetRenderID() const {
  return render_resource_data_ID_;
}

bool MM::RenderSystem::RenderResourceDataBase::operator==(
    const MM::RenderSystem::RenderResourceDataBase& rhs) const {
  return render_resource_data_ID_ == rhs.render_resource_data_ID_;
}

bool MM::RenderSystem::RenderResourceDataBase::operator!=(
    const MM::RenderSystem::RenderResourceDataBase& rhs) const {
  return !(rhs == *this);
}

bool MM::RenderSystem::RenderResourceDataBase::operator<(
    const MM::RenderSystem::RenderResourceDataBase& rhs) const {
  if (static_cast<const MM::Manager::ManagedObjectBase&>(*this) <
      static_cast<const MM::Manager::ManagedObjectBase&>(rhs))
    return true;
  if (static_cast<const MM::Manager::ManagedObjectBase&>(rhs) <
      static_cast<const MM::Manager::ManagedObjectBase&>(*this))
    return false;
  return false;
}

bool MM::RenderSystem::RenderResourceDataBase::operator>(
    const MM::RenderSystem::RenderResourceDataBase& rhs) const {
  return rhs < *this;
}

bool MM::RenderSystem::RenderResourceDataBase::operator<=(
    const MM::RenderSystem::RenderResourceDataBase& rhs) const {
  return !(rhs < *this);
}

bool MM::RenderSystem::RenderResourceDataBase::operator>=(
    const MM::RenderSystem::RenderResourceDataBase& rhs) const {
  return !(*this < rhs);
}

bool MM::RenderSystem::RenderResourceDataBase::IsUseForWrite() const {
  return used_for_write_;
}

void MM::RenderSystem::RenderResourceDataBase::MarkThisUseForWrite() {
  used_for_write_ = true;
}

void MM::RenderSystem::RenderResourceDataBase::SetRenderResourceDataID(
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID) {
  render_resource_data_ID_ = render_resource_data_ID;
}

void MM::RenderSystem::RenderResourceDataBase::MarkThisIsManaged() {
  is_managed_ = true;
}

bool MM::RenderSystem::RenderResourceDataBase::IsManaged() const {
  return is_managed_;
}

bool MM::RenderSystem::RenderResourceDataBase::IsAssetResource() const {
  return is_asset_resource_;
}

void MM::RenderSystem::RenderResourceDataBase::MarkThisIsAssetResource() {
  is_asset_resource_ = true;
}
