#pragma once

#include "application/ports/SimulationSnapshot.hpp"

#include <vector>

namespace ftree::application::ports {

class ISimulationRepository {
public:
    virtual ~ISimulationRepository() = default;

    virtual SimulationId createSimulation(const SimulationSnapshot& snapshot) = 0;
    virtual SimulationSnapshot loadSimulation(SimulationId id) = 0;
    virtual void saveSimulation(const SimulationSnapshot& snapshot) = 0;
    virtual std::vector<SimulationSummary> listSimulations() = 0;
    virtual void deleteSimulation(SimulationId id) = 0;
};

} // namespace ftree::application::ports
