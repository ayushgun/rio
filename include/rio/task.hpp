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
#include <functional>
#include <future>

namespace rio {

/// Forward declaration of the task closure struct to be used in task::make().
template <typename R>
struct task_closure;

/// Manages the execution of a single callable function.
class task {
 private:
  std::function<void()> propagator;
  std::atomic<bool> executed;

 private:
  /// Receives a propagator function built by task::make() and stores it as a
  /// member for later execution.
  task(std::function<void()>);

 public:
  task(const task&) = delete;
  auto operator=(const task&) -> task& = delete;

  /// Move constructor that transfers ownership of the task. Primarily used when
  /// transferring the task into a task closure.
  task(task&&) noexcept;

  /// Move assignment operator that transfers ownership of the task. Primarily
  /// used when transferring the task into a task closure.
  auto operator=(task&&) noexcept -> task&;

  /// Constructs a task from a callable and its arguments, and returns a
  /// task closure containing the future result and the task. Allows for
  /// asynchronous execution and result retrieval through std::future.
  template <
      typename F,
      typename... A,
      typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
  static auto make(F&& function, A&&... arguments) -> rio::task_closure<R> {
    auto promise = new std::promise<R>();
    std::future<R> future = promise->get_future();

    auto propagater = [promise, function = std::forward<F>(function),
                       ... arguments = std::forward<A>(arguments)]() mutable {
      try {
        // Invoke the callable and handle the return type appropriately
        if constexpr (std::is_same_v<R, void>) {
          std::invoke(function, arguments...);
          promise->set_value();
        } else {
          promise->set_value(std::invoke(function, arguments...));
        }
      } catch (...) {
        promise->set_exception(std::current_exception());
      }

      delete promise;
    };

    return {std::move(future), std::move(task(std::move(propagater)))};
  }

  /// Executes the encapsulated callable if it has not been executed already.
  auto operator()() -> void;

  /// Returns whether or not the callable can be executed now.
  auto is_executable() const -> bool;
};

/// Represents a task and its associated future. The future holds the result
/// of the task, which may be obtained asynchronously.
template <typename R>
struct task_closure {
  std::future<R> future;  // Future to hold the result after task execution.
  rio::task task;         // Single-callable task which fills the future.
};

}  // namespace rio
