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

#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <thread>
#include "folly/ProducerConsumerQueue.h"

namespace rio {
/// Represents a unique identifier for a worker.
using worker_id = std::size_t;

/// Invokable work wrapped with result propogation logic.
using task = std::function<void()>;

/// Represents a worker thread that executes tasks from a queue.
class worker {
 private:
  folly::ProducerConsumerQueue<rio::task> tasks;
  std::atomic_flag stop;
  std::thread thread;

  /// Processes all work in the task queue.
  void process_work();

 public:
  /// Creates and initializes a worker thread with work processing logic.
  explicit worker();

  worker(const worker&) = delete;
  void operator=(const worker&) = delete;

  /// Stops work processing logic, signals end of work processing to the master
  /// thread, and joins the thread.
  ~worker();

  /// Adds a task to the worker's task queue.
  template <typename T>
  void assign(T&& task) {
    while (!tasks.write(std::forward<T>(task))) {
      std::this_thread::yield();
    }
  }
};
}  // namespace rio
