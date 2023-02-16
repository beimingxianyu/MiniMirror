#pragma once

#include "runtime/core/reflection/type.h"

namespace MM {
namespace Reflection {

class Variable;

class DestructorWrapperBase {
 public:
  DestructorWrapperBase() = default;
  virtual ~DestructorWrapperBase() = default;
  DestructorWrapperBase(const DestructorWrapperBase& other) = default;
  DestructorWrapperBase(DestructorWrapperBase&& other) noexcept = default;
  DestructorWrapperBase& operator=(const DestructorWrapperBase& other) =
      default;
  DestructorWrapperBase& operator=(DestructorWrapperBase&& other) noexcept =
      default;

public:
  /**
   * \brief Gets the \ref MM::Reflection::Type of the this constructor.
   * \return The \ref MM::Reflection::Type  of the this constructor.
   */
  virtual Type GetType() const = 0;

  /**
   * \brief Gets the \ref MM::Reflection::Meta of the this constructor.
   * \return The \ref MM::Reflection::Meta of the this constructor.
   */
  virtual std::weak_ptr<Meta> GetMeta() const = 0;

  /**
   * \brief Destroy \ref object.
   * \param object The object to be destructed.
   */
  virtual bool Destroy(Variable& object) const = 0;
};

struct DefaultDestructor {};

template <typename TargetType_, typename DestructorType_>
class DestructorWrapper : public DestructorWrapperBase {
 public:
  DestructorWrapper() = default;
  ~DestructorWrapper() override = default;
  DestructorWrapper(const DestructorWrapper& other) = default;
  DestructorWrapper(DestructorWrapper&& other) noexcept = default;
  DestructorWrapper& operator=(const DestructorWrapper& other) = default;
  DestructorWrapper& operator=(DestructorWrapper&& other) noexcept = default;

 public:
  /**
   * \brief Gets the \ref MM::Reflection::Type of the this constructor.
   * \return The \ref MM::Reflection::Type  of the this constructor.
   */
  Type GetType() const override;

  /**
   * \brief Gets the \ref MM::Reflection::Meta of the this constructor.
   * \return The \ref MM::Reflection::Meta of the this constructor.
   */
  std::weak_ptr<Meta> GetMeta() const override;

  /**
   * \brief Destroy \ref object.
   * \param object The object to be destructed.
   * \return If the type of \ref TargetType_ is same of the type of \ref object,
   * return true,otherwise return false.
   */
  bool Destroy(Variable& object) const override;
};

template <typename TargetType_>
class DestructorWrapper<TargetType_, DefaultDestructor>
    : public DestructorWrapperBase {
 public:
  DestructorWrapper() = default;
  ~DestructorWrapper() override = default;
  DestructorWrapper(const DestructorWrapper& other) = default;
  DestructorWrapper(DestructorWrapper&& other) noexcept = default;
  DestructorWrapper& operator=(const DestructorWrapper& other) = default;
  DestructorWrapper& operator=(DestructorWrapper&& other) noexcept = default;

 public:
  /**
   * \brief Gets the \ref MM::Reflection::Type of the this constructor.
   * \return The \ref MM::Reflection::Type  of the this constructor.
   */
  Type GetType() const override;

  /**
   * \brief Gets the \ref MM::Reflection::Meta of the this constructor.
   * \return The \ref MM::Reflection::Meta of the this constructor.
   */
  std::weak_ptr<Meta> GetMeta() const override;

  /**
   * \brief Destroy \ref object.
   * \param object The object to be destructed.
   * \return If the type of \ref TargetType_ is same of the type of \ref object,
   * return true,otherwise return false.
   */
  bool Destroy(Variable& object) const override;
};

template <typename TargetType_, typename DestructorType_>
Type DestructorWrapper<TargetType_, DestructorType_>::GetType() const {
  return MM::Reflection::CreateType<TargetType_>();
}

template <typename TargetType_, typename DestructorType_>
std::weak_ptr<Meta> DestructorWrapper<TargetType_, DestructorType_>::GetMeta()
    const {
  return GetType().GetMate();
}

template <typename TargetType_>
Type DestructorWrapper<TargetType_, DefaultDestructor>::GetType() const {
  return MM::Reflection::CreateType<TargetType_>();
}

template <typename TargetType_>
std::weak_ptr<Meta> DestructorWrapper<TargetType_, DefaultDestructor>::GetMeta()
    const {
  return GetType().GetMate();
}

class Destructor {
  friend class Meta;

public:
  Destructor() = default;
 Destructor(const Destructor& other) = default;
  Destructor(Destructor&& other) noexcept = default;
 Destructor& operator=(const Destructor& other) = default;
  Destructor& operator=(Destructor&& other) noexcept = default;

 public:
  /**
   * \brief Judge whether the object is a valid object.
   * \return Returns true if the object is a valid object, otherwise returns
   * false.
   */
  bool IsValid() const;

  /**
   * \brief Gets the \ref MM::Reflection::Type of the this constructor.
   * \return The \ref MM::Reflection::Type  of the this constructor.
   */
  Type GetType() const;

  /**
   * \brief Gets the \ref MM::Reflection::Meta of the this constructor.
   * \return The \ref MM::Reflection::Meta of the this constructor.
   */
  std::weak_ptr<Meta> GetMeta() const;

  /**
   * \brief Destroy \ref object.
   * \param object The object to be destructed.
   * \return Returns true if the destruction is successful, otherwise returns
   * false.
   */
  bool Destroy(Variable& object) const;

private:
  Destructor(const std::shared_ptr<DestructorWrapperBase>& other) : destructor_wrapper_(other) {}

  static Destructor CreateDestructor(
      const std::shared_ptr<DestructorWrapperBase>& other);

 private:
  std::weak_ptr<DestructorWrapperBase> destructor_wrapper_{};
};
}  // namespace Reflection
}  // namespace MM
