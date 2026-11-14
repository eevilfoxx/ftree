// Simulation.cpp
#include "core/Simulation.hpp"
#include "core/Config.hpp"
#include "core/EventDispatcher.hpp"
#include "managers/PopulationManager.hpp"
#include "managers/FamilyManager.hpp"
#include "managers/NameManager.hpp"
#include "services/AgingService.hpp"
#include "services/DeathService.hpp"
#include "services/MarriageService.hpp"
#include "services/PregnancyService.hpp"
#include "services/BirthService.hpp"
#include "utils/StatisticsService.hpp"

Simulation::Simulation(std::shared_ptr<IPersonRepository> personRepo,
                       std::shared_ptr<IFamilyRepository> familyRepo,
                       std::shared_ptr<EventDispatcher> eventDispatcher)
    : personRepository(personRepo),
      familyRepository(familyRepo),
      eventDispatcher(eventDispatcher),
      year(0),
      populationManager(std::make_shared<PopulationManager>(personRepo, eventDispatcher)),
      familyManager(std::make_shared<FamilyManager>(familyRepo, personRepo, eventDispatcher)),
      agingService(populationManager),
      deathService(populationManager, eventDispatcher),
      marriageService(familyManager, populationManager, personRepo, eventDispatcher),
      pregnancyService(familyManager, personRepo, eventDispatcher),
      birthService(populationManager, familyManager, personRepo, eventDispatcher),
      statisticsService(populationManager, familyManager) {
    setupEventHandlers();
}

void Simulation::initialize(int initialPopulation, class RandomNumberGenerator& rng) {
    auto& config = Config::getInstance();
    int minFounderAge = config.getInt("min_founder_age", 20);
    int maxFounderAge = config.getInt("max_founder_age", 40);

    for (int i = 0; i < initialPopulation / 2; i++) {
        auto male = std::make_shared<Person>(PersonId(i*2+1), Gender::Male, rng.randomInt(minFounderAge, maxFounderAge));
        auto female = std::make_shared<Person>(PersonId(i*2+2), Gender::Female, rng.randomInt(minFounderAge, maxFounderAge));
        populationManager->addPerson(male);
        populationManager->addPerson(female);
        familyManager->createFamily(female, male);
    }
}

void Simulation::simulateYears(int years) {
    // visualization optional - called from outside
}

void Simulation::processSingleYear() {
    year++;
    agingService.processAging();
    deathService.processDeaths();
    marriageService.processMarriages();
    pregnancyService.processPregnancies();
    birthService.processBirths();
    statisticsService.printStatistics(year);
}

void Simulation::mainLoop(int totalYears) {
    // Layout calculation and rendering handled by Visualizer
    // Simulation step logic only
    if (!simulationPaused_) {
        // Simulation step logic
    }
}

void Simulation::handleSimulationEvents() {
    // Events handled by Visualizer, simulation state only
}

void Simulation::enableVisualization(bool enable) {
    // Visualizer creation deferred to outside
}

int Simulation::currentYear() const {
    return year;
}
