#pragma once

#include "application/IVisualizer.hpp"
#include "application/ports/ISimulationRepository.hpp"
#include "core/NameDatabase.hpp"
#include "core/Random.hpp"
#include "core/SimulationClock.hpp"
#include "core/SimulationConfig.hpp"
#include "domain/World.hpp"
#include "presentation/PersonDisplay.hpp"

#include <memory>
#include <string>

namespace ftree::application {

enum class StartupMode { CreateNew, LoadExisting };

struct StartupSelection {
    StartupMode mode = StartupMode::CreateNew;
    ports::SimulationId simulationId = 0;
    std::string newSimulationName = "New simulation";
};

class Application {
public:
    Application(
        std::unique_ptr<IVisualizer> visualizer,
        std::unique_ptr<ports::ISimulationRepository> repository
    );
    ~Application();

    bool initialize(
        const StartupSelection& selection,
        const std::string& configPath = "config/simulation.cfg"
    );

    int runVisual();
    int runHeadless(int years);

    void pause();
    void resume();
    void nextYear();
    void toggleAutoAdvance();
    void setSpeed(double speed);

    int getCurrentYear() const;
    bool isPaused() const;
    bool isAutoAdvance() const;
    double getSpeed() const;

    presentation::PersonDisplay getPersonData(domain::PersonId id) const;

    void getStatistics(
        int& population,
        int& families,
        int& children,
        int& adults,
        int& elders
    ) const;

private:
    static constexpr double MIN_SIMULATION_SPEED = 0.0625;
    static constexpr double MAX_SIMULATION_SPEED = 64.0;
    static constexpr double SIMULATION_SPEED_MULTIPLIER = 2.0;
    static constexpr double MAX_FRAME_TIME = 0.25;

    std::unique_ptr<core::SimulationConfig> config_;
    std::unique_ptr<core::Random> rng_;
    std::unique_ptr<core::SimulationClock> clock_;
    std::unique_ptr<core::NameDatabase> nameDb_;
    std::unique_ptr<domain::World> world_;
    std::unique_ptr<IVisualizer> visualizer_;
    std::unique_ptr<ports::ISimulationRepository> repository_;
    ports::SimulationId simulationId_ = 0;
    std::string simulationName_;

    bool isPaused_ = false;
    bool isAutoAdvance_ = false;
    double speed_ = 1.0;

    void setup();
    void update();
    void updateVisualization();
    ports::SimulationSnapshot createSimulationSnapshot() const;
    void saveCurrentSimulation();
    void processVisualizationCommands();
};

} // namespace ftree::application
