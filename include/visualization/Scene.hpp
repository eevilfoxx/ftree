#pragma once

#include <SFML/Graphics.hpp>

#include "presentation/PersonDisplay.hpp"
#include "presentation/FamilyDisplay.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ftree::visualization {

class Scene {
public:
    Scene() = default;

    void setData(
        std::vector<presentation::PersonDisplay> people,
        std::vector<presentation::FamilyDisplay> families
    );

    void update();
    void draw(sf::RenderWindow& window, const sf::View& view);

    int getPersonAtPosition(const sf::Vector2f& worldPos) const;

    std::optional<sf::Vector2f> getPersonPosition(int personId) const;

    void setSelectedPerson(int personId) noexcept {
        selectedPersonId_ = personId;
    }

    void setHoveredPerson(int personId) noexcept {
        hoveredPersonId_ = personId;
    }

    int getHoveredPersonId() const noexcept {
        return hoveredPersonId_;
    }

    int getSelectedPersonId() const noexcept {
        return selectedPersonId_;
    }

    bool loadFont(const std::string& fontPath);

private:
    std::vector<presentation::PersonDisplay> people_;
    std::vector<presentation::FamilyDisplay> families_;

    std::unordered_map<presentation::PersonId, sf::Vector2f>
        personPositions_;

    int selectedPersonId_ = -1;
    int hoveredPersonId_ = -1;

    float nodeRadius_ = 20.0f;

    sf::Color maleColor_ = sf::Color(100, 150, 255);
    sf::Color femaleColor_ = sf::Color(255, 150, 200);
    sf::Color deceasedColor_ = sf::Color(150, 150, 150);

    sf::Font font_;
    bool fontLoaded_ = false;

    void calculateTreeLayout();

    void drawFamilyConnections(
        sf::RenderWindow& window
    );

    void drawMarriageConnection(
        const sf::Vector2f& pos1,
        const sf::Vector2f& pos2,
        sf::RenderWindow& window
    ) const;

    void drawParentChildConnection(
        const sf::Vector2f& parentPos,
        const sf::Vector2f& childPos,
        sf::RenderWindow& window
    ) const;

    void drawSiblingConnections(
        const presentation::FamilyDisplay& family,
        sf::RenderWindow& window
    ) const;

    void drawPerson(
        const presentation::PersonDisplay& person,
        const sf::Vector2f& position,
        sf::RenderWindow& window
    ) const;

    void drawSelection(
        const sf::Vector2f& position,
        sf::RenderWindow& window
    ) const;

    void drawHover(
        const sf::Vector2f& position,
        sf::RenderWindow& window
    ) const;
};

} // namespace ftree::visualization