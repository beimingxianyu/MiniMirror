#pragma once

#include "runtime/function/render/AllocatedBuffer.h"

namespace MM {
namespace RenderSystem {

class AllocatedMesh {
 public:
  AllocatedMesh() = default;
  ~AllocatedMesh() = default;
  explicit AllocatedMesh(RenderEngine* render_engine);
  AllocatedMesh(RenderEngine* render_engine, std::uint32_t vertex_count);
  AllocatedMesh(const AllocatedMesh& other) = delete;
  AllocatedMesh(AllocatedMesh&& other) noexcept;
  AllocatedMesh& operator=(const AllocatedMesh& other) = delete;
  AllocatedMesh& operator=(AllocatedMesh&& other) noexcept;

 public:
  bool IsValid() const;

  void Release();

 private:
  RenderEngine* render_engine_{nullptr};
  AllocatedBuffer vertex_buffer_{};
  AllocatedBuffer index_buffer_{};
};

}  // namespace RenderSystem
}  // namespace MM
