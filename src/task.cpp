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
#include <functional>
#include <stdexcept>
#include <utility>

rio::task::task(std::function<void()> propagator)
    : propagator(propagator), executed(false) {}

rio::task::task(task&& other) noexcept
    : propagator(std::exchange(other.propagator, nullptr)),
      executed(other.executed.load()) {
  other.executed.store(true);
}

auto rio::task::operator=(task&& other) noexcept -> rio::task& {
  propagator = std::exchange(other.propagator, nullptr);
  executed.store(other.executed.load());
  other.executed.store(true);
  return *this;
}

auto rio::task::operator()() -> void {
  if (propagator && !executed.exchange(true)) {
    propagator();
  } else {
    throw std::logic_error("rio::task object should only be executed once");
  }
}

auto rio::task::is_executable() const -> bool {
  return !executed.load();
}
