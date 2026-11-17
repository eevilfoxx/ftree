// AgingService.cpp
#include "services/AgingService.hpp"

AgingService::AgingService(std::shared_ptr<PopulationManager> populationManager,
                           std::shared_ptr<IPersonRepository> personRepository)
    : populationManager_(populationManager),
      personRepository_(personRepository) {}

void AgingService::processAging() {
    auto living = populationManager_->getLivingPopulation();
    for (auto& person : living) {
        person->incrementAge();
        personRepository_->update(person);
    }
}