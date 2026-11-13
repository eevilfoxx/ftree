#include <gtest/gtest.h>
#include <entities/Family.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>
#include <managers/NameManager.hpp>

class FamilyTest : public ::testing::Test {
protected:
    void SetUp() override {
        NameManager::getInstance().loadNames(
            "/home/evilfox/ftree/config/male_names.txt",
            "/home/evilfox/ftree/config/female_names.txt"
        );
        NameManager::getInstance().loadSurnames(
            "/home/evilfox/ftree/config/surnames.txt"
        );
    }
};

TEST_F(FamilyTest, ConstructorSetsParentsAndSurname) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);

    EXPECT_EQ(family->getId(), 1);
    EXPECT_EQ(family->getMother()->getUuid(), 1);
    EXPECT_EQ(family->getFather()->getUuid(), 2);
    EXPECT_FALSE(family->getSurname().empty());
}

TEST_F(FamilyTest, GetChildrenInitiallyEmpty) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);

    auto children = family->getChildren();
    EXPECT_TRUE(children.empty());
}

TEST_F(FamilyTest, AddAndRemoveChild) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);

    auto child = std::make_shared<Person>(3, Gender::Female, 0);
    family->addChild(child);

    auto children = family->getChildren();
    ASSERT_EQ(children.size(), 1);
    EXPECT_EQ(children[0]->getUuid(), 3);
    EXPECT_TRUE(family->hasChild(child));

    family->removeChild(child);
    children = family->getChildren();
    EXPECT_TRUE(children.empty());
    EXPECT_FALSE(family->hasChild(child));
}

TEST_F(FamilyTest, RemoveNonExistentChild) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);

    auto child = std::make_shared<Person>(3, Gender::Female, 0);
    // Removing a child that was never added should not crash
    family->removeChild(child);
    EXPECT_TRUE(family->getChildren().empty());
}

TEST_F(FamilyTest, AddMultipleChildren) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);

    auto child1 = std::make_shared<Person>(3, Gender::Female, 0);
    auto child2 = std::make_shared<Person>(4, Gender::Male, 0);
    family->addChild(child1);
    family->addChild(child2);

    auto children = family->getChildren();
    ASSERT_EQ(children.size(), 2);
    EXPECT_TRUE(family->hasChild(child1));
    EXPECT_TRUE(family->hasChild(child2));
}

TEST_F(FamilyTest, GetSurnameNotEmpty) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);

    EXPECT_FALSE(family->getSurname().empty());
}

TEST_F(FamilyTest, SetSurname) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);

    family->setSurname("Testov");
    EXPECT_EQ(family->getSurname(), "Testov");
}