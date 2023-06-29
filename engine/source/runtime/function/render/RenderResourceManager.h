#include <cstdint>
#include <memory>
#include <vector>

#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/ManagedObjectUnorderedMap.h"
#include "runtime/core/manager/ManagerBase.h"
#include "runtime/function/render/RenderResourceDataBase.h"
#include "runtime/function/render/RenderResourceDataID.h"
#include "runtime/function/render/import_other_system.h"
#include "runtime/platform/base/error.h"
#include "utils/marco.h"

namespace MM {
namespace RenderSystem {
class RenderResourceManager
    : public Manager::ManagerBase<std::unique_ptr<RenderResourceDataBase>,
                                  Manager::ManagedObjectIsSmartPoint> {
 public:
  class RenderResourceHandler;

  using ManagedType = std::unique_ptr<RenderResourceDataBase>;
  using BaseManegerType =
      Manager::ManagerBase<ManagedType, Manager::ManagedObjectIsSmartPoint>;
  using HandlerType = RenderResourceHandler;
  using BaseHandlerType = typename BaseManegerType ::HandlerType;
  using RenderResourceDataIDToObjectIDContainerType =
      Manager::ManagedObjectUnorderedMultiMap<RenderResourceDataID,
                                              Manager::ManagedObjectID>;

 public:
  RenderResourceManager(const RenderResourceManager& other) = delete;
  RenderResourceManager(RenderResourceManager&& other) noexcept = delete;
  RenderResourceManager& operator=(const RenderResourceManager& other) = delete;
  RenderResourceManager& operator=(RenderResourceManager&& other) noexcept =
      delete;

 public:
  class RenderResourceHandler : public BaseHandlerType {
   public:
    RenderResourceHandler() = default;
    ~RenderResourceHandler() = default;
    RenderResourceHandler(
        BaseHandlerType&& base_handler,
        RenderResourceDataIDToObjectIDContainerType::HandlerType&&
            render_resource_ID_to_object_ID_handler)
        : BaseHandlerType(std::move(base_handler)),
          render_resource_ID_to_object_ID_handler_(
              std::move(render_resource_ID_to_object_ID_handler)) {}
    RenderResourceHandler(const RenderResourceHandler& other) = default;
    RenderResourceHandler(RenderResourceHandler&& other) noexcept = default;
    RenderResourceHandler& operator=(const RenderResourceHandler& other) {
      if (&other == this) {
        return *this;
      }

      BaseHandlerType ::operator=(other);
      render_resource_ID_to_object_ID_handler_ =
          other.render_resource_ID_to_object_ID_handler_;

      return *this;
    }
    RenderResourceHandler& operator=(RenderResourceHandler&& other) noexcept {
      if (&other == this) {
        return *this;
      }

      BaseHandlerType ::operator=(std::move(other));
      render_resource_ID_to_object_ID_handler_ =
          std::move(other.render_resource_ID_to_object_ID_handler_);

      return *this;
    }

   public:
    bool IsValid() const override {
      return BaseHandlerType ::IsValid() &&
             render_resource_ID_to_object_ID_handler_.IsValid();
    }

    std::uint32_t GetUseCount() { return GetIDToObjectHandler().GetUseCount(); }

    const RenderResourceDataID& GetRenderResourceDataID() const {
      return render_resource_ID_to_object_ID_handler_.GetKey();
    }

    const std::string& GetRenderResourceDataName() const {
      return GetObjectName();
    }

    RenderResourceDataBase& GetRenderResourceData() { return *GetObject(); }

    const RenderResourceDataBase& GetRenderResourceData() const {
      return *GetObject();
    }

    RenderResourceDataBase* GetRenderResourceDataPtr() {
      return GetObject().get();
    }

    const RenderResourceDataBase* GetRenderResourceDataPtr() const {
      return GetObject().get();
    }

    void Release() override {
      render_resource_ID_to_object_ID_handler_.Release();
      BaseHandlerType ::Release();
    }

    typename RenderResourceDataIDToObjectIDContainerType ::HandlerType&
    GetRenderResourceIDToObjectIDHandler() {
      return render_resource_ID_to_object_ID_handler_;
    }

    const typename RenderResourceDataIDToObjectIDContainerType ::HandlerType&
    GetRenderResourceIDToObjectIDHandler() const {
      return render_resource_ID_to_object_ID_handler_;
    }

   private:
    RenderResourceDataIDToObjectIDContainerType ::HandlerType
        render_resource_ID_to_object_ID_handler_;
  };

  struct Policy {
    struct OnlyWrittenType {};
    struct OnlyNotWrittenType {};
    struct BothType {};

    static OnlyNotWrittenType only_not_written_;
    static OnlyWrittenType only_written_;
    static BothType both_;
  };

 public:
  bool IsValid() const override;

  static RenderResourceManager* GetInstance();

  bool Have(const RenderResourceDataID& render_resource_data_ID);

  ExecuteResult AddRenderResourceData(ManagedType&& render_resource_data,
                                      HandlerType& handler);

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler) const;

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler,
      Policy::BothType) const;

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler,
      Policy::OnlyWrittenType) const;

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& object_ID, HandlerType& handler,
      Policy::OnlyNotWrittenType) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers, Policy::BothType) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers, Policy::OnlyWrittenType) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers, Policy::OnlyNotWrittenType) const;

  ExecuteResult GetRenderResourceDataByName(
      const std::string& name, std::vector<HandlerType>& handlers) const;

  ExecuteResult GetRenderResourceDataByName(const std::string& name,
                                            std::vector<HandlerType>& handlers,
                                            Policy::BothType) const;

  ExecuteResult GetRenderResourceDataByName(const std::string& name,
                                            std::vector<HandlerType>& handlers,
                                            Policy::OnlyWrittenType) const;

  ExecuteResult GetRenderResourceDataByName(const std::string& name,
                                            std::vector<HandlerType>& handlers,
                                            Policy::OnlyNotWrittenType) const;

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs) const;

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      Policy::BothType) const;

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      Policy::OnlyWrittenType) const;

  ExecuteResult GetIDByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      Policy::OnlyNotWrittenType) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, Policy::BothType) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, Policy::OnlyWrittenType) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, Policy::OnlyNotWrittenType) const;

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID) const;

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID, Policy::BothType) const;

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID,
      Policy::OnlyWrittenType) const;

  ExecuteResult GetRenderResourceDataIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID,
      Policy::OnlyNotWrittenType) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs,
      Policy::BothType) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs,
      Policy::OnlyWrittenType) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      std::vector<RenderResourceDataID>& render_resource_data_IDs,
      Policy::OnlyNotWrittenType) const;

 protected:
  RenderResourceManager() = default;
  explicit RenderResourceManager(std::uint64_t size);

 private:
  ~RenderResourceManager() = default;

  static bool Destroy();

 protected:
  static RenderResourceManager* render_resource_manager_;

 private:
  RenderResourceDataIDToObjectIDContainerType
      render_resource_data_ID_to_object_ID_{};

  static std::mutex sync_flag_;
};

}  // namespace RenderSystem
}  // namespace MM
