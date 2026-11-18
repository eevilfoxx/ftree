#include <gtest/gtest.h>
#include <managers/PopulationManager.hpp>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <core/Config.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>

class PopulationManagerTest : public ::testing::Test {
protected:
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<PopulationManager> popManager;

    void SetUp() override {
        personRepo = std::make_shared<MemoryPersonRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        popManager = std::make_shared<PopulationManager>(personRepo, eventDispatcher);

        auto& config = Config::getInstance();
        config.set("min_marriage_age", "18");
        config.set("max_person_age", "100");
    }
};

TEST_F(PopulationManagerTest, AddPerson) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    popManager->addPerson(person);

    auto retrieved = personRepo->getById(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getUuid(), 1);
}

TEST_F(PopulationManagerTest, RemovePerson) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    popManager->addPerson(person);
    popManager->removePerson(1);

    EXPECT_EQ(personRepo->getById(1), nullptr);
}

TEST_F(PopulationManagerTest, AgePerson) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    popManager->addPerson(person);
    popManager->agePerson(1);

    auto retrieved = personRepo->getById(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getAge(), 26);
}

TEST_F(PopulationManagerTest, AgeNonExistentPerson) {
    // Should not crash
    popManager->agePerson(999);
}

TEST_F(PopulationManagerTest, GetLivingPopulation) {
    auto alive = std::make_shared<Person>(1, Gender::Male, 25);
    auto dead = std::make_shared<Person>(2, Gender::Female, 30);
    dead->setAlive(false);

    popManager->addPerson(alive);
    popManager->addPerson(dead);

    auto living = popManager->getLivingPopulation();
    ASSERT_EQ(living.size(), 1);
    EXPECT_EQ(living[0]->getUuid(), 1);
}

TEST_F(PopulationManagerTest, GetByAgeRange) {
    popManager->addPerson(std::make_shared<Person>(1, Gender::Male, 10));
    popManager->addPerson(std::make_shared<Person>(2, Gender::Female, 25));
    popManager->addPerson(std::make_shared<Person>(3, Gender::Male, 50));

    auto range = popManager->getByAgeRange(20, 40);
    ASSERT_EQ(range.size(), 1);
    EXPECT_EQ(range[0]->getAge(), 25);
}

TEST_F(PopulationManagerTest, AreRelativesSamePerson) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    EXPECT_TRUE(popManager->areRelatives(person, person));
}

TEST_F(PopulationManagerTest, AreRelativesSameMother) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto child1 = std::make_shared<Person>(2, Gender::Male, mother, nullptr, 5);
    auto child2 = std::make_shared<Person>(3, Gender::Female, mother, nullptr, 3);

    EXPECT_TRUE(popManager->areRelatives(child1, child2));
}

TEST_F(PopulationManagerTest, AreRelativesSameFather) {
    auto father = std::make_shared<Person>(1, Gender::Male, 30);
    auto child1 = std::make_shared<Person>(2, Gender::Male, nullptr, father, 5);
    auto child2 = std::make_shared<Person>(3, Gender::Female, nullptr, father, 3);

    EXPECT_TRUE(popManager->areRelatives(child1, child2));
}

TEST_F(PopulationManagerTest, AreRelativesNotRelatives) {
    auto person1 = std::make_shared<Person>(1, Gender::Male, 25);
    auto person2 = std::make_shared<Person>(2, Gender::Female, 30);
    EXPECT_FALSE(popManager->areRelatives(person1, person2));
}

TEST_F(PopulationManagerTest, FindEligiblePartnersNoCandidates) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    popManager->addPerson(person);

    auto partners = popManager->findEligiblePartners(person);
    EXPECT_TRUE(partners.empty());
}

TEST_F(PopulationManagerTest, FindEligiblePartnersSameGender) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    auto otherMale = std::make_shared<Person>(2, Gender::Male, 26);
    popManager->addPerson(person);
    popManager->addPerson(otherMale);

    auto partners = popManager->findEligiblePartners(person);
    EXPECT_TRUE(partners.empty());
}

TEST_F(PopulationManagerTest, FindEligiblePartnersUnderage) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    auto minor = std::make_shared<Person>(2, Gender::Female, 16);
    popManager->addPerson(person);
    popManager->addPerson(minor);

    auto partners = popManager->findEligiblePartners(person);
    EXPECT_TRUE(partners.empty());
}

TEST_F(PopulationManagerTest, FindEligiblePartnersEligible) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    auto partner = std::make_shared<Person>(2, Gender::Female, 24);
    popManager->addPerson(person);
    popManager->addPerson(partner);

    auto partners = popManager->findEligiblePartners(person);
    ASSERT_EQ(partners.size(), 1);
    EXPECT_EQ(partners[0]->getUuid(), 2);
}

TEST_F(PopulationManagerTest, AreRelativesParentAndChild) {
    auto parent = std::make_shared<Person>(1, Gender::Female, 45);
    auto child = std::make_shared<Person>(2, Gender::Male, parent, nullptr, 20);

    EXPECT_TRUE(popManager->areRelatives(parent, child));
    EXPECT_TRUE(popManager->areRelatives(child, parent));
}

TEST_F(PopulationManagerTest, FindEligiblePartnersRejectsPersonAboveMaximumAge) {
    auto person = std::make_shared<Person>(1, Gender::Male, 101);
    auto partner = std::make_shared<Person>(2, Gender::Female, 30);
    popManager->addPerson(person);
    popManager->addPerson(partner);

    EXPECT_TRUE(popManager->findEligiblePartners(person).empty());
}
