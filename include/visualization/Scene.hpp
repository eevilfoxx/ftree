#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>

#include "entities/Family.hpp"
#include "entities/Person.hpp"
#include "entities/repos/IPersonRepository.hpp"
#include "managers/FamilyManager.hpp"
#include "managers/PopulationManager.hpp"

class Scene {
public:
    Scene(std::shared_ptr<FamilyManager> familyManager,
          std::shared_ptr<PopulationManager> populationManager,
          std::shared_ptr<IPersonRepository> personRepository);

    void update();
    void draw(sf::RenderWindow& window, const sf::View& view);

    int getPersonAtPosition(const sf::Vector2f& worldPos) const;
    std::optional<sf::Vector2f> getPersonPosition(int personId) const;

    void setSelectedPerson(int personId) { selectedPersonId_ = personId; }
    void setHoveredPerson(int personId) { hoveredPersonId_ = personId; }

    int getHoveredPersonId() const noexcept { return hoveredPersonId_; }
    int getSelectedPersonId() const noexcept { return selectedPersonId_; }

    bool loadFont(const std::string& fontPath);

private:
    std::shared_ptr<FamilyManager> familyManager_;
    std::shared_ptr<PopulationManager> populationManager_;
    std::shared_ptr<IPersonRepository> personRepository_;

    std::unordered_map<int, sf::Vector2f> personPositions_;
    int selectedPersonId_ = -1;
    int hoveredPersonId_ = -1;

    float nodeRadius_ = 20.0f;
    float verticalSpacing_ = 100.0f;
    float horizontalSpacing_ = 80.0f;

    sf::Color maleColor_ = sf::Color(100, 150, 255);
    sf::Color femaleColor_ = sf::Color(255, 150, 200);
    sf::Color deceasedColor_ = sf::Color(150, 150, 150);

    sf::Font font_;
    bool fontLoaded_ = false;

    void drawFamilyConnections(
        const std::vector<std::shared_ptr<Family>>& families,
        sf::RenderWindow& window);
    void drawPerson(const std::shared_ptr<Person>& person,
                    const sf::Vector2f& position,
                    sf::RenderWindow& window);
    void drawMarriageConnection(const sf::Vector2f& pos1,
                                const sf::Vector2f& pos2,
                                sf::RenderWindow& window);
    void drawParentChildConnection(const sf::Vector2f& parentPos,
                                   const sf::Vector2f& childPos,
                                   sf::RenderWindow& window);
    void drawSiblingConnections(
        const std::vector<std::shared_ptr<Person>>& children,
        sf::RenderWindow& window);

    void calculateTreeLayout();
    int getGenerationLevel(std::shared_ptr<Person> person,
                           std::unordered_map<int, int>& memo);
    void buildLevels(
        const std::vector<std::shared_ptr<Person>>& allPersons,
        std::unordered_map<int, std::vector<std::shared_ptr<Person>>>& levels,
        std::unordered_map<int, int>& memo);
    void sortLevelByParents(std::vector<std::shared_ptr<Person>>& persons);
};
