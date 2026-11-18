#include "visualization/UI.hpp"
#include <iostream>

UI::UI(sf::RenderWindow& window) : window_(window), uiView_(window.getDefaultView()) {
    uiView_.setCenter(uiView_.getSize().x / 2, uiView_.getSize().y / 2);
}

bool UI::loadFont(const std::string& fontPath) {
    if (!font_.loadFromFile(fontPath)) {
        // Попробуем загрузить стандартный шрифт как запасной вариант
        if (!font_.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "Failed to load font\n";
            return false;
        }
    }
    return true;
}

void UI::drawInfoPanel(const std::shared_ptr<PopulationManager>& populationManager,
                      const std::shared_ptr<FamilyManager>& familyManager,
                      int selectedPersonId,
                      const std::shared_ptr<IPersonRepository>& personRepository) {
    float panelWidth = 300;
    float panelX = window_.getSize().x - panelWidth;
    
    sf::RectangleShape panel(sf::Vector2f(panelWidth, window_.getSize().y));
    panel.setFillColor(sf::Color(50, 50, 50, 220));
    panel.setPosition(panelX, 0);
    window_.draw(panel);
    
    sf::Text statsText;
    statsText.setFont(font_);
    statsText.setCharacterSize(14);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(panelX + 10, 20);
    
    auto living = populationManager->getLivingPopulation();
    auto families = familyManager->getAllFamilies();
    
    std::string stats = "Population: " + std::to_string(living.size()) + 
                       "\nFamilies: " + std::to_string(families.size());
    
    statsText.setString(stats);
    window_.draw(statsText);
    
    drawPersonInfo(selectedPersonId, personRepository, panelX);
}

void UI::drawPersonInfo(int selectedPersonId, const std::shared_ptr<IPersonRepository>& personRepository, float panelX) {
    if (selectedPersonId == -1) return;
    
    auto person = personRepository->getById(selectedPersonId);
    if (!person) return;
    
    sf::Text personText;
    personText.setFont(font_);
    personText.setCharacterSize(14);
    personText.setFillColor(sf::Color::White);
    personText.setPosition(panelX + 10, 100);
    
    std::string info = "Selected: " + person->getName() +
                      "\nID: " + std::to_string(selectedPersonId) +
                      "\nAge: " + std::to_string(person->getAge()) +
                      "\nGender: " + std::string(person->getGender() == Gender::Male ? "Male" : "Female") +
                      "\nStatus: " + std::string(person->isAlive() ? "Alive" : "Deceased");
    
    if (person->isPregnant() && person->getGender() == Gender::Female) {
        info += "\nPregnant: Yes";
    }
    
    auto family = person->getFamily();
    if (family) {
        info += "\nFamily ID: " + std::to_string(family->getId());
    }
    
    personText.setString(info);
    window_.draw(personText);
}

void UI::drawControlsHint() {
    if (!showControls_) return;
    
    sf::Text controlsText;
    controlsText.setFont(font_);
    controlsText.setCharacterSize(12);
    controlsText.setFillColor(sf::Color::Black);
    controlsText.setPosition(10, window_.getSize().y - 190);
    
    std::string controls = "Controls:\n"
                          "Mouse Drag - Pan\n"
                          "Mouse Wheel - Zoom\n"
                          "</> Zoom\n"
                          "R - Reset view\n"
                          "C - Toggle controls\n"
                          "H - Select hovered\n"
                          "\nSimulation:\n"
                          "Space - Pause/Resume\n"
                          "Right Arrow - Step forward\n"
                          "A - Toggle auto-advance\n"
                          "Up/Down - Speed control";
    
    controlsText.setString(controls);
    
    sf::FloatRect textBounds = controlsText.getLocalBounds();
    sf::RectangleShape background(sf::Vector2f(textBounds.width + 20, textBounds.height + 20));
    background.setFillColor(sf::Color(255, 255, 255, 200));
    background.setPosition(5, window_.getSize().y - 195);
    window_.draw(background);
    
    window_.draw(controlsText);
}

void UI::drawSimulationStatus(const SimulationState& state) {
    sf::Text statusText;
    statusText.setFont(font_);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Black);
    statusText.setPosition(10, 10);
    
    std::string status = "Year: " + std::to_string(state.currentYear);
    status += state.isPaused ? " [PAUSED]" : " [RUNNING]";
    if (state.autoAdvance) {
        status += " [AUTO]";
    }
    
    statusText.setString(status);
    
    sf::FloatRect textBounds = statusText.getLocalBounds();
    sf::RectangleShape background(sf::Vector2f(textBounds.width + 20, 30));
    background.setFillColor(sf::Color(255, 255, 255, 230));
    background.setPosition(5, 5);
    window_.draw(background);
    
    window_.draw(statusText);
}

void UI::handleResize(const sf::Event& event) {
    uiView_.setSize(event.size.width, event.size.height);
    uiView_.setCenter(uiView_.getSize().x / 2, uiView_.getSize().y / 2);
}