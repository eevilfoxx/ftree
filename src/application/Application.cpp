#include "application/Application.hpp"
#include <SFML/Graphics.hpp>
#include "simulation/PregnancySystem.hpp"
#include "simulation/MarriageSystem.hpp"
#include "visualization/VisualizationConfig.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <cmath>

namespace ftree::application {

Application::Application(bool enableVisualization)
    : visualizationEnabled_(enableVisualization),
      isPaused_(false),
      isAutoAdvance_(false),
      speed_(visualization::VisualizationConfig::INITIAL_SIMULATION_SPEED) {
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



int Application::runVisual() {
    visualizationEnabled_ = true;
    pause();

    if (!config_ || !world_) {
        if (!initialize()) {
            return 1;
        }
    }

    if (!window_ || !window_->isOpen()) {
        sf::VideoMode videoMode(
            visualization::VisualizationConfig::WINDOW_WIDTH,
            visualization::VisualizationConfig::WINDOW_HEIGHT
        );

        window_ = std::make_unique<sf::RenderWindow>(
            videoMode,
            "Family Tree Simulation",
            sf::Style::Default
        );

        window_->setFramerateLimit(
            visualization::VisualizationConfig::FRAMERATE_LIMIT
        );
    }

    using Clock = std::chrono::steady_clock;

    // Accumulated simulation time, measured in years.
    double accumulator = 0.0;

    auto previousFrameTime = Clock::now();

    while (window_->isOpen()) {
        handleEvents();

        if (!window_->isOpen()) {
            break;
        }

        const auto currentFrameTime = Clock::now();

        const std::chrono::duration<double> elapsed =
            currentFrameTime - previousFrameTime;

        previousFrameTime = currentFrameTime;

        // Prevent large simulation jumps after a long frame.
        const double deltaTime = std::min(
            elapsed.count(),
            visualization::VisualizationConfig::MAX_FRAME_TIME
        );

        // Convert real time into simulation time.
        if (!isPaused_ || isAutoAdvance_) {
            accumulator += deltaTime * speed_;
        }

        // Execute one update for each accumulated simulation year.
        while (accumulator >= 1.0) {
            update();
            accumulator -= 1.0;
        }

        render();
    }

    window_.reset();

    return 0;
}

int Application::runHeadless(int years) {
    if (years < 0) {
        throw std::invalid_argument(
            "Number of simulation years cannot be negative"
        );
    }

    // Disable visualization before initialization.
    visualizationEnabled_ = false;

    // Close any existing window.
    window_.reset();

    // Initialize the application if necessary.
    if (!config_ || !world_) {
        if (!initialize()) {
            return 1;
        }
    }

    // Run exactly the requested number of simulation years.
    for (int year = 0; year < years; ++year) {
        update();
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

void Application::setSpeed(double speed) {
    using Config = visualization::VisualizationConfig;

    if (!std::isfinite(speed) ||
        speed < Config::MIN_SIMULATION_SPEED ||
        speed > Config::MAX_SIMULATION_SPEED) {
        throw std::invalid_argument(
            "Simulation speed is outside the allowed range"
        );
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
double Application::getSpeed() const {
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
                    setSpeed(std::min(
                        speed_ * visualization::VisualizationConfig::SIMULATION_SPEED_MULTIPLIER,
                        visualization::VisualizationConfig::MAX_SIMULATION_SPEED
                    ));
                    break;

                case sf::Keyboard::Down:
                    setSpeed(std::max(
                        speed_ / visualization::VisualizationConfig::SIMULATION_SPEED_MULTIPLIER,
                        visualization::VisualizationConfig::MIN_SIMULATION_SPEED
                    ));
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
