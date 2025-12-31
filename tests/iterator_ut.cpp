#include <gtest/gtest.h>
#include "unrolled_list.h"

TEST(IteratorTests, BasicIteration) {
    unrolled_list<int> list = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};
    int index = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(*it, expected[index++]);
    }
}

TEST(IteratorTests, ReverseIteration) {
    unrolled_list<int> list = {1, 2, 3, 4, 5};
    int expected[] = {5, 4, 3, 2, 1};
    int index = 0;
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        EXPECT_EQ(*it, expected[index++]);
    }
}

TEST(IteratorTests, ConstIterator) {
    const unrolled_list<int> list = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};
    int index = 0;
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        EXPECT_EQ(*it, expected[index++]);
    }
}

TEST(IteratorTests, IteratorArithmetic) {
    unrolled_list<int> list = {1, 2, 3, 4, 5};
    auto it = list.begin();
    EXPECT_EQ(*(it++), 1);
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(*(++it), 3);
    EXPECT_EQ(*(it--), 3);
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(*(--it), 1);
}

TEST(IteratorTests, IteratorComparison) {
    unrolled_list<int> list = {1, 2, 3, 4, 5};
    auto it1 = list.begin();
    auto it2 = list.begin();
    EXPECT_TRUE(it1 == it2);
    ++it1;
    EXPECT_FALSE(it1 == it2);
    EXPECT_TRUE(it1 != it2);
}