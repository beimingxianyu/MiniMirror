//
// Created by beimingxianyu on 23-6-28.
//

#include "runtime/function/render/RenderResourceBase.h"

MM::RenderSystem::RenderResourceBase::RenderResourceBase(
    const std::string& resource_name,
    const MM::RenderSystem::RenderResourceDataID& render_resource_data_ID)
    : ManagedObjectBase(resource_name),
      render_resource_data_ID_(render_resource_data_ID) {}

MM::RenderSystem::RenderResourceBase&
MM::RenderSystem::RenderResourceBase::operator=(
    RenderResourceBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  ManagedObjectBase::operator=(std::move(other));
  render_resource_data_ID_ = std::move(other.render_resource_data_ID_);

  return *this;
}

const std::string& MM::RenderSystem::RenderResourceBase::GetResourceName()
    const {
  return GetObjectName();
}

MM::RenderSystem::ResourceType
MM::RenderSystem::RenderResourceBase::GetResourceType() const {
  return ResourceType::UNDEFINED;
}

std::unique_ptr<MM::RenderSystem::RenderResourceBase>
MM::RenderSystem::RenderResourceBase::GetCopy(
    const std::string& new_name_of_copy_resource) const {
  RenderResourceBase* new_render_resource = new RenderResourceBase(
      new_name_of_copy_resource, render_resource_data_ID_);
  return std::unique_ptr<RenderResourceBase>(new_render_resource);
}

void MM::RenderSystem::RenderResourceBase::Release() {}

bool MM::RenderSystem::RenderResourceBase::IsArray() const { return false; }

bool MM::RenderSystem::RenderResourceBase::CanWrite() const { return false; }

VkDeviceSize MM::RenderSystem::RenderResourceBase::GetSize() const { return 0; }

const MM::RenderSystem::RenderResourceDataID&
MM::RenderSystem::RenderResourceBase::GetRenderResourceDataID() const {
  return render_resource_data_ID_;
}

bool MM::RenderSystem::RenderResourceBase::operator==(
    const MM::RenderSystem::RenderResourceBase& rhs) const {
  return render_resource_data_ID_ == rhs.render_resource_data_ID_;
}

bool MM::RenderSystem::RenderResourceBase::operator!=(
    const MM::RenderSystem::RenderResourceBase& rhs) const {
  return !(rhs == *this);
}

bool MM::RenderSystem::RenderResourceBase::operator<(
    const MM::RenderSystem::RenderResourceBase& rhs) const {
  if (static_cast<const MM::Manager::ManagedObjectBase&>(*this) <
      static_cast<const MM::Manager::ManagedObjectBase&>(rhs))
    return true;
  if (static_cast<const MM::Manager::ManagedObjectBase&>(rhs) <
      static_cast<const MM::Manager::ManagedObjectBase&>(*this))
    return false;
  return false
}

bool MM::RenderSystem::RenderResourceBase::operator>(
    const MM::RenderSystem::RenderResourceBase& rhs) const {
  return rhs < *this;
}

bool MM::RenderSystem::RenderResourceBase::operator<=(
    const MM::RenderSystem::RenderResourceBase& rhs) const {
  return !(rhs < *this);
}

bool MM::RenderSystem::RenderResourceBase::operator>=(
    const MM::RenderSystem::RenderResourceBase& rhs) const {
  return !(*this < rhs);
}

bool MM::RenderSystem::RenderResourceBase::IsUseForWrite() const {
  return used_for_write_;
}

void MM::RenderSystem::RenderResourceBase::MarkThisUseForWrite() {
  used_for_write_ = true;
}
