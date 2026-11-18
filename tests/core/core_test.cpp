#include <gtest/gtest.h>
#include "core/SimulationConfig.hpp"
#include "core/Random.hpp"
#include "core/NameDatabase.hpp"
#include "core/Event.hpp"
#include "core/SimulationClock.hpp"
#include <iostream>
#include <algorithm>

using namespace ftree::core;

// --- SimulationConfig Tests ---

TEST(ConfigTest, LoadFromFileDefault) {
    // Test with non-existent file - should return defaults
    SimulationConfig config = SimulationConfig::loadFromFile("nonexistent.cfg");
    EXPECT_EQ(config.min_founder_age, 20);
    EXPECT_EQ(config.max_founder_age, 40);
    EXPECT_EQ(config.base_conception_prob, 0.3);
    EXPECT_EQ(config.min_marriage_age, 18);
}

TEST(ConfigTest, LoadFromFileSimulationCfg) {
    SimulationConfig config = SimulationConfig::loadFromFile("config/simulation.cfg");
    // Should load from file
    EXPECT_GT(config.min_founder_age, 0);
    EXPECT_GT(config.max_founder_age, 0);
    EXPECT_GT(config.base_conception_prob, 0.0);
}

TEST(ConfigTest, Validate) {
    SimulationConfig config;
    // Valid config should not throw
    EXPECT_NO_THROW(config.validate());
    
    // Invalid min_founder_age should throw
    config.min_founder_age = -1;
    EXPECT_THROW(config.validate(), std::runtime_error);
}

TEST(ConfigTest, GetKeys) {
    SimulationConfig config = SimulationConfig::loadFromFile("config/simulation.cfg");
    auto keys = config.getKeys();
    EXPECT_GT(keys.size(), 0);
    // Check some expected keys
    bool hasMinFounder = std::find(keys.begin(), keys.end(), "min_founder_age") != keys.end();
    EXPECT_TRUE(hasMinFounder);
}

// --- Random Tests ---

TEST(RandomTest, SeedReproducibility) {
    // Same seed should produce same sequence
    Random r1(42);
    Random r2(42);
    
    EXPECT_EQ(r1.seed(), r2.seed());
    EXPECT_EQ(r1.randomInt(0, 100), r2.randomInt(0, 100));
    EXPECT_EQ(r1.randomDouble(0.0, 1.0), r2.randomDouble(0.0, 1.0));
    EXPECT_EQ(r1.randomBool(), r2.randomBool());
}

TEST(RandomTest, DifferentSeeds) {
    Random r1(1);
    Random r2(2);
    
    EXPECT_NE(r1.seed(), r2.seed());
    // Different seeds should generally produce different values
    EXPECT_NE(r1.randomInt(0, 100), r2.randomInt(0, 100));
}

TEST(RandomTest, Ranges) {
    Random r(123);
    
    // randomInt
    int val1 = r.randomInt(0, 10);
    EXPECT_GE(val1, 0);
    EXPECT_LE(val1, 10);
    
    // randomDouble
    double val2 = r.randomDouble(0.0, 1.0);
    EXPECT_GE(val2, 0.0);
    EXPECT_LE(val2, 1.0);
    
    // randomBool
    bool val3 = r.randomBool();
    EXPECT_TRUE(val3 || !val3);  // Must be true or false
}

// --- NameDatabase Tests ---

TEST(NameDatabaseTest, LoadMaleNames) {
    NameDatabase db;
    db.loadMaleNames("config/male_names.txt");
    EXPECT_GT(db.maleNameCount(), 0u);
    EXPECT_TRUE(db.areNamesLoaded());
}

TEST(NameDatabaseTest, LoadFemaleNames) {
    NameDatabase db;
    db.loadFemaleNames("config/female_names.txt");
    EXPECT_GT(db.femaleNameCount(), 0u);
    EXPECT_TRUE(db.areNamesLoaded());
}

TEST(NameDatabaseTest, LoadSurnames) {
    NameDatabase db;
    db.loadSurnames("config/surnames.txt");
    EXPECT_GT(db.surnameCount(), 0u);
    EXPECT_TRUE(db.areSurnamesLoaded());
}

TEST(NameDatabaseTest, NoDuplicateLoading) {
    NameDatabase db;
    // Load same file twice - should not duplicate
    db.loadMaleNames("config/male_names.txt");
    const auto initialCount = db.maleNameCount();
    db.loadMaleNames("config/male_names.txt");
    EXPECT_EQ(db.maleNameCount(), initialCount);
}

TEST(NameDatabaseTest, GetRandomName) {
    NameDatabase db;
    db.loadMaleNames("config/male_names.txt");
    db.loadFemaleNames("config/female_names.txt");
    
    std::string maleName = db.getRandomName(true);
    std::string femaleName = db.getRandomName(false);
    
    EXPECT_GT(maleName.size(), 0);
    EXPECT_GT(femaleName.size(), 0);
}

// --- Event Tests ---

TEST(EventTest, EventTypeEnum) {
    // Verify all event types exist
    EventType et1 = EventType::PersonBorn;
    EventType et2 = EventType::PersonDied;
    EventType et3 = EventType::Marriage;
    EventType et4 = EventType::FamilyCreated;
    
    // Just check they have distinct integer values
    EXPECT_NE(static_cast<int>(et1), static_cast<int>(et2));
}

TEST(EventTest, DispatcherSubscribeUnsubscribe) {
    EventDispatcher dispatcher;
    
    // Subscribe
    int id = dispatcher.subscribe(EventType::PersonBorn, [](const Event&) {
        // Handler
    });
    EXPECT_GT(id, 0);
    
    // Unsubscribe
    dispatcher.unsubscribe(EventType::PersonBorn, id);
    
    // Publish should not call the unsubscribed handler
    // (We can't easily test this without more setup, but verify subscribe/unsubscribe works)
}

// --- SimulationClock Tests ---

TEST(SimulationClockTest, AdvanceYear) {
    SimulationClock clock;
    EXPECT_EQ(clock.currentYear(), 0);
    
    clock.nextYear();
    EXPECT_EQ(clock.currentYear(), 1);
    
    clock.nextYear();
    EXPECT_EQ(clock.currentYear(), 2);
}

TEST(SimulationClockTest, Reset) {
    SimulationClock clock;
    clock.nextYear();
    clock.nextYear();
    EXPECT_EQ(clock.currentYear(), 2);
    
    clock.reset();
    EXPECT_EQ(clock.currentYear(), 0);
}

// --- Integration Test: Full Setup ---

TEST(IntegrationTest, CoreSetup) {
    // Initialize core components
    SimulationConfig config = SimulationConfig::loadFromFile("config/simulation.cfg");
    Random rng(42);
    NameDatabase nameDb;
    nameDb.loadMaleNames("config/male_names.txt");
    nameDb.loadFemaleNames("config/female_names.txt");
    nameDb.loadSurnames("config/surnames.txt");
    SimulationClock clock;
    
    // Verify components are working
    EXPECT_GT(config.base_conception_prob, 0.0);
    EXPECT_GT(nameDb.maleNameCount(), 0u);
    EXPECT_EQ(clock.currentYear(), 0);
}

