#pragma once

namespace MM {
namespace RenderSystem {

class RenderResourceTexture {
 public:
  RenderResourceTexture() = default;
  ~RenderResourceTexture() = default;
  RenderResourceTexture(const RenderResourceTexture& other) = delete;
  RenderResourceTexture(RenderResourceTexture&& other) noexcept = default;
  RenderResourceTexture& operator=(const RenderResourceTexture& other) = delete;
  RenderResourceTexture& operator=(RenderResourceTexture&& other) noexcept =
      default;

 private:
};

}  // namespace RenderSystem
}  // namespace MM
