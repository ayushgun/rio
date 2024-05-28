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

namespace rio {
/// Hardware concurrency setting defined at compile-time.
/// This value is typically set by the build system (e.g., CMake) because
/// std::thread::hardware_concurrency is a runtime variable that reflects the
/// actual number of concurrent threads the system supports.
constexpr std::size_t hardware_concurrency = 8;
}  // namespace rio
