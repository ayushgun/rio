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
#include <thread>
#include "rio/thread_count.hpp"

rio::fcfs_scheduler::fcfs_scheduler(std::size_t num_workers)
    : tasks(rio::hardware_concurrency),
      ready(0),
      prev_wid(0),
      max_wid(num_workers) {}

auto rio::fcfs_scheduler::schedule(rio::task&& task) -> void {
  while (!tasks.write(std::move(task))) {
    std::this_thread::yield();
  }

  ready.release();  // Signal that tasks are ready to be scheduled
}

auto rio::fcfs_scheduler::has_tasks() const -> bool {
  return !tasks.isEmpty();
}

auto rio::fcfs_scheduler::next() -> rio::scheduled_task {
  ready.acquire();  // Wait until tasks are ready to be scheduled

  // Claim the next task from task queue and, since task queue size is bounded,
  // immediately pop before distribution to avoid starving producer.
  auto task = std::move(*tasks.frontPtr());
  tasks.popFront();

  rio::worker_id wid = prev_wid++ % max_wid;
  rio::scheduled_task next_task = {std::move(task), wid};

  return next_task;
}
