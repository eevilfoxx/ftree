#include <gtest/gtest.h>
#include <entities/repos/MemoryPersonRepository.hpp>
#include <entities/repos/MemoryFamilyRepository.hpp>
#include <entities/Person.hpp>
#include <entities/Family.hpp>
#include <entities/Gender.hpp>

class PersonRepositoryTest : public ::testing::Test {
protected:
    std::shared_ptr<MemoryPersonRepository> repo;

    void SetUp() override {
        repo = std::make_shared<MemoryPersonRepository>();
    }
};

TEST_F(PersonRepositoryTest, AddAndGetById) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    repo->add(person);

    auto retrieved = repo->getById(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getUuid(), 1);
    EXPECT_EQ(retrieved->getAge(), 25);
}

TEST_F(PersonRepositoryTest, GetByIdNotFound) {
    auto result = repo->getById(999);
    EXPECT_EQ(result, nullptr);
}

TEST_F(PersonRepositoryTest, GetAll) {
    repo->add(std::make_shared<Person>(1, Gender::Male, 25));
    repo->add(std::make_shared<Person>(2, Gender::Female, 30));

    auto all = repo->getAll();
    EXPECT_EQ(all.size(), 2);
}

TEST_F(PersonRepositoryTest, Remove) {
    repo->add(std::make_shared<Person>(1, Gender::Male, 25));
    repo->remove(1);

    EXPECT_EQ(repo->getById(1), nullptr);
    EXPECT_EQ(repo->getAll().size(), 0);
}

TEST_F(PersonRepositoryTest, Update) {
    auto person = std::make_shared<Person>(1, Gender::Male, 25);
    repo->add(person);

    person->incrementAge();
    repo->update(person);

    auto retrieved = repo->getById(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getAge(), 26);
}

TEST_F(PersonRepositoryTest, FindByPredicate) {
    repo->add(std::make_shared<Person>(1, Gender::Male, 25));
    repo->add(std::make_shared<Person>(2, Gender::Female, 30));
    repo->add(std::make_shared<Person>(3, Gender::Male, 40));

    auto males = repo->find([](const Person& p) {
        return p.getGender() == Gender::Male;
    });
    EXPECT_EQ(males.size(), 2);
}

TEST_F(PersonRepositoryTest, GetByAgeRange) {
    repo->add(std::make_shared<Person>(1, Gender::Male, 10));
    repo->add(std::make_shared<Person>(2, Gender::Female, 25));
    repo->add(std::make_shared<Person>(3, Gender::Male, 50));

    auto range = repo->getByAgeRange(20, 40);
    ASSERT_EQ(range.size(), 1);
    EXPECT_EQ(range[0]->getAge(), 25);
}

TEST_F(PersonRepositoryTest, GetByGender) {
    repo->add(std::make_shared<Person>(1, Gender::Male, 25));
    repo->add(std::make_shared<Person>(2, Gender::Female, 30));
    repo->add(std::make_shared<Person>(3, Gender::Male, 40));

    auto males = repo->getByGender(Gender::Male);
    EXPECT_EQ(males.size(), 2);

    auto females = repo->getByGender(Gender::Female);
    EXPECT_EQ(females.size(), 1);
}

TEST_F(PersonRepositoryTest, GetLiving) {
    auto alive = std::make_shared<Person>(1, Gender::Male, 25);
    auto dead = std::make_shared<Person>(2, Gender::Female, 30);
    dead->setAlive(false);

    repo->add(alive);
    repo->add(dead);

    auto living = repo->getLiving();
    ASSERT_EQ(living.size(), 1);
    EXPECT_EQ(living[0]->getUuid(), 1);
}

class FamilyRepositoryTest : public ::testing::Test {
protected:
    std::shared_ptr<MemoryFamilyRepository> repo;

    void SetUp() override {
        repo = std::make_shared<MemoryFamilyRepository>();
    }
};

TEST_F(FamilyRepositoryTest, AddAndGetById) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    auto family = std::make_shared<Family>(1, mother, father);
    repo->add(family);

    auto retrieved = repo->getById(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), 1);
}

TEST_F(FamilyRepositoryTest, GetAll) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    repo->add(std::make_shared<Family>(1, mother, father));
    repo->add(std::make_shared<Family>(2, mother, father));

    auto all = repo->getAll();
    EXPECT_EQ(all.size(), 2);
}

TEST_F(FamilyRepositoryTest, Remove) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    repo->add(std::make_shared<Family>(1, mother, father));
    repo->remove(1);

    EXPECT_EQ(repo->getById(1), nullptr);
}

TEST_F(FamilyRepositoryTest, FindByPredicate) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    repo->add(std::make_shared<Family>(1, mother, father));
    repo->add(std::make_shared<Family>(2, mother, father));

    auto found = repo->find([](const Family& f) {
        return f.getId() == 2;
    });
    ASSERT_EQ(found.size(), 1);
    EXPECT_EQ(found[0]->getId(), 2);
}