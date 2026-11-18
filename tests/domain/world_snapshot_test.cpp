#include <gtest/gtest.h>
#include "domain/World.hpp"

#include <algorithm>
#include <limits>

using namespace ftree::domain;

TEST(WorldSnapshotTest, RoundTripPreservesPeopleFamiliesAndCounters) {
    World world;
    const auto mother = world.createPerson("Ada", Gender::Female, 31);
    const auto father = world.createPerson("Alan", Gender::Male, 33);
    const auto family = world.createFamily(mother, father, "Lovelace");
    const auto child = world.createPerson("Byron", Gender::Male, 4);
    auto& childData = world.getPersonRef(child);
    childData.motherId = mother;
    childData.fatherId = father;
    world.getFamilyRef(family).childrenIds.push_back(child);
    world.killPerson(father);

    const auto snapshot = world.createSnapshot();
    const auto restored = World::fromSnapshot(snapshot);
    const auto roundTrip = restored.createSnapshot();

    ASSERT_EQ(roundTrip.people.size(), 3u);
    ASSERT_EQ(roundTrip.families.size(), 1u);
    EXPECT_EQ(roundTrip.people[2].id, child);
    EXPECT_EQ(restored.getPersonRef(father).alive, false);
    EXPECT_EQ(restored.getFamilyRef(family).childrenIds, (std::vector<PersonId>{child}));
    EXPECT_EQ(roundTrip.nextPersonId, snapshot.nextPersonId);
    EXPECT_EQ(roundTrip.nextFamilyId, snapshot.nextFamilyId);
}

TEST(WorldSnapshotTest, PreservesExhaustedIdCounters) {
    WorldSnapshot snapshot;
    snapshot.nextPersonId = 0;
    snapshot.nextFamilyId = 0;
    auto restored = World::fromSnapshot(snapshot);
    EXPECT_THROW(restored.createPerson("No ID", Gender::Male), std::overflow_error);
}

TEST(WorldSnapshotTest, RejectsDuplicatePersonIds) {
    WorldSnapshot snapshot;
    PersonData person;
    person.id = 1;
    snapshot.people = {person, person};
    snapshot.nextPersonId = 2;
    EXPECT_THROW(World::fromSnapshot(snapshot), std::invalid_argument);
}

TEST(WorldSnapshotTest, RejectsMissingParentReference) {
    WorldSnapshot snapshot;
    PersonData person;
    person.id = 1;
    person.motherId = 99;
    snapshot.people.push_back(person);
    snapshot.nextPersonId = 2;
    EXPECT_THROW(World::fromSnapshot(snapshot), std::invalid_argument);
}

TEST(WorldSnapshotTest, RejectsInvalidNextId) {
    WorldSnapshot snapshot;
    PersonData person;
    person.id = 5;
    snapshot.people.push_back(person);
    snapshot.nextPersonId = 5;
    EXPECT_THROW(World::fromSnapshot(snapshot), std::invalid_argument);
}

TEST(WorldSnapshotTest, PreservesFamilyCounterAfterFamilyRemoval) {
    World world;
    const auto mother = world.createPerson("Mother", Gender::Female, 30);
    const auto father = world.createPerson("Father", Gender::Male, 30);
    const auto removedFamily = world.createFamily(mother, father, "Old");
    ASSERT_EQ(removedFamily, 1u);
    world.dissolveFamily(removedFamily);

    auto restored = World::fromSnapshot(world.createSnapshot());
    EXPECT_EQ(restored.createFamily(mother, father, "New"), 2u);
}
