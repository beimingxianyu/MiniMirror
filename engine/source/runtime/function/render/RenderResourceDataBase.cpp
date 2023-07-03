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
      have_data_(false) {}

MM::RenderSystem::RenderResourceDataBase&
MM::RenderSystem::RenderResourceDataBase::operator=(
    RenderResourceDataBase&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  ManagedObjectBase::operator=(std::move(other));
  render_resource_data_ID_ = std::move(other.render_resource_data_ID_);
  used_for_write_ = other.used_for_write_;
  have_data_ = other.have_data_;

  other.used_for_write_ = false;
  other.have_data_ = false;

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

bool MM::RenderSystem::RenderResourceDataBase::IsHaveData() const {
  return have_data_;
}

void MM::RenderSystem::RenderResourceDataBase::MarkHaveData() {
  have_data_ = true;
}

MM::ExecuteResult
MM::RenderSystem::RenderResourceDataBase::TransformQueueFamily(
    std::uint32_t new_queue_family_index) {
  return ExecuteResult ::UNDEFINED_ERROR;
}
