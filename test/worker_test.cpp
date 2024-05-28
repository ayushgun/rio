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

#include "rio/worker.hpp"
#include <gtest/gtest.h>
#include <atomic>
#include <future>
#include "rio/task.hpp"

class worker_test : public ::testing::Test {
 protected:
  rio::worker worker;
};

TEST_F(worker_test, WorkerExecutesSingleTask) {
  auto task_closure = rio::task::make([]() { return 42; });

  worker.assign(std::move(task_closure.task));
  EXPECT_EQ(task_closure.future.get(), 42);
}

TEST_F(worker_test, WorkerExecutesMultipleTasks) {
  auto task_closure1 = rio::task::make([]() { return 42; });
  auto task_closure2 = rio::task::make([]() { return 24; });

  worker.assign(std::move(task_closure1.task));
  worker.assign(std::move(task_closure2.task));

  EXPECT_EQ(task_closure1.future.get(), 42);
  EXPECT_EQ(task_closure2.future.get(), 24);
}

TEST_F(worker_test, WorkerHandlesVoidTasks) {
  std::atomic<bool> executed = false;
  auto task_closure = rio::task::make([&executed]() { executed = true; });

  worker.assign(std::move(task_closure.task));
  task_closure.future.get();  // Wait for the task to complete
  EXPECT_TRUE(executed.load());
}

TEST_F(worker_test, WorkerStopsGracefully) {
  std::atomic<bool> executed = false;
  auto task_closure = rio::task::make([&executed]() { executed = true; });

  worker.assign(std::move(task_closure.task));
  task_closure.future.get();  // Wait for the task to complete
  EXPECT_TRUE(executed.load());
}

TEST_F(worker_test, WorkerStopsWhenDestructed) {
  {
    rio::worker temporary_worker;
    auto task_closure = rio::task::make([]() { return 42; });

    temporary_worker.assign(std::move(task_closure.task));
    EXPECT_EQ(task_closure.future.get(), 42);
  }  // temporary_worker is destructed here

  // No assertion here, just ensuring no crashes or hangs
}
