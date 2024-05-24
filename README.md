# Rio ![License Badge](https://img.shields.io/badge/license-MIT-blue?link=https%3A%2F%2Fgithub.com%2Fayushgun%2Faqua%2Fblob%2Fmain%2FLICENSE) ![PR Badge](https://img.shields.io/badge/PRs-welcome-red)

A runtime for writing concurrent, asynchronous, and slim applications with C++20.

# Key Features

- **Performant:** Rio's near zero-cost abstractions enable bare-metal performance
- **Flexible:** Support for multiple task types and scheduling policies
- **Scalable:** Rio tightly manages resources and minimizes busy spinning

# Getting Started

```cpp
#include "rio/executor.hpp"

int main() {
  // Initialize the runtime by creating an `executor` object.
  rio::executor executor;
  auto& scheduler = executor.get_scheduler();

  // Submit a task which produces an integer to the `executor`.
  std::future<int> future = scheduler.await([](int n) { return n + 1; }, 10);
  std::cout << future.get() << '\n';
  //                  ^^^
  //                  Retrieve the result via the generated `std::future`
}
```

# Further Usage

```cpp
#include <iostream>
#include "rio/executor.hpp"
#include "rio/scheduler.hpp"

// Define a custom scheduling policy and task scheduler by implementing the
// `rio::scheduler` interface.
class custom_scheduler : public rio::scheduler {
  // ...
};

int main() {
  rio::executor<8, custom_scheduler> executor;
  //            ^^^^^^^^^^^^^^^^^^^
  //            Use a custom pool size and scheduler implementation.
  auto& scheduler = executor.get_scheduler();

  // Submit a void task to the `executor`. Since execution is guaranteed to
  // occur strictly once, `std::cout` synchronization is not required in this
  // trivial case.
  scheduler.await([]() { std::cout << "Hello World\n"; });
}
```
