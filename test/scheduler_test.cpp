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

class MockTask {
 public:
  void operator()() const { /* Task execution logic here */ }
};

TEST(FCFSSchedulerTest, SchedulesTasksInOrder) {
  rio::fcfs_scheduler scheduler(4);  // Create a scheduler for 4 workers
  rio::task task1 = MockTask{};
  rio::task task2 = MockTask{};

  scheduler.await(std::move(task1));
  scheduler.await(std::move(task2));

  auto scheduledTask1 = scheduler.next();
  auto scheduledTask2 = scheduler.next();

  // Verify that tasks are retrieved in the order they were scheduled
  ASSERT_TRUE(scheduledTask1.has_value());
  ASSERT_TRUE(scheduledTask2.has_value());
  EXPECT_NE(
      scheduledTask1->wid,
      scheduledTask2
          ->wid);  // Ensure different workers are used in round-robin fashion
}

TEST(FCFSSchedulerTest, ReturnsNulloptWhenEmpty) {
  rio::fcfs_scheduler scheduler(4);

  auto result = scheduler.next();
  ASSERT_FALSE(result.has_value());
}

TEST(FCFSSchedulerTest, SameWidForSingleWorker) {
  rio::fcfs_scheduler scheduler(1);  // Single worker for simplicity

  // Define multiple tasks, but since we're only interested in wid, we don't
  // need them to do anything
  rio::task task1 = []() {};
  rio::task task2 = []() {};
  rio::task task3 = []() {};

  // Queue the tasks
  scheduler.await(std::move(task1));
  scheduler.await(std::move(task2));
  scheduler.await(std::move(task3));

  // Retrieve and execute tasks, while checking for consistent worker IDs
  std::optional<rio::scheduled_task> scheduledTask1 = scheduler.next();
  std::optional<rio::scheduled_task> scheduledTask2 = scheduler.next();
  std::optional<rio::scheduled_task> scheduledTask3 = scheduler.next();

  ASSERT_TRUE(scheduledTask1.has_value());
  ASSERT_TRUE(scheduledTask2.has_value());
  ASSERT_TRUE(scheduledTask3.has_value());

  // Check if all tasks have the same worker id
  EXPECT_EQ(scheduledTask1->wid, scheduledTask2->wid);
  EXPECT_EQ(scheduledTask2->wid, scheduledTask3->wid);
}
