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

#include "rio/task.hpp"
#include <gtest/gtest.h>
#include <future>

TEST(task_test, TaskExecutesFunctionCorrectly) {
  auto task_closure = rio::task::make([]() { return 42; });

  task_closure.task();
  EXPECT_EQ(task_closure.future.get(), 42);
}

TEST(task_test, TaskExecutesVoidFunctionCorrectly) {
  bool executed = false;
  auto task_closure = rio::task::make([&]() { executed = true; });

  task_closure.task();
  task_closure.future.get();  // Wait for the task to complete
  EXPECT_TRUE(executed);
}

TEST(task_test, TaskExecutesOnlyOnce) {
  int count = 0;
  auto task_closure = rio::task::make([&]() { ++count; });

  task_closure.task();
  EXPECT_THROW(task_closure.task(), std::logic_error);
  task_closure.future.get();
  EXPECT_EQ(count, 1);
}

TEST(task_test, TaskHandlesException) {
  auto task_closure =
      rio::task::make([]() { throw std::runtime_error("error"); });

  task_closure.task();
  EXPECT_THROW(task_closure.future.get(), std::runtime_error);
}

TEST(task_test, TaskMoveConstructorTransfersOwnership) {
  auto task_closure1 = rio::task::make([]() { return 42; });
  rio::task moved_task = std::move(task_closure1.task);

  moved_task();
  EXPECT_EQ(task_closure1.future.get(), 42);
}

TEST(task_test, TaskMoveAssignmentTransfersOwnership) {
  auto task_closure1 = rio::task::make([]() { return 42; });
  rio::task moved_task = std::move(task_closure1.task);
  auto task_closure2 = rio::task::make([]() { return 24; });

  task_closure2.task = std::move(moved_task);

  task_closure2.task();
  EXPECT_EQ(task_closure1.future.get(), 42);
}

TEST(task_test, TaskMoveAssignmentTransfersHasExecutedFlag) {
  auto task_closure1 = rio::task::make([]() { return 42; });
  rio::task moved_task = std::move(task_closure1.task);
  auto task_closure2 = rio::task::make([]() { return 24; });

  task_closure2.task = std::move(moved_task);
  task_closure2.task();

  // Check if the task has been executed once
  EXPECT_FALSE(task_closure2.task.is_executable());
  EXPECT_EQ(task_closure1.future.get(), 42);
}
