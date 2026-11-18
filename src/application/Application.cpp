#include "application/Application.hpp"
#include <SFML/Graphics.hpp>
#include "simulation/PregnancySystem.hpp"
#include "simulation/MarriageSystem.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <iostream>

namespace ftree::application {

Application::Application(bool enableVisualization)
    : visualizationEnabled_(enableVisualization),
      isPaused_(false),
      isAutoAdvance_(false),
      speed_(1) {
    clock_ = std::make_unique<core::SimulationClock>();
    nameDb_ = std::make_unique<core::NameDatabase>();
}

Application::~Application() = default;

bool Application::initialize(const std::string& configPath) {
    // 1. Load configuration
    clock_->reset();
    config_ = std::make_unique<core::SimulationConfig>(
        core::SimulationConfig::loadFromFile(configPath));

    // 2. Load names
    rng_ = std::make_unique<core::Random>(42);  // fixed seed for reproducibility

    const std::filesystem::path configDirectory =
        std::filesystem::path(configPath).parent_path();
    const auto resolveConfigPath = [&configDirectory](const std::string& path) {
        const std::filesystem::path configuredPath(path);
        return (configuredPath.is_absolute()
            ? configuredPath
            : configDirectory / configuredPath).lexically_normal().string();
    };

    nameDb_->loadMaleNames(resolveConfigPath(
        config_->getString("male_names_file", "male_names.txt")));
    nameDb_->loadFemaleNames(resolveConfigPath(
        config_->getString("female_names_file", "female_names.txt")));
    nameDb_->loadSurnames(resolveConfigPath(
        config_->getString("surnames_file", "surnames.txt")));

    // 3. Create World
    world_ = std::make_unique<domain::World>();
    
    // 3. Create presentation layer
    presentation_ = std::make_unique<presentation::WorldPresentation>();
    
    // 4. Setup initial population
    setup();
    
    // 5. Create window if visualization enabled
    if (visualizationEnabled_) {
        sf::VideoMode vm(1024, 768);
        window_ = std::make_unique<sf::RenderWindow>(vm, "Family Tree Simulation", sf::Style::Default);
        window_->setFramerateLimit(60);
    }
    
    return true;
}

void Application::setup() {
    const int minAge = config_->min_founder_age;
    const int maxAge = config_->max_founder_age;

    const auto male1 = world_->createPerson(
        nameDb_->getRandomName(true), domain::Gender::Male,
        rng_->randomInt(minAge, maxAge));
    const auto male2 = world_->createPerson(
        nameDb_->getRandomName(true), domain::Gender::Male,
        rng_->randomInt(minAge, maxAge));
    const auto female1 = world_->createPerson(
        nameDb_->getRandomName(false), domain::Gender::Female,
        rng_->randomInt(minAge, maxAge));
    const auto female2 = world_->createPerson(
        nameDb_->getRandomName(false), domain::Gender::Female,
        rng_->randomInt(minAge, maxAge));

    world_->createFamily(female1, male1, nameDb_->getRandomSurname());
    world_->createFamily(female2, male2, nameDb_->getRandomSurname());

    presentation_->getAllPeople(*world_);
    presentation_->getAllFamilies(*world_);
}

int Application::run(int years, bool headless) {
    if (years < 0) {
        throw std::invalid_argument("Number of simulation years cannot be negative");
    }

    if (!config_ || !world_) {
        if (!initialize()) {
            return 1;
        }
    }

    visualizationEnabled_ = !headless && visualizationEnabled_;

    if (headless) {
        window_.reset();
    }

    for (int frame = 0; frame < years; ++frame) {
        if (!isPaused_ || isAutoAdvance_) {
            for (int step = 0; step < speed_; ++step) {
                update();
            }
        }

        if (!headless && window_ && window_->isOpen()) {
            handleEvents();
            render();
        }
    }

    return 0;
}

void Application::pause() {
    isPaused_ = true;
}

void Application::resume() {
    isPaused_ = false;
}

void Application::nextYear() {
    update();
}

void Application::setSpeed(int speed) {
    if (speed < 1) {
        throw std::invalid_argument("Simulation speed must be at least 1");
    }
    speed_ = speed;
}

void Application::toggleAutoAdvance() {
    isAutoAdvance_ = !isAutoAdvance_;
}

int Application::getCurrentYear() const {
    return clock_->currentYear();
}

bool Application::isPaused() const {
    return isPaused_;
}

bool Application::isAutoAdvance() const {
    return isAutoAdvance_;
}

int Application::getSpeed() const {
    return speed_;
}

presentation::PersonDisplay Application::getPersonData(domain::PersonId id) const {
    return presentation::WorldPresentation::getPersonData(*world_, id);
}

void Application::getStatistics(int& population, int& families,
                                 int& children, int& adults, int& elders) const {
    ftree::simulation::StatisticsSystem::collectStatistics(*world_, 0,
        population, families, children, adults, elders);
}

void Application::handleEvents() {
    if (!window_) return;

    sf::Event event;
    while (window_->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_->close();
        }
        
        // Key handling
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    window_->close();
                    break;
                case sf::Keyboard::R:
                    // Reset camera
                    break;
                case sf::Keyboard::Space:
                    isPaused_ = !isPaused_;
                    break;
                case sf::Keyboard::Right:
                    if (isPaused_) {
                        nextYear();
                    }
                    break;
                case sf::Keyboard::A:
                    toggleAutoAdvance();
                    break;
                case sf::Keyboard::Up:
                    if (speed_ < std::numeric_limits<int>::max()) {
                        ++speed_;
                    }
                    break;
                case sf::Keyboard::Down:
                    speed_ = std::max(1, speed_ - 1);
                    break;
                case sf::Keyboard::C:
                    // Toggle controls hint
                    break;
                case sf::Keyboard::H:
                    // Select hovered person
                    break;
                default:
                    break;
            }
        }
    }
}

void Application::update() {
    if (!world_ || !config_ || !rng_ || !clock_) {
        return;
    }

    clock_->nextYear();

    ftree::simulation::AgingSystem::ageAllLiving(*world_);
    ftree::simulation::DeathSystem::processDeaths(
        *world_, config_->mortality_a, config_->mortality_b, *rng_);

    ftree::simulation::MarriageSystem::processMarriages(
        *world_, *config_, *rng_, *nameDb_);

    for (const auto& [familyId, family] : world_->getAllFamilies()) {
        (void)familyId;
        ftree::simulation::PregnancySystem::tryConceive(
            *world_, family.motherId, family.fatherId,
            config_->base_conception_prob, *rng_);
    }

    ftree::simulation::BirthSystem::processBirths(
        *world_, config_->prob_triple, config_->prob_twin, *rng_);

    int population = 0;
    int families = 0;
    int children = 0;
    int adults = 0;
    int elders = 0;
    ftree::simulation::StatisticsSystem::collectStatistics(
        *world_, clock_->currentYear(),
        population, families, children, adults, elders);
    ftree::simulation::StatisticsSystem::printStatistics(
        clock_->currentYear(), population, families, children, adults, elders);
}

void Application::render() {
    if (!window_) return;
    
    window_->clear(sf::Color(240, 240, 240));
    
    // Render the visualization scene
    // Would call scene_->render(*window_)
    
    window_->display();
}

} // namespace ftree::application
