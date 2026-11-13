#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "Camera.hpp"
#include "Scene.hpp"
#include "UI.hpp"

class Visualizer {
public:
    Visualizer(std::shared_ptr<FamilyManager> familyManager,
               std::shared_ptr<PopulationManager> populationManager,
               std::shared_ptr<IPersonRepository> personRepository,
               unsigned int width, unsigned int height);
    
    void update();
    void render();
    void handleEvents();
    
    void setSimulationState(const SimulationState& state);
    void setCurrentYear(int year) { simulationState_.currentYear = year; }
    
    bool isOpen() const { return window_.isOpen(); }
    
private:
    sf::RenderWindow window_;
    Camera camera_;
    Scene scene_;
    UI ui_;
    SimulationState simulationState_;

    std::shared_ptr<FamilyManager> familyManager_;
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<IPersonRepository> personRepository_;
    
    bool showControls_;

    void handleKeyboardEvents(const sf::Event& event);
};