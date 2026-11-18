#include "visualization/UI.hpp"

#include <algorithm>
#include <iostream>

namespace ftree::visualization {

UI::UI(sf::RenderWindow& window)
    : window_(window),
      uiView_(window.getDefaultView()) {
    uiView_.setCenter(
        uiView_.getSize().x / 2.0f,
        uiView_.getSize().y / 2.0f
    );
}

bool UI::loadFont(const std::string& fontPath) {
    fontLoaded_ = font_.loadFromFile(fontPath);

    if (!fontLoaded_) {
        std::cerr << "Failed to load font: "
                  << fontPath << '\n';
    }

    return fontLoaded_;
}

void UI::drawInfoPanel(
    const std::vector<presentation::PersonDisplay>& people,
    const std::vector<presentation::FamilyDisplay>& families,
    int selectedPersonId
) {
    constexpr float panelWidth = 300.0f;

    const float panelX =
        static_cast<float>(window_.getSize().x) - panelWidth;

    sf::RectangleShape panel(
        sf::Vector2f(
            panelWidth,
            static_cast<float>(window_.getSize().y)
        )
    );

    panel.setFillColor(sf::Color(50, 50, 50, 220));
    panel.setPosition(panelX, 0.0f);

    window_.draw(panel);

    if (fontLoaded_) {
        sf::Text statsText;

        statsText.setFont(font_);
        statsText.setCharacterSize(14);
        statsText.setFillColor(sf::Color::White);
        statsText.setPosition(panelX + 10.0f, 20.0f);

        const auto livingCount = std::count_if(
            people.begin(),
            people.end(),
            [](const presentation::PersonDisplay& person) {
                return person.isAlive && !person.isDeceased;
            }
        );

        const std::string stats =
            "Population: " + std::to_string(livingCount) +
            "\nFamilies: " + std::to_string(families.size());

        statsText.setString(stats);

        window_.draw(statsText);

        drawPersonInfo(people, selectedPersonId, panelX);
    }
}

void UI::drawPersonInfo(
    const std::vector<presentation::PersonDisplay>& people,
    int selectedPersonId,
    float panelX
) {
    if (selectedPersonId < 0) {
        return;
    }

    const auto personIt = std::find_if(
        people.begin(),
        people.end(),
        [selectedPersonId](const presentation::PersonDisplay& person) {
            return person.id ==
                static_cast<presentation::PersonId>(selectedPersonId);
        }
    );

    if (personIt == people.end()) {
        return;
    }

    const auto& person = *personIt;

    sf::Text personText;

    personText.setFont(font_);
    personText.setCharacterSize(14);
    personText.setFillColor(sf::Color::White);
    personText.setPosition(panelX + 10.0f, 100.0f);

    std::string info =
        "Selected: " + person.name +
        "\nID: " + std::to_string(person.id) +
        "\nAge: " + std::to_string(person.age) +
        "\nGender: " + std::string(person.isMale ? "Male" : "Female") +
        "\nStatus: " +
        std::string(
            (!person.isAlive || person.isDeceased)
                ? "Deceased"
                : "Alive"
        );

    if (person.isPregnant && !person.isMale) {
        info += "\nPregnant: Yes";
    }

    if (person.familyId != 0) {
        info += "\nFamily ID: " + std::to_string(person.familyId);
    }

    personText.setString(info);

    window_.draw(personText);
}

void UI::drawControlsHint() {
    if (!showControls_) {
        return;
    }

    if (!fontLoaded_) {
        return;
    }

    sf::Text controlsText;

    controlsText.setFont(font_);
    controlsText.setCharacterSize(12);
    controlsText.setFillColor(sf::Color::Black);
    controlsText.setPosition(
        10.0f,
        static_cast<float>(window_.getSize().y) - 190.0f
    );

    const std::string controls =
        "Controls:\n"
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

    const sf::FloatRect textBounds =
        controlsText.getLocalBounds();

    sf::RectangleShape background(
        sf::Vector2f(
            textBounds.width + 20.0f,
            textBounds.height + 20.0f
        )
    );

    background.setFillColor(sf::Color(255, 255, 255, 200));
    background.setPosition(
        5.0f,
        static_cast<float>(window_.getSize().y) - 195.0f
    );

    window_.draw(background);
    window_.draw(controlsText);
}

void UI::drawSimulationStatus(const application::VisualizationState& state) {
    if (!fontLoaded_) {
        return;
    }
    
    sf::Text statusText;

    statusText.setFont(font_);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Black);
    statusText.setPosition(10.0f, 10.0f);

    std::string status =
        "Year: " + std::to_string(state.currentYear);

    status += state.isPaused
        ? " [PAUSED]"
        : " [RUNNING]";

    if (state.autoAdvance) {
        status += " [AUTO]";
    }

    statusText.setString(status);

    const sf::FloatRect textBounds =
        statusText.getLocalBounds();

    sf::RectangleShape background(
        sf::Vector2f(textBounds.width + 20.0f, 30.0f)
    );

    background.setFillColor(sf::Color(255, 255, 255, 230));
    background.setPosition(5.0f, 5.0f);

    window_.draw(background);
    window_.draw(statusText);
}

void UI::handleResize(const sf::Event& event) {
    uiView_.setSize(
        static_cast<float>(event.size.width),
        static_cast<float>(event.size.height)
    );

    uiView_.setCenter(
        uiView_.getSize().x / 2.0f,
        uiView_.getSize().y / 2.0f
    );
}

} // namespace ftree::visualization