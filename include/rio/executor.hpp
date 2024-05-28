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

#include <cstddef>
#include "rio/core_count.hpp"
#include "rio/scheduler.hpp"
#include "rio/worker.hpp"

namespace rio {
/// Manages the execution of tasks using a specific scheduler.
template <std::size_t N = rio::hardware_concurrency,
          executor_scheduler S = rio::fcfs_scheduler>
  requires(N >= 2)
class executor {
 private:
  S scheduler;
  std::array<rio::worker, N - 1> workers;
  std::atomic_flag stop;
  std::thread master;

  /// Continuously retrieves and assigns scheduled tasks to workers.
  void distribute_work() {
    while (!stop.test() || scheduler.has_tasks()) {
      std::optional<rio::scheduled_task> next_task = scheduler.next();

      if (next_task.has_value()) {
        workers[next_task->wid].assign(std::move(next_task->task));
      } else {
        std::this_thread::yield();
      }
    }
  }

 public:
  /// Creates and initializes a master thread with work distribution logic.
  /// Additionally, creates N - 1 worker threads.
  explicit executor()
      : scheduler(N - 1), master([&]() { distribute_work(); }) {}

  executor(const executor&) = delete;
  void operator=(const executor&) = delete;

  /// Stops work processing logic and joins the master thread and all worker
  /// threads.
  ~executor() {
    stop.test_and_set();

    if (master.joinable()) {
      master.join();
    }
  }

  /// Exposes a mutable reference to the task scheduler.
  constexpr S& get_scheduler() { return scheduler; }
};
}  // namespace rio
