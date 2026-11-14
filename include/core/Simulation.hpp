#pragma once

#include <memory>
#include "entities/repos/IPersonRepository.hpp"
#include "entities/repos/IFamilyRepository.hpp"
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

class Simulation {
public:
    Simulation(std::shared_ptr<IPersonRepository> personRepo,
               std::shared_ptr<IFamilyRepository> familyRepo,
               std::shared_ptr<EventDispatcher> eventDispatcher);

    void initialize(int initialPopulation, class RandomNumberGenerator& rng);
    void simulateYears(int years);
    void enableVisualization(bool enable);

    int currentYear() const;

private:
    void processSingleYear();
    void mainLoop(int totalYears);

    void handleSimulationEvents();
    void setupEventHandlers();

    // Зависимости
    std::shared_ptr<IPersonRepository> personRepository;
    std::shared_ptr<IFamilyRepository> familyRepository;
    std::shared_ptr<EventDispatcher> eventDispatcher;

    // Менеджеры
    std::shared_ptr<PopulationManager> populationManager;
    std::shared_ptr<FamilyManager> familyManager;

    // Сервисы
    AgingService agingService;
    DeathService deathService;
    MarriageService marriageService;
    PregnancyService pregnancyService;
    BirthService birthService;
    StatisticsService statisticsService;

    // Состояние
    int year;

    // Управление симуляцией
    bool simulationPaused_ = true;
    bool stepRequested_ = false;
    bool autoAdvance_ = false;

    //TEMПОР - будет вынесен во Visualizer
    // float timePerYear_ = 1.0f;
    //sf::Clock autoAdvanceTimer_;
};

#endif // SIMULATION_HPP
