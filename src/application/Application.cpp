#include "application/Application.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
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
    config_ = std::make_unique<core::SimulationConfig>();
    config_->loadFromFile(configPath);

    // 2. Load names
    rng_ = std::make_unique<core::Random>(42);  // fixed seed for reproducibility

    nameDb_->loadMaleNames("config/male_names.txt");
    nameDb_->loadFemaleNames("config/female_names.txt");
    nameDb_->loadSurnames("config/surnames.txt");

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
    // Create initial population - founders
    // Age range: 20-40 as per config
    int minAge = config_->getInt("min_founder_age", 20);
    int maxAge = config_->getInt("max_founder_age", 40);

    // Create 4 founders: 2 males, 2 females
    domain::PersonId m1 = world_->createPerson("John", domain::Gender::Male,
                                              minAge + (maxAge - minAge) / 2);
    domain::PersonId m2 = world_->createPerson("Mike", domain::Gender::Male, 32);
    domain::PersonId f1 = world_->createPerson("Jane", domain::Gender::Female, 28);
    domain::PersonId f2 = world_->createPerson("Mary", domain::Gender::Female, 31);

    // Create families
    domain::FamilyId fam1 = world_->createFamily(f1, m1, "Smith");
    domain::FamilyId fam2 = world_->createFamily(f2, m2, "Jones");

    // Initialize presentation data
    presentation_->getAllPeople(*world_);
    presentation_->getAllFamilies(*world_);
}

int Application::run(int years, bool headless) {
    // Initialize only once; do not re-initialize if already done.
    if (!config_ || !world_) {
        initialize();
    }

    visualizationEnabled_ = !headless && visualizationEnabled_;

    // If headless and no window exists, ensure window_ stays null.
    if (headless) {
        window_ = nullptr;
    }

    // Main loop
    for (int y = 0; y < years; y++) {
        // Simulation step
        // 1. Aging
        ftree::simulation::AgingSystem::ageAllLiving(*world_);

        // 2. Deaths
        ftree::simulation::DeathSystem::processDeaths(*world_,
            config_->getDouble("mortality_a", 0.001),
            config_->getDouble("mortality_b", 0.0693));

        // 3. Marriages
        // (would iterate over singles and try marriages)

        // 4. Pregnancies
        // (would iterate over families and try conceptions)

        // 5. Births
        ftree::simulation::BirthSystem::processBirths(*world_);

        // 6. Statistics
        int pop, families, children, adults, elders;
        ftree::simulation::StatisticsSystem::collectStatistics(*world_, y + 1,
            pop, families, children, adults, elders);

        // Handle events (for visualization only)
        if (!headless && window_) {
            handleEvents();
        }

        // Auto advance or pause handling
        if (!isPaused_) {
            // Could have speed control here
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
    clock_->nextYear();
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
                    // Toggle pause
                    break;
                case sf::Keyboard::Right:
                    // Next year
                    break;
                case sf::Keyboard::A:
                    // Toggle auto advance
                    break;
                case sf::Keyboard::Up:
                    // Increase speed
                    break;
                case sf::Keyboard::Down:
                    // Decrease speed
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

void Application::render() {
    if (!window_) return;
    
    window_->clear(sf::Color(240, 240, 240));
    
    // Render the visualization scene
    // Would call scene_->render(*window_)
    
    window_->display();
}

} // namespace ftree::application
