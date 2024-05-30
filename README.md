# Rio ![License Badge](https://img.shields.io/badge/license-MIT-blue?link=https%3A%2F%2Fgithub.com%2Fayushgun%2Faqua%2Fblob%2Fmain%2FLICENSE) ![PR Badge](https://img.shields.io/badge/PRs-welcome-red)

A runtime for writing slim, concurrent, and asynchronous applications with C++20.

# Key Features

- **Performant:** Rio's near zero-cost abstractions enable bare-metal performance.
- **Flexible:** Supports multiple task types and scheduling policies.
- **Scalable:** Rio tightly manages resources and minimizes busy spinning.

# Getting Started

## Task Submission

```cpp
#include "rio/executor.hpp"

int make_http_request(std::string_view body) {
  // ...
}

int main() {
  // Initialize the runtime by creating an executor object.
  rio::executor executor;
  auto& scheduler = executor.get_scheduler();

  // a. Submit an asynchronous task which produces an integer to the executor.
  std::future<int> f1 = scheduler.await([](int n) { return 2 * n; }, 10);
  std::cout << f1.get() << '\n';
  //              ^^^
  //              Retrieve the result via the generated std::future.

  // b. Submit an asynchronous void task to the executor.
  scheduler.await([](int n) { std::cout << n << '\n'; }, 10);

  // c. Submit a task via a function pointer to the executor.
  std::future<int> f2 = scheduler.await(make_http_request, "...");
  std::cout << f2.get() << '\n';
  //              ^^^
  //              Similarly retrieve the result via the generated std::future.
}
```

## Custom Schedulers & Pool Sizes

```cpp
#include "rio/executor.hpp"
#include "rio/scheduler.hpp"

// Define a custom scheduling policy and task scheduler by implementing the
// rio::scheduler interface.
class custom_scheduler : public rio::scheduler {
  // ...
};

int main() {
  rio::executor<8, custom_scheduler> executor;
  //            ^^^^^^^^^^^^^^^^^^^
  //            Use a custom pool size and scheduler implementation.
  auto& scheduler = executor.get_scheduler();

  // Submit a void task to the executor. Since execution is guaranteed to
  // occur strictly once, std::cout synchronization is not required in this
  // trivial case.
  scheduler.await([]() { std::cout << "Hello World\n"; });
}
```

## Example: Reading Files

```cpp
#include "rio/executor.hpp"

void append_to_file(std::filesystem::path path, std::string_view content) {
  // ...
}

int main() {
  // Initialize the runtime by creating an executor object.
  rio::executor executor;
  auto& scheduler = executor.get_scheduler();

  // Get the file system path for all files to append to.
  std::vector<std::filesystem::path> paths = get_file_paths();

  // Asynchronously append content to all files in the paths vector. Optionally,
  // store all generated std::future objects to check for thrown exceptions.
  for (auto& path : paths) {
    scheduler.await(append_to_file, path, "👋");
  }
}
```
