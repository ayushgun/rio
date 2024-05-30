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
#include <thread>
#include "rio/core_count.hpp"

void rio::worker::process_work() {
  while (!stop.load()) {
    ready.acquire();  // Wait until tasks are ready to be executed

    while (!tasks.isEmpty()) {
      // Claim the next task from task queue and, since task queue size is
      // bounded, immediately pop before invocation to avoid starving
      // producer.
      auto task = std::move(*tasks.frontPtr());
      tasks.popFront();
      std::invoke(std::move(task));
    }
  }
}

rio::worker::worker()
    : tasks(rio::hardware_concurrency),
      ready(0),
      stop(false),
      thread([&]() { process_work(); }) {}

rio::worker::~worker() {
  stop.store(true);
  ready.release();

  if (thread.joinable()) {
    thread.join();
  }
}
