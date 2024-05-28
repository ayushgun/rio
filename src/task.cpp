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
#include <utility>

rio::task::task(std::function<void()> propagator)
    : propagator(propagator), has_executed(false) {}

rio::task::task(task&& other)
    : propagator(std::move(other.propagator)),
      has_executed(other.has_executed.load()) {}

rio::task& rio::task::operator=(task&& other) {
  propagator = std::move(other.propagator);
  has_executed.store(other.has_executed.load());
  return *this;
}

void rio::task::operator()() {
  if (!has_executed.exchange(true)) {
    propagator();
  }
}

bool rio::task::is_executed() const {
  return has_executed.load();
}
