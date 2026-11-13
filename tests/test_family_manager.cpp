#include <gtest/gtest.h>
#include <managers/FamilyManager.hpp>
#include <entities/repos/IFamilyRepository.hpp>
#include <entities/repos/IPersonRepository.hpp>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <entities/repos/MemoryFamilyRepository.hpp>
#include <core/EventDispatcher.hpp>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>

class FamilyManagerTest : public ::testing::Test {
protected:
    std::shared_ptr<IFamilyRepository> familyRepo;
    std::shared_ptr<IPersonRepository> personRepo;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<FamilyManager> famManager;

    void SetUp() override {
        familyRepo = std::make_shared<MemoryFamilyRepository>();
        personRepo = std::make_shared<MemoryPersonRepository>();
        eventDispatcher = std::make_shared<EventDispatcher>();
        famManager = std::make_shared<FamilyManager>(familyRepo, personRepo, eventDispatcher);
    }
};

TEST_F(FamilyManagerTest, CreateFamily) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    personRepo->add(mother);
    personRepo->add(father);

    auto family = famManager->createFamily(mother, father);
    ASSERT_NE(family, nullptr);
    EXPECT_EQ(family->getMother()->getUuid(), 1);
    EXPECT_EQ(family->getFather()->getUuid(), 2);
    EXPECT_FALSE(family->getSurname().empty());
}

TEST_F(FamilyManagerTest, CreateFamilyInvalidGender) {
    auto person1 = std::make_shared<Person>(1, Gender::Male, 30);
    auto person2 = std::make_shared<Person>(2, Gender::Male, 32);
    personRepo->add(person1);
    personRepo->add(person2);

    auto family = famManager->createFamily(person1, person2);
    EXPECT_EQ(family, nullptr);
}

TEST_F(FamilyManagerTest, CreateFamilyDeadPerson) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    father->setAlive(false);
    personRepo->add(mother);
    personRepo->add(father);

    auto family = famManager->createFamily(mother, father);
    EXPECT_EQ(family, nullptr);
}

TEST_F(FamilyManagerTest, DissolveFamily) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    personRepo->add(mother);
    personRepo->add(father);

    auto family = famManager->createFamily(mother, father);
    ASSERT_NE(family, nullptr);

    famManager->dissolveFamily(family->getId());
    EXPECT_EQ(familyRepo->getById(family->getId()), nullptr);
}

TEST_F(FamilyManagerTest, DissolveNonExistentFamily) {
    // Should not crash
    famManager->dissolveFamily(999);
}

TEST_F(FamilyManagerTest, AddChildToFamily) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    personRepo->add(mother);
    personRepo->add(father);

    auto family = famManager->createFamily(mother, father);
    ASSERT_NE(family, nullptr);

    auto child = std::make_shared<Person>(3, Gender::Female, 0);
    personRepo->add(child);
    famManager->addChildToFamily(family->getId(), child);

    auto members = famManager->getFamilyMembers(family->getId());
    bool found = false;
    for (const auto& m : members) {
        if (m->getUuid() == 3) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(FamilyManagerTest, AddChildToNonExistentFamily) {
    auto child = std::make_shared<Person>(3, Gender::Female, 0);
    // Should not crash
    famManager->addChildToFamily(999, child);
}

TEST_F(FamilyManagerTest, RemoveChildFromFamily) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    personRepo->add(mother);
    personRepo->add(father);

    auto family = famManager->createFamily(mother, father);
    ASSERT_NE(family, nullptr);

    auto child = std::make_shared<Person>(3, Gender::Female, 0);
    personRepo->add(child);
    famManager->addChildToFamily(family->getId(), child);

    famManager->removeChildFromFamily(family->getId(), 3);

    auto members = famManager->getFamilyMembers(family->getId());
    bool found = false;
    for (const auto& m : members) {
        if (m->getUuid() == 3) {
            found = true;
            break;
        }
    }
    EXPECT_FALSE(found);
}

TEST_F(FamilyManagerTest, GetAllFamilies) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    personRepo->add(mother);
    personRepo->add(father);

    famManager->createFamily(mother, father);
    auto mother2 = std::make_shared<Person>(3, Gender::Female, 28);
    auto father2 = std::make_shared<Person>(4, Gender::Male, 30);
    personRepo->add(mother2);
    personRepo->add(father2);
    famManager->createFamily(mother2, father2);

    auto families = famManager->getAllFamilies();
    EXPECT_EQ(families.size(), 2);
}