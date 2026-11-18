
#include <gtest/gtest.h>

#include "presentation/TreeLayout.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace {

using namespace ftree::presentation;

PersonDisplay makePerson(
    PersonId id,
    int generation,
    PersonId motherId = 0,
    PersonId fatherId = 0
) {
    PersonDisplay person;
    person.id = id;
    person.generationLevel = generation;
    person.motherId = motherId;
    person.fatherId = fatherId;
    return person;
}

FamilyDisplay makeFamily(
    FamilyId id,
    PersonId motherId,
    PersonId fatherId
) {
    FamilyDisplay family;
    family.id = id;
    family.motherId = motherId;
    family.fatherId = fatherId;
    return family;
}

const LayoutNode& findNode(
    const std::vector<LayoutNode>& nodes,
    PersonId id
) {
    const auto it = std::find_if(
        nodes.begin(),
        nodes.end(),
        [id](const LayoutNode& node) {
            return node.id == id;
        }
    );

    if (it == nodes.end()) {
        throw std::runtime_error("Layout node not found");
    }

    return *it;
}

TEST(TreeLayoutTest, EmptyPeopleProducesEmptyLayout) {
    const auto result = calculateTreeLayout({}, {});

    EXPECT_TRUE(result.empty());
}

TEST(TreeLayoutTest, CreatesExactlyOneNodeForEachPerson) {
    std::vector<PersonDisplay> people = {
        makePerson(1, 0),
        makePerson(2, 0),
        makePerson(3, 1, 1, 2)
    };

    const auto result = calculateTreeLayout(people, {});

    ASSERT_EQ(result.size(), 3u);

    EXPECT_EQ(result[0].id, 1u);
    EXPECT_EQ(result[1].id, 2u);
    EXPECT_EQ(result[2].id, 3u);
}

TEST(TreeLayoutTest, PlacesGenerationsAtDifferentVerticalLevels) {
    std::vector<PersonDisplay> people = {
        makePerson(1, 0),
        makePerson(2, 0),
        makePerson(3, 1, 1, 2),
        makePerson(4, 2, 3)
    };

    const auto result = calculateTreeLayout(
        people,
        {},
        80.0f,
        100.0f
    );

    ASSERT_EQ(result.size(), 4u);

    EXPECT_FLOAT_EQ(findNode(result, 1).y, 0.0f);
    EXPECT_FLOAT_EQ(findNode(result, 3).y, 130.0f);
    EXPECT_FLOAT_EQ(findNode(result, 4).y, 260.0f);
}

TEST(TreeLayoutTest, PlacesPartnersNextToEachOther) {
    std::vector<PersonDisplay> people = {
        makePerson(1, 0),
        makePerson(2, 0),
        makePerson(3, 0)
    };

    std::vector<FamilyDisplay> families = {
        makeFamily(1, 1, 3)
    };

    const auto result = calculateTreeLayout(people, families);

    ASSERT_EQ(result.size(), 3u);

    const float distance = std::abs(
        findNode(result, 1).x - findNode(result, 3).x
    );

    EXPECT_FLOAT_EQ(distance, 230.0f);
}

TEST(TreeLayoutTest, KeepsSiblingsInDeterministicOrder) {
    std::vector<PersonDisplay> people = {
        makePerson(1, 0),
        makePerson(2, 0),
        makePerson(3, 1, 1, 2),
        makePerson(4, 1, 1, 2),
        makePerson(5, 1, 1, 2)
    };

    const auto result = calculateTreeLayout(people, {});

    ASSERT_EQ(result.size(), 5u);

    EXPECT_LT(findNode(result, 3).x, findNode(result, 4).x);
    EXPECT_LT(findNode(result, 4).x, findNode(result, 5).x);
}

TEST(TreeLayoutTest, ProducesSameCoordinatesRegardlessOfInputOrder) {
    std::vector<PersonDisplay> peopleA = {
        makePerson(1, 0),
        makePerson(2, 0),
        makePerson(3, 1, 1, 2),
        makePerson(4, 1, 1, 2)
    };

    std::vector<PersonDisplay> peopleB = {
        makePerson(4, 1, 1, 2),
        makePerson(2, 0),
        makePerson(3, 1, 1, 2),
        makePerson(1, 0)
    };

    const auto resultA = calculateTreeLayout(peopleA, {});
    const auto resultB = calculateTreeLayout(peopleB, {});

    ASSERT_EQ(resultA.size(), resultB.size());

    for (std::size_t i = 0; i < resultA.size(); ++i) {
        EXPECT_EQ(resultA[i].id, resultB[i].id);
        EXPECT_FLOAT_EQ(resultA[i].x, resultB[i].x);
        EXPECT_FLOAT_EQ(resultA[i].y, resultB[i].y);
    }
}

TEST(TreeLayoutTest, IgnoresFamiliesWithMissingPartners) {
    std::vector<PersonDisplay> people = {
        makePerson(1, 0),
        makePerson(2, 0)
    };

    std::vector<FamilyDisplay> families = {
        makeFamily(1, 1, 999)
    };

    const auto result = calculateTreeLayout(people, families);

    EXPECT_EQ(result.size(), 2u);
}

TEST(TreeLayoutTest, RejectsNegativeSpacing) {
    std::vector<PersonDisplay> people = {
        makePerson(1, 0)
    };

    EXPECT_TRUE(
        calculateTreeLayout(people, {}, -10.0f, 100.0f).empty()
    );

    EXPECT_TRUE(
        calculateTreeLayout(people, {}, 80.0f, -10.0f).empty()
    );
}

TEST(TreeLayoutTest, NodesDoNotOverlapHorizontally) {
    std::vector<PersonDisplay> people;

    for (PersonId id = 1; id <= 10; ++id) {
        people.push_back(makePerson(id, 0));
    }

    const auto result = calculateTreeLayout(people, {}, 20.0f, 100.0f);

    ASSERT_EQ(result.size(), 10u);

    for (std::size_t i = 1; i < result.size(); ++i) {
        EXPECT_GE(
            result[i].x,
            result[i - 1].x + result[i - 1].width
        );
    }
}

} // namespace
