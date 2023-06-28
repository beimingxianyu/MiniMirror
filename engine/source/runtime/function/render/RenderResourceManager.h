#include <cstdint>
#include <memory>
#include <vector>

#include "RenderResourceDataID.h"
#include "runtime/core/manager/ManagedObjectBase.h"
#include "runtime/core/manager/ManagedObjectUnorderedMap.h"
#include "runtime/core/manager/ManagerBase.h"
#include "runtime/function/render/RenderResourceBase.h"
#include "runtime/platform/base/error.h"

namespace MM {
namespace RenderSystem {
class RenderResourceManager
    : public Manager::ManagerBase<std::unique_ptr<RenderResourceBase>,
                                  Manager::ManagedObjectIsSmartPoint> {
 public:
  class RenderResourceHandler;

  using ManagedType = std::unique_ptr<RenderResourceBase>;
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
    bool IsValid() const override;

    std::uint32_t GetUseCount() { return GetIDToObjectHandler().GetUseCount(); }

    const RenderResourceDataID& GetRenderResourceDataID() const {
      return render_resource_ID_to_object_ID_handler_.GetKey();
    }

    const std::string& GetRenderResourceDataName() const {
      return GetObjectName();
    }

    RenderResourceBase& GetRenderResourceData() { return *GetObject(); }

    const RenderResourceBase& GetRenderResourceData() const {
      return *GetObject();
    }

    RenderResourceBase* GetRenderResourceDataPtr() { return GetObject().get(); }

    const RenderResourceBase* GetRenderResourceDataPtr() const {
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
    struct OnlyWritten {};
    struct OnlyNotWritten {};
    struct Both {};
  };

 public:
  bool IsValid() const override;

  static RenderResourceManager* GetInstance();

  bool Have(const RenderResourceDataID& render_resource_data_ID) {
    return render_resource_data_ID_to_object_ID_.Have(render_resource_data_ID);
  }

  ExecuteResult AddRenderResourceData(ManagedType&& render_resource_data,
                                      HandlerType& handler) {
    RenderResourceDataID render_resource_data_ID =
        render_resource_data->GetRenderResourceDataID();

    BaseHandlerType base_handler_type;
    MM_CHECK_WITHOUT_LOG(
        AddObjectBase(std::move(render_resource_data), base_handler_type),
        return MM_RESULT_CODE;);

    RenderResourceDataIDToObjectIDContainerType ::HandlerType
        render_resource_ID_to_object_ID_handler;
    MM_CHECK_WITHOUT_LOG(
        render_resource_data_ID_to_object_ID_.AddObject(
            render_resource_data_ID, base_handler_type.GetObjectID(),
            render_resource_ID_to_object_ID_handler),
        return MM_RESULT_CODE;);

    handler = HandlerType{std::move(base_handler_type),
                          std::move(render_resource_ID_to_object_ID_handler)};

    return ExecuteResult ::SUCCESS;
  }

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& render_resource_data_ID,
      HandlerType& handler) const;

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& render_resource_data_ID,
      HandlerType& handler, Policy::Both) const;

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& render_resource_data_ID,
      HandlerType& handler, Policy::OnlyWritten) const;

  ExecuteResult GetRenderResourceDataByID(
      const Manager::ManagedObjectID& render_resource_data_ID,
      HandlerType& handler, Policy::OnlyNotWritten) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers, Policy::Both) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers, Policy::OnlyWritten) const;

  ExecuteResult GetRenderResourceDataByRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<HandlerType>& handlers, Policy::OnlyNotWritten) const;

  ExecuteResult GetRenderResourceDataByName(
      const std::string& name, std::vector<HandlerType>& handlers) const;

  ExecuteResult GetRenderResourceDataByName(const std::string& name,
                                            std::vector<HandlerType>& handlers,
                                            Policy::Both) const;

  ExecuteResult GetRenderResourceDataByName(const std::string& name,
                                            std::vector<HandlerType>& handlers,
                                            Policy::OnlyWritten) const;

  ExecuteResult GetRenderResourceDataByName(const std::string& name,
                                            std::vector<HandlerType>& handlers,
                                            Policy::OnlyNotWritten) const;

  ExecuteResult GetIDBYRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs) const;

  ExecuteResult GetIDBYRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      Policy::Both) const;

  ExecuteResult GetIDBYRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      Policy::OnlyWritten) const;

  ExecuteResult GetIDBYRenderResourceDataID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<Manager::ManagedObjectID>& managed_object_IDs,
      Policy::OnlyNotWritten) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, Policy::Both) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, Policy::OnlyWritten) const;

  ExecuteResult GetNameByRenderResourceID(
      const RenderResourceDataID& render_resource_data_ID,
      std::vector<std::string>& names, Policy::OnlyNotWritten) const;

  ExecuteResult GetRenderResourceIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID) const;

  ExecuteResult GetRenderResourceIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID, Policy::Both) const;

  ExecuteResult GetRenderResourceIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID, Policy::OnlyWritten) const;

  ExecuteResult GetRenderResourceIDByID(
      const Manager::ManagedObjectID& managed_object_ID,
      RenderResourceDataID& render_resource_data_ID,
      Policy::OnlyNotWritten) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name,
      RenderResourceDataID& render_resource_data_ID) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name, RenderResourceDataID& render_resource_data_ID,
      Policy::Both) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name, RenderResourceDataID& render_resource_data_ID,
      Policy::OnlyWritten) const;

  ExecuteResult GetRenderResourceDataIDByName(
      const std::string& name, RenderResourceDataID& render_resource_data_ID,
      Policy::OnlyNotWritten) const;

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
