#include <gtest/gtest.h>
#include <domain/Person.hpp>
#include <domain/Family.hpp>
#include <domain/World.hpp>
#include <iostream>

using namespace ftree::domain;

// --- Person Tests ---

TEST(PersonTest, DefaultConstructor) {
    Person p;
    EXPECT_EQ(p.getId(), 0);
    EXPECT_EQ(p.getName(), "");
    EXPECT_EQ(p.getGender(), Gender::Male);
    EXPECT_EQ(p.getAge(), 0);
    EXPECT_TRUE(p.isAlive());
    EXPECT_FALSE(p.isPregnant());
}

TEST(PersonTest, SimpleConstructor) {
    Person p(1, "John", Gender::Male, 30);
    EXPECT_EQ(p.getId(), 1);
    EXPECT_EQ(p.getName(), "John");
    EXPECT_EQ(p.getGender(), Gender::Male);
    EXPECT_EQ(p.getAge(), 30);
    EXPECT_TRUE(p.isAlive());
    EXPECT_FALSE(p.isPregnant());
}

TEST(PersonTest, FullConstructor) {
    Person p(1, "John", Gender::Male, 30, true, false, 2, 3, 5);
    EXPECT_EQ(p.getId(), 1);
    EXPECT_EQ(p.getName(), "John");
    EXPECT_EQ(p.getGender(), Gender::Male);
    EXPECT_EQ(p.getAge(), 30);
    EXPECT_TRUE(p.isAlive());
    EXPECT_FALSE(p.isPregnant());
    EXPECT_EQ(p.getMotherId(), 2);
    EXPECT_EQ(p.getFatherId(), 3);
    EXPECT_EQ(p.getFamilyId(), 5);
}

TEST(PersonTest, IncrementAge) {
    Person p(1, "John", Gender::Male, 25);
    p.incrementAge();
    EXPECT_EQ(p.getAge(), 26);
    p.incrementAge();
    EXPECT_EQ(p.getAge(), 27);
}

TEST(PersonTest, IncrementAgeDeadPerson) {
    Person p(1, "John", Gender::Male, 25);
    p.setAlive(false);
    p.incrementAge();
    EXPECT_EQ(p.getAge(), 25);
}

TEST(PersonTest, SetAlive) {
    Person p(1, "John", Gender::Male, 25);
    EXPECT_TRUE(p.isAlive());
    p.setAlive(false);
    EXPECT_FALSE(p.isAlive());
    p.setAlive(true);
    EXPECT_TRUE(p.isAlive());
}

TEST(PersonTest, SetPregnant) {
    Person male(1, "MaleInd", Gender::Male, 30);
    male.setPregnant(true);
    EXPECT_FALSE(male.isPregnant());
    Person female(2, "FemaleInd", Gender::Female, 30);
    female.setPregnant(true);
    EXPECT_TRUE(female.isPregnant());
    female.setPregnant(false);
    EXPECT_FALSE(female.isPregnant());
}

TEST(PersonTest, CanConceive) {
    Person youngFemale(1, "YoungF", Gender::Female, 25);
    EXPECT_TRUE(youngFemale.canConceive());
    Person tooYoung(2, "YoungF2", Gender::Female, 16);
    EXPECT_FALSE(tooYoung.canConceive());
    Person tooOld(3, "OldF", Gender::Female, 46);
    EXPECT_FALSE(tooOld.canConceive());
    Person pregnant(4, "PregF", Gender::Female, 30);
    pregnant.setPregnant(true);
    EXPECT_FALSE(pregnant.canConceive());
    Person male(5, "MaleInd", Gender::Male, 30);
    EXPECT_FALSE(male.canConceive());
}

// --- Family Tests ---

TEST(FamilyTest, DefaultConstructor) {
    Family f;
    EXPECT_EQ(f.getId(), 0);
    EXPECT_EQ(f.getMotherId(), 0);
    EXPECT_EQ(f.getFatherId(), 0);
    EXPECT_TRUE(f.getChildrenIds().empty());
    EXPECT_EQ(f.getSurname(), "");
}

TEST(FamilyTest, FullConstructor) {
    Family f(1, 2, 3, "Smith");
    EXPECT_EQ(f.getId(), 1);
    EXPECT_EQ(f.getMotherId(), 2);
    EXPECT_EQ(f.getFatherId(), 3);
    EXPECT_EQ(f.getSurname(), "Smith");
}

TEST(FamilyTest, AddChild) {
    Family f(1, 2, 3);
    f.addChild(10);
    f.addChild(20);
    EXPECT_TRUE(f.hasChild(10));
    EXPECT_TRUE(f.hasChild(20));
    EXPECT_EQ(f.getChildrenIds().size(), 2u);
}

TEST(FamilyTest, AddDuplicateChild) {
    Family f(1, 2, 3);
    f.addChild(10);
    f.addChild(10);
    EXPECT_EQ(f.getChildrenIds().size(), 1u);
    EXPECT_TRUE(f.hasChild(10));
}

TEST(FamilyTest, RemoveChild) {
    Family f(1, 2, 3);
    f.addChild(10);
    f.addChild(20);
    EXPECT_TRUE(f.removeChild(10));
    EXPECT_FALSE(f.hasChild(10));
    EXPECT_EQ(f.getChildrenIds().size(), 1u);
    EXPECT_TRUE(f.hasChild(20));
}

TEST(FamilyTest, RemoveNonExistentChild) {
    Family f(1, 2, 3);
    f.removeChild(999);
    EXPECT_EQ(f.getChildrenIds().size(), 0u);
}

TEST(FamilyTest, HasChild) {
    Family f(1, 2, 3);
    f.addChild(10);
    EXPECT_TRUE(f.hasChild(10));
    EXPECT_FALSE(f.hasChild(999));
}

TEST(FamilyTest, GetAllMembers) {
    Family f(1, 2, 3, "Smith");
    f.addChild(10);
    f.addChild(20);
    auto members = f.getAllMembers();
    EXPECT_EQ(members.size(), 4u);
    EXPECT_TRUE(f.isMember(2));
    EXPECT_TRUE(f.isMember(3));
    EXPECT_TRUE(f.isMember(10));
    EXPECT_TRUE(f.isMember(20));
    EXPECT_FALSE(f.isMember(999));
}

// --- World Tests ---

TEST(WorldTest, CreatePerson) {
    World world;
    PersonId id = world.createPerson("John", Gender::Male, 25);
    EXPECT_TRUE(world.personExists(id));
    auto person = world.getPerson(id);
    EXPECT_TRUE(person.has_value());
    EXPECT_EQ(person->name, "John");
    EXPECT_EQ(person->gender, Gender::Male);
    EXPECT_EQ(person->age, 25);
    EXPECT_TRUE(person->alive);
}

TEST(WorldTest, CreateMultiplePeople) {
    World world;
    PersonId id1 = world.createPerson("Alice", Gender::Female, 30);
    PersonId id2 = world.createPerson("Bob", Gender::Male, 35);
    PersonId id3 = world.createPerson("Charlie", Gender::Male, 40);
    EXPECT_TRUE(world.personExists(id1));
    EXPECT_TRUE(world.personExists(id2));
    EXPECT_TRUE(world.personExists(id3));
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}

TEST(WorldTest, GetLivingPeople) {
    World world;
    PersonId id1 = world.createPerson("Alice", Gender::Female, 30);
    PersonId id2 = world.createPerson("Bob", Gender::Male, 35);
    PersonId id3 = world.createPerson("Charlie", Gender::Male, 40);
    world.killPerson(id2);
    auto living = world.getLivingPeople();
    EXPECT_EQ(living.size(), 2u);
    EXPECT_TRUE(world.personExists(id1));
    EXPECT_TRUE(world.personExists(id2));
    EXPECT_TRUE(world.personExists(id3));
}

TEST(WorldTest, KillPreservesData) {
    World world;
    PersonId id = world.createPerson("John", Gender::Male, 50);
    world.killPerson(id);
    auto person = world.getPerson(id);
    EXPECT_TRUE(person.has_value());
    EXPECT_FALSE(person->alive);
    EXPECT_FALSE(person->pregnant);
    EXPECT_EQ(person->age, 50);
    EXPECT_EQ(person->name, "John");
    EXPECT_EQ(person->motherId, 0);
    EXPECT_EQ(person->fatherId, 0);
    EXPECT_EQ(person->familyId, 0);
}

TEST(WorldTest, FamilyCreation) {
    World world;
    PersonId motherId = world.createPerson("Alice", Gender::Female, 30);
    PersonId fatherId = world.createPerson("Bob", Gender::Male, 35);
    FamilyId famId = world.createFamily(motherId, fatherId, "Smith");
    EXPECT_TRUE(world.familyExists(famId));
    auto family = world.getFamily(famId);
    EXPECT_TRUE(family.has_value());
    EXPECT_EQ(family->motherId, motherId);
    EXPECT_EQ(family->fatherId, fatherId);
    EXPECT_EQ(family->surname, "Smith");
    EXPECT_TRUE(family->childrenIds.empty());
}

TEST(WorldTest, NoDanglingReferences) {
    World world;
    PersonId id = world.createPerson("John", Gender::Male, 25);
    auto person = world.getPerson(999);
    EXPECT_FALSE(person.has_value());
    auto family = world.getFamily(999);
    EXPECT_FALSE(family.has_value());
}

TEST(WorldTest, UniqueIDs) {
    World world;
    std::vector<PersonId> ids;
    for (int i = 0; i < 100; i++) {
        PersonId id = world.createPerson("Person" + std::to_string(i), Gender::Male, i);
        ids.push_back(id);
    }
    for (size_t i = 0; i < ids.size(); i++) {
        for (size_t j = i + 1; j < ids.size(); j++) {
            EXPECT_NE(ids[i], ids[j]);
        }
    }
    for (PersonId id : ids) {
        auto person = world.getPerson(id);
        EXPECT_TRUE(person.has_value());
    }
}

// --- Invariants Tests ---

TEST(WorldTest, KillPreservesGenealogyData) {
    World world;
    PersonId motherId = world.createPerson("Alice", Gender::Female, 50);
    PersonId fatherId = world.createPerson("Bob", Gender::Male, 50);
    world.killPerson(motherId);
    auto person = world.getPerson(motherId);
    EXPECT_FALSE(!person->alive);
    EXPECT_EQ(person->name, "Alice");
    EXPECT_EQ(person->age, 50);
    EXPECT_EQ(person->motherId, 0);
    EXPECT_EQ(person->fatherId, 0);
}

TEST(WorldTest, LifeSeparateFromExistence) {
    World world;
    PersonId id = world.createPerson("John", Gender::Male, 25);
    EXPECT_TRUE(world.personExists(id));
    EXPECT_TRUE(world.getPerson(id)->alive);
    world.killPerson(id);
    EXPECT_TRUE(world.personExists(id));
    EXPECT_FALSE(world.getPerson(id)->alive);
    EXPECT_EQ(world.getPerson(id)->name, "John");
}

TEST(WorldTest, FamilyDissolutionPreservesPeople) {
    World world;
    PersonId motherId = world.createPerson("Alice", Gender::Female, 30);
    PersonId fatherId = world.createPerson("Bob", Gender::Male, 35);
    FamilyId famId = world.createFamily(motherId, fatherId, "Smith");
    world.dissolveFamily(famId);
    auto mom = world.getPerson(motherId);
    auto dad = world.getPerson(fatherId);
    EXPECT_TRUE(mom.has_value());
    EXPECT_TRUE(dad.has_value());
    EXPECT_EQ(mom->familyId, 0);
    EXPECT_EQ(dad->familyId, 0);
    EXPECT_FALSE(world.familyExists(famId));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
