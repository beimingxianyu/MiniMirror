//
// Created by beimingxianyu on 23-5-20.
//

#include <gtest/gtest.h>

#include <list>
#include <thread>

#include "runtime/core/manager/ManagedObjectSet.h"

TEST(manager, set) {
  MM::Manager::ManagedObjectSet<int> set_manager, set_manager2;
  {
    //    MM::Manager::ManagedObjectSet<int>::HandlerType handler;
    {
      EXPECT_EQ(set_manager.GetSize(), 0);
      auto handler1 = set_manager.AddObject(10).Exception();
      EXPECT_EQ(handler1.Success(), true);
      EXPECT_EQ(set_manager.GetSize(), 1);
      EXPECT_EQ(handler1.GetResult().GetObject(), 10);
      EXPECT_EQ(set_manager.Have(10), true);
      EXPECT_EQ(set_manager.Have(20), false);
      EXPECT_EQ(set_manager.GetSize(10), 1);
      EXPECT_EQ(set_manager.GetSize(20), 0);
      EXPECT_EQ(set_manager.IsMultiContainer(), false);
      EXPECT_EQ(set_manager.IsRelationshipContainer(), false);
      auto handler2 = set_manager.GetObject(10).Exception();
      EXPECT_EQ(handler2.Success(), true);
      auto handler3 = set_manager.GetObject(20).Exception();
      EXPECT_EQ(handler3.GetError().GetErrorCode(),
                MM::ErrorCode::PARENT_OBJECT_NOT_CONTAIN_SPECIFIC_CHILD_OBJECT);
      EXPECT_EQ(handler3.GetResult().IsValid(), false);
      handler3 = set_manager.AddObject(20).Exception();
      EXPECT_EQ(handler3.Success(), true);
      EXPECT_EQ(handler3.GetResult().GetObject(), 20);
      EXPECT_EQ(set_manager.GetSize(), 2);

      std::vector<MM::Manager::ManagedObjectSet<int>::HandlerType> handlers{50};
      for (int i = 0; i < handlers.size(); ++i) {
        int temp = i;
        auto handler = set_manager.AddObject(std::move(temp)).Exception();
        EXPECT_EQ(handler.Success(), true);
        handlers[i] = std::move(handler.GetResult());
      }

      for (int i = 0; i < handlers.size(); ++i) {
        EXPECT_EQ(handlers[i].GetObject(), i);
      }

      EXPECT_EQ(set_manager.GetSize(), 50);
      EXPECT_EQ(handler1.GetResult().GetUseCount(), 3);
      EXPECT_EQ(set_manager.GetUseCount(handler1.GetResult().GetObject()), 3);
      EXPECT_EQ(handler2.GetResult().GetUseCount(), 3);
      EXPECT_EQ(set_manager.GetUseCount(handler2.GetResult().GetObject()), 3);
      EXPECT_EQ(handler3.GetResult().GetUseCount(), 2);
      EXPECT_EQ(set_manager.GetUseCount(handler3.GetResult().GetObject()), 2);

      auto handler = set_manager.GetObject(49).Exception();

      handler3 = handler1;

      EXPECT_EQ(handler3.GetResult().GetUseCount(), 4);
      EXPECT_EQ(handler1.GetResult().GetUseCount(), 4);
      handler3 = set_manager.AddObject(100).Exception();
      EXPECT_EQ(handler3.Success(), true);
      auto handler5 =
          set_manager.GetObject(handler1.GetResult().GetObject()).Exception();
      EXPECT_EQ(handler5.Success(), true);
      handler5 =
          set_manager.GetObject(handler3.GetResult().GetObject()).Exception();
      EXPECT_EQ(handler5.Success(), true);
    }

    EXPECT_EQ(set_manager.GetSize(), 1);

    set_manager2 = std::move(set_manager);

    EXPECT_EQ(set_manager.GetSize(), 0);
    EXPECT_EQ(set_manager2.GetSize(), 1);
  }

  EXPECT_EQ(set_manager2.GetSize(), 0);
}

#define COUNT_SIZE 100000
#define STEP_SIZE 25000

void InsertObject(
    MM::Manager::ManagedObjectSet<int>& set_manager, int start,
    std::vector<MM::Manager::ManagedObjectSet<int>::HandlerType>& handlers) {
  int end = start + COUNT_SIZE;
  for (; start != end; ++start) {
    int temp = start;
    auto handler = set_manager.AddObject(std::move(temp)).Exception();
    EXPECT_EQ(handler.Success(), true);
    handlers.emplace_back(std::move(handler.GetResult()));
  }
}

TEST(manager, set_thread) {
  MM::Manager::ManagedObjectSet<int> set_manager;
  std::vector<std::thread> threads;
  std::vector<std::vector<MM::Manager::ManagedObjectSet<int>::HandlerType>>
      handlers_vector{20};
  int step = STEP_SIZE;
  assert(COUNT_SIZE % step == 0);
  for (int i = 0; i != handlers_vector.size(); ++i) {
    std::thread t(InsertObject, std::ref(set_manager), i * step,
                  std::ref(handlers_vector[i]));
    threads.push_back(std::move(t));
  }

  for (int i = 0; i != handlers_vector.size(); ++i) {
    threads[i].join();
  }

  for (const auto& handlers : handlers_vector) {
    for (const auto& handler : handlers) {
      EXPECT_EQ(handler.IsValid(), true);
    }
  }

  int manx_index = (handlers_vector.size() - 1) * step + COUNT_SIZE;
  for (int i = 0; i != manx_index; ++i) {
    if (i < step) {
      EXPECT_EQ(set_manager.GetUseCount(i), 1);
      continue;
    }

    if (i < 2 * step) {
      EXPECT_EQ(set_manager.GetUseCount(i), 2);
      continue;
    }

    if (i < 3 * step) {
      EXPECT_EQ(set_manager.GetUseCount(i), 3);
      continue;
    }

    if (i >= manx_index - 1 * step) {
      EXPECT_EQ(set_manager.GetUseCount(i), 1);
      continue;
    }

    if (i >= manx_index - 2 * step) {
      EXPECT_EQ(set_manager.GetUseCount(i), 2);
      continue;
    }

    if (i >= manx_index - 3 * step) {
      EXPECT_EQ(set_manager.GetUseCount(i), 3);
      continue;
    }

    EXPECT_EQ(set_manager.GetUseCount(i), 4);
  }

  threads.clear();

  int size = handlers_vector.size();

  MM::Manager::ManagedObjectSet<int> set_manager2;

  for (int i = 0; i != size; ++i) {
    std::thread t(
        [&handlers_vector, &set_manager, &set_manager2](int i) {
          if (i == 10) {
            set_manager2 = std::move(set_manager);
            EXPECT_EQ(set_manager.GetSize(), 0);
            EXPECT_NE(set_manager2.GetSize(), 0);
          }
          handlers_vector[i].clear();
        },
        i);
    threads.push_back(std::move(t));
  }

  for (int i = 0; i != size; ++i) {
    threads[i].join();
  }

  EXPECT_EQ(set_manager2.GetSize(), 0);
  EXPECT_EQ(set_manager.GetSize(), 0);

  set_manager = std::move(set_manager2);

  threads.clear();
  for (int i = 0; i != 5; ++i) {
    std::thread t(InsertObject, std::ref(set_manager), i * COUNT_SIZE,
                  std::ref(handlers_vector[i]));
    threads.push_back(std::move(t));
  }
  for (int i = 0; i != 5; ++i) {
    threads[i].join();
  }
  threads.clear();
  for (int i = 5; i != 10; ++i) {
    std::thread t([&handlers_vector](int i) { handlers_vector[i - 5].clear(); },
                  i);
    threads.push_back(std::move(t));
  }
  for (int i = 5; i != 10; ++i) {
    std::thread t(InsertObject, std::ref(set_manager), i * COUNT_SIZE,
                  std::ref(handlers_vector[i]));
    threads.push_back(std::move(t));
  }
  for (auto& t : threads) {
    t.join();
  }

  EXPECT_EQ(set_manager.GetSize(), 5 * COUNT_SIZE);
}

TEST(manager, multi_set) {
  MM::Manager::ManagedObjectMultiSet<int> multi_set_manager;

  EXPECT_EQ(multi_set_manager.GetSize(), 0);
  EXPECT_EQ(multi_set_manager.IsMultiContainer(), true);
  EXPECT_EQ(multi_set_manager.IsRelationshipContainer(), false);
  auto handler1 = multi_set_manager.AddObject(1).Exception();
  EXPECT_EQ(handler1.Success(), true);
  auto handler2 = multi_set_manager.AddObject(1).Exception();
  EXPECT_EQ(handler2.Success(), true);
  auto handler3 = multi_set_manager.AddObject(1).Exception();
  EXPECT_EQ(handler3.Success(), true);
  auto handler4 = handler3;
  auto handler5 = handler3;
  EXPECT_EQ(handler3.GetResult().GetUseCount(), 3);
  EXPECT_EQ(
      multi_set_manager.GetUseCount(handler4.GetResult().GetObject(),
                                    handler4.GetResult().GetUseCountPtr()),
      3);
  ASSERT_EQ(multi_set_manager.GetSize(), 3);
  handler1.GetResult().Release();
  EXPECT_EQ(multi_set_manager.GetSize(), 2);
  handler2.GetResult().Release();
  EXPECT_EQ(multi_set_manager.GetSize(), 1);
  EXPECT_EQ(multi_set_manager.GetUseCount(handler3.GetResult().GetObject()), 3);
}

void InsertObject2(
    MM::Manager::ManagedObjectMultiSet<int>& multi_set_manager,
    std::vector<MM::Manager::ManagedObjectMultiSet<int>::HandlerType>&
        handlers) {
  for (int i = 0; i < COUNT_SIZE; ++i) {
    int temp = i;
    auto handler = multi_set_manager.AddObject(std::move(temp)).Exception();
    EXPECT_EQ(handler.Success(), true);
    handlers.emplace_back(std::move(handler.GetResult()));
  }
}

TEST(manager, multi_set_thread) {
  MM::Manager::ManagedObjectMultiSet<int> multi_set_manager, multi_set_manager2;
  int size = 20;
  std::vector<std::vector<MM::Manager::ManagedObjectMultiSet<int>::HandlerType>>
      handlers_vector{static_cast<std::uint32_t>(size)};
  std::vector<std::thread> threads;

  for (int i = 0; i != size; ++i) {
    std::thread t(InsertObject2, std::ref(multi_set_manager),
                  std::ref(handlers_vector[i]));
    threads.emplace_back(std::move(t));
  }

  for (int i = 0; i != size; ++i) {
    threads[i].join();
  }

  EXPECT_EQ(multi_set_manager.GetSize(), size * COUNT_SIZE);

  threads.clear();
  for (int i = 0; i != size; ++i) {
    std::thread t(
        [&handlers_vector, &multi_set_manager, &multi_set_manager2,
         size](int i) {
          if (i == size / 2) {
            multi_set_manager2 = std::move(multi_set_manager);
          }
          handlers_vector[i].clear();
        },
        i);
    threads.emplace_back(std::move(t));
  }

  for (int i = 0; i != size; ++i) {
    threads[i].join();
  }

  EXPECT_EQ(multi_set_manager2.GetSize(), 0);
  EXPECT_EQ(multi_set_manager.GetSize(), 0);
}