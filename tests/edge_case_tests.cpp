#include <gtest/gtest.h>
#include "unrolled_list.h"

TEST(EdgeCaseTests, EmptyListOperations) {
    unrolled_list<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);
}

TEST(EdgeCaseTests, MaxSizeNodeInsertion) {
    unrolled_list<int, 3> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    EXPECT_EQ(list.size(), 4);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 4);
}

TEST(EdgeCaseTests, LargeInsertion) {
    unrolled_list<int> list;
    for (int i = 0; i < 1000; ++i) {
        list.push_back(i);
    }
    EXPECT_EQ(list.size(), 1000);
    EXPECT_EQ(list.front(), 0);
    EXPECT_EQ(list.back(), 999);
}