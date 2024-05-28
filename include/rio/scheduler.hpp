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

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <future>
#include <semaphore>
#include <utility>
#include "rio/task.hpp"
#include "rio/worker.hpp"

namespace rio {
/// Represents a task scheduled for execution with the assigned worker.
struct scheduled_task {
  rio::task task;      // Task to be executed.
  rio::worker_id wid;  // Worker ID for task execution.
};

/// Base class for schedulers. Derived classes should provide a constructor
/// that can take necessary initialization parameters, such as the number of
/// worker threads.
class scheduler {
 protected:
  /// Schedules a task for execution.
  virtual void schedule(rio::task&&) = 0;

 public:
  virtual ~scheduler() = default;

  /// Returns true if there are tasks pending to be scheduled.
  virtual bool has_tasks() const = 0;

  /// Retrieves the next scheduled task if available.
  virtual rio::scheduled_task next() = 0;

  /// Submits a task for execution and returns a future containing the task's
  /// return value or exception.
  template <
      typename F,
      typename... A,
      typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
  std::future<R> await(F&& function, A&&... arguments) {
    auto [future, task] = rio::task::build(std::forward<F>(function),
                                           std::forward<A>(arguments)...);

    schedule(std::move(task));
    return std::move(future);
  }
};

/// Validates if a type S is derived from scheduler and constructible with a
/// std::size_t representing the number of worker threads.
template <typename S>
concept constructible_scheduler =
    std::derived_from<S, scheduler> && std::constructible_from<S, std::size_t>;

/// Default FCFS task scheduler.
class fcfs_scheduler : public rio::scheduler {
 private:
  folly::ProducerConsumerQueue<rio::task> tasks;
  std::binary_semaphore ready;
  rio::worker_id prev_wid;
  rio::worker_id max_wid;

 protected:
  /// Schedules a task using a FCFS approach.
  void schedule(rio::task&&) override;

 public:
  /// Constructs a FCFS scheduler for a specified number of workers.
  explicit fcfs_scheduler(std::size_t);

  /// Returns true if there are tasks in the scheduler's queue.
  bool has_tasks() const override;

  /// Retrieves and prepares the next task for execution, determining the
  /// appropriate worker ID. Blocks until a task is ready to be scheduled.
  rio::scheduled_task next() override;
};
}  // namespace rio
