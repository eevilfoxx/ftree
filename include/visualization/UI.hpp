#pragma once

#include <SFML/Graphics.hpp>

#include "application/IVisualizer.hpp"
#include "presentation/PersonDisplay.hpp"
#include "presentation/FamilyDisplay.hpp"

#include <string>
#include <vector>

namespace ftree::visualization {

class UI {
public:
    explicit UI(sf::RenderWindow& window);

    bool loadFont(const std::string& fontPath);

    void drawInfoPanel(
        const std::vector<presentation::PersonDisplay>& people,
        const std::vector<presentation::FamilyDisplay>& families,
        int selectedPersonId
    );

    void drawControlsHint();
    void drawSimulationStatus(const application::VisualizationState& state);

    void setShowControls(bool show) {
        showControls_ = show;
    }

    void handleResize(const sf::Event& event);

    sf::View getView() const {
        return uiView_;
    }

private:
    sf::RenderWindow& window_;
    sf::View uiView_;
    sf::Font font_;
    bool fontLoaded_ = false;
    bool showControls_ = true;

    void drawPersonInfo(
        const std::vector<presentation::PersonDisplay>& people,
        int selectedPersonId,
        float panelX
    );
};

} // namespace ftree::visualization