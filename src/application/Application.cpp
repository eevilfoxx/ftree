#include "application/Application.hpp"

#include "presentation/WorldPresentation.hpp"
#include "simulation/AgingSystem.hpp"
#include "simulation/BirthSystem.hpp"
#include "simulation/DeathSystem.hpp"
#include "simulation/MarriageSystem.hpp"
#include "simulation/PregnancySystem.hpp"
#include "simulation/StatisticsSystem.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace ftree::application {

Application::Application(
    std::unique_ptr<IVisualizer> visualizer,
    std::unique_ptr<ports::ISimulationRepository> repository
)
    : clock_(std::make_unique<core::SimulationClock>()),
      nameDb_(std::make_unique<core::NameDatabase>()),
      visualizer_(std::move(visualizer)),
      repository_(std::move(repository))
{
}

Application::~Application() = default;

bool Application::initialize(
    const StartupSelection& selection,
    const std::string& configPath)
{
    if (!repository_) {
        throw std::logic_error("Application requires a simulation repository");
    }
    if (world_) {
        throw std::logic_error("Application is already initialized");
    }

    if (selection.mode == StartupMode::LoadExisting) {
        if (selection.simulationId <= 0) {
            throw std::invalid_argument("A valid simulation ID is required");
        }
        const auto snapshot = repository_->loadSimulation(selection.simulationId);
        world_ = std::make_unique<domain::World>(
            domain::World::fromSnapshot(snapshot.world));
        config_ = std::make_unique<core::SimulationConfig>(snapshot.config);
        rng_ = std::make_unique<core::Random>(snapshot.randomSeed);
        rng_->restoreState(snapshot.randomEngineState);
        clock_->restoreYear(snapshot.currentYear);
        nameDb_->restoreSnapshot(snapshot.nameDatabase);
        simulationId_ = snapshot.id;
        simulationName_ = snapshot.name;
    } else {
        if (selection.newSimulationName.empty()) {
            throw std::invalid_argument("Simulation name cannot be empty");
        }
        clock_->reset();
        config_ = std::make_unique<core::SimulationConfig>(
            core::SimulationConfig::loadFromFile(configPath));
        rng_ = std::make_unique<core::Random>(42);

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

        world_ = std::make_unique<domain::World>();
        setup();
        auto initialSnapshot = createSimulationSnapshot();
        initialSnapshot.name = selection.newSimulationName;
        simulationName_ = selection.newSimulationName;
        simulationId_ = repository_->createSimulation(initialSnapshot);
    }

    updateVisualization();
    return true;
}

void Application::setup()
{
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
}

ports::SimulationSnapshot Application::createSimulationSnapshot() const
{
    if (!world_ || !config_ || !rng_ || !clock_ || !nameDb_) {
        throw std::logic_error("Cannot snapshot an uninitialized application");
    }
    ports::SimulationSnapshot snapshot;
    snapshot.id = simulationId_;
    snapshot.name = simulationName_;
    snapshot.currentYear = clock_->currentYear();
    snapshot.world = world_->createSnapshot();
    snapshot.config = *config_;
    snapshot.randomSeed = rng_->seed();
    snapshot.randomEngineState = rng_->serializeState();
    snapshot.nameDatabase = nameDb_->createSnapshot();
    return snapshot;
}

void Application::saveCurrentSimulation()
{
    if (!repository_ || simulationId_ <= 0) {
        throw std::logic_error("Cannot save: simulation repository or ID is unavailable");
    }
    repository_->saveSimulation(createSimulationSnapshot());
}

void Application::updateVisualization()
{
    if (!visualizer_ || !world_) {
        return;
    }

    visualizer_->setData(
        presentation::WorldPresentation::getAllPeople(*world_),
        presentation::WorldPresentation::getAllFamilies(*world_)
    );

    visualizer_->setState({
        isPaused_,
        isAutoAdvance_,
        clock_->currentYear()
    });
}

void Application::processVisualizationCommands()
{
    if (!visualizer_) {
        return;
    }

    for (const auto command : visualizer_->consumeCommands()) {
        switch (command) {
        case VisualizationCommand::PauseResume:
            isPaused_ = !isPaused_;
            break;

        case VisualizationCommand::NextYear:
            if (isPaused_) {
                nextYear();
            }
            break;

        case VisualizationCommand::ToggleAutoAdvance:
            toggleAutoAdvance();
            break;

        case VisualizationCommand::IncreaseSpeed:
            setSpeed(std::min(
                speed_ * SIMULATION_SPEED_MULTIPLIER,
                MAX_SIMULATION_SPEED
            ));
            break;

        case VisualizationCommand::DecreaseSpeed:
            setSpeed(std::max(
                speed_ / SIMULATION_SPEED_MULTIPLIER,
                MIN_SIMULATION_SPEED
            ));
            break;
        }
    }

}

int Application::runVisual()
{
    if (!visualizer_) {
        throw std::logic_error("Visual mode requested without a visualizer");
    }

    if (!world_ || !config_) {
        throw std::logic_error("Application must be initialized before runVisual");
    }

    updateVisualization();

    using Clock = std::chrono::steady_clock;
    double accumulator = 0.0;
    auto previousFrameTime = Clock::now();

    while (visualizer_->isOpen()) {
        visualizer_->processEvents();
        processVisualizationCommands();

        if (!visualizer_->isOpen()) {
            break;
        }

        const auto currentFrameTime = Clock::now();
        const double elapsed = std::chrono::duration<double>(
            currentFrameTime - previousFrameTime
        ).count();
        previousFrameTime = currentFrameTime;

        const double deltaTime = std::min(elapsed, MAX_FRAME_TIME);

        // Preserve the existing semantics: auto-advance continues even
        // while paused; otherwise a paused simulation waits for a step command.
        if (!isPaused_ || isAutoAdvance_) {
            accumulator += deltaTime * speed_;
        }

        while (accumulator >= 1.0) {
            update();
            accumulator -= 1.0;
        }

        visualizer_->setState({
            isPaused_,
            isAutoAdvance_,
            clock_->currentYear()
        });
        visualizer_->render();
    }

    return 0;
}

int Application::runHeadless(int years)
{
    if (years < 0) {
        throw std::invalid_argument(
            "Number of simulation years cannot be negative"
        );
    }

    if (!world_ || !config_) {
        throw std::logic_error("Application must be initialized before runHeadless");
    }

    for (int year = 0; year < years; ++year) {
        update();
    }

    return 0;
}

void Application::pause()
{
    isPaused_ = true;
    updateVisualization();
}

void Application::resume()
{
    isPaused_ = false;
    updateVisualization();
}

void Application::nextYear()
{
    update();
}

void Application::toggleAutoAdvance()
{
    isAutoAdvance_ = !isAutoAdvance_;
    updateVisualization();
}

void Application::setSpeed(double speed)
{
    if (!std::isfinite(speed) ||
        speed < MIN_SIMULATION_SPEED ||
        speed > MAX_SIMULATION_SPEED) {
        throw std::invalid_argument(
            "Simulation speed is outside the allowed range"
        );
    }

    speed_ = speed;
}

int Application::getCurrentYear() const
{
    return clock_->currentYear();
}

bool Application::isPaused() const
{
    return isPaused_;
}

bool Application::isAutoAdvance() const
{
    return isAutoAdvance_;
}

double Application::getSpeed() const
{
    return speed_;
}

presentation::PersonDisplay Application::getPersonData(domain::PersonId id) const
{
    if (!world_) {
        throw std::logic_error("Application has not been initialized");
    }
    return presentation::WorldPresentation::getPersonData(*world_, id);
}

void Application::getStatistics(
    int& population,
    int& families,
    int& children,
    int& adults,
    int& elders
) const
{
    if (!world_) {
        population = families = children = adults = elders = 0;
        return;
    }

    simulation::StatisticsSystem::collectStatistics(
        *world_, clock_->currentYear(),
        population, families, children, adults, elders
    );
}

void Application::update()
{
    if (!world_ || !config_ || !rng_ || !clock_ || !nameDb_) {
        return;
    }

    clock_->nextYear();

    simulation::AgingSystem::ageAllLiving(*world_);
    simulation::DeathSystem::processDeaths(
        *world_, config_->mortality_a, config_->mortality_b, *rng_);

    simulation::MarriageSystem::processMarriages(
        *world_, *config_, *rng_, *nameDb_);

    for (const auto& [familyId, family] : world_->getAllFamilies()) {
        (void)familyId;
        simulation::PregnancySystem::tryConceive(
            *world_, family.motherId, family.fatherId,
            config_->base_conception_prob, *rng_);
    }

    simulation::BirthSystem::processBirths(
        *world_, config_->prob_triple, config_->prob_twin, *rng_);

    int population = 0;
    int families = 0;
    int children = 0;
    int adults = 0;
    int elders = 0;
    simulation::StatisticsSystem::collectStatistics(
        *world_, clock_->currentYear(),
        population, families, children, adults, elders);
    simulation::StatisticsSystem::printStatistics(
        clock_->currentYear(), population, families, children, adults, elders);

    saveCurrentSimulation();
    updateVisualization();
}

} // namespace ftree::application
