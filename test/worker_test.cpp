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

class worker_test : public ::testing::Test {};

TEST_F(worker_test, task_execution) {
  rio::worker test_worker;
  std::atomic<bool> task_completed{false};

  test_worker.assign([&]() { task_completed.store(true); });

  // Allow some time for the worker to pick up and execute the task
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(task_completed.load());
}

TEST_F(worker_test, multiple_tasks) {
  rio::worker test_worker;
  std::atomic<int> counter{0};

  for (int i = 0; i < 10; ++i) {
    test_worker.assign([&]() { counter.fetch_add(1); });
  }

  // Allow some time for the worker to pick up and execute all tasks
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  EXPECT_EQ(counter.load(), 10);
}
