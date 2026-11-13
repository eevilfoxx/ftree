// Simulation.cpp
#include "core/Simulation.hpp"
#include "core/Config.hpp"
#include "core/RandomEngine.hpp"
#include <iostream>

Simulation::Simulation(std::shared_ptr<IPersonRepository> personRepo,
                       std::shared_ptr<IFamilyRepository> familyRepo,
                       std::shared_ptr<EventDispatcher> eventDispatcher)
    : personRepository(personRepo), familyRepository(familyRepo), 
      eventDispatcher(eventDispatcher), year(0),
      populationManager(std::make_shared<PopulationManager>(personRepo, eventDispatcher)),
      familyManager(std::make_shared<FamilyManager>(familyRepo, personRepo, eventDispatcher)),
      agingService(populationManager),
      deathService(populationManager, eventDispatcher),
      marriageService(familyManager, populationManager, personRepository, eventDispatcher),
      pregnancyService(familyManager, personRepository, eventDispatcher),
      birthService(populationManager, familyManager, personRepository, eventDispatcher),
      statisticsService(populationManager, familyManager) {
    
    setupEventHandlers();
}

void Simulation::initialize(int initialPopulation) {
    auto& config = Config::getInstance();
    int minFounderAge = config.getInt("min_founder_age", 20);
    int maxFounderAge = config.getInt("max_founder_age", 40);
    
    for (int i = 0; i < initialPopulation / 2; i++) {
        auto male = std::make_shared<Person>(
            personRepository->getAll().size() + 1, 
            Gender::Male,
            RandomEngine::getInstance().randomInt(minFounderAge, maxFounderAge)
        );
        
        auto female = std::make_shared<Person>(
            personRepository->getAll().size() + 2, 
            Gender::Female,
            RandomEngine::getInstance().randomInt(minFounderAge, maxFounderAge)
        );
        
        populationManager->addPerson(male);
        populationManager->addPerson(female);
        
        familyManager->createFamily(female, male);
    }
}

void Simulation::simulateYears(int years) {
    if (visualizer_) {
        mainLoop(years);
    } else {
        for (int i = 0; i < years; i++) {
            processSingleYear();
            std::cout << "Press Enter to continue to next year...";
            std::cin.ignore();
        }
    }
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
    if (!visualizer_) return;
    
    simulationPaused_ = true;
    stepRequested_ = false;
    autoAdvance_ = false;
    autoAdvanceTimer_.restart();
    
    sf::Clock frameClock;
    sf::Time timeAccumulator = sf::Time::Zero;
    sf::Time timePerUpdate = sf::seconds(timePerYear_);
    
    while (visualizer_->isOpen() && year < totalYears) {
        // Обработка событий
        handleSimulationEvents();
        visualizer_->handleEvents();
        
        // Фиксированный шаг времени для симуляции
        if (!simulationPaused_) {
            sf::Time deltaTime = frameClock.restart();
            
            if (autoAdvance_) {
                timeAccumulator += deltaTime;
                
                while (timeAccumulator >= timePerUpdate && year < totalYears) {
                    processSingleYear();
                    timeAccumulator -= timePerUpdate;
                }
            }
        } else {
            frameClock.restart(); // Сбрасываем таймер при паузе
            timeAccumulator = sf::Time::Zero;
            
            if (stepRequested_) {
                processSingleYear();
                stepRequested_ = false;
            }
        }
        
        // Обновление визуализации
        visualizer_->setCurrentYear(year);
        visualizer_->update();
        visualizer_->render();
        
        // Небольшая задержка для снижения нагрузки на CPU
        sf::sleep(sf::milliseconds(8));
    }
    
    if (year >= totalYears) {
        std::cout << "Simulation completed! " << totalYears << " years simulated." << std::endl;
    }
}

void Simulation::handleSimulationEvents() {
    static sf::Clock keyCooldown;
    
    // Клавиша пробела - пауза/продолжение
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && keyCooldown.getElapsedTime().asSeconds() > 0.3f) {
        simulationPaused_ = !simulationPaused_;
        std::cout << (simulationPaused_ ? "Simulation Paused" : "Simulation Resumed") << std::endl;
        keyCooldown.restart();
    }
    
    // Стрелка вправо - шаг вперед (только в паузе)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && simulationPaused_ && keyCooldown.getElapsedTime().asSeconds() > 0.2f) {
        stepRequested_ = true;
        keyCooldown.restart();
    }
    
    // A - включить/выключить авторежим
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && keyCooldown.getElapsedTime().asSeconds() > 0.3f) {
        autoAdvance_ = !autoAdvance_;
        std::cout << (autoAdvance_ ? "Auto-advance ON" : "Auto-advance OFF") << std::endl;
        keyCooldown.restart();
    }
    
    // Стрелка вверх/вниз - скорость
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && keyCooldown.getElapsedTime().asSeconds() > 0.1f) {
        timePerYear_ = std::max(0.1f, timePerYear_ - 0.1f);
        std::cout << "Speed increased: " << (1.0f / timePerYear_) << " years/second" << std::endl;
        keyCooldown.restart();
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && keyCooldown.getElapsedTime().asSeconds() > 0.1f) {
        timePerYear_ += 0.1f;
        std::cout << "Speed decreased: " << (1.0f / timePerYear_) << " years/second" << std::endl;
        keyCooldown.restart();
    }
}

void Simulation::setupEventHandlers() {
    // Безопасный обработчик для рождения человека
    eventDispatcher->subscribe(EventType::PersonBorn, [](const Event& event) {
        try {
            auto id = event.getData("person_id");
            auto name = event.getData("person_name");
            
            if (id && name) {
                std::cout << "Person born: " << *name << " (ID: " << *id << ")\n";
            } else if (id) {
                std::cout << "Person born: Unknown (ID: " << *id << ")\n";
            } else if (name) {
                std::cout << "Person born: " << *name << " (ID: unknown)\n";
            } else {
                std::cout << "Person born: unknown ID\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in PersonBorn handler: " << e.what() << "\n";
        }
    });

    eventDispatcher->subscribe(EventType::PersonDied, [](const Event& event) {
        try {
            auto id = event.getData("person_id");
            auto name = event.getData("person_name");
            auto age = event.getData("person_age");
            auto cause = event.getData("death_cause");
            
            if (id && name && age) {
                std::cout << "Person died: " << *name << " (ID: " << *id << ", Age: " << *age << ")";
                if (cause) {
                    std::cout << " - " << *cause;
                }
                std::cout << "\n";
            } else if (id && name) {
                std::cout << "Person died: " << *name << " (ID: " << *id << ")";
                if (cause) {
                    std::cout << " - " << *cause;
                }
                std::cout << "\n";
            } else if (id) {
                std::cout << "Person died: ID " << *id;
                if (cause) {
                    std::cout << " - " << *cause;
                }
                std::cout << "\n";
            } else {
                std::cout << "Person died: unknown ID";
                if (cause) {
                    std::cout << " - " << *cause;
                }
                std::cout << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in PersonDied handler: " << e.what() << "\n";
        }
    });
        
        // Обработчик для рождения детей с информацией о родителях
        eventDispatcher->subscribe(EventType::Childbirth, [this](const Event& event) {
        try {
            auto childId = event.getData("child_id");
            auto motherId = event.getData("mother_id");
            auto fatherId = event.getData("father_id");
            auto childName = event.getData("child_name");
            auto familyName = event.getData("family_name");
            
            if (childId && motherId && fatherId) {
                auto mother = personRepository->getById(std::stoi(*motherId));
                auto father = personRepository->getById(std::stoi(*fatherId));
                
                std::string motherName = mother ? mother->getName() : "Unknown";
                std::string fatherName = father ? father->getName() : "Unknown";
                std::string actualChildName = childName ? *childName : "Unknown";
                std::string actualFamilyName = familyName ? *familyName : "Unknown";
                
                std::cout << "Childbirth: " << motherName << " and " << fatherName 
                        << " welcomed " << actualChildName << " " << actualFamilyName << "\n";
            } else {
                std::cout << "Childbirth: Incomplete information\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in Childbirth handler: " << e.what() << "\n";
        }
    });

    eventDispatcher->subscribe(EventType::Marriage, [](const Event& event) {
        try {
            auto maleId = event.getData("male_id");
            auto femaleId = event.getData("female_id");
            auto maleName = event.getData("male_name");
            auto femaleName = event.getData("female_name");
            
            if (maleName && femaleName) {
                std::cout << "Marriage: " << *maleName << " and " << *femaleName << " got married\n";
            } else if (maleId && femaleId) {
                std::cout << "Marriage: Persons " << *maleId << " and " << *femaleId << " got married\n";
            } else {
                std::cout << "Marriage: Two people got married\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in Marriage handler: " << e.what() << "\n";
        }
    });

    eventDispatcher->subscribe(EventType::Pregnancy, [](const Event& event) {
        try {
            auto motherId = event.getData("mother_id");
            auto motherName = event.getData("mother_name");
            auto fatherName = event.getData("father_name");
            
            if (motherName && fatherName) {
                std::cout << "Pregnancy: " << *motherName << " is expecting a child with " << *fatherName << "\n";
            } else if (motherName) {
                std::cout << "Pregnancy: " << *motherName << " is expecting a child\n";
            } else if (motherId) {
                std::cout << "Pregnancy: Person " << *motherId << " is expecting a child\n";
            } else {
                std::cout << "Pregnancy: Someone is expecting a child\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in Pregnancy handler: " << e.what() << "\n";
        }
    });

    eventDispatcher->subscribe(EventType::FamilyCreated, [](const Event& event) {
        try {
            auto familyId = event.getData("family_id");
            auto surname = event.getData("family_surname");
            
            if (familyId && surname) {
                std::cout << "Family created: " << *surname << " family (ID: " << *familyId << ")\n";
            } else if (familyId) {
                std::cout << "Family created: ID " << *familyId << "\n";
            } else {
                std::cout << "Family created: New family formed\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in FamilyCreated handler: " << e.what() << "\n";
        }
    });
}


void Simulation::enableVisualization(bool enable) {
    visualizationEnabled_ = enable;
    if (enable && !visualizer_) {
        visualizer_ = std::make_unique<Visualizer>(familyManager, populationManager, personRepository, 1280.0, 720.0);
    } else if (!enable) {
        visualizer_.reset();
    }
}