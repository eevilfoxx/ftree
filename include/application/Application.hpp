#pragma once

#include "domain/World.hpp"
#include "core/SimulationConfig.hpp"
#include "core/Random.hpp"
#include "core/SimulationClock.hpp"
#include "core/NameDatabase.hpp"
#include "presentation/WorldPresentation.hpp"
#include <memory>
#include <string>

// Forward declare SFML types to avoid mandatory SFML dependency in header.
// SFML is optional; the full definition is only needed in Application.cpp.
namespace sf {
class RenderWindow;
}

namespace ftree::application {

class Application {
public:
    // Constructor - enableVisualization=true means graphics window is created
    explicit Application(bool enableVisualization = true);
    ~Application();

    // Initialize the application (loads config, names, creates world)
    bool initialize(const std::string& configPath = "config/simulation.cfg");

    // Run the simulation for the given number of years.
    // If headless is true, no window is created and no events are polled.
    int run(int years = 100, bool headless = false);

    // Simulation control
    void pause();
    void resume();
    void nextYear();
    void toggleAutoAdvance();
    void setSpeed(int speed);  // speed multiplier

    // State queries
    int getCurrentYear() const;
    bool isPaused() const;
    bool isAutoAdvance() const;
    int getSpeed() const;

    // Person data
    presentation::PersonDisplay getPersonData(domain::PersonId id) const;

    // Statistics
    void getStatistics(int& population, int& families,
                       int& children, int& adults, int& elders) const;

private:
    std::unique_ptr<core::SimulationConfig> config_;
    std::unique_ptr<core::Random> rng_;
    std::unique_ptr<core::SimulationClock> clock_;
    std::unique_ptr<core::NameDatabase> nameDb_;
    std::unique_ptr<domain::World> world_;
    std::unique_ptr<presentation::WorldPresentation> presentation_;
    std::unique_ptr<sf::RenderWindow> window_;  // null in headless mode
    bool visualizationEnabled_;
    bool isPaused_;
    bool isAutoAdvance_;
    int speed_;

    // Internal methods
    void setup();
    void handleEvents();
    void update();
    void render();
};

} // namespace ftree::application
