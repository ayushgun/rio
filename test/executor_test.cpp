// MIT License
// Copyright (c) 2024 Ayush Gundawar <ayushgundawar (at) gmail (dot) com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

#include "rio/executor.hpp"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include "rio/scheduler.hpp"

class executor_test : public ::testing::Test {
 protected:
  rio::executor<4, rio::fcfs_scheduler> executor;
};

TEST_F(executor_test, ExecutorCanSubmitAndExecuteSingleTask) {
  auto future = executor.get_scheduler().await([]() { return 42; });
  EXPECT_EQ(future.get(), 42);
}

TEST_F(executor_test, ExecutorCanSubmitAndExecuteMultipleTasks) {
  auto future1 = executor.get_scheduler().await([]() { return 42; });
  auto future2 = executor.get_scheduler().await([]() { return 24; });

  EXPECT_EQ(future1.get(), 42);
  EXPECT_EQ(future2.get(), 24);
}

TEST_F(executor_test, ExecutorHandlesVoidTasks) {
  std::atomic<bool> executed = false;
  auto future =
      executor.get_scheduler().await([&executed]() { executed = true; });

  future.get();  // Wait for the task to complete
  EXPECT_TRUE(executed.load());
}

TEST_F(executor_test, ExecutorDistributesTasksToWorkers) {
  std::array<std::atomic<bool>, 3> executed = {false, false, false};
  auto future1 =
      executor.get_scheduler().await([&executed]() { executed[0] = true; });
  auto future2 =
      executor.get_scheduler().await([&executed]() { executed[1] = true; });
  auto future3 =
      executor.get_scheduler().await([&executed]() { executed[2] = true; });

  future1.get();
  future2.get();
  future3.get();

  EXPECT_TRUE(executed[0].load());
  EXPECT_TRUE(executed[1].load());
  EXPECT_TRUE(executed[2].load());
}

TEST_F(executor_test, ExecutorStopsGracefully) {
  auto future = executor.get_scheduler().await([]() { return 42; });
  EXPECT_EQ(future.get(), 42);

  // Allow some time for the executor to potentially process the shutdown
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Ensure no crashes or exceptions on shutdown
}

TEST_F(executor_test, ExecutorHandlesBlockingTasks) {
  auto future = executor.get_scheduler().await([]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 42;
  });

  EXPECT_EQ(future.get(), 42);
}
