
#include <gtest/gtest.h>

#include <presentation/PersonDisplay.hpp>

using namespace ftree::presentation;

TEST(PersonDisplayTest, DefaultConstruction) {
    PersonDisplay pd{};

    EXPECT_EQ(pd.id, 0u);
    EXPECT_TRUE(pd.name.empty());
    EXPECT_TRUE(pd.displayName.empty());

    EXPECT_EQ(pd.age, 0);

    EXPECT_FALSE(pd.isMale);
    EXPECT_TRUE(pd.isAlive);
    EXPECT_FALSE(pd.isPregnant);
    EXPECT_FALSE(pd.isDeceased);

    EXPECT_EQ(pd.motherId, 0u);
    EXPECT_EQ(pd.fatherId, 0u);
    EXPECT_EQ(pd.familyId, 0u);

    EXPECT_EQ(pd.generationLevel, 0);
    EXPECT_EQ(pd.depth, 0);
}

TEST(PersonDisplayTest, AggregateInitialization) {
    PersonDisplay pd{
        1,                  // id
        "John",             // name
        "John 25 yrs",      // displayName
        25,                 // age
        true,               // isMale
        true,               // isAlive
        false,              // isPregnant
        false,              // isDeceased
        1,                  // motherId
        2,                  // fatherId
        3,                  // familyId
        5,                  // generationLevel
        10                  // depth
    };

    EXPECT_EQ(pd.id, 1u);
    EXPECT_EQ(pd.name, "John");
    EXPECT_EQ(pd.displayName, "John 25 yrs");
    EXPECT_EQ(pd.age, 25);

    EXPECT_TRUE(pd.isMale);
    EXPECT_TRUE(pd.isAlive);
    EXPECT_FALSE(pd.isPregnant);
    EXPECT_FALSE(pd.isDeceased);

    EXPECT_EQ(pd.motherId, 1u);
    EXPECT_EQ(pd.fatherId, 2u);
    EXPECT_EQ(pd.familyId, 3u);

    EXPECT_EQ(pd.generationLevel, 5);
    EXPECT_EQ(pd.depth, 10);
}

TEST(PersonDisplayTest, FieldAssignment) {
    PersonDisplay pd;

    pd.id = 1;
    pd.name = "John";
    pd.displayName = "John 25 yrs";
    pd.age = 25;

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
    EXPECT_EQ(pd.age, 25);

    EXPECT_TRUE(pd.isMale);
    EXPECT_TRUE(pd.isAlive);
    EXPECT_FALSE(pd.isPregnant);
    EXPECT_FALSE(pd.isDeceased);

    EXPECT_EQ(pd.motherId, 1u);
    EXPECT_EQ(pd.fatherId, 2u);
    EXPECT_EQ(pd.familyId, 3u);

    EXPECT_EQ(pd.generationLevel, 5);
    EXPECT_EQ(pd.depth, 10);
}

TEST(PersonDisplayTest, DeceasedPerson) {
    PersonDisplay pd;

    pd.isAlive = false;
    pd.isDeceased = true;

    EXPECT_FALSE(pd.isAlive);
    EXPECT_TRUE(pd.isDeceased);
}

