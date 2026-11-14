// DeathService.cpp
#include <services/DeathService.hpp>
#include <entities/Person.hpp>

DeathService::DeathService(std::shared_ptr<PopulationManager> populationManager,
                           std::shared_ptr<EventDispatcher> eventDispatcher)
    : populationManager_(populationManager), eventDispatcher_(eventDispatcher) {}

void DeathService::processDeaths() {
    auto living = populationManager_->getLivingPopulation();
    for (auto& person : living) {
        if (person->tryDie()) {
            // Remove deceased from repository
            populationManager_->removePerson(person->getUuid());
            Event deathEvent(EventType::PersonDied);
            deathEvent.setData("person_id", std::to_string(person->getUuid()));
            deathEvent.setData("person_name", person->getName());
            deathEvent.setData("person_age", std::to_string(person->getAge()));
            deathEvent.setData("death_cause", "natural causes");

            eventDispatcher_->publish(deathEvent);
        }
    }
}
