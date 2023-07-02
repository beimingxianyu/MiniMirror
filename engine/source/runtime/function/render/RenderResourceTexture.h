#pragma once

#include "runtime/function/render/RenderResourceManager.h"
#include "runtime/function/render/vk_type_define.h"
#include "vk_engine.h"

namespace MM {
namespace RenderSystem {
class RenderEngine;

class RenderResourceTexture {
 public:
  using RenderResourceHandler = RenderResourceManager::HandlerType;

 public:
  RenderResourceTexture() = default;
  ~RenderResourceTexture() = default;
  RenderResourceTexture(const RenderResourceTexture& other) = delete;
  RenderResourceTexture(RenderResourceTexture&& other) noexcept = default;
  RenderResourceTexture& operator=(const RenderResourceTexture& other) = delete;
  RenderResourceTexture& operator=(RenderResourceTexture&& other) noexcept =
      default;

 private:
  RenderEngine* render_engine_{nullptr};
  RenderResourceHandler texture_handler_{};
  AllocatedImage* image_{nullptr};
  ImageBindData image_bind_data_{};
};

}  // namespace RenderSystem
}  // namespace MM
