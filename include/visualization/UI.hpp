#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include <managers/PopulationManager.hpp>
#include <managers/FamilyManager.hpp>

struct SimulationState {
    bool isPaused = false;
    bool autoAdvance = false;
    int currentYear = 0;
};

class UI {
public:
    UI(sf::RenderWindow& window);
    
    bool loadFont(const std::string& fontPath);
    void drawInfoPanel(const std::shared_ptr<PopulationManager>& populationManager,
                      const std::shared_ptr<FamilyManager>& familyManager,
                      int selectedPersonId,
                      const std::shared_ptr<IPersonRepository>& personRepository);
    void drawControlsHint();
    void drawSimulationStatus(const SimulationState& state);
    
    void setShowControls(bool show) { showControls_ = show; }
    void handleResize(const sf::Event& event);

    sf::View getView() { return uiView_;}
    
private:
    sf::RenderWindow& window_;
    sf::View uiView_;
    sf::Font font_;
    bool showControls_ = true;
    
    void drawPersonInfo(int selectedPersonId, const std::shared_ptr<IPersonRepository>& personRepository, float panelX);
};