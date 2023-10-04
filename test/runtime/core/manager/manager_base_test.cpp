//
// Created by beimingxianyu on 23-6-13.
//
#include <gtest/gtest.h>

#include <thread>

#include "runtime/core/manager/ManagerBase.h"

TEST(manager, manager_base) {
  struct TestString : MM::Manager::ManagedObjectBase {
    std::string data_{};

    explicit TestString(const std::string& name)
        : MM::Manager::ManagedObjectBase(name), data_(name + "_data") {}
    ~TestString() override = default;
  };

  class StringManager : public MM::Manager::ManagerBase<TestString> {
   public:
    using HandlerType = MM::Manager::ManagerBase<TestString>::HandlerType;

   public:
    StringManager() = default;
    ~StringManager() override = default;
    explicit StringManager(std::uint64_t size)
        : MM::Manager::ManagerBase<TestString>(size) {}
    StringManager(const StringManager& other) = delete;
    StringManager(StringManager&& other) = delete;
    StringManager& operator=(const StringManager& other) = delete;
    StringManager& operator=(StringManager& other) = delete;

   public:
    MM::Result<HandlerType, MM::ErrorResult> AddObject(TestString&& object) {
      return AddObjectBase(std::move(object));
    }

    MM::Result<HandlerType, MM::ErrorResult> GetObjectByID(
        MM::Manager::ManagedObjectID object_id) const {
      return GetObjectByIDBase(object_id);
    }

    MM::Result<std::vector<HandlerType>, MM::ErrorResult> GetObjectByName(
        const std::string& object_name) const {
      return GetObjectByNameBase(object_name, MM::st_get_multiply_object);
    }
  };

  StringManager manager;

  MM::Manager::ManagedObjectID ID1, ID2;

  ASSERT_EQ(manager.Have(ID1), false);
  ASSERT_EQ(manager.Count(ID1), 0);
  auto handler1 = manager.GetObjectByID(ID2).Exception();
  ASSERT_EQ(handler1.IsSuccess(), false);
  ASSERT_EQ(handler1.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handler1.GetResult().IsValid(), false);
  auto IDs1 =
      manager.GetIDByName("Test", MM::st_get_multiply_object).Exception();
  ASSERT_EQ(IDs1.IsSuccess(), false);
  ASSERT_EQ(IDs1.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(IDs1.GetResult().empty(), true);
  auto handlers1 = manager.GetObjectByName("Test").Exception();
  ASSERT_EQ(handlers1.IsSuccess(), false);
  ASSERT_EQ(handlers1.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handlers1.GetResult().empty(), true);
  handler1 = manager.AddObject(TestString{"TestString1"}).Exception();
  ASSERT_EQ(handler1.IsSuccess(), true);
  ASSERT_EQ(handler1.GetResult().IsValid(), true);
  ASSERT_EQ(handler1.GetResult().GetObjectName(), std::string{"TestString1"});
  auto handler2 = manager.AddObject(TestString{"TestString2"}).Exception();
  ASSERT_EQ(handler2.IsSuccess(), true);
  ASSERT_EQ(handler2.GetResult().IsValid(), true);
  ASSERT_EQ(handler2.GetResult().GetObjectName(), std::string{"TestString2"});
  auto handler3 = manager.AddObject(TestString{"TestStringMult"}).Exception();
  ASSERT_EQ(handler3.IsSuccess(), true);
  ASSERT_EQ(handler3.GetResult().IsValid(), true);
  ASSERT_EQ(handler3.GetResult().GetObjectName(),
            std::string{"TestStringMult"});
  auto handler4 = manager.AddObject(TestString{"TestStringMult"}).Exception();
  ASSERT_EQ(handler4.IsSuccess(), true);
  ASSERT_EQ(handler4.GetResult().IsValid(), true);
  ASSERT_EQ(handler4.GetResult().GetObjectName(),
            std::string{"TestStringMult"});
  ID2 = handler4.GetResult().GetObjectID();
  handler4 = manager.AddObject(TestString{"TestStringMult"}).Exception();
  ASSERT_EQ(handler4.IsSuccess(), true);
  ASSERT_EQ(handler4.GetResult().IsValid(), true);
  ASSERT_EQ(handler4.GetResult().GetObjectName(),
            std::string{"TestStringMult"});
  ASSERT_NE(handler4.GetResult().GetObjectID(), ID2);
  ASSERT_EQ(manager.Have(handler1.GetResult().GetObjectID()), true);
  ASSERT_EQ(manager.Have(handler2.GetResult().GetObjectID()), true);
  ASSERT_EQ(manager.Have(handler3.GetResult().GetObjectID()), true);
  ASSERT_EQ(manager.Have(handler4.GetResult().GetObjectID()), true);
  ASSERT_EQ(manager.Have(ID2), false);
  ASSERT_EQ(manager.Count(handler1.GetResult().GetObjectID()), 1);
  ASSERT_EQ(manager.Count(handler2.GetResult().GetObjectID()), 1);
  ASSERT_EQ(manager.Count(handler3.GetResult().GetObjectID()), 1);
  ASSERT_EQ(manager.Count(handler4.GetResult().GetObjectID()), 1);
  ASSERT_EQ(manager.Count(ID2), 0);
  auto handler5 =
      manager.GetObjectByID(handler4.GetResult().GetObjectID()).Exception();
  ASSERT_EQ(handler5.IsSuccess(), true);
  ASSERT_EQ(handler5.GetResult().GetObjectName(),
            handler3.GetResult().GetObjectName());
  ASSERT_EQ(handler5.GetResult().GetObjectName(),
            handler4.GetResult().GetObjectName());
  ASSERT_NE(handler5.GetResult().GetObjectID(),
            handler3.GetResult().GetObjectID());
  ASSERT_EQ(handler5.GetResult().GetObjectID(),
            handler4.GetResult().GetObjectID());
  auto handlers2 = manager.GetObjectByName("TestStringMult").Exception();
  ASSERT_EQ(handlers2.IsSuccess(), true);
  ASSERT_EQ(handlers2.GetResult().size(), 2);
  ASSERT_EQ(handlers2.GetResult()[0].GetObjectName(),
            std::string("TestStringMult"));
  ASSERT_EQ(handlers2.GetResult()[1].GetObjectName(),
            std::string("TestStringMult"));
  auto handlers3 = manager.GetObjectByName("TestString1").Exception();
  ASSERT_EQ(handlers3.IsSuccess(), true);
  ASSERT_EQ(handlers3.GetResult().size(), 1);
  ASSERT_EQ(handlers3.GetResult()[0].GetObjectName(),
            std::string("TestString1"));
  auto handlers4 = manager.GetObjectByName("TestString5").Exception();
  ASSERT_EQ(handlers4.IsSuccess(), false);
  ASSERT_EQ(handlers4.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handlers4.GetResult().empty(), true);

  auto IDs2 = manager.GetObjectByName("TestStringMult").Exception();
  ASSERT_EQ(IDs2.IsSuccess(), true);
  ASSERT_EQ(IDs2.GetResult().size(), 2);
  auto IDs3 = manager.GetObjectByName("TestString1").Exception();
  ASSERT_EQ(IDs3.IsSuccess(), true);
  ASSERT_EQ(IDs3.GetResult().size(), 1);
  auto IDs4 = manager.GetIDByName("TestString5", MM::st_get_multiply_object)
                  .Exception();
  ASSERT_EQ(IDs4.IsSuccess(), false);
  ASSERT_EQ(IDs4.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(IDs4.GetResult().empty(), true);

  handler1.GetResult().Release();
  handler2.GetResult().Release();
  handler3.GetResult().Release();
  handler4.GetResult().Release();
  handler5.GetResult().Release();

  handlers1.GetResult().clear();
  handlers2.GetResult().clear();
  handlers3.GetResult().clear();
  handlers4.GetResult().clear();
  ASSERT_EQ(manager.Have(ID1), false);
  ASSERT_EQ(manager.Count(ID1), 0);
  handler1 = manager.GetObjectByID(ID2).Exception();
  ASSERT_EQ(handler1.IsSuccess(), false);
  ASSERT_EQ(handler1.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handler1.GetResult().IsValid(), false);
  IDs1 = manager.GetIDByName("Test", MM::st_get_multiply_object).Exception();
  ASSERT_EQ(IDs1.IsSuccess(), false);
  ASSERT_EQ(IDs1.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(IDs1.GetResult().empty(), true);
  handlers1 = manager.GetObjectByName("Test").Exception();
  ASSERT_EQ(handlers1.IsSuccess(), false);
  ASSERT_EQ(handlers1.GetError().GetErrorCode(),
            MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handlers1.GetResult().empty(), true);
}

TEST(manager, manager_base_thread) {
  struct TestString : MM::Manager::ManagedObjectBase {
    std::string data_{};

    explicit TestString(const std::string& name)
        : MM::Manager::ManagedObjectBase(name), data_(name + "_data") {}
    ~TestString() override = default;
  };

  class StringManager : public MM::Manager::ManagerBase<TestString> {
   public:
    using HandlerType = MM::Manager::ManagerBase<TestString>::HandlerType;

   public:
    StringManager() = default;
    ~StringManager() override = default;
    explicit StringManager(std::uint64_t size)
        : MM::Manager::ManagerBase<TestString>(size) {}
    StringManager(const StringManager& other) = delete;
    StringManager(StringManager&& other) = delete;
    StringManager& operator=(const StringManager& other) = delete;
    StringManager& operator=(StringManager& other) = delete;

   public:
    MM::Result<HandlerType, MM::ErrorResult> AddObject(TestString&& object) {
      return AddObjectBase(std::move(object));
    }

    MM::Result<HandlerType, MM::ErrorResult> GetObjectByID(
        MM::Manager::ManagedObjectID object_id) const {
      return GetObjectByIDBase(object_id);
    }

    MM::Result<std::vector<HandlerType>, MM::ErrorResult> GetObjectByName(
        const std::string& object_name) const {
      return GetObjectByNameBase(object_name, MM::st_get_multiply_object);
    }
  };

  StringManager manager;

  std::uint64_t batch_count = 500000;
  std::vector<std::vector<typename StringManager::HandlerType>> handlers_vector(
      8);

  std::vector<std::thread> threads;

  for (std::uint32_t i = 0; i != 8; ++i) {
    threads.emplace_back([&manager, batch_count, i,
                          &handlers = handlers_vector[i]]() {
      for (std::uint32_t j = 0; j != batch_count; ++j) {
        std::string name = std::to_string(i * batch_count / 2 + j);
        auto handler =
            manager
                .AddObject(TestString{std::to_string(i * batch_count / 2 + j)})
                .Exception();
        ASSERT_EQ(handler.IsSuccess(), true);
        handlers.emplace_back(std::move(handler.GetResult()));
        ASSERT_EQ(handlers.back().IsValid(), true);
        ASSERT_EQ(handlers.back().GetObjectName(), name);
        ASSERT_EQ(manager.Have(handlers.back().GetObjectID()), true);
        ASSERT_EQ(manager.Count(handlers.back().GetObjectID()), 1);
      }
    });
  }

  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(manager.GetSize(), 8 * batch_count);

  threads.clear();

  for (std::uint32_t i = 0; i != 8; ++i) {
    threads.emplace_back(
        [&manager, i, batch_count, &handlers = handlers_vector[i]]() {
          for (std::uint32_t j = 0; j != batch_count; ++j) {
            MM::Manager::ManagedObjectID ID = handlers[j].GetObjectID();
            handlers[j].Release();
            ASSERT_EQ(manager.Have(ID), false);
          }
          handlers.clear();
        });
  }

  for (auto& th : threads) {
    th.join();
  }

  ASSERT_EQ(manager.GetSize(), 0);
}