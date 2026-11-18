#include <gtest/gtest.h>
#include <domain/World.hpp>
#include <simulation/StatisticsSystem.hpp>

using namespace ftree::simulation;
using namespace ftree::domain;

/** --- Test: collectStatistics with one person killed --- */
TEST(StatisticsSystemTest, CollectStatisticsOnePersonKilled) {
    World testWorld;
    PersonId dianaId = testWorld.createPerson("Diana", ftree::domain::Gender::Female, 5);
    testWorld.killPerson(dianaId);
    
    int population = 0, families = 0, children = 0, adults = 0, elders = 0;
    StatisticsSystem::collectStatistics(testWorld, 1, population, families, children, adults, elders);
    // Diana is dead, so 0 living people
    EXPECT_EQ(population, 0u);
};

/** --- Test: collectStatistics with multiple people --- */
TEST(StatisticsSystemTest, CollectStatisticsMultiplePeople) {
    World testWorld;
    PersonId aliceId = testWorld.createPerson("Alice", ftree::domain::Gender::Female, 25);
    PersonId bobId = testWorld.createPerson("Bob", ftree::domain::Gender::Male, 30);
    PersonId charlieId = testWorld.createPerson("Charlie", ftree::domain::Gender::Male, 10);
    PersonId dianaId = testWorld.createPerson("Diana", ftree::domain::Gender::Female, 5);
    PersonId eveId = testWorld.createPerson("Eve", ftree::domain::Gender::Female, 35);
    PersonId frankId = testWorld.createPerson("Frank", ftree::domain::Gender::Male, 70);
    testWorld.killPerson(dianaId);
    
    int population = 0, families = 0, children = 0, adults = 0, elders = 0;
    StatisticsSystem::collectStatistics(testWorld, 1, population, families, children, adults, elders);
    // Alice, Bob, Charlie are alive (3 adults), Eve is alive (1 adult), Frank is dead
    // So living = 4 (Alice, Bob, Charlie, Eve)
    // Children (< 18): Charlie (10) = 1
    // Adults (18-59): Alice (25), Bob (30), Eve (35) = 3
    // Elders (>= 60): none living
    EXPECT_EQ(population, 4u);
    EXPECT_EQ(children, 1u);
    EXPECT_EQ(adults, 3u);
    EXPECT_EQ(elders, 0u);
};

/** --- Test: collectStatistics with empty world --- */
TEST(StatisticsSystemTest, CollectStatisticsEmptyWorld) {
    World emptyWorld;
    int pop = 0, families = 0, children = 0, adults = 0, elders = 0;
    StatisticsSystem::collectStatistics(emptyWorld, 0, pop, families, children, adults, elders);
    EXPECT_EQ(pop, 0u);
    EXPECT_EQ(families, 0u);
    EXPECT_EQ(children, 0u);
    EXPECT_EQ(adults, 0u);
    EXPECT_EQ(elders, 0u);
}

// No main function - Google Test provides main()
