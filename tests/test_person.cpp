#include <gtest/gtest.h>
#include <entities/Person.hpp>
#include <entities/Gender.hpp>
#include <core/Config.hpp>
#include <core/RandomEngine.hpp>
#include <managers/NameManager.hpp>

class PersonTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& config = Config::getInstance();
        config.set("mortality_a", "0.001");
        config.set("mortality_b", "0.0693");
        config.set("base_conception_prob", "0.3");
        config.set("min_founder_age", "20");
        config.set("max_founder_age", "40");

        NameManager::getInstance().loadNames(
            "/home/evilfox/ftree/config/male_names.txt",
            "/home/evilfox/ftree/config/female_names.txt"
        );
        NameManager::getInstance().loadSurnames(
            "/home/evilfox/ftree/config/surnames.txt"
        );
    }
};

TEST_F(PersonTest, ConstructorSetsAgeAndGender) {
    Person p(1, Gender::Male, 25);
    EXPECT_EQ(p.getUuid(), 1);
    EXPECT_EQ(p.getGender(), Gender::Male);
    EXPECT_EQ(p.getAge(), 25);
    EXPECT_TRUE(p.isAlive());
    EXPECT_FALSE(p.isPregnant());
    EXPECT_FALSE(p.getName().empty());
}

TEST_F(PersonTest, ConstructorWithParents) {
    auto mother = std::make_shared<Person>(1, Gender::Female, 30);
    auto father = std::make_shared<Person>(2, Gender::Male, 32);
    Person child(3, Gender::Female, mother, father, 0);

    EXPECT_EQ(child.getGender(), Gender::Female);
    EXPECT_EQ(child.getAge(), 0);
    EXPECT_EQ(child.getMother()->getUuid(), 1);
    EXPECT_EQ(child.getFather()->getUuid(), 2);
}

TEST_F(PersonTest, IncrementAge) {
    Person p(1, Gender::Male, 25);
    p.incrementAge();
    EXPECT_EQ(p.getAge(), 26);

    p.incrementAge();
    EXPECT_EQ(p.getAge(), 27);
}

TEST_F(PersonTest, IncrementAgeDeadPerson) {
    Person p(1, Gender::Male, 25);
    p.setAlive(false);
    p.incrementAge();
    EXPECT_EQ(p.getAge(), 25);  // Age should not change when dead
}

TEST_F(PersonTest, Pregnancy) {
    Person p(1, Gender::Female, 25);
    EXPECT_FALSE(p.isPregnant());

    p.setPregnant(true);
    EXPECT_TRUE(p.isPregnant());

    p.setPregnant(false);
    EXPECT_FALSE(p.isPregnant());
}

TEST_F(PersonTest, PregnancyOnlyForFemale) {
    Person male(1, Gender::Male, 25);
    male.setPregnant(true);
    EXPECT_FALSE(male.isPregnant());  // Male cannot be pregnant
}

TEST_F(PersonTest, SetAlive) {
    Person p(1, Gender::Male, 25);
    EXPECT_TRUE(p.isAlive());

    p.setAlive(false);
    EXPECT_FALSE(p.isAlive());
    EXPECT_FALSE(p.isPregnant());  // Pregnancy reset on death

    p.setAlive(true);
    EXPECT_TRUE(p.isAlive());
}

TEST_F(PersonTest, TryDieWithZeroMortality) {
    auto& config = Config::getInstance();
    config.set("mortality_a", "0.0");

    Person p(1, Gender::Male, 100);
    // With mortality_a = 0, death probability is 0
    // RandomEngine may still return 0, but 0 < 0 is false
    for (int i = 0; i < 1000; ++i) {
        EXPECT_FALSE(p.tryDie()) << "Person should not die with zero mortality";
    }
    EXPECT_TRUE(p.isAlive());
}