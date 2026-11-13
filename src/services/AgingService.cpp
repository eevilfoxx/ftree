// AgingService.cpp
#include "services/AgingService.hpp"

AgingService::AgingService(std::shared_ptr<PopulationManager> populationManager)
    : populationManager_(populationManager) {}

void AgingService::processAging() {
    auto living = populationManager_->getLivingPopulation();
    for (auto& person : living) {
        person->incrementAge();
    }
}