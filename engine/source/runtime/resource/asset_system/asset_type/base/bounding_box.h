#pragma once

#include "runtime/core/math/math.h"
#include "runtime/resource/asset_system/asset_type/base/vertex.h"
#include "utils/Json.h"
#include "utils/error.h"
#include "utils/type_utils.h"

namespace MM {
namespace AssetSystem {
namespace AssetType {
class Mesh;

class BoundingBox {
 public:
  enum class BoundingBoxType { AABB, CAPSULE };

  BoundingBox() = default;
  virtual ~BoundingBox() = default;
  BoundingBox(const BoundingBox& other) = default;
  BoundingBox(BoundingBox&& other) noexcept = default;
  BoundingBox& operator=(const BoundingBox& other);

  BoundingBox& operator=(BoundingBox&& other) noexcept;

  virtual bool IsValid() const = 0;

  virtual BoundingBoxType GetBoundingType() const = 0;

  virtual Result<Nil, ErrorResult> UpdateBoundingBox(const Mesh& mesh) = 0;

  virtual void UpdateBoundingBoxWithOneVertex(const Vertex& vertex) = 0;

  virtual Utils::Json::Value GetJson(
      Utils::Json::MemoryPoolAllocator<>& allocator) const;
};

class RectangleBox : public BoundingBox {
 public:
  RectangleBox() = default;
  ~RectangleBox() override = default;
  RectangleBox(const Math::vec3& left_bottom_forward,
               const Math::vec3& right_top_back);
  RectangleBox(const RectangleBox& other) = default;
  RectangleBox(RectangleBox&& other) noexcept = default;
  RectangleBox& operator=(const RectangleBox& other);
  RectangleBox& operator=(RectangleBox&& other) noexcept;

 public:
  float& GetTop();
  const float& GetTop() const;
  void SetTop(const float& new_top);

  float& GetBottom();
  const float& GetBottom() const;
  void SetBottom(const float& new_bottom);

  float& GetLeft();
  const float& GetLeft() const;
  void SetLeft(const float& new_left);

  float& GetRight();
  const float& GetRight() const;
  void SetRight(const float& new_right);

  float& GetForward();
  const float& GetForward() const;
  void SetForward(const float& new_forward);

  float& GetBack();
  const float& GetBack() const;
  void SetBack(const float& new_back);

  Math::vec3& GetLeftBottomForward();
  const Math::vec3& GetLeftBottomForward() const;
  void SetLeftBottomForward(const Math::vec3& new_left_bottom_forward);

  Math::vec3& GetRightTopBack();
  const Math::vec3& GetRightTopBack() const;
  void SetRightTopBack(const Math::vec3& new_right_top_back);

  bool IsValid() const override;

  BoundingBoxType GetBoundingType() const override;

  Result<Nil, ErrorResult> UpdateBoundingBox(const Mesh& mesh) override;

  void UpdateBoundingBoxWithOneVertex(const Vertex& vertex) override;

  Utils::Json::Value GetJson(
      Utils::Json::MemoryPoolAllocator<>& allocator) const override;

  friend void Swap(RectangleBox& lhs, RectangleBox& rhs) noexcept {
    using std::swap;
    swap(lhs.left_bottom_forward_, rhs.left_bottom_forward_);
    swap(lhs.right_top_back_, rhs.right_top_back_);
  }

  friend void swap(RectangleBox& lhs, RectangleBox& rhs) noexcept {
    using std::swap;
    swap(lhs.left_bottom_forward_, rhs.left_bottom_forward_);
    swap(lhs.right_top_back_, rhs.right_top_back_);
  }

 private:
  /**
   * \brief The offset value from the center of the mesh.
   */
  Math::vec3 left_bottom_forward_{MathDefinition::VEC3_ZERO};
  /**
   * \brief The offset value from the center of the mesh.
   */
  Math::vec3 right_top_back_{MathDefinition::VEC3_ZERO};
};

class CapsuleBox : public BoundingBox {
 public:
  CapsuleBox() = default;
  ~CapsuleBox() override = default;
  CapsuleBox(const CapsuleBox& other) = default;
  CapsuleBox(CapsuleBox&& other) noexcept = default;
  CapsuleBox& operator=(const CapsuleBox& other);
  CapsuleBox& operator=(CapsuleBox&& other) noexcept;

 public:
  float& GetRadius();
  const float& GetRadius() const;
  void SetRadius(const float& new_radius);

  float& GetTop();
  const float& GetTop() const;
  void SetTop(const float& new_top);

  float& GetBottom();
  const float& GetBottom() const;
  void SetBottom(const float& new_bottom);

  RectangleBox GetRectangleBoxFormThis() const;

  bool IsValid() const override;

  BoundingBoxType GetBoundingType() const override;

  Result<Nil, ErrorResult> UpdateBoundingBox(const Mesh& mesh) override;

  void UpdateBoundingBoxWithOneVertex(const Vertex& vertex) override;

  Utils::Json::Value GetJson(
      Utils::Json::MemoryPoolAllocator<>& allocator) const override;

 private:
  float radius_{0.0f};
  float top_{0.0f};
  float bottom_{0.0f};
  /*
   *
   *          ====     <------ top
   *        =      =
   *      =          =
   *      =          =
   *      =          =
   * ---> =          = <------ radius
   *      =          =
   *      =          =
   *      =          =
   *       =        =
   *          ====     <------ bottom
   */
};
}  // namespace AssetType
}  // namespace AssetSystem
}  // namespace MM
