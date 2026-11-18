#include <gtest/gtest.h>

#include "core/SimulationConfig.hpp"
#include "core/Random.hpp"
#include "core/NameDatabase.hpp"
#include "core/Event.hpp"
#include "core/SimulationClock.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace ftree::core;

namespace {

// Единый каталог с тестовыми конфигурационными файлами.
const std::filesystem::path CONFIG_DIR =
    std::filesystem::path(FTREE_TEST_SOURCE_DIR) / "config";

const std::filesystem::path SIMULATION_CONFIG =
    CONFIG_DIR / "simulation.cfg";

const std::filesystem::path MALE_NAMES =
    CONFIG_DIR / "male_names.txt";

const std::filesystem::path FEMALE_NAMES =
    CONFIG_DIR / "female_names.txt";

const std::filesystem::path SURNAMES =
    CONFIG_DIR / "surnames.txt";

} // namespace

// --- SimulationConfig Tests ---

TEST(ConfigTest, LoadFromFileDefault) {
    // Test with non-existent file - should return defaults.
    SimulationConfig config =
        SimulationConfig::loadFromFile("nonexistent.cfg");

    EXPECT_EQ(config.min_founder_age, 20);
    EXPECT_EQ(config.max_founder_age, 40);
    EXPECT_EQ(config.base_conception_prob, 0.3);
    EXPECT_EQ(config.min_marriage_age, 18);
}

TEST(ConfigTest, LoadFromFileSimulationCfg) {
    SimulationConfig config =
        SimulationConfig::loadFromFile(SIMULATION_CONFIG.string());

    // Should load from file.
    EXPECT_GT(config.min_founder_age, 0);
    EXPECT_GT(config.max_founder_age, 0);
    EXPECT_GT(config.base_conception_prob, 0.0);
}

TEST(ConfigTest, Validate) {
    SimulationConfig config;

    // Valid config should not throw.
    EXPECT_NO_THROW(config.validate());

    // Invalid min_founder_age should throw.
    config.min_founder_age = -1;
    EXPECT_THROW(config.validate(), std::runtime_error);
}

TEST(ConfigTest, GetKeys) {
    SimulationConfig config =
        SimulationConfig::loadFromFile(SIMULATION_CONFIG.string());

    auto keys = config.getKeys();

    EXPECT_GT(keys.size(), 0);

    // Check some expected keys.
    bool hasMinFounder =
        std::find(keys.begin(), keys.end(), "min_founder_age") != keys.end();

    EXPECT_TRUE(hasMinFounder);
}