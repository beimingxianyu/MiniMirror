//
// Created by beimingxianyu on 23-7-31.
//
namespace MM {
namespace RenderSystem {

class PipelineLayout {
 public:
  PipelineLayout() = default;
  ~PipelineLayout() ;
  PipelineLayout(const PipelineLayout& other) = delete;
  PipelineLayout(PipelineLayout&& other) noexcept;
  PipelineLayout& operator=(const PipelineLayout& other) = delete;
  PipelineLayout& operator=(PipelineLayout&& other) noexcept;



};

}  // namespace RenderSystem
}  // namespace MM
