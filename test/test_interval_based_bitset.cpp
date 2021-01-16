#include "interval_based_bitset.h"
#include "gtest/gtest.h"


TEST(IntervalBasedBitset, DefaultConstructor) {

    auto bitset = interval_based_bitset<32>();
    ASSERT_TRUE(bitset.none());
    ASSERT_FALSE(bitset.all());
    ASSERT_FALSE(bitset.any());
    ASSERT_EQ(bitset.size(), 32);
}

