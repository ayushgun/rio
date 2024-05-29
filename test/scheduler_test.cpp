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

#include "rio/scheduler.hpp"
#include <gtest/gtest.h>
#include <future>
#include <thread>
#include "rio/task.hpp"

class fcfs_scheduler_test : public ::testing::Test {
 protected:
  rio::fcfs_scheduler scheduler;

  fcfs_scheduler_test() : scheduler(4) {}
};

TEST_F(fcfs_scheduler_test, SchedulerCanScheduleAndRetrieveTask) {
  auto future = scheduler.spawn([]() { return 42; });
  EXPECT_TRUE(scheduler.has_tasks());

  auto scheduled_task = scheduler.next();
  scheduled_task.task();
  EXPECT_EQ(future.get(), 42);
}

TEST_F(fcfs_scheduler_test, SchedulerHandlesMultipleTasks) {
  auto future1 = scheduler.spawn([]() { return 42; });
  auto future2 = scheduler.spawn([]() { return 24; });
  EXPECT_TRUE(scheduler.has_tasks());

  auto scheduled_task1 = scheduler.next();
  scheduled_task1.task();
  EXPECT_EQ(future1.get(), 42);

  auto scheduled_task2 = scheduler.next();
  scheduled_task2.task();
  EXPECT_EQ(future2.get(), 24);
}

TEST_F(fcfs_scheduler_test, SchedulerAssignsTasksToWorkerInFCFS) {
  auto future1 = scheduler.spawn([]() { return 42; });
  auto future2 = scheduler.spawn([]() { return 24; });
  EXPECT_TRUE(scheduler.has_tasks());

  auto scheduled_task1 = scheduler.next();
  EXPECT_EQ(scheduled_task1.wid, 0);
  scheduled_task1.task();
  EXPECT_EQ(future1.get(), 42);

  auto scheduled_task2 = scheduler.next();
  EXPECT_EQ(scheduled_task2.wid, 1);
  scheduled_task2.task();
  EXPECT_EQ(future2.get(), 24);
}

TEST_F(fcfs_scheduler_test, SchedulerNextBlocksUntilTaskIsAvailable) {
  auto future = scheduler.spawn([]() { return 42; });

  std::thread t([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto scheduled_task = scheduler.next();
    scheduled_task.task();
  });

  t.join();
  EXPECT_EQ(future.get(), 42);
}

TEST_F(fcfs_scheduler_test, SpawnSchedulesTaskAndReturnsFuture) {
  auto future = scheduler.spawn([]() { return 42; });
  EXPECT_TRUE(scheduler.has_tasks());

  auto scheduled_task = scheduler.next();
  scheduled_task.task();
  EXPECT_EQ(future.get(), 42);
}
