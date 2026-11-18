#include <gtest/gtest.h>

#include <domain/World.hpp>
#include <presentation/WorldPresentation.hpp>

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace {

using namespace ftree::domain;
using namespace ftree::presentation;

class WorldPresentationTest : public ::testing::Test {
protected:
    World world;

    PersonId aliceId = 0;
    PersonId bobId = 0;
    PersonId charlieId = 0;
    PersonId dianaId = 0;
    PersonId evanId = 0;
    PersonId frankId = 0;

    FamilyId smithFamilyId = 0;
    FamilyId johnsonFamilyId = 0;

    void SetUp() override {
        // Generation 0
        aliceId = world.createPerson(
            "Alice",
            Gender::Female,
            60
        );

        bobId = world.createPerson(
            "Bob",
            Gender::Male,
            62
        );

        // Generation 1
        charlieId = world.createPerson(
            "Charlie",
            Gender::Male,
            35
        );

        dianaId = world.createPerson(
            "Diana",
            Gender::Female,
            33
        );

        // Generation 2
        evanId = world.createPerson(
            "Evan",
            Gender::Male,
            10
        );

        frankId = world.createPerson(
            "Frank",
            Gender::Male,
            8
        );

        // First family: Alice + Bob
        smithFamilyId = world.createFamily(
            aliceId,
            bobId,
            "Smith"
        );

        // Establish parent-child relationships.
        world.getFamilyRef(smithFamilyId).childrenIds = {
            charlieId,
            dianaId
        };

        world.getPersonRef(charlieId).motherId = aliceId;
        world.getPersonRef(charlieId).fatherId = bobId;

        world.getPersonRef(dianaId).motherId = aliceId;
        world.getPersonRef(dianaId).fatherId = bobId;

        // Second family: Diana + Charlie.
        // They are already children of the first family.
        // Their family of origin and their current family are
        // different concepts.
        johnsonFamilyId = world.createFamily(
            dianaId,
            charlieId,
            "Johnson"
        );

        world.getFamilyRef(johnsonFamilyId).childrenIds = {
            evanId,
            frankId
        };

        world.getPersonRef(evanId).motherId = dianaId;
        world.getPersonRef(evanId).fatherId = charlieId;

        world.getPersonRef(frankId).motherId = dianaId;
        world.getPersonRef(frankId).fatherId = charlieId;
    }

    static bool contains(
        const std::vector<PersonId>& values,
        PersonId id
    ) {
        return std::find(
            values.begin(),
            values.end(),
            id
        ) != values.end();
    }
};

// ------------------------------------------------------------
// getAllPeople
// ------------------------------------------------------------

TEST_F(
    WorldPresentationTest,
    GetAllPeopleReturnsAllPeople
) {
    const auto people =
        WorldPresentation::getAllPeople(world);

    EXPECT_EQ(people.size(), 6u);
}

TEST_F(
    WorldPresentationTest,
    GetAllPeopleReturnsCorrectPersonData
) {
    const auto people =
        WorldPresentation::getAllPeople(world);

    ASSERT_EQ(people.size(), 6u);

    const auto it = std::find_if(
        people.begin(),
        people.end(),
        [this](const PersonDisplay& person) {
            return person.id == aliceId;
        }
    );

    ASSERT_NE(it, people.end());

    EXPECT_EQ(it->name, "Alice");
    EXPECT_EQ(it->age, 60);
    EXPECT_EQ(it->displayName, "Alice 60 yrs");
    EXPECT_FALSE(it->isMale);
    EXPECT_TRUE(it->isAlive);
    EXPECT_FALSE(it->isDeceased);
    EXPECT_EQ(it->motherId, 0u);
    EXPECT_EQ(it->fatherId, 0u);
}

TEST_F(
    WorldPresentationTest,
    GetAllPeopleReturnsSortedResults
) {
    const auto people =
        WorldPresentation::getAllPeople(world);

    ASSERT_EQ(people.size(), 6u);

    for (std::size_t i = 1; i < people.size(); ++i) {
        EXPECT_LT(
            people[i - 1].id,
            people[i].id
        );
    }
}

TEST_F(
    WorldPresentationTest,
    GetAllPeopleIncludesDeceasedPeople
) {
    world.killPerson(aliceId);

    const auto people =
        WorldPresentation::getAllPeople(world);

    const auto it = std::find_if(
        people.begin(),
        people.end(),
        [this](const PersonDisplay& person) {
            return person.id == aliceId;
        }
    );

    ASSERT_NE(it, people.end());

    EXPECT_FALSE(it->isAlive);
    EXPECT_TRUE(it->isDeceased);
}

TEST_F(
    WorldPresentationTest,
    GetAllPeopleReturnsCorrectGender
) {
    const auto people =
        WorldPresentation::getAllPeople(world);

    const auto charlie = std::find_if(
        people.begin(),
        people.end(),
        [this](const PersonDisplay& person) {
            return person.id == charlieId;
        }
    );

    const auto diana = std::find_if(
        people.begin(),
        people.end(),
        [this](const PersonDisplay& person) {
            return person.id == dianaId;
        }
    );

    ASSERT_NE(charlie, people.end());
    ASSERT_NE(diana, people.end());

    EXPECT_TRUE(charlie->isMale);
    EXPECT_FALSE(diana->isMale);
}

// ------------------------------------------------------------
// Generation levels
// ------------------------------------------------------------

TEST_F(
    WorldPresentationTest,
    RootPeopleHaveGenerationZero
) {
    const auto people =
        WorldPresentation::getAllPeople(world);

    for (const auto& person : people) {
        if (person.id == aliceId || person.id == bobId) {
            EXPECT_EQ(person.generationLevel, 0);
        }
    }
}

TEST_F(
    WorldPresentationTest,
    ChildrenHaveGenerationOne
) {
    const auto people =
        WorldPresentation::getAllPeople(world);

    for (const auto& person : people) {
        if (person.id == charlieId || person.id == dianaId) {
            EXPECT_EQ(person.generationLevel, 1);
        }
    }
}

TEST_F(
    WorldPresentationTest,
    GrandchildrenHaveGenerationTwo
) {
    const auto people =
        WorldPresentation::getAllPeople(world);

    for (const auto& person : people) {
        if (person.id == evanId || person.id == frankId) {
            EXPECT_EQ(person.generationLevel, 2);
        }
    }
}

TEST_F(
    WorldPresentationTest,
    GenerationUsesHighestKnownParentGeneration
) {
    // Create another person without a known mother.
    // Their father belongs to generation 1.
    const PersonId childId = world.createPerson(
        "Greg",
        Gender::Male,
        5
    );

    world.getPersonRef(childId).fatherId = charlieId;

    const auto display =
        WorldPresentation::getPersonData(world, childId);

    EXPECT_EQ(display.generationLevel, 2);
}

// ------------------------------------------------------------
// getAllFamilies
// ------------------------------------------------------------

TEST_F(
    WorldPresentationTest,
    GetAllFamiliesReturnsAllFamilies
) {
    const auto families =
        WorldPresentation::getAllFamilies(world);

    EXPECT_EQ(families.size(), 2u);
}

TEST_F(
    WorldPresentationTest,
    GetAllFamiliesReturnsSortedResults
) {
    const auto families =
        WorldPresentation::getAllFamilies(world);

    ASSERT_EQ(families.size(), 2u);

    for (std::size_t i = 1; i < families.size(); ++i) {
        EXPECT_LT(
            families[i - 1].id,
            families[i].id
        );
    }
}

TEST_F(
    WorldPresentationTest,
    GetAllFamiliesReturnsCorrectParents
) {
    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == smithFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    EXPECT_EQ(it->motherId, aliceId);
    EXPECT_EQ(it->fatherId, bobId);
    EXPECT_EQ(it->surname, "Smith");
}

TEST_F(
    WorldPresentationTest,
    GetAllFamiliesReturnsSortedChildren
) {
    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == smithFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    ASSERT_EQ(it->childrenIds.size(), 2u);

    EXPECT_TRUE(std::is_sorted(
        it->childrenIds.begin(),
        it->childrenIds.end()
    ));
}

TEST_F(
    WorldPresentationTest,
    GetAllFamiliesRemovesDuplicateChildren
) {
    world.getFamilyRef(smithFamilyId).childrenIds.push_back(
        charlieId
    );

    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == smithFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    EXPECT_EQ(it->childrenIds.size(), 2u);
    EXPECT_EQ(
        std::count(
            it->childrenIds.begin(),
            it->childrenIds.end(),
            charlieId
        ),
        1
    );
}

// ------------------------------------------------------------
// Family generation and member count
// ------------------------------------------------------------

TEST_F(
    WorldPresentationTest,
    RootFamilyHasGenerationZero
) {
    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == smithFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    EXPECT_EQ(it->generationLevel, 0);
}

TEST_F(
    WorldPresentationTest,
    FamilyGenerationMatchesParentGeneration
) {
    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == johnsonFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    EXPECT_EQ(it->generationLevel, 1);
}

TEST_F(
    WorldPresentationTest,
    FamilyMemberCountIncludesParentsAndChildren
) {
    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == smithFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    EXPECT_EQ(it->memberCount, 4);
}

TEST_F(
    WorldPresentationTest,
    FamilyMemberCountIgnoresDuplicateIds
) {
    world.getFamilyRef(smithFamilyId).childrenIds.push_back(
        charlieId
    );

    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == smithFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    EXPECT_EQ(it->memberCount, 4);
}

TEST_F(
    WorldPresentationTest,
    FamilyMemberCountIncludesDeceasedPeople
) {
    world.killPerson(aliceId);

    const auto families =
        WorldPresentation::getAllFamilies(world);

    const auto it = std::find_if(
        families.begin(),
        families.end(),
        [this](const FamilyDisplay& family) {
            return family.id == smithFamilyId;
        }
    );

    ASSERT_NE(it, families.end());

    EXPECT_EQ(it->memberCount, 4);
}

// ------------------------------------------------------------
// getLivingPeopleIds
// ------------------------------------------------------------

TEST_F(
    WorldPresentationTest,
    GetLivingPeopleIdsReturnsOnlyLivingPeople
) {
    world.killPerson(aliceId);
    world.killPerson(frankId);

    const auto livingIds =
        WorldPresentation::getLivingPeopleIds(world);

    EXPECT_EQ(livingIds.size(), 4u);

    EXPECT_FALSE(contains(livingIds, aliceId));
    EXPECT_FALSE(contains(livingIds, frankId));

    EXPECT_TRUE(contains(livingIds, bobId));
    EXPECT_TRUE(contains(livingIds, charlieId));
    EXPECT_TRUE(contains(livingIds, dianaId));
    EXPECT_TRUE(contains(livingIds, evanId));
}

TEST_F(
    WorldPresentationTest,
    GetLivingPeopleIdsReturnsSortedResults
) {
    const auto livingIds =
        WorldPresentation::getLivingPeopleIds(world);

    EXPECT_TRUE(std::is_sorted(
        livingIds.begin(),
        livingIds.end()
    ));
}

TEST_F(
    WorldPresentationTest,
    GetLivingPeopleIdsReturnsEmptyForWorldWithoutPeople
) {
    const World emptyWorld;

    const auto livingIds =
        WorldPresentation::getLivingPeopleIds(emptyWorld);

    EXPECT_TRUE(livingIds.empty());
}

// ------------------------------------------------------------
// getFamilyMembers
// ------------------------------------------------------------

TEST_F(
    WorldPresentationTest,
    GetFamilyMembersReturnsParentsAndChildren
) {
    const auto members =
        WorldPresentation::getFamilyMembers(
            world,
            smithFamilyId
        );

    EXPECT_EQ(members.size(), 4u);

    EXPECT_TRUE(contains(members, aliceId));
    EXPECT_TRUE(contains(members, bobId));
    EXPECT_TRUE(contains(members, charlieId));
    EXPECT_TRUE(contains(members, dianaId));
}

TEST_F(
    WorldPresentationTest,
    GetFamilyMembersReturnsSortedResults
) {
    const auto members =
        WorldPresentation::getFamilyMembers(
            world,
            smithFamilyId
        );

    EXPECT_TRUE(std::is_sorted(
        members.begin(),
        members.end()
    ));
}

TEST_F(
    WorldPresentationTest,
    GetFamilyMembersReturnsEmptyForUnknownFamily
) {
    const auto members =
        WorldPresentation::getFamilyMembers(world, 9999);

    EXPECT_TRUE(members.empty());
}

TEST_F(
    WorldPresentationTest,
    GetFamilyMembersDoesNotReturnDuplicateIds
) {
    world.getFamilyRef(smithFamilyId).childrenIds.push_back(
        charlieId
    );

    const auto members =
        WorldPresentation::getFamilyMembers(
            world,
            smithFamilyId
        );

    EXPECT_EQ(members.size(), 4u);
}

// ------------------------------------------------------------
// getPersonData
// ------------------------------------------------------------

TEST_F(
    WorldPresentationTest,
    GetPersonDataReturnsCorrectPerson
) {
    const auto person =
        WorldPresentation::getPersonData(world, aliceId);

    EXPECT_EQ(person.id, aliceId);
    EXPECT_EQ(person.name, "Alice");
    EXPECT_EQ(person.age, 60);
    EXPECT_EQ(person.displayName, "Alice 60 yrs");
    EXPECT_FALSE(person.isMale);
    EXPECT_TRUE(person.isAlive);
    EXPECT_FALSE(person.isDeceased);
}

TEST_F(
    WorldPresentationTest,
    GetPersonDataReturnsCorrectGeneration
) {
    const auto person =
        WorldPresentation::getPersonData(world, evanId);

    EXPECT_EQ(person.generationLevel, 2);
}

TEST_F(
    WorldPresentationTest,
    GetPersonDataWorksForDeceasedPerson
) {
    world.killPerson(dianaId);

    const auto person =
        WorldPresentation::getPersonData(world, dianaId);

    EXPECT_EQ(person.id, dianaId);
    EXPECT_FALSE(person.isAlive);
    EXPECT_TRUE(person.isDeceased);
}

TEST_F(
    WorldPresentationTest,
    GetPersonDataThrowsForUnknownPerson
) {
    EXPECT_THROW(
        WorldPresentation::getPersonData(world, 9999),
        std::out_of_range
    );
}

// ------------------------------------------------------------
// Empty world
// ------------------------------------------------------------

TEST(
    WorldPresentationEmptyWorldTest,
    GetAllPeopleReturnsEmptyVector
) {
    const World world;

    const auto people =
        WorldPresentation::getAllPeople(world);

    EXPECT_TRUE(people.empty());
}

TEST(
    WorldPresentationEmptyWorldTest,
    GetAllFamiliesReturnsEmptyVector
) {
    const World world;

    const auto families =
        WorldPresentation::getAllFamilies(world);

    EXPECT_TRUE(families.empty());
}

} // namespace