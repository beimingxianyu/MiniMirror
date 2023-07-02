//
// Created by beimingxianyu on 23-6-28.
//

#include "runtime/function/render/RenderResourceDataBase.h"

MM::RenderSystem::RenderResourceDataBase::RenderResourceDataBase(
    const std::string& resource_name,
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID)
    : ManagedObjectBase(resource_name),
      render_resource_data_ID_(render_resource_data_ID) {}

MM::RenderSystem::RenderResourceDataBase&
MM::RenderSystem::RenderResourceDataBase::operator=(
    RenderResourceDataBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  ManagedObjectBase::operator=(std::move(other));
  render_resource_data_ID_ = std::move(other.render_resource_data_ID_);

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

std::unique_ptr<MM::RenderSystem::RenderResourceDataBase>
MM::RenderSystem::RenderResourceDataBase::GetCopy(
    const std::string& new_name_of_copy_resource) const {
  RenderResourceDataBase* new_render_resource = new RenderResourceDataBase(
      new_name_of_copy_resource, render_resource_data_ID_);
  return std::unique_ptr<RenderResourceDataBase>(new_render_resource);
}

void MM::RenderSystem::RenderResourceDataBase::Release() {}

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
  return false
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
