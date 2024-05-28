// // MIT License
// // Copyright (c) 2024 Ayush Gundawar <ayushgundawar (at) gmail (dot) com>
// //
// // Permission is hereby granted, free of charge, to any person obtaining a
// copy
// // of this software and associated documentation files (the "Software"), to
// deal
// // in the Software without restriction, including without limitation the
// rights
// // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// // copies of the Software, and to permit persons to whom the Software is
// // furnished to do so, subject to the following conditions:
// //
// // The above copyright notice and this permission notice shall be included in
// // all copies or substantial portions of the Software.

// #include "rio/scheduler.hpp"
// #include <gtest/gtest.h>

// TEST(fcfs_scheduler_test, in_order_scheduling) {
//   rio::fcfs_scheduler scheduler(4);  // Create a scheduler for 4 workers
//   rio::task task_1 = []() {};
//   rio::task task_2 = []() {};

//   scheduler.await(std::move(task_1));
//   scheduler.await(std::move(task_2));

//   auto scheduled_task_1 = scheduler.next();
//   auto scheduled_task_2 = scheduler.next();

//   // Verify that tasks are retrieved in the order they were scheduled
//   ASSERT_TRUE(scheduled_task_1.has_value());
//   ASSERT_TRUE(scheduled_task_2.has_value());
//   EXPECT_NE(
//       scheduled_task_1->wid,
//       scheduled_task_2
//           ->wid);  // Ensure different workers are used in round-robin
//           fashion
// }

// TEST(fcfs_scheduler_test, nullopt_when_empty) {
//   rio::fcfs_scheduler scheduler(4);

//   auto result = scheduler.next();
//   ASSERT_FALSE(result.has_value());
// }

// TEST(fcfs_scheduler_test, same_wid_for_single_worker) {
//   rio::fcfs_scheduler scheduler(1);  // Single worker for simplicity

//   // Define multiple tasks, but since we're only interested in wid, we don't
//   // need them to do anything
//   rio::task task_1 = []() {};
//   rio::task task_2 = []() {};
//   rio::task task_3 = []() {};

//   // Queue the tasks
//   scheduler.await(std::move(task_1));
//   scheduler.await(std::move(task_2));
//   scheduler.await(std::move(task_3));

//   // Retrieve and execute tasks, while checking for consistent worker IDs
//   std::optional<rio::scheduled_task> scheduled_task_1 = scheduler.next();
//   std::optional<rio::scheduled_task> scheduled_task_2 = scheduler.next();
//   std::optional<rio::scheduled_task> scheduled_task_3 = scheduler.next();

//   ASSERT_TRUE(scheduled_task_1.has_value());
//   ASSERT_TRUE(scheduled_task_2.has_value());
//   ASSERT_TRUE(scheduled_task_3.has_value());

//   // Check if all tasks have the same worker id
//   EXPECT_EQ(scheduled_task_1->wid, scheduled_task_2->wid);
//   EXPECT_EQ(scheduled_task_2->wid, scheduled_task_3->wid);
// }
