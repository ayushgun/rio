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
#include <future>

class executor_test : public ::testing::Test {
 protected:
  rio::executor<> executor;  // Instantiate the default executor for each test
};

TEST_F(executor_test, future_return_value) {
  auto& scheduler = executor.get_scheduler();
  std::future<int> future = scheduler.await([](int n) { return n + 1; }, 10);
  ASSERT_EQ(future.get(), 11);
}

TEST_F(executor_test, void_future) {
  auto& scheduler = executor.get_scheduler();
  int value = 10;
  std::future<void> future = scheduler.await([&value]() { value *= 2; });
  future.get();
  ASSERT_EQ(value, 20);
}

TEST_F(executor_test, exception_handling) {
  auto& scheduler = executor.get_scheduler();
  std::future<int> future =
      scheduler.await([]() -> int { throw std::runtime_error("Test Error"); });

  try {
    int result = future.get();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    ASSERT_STREQ("Test Error", e.what());
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}
