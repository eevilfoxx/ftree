#include <gtest/gtest.h>
#include <entities/Gender.hpp>

TEST(GenderTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(Gender::Male), 0);
    EXPECT_EQ(static_cast<int>(Gender::Female), 1);
}

TEST(GenderTest, DistinctValues) {
    EXPECT_NE(Gender::Male, Gender::Female);
}