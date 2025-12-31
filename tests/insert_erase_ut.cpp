#include <gtest/gtest.h>
#include "unrolled_list.h"

TEST(InsertEraseTests, SingleInsert) {
    unrolled_list<int> list;
    list.insert(list.begin(), 42);
    EXPECT_EQ(list.front(), 42);
    EXPECT_EQ(list.size(), 2);
}

TEST(InsertEraseTests, MultipleInserts) {
    unrolled_list<int> list;
    list.insert(list.begin(), 1);
    list.insert(list.begin(), 2);
    list.insert(list.begin(), 3);
    EXPECT_EQ(list.front(), 3);
    EXPECT_EQ(list.back(), 1);
    EXPECT_EQ(list.size(), 6);
}

TEST(InsertEraseTests, InsertAtEnd) {
    unrolled_list<int> list = {1, 2, 3};
    list.insert(1, 4);
    EXPECT_EQ(list.back(), 3);
    EXPECT_EQ(list.size(), 4);
}