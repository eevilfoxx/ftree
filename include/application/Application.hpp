#pragma once

#include "core/World.hpp"
#include "core/SimulationConfig.hpp"
#include "core/Random.hpp"
#include "core/SimulationClock.hpp"
#include "presentation/WorldPresentation.hpp"
#include <memory>
#include <string>

namespace ftree::application {

class Application {
public:
    // Constructor - headless mode optional
    Application(bool enableVisualization = true);
    
    // Initialize the application
    bool initialize(const std::string& configPath = "config/simulation.cfg");
    
    // Run the application (with or without visualization)
    int run(int years = 100, bool headless = false);
    
    // Simulation control
    void pause();
    void resume();
    void nextYear();
    void toggleAutoAdvance();
    void setSpeed(int speed);  // speed multiplier
    
    // Data access
    int getCurrentYear() const;
    bool isPaused() const;
    bool isAutoAdvance() const;
    int getSpeed() const;
    
    // Person data
    presentation::PersonDisplay getPersonData(PersonId id) const;
    
    // Statistics
    void getStatistics(int& population, int& families, int& children, int& adults, int& elders) const;
    
private:
    std::unique_ptr<core::SimulationConfig> config_;
    std::unique_ptr<core::Random> rng_;
    std::unique_ptr<core::SimulationClock> clock_;
    std::unique_ptr<core::World> world_;
    std::unique_ptr<presentation::WorldPresentation> presentation_;
    std::unique_ptr<sf::RenderWindow> window_;  // optional, null for headless
    bool visualizationEnabled_;
    int speed_;
    
    // Internal methods
    void setup();
    void handleEvents();
    void update();
    void render();
};

} // namespace ftree::application
