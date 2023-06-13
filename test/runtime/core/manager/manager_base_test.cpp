//
// Created by beimingxianyu on 23-6-13.
//
#include <gtest/gtest.h>

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
    MM::ExecuteResult AddObject(TestString&& object, HandlerType& handler) {
      return AddObjectBase(std::move(object), handler);
    }

    MM::ExecuteResult GetObjectByID(MM::Manager::ManagedObjectID object_id,
                                    HandlerType& handler) const {
      return GetObjectByIDBase(object_id, handler);
    }

    MM::ExecuteResult GetObjectByName(
        const std::string& object_name,
        std::vector<HandlerType>& handlers) const {
      return GetObjectByNameBase(object_name, handlers);
    }
  };

  StringManager manager;

  StringManager::HandlerType handler1, handler2, handler3, handler4, handler5;
  std::vector<StringManager::HandlerType> handlers1, handlers2, handlers3,
      handlers4;
  MM::Manager::ManagedObjectID ID1, ID2;
  std::vector<MM::Manager::ManagedObjectID> IDs1, IDs2, IDs3, IDs4;

  ASSERT_EQ(manager.Have(ID1), false);
  ASSERT_EQ(manager.Count(ID1), 0);
  ASSERT_EQ(manager.GetObjectByID(ID2, handler1),
            MM::ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handler1.IsValid(), false);
  ASSERT_EQ(manager.GetIDsByName("Test", IDs1),
            MM::ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(IDs1.empty(), true);
  ASSERT_EQ(manager.GetObjectByName("Test", handlers1),
            MM::ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handlers1.empty(), true);

  ASSERT_EQ(manager.AddObject(TestString{"TestString1"}, handler1),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handler1.IsValid(), true);
  ASSERT_EQ(handler1.GetObject().data_, std::string{"TestString1"});
  ASSERT_EQ(manager.AddObject(TestString{"TestString2"}, handler2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handler2.IsValid(), true);
  ASSERT_EQ(handler2.GetObject(), TestString{"TestString2"});
  ASSERT_EQ(manager.AddObject(TestString{"TestStringMult"}, handler3),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handler3.IsValid(), true);
  ASSERT_EQ(handler3.GetObject(), TestString{"TestStringMult"});
  ASSERT_EQ(manager.AddObject(TestString{"TestStringMult"}, handler4),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handler4.IsValid(), true);
  ASSERT_EQ(handler4.GetObject(), TestString{"TestStringMult"});
  ID2 = handler3.GetObjectID();
  ASSERT_EQ(manager.AddObject(TestString{"TestStringMult"}, handler4),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handler1.IsValid(), true);
  ASSERT_EQ(handler1.GetObject(), TestString{"TestStringMult"});
  ASSERT_NE(handler4.GetObjectID(), ID2);
  ASSERT_EQ(manager.Have(handler1.GetObjectID()), true);
  ASSERT_EQ(manager.Have(handler2.GetObjectID()), true);
  ASSERT_EQ(manager.Have(handler3.GetObjectID()), true);
  ASSERT_EQ(manager.Have(handler4.GetObjectID()), true);
  ASSERT_EQ(manager.Have(ID2), false);
  ASSERT_EQ(manager.Count(handler1.GetObjectID()), 1);
  ASSERT_EQ(manager.Count(handler2.GetObjectID()), 1);
  ASSERT_EQ(manager.Count(handler3.GetObjectID()), 2);
  ASSERT_EQ(manager.Count(handler4.GetObjectID()), 2);
  ASSERT_EQ(manager.Count(ID2), 0);
  ASSERT_EQ(manager.GetObjectByID(handler4.GetObjectID(), handler5),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handler5.GetObjectName(), handler3.GetObjectName());
  ASSERT_EQ(handler5.GetObjectName(), handler4.GetObjectName());
  ASSERT_NE(handler5.GetObjectID(), handler3.GetObjectID());
  ASSERT_EQ(handler5.GetObjectID(), handler4.GetObjectID());
  ASSERT_EQ(manager.GetObjectByName("TestStringMult", handlers2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handlers2.size(), 2);
  ASSERT_EQ(handlers2[0].GetObjectName(), std::string("TestStringMult"));
  ASSERT_EQ(handlers2[1].GetObjectName(), std::string("TestStringMult"));
  ASSERT_EQ(manager.GetObjectByName("TestString1", handlers3),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handlers3.size(), 1);
  ASSERT_EQ(handlers3[0].GetObjectName(), std::string("TestString1"));
  ASSERT_EQ(manager.GetObjectByName("TestString5", handlers4),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(handlers4.empty(), true);

  ASSERT_EQ(manager.GetIDsByName("TestStringMult", IDs2),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(IDs2.size(), 2);
  ASSERT_EQ(manager.GetIDsByName("TestString1", IDs3),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(IDs3.size(), 1);
  ASSERT_EQ(manager.GetIDsByName("TestString5", IDs4),
            MM::ExecuteResult::SUCCESS);
  ASSERT_EQ(IDs4.empty(), true);

  handler1.Release();
  handler2.Release();
  handler3.Release();
  handler4.Release();
  handler5.Release();

  handlers1.clear();
  handlers2.clear();
  handlers3.clear();
  handlers4.clear();

  ASSERT_EQ(manager.Have(ID1), false);
  ASSERT_EQ(manager.Count(ID1), 0);
  ASSERT_EQ(manager.GetObjectByID(ID2, handler1),
            MM::ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handler1.IsValid(), false);
  ASSERT_EQ(manager.GetIDsByName("Test", IDs1),
            MM::ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(IDs1.empty(), true);
  ASSERT_EQ(manager.GetObjectByName("Test", handlers1),
            MM::ExecuteResult::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
  ASSERT_EQ(handlers1.empty(), true);
}
