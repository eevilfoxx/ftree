#include <gtest/gtest.h>
#include <presentation/FamilyDisplay.hpp>

using namespace ftree::presentation;

/** --- Test: FamilyDisplay --- */
TEST(FamilyDisplayTest, DefaultConstruction) {
    // FamilyDisplay is an aggregate, default-initialization may leave members uninitialized
    // Use aggregate initialization with braces to ensure zero-initialization
    FamilyDisplay fd{};
    EXPECT_EQ(fd.id, 0u);
    EXPECT_EQ(fd.motherId, 0u);
    EXPECT_EQ(fd.fatherId, 0u);
    EXPECT_EQ(fd.childrenIds.size(), 0u);
    EXPECT_EQ(fd.surname, "");
    EXPECT_EQ(fd.generationLevel, 0);
    EXPECT_EQ(fd.memberCount, 0);
}

TEST(FamilyDisplayTest, AggregateInitialization) {
    FamilyDisplay fd{1, 2, 3, {4, 5}, "Smith", 10, 5};
    EXPECT_EQ(fd.id, 1u);
    EXPECT_EQ(fd.motherId, 2u);
    EXPECT_EQ(fd.fatherId, 3u);
    EXPECT_EQ(fd.childrenIds.size(), 2u);
    EXPECT_EQ(fd.childrenIds[0], 4u);
    EXPECT_EQ(fd.childrenIds[1], 5u);
    EXPECT_EQ(fd.surname, "Smith");
    EXPECT_EQ(fd.generationLevel, 10);
    EXPECT_EQ(fd.memberCount, 5);
}

TEST(FamilyDisplayTest, Setters) {
    FamilyDisplay fd;
    fd.id = 1;
    fd.motherId = 2;
    fd.fatherId = 3;
    fd.childrenIds = {4, 5};
    fd.surname = "Smith";
    fd.generationLevel = 10;
    fd.memberCount = 5;

    EXPECT_EQ(fd.id, 1u);
    EXPECT_EQ(fd.motherId, 2u);
    EXPECT_EQ(fd.fatherId, 3u);
    EXPECT_EQ(fd.childrenIds.size(), 2u);
    EXPECT_EQ(fd.childrenIds[0], 4u);
    EXPECT_EQ(fd.childrenIds[1], 5u);
    EXPECT_EQ(fd.surname, "Smith");
    EXPECT_EQ(fd.generationLevel, 10);
    EXPECT_EQ(fd.memberCount, 5);
}
