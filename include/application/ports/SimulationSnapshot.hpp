#pragma once

#include "core/SimulationConfig.hpp"
#include "core/NameDatabase.hpp"
#include "domain/WorldSnapshot.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ftree::application::ports {

using SimulationId = std::int64_t;

struct SimulationSnapshot {
    SimulationId id = 0;
    std::string name;
    int currentYear = 0;
    domain::WorldSnapshot world;
    core::SimulationConfig config;
    std::uint32_t randomSeed = 0;
    std::string randomEngineState;
    core::NameDatabaseSnapshot nameDatabase;
};

struct SimulationSummary {
    SimulationId id = 0;
    std::string name;
    int currentYear = 0;
    std::string updatedAt;
};

} // namespace ftree::application::ports
