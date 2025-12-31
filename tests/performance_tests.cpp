#include <gtest/gtest.h>
#include "unrolled_list.h"
#include <chrono>

TEST(PerformanceTests, LargePushBack) {
    unrolled_list<int> list;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; ++i) {
        list.push_front(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time for 100,000 push_front operations: " << elapsed.count() << " seconds\n";
    EXPECT_EQ(list.size(), 100000);
}

TEST(PerformanceTests, LargeInsert) {
    unrolled_list<int> list;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; ++i) {
        list.insert(list.begin(), i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time for 100,000 insert at begin operations: " << elapsed.count() << " seconds\n";
    EXPECT_EQ(list.size(), 200000);
}

TEST(PerformanceTests, LargeErase) {
    unrolled_list<int> list;
    for (int i = 0; i < 100000; ++i) {
        list.push_back(i);
    }
    auto start = std::chrono::high_resolution_clock::now();
    while (!list.empty()) {
        list.pop_back();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time for 100,000 pop_back operations: " << elapsed.count() << " seconds\n";
    EXPECT_TRUE(list.empty());
}