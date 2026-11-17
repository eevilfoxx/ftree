#include <gtest/gtest.h>
#include <entities/Person.hpp>
#include <entities/Family.hpp>

#include <core/Config.hpp>
#include <presentation/WorldPresentation.hpp>
#include <presentation/PersonDisplay.hpp>
#include <presentation/FamilyDisplay.hpp>
#include <algorithm>

using namespace ftree::presentation;
using namespace ftree::domain;

// File-level test world and IDs
World testWorld;
PersonId aliceId;
PersonId bobId;
PersonId charlieId;
PersonId dianaId;
PersonId eveId;
FamilyId fam1Id;
FamilyId fam2Id;

// Initialize test world once before all tests
void initTestWorld() {
    testWorld = World();
    aliceId = testWorld.createPerson("Alice", ftree::domain::Gender::Female, 30);
    bobId = testWorld.createPerson("Bob", ftree::domain::Gender::Male, 35);
    charlieId = testWorld.createPerson("Charlie", ftree::domain::Gender::Male, 25);
    dianaId = testWorld.createPerson("Diana", ftree::domain::Gender::Female, 28);
    eveId = testWorld.createPerson("Eve", ftree::domain::Gender::Female, 32);
    testWorld.killPerson(dianaId);
    fam1Id = testWorld.createFamily(aliceId, bobId, "Smith");
    fam2Id = testWorld.createFamily(dianaId, eveId, "Johnson");
    testWorld.getFamilyRef(fam1Id).childrenIds.push_back(charlieId);
}

// Test fixture class
class WorldPresentationFixture : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        initTestWorld();
    }
};

/** --- Test: getAllPeople --- */
TEST_F(WorldPresentationFixture, GetAllPeopleReturnsCorrectCount) {
    auto people = WorldPresentation::getAllPeople(testWorld);
    EXPECT_EQ(people.size(), 5u);
}

TEST_F(WorldPresentationFixture, GetAllPeopleReturnsPersonDisplays) {
    auto people = WorldPresentation::getAllPeople(testWorld);
    for (const auto& pd : people) {
        EXPECT_NE(pd.id, 0u);
        EXPECT_FALSE(pd.name.empty());
        EXPECT_FALSE(pd.displayName.empty());
    }
}

/** --- Test: getAllFamilies --- */
TEST_F(WorldPresentationFixture, GetAllFamiliesReturnsCorrectCount) {
    auto families = WorldPresentation::getAllFamilies(testWorld);
    EXPECT_EQ(families.size(), 2u);
}

TEST_F(WorldPresentationFixture, GetAllFamiliesReturnsFamilyDisplays) {
    auto families = WorldPresentation::getAllFamilies(testWorld);
    for (const auto& fd : families) {
        EXPECT_NE(fd.id, 0u);
        EXPECT_FALSE(fd.surname.empty());
        EXPECT_NE(fd.motherId, 0u);
        EXPECT_NE(fd.fatherId, 0u);
    }
}

/** --- Test: getLivingPeopleIds --- */
TEST_F(WorldPresentationFixture, GetLivingPeopleIdsReturnsLivingPeople) {
    auto livingIds = WorldPresentation::getLivingPeopleIds(testWorld);
    EXPECT_EQ(livingIds.size(), 4u);
    for (auto id : livingIds) {
        auto person = testWorld.getPerson(id);
        ASSERT_TRUE(person.has_value());
        EXPECT_TRUE(person->alive);
    }
}

TEST_F(WorldPresentationFixture, GetLivingPeopleIdsExcludesDead) {
    auto livingIds = WorldPresentation::getLivingPeopleIds(testWorld);
    EXPECT_TRUE(std::find(livingIds.begin(), livingIds.end(), dianaId) == livingIds.end());
}

/** --- Test: getFamilyMembers --- */
TEST_F(WorldPresentationFixture, GetFamilyMembersReturnsMembers) {
    auto familyMembers = WorldPresentation::getFamilyMembers(testWorld, fam1Id);
    EXPECT_TRUE(std::find(familyMembers.begin(), familyMembers.end(), aliceId) != familyMembers.end());
    EXPECT_TRUE(std::find(familyMembers.begin(), familyMembers.end(), bobId) != familyMembers.end());
    EXPECT_TRUE(std::find(familyMembers.begin(), familyMembers.end(), charlieId) != familyMembers.end());
}

TEST_F(WorldPresentationFixture, GetFamilyMembersExcludesNonExistentFamily) {
    auto familyMembers = WorldPresentation::getFamilyMembers(testWorld, 9999);
    EXPECT_TRUE(familyMembers.empty());
}

/** --- Test: getPersonData --- */
TEST_F(WorldPresentationFixture, GetPersonDataReturnsValidDisplay) {
    auto pd = WorldPresentation::getPersonData(testWorld, aliceId);
    EXPECT_NE(pd.id, 0u);
    EXPECT_EQ(pd.id, aliceId);
    EXPECT_FALSE(pd.name.empty());
    EXPECT_FALSE(pd.displayName.empty());
}

TEST_F(WorldPresentationFixture, GetPersonDataForDeadPerson) {
    auto pd = WorldPresentation::getPersonData(testWorld, dianaId);
    EXPECT_NE(pd.id, 0u);
    EXPECT_EQ(pd.id, dianaId);
    EXPECT_FALSE(pd.isAlive);
    EXPECT_TRUE(pd.isDeceased);
}

// Google Test main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
