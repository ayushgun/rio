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
#include <cstddef>
#include <future>
#include <optional>
#include <utility>
#include "rio/worker.hpp"

namespace rio {
/// Represents a task scheduled for execution with the assigned worker.
struct scheduled_task {
  rio::task&& task;    // Task to be executed.
  rio::worker_id wid;  // Worker ID for task execution.
};

/// Interface for task schedulers. Defines generic scheduling operations.
class scheduler {
 protected:
  /// Schedules a task for execution.
  virtual void schedule(rio::task&&) = 0;

 public:
  /// Retrieves the next scheduled task if available.
  virtual std::optional<rio::scheduled_task> next() = 0;

  /// Submits a task for execution and returns a future containing the task's
  /// return value or exception.
  template <
      typename F,
      typename... A,
      typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
  std::future<R> await(F&& function, A&&... arguments) {
    auto promise = new std::promise<R>;
    std::future<R> future = promise->get_future();

    // Wrap the function into a task that handles result propagation and
    // exception management.
    auto task = [promise = std::move(promise),
                 function = std::forward<F>(function),
                 ... arguments = std::forward<A>(arguments)]() mutable {
      try {
        if constexpr (std::is_same_v<R, void>) {
          std::invoke(std::forward<F>(function), std::forward<A>(arguments)...);
          promise->set_value();
        } else {
          // Handle function pointers directly without std::forward
          if constexpr (std::is_pointer_v<std::decay_t<F>>) {
            promise->set_value(
                std::invoke(function, std::forward<A>(arguments)...));
          } else {
            promise->set_value(std::invoke(std::forward<F>(function),
                                           std::forward<A>(arguments)...));
          }
        }
      } catch (...) {
        promise->set_exception(std::current_exception());
      }

      delete promise;
    };

    schedule(std::move(task));
    return future;
  }
};

/// Default FCFS task scheduler.
class fcfs_scheduler : public rio::scheduler {
 private:
  folly::ProducerConsumerQueue<rio::task> tasks;
  rio::worker_id prev_wid;
  rio::worker_id max_wid;

 public:
  /// Constructs a FCFS scheduler for a specified number of workers.
  explicit fcfs_scheduler(std::size_t n);

  /// Schedules a task using a FCFS approach.
  void schedule(rio::task&& task) override;

  /// Retrieves and prepares the next task for execution, determining the
  /// appropriate worker ID.
  std::optional<rio::scheduled_task> next() override;
};
}  // namespace rio
