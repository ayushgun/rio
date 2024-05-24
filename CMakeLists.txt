# Project declaration
cmake_minimum_required(VERSION 3.19)
project(rio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Installation prefix based on architecture
set(HOMEBREW_PREFIX "/usr/local")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(HOMEBREW_PREFIX "/opt/homebrew")
endif()

# Find Folly libraries and include directories
find_path(FOLLY_INCLUDE_DIR folly/Optional.h HINTS ${HOMEBREW_PREFIX}/include)
find_library(FOLLY_LIBRARY folly HINTS ${HOMEBREW_PREFIX}/lib)

# Custom command and target to generate core_count.hpp
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/include/rio/core_count.hpp
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/core_count.sh
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts/core_count.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating core_count.hpp"
    VERBATIM)

add_custom_target(GenerateCoreCountHeader ALL DEPENDS ${CMAKE_SOURCE_DIR}/include/rio/core_count.hpp)

# Define the library, link dependencies, and include directories
add_library(rio STATIC src/worker.cpp src/scheduler.cpp)
target_include_directories(rio PUBLIC include ${FOLLY_INCLUDE_DIR})
target_link_libraries(rio PRIVATE ${FOLLY_LIBRARY})
add_dependencies(rio GenerateCoreCountHeader)

# Compiler and linker flags for debug builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(rio PRIVATE -Wall -Werror -Wextra -Wpedantic)
    target_compile_options(rio PRIVATE -fsanitize=thread)
    target_link_options(rio PRIVATE -fsanitize=thread)
endif()
