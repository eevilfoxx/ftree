#include <gtest/gtest.h>
#include <presentation/PersonDisplay.hpp>

using namespace ftree::presentation;

/** --- Test: PersonDisplay --- */
TEST(PersonDisplayTest, DefaultConstruction) {
    // PersonDisplay is an aggregate; use brace initialization to ensure zero-init
    PersonDisplay pd{};
    // Verify integer members are zero-initialized
    EXPECT_EQ(pd.id, 0u);
    EXPECT_EQ(pd.motherId, 0u);
    EXPECT_EQ(pd.fatherId, 0u);
    EXPECT_EQ(pd.familyId, 0u);
    EXPECT_EQ(pd.generationLevel, 0);
    EXPECT_EQ(pd.depth, 0);
    // Bool members are false with value-initialization - that's expected
    EXPECT_FALSE(pd.isMale);
    EXPECT_FALSE(pd.isAlive);
    EXPECT_FALSE(pd.isPregnant);
    EXPECT_FALSE(pd.isDeceased);
}

TEST(PersonDisplay, AggregateInitialization) {
    PersonDisplay pd{1, "John", "John 25 yrs", true, true, false, false, 1, 2, 3, 5, 10};
    EXPECT_EQ(pd.id, 1u);
    EXPECT_EQ(pd.name, "John");
    EXPECT_EQ(pd.displayName, "John 25 yrs");
    EXPECT_TRUE(pd.isMale);
    EXPECT_TRUE(pd.isAlive);
    EXPECT_FALSE(pd.isPregnant);
    EXPECT_FALSE(pd.isDeceased);
    EXPECT_EQ(pd.motherId, 1);
    EXPECT_EQ(pd.fatherId, 2);
    EXPECT_EQ(pd.familyId, 3);
    EXPECT_EQ(pd.generationLevel, 5);
    EXPECT_EQ(pd.depth, 10);
}

TEST(PersonDisplay, Setters) {
    PersonDisplay pd;
    pd.id = 1;
    pd.name = "John";
    pd.displayName = "John 25 yrs";
    pd.isMale = true;
    pd.isAlive = true;
    pd.isPregnant = false;
    pd.isDeceased = false;
    pd.motherId = 1;
    pd.fatherId = 2;
    pd.familyId = 3;
    pd.generationLevel = 5;
    pd.depth = 10;

    EXPECT_EQ(pd.id, 1u);
    EXPECT_EQ(pd.name, "John");
    EXPECT_EQ(pd.displayName, "John 25 yrs");
    EXPECT_TRUE(pd.isMale);
    EXPECT_TRUE(pd.isAlive);
    EXPECT_FALSE(pd.isPregnant);
    EXPECT_FALSE(pd.isDeceased);
    EXPECT_EQ(pd.motherId, 1);
    EXPECT_EQ(pd.fatherId, 2);
    EXPECT_EQ(pd.familyId, 3);
    EXPECT_EQ(pd.generationLevel, 5);
    EXPECT_EQ(pd.depth, 10);
}
